@echo off
setlocal enableextensions enabledelayedexpansion
  if not defined VCInstallDir (
    echo You must run this script from a Visual Studio Command Prompt.
    goto :eof
  )

  dotnet build -c Release
  cd build
  Riks.Samples.MQTTClient.dll

  :eof
endlocal