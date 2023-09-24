#include "ofApp.h"

#include "midi.h"

void ofApp::setup(){
	ofBackground(0);

	// Enable or disable audio for video sources globally
	// Set this to false to save resources on the Raspberry Pi
	ofx::piMapper::VideoSource::enableAudio = false;
	ofx::piMapper::VideoSource::useHDMIForAudio = false;

	// print input ports to console
	midiIn.listInPorts();

	// open port by number (you may need to change this)
	while (!midiIn.openPort("APC MINI:APC MINI MIDI 1 24:0")) {
		ofLogNotice("Waiting to detect APC MINI for input...");
		midiIn.listInPorts();
		sleep(1);
	}
	while (!midiOut.openPort("APC MINI:APC MINI MIDI 1 24:0")) {
		ofLogNotice("Waiting to detect APC MINI for output...");
		midiOut.listOutPorts();
		sleep(1);
	}
	//midiIn.openPort("IAC Pure Data In");  // by name
	//midiIn.openVirtualPort("ofxMidiIn Input"); // open a virtual port

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

	apc_display = new APCDisplayManager(&mapper, &midiIn, &midiOut);

	//ofSetFullscreen(Settings::instance()->getFullscreen());
	//ofSetEscapeQuitsApp(false);
    //ofSetLogLevel(OF_LOG_VERBOSE);

	//#ifdef TARGET_RASPBERRY_PI
	  	ofSetFullscreen(true);
  	//#endif
}

void ofApp::update(){
	mapper.update();
}

void ofApp::draw(){
	mapper.draw();
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

        // add the latest message to the message queue
        midiMessages.push_back(msg);

		if (msg.status==MIDI_CONTROL_CHANGE) {
			if (msg.control==APCMINI_CC_SLIDER_1)
				mapper._application.getSurfaceManager()->setTransparency((byte)(msg.value * 2));
		} else if (msg.status==MIDI_NOTE_ON) {
			if (msg.pitch>=0x30 && msg.pitch<=0x3F) {
				u_int f_key = apc_display->get_preset_for_apcmini_note(msg.pitch);

				//int f_key = msg.pitch; //args.key - OF_KEY_F1;
				printf("Switching to preset scene %i/%i\n", f_key+1, mapper._application.getSurfaceManager()->getNumPresets());
				while (mapper._application.getSurfaceManager()->getNumPresets() <= f_key) {
					printf("num presets is currently %i, so creating new?", mapper._application.getSurfaceManager()->getNumPresets());
					mapper._application.getSurfaceManager()->createPreset();
				}
				mapper._application.setPreset(f_key);
				apc_display->update();
			}
		}

        // remove any old messages if we have too many
        while(midiMessages.size() > maxMessages) {
                midiMessages.erase(midiMessages.begin());
        }
}