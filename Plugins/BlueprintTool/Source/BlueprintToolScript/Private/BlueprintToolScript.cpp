// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BlueprintToolScript.h"

#define LOCTEXT_NAMESPACE "FBlueprintToolScriptModule"

void FBlueprintToolScriptModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FBlueprintToolScriptModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBlueprintToolScriptModule, BlueprintToolScript)