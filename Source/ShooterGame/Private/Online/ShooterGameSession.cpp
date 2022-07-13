// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "ShooterGameSession.h"
#include "ShooterOnlineGameSettings.h"
#include "OnlineSubsystemSessionSettings.h"
#include "OnlineSubsystemUtils.h"

namespace
{
	const FString CustomMatchKeyword("Custom");
}

AShooterGameSession::AShooterGameSession(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		OnCreateSessionCompleteDelegate = IMSSessionManagerAPI::OpenAPISessionManagerV0Api::FCreateSessionV0Delegate::CreateUObject(this, &AShooterGameSession::OnCreateSessionComplete);
		OnFindSessionsCompleteDelegate = IMSSessionManagerAPI::OpenAPISessionManagerV0Api::FListSessionsV0Delegate::CreateUObject(this, &AShooterGameSession::OnFindSessionsComplete);

		RetryPolicy = IMSSessionManagerAPI::HttpRetryParams(RetryLimitCount, RetryTimeoutRelativeSeconds);
		SessionManagerAPI = MakeShared<IMSSessionManagerAPI::OpenAPISessionManagerV0Api>();
		CurrentSessionSearch = MakeShared<class SessionSearch>();
	}
}

/**
 * Delegate fired when a session create request has completed
 */
void AShooterGameSession::OnCreateSessionComplete(const IMSSessionManagerAPI::OpenAPISessionManagerV0Api::CreateSessionV0Response& Response)
{
	if (Response.IsSuccessful() && Response.Content.Address.IsSet() && Response.Content.Ports.IsSet())
	{
		FString IP = Response.Content.Address.GetValue();
		// Filtering the ports in the response for the "GamePort", this value should match what you have indicated in your allocation
		const IMSSessionManagerAPI::OpenAPIV0Port* GamePortResponse = Response.Content.Ports.GetValue().FindByPredicate([](IMSSessionManagerAPI::OpenAPIV0Port PortResponse) { return PortResponse.Name == "GamePort"; });

		if (GamePortResponse != nullptr)
		{
			FString SessionAddress = IP + ":" + FString::FromInt(GamePortResponse->Port);

			UE_LOG(LogOnlineGame, Display, TEXT("Successfully created a session. Connect to session address: '%s'"), *SessionAddress);
			OnCreateSessionComplete().Broadcast(SessionAddress, true);
		}
		else
		{
			UE_LOG(LogOnlineGame, Error, TEXT("Successfully created a session but could not find the Game Port."));
			OnCreateSessionComplete().Broadcast("", false);
		}
	}
	else
	{
		UE_LOG(LogOnlineGame, Display, TEXT("Failed to create a session."));
		OnCreateSessionComplete().Broadcast("", false);
	}
}

void AShooterGameSession::HostSession(const int32 MaxNumPlayers, const int32 BotsCount, const FString SessionTicket)
{
	// See the following doc for more information https://docs.ims.improbable.io/docs/ims-session-manager/guides/authetication
	SessionManagerAPI->AddHeaderParam("Authorization", "Bearer playfab/" + SessionTicket);

	IMSSessionManagerAPI::OpenAPISessionManagerV0Api::CreateSessionV0Request Request;
	Request.SetShouldRetry(RetryPolicy);
	Request.ProjectId = IMSProjectId;
	Request.SessionType = IMSSessionType;

	IMSSessionManagerAPI::OpenAPIV0CreateSessionRequestBody RequestBody;
	RequestBody.SessionConfig = CreateSessionConfigJson(MaxNumPlayers, BotsCount);
	Request.Body = RequestBody;

	UE_LOG(LogOnlineGame, Display, TEXT("Attempting to create a session..."));
	SessionManagerAPI->CreateSessionV0(Request, OnCreateSessionCompleteDelegate);

	FHttpModule::Get().GetHttpManager().Flush(false);
}

