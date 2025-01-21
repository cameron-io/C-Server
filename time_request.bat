@echo off
setlocal
FOR /F "eol=# tokens=*" %%i IN (%~dp0.env) DO SET %%i
curl -w "@%~dp0curl-format.txt" -o NUL -s http://localhost:%PORT%
endlocal