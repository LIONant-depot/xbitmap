rmdir /s /q xbmp_unit_test.vs2022
rmdir /s /q ..\dependencies
cmake ../ -G "Visual Studio 17 2022" -A x64 -B xbmp_unit_test.vs2022
pause