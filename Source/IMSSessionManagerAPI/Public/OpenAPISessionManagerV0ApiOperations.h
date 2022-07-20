/**
 * IMS Session Manager
 * No description provided (generated by Openapi Generator https://github.com/openapitools/openapi-generator)
 *
 * OpenAPI spec version: 0.1
 * 
 *
 * NOTE: This class is auto generated by OpenAPI Generator
 * https://github.com/OpenAPITools/openapi-generator
 * Do not edit the class manually.
 */

#pragma once

#include "OpenAPIBaseModel.h"
#include "OpenAPISessionManagerV0Api.h"

#include "OpenAPIRpcStatus.h"
#include "OpenAPIV0CreateSessionRequestBody.h"
#include "OpenAPIV0CreateSessionResponse.h"
#include "OpenAPIV0ErrorResponse.h"
#include "OpenAPIV0ListSessionsResponse.h"

namespace IMSSessionManagerAPI
{

/* CreateSession
 *
 * creates and returns a new session from an allocation with a matching &#x60;session_type&#x60; selector for the provided &#x60;project_id&#x60;
*/
class IMSSESSIONMANAGERAPI_API OpenAPISessionManagerV0Api::CreateSessionV0Request : public Request
{
public:
    virtual ~CreateSessionV0Request() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	/* project_id of the project to use. */
	FString ProjectId;
	/* session_type is the allocation selector. */
	FString SessionType;
	/* body contains the session config to apply to the session. */
	OpenAPIV0CreateSessionRequestBody Body;
};

class IMSSESSIONMANAGERAPI_API OpenAPISessionManagerV0Api::CreateSessionV0Response : public Response
{
public:
    virtual ~CreateSessionV0Response() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    OpenAPIV0CreateSessionResponse Content;
};

/* ListSessions
 *
 * returns a list of all sessions from allocation(s) with a matching &#x60;session_type&#x60; selector for the provided &#x60;project_id&#x60;
*/
class IMSSESSIONMANAGERAPI_API OpenAPISessionManagerV0Api::ListSessionsV0Request : public Request
{
public:
    virtual ~ListSessionsV0Request() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	/* project_id of the project to use. */
	FString ProjectId;
	/* session_type is the allocation selector. */
	FString SessionType;
};

class IMSSESSIONMANAGERAPI_API OpenAPISessionManagerV0Api::ListSessionsV0Response : public Response
{
public:
    virtual ~ListSessionsV0Response() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    OpenAPIV0ListSessionsResponse Content;
};

}
