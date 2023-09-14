#include "ofMain.h"
#include "ofApp.h"
#include "Settings.h"

int main(int argc, char * argv[]){
	bool fullscreen = false;
	vector<string> arguments = vector<string>(argv, argv + argc);
	for(int i = 0; i < arguments.size(); ++i){
		if(arguments.at(i) == "-f"){
			fullscreen = true;
			break;
		}
	}
	if (fullscreen)
		Settings::instance()->setFullscreen(fullscreen);
	else {
		ofSetupOpenGL(DISPLAY_WIDTH, DISPLAY_HEIGHT, OF_WINDOW);
	}
	
	int w = 800;
	int h = 450;
	ofWindowMode mode = OF_WINDOW;

	// Loglever festlegen
    ofSetLogLevel(OF_LOG_VERBOSE);

    ofGLESWindowSettings settings;
    settings.setSize(w,h);
    settings.setGLESVersion(2);
    settings.windowMode = mode;
    //ofCreateWindow(settings);

	ofRunApp(new ofApp());
}