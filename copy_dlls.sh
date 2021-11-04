BUILD_MODE=$1

cp build/*/$BUILD_MODE/*.dll ./build/Parser/test/$BUILD_MODE/

cp "C:/Program Files (x86)/RaychelLogger/lib/RaychelLogger/RaychelLogger.dll" ./build/Parser/test/$BUILD_MODE