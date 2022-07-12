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

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"

DECLARE_LOG_CATEGORY_EXTERN(LogIMSSessionManagerAPI, Log, All);

class IMSSESSIONMANAGERAPI_API IMSSessionManagerAPIModule : public IModuleInterface
{
public:
	void StartupModule() final;
	void ShutdownModule() final;
};