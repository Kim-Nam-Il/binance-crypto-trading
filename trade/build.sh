#!/bin/bash

echo "바이낸스 거래 프로그램 빌드 시작..."

# 빌드 디렉토리 생성
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# CMake 설정 및 빌드
cmake .. && make

if [ $? -eq 0 ]; then
    echo "빌드 성공!"
    echo "실행하려면: ./build/binance_trader"
else
    echo "빌드 실패. 의존성을 확인하세요."
    echo "macOS: brew install curl openssl pkg-config cmake"
    echo "Ubuntu: sudo apt-get install libcurl4-openssl-dev libssl-dev pkg-config cmake build-essential"
fi 