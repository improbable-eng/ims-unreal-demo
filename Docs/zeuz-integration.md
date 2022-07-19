# Support IMS Zeuz orchestration in your game

## Table of Contents

- [Hosting your game on IMS zeuz](#hosting-your-game-on-ims-zeuz)
	1. [Set Payload to Ready upon Initialization](#1-set-payload-to-ready-upon-initialization)
	2. [Publish image to IMS Image Manager](#2-publish-image-to-ims-image-manager)
	3. [Create allocation](#3-create-allocation)
	4. [Reserve and join a game](#4-reserve-and-join-a-game)
	5. [Next steps](#5-next-steps)
- [Server Waiting & Shutdown](#server-waiting--shutdown)
	1. [Server Waiting](#1-server-waiting)
	2. [Server Shutdown](#2-server-shutdown)


## Hosting your game on IMS zeuz
### 1. Set Payload to Ready upon Initialization
>**Associated commits:** [Payload Local API Client Generation](https://github.com/improbable-eng/ims-unreal-demo/commit/b501e942e9ff7eca3ad4d4ef49b2430f29a10eba), [Temporary fix to OpenAPI bug involving body being set for GET requests](https://github.com/improbable-eng/ims-unreal-demo/commit/98e8f7d7ae7945fe03cf4ad3bef5a4fa1136a8b5), [Set Payload to Ready upon Initialization](https://github.com/improbable-eng/ims-unreal-demo/commit/6343c6e8e1ea883bd1e1ca770493b9b887f8fc56)

As a reminder, a simplified version of the [payload](https://docs.ims.improbable.io/docs/ims-zeuz/payload) lifecycle is:

![Payload lifecycle](/Docs/Assets/payload-lifecycle.png)

Once the payload has been created and the server initialized, the server needs to mark its payload as `Ready` so that the payload is available for reservation. This is achieved through a call to the [Payload Local API](https://docs.ims.improbable.io/openapi/ims-zeuz/payload-local-api), and should be done directly from the server code once the server has fully started up and is ready to accept player connections.

#### DIY Approach
In the server code, you can detect whether you are running on IMS zeuz by checking for the environment variable `ORCHESTRATION_PAYLOAD_API` which will be the localhost and port (e.g. `localhost:1234`). 
You then need to call the [Payload Ready API](https://docs.ims.improbable.io/openapi/ims-zeuz/payload-local-api#operation/GetPayloadV0). The following C++ snippet demonstrates that:
```cpp
FString payloadApiDomain = FPlatformMisc::GetEnvironmentVariable(*FString("ORCHESTRATION_PAYLOAD_API"));
FString payloadApiUrl = "http://" + payloadApiDomain + "/api/v0/ready";
TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
Request->SetURL(payloadApiUrl);
Request->SetVerb("POST");
Request->SetHeader("Content-Type", TEXT("application/json"));
Request->SetTimeout(5);
Request->OnProcessRequestComplete().BindUObject(this, &AShooterGameMode::OnReadyRequestResponse);
Request->ProcessRequest();
```

This will mark the payload as `Ready` in IMS zeuz so that it can be reserved.

**Note:** The Payload Local API starts up alongside your game server, therefore you should wrap the code above in some retry logic in case the game server is ready before the Payload Local API.

While this solution works and is simple enough for the Ready API there are other APIs you will likely want to call, and therefore this approach has some disadvantages:
- Lots of manual boilerplate required for each API call
- Does not include any retry logic
- Requires manual parsing of JSON response
- Future upgrades to the API become harder to adopt

#### OpenAPI Approach *(recommended)*
The better solution is to abstract all the Payload API setup and logic into a separate module that can easily be updated. You could do this manually, however instead we recommend using the [OpenAPI](https://github.com/OpenAPITools/openapi-generator) generator for [UE4](https://github.com/OpenAPITools/openapi-generator/blob/master/docs/generators/cpp-ue4.md). This will generate an `IMSZeuzAPI` module from the [Payload Local API OpenAPI specification](https://docs.ims.improbable.io/openapi/ims-zeuz/payload-local-api) with an interface to access Payload API calls, with retry logic.

##### How to generate an API Module from an OpenAPI specification 
>**Associated commits:** [Payload Local API Client Generation](https://github.com/improbable-eng/ims-unreal-demo/commit/b501e942e9ff7eca3ad4d4ef49b2430f29a10eba), [Temporary fix to OpenAPI bug involving body being set for GET requests](https://github.com/improbable-eng/ims-unreal-demo/commit/98e8f7d7ae7945fe03cf4ad3bef5a4fa1136a8b5)

We have provided an executable you can use to generate an API Module from the latest API specification [here](https://github.com/improbable-eng/ims-unreal-demo/tree/main/Docs/OpenAPI/payload-local-api-generation). You will need to have NodeJS and a java runtime installed, as well as the [openapi-generator-cli](https://www.npmjs.com/package/@openapitools/openapi-generator-cli) module.

**Note:** If you use the OpenAPI generated code, you should understand how the retry policy works. For example, it will only retry bad responses if they are connection errors (not protocol errors or unknown) otherwise request may be sent twice (even though that would not matter for this particular request).

**Note:** Currently, making `GET` requests using OpenAPI does not work because a content body is being set. In the meantime, there is a [temporary fix](https://github.com/improbable-eng/ims-unreal-demo/commit/98e8f7d7ae7945fe03cf4ad3bef5a4fa1136a8b5) for this.

##### How to use the OpenAPI Module
>**Associated commit:** [Set Payload to Ready upon Initialization](https://github.com/improbable-eng/ims-unreal-demo/commit/6343c6e8e1ea883bd1e1ca770493b9b887f8fc56)

Putting everything together, in the ShooterGame project we want to mark the payload as `Ready` in `AShooterGameMode::HandleMatchIsWaitingToStart` once initialization is finished. If all retries fail, the server should shutdown otherwise the payload will be stuck in the `Starting` state indefinitely (filling up the buffer with unreservable payloads).

```cpp
AShooterGameMode::AShooterGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	...
​
	RetryLimitCount = 10;
	RetryTimeoutRelativeSeconds = 5;
​
	if (IsRunningOnIMS())
	{
		SetupPayloadLocalAPI();
	}
}
​
...
​
bool AShooterGameMode::IsRunningOnZeuz()
{
	return FParse::Param(FCommandLine::Get(), TEXT("zeuz"));
}
​
void AShooterGameMode::SetupPayloadLocalAPI()
{
	RetryPolicy = IMSZeuzAPI::HttpRetryParams(RetryLimitCount, RetryTimeoutRelativeSeconds);
	PayloadLocalAPI = MakeShared<IMSZeuzAPI::OpenAPIPayloadLocalApi>();
	OnSetPayloadToReadyDelegate = IMSZeuzAPI::OpenAPIPayloadLocalApi::FReadyV0Delegate::CreateUObject(this, &AShooterGameMode::OnSetPayloadToReadyComplete);
​
	FString payloadApiDomain = FPlatformMisc::GetEnvironmentVariable(*FString("ORCHESTRATION_PAYLOAD_API"));
​
	if (!payloadApiDomain.IsEmpty())
	{
		FString payloadApiUrl = "http://" + payloadApiDomain;
		PayloadLocalAPI->SetURL(payloadApiUrl);
​
		UE_LOG(LogGameMode, Display, TEXT("Payload Local API URL was set to '%s'"), *payloadApiUrl);
	}
	else
	{
		UE_LOG(LogGameMode, Error, TEXT("No environment variable with key 'ORCHESTRATION_PAYLOAD_API' was found."));
	}
}
​
...
​
void AShooterGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
​
	...
​
	if (IsRunningOnZeuz() && PayloadLocalAPI != NULL)
	{
		TrySetPayloadToReady();
	}
}
​
void AShooterGameMode::TrySetPayloadToReady()
{
	IMSZeuzAPI::OpenAPIPayloadLocalApi::ReadyV0Request Request;
	Request.SetShouldRetry(RetryPolicy);
​
	UE_LOG(LogGameMode, Display, TEXT("Attempting to set payload to Ready state..."));
	PayloadLocalAPI->ReadyV0(Request, OnSetPayloadToReadyDelegate);
​
	FHttpModule::Get().GetHttpManager().Flush(false);
}
​
void AShooterGameMode::OnSetPayloadToReadyComplete(const IMSZeuzAPI::OpenAPIPayloadLocalApi::ReadyV0Response& Response)
{
	if (Response.IsSuccessful())
	{
		UE_LOG(LogGameMode, Display, TEXT("Successfully set Payload to Ready state."));
	}
	else
	{
		UE_LOG(LogGameMode, Display, TEXT("Failed to set Payload to Ready state."));
		FGenericPlatformMisc::RequestExit(false); // Shutdown server
	}
}
```
​
**Note:** Instead of relying on the `ORCHESTRATION_PAYLOAD_API` environment variable, we use a flag `-zeuz` to specify that we are running on IMS. We then set the flag in the allocation as an argument. This allows an easy way to turn off IMS specific functionality when testing locally.

### 2. Publish image to IMS Image Manager
You can now package your dedicated game server for **Linux** and publish it on IMS Image Manager. This will produce a Docker image of your server that can be run on IMS zeuz. Refer to the [documentation](https://docs.ims.improbable.io/docs/ims-cli/installation) for how to do this. In the IMS CLI, the command should look like this:

```
ims image publish --project-id your-project-id --name "your-image-name" --description "your-image-description" --version "0.0.1" --directory LinuxServer
```

### 3. Create allocation
Now that your image is available in the [IMS Image Manager Console](https://console.ims.improbable.io/image-manager), the next step is to create an allocation which defines how your payloads will run and scale. Refer to the [allocation documentation](https://docs.ims.improbable.io/docs/ims-zeuz/allocation) for more details. You will want to specify the image you have just published and the command to run your game server along with any arguments (e.g. the map, any flags).

Additionally, it is important to understand the [port policies](https://docs.ims.improbable.io/docs/ims-zeuz/guides/connect-to-game). As the default port in Unreal Engine is `7777`, you should specify that as your port number. Alternatively, you can choose a different port number, but you will need to tell Unreal by specifying a port flag (`-PORT=XXXX`) as an argument to your run command.

At this point, your allocation should be running and payloads running your game image will have been created. You should now see that your payloads are set to the `Ready` state after initialization.

### 4. Reserve and join a game
For the moment, the only way for a client to join your game server is to manually connect to the session address of your `Ready` payload. Via the in-game client command line you can type `open 12.345.67.890:1234`.

### 5. Next steps
Whilst this is a functional game hosted on IMS zeuz, there are a few problems which are addressed next in this project:
1. Matches can start for unreserved payloads as the server does not wait to start the game.
    - See [Server Waiting](#server-waiting)
2. The server keeps restarting matches.
    - See [Server Shutdown](#server-shutdown)

## Server Waiting & Shutdown
> **Associated commit:** [Server waiting & shutdown](https://github.com/improbable-eng/ims-unreal-demo/commit/e9b416a703be11fcc8cba33202fbbe9a6e79f765)

### 1. Server Waiting
When the payload starts, matches can start for unreserved/not ready payloads as the server does not wait to start a match. The ShooterGame server moves through three phases (pre-match, in-match, post-match) after specified intervals of time. This can cause problems if a payload spends a long amount of time as unreserved before players begin to connect to it as the server may not be in its pre-match phase when a player connects.

To overcome this, the timer of the game (see `AShooterGameMode::DefaultTimer`) shouldn't count down when the match is waiting to start and there are no connected players. After the first player connects, the timer will count down to the start of the game.

```cpp
void AShooterGameMode::DefaultTimer()
{
    ...
    AShooterGameState* const MyGameState = Cast<AShooterGameState>(GameState);
    if (MyGameState && MyGameState->RemainingTime > 0 && !MyGameState->bTimerPaused)
    {
        // if we are waiting for match to start (MatchState::WaitingToStart) and there are no connected
        // players, do not decrement the counter
        if (GetMatchState() == MatchState::WaitingToStart && GetNumPlayers() == 0)
        {
            return;
        }
        MyGameState->RemainingTime--;
        
        ...
    }
}
```

### 2. Server Shutdown
The default behaviour in the ShooterGame project is to automatically restart a match at the end of a match. However, to support automatic payload release, your game server executable should instead terminate at the end of the match. To gracefully achieve this, the client should be sent back to the main menu before the server exits.

#### Send client back to the main menu
Instead of the game server calling `AShooterGameMode::RestartGame` when it is transitioning out of its post-match state, we instead instruct clients to return to the main menu, using the RPC `ClientReturnToMainMenuWithTextReason`. Note that the deprecated `AShooterPlayerController::ClientReturnToMainMenu_Implementation` was updated to `AShooterPlayerController::ClientReturnToMainMenuWithTextReason_Implementation` in [ShooterPlayerController.cpp](https://github.com/improbable-eng/ims-unreal-demo/tree/main/Source/ShooterGame/Private/Player/ShooterPlayerController.cpp) and the Scoreboard UI was updated to reflect the game flow change.

```cpp
void AShooterGameMode::DefaultTimer()
{
    ...
    if (GetMatchState() == MatchState::WaitingPostMatch)
    {
        ExitPlayersToMainMenu();
    }
    ...
}

void AShooterGameMode::ExitPlayersToMainMenu()
{
    // send the players back to the main menu
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        (*It)->ClientReturnToMainMenuWithTextReason(NSLOCTEXT("GameMessages", "MatchEnded", "The match has ended."));
        AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(*It);
        ShooterPlayerController->HandleReturnToMainMenu();
    }
}
```

#### Exit the process
Once the game server has instructed the clients to return to the main menu, we then wait for all of the players to disconnect. This waiting also occurs in the `DefaultTimer` method, as once we instruct players to return to the main menu (and therefore disconnect), we do not transition the game state, staying in post-match. Subsequent calls to `DefaultTimer` then enter the case in the snippet below, where the number of connected players is checked and the game server is exited if there are no connected players.

```cpp
void AShooterGameMode::DefaultTimer()
{
    ...
    if (MyGameState && MyGameState->RemainingTime > 0 && !MyGameState->bTimerPaused)
    {
        ...
    }
    // if the match is over and all clients have been disconnected, exit the server
    else if (GetMatchState() == MatchState::WaitingPostMatch && GetNumPlayers() == 0)
    {
        FGenericPlatformMisc::RequestExit(false);
    }
}
```