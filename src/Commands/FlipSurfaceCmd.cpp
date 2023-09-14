#include "FlipSurfaceCmd.h"

#include "Gui.h"

namespace ofx {
namespace piMapper {

FlipSurfaceCmd::FlipSurfaceCmd(BaseSurface * surface, SurfaceManager * surfaceManager){
	_surface = surface;
	_surfaceManager = surfaceManager;
}

void FlipSurfaceCmd::exec(){
	ofLogNotice("FlipSurfaceCmd", "exec");
    _surface->flip();
	Gui::instance()->getProjectionEditorWidget().updateJoints();
}

void FlipSurfaceCmd::undo(){
	ofLogNotice("FlipSurfaceCmd", "undo");
    _surface->flip();
	Gui::instance()->getProjectionEditorWidget().updateJoints();
}

} // namespace piMapper
} // namespace ofx

