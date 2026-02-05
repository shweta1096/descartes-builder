@echo off
setlocal

:: ========================================
:: CONFIGURATION
:: ========================================
set PYTHON_VERSION=3.10.8
set INSTALL_DIR=%~dp0\python_win
set REPO_ZIP=https://github.com/CPS-research-group/kedro-umbrella/archive/refs/heads/main.zip
set PYTHON_EMBED_ZIP=https://github.com/astral-sh/python-build-standalone/releases/download/20221106/cpython-3.10.8+20221106-x86_64-pc-windows-msvc-shared-install_only.tar.gz

echo.
echo ========================================
echo Installing Python %PYTHON_VERSION% to: %INSTALL_DIR%
echo ========================================

:: ========================================
:: Clean install directory
:: ========================================
if exist "%INSTALL_DIR%" (
    echo Removing existing directory...
    rmdir /S /Q "%INSTALL_DIR%"
)
echo Creating Embedded-Python directory at: %INSTALL_DIR%
mkdir %INSTALL_DIR%
cd /d %INSTALL_DIR%

:: ========================================
:: Download and extract embedded Python
:: ========================================
echo.
echo Downloading Embedded Python %PYTHON_VERSION% ...
curl -L -o python.tar.gz "%PYTHON_EMBED_ZIP%"

echo Extracting Python...
tar -xzf python.tar.gz
del python.tar.gz

:: ========================================
:: Download and install kedro_umbrella
:: ========================================
echo.
echo Downloading kedro-umbrella repository...
curl -L -o repo.zip %REPO_ZIP%

echo Extracting repository...
powershell -Command "Expand-Archive -Path 'repo.zip' -DestinationPath '%INSTALL_DIR%\repo'"
del repo.zip

set REPO_DIR=%INSTALL_DIR%\repo\kedro-umbrella-main
if not exist "%REPO_DIR%" (
    echo kedro-umbrella directory not found: %REPO_DIR%
    exit /b 1
)

echo.
echo Installing kedro_umbrella into embedded Python...
:: Use CPU-only PyTorch
cd /d %REPO_DIR%
"%INSTALL_DIR%\python\python.exe" -m pip install --extra-index-url https://download.pytorch.org/whl/cpu .

:: ========================================
:: Bundle the complete python directory
:: ========================================

echo.
echo Bundling embedded Python directory...

cd /d "%INSTALL_DIR%"

tar -cf python.tar .\python && rmdir /s /q python

echo Python directory zipped successfully: python.tar

echo.
echo ========================================
echo Python and kedro_umbrella setup complete!
echo ========================================

endlocal
exit /b 0
