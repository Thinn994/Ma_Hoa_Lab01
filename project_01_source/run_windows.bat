@echo off
chcp 65001 >nul
cd /d "%~dp0"

if not exist "bin\dh.exe" (
    echo [THÔNG BÁO] Chưa có file bin\dh.exe. Đang tự động biên dịch...
    call build_windows.bat
)

echo ========================================================
echo   CHẠY KIỂM THỬ GIAO THỨC DIFFIE-HELLMAN TRÊN WINDOWS
echo ========================================================
echo.
echo Bạn muốn chạy với kích thước bao nhiêu bit?
echo 1. 64-bit  (Chạy nhanh trong 1-2 giây)
echo 2. 128-bit (Chạy khoảng 5-10 giây)
echo 3. 512-bit (Chuẩn đề bài - có thể mất vài phút)
echo.
set /p choice="Nhập lựa chọn của bạn (1/2/3 hoặc nhập trực tiếp số bit): "

if "%choice%"=="1" set BITS=64
if "%choice%"=="2" set BITS=128
if "%choice%"=="3" set BITS=512
if not "%choice%"=="1" if not "%choice%"=="2" if not "%choice%"=="3" set BITS=%choice%

echo.
echo Đang chạy với %BITS% bit...
echo.
bin\dh.exe %BITS%

echo.
pause
