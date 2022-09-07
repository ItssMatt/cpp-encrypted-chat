#pragma once

#include <conio.h>
#include <mutex>
#include <iostream>
#include <iomanip>


class CConsole {
private:
    std::mutex _mtx;
    std::string _input;
    std::string _prompt;

public:
    CConsole() { }

    static CConsole& getInstance()
    {
        static CConsole instance;
        return instance;
    }

    CConsole(const CConsole&) = delete;
    CConsole& operator = (const CConsole&) = delete;

    std::string read();

    void write(const char* text, size_t size);
    void write(const char* text) { write(text, strlen(text)); }
    void write(const std::string& text) { write(text.c_str(), text.size()); }
}; 
#define console CConsole::getInstance()

enum console_colors {
    COLOR_DARK_BLUE = 1,
    COLOR_DARK_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_DARK_RED = 4,
    COLOR_PURPLE = 5,
    COLOR_GOLD = 6,
    COLOR_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_BLUE = 11,
    COLOR_RED = 12,
    COLOR_PINK = 13,
    COLOR_YELLOW = 14,
    COLOR_WHITE = 15
};

#define SET_CONSOLE_COLOR(X) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), X)