#include "ofApp.h"

void ofApp::setup(){
	ofBackground(0);


	// print input ports to console
	midiIn.listInPorts();

	// open port by number (you may need to change this)
	while (!midiIn.openPort("APC MINI:APC MINI MIDI 1 24:0")) {
		ofLogNotice("Waiting to detect APC MINI...");
		midiIn.listInPorts();
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

		mapper._application.getSurfaceManager()->setTransparency((byte)(msg.value * 2));

        // remove any old messages if we have too many
        while(midiMessages.size() > maxMessages) {
                midiMessages.erase(midiMessages.begin());
        }
}