void AShooterGameSession::OnFindSessionsComplete(const IMSSessionManagerAPI::OpenAPISessionManagerV0Api::ListSessionsV0Response& Response)
{
	if (Response.IsSuccessful())
	{
		UE_LOG(LogOnlineGame, Display, TEXT("Successfully listed sessions."));

		TArray<Session> SearchResults;

		for (IMSSessionManagerAPI::OpenAPIV0Session SessionResult : Response.Content.Sessions)
		{
			if (SearchResults.Num() < CurrentSessionSearch->MaxSearchResults)
			{
				SearchResults.Add(Session(SessionResult));
			}
		}

		CurrentSessionSearch->SearchResults = SearchResults;
		CurrentSessionSearch->SearchState = SearchState::Done;

		OnFindSessionsComplete().Broadcast(true);
	}
	else
	{
		UE_LOG(LogOnlineGame, Display, TEXT("Failed to list sessions."));
		CurrentSessionSearch->SearchState = SearchState::Failed;
		OnFindSessionsComplete().Broadcast(false);
	}
}

FString AShooterGameSession::CreateSessionConfigJson(const int32 MaxNumPlayers, const int32 BotsCount)
{
	UE_LOG(LogOnlineGame, Log, TEXT("Creating Session Config Json: MaxNumPlayers = %d, BotsCount = %d"), MaxNumPlayers, BotsCount);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField("MaxNumPlayers", MaxNumPlayers);
	JsonObject->SetNumberField("BotsCount", BotsCount);

	FString SessionConfig;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&SessionConfig);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	return SessionConfig;
}

const SearchState AShooterGameSession::GetSearchSessionsStatus() const
{
	return CurrentSessionSearch->SearchState;
}

const TArray<Session>& AShooterGameSession::GetSearchResults() const
{
	return CurrentSessionSearch->SearchResults;
}

void AShooterGameSession::FindSessions(FString SessionTicket)
{
	// See the following doc for more information https://docs.ims.improbable.io/docs/ims-session-manager/guides/authetication
	SessionManagerAPI->AddHeaderParam("Authorization", "Bearer playfab/" + SessionTicket);

	IMSSessionManagerAPI::OpenAPISessionManagerV0Api::ListSessionsV0Request Request;
	Request.SetShouldRetry(RetryPolicy);
	Request.ProjectId = IMSProjectId;
	Request.SessionType = IMSSessionType;

	UE_LOG(LogOnlineGame, Display, TEXT("Attempting to list sessions..."));
	CurrentSessionSearch->SearchState = SearchState::InProgress;
	SessionManagerAPI->ListSessionsV0(Request, OnFindSessionsCompleteDelegate);

	FHttpModule::Get().GetHttpManager().Flush(false);
}

bool AShooterGameSession::JoinSession(int32 SessionIndexInSearchResults)
{
	UE_LOG(LogOnlineGame, Display, TEXT("Attempting to join session..."));

	if (SessionIndexInSearchResults >= 0 && SessionIndexInSearchResults < CurrentSessionSearch->SearchResults.Num())
	{
		Session SessionToJoin = CurrentSessionSearch->SearchResults[SessionIndexInSearchResults];
		FString SessionAddress = SessionToJoin.GetSessionAddress();

		if (TravelToSession(SessionAddress))
		{
			OnJoinSessionComplete().Broadcast(true);
			return true;
		}
	}

	OnJoinSessionComplete().Broadcast(false);
	return false;
}

bool AShooterGameSession::JoinSession(FString SessionAddress)
{
	UE_LOG(LogOnlineGame, Display, TEXT("Attempting to join session..."));

	if (TravelToSession(SessionAddress))
	{
		OnJoinSessionComplete().Broadcast(true);
		return true;
	}

	OnJoinSessionComplete().Broadcast(false);
	return false;
}

bool AShooterGameSession::TravelToSession(FString SessionAddress)
{
	APlayerController* const PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController->ClientTravel(SessionAddress, TRAVEL_Absolute);
		return true;
	}

	return false;
}
