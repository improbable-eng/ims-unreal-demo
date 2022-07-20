// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "SessionSearch.h"

Session::Session(IMSSessionManagerAPI::OpenAPIV0Session SessionResponse) : IMSSessionManagerAPI::OpenAPIV0Session(SessionResponse)
{
	const IMSSessionManagerAPI::OpenAPIV0Port* GamePortResponse = Ports.FindByPredicate([](IMSSessionManagerAPI::OpenAPIV0Port Port) { return Port.Name == "GamePort"; });

	if (GamePortResponse != nullptr)
	{
		SessionAddress = Address + ":" + FString::FromInt(GamePortResponse->Port);;
	}
}

Session::~Session()
{
}

const FString Session::GetSessionAddress() const
{
	return SessionAddress.IsEmpty() ? "Unknown" : SessionAddress;
}

const FString Session::GetFromSessionStatus(FString Key) const
{
	if (SessionStatus.Contains(Key))
	{
		return *SessionStatus.Find(Key);
	}

	return "Unknown";
}

const FString Session::GetPlayerCount() const
{
	if (SessionStatus.Contains("CurrentNumPlayers") && SessionStatus.Contains("MaxNumPlayers"))
	{
		return *SessionStatus.Find("CurrentNumPlayers") + "/" + *SessionStatus.Find("MaxNumPlayers");
	}

	return "Unknown";
}
