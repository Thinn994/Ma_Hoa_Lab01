@echo off
chcp 65001 >nul
echo ========================================================
echo   BIÊN DỊCH DỰ ÁN DIFFIE-HELLMAN BẰNG MSVC (VISUAL STUDIO)
echo ========================================================

set "VS=C:\Program Files\Microsoft Visual Studio\2022\Community"
set "CL=%VS%\VC\Tools\MSVC\14.41.34120\bin\Hostx64\x64\cl.exe"
set "MSVC_INC=%VS%\VC\Tools\MSVC\14.41.34120\include"
set "MSVC_LIB=%VS%\VC\Tools\MSVC\14.41.34120\lib\x64"
set "WIN_INC=C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0"
set "WIN_LIB=C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22621.0"

if not exist "%CL%" (
    echo [LỖI] Không tìm thấy MSVC cl.exe tại đường dẫn mặc định!
    pause
    exit /b 1
)

if not exist bin mkdir bin

echo Đang biên dịch...
"%CL%" /std:c++14 /EHsc /O2 /W0 ^
  /I"src" /I"lib\fips202" ^
  /I"%MSVC_INC%" /I"%WIN_INC%\ucrt" /I"%WIN_INC%\um" /I"%WIN_INC%\shared" ^
  src\main.cpp src\dh.cpp lib\fips202\prng_win.cpp lib\fips202\fips202.c ^
  /Fe"bin\dh.exe" ^
  /link /LIBPATH:"%MSVC_LIB%" /LIBPATH:"%WIN_LIB%\um\x64" /LIBPATH:"%WIN_LIB%\ucrt\x64" ^
  bcrypt.lib kernel32.lib >nul 2>&1

if exist bin\dh.exe (
    echo [THÀNH CÔNG] Đã tạo file thực thi: bin\dh.exe
) else (
    echo [THẤT BÀI] Có lỗi trong quá trình biên dịch.
)

pause
