REM Begin

SET exec=ProfitMax1.0.exe
::CALL %exec% -func=0 -gname=facebook
::PAUSE
::CALL %exec% -func=1 -gname=facebook -alg=random
::CALL %exec% -func=1 -gname=facebook -alg=highdegree
::CALL %exec% -func=1 -gname=facebook -alg=infmax
CALL %exec% -func=1 -gname=facebook -ptype=0 -alg=greedy
::CALL %exec% -func=1 -gname=facebook -ptype=0 -alg=doublegreedy
::CALL %exec% -func=1 -gname=facebook -ptype=1 -alg=greedy
CALL %exec% -func=1 -gname=facebook -ptype=1 -alg=doublegreedy
PAUSE
