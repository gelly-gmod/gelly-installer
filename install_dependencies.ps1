# Installs all the ridiculous dependencies (libcurl, zlib) which aren't trivial to install on Windows.
$CurlUrl = "https://curl.se/windows/dl-8.11.1_2/curl-8.11.1_2-win64-mingw.zip"
$ZlibInstallUrl = "https://raw.githubusercontent.com/gelly-gmod/zlib.install/refs/heads/master/install.bat"

Write-Output "[1/4] Setting up environment..."
mkdir -Force "external"
Set-Location "external"

Write-Output "[2/4] Installing libcurl..."
Invoke-WebRequest -Uri $CurlUrl -OutFile "curl.zip"
Expand-Archive -Force -Path "curl.zip" -DestinationPath "curl"
Remove-Item "curl.zip"

Write-Output "[3/4] Installing and building zlib..."
Set-Location ".."
Invoke-WebRequest -Uri $ZlibInstallUrl -OutFile install_zlib.bat
.\install_zlib.bat
Remove-Item install_zlib.bat

Write-Output "[4/4] Done!"
