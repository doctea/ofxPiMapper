#include "ofApp.h"
#include "midi.h"

void ofApp::setup(){
	ofBackground(0);

	audio_player.load("data/sounds/synth.wav");
	audio_player.play();
	audio_player.unload();

	// Enable or disable audio for video sources globally
	// Set this to false to save resources on the Raspberry Pi
	ofx::piMapper::VideoSource::enableAudio = false;
	ofx::piMapper::VideoSource::useHDMIForAudio = false;

	// print input ports to console
	midiIn.listInPorts();

	// open port by number (you may need to change this)
	while (!midiIn.openPort(1)) { //}"APC MINI:APC MINI MIDI 1 24:0")) {
		ofLogNotice("Waiting to detect APC MINI for input...");
		midiIn.listInPorts();
		sleep(1);
	}
	while (!midiOut.openPort(1)) { //}"APC MINI:APC MINI MIDI 1 24:0")) {
		ofLogNotice("Waiting to detect APC MINI for output...");
		midiOut.listOutPorts();
		sleep(1);
	}

	// don't ignore sysex, timing, & active sense messages,
	// these are ignored by default
	midiIn.ignoreTypes(false, false, false);
	// add ofApp as a listener
	midiIn.addListener(this);
	// print received messages to the console
	midiIn.setVerbose(true);

	// Add our CustomSource to list of fbo sources of the piMapper
	// FBO sources should be added before piMapper.setup() so the
	// piMapper is able to load the source if it is assigned to
	// a surface in XML settings.
	crossSource = new CrossSource();
	customSource = new CustomSource();
	customShader = new CustomShader();
	mapper.registerFboSource(crossSource);
	mapper.registerFboSource(customSource);
	mapper.registerFboSource(customShader);

	mapper.setup();

	apc_display = new APCDisplayManager(&mapper, &midiIn, &midiOut, &audio_player);

	//ofSetFullscreen(Settings::instance()->getFullscreen());
	//ofSetEscapeQuitsApp(false);
    //ofSetLogLevel(OF_LOG_VERBOSE);

	//#ifdef TARGET_RASPBERRY_PI
	  	//ofSetFullscreen(true);
  	//#endif
}

void ofApp::update(){
	midiMutex.lock();
	mapper.update();
	midiMutex.unlock();
	// update the sound playing system:
	//ofSoundUpdate();

	//audio_player.play();
	//ofLogNotice("update");
	// update the sound playing system:
	ofSoundUpdate();

	if (ofGetUnixTimeMillis()%1000==0)
		apc_display->update();
}

void ofApp::draw(){
	midiMutex.lock();
	mapper.draw();
	midiMutex.unlock();
}

void ofApp::keyPressed(int key){
	if(key == OF_KEY_F11 || key == OF_KEY_ESC ){
		ofToggleFullscreen();
		ofShowCursor();
	}
	mapper.keyPressed(key);
	apc_display->update();
}

void ofApp::keyReleased(int key){
	mapper.keyReleased(key);
}

void ofApp::mouseDragged(int x, int y, int button){
	mapper.mouseDragged(x, y, button);
}

void ofApp::mousePressed(int x, int y, int button){
	mapper.mousePressed(x, y, button);
}

void ofApp::mouseReleased(int x, int y, int button){
	mapper.mouseReleased(x, y, button);
}

//--------------------------------------------------------------
void ofApp::exit() {

        // clean up
        midiIn.closePort();
        midiIn.removeListener(this);
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {

	static bool _shift_is_held = false;
	midiMutex.lock();
	// add the latest message to the message queue
	midiMessages.push_back(msg);

	if (msg.status==MIDI_CONTROL_CHANGE) {
		if (msg.control==APCMINI_CC_SLIDER_1)
			mapper._application.getSurfaceManager()->setTransparency((byte)(msg.value * 2));
		else if (msg.control==APCMINI_CC_SLIDER_8)
			audio_player.setVolume((float)msg.value / 127.0);
	} else if (msg.pitch==APCMINI_BUTTON_SHIFT && (msg.status==MIDI_NOTE_ON || msg.status==MIDI_NOTE_OFF)) {
		_shift_is_held = msg.status==MIDI_NOTE_ON;
	} else if (msg.status==MIDI_NOTE_ON) {
		if (msg.pitch>=0x30 && msg.pitch<=0x3F) {
			u_int f_key = apc_display->get_preset_for_apcmini_note(msg.pitch);
			mapper._application.switchPreset(f_key, _shift_is_held);
			//mapper.setPreset(f_key);
			//mapper.keyPressed(OF_KEY_F1 + f_key);
			apc_display->update();
		} else if (msg.pitch>=0x00 && msg.pitch <=0x0F) {
			// trigger audio clip x
			int clip = apc_display->get_audio_slot_for_apcmini_note(msg.pitch);
			ofLogNotice("Triggering clip ") << clip;
			if (apc_display->currently_selected_audio_clip==clip) {
				if (audio_player.isPlaying()) {
					ofLogNotice("stopping.");
					audio_player.stop();
				} else {
					ofLogNotice("playing.");
					audio_player.play();
				}
			} else {
				audio_player.stop();
				audio_player.unload();
				ofLogNotice("loading ") << audio_filenames[clip%audio_filenames.size()] << " and playing";
				try {
					audio_player.loadSound(audio_filenames[clip%audio_filenames.size()], false);
					audio_player.setVolume(1.0f);
					audio_player.play();
				} catch (...) {
					ofLogError("Exception occurred");
				}
			}
			apc_display->currently_selected_audio_clip = clip;
			apc_display->update();
			ofLogNotice("====== processed clip message!");
		}
	}

	// remove any old messages if we have too many
	while(midiMessages.size() > maxMessages) {
			midiMessages.erase(midiMessages.begin());
	}

	midiMutex.unlock();
}