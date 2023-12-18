// Copyright (C) 2023 owoDra

#pragma once

#include "Modules/ModuleManager.h"

/**
 *  Modules for the main features of the Game Team Extension plugin
 */
class FGTExtModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};
