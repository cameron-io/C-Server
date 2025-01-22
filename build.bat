@echo off
setlocal
FOR /F "eol=# tokens=*" %%i IN (%~dp0.env) DO SET %%i
@echo on
cmake -S. -B%BUILD_DIR%
cmake --build %BUILD_DIR% --config Release
endlocal
