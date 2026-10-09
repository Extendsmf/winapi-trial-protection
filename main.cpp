#define UNICODE
#define _UNICODE

#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>


bool exists = false;
int maxRuns = 5;
int countRuns;

std::string ToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    // Узнаем размер необходимого буфера в байтах
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    // Создаем строку нужной длины
    std::string str(sizeNeeded, 0);
    // Выполняем саму конвертацию
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], sizeNeeded, NULL, NULL);
    return str;
}

int CheckAndIncrementRuns(int maxRuns){
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

    if (result == ERROR_SUCCESS) {
        // Ключ успешно открыт или создан! Теперь можно читать и писать.
       LONG queryRes = RegQueryValueEx(
            hKey,
            L"LaunchCount",
            NULL,
            NULL,
            (LPBYTE)&count,
            &dataSize
        );
        if (count >= maxRuns) {
            RegCloseKey(hKey); 
            return -1;
        } else {
            count++;
            RegSetValueEx(
                hKey,                   // Дескриптор ключа
                L"LaunchCount",         // Имя параметра
                0,                      // Зарезервировано 
                REG_DWORD,              // Тип данных (число DWORD)
                (const BYTE*)&count,    // Адрес нашего числа
                sizeof(DWORD)           // Размер числа в байтах 
            );
            RegCloseKey(hKey); 
            return count;

        }
  
    }
    
    return -1;
}

HWND hEditFio; // переменная, где будет храниться наше поле ввода
#define ID_BTN_SAVE 1 // уникальный номер для кнопки "Сохранить"

// 1. Оконная процедура - сюда Windows присылает все события
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        // Когда пользователь нажал крестик закрытия окна:
        case WM_DESTROY:
            PostQuitMessage(0); // Говорим Windows остановить цикл сообщений
            return 0;

        case WM_CREATE: {
        // 1. Надпись "Введите ФИО:"
        CreateWindowEx(0, L"STATIC", L"Введите ФИО:",
            WS_CHILD | WS_VISIBLE,
            20, 20, 150, 20, // X, Y, Ширина, Высота
            hwnd, NULL, NULL, NULL);

        // 2. Поле ввода
        hEditFio = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            20, 50, 340, 25,
            hwnd, NULL, NULL, NULL);

        // 3. Кнопка
        CreateWindowEx(0, L"BUTTON", L"Сохранить",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            20, 90, 120, 30,
            hwnd, (HMENU)ID_BTN_SAVE, NULL, NULL);
        return 0;
        }

        case WM_COMMAND: {
        if (LOWORD(wParam) == ID_BTN_SAVE) {

            wchar_t buffer[256];
            GetWindowText(hEditFio, buffer, 256);
            std::wstring inputName = buffer;
            std::string utf8Name = ToUtf8(inputName);

            if (!inputName.empty()) {
                std::string line;
                std::ifstream fileIn(L"users.txt");
                while(std::getline(fileIn, line)) {
                    if (line == utf8Name) {
                        exists = true;
                        break;
                    }
                }

                fileIn.close();
                if (exists) {
                    std::wstring ans = L"Введенные ФИО уже находятся в файле. У вас осталось запусков: " 
                    + std::to_wstring(maxRuns - countRuns) 
                     + L"/" 
                     + std::to_wstring(maxRuns);

                    MessageBox(hwnd, ans.c_str(), L"Инфо", MB_OK);
                    DestroyWindow(hwnd);
                } else {
                    std::wstring ans = L"Введенные ФИО были записаны в файл. У вас осталось запусков: " 
                    + std::to_wstring(maxRuns - countRuns) 
                     + L"/" 
                     + std::to_wstring(maxRuns);

                    std::ofstream fileOut(L"users.txt", std::ios::app);
                    fileOut << utf8Name << std::endl;
                    MessageBox(hwnd, ans.c_str(), L"Инфо", MB_OK);
                    DestroyWindow(hwnd);
                }
                
            } else {
                MessageBox(hwnd, L"Введена пустая строка", L"Инфо", MB_OK);
                DestroyWindow(hwnd);
            }
    
        }
        return 0;
        }
    }
    // Все остальные события передаем стандартному системному обработчику
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// 2. Точка входа в программу (вместо обычного main)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    countRuns = CheckAndIncrementRuns(maxRuns);


    const wchar_t CLASS_NAME[] = L"Lab2WindowClass";

    // 3. Описание класса окна
    WNDCLASS wc = {};
    wc.lpfnWndProc   = WndProc;            // Какая функция обрабатывает события
    wc.hInstance     = hInstance;          // Дескриптор нашего приложения
    wc.lpszClassName = CLASS_NAME;        // Имя класса окна
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW); // Обычная стрелка мыши

    // Регистрируем наш класс в Windows
    RegisterClass(&wc);



    // 4. Создаем само окно
    HWND hwnd = CreateWindowEx(
        0,                                  // Дополнительные стили
        CLASS_NAME,                         // Имя класса
        L"Лабораторная работа №2",          // Заголовок окна
        WS_OVERLAPPEDWINDOW,                // Стиль окна (с рамкой, кнопками свернуть/закрыть)
        CW_USEDEFAULT, CW_USEDEFAULT,       // Позиция на экране (X, Y)
        400, 250,                           // Размеры окна (Ширина, Высота)
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    if (countRuns == -1) {
        MessageBox(hwnd, L"Лимит исчерпан. Приобретите полную версию программы или деинсталлируйте её.", L"Инфо", MB_OK);
        return 0;
    }

    // Показываем окно
    ShowWindow(hwnd, nCmdShow);

    // 5. Главный цикл сообщений
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // Преобразует сигналы клавиш в символы
        DispatchMessage(&msg);  // Отправляет сообщение в наш WndProc
    }

    return 0;
}