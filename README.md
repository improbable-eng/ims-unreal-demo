# IMS Unreal Demo Game
## Overview
This demo game was adapted from the Unreal Engine [ShooterGame](https://docs.unrealengine.com/4.27/en-US/Resources/SampleGames/ShooterGame/) and is built in UE 4.27. The goal is to demonstrate:
1. How to support [IMS zeuz](https://docs.ims.improbable.io/docs/ims-zeuz/getting-started) orchestration in your game.
2. How to integrate [IMS Session Manager](https://docs.ims.improbable.io/docs/ims-session-manager/getting-started) functionality into your game.
2. How to integrate [IMS Matchmaking](https://docs.ims.improbable.io/docs/matchmaker/getting-started) functionality into your game. (*coming soon*)


## Table of Contents

**[1 - Support IMS Zeuz orchestration in your game](/Docs/zeuz-integration.md)**
- [Hosting your game on IMS zeuz](/Docs/zeuz-integration.md#hosting-your-game-on-ims-zeuz)
	1. [Set Payload to Ready upon Initialization](/Docs/zeuz-integration.md#1-set-payload-to-ready-upon-initialization)
	2. [Publish image to IMS Image Manager](/Docs/zeuz-integration.md#2-publish-image-to-ims-image-manager)
	3. [Create allocation](/Docs/zeuz-integration.md#3-create-allocation)
	4. [Reserve and join a game](/Docs/zeuz-integration.md#4-reserve-and-join-a-game)
	5. [Next steps](/Docs/zeuz-integration.md#5-next-steps)
- [Server Waiting & Shutdown](/Docs/zeuz-integration.md#server-waiting--shutdown)
	1. [Server Waiting](/Docs/zeuz-integration.md#1-server-waiting)
	2. [Server Shutdown](/Docs/zeuz-integration.md#2-server-shutdown)


**[2 - Integrate IMS Session Manager functionality into your game](/Docs/session-manager-integration.md)**
- [Before Getting Started](/Docs/session-manager-integration.md#before-getting-started)
- [Connecting players to IMS Session Manager (Client-Side)](/Docs/session-manager-integration.md#connecting-players-with-ims-session-manager)
    1. [Authenticate players with PlayFab](/Docs/session-manager-integration.md#1-authenticate-players-with-playfab)
    2. [Generate Session Manager API Client with OpenAPI](/Docs/session-manager-integration.md#2-generate-session-manager-api-client-with-openapi)
    3. [Allow players to create sessions](/Docs/session-manager-integration.md#3-allow-players-to-create-sessions)
    4. [Allow players to browse sessions](/Docs/session-manager-integration.md#4-allow-players-to-browse-sessions)
    5. [Allow players to join sessions](/Docs/session-manager-integration.md#5-allow-players-to-join-sessions)
    6. [Set Project Id and Session Type from the CLI](/Docs/session-manager-integration.md#6-set-project-id-and-session-type-from-the-cli)
    7. [Next steps](/Docs/session-manager-integration.md#7-next-steps)
- [Configuring your game server to work with IMS Session Manager (Server-Side)](/Docs/session-manager-integration.md#configuring-your-game-server-to-work-with-ims-session-manager)
    1. [Check for payload status updates](/Docs/session-manager-integration.md#1-check-for-payload-status-updates)
    2. [Shutdown server if no players join a reserved session](/Docs/session-manager-integration.md#2-shutdown-server-if-no-players-join-a-reserved-session)
    3. [Retrieve session config and apply to the game server](/Docs/session-manager-integration.md#3-retrieve-session-config-and-apply-to-the-game-server)
    4. [Set session status](/Docs/session-manager-integration.md#4-set-session-status)
- [Conclusion](/Docs/session-manager-integration.md#conclusion)

## Before Getting Started
Before getting started, please read through the IMS [documentation](https://docs.ims.improbable.io/). Follow the tutorial to [run your first game server](https://docs.ims.improbable.io/docs/ims-zeuz/guides/my-first-payload) to familiarize yourself with IMS. Each section addresses a different behaviour missing from the unmodified ShooterGame project. You should use this project as an example of what changes you need to make to your game to support IMS services.

Lastly, if you are following along, making similar changes to the ShooterGame project, then you should first ensure that you are able to package the unmodified game as a dedicated server and connect with a client. There is an [Unreal Engine tutorial](https://docs.unrealengine.com/4.27/en-US/InteractiveExperiences/Networking/HowTo/DedicatedServers/) for this. To run the dedicated server, you can run the command `ShooterServer.exe /Game/Maps/Sanctuary -log`, which by default will run your game on port `7777`. Similarly, the client can be run with `ShooterClient.exe -log`.

In order to start, you must already have:
- An IMS project
- An account linked to IMS, 
- The [IMS CLI](https://docs.ims.improbable.io/docs/ims-cli/installation) downloaded
- IMS zeuz cluster.

If you do not have any of these please reach out on your Improbable Slack channel.
