// FlipSurfaceCmd
// Flips selected surface
// Created by Krisjanis Rijnieks 2016-03-04

#pragma once

#include "BaseCmd.h"
#include "BaseSurface.h"
#include "SurfaceManager.h"
#include "Vec3.h"

namespace ofx {
namespace piMapper {

class FlipSurfaceCmd : public BaseUndoCmd {

	public:
		FlipSurfaceCmd(BaseSurface * surface, SurfaceManager * surfaceManager);
		void exec();
		void undo();

	private:
		BaseSurface * _surface;
		SurfaceManager * _surfaceManager;

};

} // namespace piMapper
} // namespace ofx

