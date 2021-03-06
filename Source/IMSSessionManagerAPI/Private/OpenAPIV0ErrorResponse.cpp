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

#include "OpenAPIV0ErrorResponse.h"

#include "IMSSessionManagerAPIModule.h"
#include "OpenAPIHelpers.h"

#include "Templates/SharedPointer.h"

namespace IMSSessionManagerAPI
{

void OpenAPIV0ErrorResponse::WriteJson(JsonWriter& Writer) const
{
	Writer->WriteObjectStart();
	if (Message.IsSet())
	{
		Writer->WriteIdentifierPrefix(TEXT("message")); WriteJsonValue(Writer, Message.GetValue());
	}
	if (RequestId.IsSet())
	{
		Writer->WriteIdentifierPrefix(TEXT("request_id")); WriteJsonValue(Writer, RequestId.GetValue());
	}
	Writer->WriteObjectEnd();
}

bool OpenAPIV0ErrorResponse::FromJson(const TSharedPtr<FJsonValue>& JsonValue)
{
	const TSharedPtr<FJsonObject>* Object;
	if (!JsonValue->TryGetObject(Object))
		return false;

	bool ParseSuccess = true;

	ParseSuccess &= TryGetJsonValue(*Object, TEXT("message"), Message);
	ParseSuccess &= TryGetJsonValue(*Object, TEXT("request_id"), RequestId);

	return ParseSuccess;
}

}
