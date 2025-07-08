# Ubuntu 20.04 베이스 이미지 사용
FROM ubuntu:20.04

# 비대화식 설치를 위한 환경 변수 설정
ENV DEBIAN_FRONTEND=noninteractive

# 시스템 업데이트 및 필요한 패키지 설치
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libcurl4-openssl-dev \
    libssl-dev \
    pkg-config \
    git \
    && rm -rf /var/lib/apt/lists/*

# 작업 디렉토리 설정
WORKDIR /app

# 소스 코드 복사
COPY . .

# 빌드 디렉토리 생성 및 프로젝트 빌드
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make

# 실행 파일에 실행 권한 부여
RUN chmod +x build/binance_trader

# 데이터 저장을 위한 볼륨 설정
VOLUME ["/app/data"]

# 기본 명령어 설정
CMD ["./build/binance_trader"] 