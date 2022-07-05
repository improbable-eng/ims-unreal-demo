/**
 * Payload Local API
 * The Payload Local API available in your running payloads. It provides an interface between your game server and the IMS zeuz orchestration service.  # Warning The Payload Local API is starting up at the same time as your game server, and may not be initially available. Use a retry mechanism to ensure the request is successful.  # OpenAPI Best practices The Payload local API is using the OpenAPI standard, it is advised to use an OpenAPI client generator for the language your game server uses. See a [list of OpenAPI client generators](https://openapi-generator.tech/docs/generators/).  # Address The Payload Local API address can be obtained using an environment variable: `http://${ORCHESTRATION_PAYLOAD_API}`.  # Authentication As the Payload Local API is only accessible from within the payload, it does not require authentication. 
 *
 * OpenAPI spec version: 0.0.1
 * 
 *
 * NOTE: This class is auto generated by OpenAPI Generator
 * https://github.com/OpenAPITools/openapi-generator
 * Do not edit the class manually.
 */

#pragma once

#include "OpenAPIBaseModel.h"

namespace IMSZeuzAPI
{

/*
 * OpenAPIPayloadStatusStateV0
 *
 * Payload state.  - Unknown: The payload is in an unknown state.  - Preparing: The payload is downloading its container image.  - Creating: The payload is being scheduled and has not yet started.  - Starting: The payload has been scheduled and is starting. Waiting for the payload to mark itself as ready.  - Ready: The payload has called the &#x60;ready&#x60; endpoint, making it available to the system to be reserved for a game session. While ready, the system is free to remove the payload at any time, and players shouldn&#39;t be connected to this payload.  - Reserved: The payload has been reserved for a game session, players can be connected to it. The system will not be able to remove the payload until it shuts down or gets unreserved.  - Shutdown: The payload is stopping and is being removed.  - Error: Something has gone wrong with the payload.  - Unhealthy: The payload is failing to report itself as healthy.
 */
class IMSZEUZAPI_API OpenAPIPayloadStatusStateV0 : public Model
{
public:
    virtual ~OpenAPIPayloadStatusStateV0() {}
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;
	void WriteJson(JsonWriter& Writer) const final;

	enum class Values
	{
		Unknown,
		Creating,
		Starting,
		Ready,
		Reserved,
		Shutdown,
		Error,
		Unhealthy,
  	};

	Values Value;

	static FString EnumToString(const Values& EnumValue);
	static bool EnumFromString(const FString& EnumAsString, Values& EnumValue);
};

}