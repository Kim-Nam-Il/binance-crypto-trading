@echo off
chcp 65001 > nul
echo [BINANCE] 바이낸스 비트코인 거래 프로그램
echo.

echo [INFO] 보안 강화 모드로 실행됩니다:
echo    - 저장된 키는 컨테이너 삭제 시 완전히 제거됩니다
echo    - 호스트 접근이 차단되어 더 안전합니다
echo    - 비밀번호는 화면에 표시되지 않지만 입력됩니다
echo.

set /p confirm=Continue? (y/N): 
if /i not "%confirm%"=="y" (
    echo Cancelled.
    pause
    exit /b
)

echo.
echo [BUILD] Building Docker image...
docker build -f Dockerfile.secure -t binance-trader-secure . --quiet

if %errorlevel% == 0 (
    echo [SUCCESS] Build completed!
    echo.
    echo [RUN] Starting container...
    echo [TIP] Passwords will not be displayed on screen but input is working.
    echo.
    
    docker run -it --rm ^
        --name binance-trader-secure ^
        --security-opt no-new-privileges:true ^
        --tmpfs /tmp:rw,noexec,nosuid,size=100m ^
        binance-trader-secure
) else (
    echo [ERROR] Build failed. Please check if Docker is properly installed.
    pause
    exit /b
)

echo.
echo [DONE] Program terminated
pause 