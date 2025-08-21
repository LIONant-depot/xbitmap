rmdir /s /q xbmp_unit_test.vs2022
cmake . -G "Visual Studio 17 2022" -A x64 -B xbmp_unit_test.vs2022
pause