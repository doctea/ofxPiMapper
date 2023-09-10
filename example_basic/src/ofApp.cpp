#include "ofApp.h"

void ofApp::setup(){
	ofBackground(0);

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
