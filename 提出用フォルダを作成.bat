@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0CreateSubmission.ps1" -Configuration Release
if errorlevel 1 (
  echo.
  echo Failed to create the submission folder.
  pause
  exit /b 1
)
echo.
echo Done. Check generated\submission\TatariBashiri.
pause
