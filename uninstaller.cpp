#define UNICODE
#define _UNICODE

#include <iostream>
#include <windows.h>
#include <shellapi.h>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    int result = MessageBox(NULL, L"Вы действительно хотите удалить Lab2App?", L"Инфо", MB_YESNO);

    if (result == IDYES) {
        DeleteFile(L"C:\\Lab2App\\app.exe");
        MessageBox(NULL, L"Программа успешно удалена!", L"Успех", MB_OK);
        ShellExecute(NULL, L"open", L"cmd.exe", 
             L"/c timeout /t 1 > nul & rmdir /s /q C:\\Lab2App", 
             NULL, SW_HIDE);
        return 0;
    } 
    return 0;
}