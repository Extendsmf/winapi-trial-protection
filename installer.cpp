#define UNICODE
#define _UNICODE

#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>

int maxRuns = 5;

bool ExtractResource(int resourceId, const std::string& outputPath) {
    HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(resourceId), RT_RCDATA);
    if (!hRes) return false;

    HGLOBAL hMem = LoadResource(NULL, hRes);
    if (!hMem) return false;

    DWORD size = SizeofResource(NULL, hRes);
    void* pData = LockResource(hMem);
    if (!pData) return false;

    // Записываем байты на диск
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) return false;

    outFile.write(reinterpret_cast<char*>(pData), size);
    return true;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    HKEY hKey; // сюда Windows положит ключ

    LONG result = RegCreateKeyEx(
        HKEY_CURRENT_USER,          // ветка
        L"Software\\Lab2App",       // путь
        0,                          // зарезервировано
        NULL,                       // класс
        REG_OPTION_NON_VOLATILE,    // сохранять на диск, не сбрасывать при перезагрузке
        KEY_READ | KEY_WRITE,       // хотим читать и писать
        NULL,                       // безопасность
        &hKey,                      // адрес нашей переменной hKey
        NULL                        // нам не важно, новый он или старый
    );

    DWORD count = 0;
    DWORD dataSize = sizeof(DWORD);


     LONG queryRes = RegQueryValueEx(
         hKey,
        L"LaunchCount",
         NULL,
        NULL,
        (LPBYTE)&count,
         &dataSize
    );
         
    if (count >= maxRuns) {
        MessageBox(NULL, L"Лимит исчерпан. Приобретите полную версию программы.", L"Инфо", MB_OK);
        return 0;

    } else if (count > 0) {
        std::wstring ans = L"Внимание! Программа уже была установлена ранее на этом компьютере. Использовано запусков: " 
         + std::to_wstring(count) 
          + L"/" 
         + std::to_wstring(maxRuns);
        MessageBox(NULL, ans.c_str(), L"Инфо", MB_OK);
    } else {
        MessageBox(NULL, L"Добро пожаловать! Первичная установка программы. Вам доступно 5 пробных запусков.", L"Инфо", MB_OK);
    }

    CreateDirectory(L"C:\\Lab2App", NULL);
    ExtractResource(101, "C:\\Lab2App\\app.exe");
    ExtractResource(102, "C:\\Lab2App\\uninstaller.exe");
    MessageBox(NULL, L"Установка успешно завершена в C:\\Lab2App!", L"Успех", MB_OK);
    return 0;
}