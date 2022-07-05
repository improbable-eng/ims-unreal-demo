// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ShooterGameUserSettings.generated.h"

UCLASS()
class UShooterGameUserSettings : public UGameUserSettings
{
	GENERATED_UCLASS_BODY()

	/** Applies all current user settings to the game and saves to permanent storage (e.g. file), optionally checking for command line overrides. */
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;

	static void InitNVIDIAReflex();

	int32 GetGraphicsQuality() const
	{
		return GraphicsQuality;
	}

	void SetGraphicsQuality(int32 InGraphicsQuality)
	{
		GraphicsQuality = InGraphicsQuality;
	}

	int32 GetNVIDIAReflex() const
	{
		return NVIDIAReflex;
	}

	void SetNVIDIAReflex(int32 InNVIDIAReflex)
	{
		NVIDIAReflex = InNVIDIAReflex;
	}

	int32 GetLatencyFlashIndicator() const
	{
		return LatencyFlashIndicator;
	}

	void SetLatencyFlashIndicator(int32 InLatencyFlashIndicator)
	{
		LatencyFlashIndicator = InLatencyFlashIndicator;
	}

	int32 GetFramerateVisibility() const
	{
		return FramerateVisibility;
	}

	void SetFramerateVisibility(int32 InFramerateVisibility)
	{
		FramerateVisibility = InFramerateVisibility;
	}

	int32 GetGameToRenderVisibility() const
	{
		return GameToRenderVisibility;
	}

	void SetGameToRenderVisibility(int32 InGameToRenderVisibility)
	{
		GameToRenderVisibility = InGameToRenderVisibility;
	}

	int32 GetGameLatencyVisibility() const
	{
		return GameLatencyVisibility;
	}

	void SetGameLatencyVisibility(int32 InGameLatencyVisibility)
	{
		GameLatencyVisibility = InGameLatencyVisibility;
	}

	int32 GetRenderLatencyVisibility() const
	{
		return RenderLatencyVisibility;
	}

	void SetRenderLatencyVisibility(int32 InRenderLatencyVisibility)
	{
		RenderLatencyVisibility = InRenderLatencyVisibility;
	}

	bool IsLanMatch() const
	{
		return bIsLanMatch;
	}

	bool IsForceSystemResolution() const
	{
		return bIsForceSystemResolution;
	}

	void SetLanMatch(bool InbIsLanMatch)
	{
		bIsLanMatch = InbIsLanMatch;
	}
	
	bool IsDedicatedServer() const
	{
		return bIsDedicatedServer;
	}

	void SetDedicatedServer(bool InbIsDedicatedServer)
	{
		bIsDedicatedServer = InbIsDedicatedServer;
	}

	void SetForceSystemResolution(bool InbIsForceSystemResolution)
	{
		bIsForceSystemResolution = InbIsForceSystemResolution;
	}

	// interface UGameUserSettings
	virtual void SetToDefaults() override;

private:
	/**
	 * Graphics Quality
	 *	0 = Low
	 *	1 = High
	 */
	UPROPERTY(config)
	int32 GraphicsQuality;

	/** NVIDIA Reflex */
	UPROPERTY(config)
	int32 NVIDIAReflex;

	/** Latency Flash Indicator */
	UPROPERTY(config)
	int32 LatencyFlashIndicator;

	/** Framerate meter */
	UPROPERTY(config)
	int32 FramerateVisibility;

	/** Game to render latency meter */
	UPROPERTY(config)
	int32 GameToRenderVisibility;

	/** Game latency meter */
	UPROPERTY(config)
	int32 GameLatencyVisibility;

	/** Render latency meter */
	UPROPERTY(config)
	int32 RenderLatencyVisibility;

	/** is lan match? */
	UPROPERTY(config)
	bool bIsLanMatch;

	/** is dedicated server? */
	UPROPERTY(config)
	bool bIsDedicatedServer;

	/** Enable if UShooterGameUserSettings is not the authority on resolution */
	UPROPERTY(config)
	bool bIsForceSystemResolution;
};
