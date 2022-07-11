@echo off

REM swagger.json can be retrieved from https://docs.ims.improbable.io/redocusaurus/ims-session-manager-api.yaml (the OpenAPI specification download)
REM needs nodejs and the java runtime
REM npm install @openapitools/openapi-generator-cli -g

rd /s /q ..\IMSSessionManagerAPI
npx @openapitools/openapi-generator-cli generate -i https://docs.ims.improbable.io/redocusaurus/ims-session-manager-api.yaml -g cpp-ue4 -o ..\IMSSessionManagerAPI -c .\ims_session_manager_api_config.json --package-name IMSSessionManagerAPI
