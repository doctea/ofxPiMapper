#include "SurfaceManager.h"

namespace ofx {
namespace piMapper {

SurfaceManager::SurfaceManager(){
	mediaServer = 0;
	selectedSurface = 0;
	
	//SurfaceStack * preset = new SurfaceStack();
	//_presets.push_back(preset);
	//_activePresetIndex = 0;
	
	_activePresetIndex = -1;
	_selectedVertexIndex = -1;
}

void SurfaceManager::draw(){
	if(_activePresetIndex < 0){
		return;
	}
	ofEnableAlphaBlending();
	ofSetColor(255, 255, 255, transparency());
	_presets[_activePresetIndex]->draw();
}

void SurfaceManager::addSurface(BaseSurface * surface){
	if(_activePresetIndex < 0){
		ofLogWarning("SurfaceManager::addSurface", "Can not add surface. No active preset.");
		return;
	}

	_presets[_activePresetIndex]->push_back(surface);
}

void SurfaceManager::removeSelectedSurface(){
	if(_activePresetIndex < 0){
		ofLogWarning(
			"SurfaceManager::removeSelectedSurface",
			"Can not remove surface. No Active preset");
		return;
	}
	
	if(selectedSurface == 0){
		return;
	}
	
	for(int i = 0; i < _presets[_activePresetIndex]->size(); i++){
		if(_presets[_activePresetIndex]->at(i) == selectedSurface){
			_presets[_activePresetIndex]->erase(i);
			selectedSurface = 0;
			_selectedVertexIndex = -1;
			break;
		}
	}
}

void SurfaceManager::removeSurface(int i){
	if(_activePresetIndex < 0){
		ofLogWarning("SurfaceManager::removeSurface", "Can not remove surface. No active preset.");
		return;
	}
	
	if(_presets[_activePresetIndex]->size() <= 0){
		return;
	}
	
	//_presets[_activePresetIndex]->erase(i);
	_presets[_activePresetIndex]->swap(i, _presets[_activePresetIndex]->size() - 1);
	_presets[_activePresetIndex]->pop_back();
	selectedSurface = 0;
}

void SurfaceManager::removeSurface(){
	if(_activePresetIndex < 0){
		ofLogWarning("SurfaceManager::removeSurface", "Can not remove surface. No active preset.");
		return;
	}
	
	if(_presets[_activePresetIndex]->size() <= 0){
		return;
	}
	
    BaseSurface * s = _presets[_activePresetIndex]->back();
	_presets[_activePresetIndex]->pop_back();
	selectedSurface = 0;
    delete s;
}

void SurfaceManager::deleteSurface(ofx::piMapper::BaseSurface * surface){
	if(_activePresetIndex < 0){
		ofLogWarning("SurfaceManager::deleteSurface", "Can not delete surface. No active preset.");
		return;
	}

	for(int i = 0; i < _presets[_activePresetIndex]->size(); ++i){
		if(_presets[_activePresetIndex]->at(i) == surface){
			_presets[_activePresetIndex]->erase(i);
			break;
		}
	}
}

void SurfaceManager::clearPresets(){
	for(unsigned int i = 0; i < _presets.size(); ++i){
		while(_presets[i]->size()){
			delete _presets[i]->back();
			_presets[i]->pop_back();
		}
	}
	_presets.clear();
}

// TODO: Do serious adjustment here. We need to save all presets. Not just the active one.
void SurfaceManager::saveXmlSettings(std::string fileName){
	if(_presets.size() <= 0){
		ofLogWarning(
			"SurfaceManager::saveXmlSettings",
			"Can not save XML settings. No presets.");
		return;
	}
	
	if(mediaServer == 0){
		ofLogFatalError("SurfaceManager") << "Media server not set";
		exit(EXIT_FAILURE);
	}
	
	SettingsLoader::instance()->save(*this, fileName);
}

// TODO: We need to load all presets. Not just the active one.
bool SurfaceManager::loadXmlSettings(std::string fileName){
	if(mediaServer == 0){
		ofLogFatalError("SurfaceManager") << "Media server not set";
		exit(EXIT_FAILURE);
	}
	ofLogNotice("SurfaceManager") << "before load()";
	bool success = SettingsLoader::instance()->load(*this, *mediaServer, fileName);

	ofLogNotice("SurfaceManager") << "after load()";

    for (int i=0; i<_presets.size(); i++){
        setPresetSourcesActiveState(i, false);
    }

	ofLogNotice("SurfaceManager") << "after presets deactivate loop()";

    setPresetSourcesActiveState(_activePresetIndex, true);

	ofLogNotice("SurfaceManager") << "after preset set active";
	return success;
}

void SurfaceManager::setMediaServer(MediaServer * newMediaServer){
	mediaServer = newMediaServer;
}

BaseSurface * SurfaceManager::selectSurface(int index){
	if(_activePresetIndex < 0){
		ofLogWarning("SurfaceManager::selectSurface", "Can not select surface. No active preset.");
		return 0;
	}

	if(index >= _presets[_activePresetIndex]->size()){
		throw runtime_error("Surface index out of bounds.");
	}

	selectedSurface = _presets[_activePresetIndex]->at(index);
	_selectedVertexIndex = -1;
	ofSendMessage("surfaceSelected");
	return selectedSurface;
}

BaseSurface * SurfaceManager::selectSurface(BaseSurface * surface){
	if(_activePresetIndex < 0){
		ofLogWarning("SurfaceManager::selectSurface", "Can not select surface. No active preset.");
	}
	
	for(int i = 0; i < _presets[_activePresetIndex]->size(); i++){
		if(_presets[_activePresetIndex]->at(i) == surface){
			selectedSurface = surface;
			_selectedVertexIndex = -1;
			ofSendMessage("surfaceSelected");
			ofNotifyEvent(surfaceSelectedEvent, i, this);
			return selectedSurface;
		}
	}
	
	deselectSurface();
	return 0;
}

BaseSurface * SurfaceManager::selectNextSurface(){
	if(_activePresetIndex < 0){
		ofLogWarning(
			"SurfaceManager::selectNextSurface",
			"Can not select next surface. No active preset.");
		return 0;
	}

	int next;
	_selectedVertexIndex = -1;
	
	if(selectedSurface == 0){
		next = 0;
		selectedSurface = selectSurface(next);
		ofNotifyEvent(surfaceSelectedEvent, next, this);
		return selectedSurface;
	}
	
	for(int i = 0; i < _presets[_activePresetIndex]->size(); ++i){
		if(_presets[_activePresetIndex]->at(i) == selectedSurface){
			if(i < _presets[_activePresetIndex]->size() - 1){
				next = i + 1;
			}else{
				next = 0;
			}
			
			selectedSurface = _presets[_activePresetIndex]->at(next);
			ofNotifyEvent(surfaceSelectedEvent, next, this);
			return selectedSurface;
		}
	}
	
	return 0;
}

BaseSurface * SurfaceManager::selectPrevSurface(){
	if(_activePresetIndex < 0){
		ofLogWarning(
			"SurfaceManager::selectPrevSurface",
			"Can not select prev surface. No active preset.");
		return 0;
	}

	int prev;
	_selectedVertexIndex = -1;
	
	if(selectedSurface == 0){
		prev = _presets[_activePresetIndex]->size() - 1;
		selectedSurface = selectSurface(prev);
		ofNotifyEvent(surfaceSelectedEvent, prev, this);
		return selectedSurface;
	}
	
	for(int i = 0; i < _presets[_activePresetIndex]->size(); ++i){
		if(_presets[_activePresetIndex]->at(i) == selectedSurface){
			if(i > 0){
				prev = i - 1;
			}else{
				prev = _presets[_activePresetIndex]->size() - 1;
			}
			
			selectedSurface = _presets[_activePresetIndex]->at(prev);
			ofNotifyEvent(surfaceSelectedEvent, prev, this);
			return selectedSurface;
		}
	}
	
	return 0;
}

BaseSurface * SurfaceManager::getSelectedSurface(){
	return selectedSurface;
}

// TODO: select vertex should be implemented ad BaseSurface level
void SurfaceManager::selectNextVertex(){
	if(selectedSurface == 0){
		return;
	}
	
	int numVertices = selectedSurface->getVertices().size();
	
	if(_selectedVertexIndex == -1){
		_selectedVertexIndex = 0;
	}else if(_selectedVertexIndex < numVertices - 1){
		_selectedVertexIndex += 1;
	}else{
		_selectedVertexIndex = 0;
	}
	
	ofNotifyEvent(vertexSelectedEvent, _selectedVertexIndex, this);
}

// TODO: select vertex should be implemented at BaseSurface level
void SurfaceManager::selectPrevVertex(){
	if(selectedSurface == 0){
		return;
	}
	
	int numVertices = selectedSurface->getVertices().size();
	
	if(_selectedVertexIndex > 0){
		_selectedVertexIndex -= 1;
	}else{
		_selectedVertexIndex = numVertices - 1;
	}
	
	ofNotifyEvent(vertexSelectedEvent, _selectedVertexIndex, this);
}

void SurfaceManager::selectVertex(int i){
	if(selectedSurface == 0){
		return;
	}
	
	if(i > selectedSurface->getVertices().size() - 1){
		ofLogError("SurfaceManager::selectVertex") << "index (" << i << ") out of bounds" << std::endl;
		int prevVertIndex = _selectedVertexIndex;
		ofNotifyEvent(vertexUnselectedEvent, prevVertIndex, this);
		_selectedVertexIndex = -1;
		return;
	}
	
	_selectedVertexIndex = i;
	ofNotifyEvent(vertexSelectedEvent, _selectedVertexIndex, this);
}

void SurfaceManager::moveSelectionBy(Vec3 v){
	if(selectedSurface == 0){
		moveAllSurfacesBy(v);
		return;
	}
	
	if(_selectedVertexIndex != -1){
		selectedSurface->getMesh().getVertices()[_selectedVertexIndex] += v.toOf();
		ofNotifyEvent(vertexChangedEvent, _selectedVertexIndex, this);
	}else{
		selectedSurface->moveBy(v);
	}
	
	// TODO: instead of having single selected surface
	// we want to have a selection of one or more.
	// it could be implemented as vector here.
}

void SurfaceManager::moveAllSurfacesBy(Vec3 v){
	if(_activePresetIndex < 0){
		ofLogWarning(
			"SurfaceManager::moveAllSurfacesBy",
			"Can not move surfaces. No active preset.");
		return;
	}

	for(int i = 0; i < _presets[_activePresetIndex]->size(); ++i){
		_presets[_activePresetIndex]->at(i)->moveBy(v);
	}
}

void SurfaceManager::deselectSurface(){
	selectedSurface = 0;
	_selectedVertexIndex = -1;
}

BaseSurface * SurfaceManager::getSurface(int index){
	if(_activePresetIndex < 0){
		throw runtime_error("No active preset.");
		return 0;
	}

	if(index >= _presets[_activePresetIndex]->size()){
		throw runtime_error("Surface index out of bounds.");
		return 0;
	}
	
	return _presets[_activePresetIndex]->at(index);
}

/* TODO: Solve fundamental question
 * What size are we talking about here? 
 * Is it the number of presets or surfaces?
 */
int SurfaceManager::size(){
	if(_activePresetIndex < 0){
		return 0;
	}
	
	return _presets[_activePresetIndex]->size();
}

int SurfaceManager::getSelectedVertexIndex(){
	return _selectedVertexIndex;
}

int SurfaceManager::getActivePresetIndex(){
	return _activePresetIndex;
}

int SurfaceManager::getSelectedSurfaceIndex(){
	if(selectedSurface == 0){
		return -1;
	}
	for(unsigned int i = 0; i < _presets[_activePresetIndex]->size(); ++i){
		if(_presets[_activePresetIndex]->getSurfaces()[i] == selectedSurface){
			return i;
		}
	}
	return -1;
}

unsigned int SurfaceManager::getNumPresets(){
	return _presets.size();
}

void SurfaceManager::onVertexChanged(int & i){
	ofNotifyEvent(vertexChangedEvent, i, this);
}

void SurfaceManager::onVerticesChanged(std::vector<Vec3> & vertices){
	ofNotifyEvent(verticesChangedEvent, vertices, this);
}

SurfaceStack * SurfaceManager::getActivePreset(){
	if(_activePresetIndex < 0){
		throw runtime_error(
			"SurfaceManager::getActivePreset: Can not getActivePreset. No active preset.");
	}

	return _presets[_activePresetIndex];
}

SurfaceStack * SurfaceManager::createPreset(){
	SurfaceStack * preset = new SurfaceStack();
	_presets.push_back(preset);
	
	// If we did not have any presets before, set the new as active one.
	if(_presets.size() == 1){
		_activePresetIndex = 0;
	}
	
	// Remember to remove these listeners when adding a removePreset method.
	ofAddListener(
		preset->vertexChangedEvent,
		this,
		&SurfaceManager::onVertexChanged);
	ofAddListener(
		preset->verticesChangedEvent,
		this,
		&SurfaceManager::onVerticesChanged);
	
	return preset;
	
	// TODO: Create command for this. And what not.
}


void SurfaceManager::swapPreset(int source_preset, int target_preset) {
	// swap..
	while (getNumPresets() <= target_preset) {
		printf("num presets is currently %i, so creating new?", getNumPresets());
		createPreset();
	}
	iter_swap(_presets.begin() + source_preset, _presets.begin() + target_preset);
	setPreset(target_preset);
}


SurfaceStack * SurfaceManager::getPresetAt(unsigned int i){
	if(i >= _presets.size()){
		throw runtime_error("SurfaceManager::getPresetAt index out of bounds.");
	}
	return _presets[i];
}

void SurfaceManager::setNextPreset(){
	if(_presets.size() <= 1){
		return;
	}
	
	if(_activePresetIndex == _presets.size() - 1){
		_activePresetIndex = 0;
	}else{
		_activePresetIndex += 1;
	}
	
	// TODO: Create command for this.
}

void SurfaceManager::setTransparency(byte transparency) {
	this->transparency_value = transparency;
}

int SurfaceManager::transparency() {
	uint64_t elapsed = ofGetElapsedTimeMillis();// preset_activated_at - ofGetElapsedTimeMillis();
	float fade_time_millis = 1000.0f;
	//float amt = elapsed / fade_time_millis;
	float t = ofMap(elapsed, 0.0f, fade_time_millis, 0.0f, (float)transparency_value, true);
	//printf("%5i: draw transparency %i\n", elapsed, (int)t);
	return t;
}

void SurfaceManager::setPresetSourcesActiveState(unsigned int presetIndex, bool state){
    // tell sources associated with current preset that they are not displayed any more
    // this is so that we can optionally update the buffers or not.
    for (int j=0; j<_presets[presetIndex]->getSurfaces().size(); j++){
        _presets[presetIndex]->getSurfaces()[j]->getSource()->setActive(state);
    }
}

void SurfaceManager::setPreset(unsigned int i){
	//preset_activated_at = ofGetSystemTimeMillis();
	ofResetElapsedTimeCounter();
    cout << "CALLED IT" << endl;
	if(_presets.size() < 1){
		throw runtime_error("ofxPiMapper: No presets to set.");
	}
	
	if(i >= _presets.size()){
		throw runtime_error("ofxPiMapper: Preset index out of bounds.");
	}
	
	cout << "about to setPresetSourcesActiveState" << endl;
    //let sources associated with all preset know that they are not being displayed any more
    setPresetSourcesActiveState(_activePresetIndex, false);

	cout << "about to setPresetSourcesActiveState again" << endl;
    //change preset
    _activePresetIndex = i;
    //let sources associated with NEW preset know that they are now being displayed
    setPresetSourcesActiveState(_activePresetIndex, true);

    //when preset it changed, call reset on all sources, if it's defined
    for (int i=0; i<_presets[_activePresetIndex]->getSurfaces().size(); i++){
		cout << "about to check if source is FBO" << endl;
        //if source is of type FBO then cast it from BaseSource to FboSource and call the beginFbo function
        /*if (_presets[_activePresetIndex]->getSurfaces()[i]->getSource()->getType() == SourceType::SOURCE_TYPE_FBO){
            FboSource *fboSource;
			cout << "about to do getSource" << endl;
            fboSource = (FboSource*)_presets[_activePresetIndex]->getSurfaces()[i]->getSource();
			cout << "about to do beginFbo" << endl;
            fboSource->beginFbo();
			cout << "loop end" << endl;
        }
		cout << "checked if source is FBO" << endl;*/

        _presets[_activePresetIndex]->getSurfaces()[i]->getSource()->reset();

		/*cout << "about to check if source is FBO again" << endl;
        //if source is of type FBO then cast it from BaseSource to FboSource and call the endFbo function
        if (_presets[_activePresetIndex]->getSurfaces()[i]->getSource()->getType() == SourceType::SOURCE_TYPE_FBO){
            FboSource *fboSource;
            fboSource = (FboSource*)_presets[_activePresetIndex]->getSurfaces()[i]->getSource();
            fboSource->endFbo();
        }
		cout << "checked if source is again" << endl;*/
    }
}

void SurfaceManager::cloneActivePreset(){
	if(_presets.size() <= 0){
		return;
	}
	
	SurfaceStack * preset = _presets[_activePresetIndex]->clone();
	_presets.push_back(preset);
	_activePresetIndex = _presets.size() - 1;
}

void SurfaceManager::eraseActivePreset(){
	if(_presets.size() <= 0){
		return;
	}
	
	_presets[_activePresetIndex]->clear();
	_presets.erase(_presets.begin() + _activePresetIndex);
	
	_activePresetIndex--;
	
	if(_presets.size() > 0 && _activePresetIndex < 0){
		_activePresetIndex = 0;
	}
	
	if(_activePresetIndex < 0){
		createPreset();
	}
}

} // namespace piMapper
} // namespace ofx
