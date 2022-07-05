// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "ShooterGameUserSettings.h"
#include "Performance/MaxTickRateHandlerModule.h"
#include "Performance/LatencyMarkerModule.h"

UShooterGameUserSettings::UShooterGameUserSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetToDefaults();
}

void UShooterGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();

	GraphicsQuality = 1;	
	bIsLanMatch = true;
	bIsDedicatedServer = false;
	bIsForceSystemResolution = false;
	NVIDIAReflex = 1;
}

void UShooterGameUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	if (GraphicsQuality == 0)
	{
		ScalabilityQuality.SetFromSingleQualityLevel(1);
	}
	else
	{
		ScalabilityQuality.SetFromSingleQualityLevel(3);
	}

	InitNVIDIAReflex();

	Super::ApplySettings(bCheckForCommandLineOverrides);
}

void UShooterGameUserSettings::InitNVIDIAReflex()
{
	UShooterGameUserSettings* const UserSettings = CastChecked<UShooterGameUserSettings>(GEngine->GetGameUserSettings());
	if (UserSettings == NULL)
		return;

	TArray<IMaxTickRateHandlerModule*> MaxTickRateHandlerModules = IModularFeatures::Get().GetModularFeatureImplementations<IMaxTickRateHandlerModule>(IMaxTickRateHandlerModule::GetModularFeatureName());
	for (IMaxTickRateHandlerModule* MaxTickRateHandlerModule : MaxTickRateHandlerModules)
	{
		if (UserSettings->GetNVIDIAReflex() != 0)
		{
			MaxTickRateHandlerModule->SetEnabled(true);
			MaxTickRateHandlerModule->SetFlags(UserSettings->GetNVIDIAReflex() == 1 ? 1 : 3);
		}
		else
		{
			MaxTickRateHandlerModule->SetEnabled(false);
		}
	}

	TArray<ILatencyMarkerModule*> LatencyMarkerModules = IModularFeatures::Get().GetModularFeatureImplementations<ILatencyMarkerModule>(ILatencyMarkerModule::GetModularFeatureName());
	for (ILatencyMarkerModule* LatencyMarkerModule : LatencyMarkerModules)
	{
		if (UserSettings->GetLatencyFlashIndicator() != 0)
		{
			LatencyMarkerModule->SetFlashIndicatorEnabled(true);
		}
		else
		{
			LatencyMarkerModule->SetFlashIndicatorEnabled(false);
		}
	}
}