#pragma once

#include "ofMain.h"
#include "FboSource.h"

class CustomSource : public ofx::piMapper::FboSource {
	public:
		CustomSource() {}

        virtual void setup() override;
		virtual void update() override;
		virtual void draw() override;

		vector <ofRectangle> rects;
		vector <float> rectSpeeds;
};