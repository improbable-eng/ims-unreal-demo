// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OpenAPISessionManagerV0Api.h"
#include "OpenAPIV0Session.h"
#include "OpenAPIV0Port.h"
#include "CoreMinimal.h"

enum SearchState
{
	/** The task has not been started */
	NotStarted,
	/** The task is currently being processed */
	InProgress,
	/** The task has completed successfully */
	Done,
	/** The task failed to complete */
	Failed
};

class Session : public IMSSessionManagerAPI::OpenAPIV0Session
{
public:
	FString SessionAddress;

public:
	Session(IMSSessionManagerAPI::OpenAPIV0Session SessionResponse);
	~Session();

	const FString GetSessionAddress() const;
	const FString GetPlayerCount() const;
	const FString GetFromSessionStatus(FString Key) const;
	const FString GetGamePhase() const { return GetFromSessionStatus("GamePhase"); }
	const FString GetMapName() const { return GetFromSessionStatus("MapName"); }
};

class SessionSearch
{
public:
	int32 MaxSearchResults;
	TArray<Session> SearchResults;
	SearchState SearchState;

public:
	SessionSearch() : MaxSearchResults(8), SearchState(SearchState::NotStarted) {}
	~SessionSearch() {}

};