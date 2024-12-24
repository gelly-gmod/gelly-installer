# Configures and builds the project in the CI environment with all external dependencies.

Write-Output "[1/3] Setting up environment..."
$env:CURL_LIBRARY = "$( Get-Location )\external\curl\curl-8.11.1_2-win64-mingw\lib\libcurl.dll.a"
$env:CURL_INCLUDE_DIRS = "$( Get-Location )\external\curl\curl-8.11.1_2-win64-mingw\include"
$env:ZLIB_LIBRARY = "$( Get-Location )\external\zlib\lib\zlibstatic.lib"
$env:ZLIB_INCLUDE_DIR = "$( Get-Location )\external\zlib\include"

Write-Output "[2/3] Configuring project..."
cmake -DCMAKE_POLICY_DEFAULT_CMP0077=NEW -DCMAKE_PROGRAM_PATH="$( Get-Location )" --fresh --preset ci-release -S . -B build/ci-release

Write-Output "[3/3] Building project..."
cmake --build build/ci-release -t gelly_installer