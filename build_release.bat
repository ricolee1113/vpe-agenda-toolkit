@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

set QT_DIR=C:\Qt\6.11.0\msvc2022_64
set CMAKE_EXE=C:\Qt\Tools\CMake_64\bin\cmake.exe
set SRC_DIR=C:\Users\Spade-10\Documents\Qt Project\VPEToolkit
set BUILD_DIR=C:\Users\Spade-10\Documents\Qt Project\VPEToolkit\build\Desktop_Qt_6_11_0_MSVC2022_64bit-Release
set DEPLOY_DIR=C:\Users\Spade-10\Documents\Qt Project\VPEToolkit\deploy

echo [1/4] Configuring Release build...
"%CMAKE_EXE%" -B "%BUILD_DIR%" -S "%SRC_DIR%" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%QT_DIR%"
if errorlevel 1 goto :error

echo [2/4] Building...
"%CMAKE_EXE%" --build "%BUILD_DIR%" --config Release
if errorlevel 1 goto :error

echo [3/4] Running windeployqt...
if not exist "%DEPLOY_DIR%" mkdir "%DEPLOY_DIR%"
copy /Y "%BUILD_DIR%\Release\VPEToolkit.exe" "%DEPLOY_DIR%\VPEToolkit.exe"
"%QT_DIR%\bin\windeployqt.exe" --release --no-translations "%DEPLOY_DIR%\VPEToolkit.exe"
if errorlevel 1 goto :error

echo [4/4] Copying scripts and templates...
if not exist "%DEPLOY_DIR%\scripts"   mkdir "%DEPLOY_DIR%\scripts"
if not exist "%DEPLOY_DIR%\templates" mkdir "%DEPLOY_DIR%\templates"
xcopy /Y /E "%SRC_DIR%\scripts\*"   "%DEPLOY_DIR%\scripts\"
xcopy /Y /E "%SRC_DIR%\templates\*" "%DEPLOY_DIR%\templates\"

echo.
echo === Deploy complete: %DEPLOY_DIR% ===
goto :end

:error
echo.
echo === BUILD FAILED ===
exit /b 1

:end
