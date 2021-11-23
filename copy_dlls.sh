BUILD_MODE=$1
MODULE=$2

cp build/*/$BUILD_MODE/*.dll ./build/$MODULE/test/$BUILD_MODE/

cp "C:/Program Files (x86)/RaychelLogger/lib/RaychelLogger/RaychelLogger.dll" ./build/$MODULE/test/$BUILD_MODE