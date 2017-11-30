#include "TriangleSurface.h"

namespace ofx {
namespace piMapper {

TriangleSurface::TriangleSurface(){
	setup();
}

void TriangleSurface::setup(){
	// Create 3 points for the triangle
	ofVec3f v1 = ofVec3f(ofGetWidth() / 2.0f, 0, 0);
	ofVec3f v2 = ofVec3f(0, ofGetHeight(), 0);
	ofVec3f v3 = ofVec3f(ofGetWidth(), ofGetHeight(), 0);

	// Create 3 point for the texture coordinates
	ofVec2f t1 = ofVec2f(0.5f, 0);
	ofVec2f t2 = ofVec2f(0, 1.0f);
	ofVec2f t3 = ofVec2f(1, 1.0f);

	setup(v1, v2, v3, t1, t2, t3, source);
}

void TriangleSurface::setup(
	ofVec3f v1,
	ofVec3f v2,
	ofVec3f v3,
	ofVec2f t1,
	ofVec2f t2,
	ofVec2f t3,
	BaseSource * newSource){
	
	// Assign texture
	source = newSource;

	// Clear mesh
	mesh.clear();

	// Create a surface with the points
	mesh.addVertex(v1);
	mesh.addVertex(v2);
	mesh.addVertex(v3);

	// Add texture coordinates
	mesh.addTexCoord(t1);
	mesh.addTexCoord(t2);
	mesh.addTexCoord(t3);
}

void TriangleSurface::draw(){
	if(source->getTexture() == 0){
		return;
	}
	
	if(!source->getTexture()->isAllocated()){
		return;
	}
	
	bool normalizedTexCoords = ofGetUsingNormalizedTexCoords();
	ofEnableNormalizedTexCoords();

	source->getTexture()->bind();
	mesh.draw();
	source->getTexture()->unbind();
	
	if(!normalizedTexCoords){
		ofDisableNormalizedTexCoords();
	}
}

void TriangleSurface::setVertex(int index, ofVec3f v){
	if(index > 2){
		ofLog() << "Vertex with this index does not exist: " << index << endl;
		return;
	}
	
	mesh.setVertex(index, v);
	ofNotifyEvent(vertexChangedEvent, index, this);
}


/*void TriangleSurface::setVertices(vector<ofVecf> v){
	if(v.size() != 3){
		throw runtime_error("Wrong number of vertices (expected 3)");
	}
	
	for(int i = 0; i < 3; ++i){
		mesh.setVertex(i, v[i]);
	}
	
	ofNotifyEvent(verticesChangedEvent, mesh.getVertices(), this);
}*/

void TriangleSurface::setVertices(vector<ofVec3f> v){
	if(v.size() != 3){
		throw runtime_error("Wrong number of vertices");
	}
	
	for(int i = 0; i < 3; ++i){
		mesh.setVertex(i, v[i]);
	}
	
	ofNotifyEvent(verticesChangedEvent, mesh.getVertices(), this);
}

void TriangleSurface::setTexCoord(int index, ofVec2f t){
	if(index > 2){
		ofLog() << "Texture coordinate with this index does not exist: " << index
				<< endl;
		return;
	}

	mesh.setTexCoord(index, t);
}

void TriangleSurface::setTexCoords(vector<ofVec2f> t){
	if(t.size() != 3){
		throw runtime_error("Wrong number of texture coordinates");
	}
	for(int i = 0; i < 3; ++i){
		mesh.setTexCoord(i, t[i]);
	}
}

void TriangleSurface::moveBy(ofVec2f v){
	vector <ofVec3f> & vertices = getVertices();
	
	for(int i = 0; i < vertices.size(); i++){
		vertices[i] += ofVec3f(v.x, v.y, 0);
	}
	
	setMoved(true);
	ofNotifyEvent(verticesChangedEvent, mesh.getVertices(), this);
}

int TriangleSurface::getType(){
	return SurfaceType::TRIANGLE_SURFACE;
}

bool TriangleSurface::hitTest(ofVec2f p){
	// Construct ofPolyline from vertices
	ofPolyline line = getHitArea();

	if(line.inside(p.x, p.y)){
		return true;
	}else{
		return false;
	}
}

ofVec3f TriangleSurface::getVertex(int index){
	if(index > 2){
		ofLog() << "Vertex with this index does not exist: " << index << endl;
		throw runtime_error("Vertex index out of bounds.");
	}

	return mesh.getVertex(index);
}

ofVec2f TriangleSurface::getTexCoord(int index){
	if(index > 2){
		throw runtime_error("Texture coordinate index out of bounds.");
	}

	return mesh.getTexCoord(index);
}

ofPolyline TriangleSurface::getHitArea(){
	ofPolyline line;
	line.addVertex(ofPoint(mesh.getVertex(0).x, mesh.getVertex(0).y));
	line.addVertex(ofPoint(mesh.getVertex(1).x, mesh.getVertex(1).y));
	line.addVertex(ofPoint(mesh.getVertex(2).x, mesh.getVertex(2).y));
	line.close();

	return line;
}

ofPolyline TriangleSurface::getTextureHitArea(){
	ofPolyline line;
	vector <ofVec2f> & texCoords = mesh.getTexCoords();
	ofVec2f textureSize = ofVec2f(source->getTexture()->getWidth(), source->getTexture()->getHeight());
	for(int i = 0; i < texCoords.size(); i++){
		line.addVertex(ofPoint(texCoords[i] * textureSize));
	}
	line.close();

	return line;
}

vector <ofVec3f> & TriangleSurface::getVertices(){
	return mesh.getVertices();
}

vector <ofVec2f> & TriangleSurface::getTexCoords(){
	return mesh.getTexCoords();
}

BaseSurface * TriangleSurface::clone(){
	TriangleSurface * s = new TriangleSurface();
	s->setVertices(getVertices());
	s->setTexCoords(getTexCoords());
    BaseSource * src = getSource();
    src->referenceCount++;
	s->setSource(src);
	return s;
}

} // namespace piMapper
} // namespace ofx
