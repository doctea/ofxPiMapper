#pragma once

#include "ofMain.h"
#include "BaseSurface.h"
#include "SurfaceType.h"

namespace ofx {
namespace piMapper {

class TriangleSurface : public BaseSurface {
	public:
		TriangleSurface();

		void setup();
		void setup(
			ofVec3f v1,
			ofVec3f v2,
			ofVec3f v3,
			ofVec2f t1,
			ofVec2f t2,
			ofVec2f t3,
			BaseSource * newSource);
		void draw();
	
		void setVertex(int index, ofVec3f v);
		void setVertices(vector<ofVec3f> v);
	
		void setTexCoord(int index, ofVec2f t);
		void setTexCoords(vector<ofVec2f> t);
	
		void moveBy(ofVec2f v);

		int getType();
		bool hitTest(ofVec2f p);
		ofVec3f getVertex(int index);
		ofVec2f getTexCoord(int index);
		ofPolyline getHitArea();
		ofPolyline getTextureHitArea();
		vector <ofVec3f> & getVertices();
		vector <ofVec2f> & getTexCoords();
	
		BaseSurface * clone();
};

} // namespace piMapper
} // namespace ofx