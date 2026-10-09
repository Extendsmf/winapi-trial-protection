g++ main.cpp -o app.exe -mwindows
g++ uninstaller.cpp -o uninstaller.exe -mwindows
windres resources.rc -O coff -o resources.res
g++ installer.cpp resources.res -o installer.exe -mwindows