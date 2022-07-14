// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Online.h"
#include "ShooterLeaderboards.h"
#include "HttpModule.h"
#include "OpenAPISessionManagerV0Api.h"
#include "OpenAPISessionManagerV0ApiOperations.h"
#include "SessionSearch.h"
#include "ShooterGameSession.generated.h"


UCLASS(config=Game)
class SHOOTERGAME_API AShooterGameSession : public AGameSession
{
	GENERATED_UCLASS_BODY()

protected:

	UPROPERTY(config)
	FString IMSProjectId;

	UPROPERTY(config)
	FString IMSSessionType;

	/* Retry policy and configuration */
	int RetryLimitCount = 5;
	int RetryTimeoutRelativeSeconds = 10;
	IMSSessionManagerAPI::HttpRetryParams RetryPolicy;

	/* Session Manager API interface */
	TSharedPtr<IMSSessionManagerAPI::OpenAPISessionManagerV0Api> SessionManagerAPI;

	/* Session Manager Search */
	TSharedPtr<class SessionSearch> CurrentSessionSearch;

	/** Delegate for creating a new session */
	IMSSessionManagerAPI::OpenAPISessionManagerV0Api::FCreateSessionV0Delegate OnCreateSessionCompleteDelegate;
	/** Delegate for searching for sessions */
	IMSSessionManagerAPI::OpenAPISessionManagerV0Api::FListSessionsV0Delegate OnFindSessionsCompleteDelegate;
	/** Delegate after joining a session */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	// If ProjectId is set when running the client in command line then use that, otherwise use hardcoded default from config
	FString GetIMSProjectId();

	// If SessionType is set when running the client in command line then use that, otherwise use hardcoded default from config
	FString GetIMSSessionType();

	/**
	 * Delegate fired when a session create request has completed
	 */
	void OnCreateSessionComplete(const IMSSessionManagerAPI::OpenAPISessionManagerV0Api::CreateSessionV0Response& Response);

	/**
	 * Delegate fired when a session search query has completed
	 *
	 * @param bWasSuccessful true if the async action completed without error, false if there was an error
	 */
	void OnFindSessionsComplete(const IMSSessionManagerAPI::OpenAPISessionManagerV0Api::ListSessionsV0Response& Response);

	/**
	 * Create session config for create session request
	 */
	FString CreateSessionConfigJson(const int32 MaxNumPlayers, const int32 BotsCount);

	/*
	 * Event triggered when a session is created
	 */
	DECLARE_EVENT_TwoParams(AShooterGameSession, FOnCreateSessionComplete, FString /*SessionAddress*/, bool /*bWasSuccessful*/);
	FOnCreateSessionComplete CreateSessionCompleteEvent;

	/* 
	 * Event triggered when a session is joined
	 *
	 * @param bWasSuccessful was the join successful
	 */
	DECLARE_EVENT_OneParam(AShooterGameSession, FOnJoinSessionComplete, bool /*bWasSuccessful*/);
	FOnJoinSessionComplete JoinSessionCompleteEvent;

	/*
	 * Event triggered after session search completes
	 */
	//DECLARE_EVENT(AShooterGameSession, FOnFindSessionsComplete);
	DECLARE_EVENT_OneParam(AShooterGameSession, FOnFindSessionsComplete, bool /*bWasSuccessful*/);
	FOnFindSessionsComplete FindSessionsCompleteEvent;

public:

	/**
	 * Host a new online session
	 *
	 * @param MaxNumPlayers Maximum number of players to allow in the session
	 * @param BotsCount Number of bots to spawn in the session
	 * @param SessionTicket Ticket to authenticate session creation
	 */
	void HostSession(const int32 MaxNumPlayers, const int32 BotsCount, const FString SessionTicket);

	/**
	 * Find an online session
	 *
	 * @param UserId user that initiated the request
	 * @param SessionName name of session this search will generate
	 * @param bIsLAN are we searching LAN matches
	 * @param bIsPresence are we searching presence sessions
	 */
	void FindSessions(FString SessionTicket);

	/**
	 * Joins one of the session in search results
	 *
	 * @param SessionIndexInSearchResults Index of the session in search results
	 *
	 * @return bool true if successful, false otherwise
	 */
	bool JoinSession(int32 SessionIndexInSearchResults);

	/**
	 * Joins a session via its address after creating a session
	 *
	 * @param SessionAddress session to join
	 *
	 * @return bool true if successful, false otherwise
	 */
	bool JoinSession(FString SessionAddress);

	const SearchState GetSearchSessionsStatus() const;
	const TArray<Session>& GetSearchResults() const;

	/** @return the delegate fired when creating a session */
	FOnCreateSessionComplete& OnCreateSessionComplete() { return CreateSessionCompleteEvent; }

	/** @return the delegate fired when joining a session */
	FOnJoinSessionComplete& OnJoinSessionComplete() { return JoinSessionCompleteEvent; }

	/** @return the delegate fired when search of session completes */
	FOnFindSessionsComplete& OnFindSessionsComplete() { return FindSessionsCompleteEvent; }

	/**
	 * Travel to a session address (as client) for a given session
	 *
	 * @return true if successful, false otherwise
	 */
	bool TravelToSession(FString SessionAddress);
};

