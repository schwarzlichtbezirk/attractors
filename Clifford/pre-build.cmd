rem %1 target dir, %2 project dir

if not exist "%1clifford.txt" copy "%2clifford.txt" %1
if not exist "%1clifford.lua" copy "%2clifford.lua" %1

rem http://www.rsdn.ru/article/winshell/batanyca.xml