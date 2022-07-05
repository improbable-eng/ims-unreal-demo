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

#include "CoreMinimal.h"
#include "OpenAPIBaseModel.h"

namespace IMSZeuzAPI
{

class IMSZEUZAPI_API OpenAPISessionManagerLocalApi
{
public:
	OpenAPISessionManagerLocalApi();
	~OpenAPISessionManagerLocalApi();

	/* Sets the URL Endpoint.
	* Note: several fallback endpoints can be configured in request retry policies, see Request::SetShouldRetry */
	void SetURL(const FString& Url);

	/* Adds global header params to all requests */
	void AddHeaderParam(const FString& Key, const FString& Value);
	void ClearHeaderParams();

	/* Sets the retry manager to the user-defined retry manager. User must manage the lifetime of the retry manager.
	* If no retry manager is specified and a request needs retries, a default retry manager will be used.
	* See also: Request::SetShouldRetry */
	void SetHttpRetryManager(FHttpRetrySystem::FManager& RetryManager);
	FHttpRetrySystem::FManager& GetHttpRetryManager();

	class ApiV0SessionManagerStatusGetRequest;
	class ApiV0SessionManagerStatusGetResponse;
	class ApiV0SessionManagerStatusPostRequest;
	class ApiV0SessionManagerStatusPostResponse;
	class GetSessionConfigV0Request;
	class GetSessionConfigV0Response;
	
    DECLARE_DELEGATE_OneParam(FApiV0SessionManagerStatusGetDelegate, const ApiV0SessionManagerStatusGetResponse&);
    DECLARE_DELEGATE_OneParam(FApiV0SessionManagerStatusPostDelegate, const ApiV0SessionManagerStatusPostResponse&);
    DECLARE_DELEGATE_OneParam(FGetSessionConfigV0Delegate, const GetSessionConfigV0Response&);
    
    FHttpRequestPtr ApiV0SessionManagerStatusGet(const ApiV0SessionManagerStatusGetRequest& Request, const FApiV0SessionManagerStatusGetDelegate& Delegate = FApiV0SessionManagerStatusGetDelegate()) const;
    FHttpRequestPtr ApiV0SessionManagerStatusPost(const ApiV0SessionManagerStatusPostRequest& Request, const FApiV0SessionManagerStatusPostDelegate& Delegate = FApiV0SessionManagerStatusPostDelegate()) const;
    FHttpRequestPtr GetSessionConfigV0(const GetSessionConfigV0Request& Request, const FGetSessionConfigV0Delegate& Delegate = FGetSessionConfigV0Delegate()) const;
    
private:
    void OnApiV0SessionManagerStatusGetResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FApiV0SessionManagerStatusGetDelegate Delegate) const;
    void OnApiV0SessionManagerStatusPostResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FApiV0SessionManagerStatusPostDelegate Delegate) const;
    void OnGetSessionConfigV0Response(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FGetSessionConfigV0Delegate Delegate) const;
    
	FHttpRequestRef CreateHttpRequest(const Request& Request) const;
	bool IsValid() const;
	void HandleResponse(FHttpResponsePtr HttpResponse, bool bSucceeded, Response& InOutResponse) const;

	FString Url;
	TMap<FString,FString> AdditionalHeaderParams;
	mutable FHttpRetrySystem::FManager* RetryManager = nullptr;
	mutable TUniquePtr<HttpRetryManager> DefaultRetryManager;
};

}