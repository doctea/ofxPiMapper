// SetActivePresetCmd
// Sets active preset, stores previous preset index and restores it on undo.
// Created by Krisjanis Rijnieks 2016-10-07
// At the ORA bar in Berlin. Waiting for Park to celebrate his birthday.

#pragma once

#include "BaseCmd.h"
#include "BaseSurface.h"
#include "Application.h"

namespace ofx {
namespace piMapper {

class Application;

class SetActivePresetCmd : public BaseUndoCmd {

	public:
		SetActivePresetCmd(Application * app, unsigned int pi);
		void exec();
		void undo();

	private:
		Application * _app;
		unsigned int _newPresetIndex;
		unsigned int _prevPresetIndex;
};

} // namespace piMapper
} // namespace ofx
