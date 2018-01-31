
rem ./build_lib.bat <name> <src> <path>

echo "Building: %1"

pushd %3

IF NOT EXIST build (
   md build
)

pushd build

cmake .. -G"Visual Studio 14 Win64"
if %errorlevel% neq 0 exit -1

cmake --build .
if %errorlevel% neq 0 exit -1

cmake --build . --target install
if %errorlevel% neq 0 exit -1

popd

popd
