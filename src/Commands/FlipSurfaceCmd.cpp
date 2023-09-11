#include "FlipSurfaceCmd.h"

namespace ofx {
namespace piMapper {

FlipSurfaceCmd::FlipSurfaceCmd(BaseSurface * surface, SurfaceManager * surfaceManager){
	_surface = surface;
	_surfaceManager = surfaceManager;
}

void FlipSurfaceCmd::exec(){
	ofLogNotice("FlipSurfaceCmd", "exec");
    _surface->flip();
}

void FlipSurfaceCmd::undo(){
	ofLogNotice("FlipSurfaceCmd", "undo");
    _surface->flip();
}

} // namespace piMapper
} // namespace ofx

