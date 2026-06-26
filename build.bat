@echo off
C:\msys64\mingw64\bin\g++.exe -std=c++20 -g src\core\main.cpp src\core\GameLoop.cpp src\platform\Window.cpp -o build\game.exe -Isrc -IC:\msys64\mingw64\include -LC:\msys64\mingw64\lib -lsfml-graphics -lsfml-window -lsfml-system
if %errorlevel% equ 0 (
    echo BUILD SUCCESS
    dir build\game.exe
) else (
    echo BUILD FAILED
)
pause