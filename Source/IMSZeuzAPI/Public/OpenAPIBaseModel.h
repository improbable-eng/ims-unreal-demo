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

#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonWriter.h"
#include "Dom/JsonObject.h"
#include "HttpRetrySystem.h"
#include "Containers/Ticker.h"

namespace IMSZeuzAPI
{

typedef TSharedRef<TJsonWriter<>> JsonWriter;
using namespace FHttpRetrySystem;

struct IMSZEUZAPI_API HttpRetryManager : public FManager, public FTickerObjectBase
{
	using FManager::FManager;

	bool Tick(float DeltaTime) final;
};

struct IMSZEUZAPI_API HttpRetryParams
{
	HttpRetryParams(
		const FRetryLimitCountSetting& InRetryLimitCountOverride = FRetryLimitCountSetting(),
		const FRetryTimeoutRelativeSecondsSetting& InRetryTimeoutRelativeSecondsOverride = FRetryTimeoutRelativeSecondsSetting(),
		const FRetryResponseCodes& InRetryResponseCodes = FRetryResponseCodes(),
		const FRetryVerbs& InRetryVerbs = FRetryVerbs(),
		const FRetryDomainsPtr& InRetryDomains = FRetryDomainsPtr()
	);

	FRetryLimitCountSetting              RetryLimitCountOverride;
	FRetryTimeoutRelativeSecondsSetting  RetryTimeoutRelativeSecondsOverride;
	FRetryResponseCodes					 RetryResponseCodes;
	FRetryVerbs                          RetryVerbs;
	FRetryDomainsPtr					 RetryDomains;
};

class IMSZEUZAPI_API Model
{
public:
	virtual ~Model() {}
	virtual void WriteJson(JsonWriter& Writer) const = 0;
	virtual bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) = 0;
};

class IMSZEUZAPI_API Request
{
public:
	virtual ~Request() {}
	virtual void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const = 0;
	virtual FString ComputePath() const = 0;

	/* Enables retry and optionally sets a retry policy for this request */
	void SetShouldRetry(const HttpRetryParams& Params = HttpRetryParams()) { RetryParams = Params; }
	const TOptional<HttpRetryParams>& GetRetryParams() const { return RetryParams; }

private:
	TOptional<HttpRetryParams> RetryParams;
};

class IMSZEUZAPI_API Response
{
public:
	virtual ~Response() {}
	virtual bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) = 0;

	void SetSuccessful(bool InSuccessful) { Successful = InSuccessful; }
	bool IsSuccessful() const { return Successful; }

	virtual void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode);
	EHttpResponseCodes::Type GetHttpResponseCode() const { return ResponseCode; }

	void SetResponseString(const FString& InResponseString) { ResponseString = InResponseString; }
	const FString& GetResponseString() const { return ResponseString; }

	void SetHttpResponse(const FHttpResponsePtr& InHttpResponse) { HttpResponse = InHttpResponse; }
	const FHttpResponsePtr& GetHttpResponse() const { return HttpResponse; }

private:
	bool Successful;
	EHttpResponseCodes::Type ResponseCode;
	FString ResponseString;
	FHttpResponsePtr HttpResponse;
};

}
