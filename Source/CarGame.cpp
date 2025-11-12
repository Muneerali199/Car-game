// CarGame.cpp
// Main game module implementation
// Copyright 2025. All Rights Reserved.

#include "CarGame.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FCarGameModule, CarGame, "CarGame" );

void FCarGameModule::StartupModule()
{
	// This code will execute after your module is loaded into memory
	UE_LOG(LogTemp, Log, TEXT("CarGame Module Started"));
}

void FCarGameModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module
	UE_LOG(LogTemp, Log, TEXT("CarGame Module Shutdown"));
}
