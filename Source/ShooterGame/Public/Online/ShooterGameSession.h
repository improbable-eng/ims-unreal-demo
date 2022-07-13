// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Online.h"
#include "ShooterLeaderboards.h"
#include "HttpModule.h"
#include "OpenAPISessionManagerV0Api.h"
#include "OpenAPISessionManagerV0ApiOperations.h"
#include "SessionSearch.h"
#include "ShooterGameSession.generated.h"

struct FShooterGameSessionParams
{
	/** Name of session settings are stored with */
	FName SessionName;
	/** LAN Match */
	bool bIsLAN;
	/** Presence enabled session */
	bool bIsPresence;
	/** Id of player initiating lobby */
	TSharedPtr<const FUniqueNetId> UserId;
	/** Current search result choice to join */
	int32 BestSessionIdx;

	FShooterGameSessionParams()
		: SessionName(NAME_None)
		, bIsLAN(false)
		, bIsPresence(false)
		, BestSessionIdx(0)
	{
	}
};

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
	/** Delegate after starting a session */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	/** Delegate for destroying a session */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	/** Delegate for searching for sessions */
	IMSSessionManagerAPI::OpenAPISessionManagerV0Api::FListSessionsV0Delegate OnFindSessionsCompleteDelegate;
	/** Delegate after joining a session */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	/** Transient properties of a session during game creation/matchmaking */
	FShooterGameSessionParams CurrentSessionParams;
	/** Current host settings */
	TSharedPtr<class FShooterOnlineSessionSettings> HostSettings;
	/** Current search settings */
	TSharedPtr<class FShooterOnlineSearchSettings> SearchSettings;

	/**
	 * Delegate fired when a session create request has completed
	 */
	void OnCreateSessionComplete(const IMSSessionManagerAPI::OpenAPISessionManagerV0Api::CreateSessionV0Response& Response);

	/**
	 * Delegate fired when a session start request has completed
	 *
	 * @param SessionName the name of the session this callback is for
	 * @param bWasSuccessful true if the async action completed without error, false if there was an error
	 */
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	/**
	 * Delegate fired when a session search query has completed
	 *
	 * @param bWasSuccessful true if the async action completed without error, false if there was an error
	 */
	void OnFindSessionsComplete(const IMSSessionManagerAPI::OpenAPISessionManagerV0Api::ListSessionsV0Response& Response);

	/**
	 * Delegate fired when a session join request has completed
	 *
	 * @param SessionName the name of the session this callback is for
	 * @param bWasSuccessful true if the async action completed without error, false if there was an error
	 */
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/**
	 * Delegate fired when a destroying an online session has completed
	 *
	 * @param SessionName the name of the session this callback is for
	 * @param bWasSuccessful true if the async action completed without error, false if there was an error
	 */
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);	

	/**
	 * Reset the variables the are keeping track of session join attempts
	 */
	void ResetBestSessionVars();

	/**
	 * Choose the best session from a list of search results based on game criteria
	 */
	void ChooseBestSession();

	/**
	 * Entry point for matchmaking after search results are returned
	 */
	void StartMatchmaking();

	/**
	 * Return point after each attempt to join a search result
	 */
	void ContinueMatchmaking();

	/**
	 * Delegate triggered when no more search results are available
	 */
	void OnNoMatchesAvailable();

	/**
	 * Create session config for create session request
	 */
	FString CreateSessionConfigJson(const int32 MaxNumPlayers, const int32 BotsCount);

	/*
	 * Event triggered when a session is created
	 */
	DECLARE_EVENT_TwoParams(AShooterGameSession, FOnCreateSessionIMSComplete, FString /*SessionAddress*/, bool /*bWasSuccessful*/);
	FOnCreateSessionIMSComplete CreateSessionCompleteEvent;

	/* 
	 * Event triggered when a session is joined
	 *
	 * @param SessionName name of session that was joined
	 * @param bWasSuccessful was the create successful
	 */
	//DECLARE_DELEGATE_RetVal_TwoParams(bool, FOnJoinSessionComplete, FName /*SessionName*/, bool /*bWasSuccessful*/);
	DECLARE_EVENT_OneParam(AShooterGameSession, FOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type /*Result*/);
	FOnJoinSessionComplete JoinSessionCompleteEvent;

	/*
	 * Event triggered after session search completes
	 */
	//DECLARE_EVENT(AShooterGameSession, FOnFindSessionsComplete);
	DECLARE_EVENT_OneParam(AShooterGameSession, FOnFindSessionsComplete, bool /*bWasSuccessful*/);
	FOnFindSessionsComplete FindSessionsCompleteEvent;

public:

	/** Default number of players allowed in a game */
	static const int32 DEFAULT_NUM_PLAYERS = 8;

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
	 * @param UserId user that initiated the request
	 * @param SessionName name of session 
	 * @param SessionIndexInSearchResults Index of the session in search results
	 *
	 * @return bool true if successful, false otherwise
	 */
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, int32 SessionIndexInSearchResults);

	/**
	 * Joins a session via a search result
	 *
	 * @param SessionName name of session 
	 * @param SearchResult Session to join
	 *
	 * @return bool true if successful, false otherwise
	 */
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	/** @return true if any online async work is in progress, false otherwise */
	bool IsBusy() const;

	const SearchState GetSearchSessionsStatus() const;
	const TArray<Session>& GetSearchResults() const;

	/** @return the delegate fired when creating a session */
	FOnCreateSessionIMSComplete& OnCreateSessionComplete() { return CreateSessionCompleteEvent; }

	/** @return the delegate fired when joining a session */
	FOnJoinSessionComplete& OnJoinSessionComplete() { return JoinSessionCompleteEvent; }

	/** @return the delegate fired when search of session completes */
	FOnFindSessionsComplete& OnFindSessionsComplete() { return FindSessionsCompleteEvent; }

	/** Handle starting the match */
	virtual void HandleMatchHasStarted() override;

	/** Handles when the match has ended */
	virtual void HandleMatchHasEnded() override;

	/**
	 * Travel to a session URL (as client) for a given session
	 *
	 * @param ControllerId controller initiating the session travel
	 * @param SessionName name of session to travel to
	 *
	 * @return true if successful, false otherwise
	 */
	bool TravelToSession(int32 ControllerId, FName SessionName);

	/** Handles to various registered delegates */
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
};

