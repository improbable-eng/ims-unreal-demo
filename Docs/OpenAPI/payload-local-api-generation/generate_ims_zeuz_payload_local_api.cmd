@echo off

REM swagger.json can be retrieved from https://docs.ims.improbable.io/redocusaurus/ims-zeuz-payload-api.yaml (the OpenAPI specification download)
REM needs nodejs and the java runtime
REM npm install @openapitools/openapi-generator-cli -g

rd /s /q ..\IMSZeuz
npx @openapitools/openapi-generator-cli generate -i https://docs.ims.improbable.io/redocusaurus/ims-zeuz-payload-api.yaml -g cpp-ue4 -o .\IMSZeuzAPI -c .\ims_zeuz_payload_local_api_config.json --package-name IMSZeuzAPI
