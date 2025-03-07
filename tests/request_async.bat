@echo off

setlocal
FOR /F "eol=# tokens=*" %%i IN (%~dp0\..\.env) DO SET %%i

set "n=%1"
set "counter=1"

:loop_start 
    rem Your command here
    echo (%counter%) Request... 
    START /B curl -o NUL -s http://localhost:%PORT%/hello.json

    rem Check condition to continue or exit the loop
    if %counter% lss %n% (
        set /A counter=%counter%+1
        goto loop_start 
    ) else (
        goto loop_end
    )

:loop_end

endlocal