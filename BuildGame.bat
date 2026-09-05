@echo off
cmake --preset game
if errorlevel 1 exit /b %errorlevel%
cmake --build --preset game-debug
