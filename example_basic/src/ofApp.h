#pragma once

#include "ofMain.h"
#include "Settings.h"
#include "ofxPiMapper.h"
#include "Sources/CustomSource.h"
#include "Sources/CrossSource.h"
#include "Sources/VideoSource.h"
#include "Sources/CustomShader.h"

#define DISPLAY_WIDTH 1024
#define DISPLAY_HEIGHT 768

#define TEXTURE_WIDTH 1024
#define TEXTURE_HEIGHT 1024

class ofApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();

	// We need to forward key and mouse events to ofxPiMapper.
	// By not doing it we have the opportunity to use ofxPiMapper
	// witout the interface.
	
	void keyPressed(int key);
	void keyReleased(int key);

	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	
	// This is our mapper object.
	ofxPiMapper mapper;

    // By using a custom source that is derived from FboSource
    // you will be able to see the source listed in sources editor
    CustomSource *customSource;
    CrossSource *crossSource;
    CustomShader *customShader;
};
