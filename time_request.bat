@echo off

setlocal
FOR /F "eol=# tokens=*" %%i IN (%~dp0.env) DO SET %%i

set n = 5
set "counter=0"

:loop_start 
    rem Your command here
    echo (%counter%) Request... 
    curl -w "@%~dp0curl-format.txt" -o NUL -s http://localhost:%PORT%
    set /A counter=%counter%+1

    rem Check condition to continue or exit the loop
    if %counter% lss 5 (
        goto loop_start 
    ) else (
        goto loop_end
    )

:loop_end

endlocal