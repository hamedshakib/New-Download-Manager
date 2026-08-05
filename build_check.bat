@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Enterprise\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
cmake --build E:\Programming\C++ANDQML\New-Download-Manager\out\build\x64-Debug --target DownloadManager_lrelease
