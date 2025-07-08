# 바이낸스 비트코인 최소수량 거래 프로그램 (보안 강화)

이 프로그램은 바이낸스 API를 사용하여 비트코인의 최소수량을 사고팔 수 있는 보안이 강화된 C++ 트레이딩 프로그램입니다.

## 🚀 빠른 시작

**Windows 사용자:**
```batch
docker-run.bat
```

**Linux/macOS 사용자:**
```bash
./docker-run.sh
```

프로그램이 자동으로 빌드되고 실행됩니다!

## 주요 기능

### 현물거래 기능
- 계정 잔고 조회 (BTC, USDT)
- 다양한 암호화폐 현재 가격 조회
- 최소수량으로 암호화폐 구매/판매
- 사용자 정의 수량으로 암호화폐 구매/판매
- 실시간 주문 실행 및 결과 확인

**지원 자산 (현물거래):**
- BTC (Bitcoin) 
- ETH (Ethereum)
- BNB (Binance Coin)
- ADA (Cardano)
- XRP (Ripple)
- SOL (Solana)
- DOT (Polkadot)
- DOGE (Dogecoin)
- AVAX (Avalanche)
- MATIC (Polygon)
- LTC (Litecoin)
- LINK (Chainlink)

*참고: 현재 실제 구매/판매는 BTC만 지원되며, 다른 자산은 가격 조회만 가능합니다.*

### 선물거래 기능 🚀
- **선물거래 계정 정보 조회**: 마진 잔고, 미실현 손익, 사용 가능한 잔고 확인
- **포지션 관리**: 현재 포지션 조회, 수익률 및 손익 실시간 확인
- **레버리지 설정**: 1x~125x 레버리지 자유 설정
- **롱/숏 포지션**: 시장가 주문으로 롱/숏 포지션 진입
- **포지션 종료**: 원클릭으로 포지션 완전 종료
- **지정가 주문**: 원하는 가격에 지정가 주문 실행
- **리스크 관리**: 미실현 손익 실시간 모니터링

**지원 자산 (선물거래):**
- BTC (Bitcoin)
- ETH (Ethereum)
- BNB (Binance Coin)
- ADA (Cardano)
- XRP (Ripple)
- SOL (Solana)
- DOT (Polkadot)
- DOGE (Dogecoin)
- AVAX (Avalanche)
- MATIC (Polygon)
- LTC (Litecoin)
- LINK (Chainlink)

*모든 선물거래 기능은 위 자산들에 대해 완전히 지원됩니다.*

### 🔍 선물거래 심볼 목록 조회 기능 (메뉴 18)
- **실시간 목록**: 바이낸스 API에서 실제 거래 가능한 심볼들을 실시간으로 조회
- **인기 코인 우선 표시**: BTC, ETH, BNB 등 인기 코인들을 먼저 보여줌
- **알파벳 순 정렬**: 전체 목록을 알파벳 순으로 정렬하여 찾기 쉽게 표시
- **상세 정보 제공**: 각 심볼의 최소 주문 수량, 최소 주문 금액, 거래 상태 등 표시
- **100+ 심볼 지원**: 비트코인부터 알트코인까지 100개 이상의 USDT 페어 지원

이 기능을 통해 어떤 암호화폐가 선물거래 가능한지 미리 확인할 수 있습니다!

### 보안 기능 🔒
- **암호화된 API 키 저장**: API 키와 시크릿 키를 암호화하여 안전하게 저장
- **마스터 비밀번호 시스템**: 하나의 마스터 비밀번호로 모든 키를 보호
- **24시간 세션 관리**: 한 번 인증하면 24시간 동안 유효한 세션 유지
- **자동 세션 만료**: 24시간 후 자동으로 세션 만료 및 재인증 요구
- **안전한 메모리 관리**: 민감한 데이터를 메모리에서 안전하게 제거
- **비밀번호 숨김 입력**: 비밀번호 입력 시 화면에 표시되지 않음
- **키 관리 기능**: 저장된 키 삭제, 마스터 비밀번호 변경 등

### Docker 지원 🐳
- **크로스 플랫폼**: Mac, Linux, Windows 어디서든 동일하게 작동
- **의존성 자동 해결**: 필요한 모든 라이브러리가 컨테이너에 포함
- **간편한 실행**: 스크립트 하나로 빌드부터 실행까지 자동화
- **이중 보안 모드**: 편의성과 보안 강화 중 선택 가능
  - **일반 모드**: 데이터 지속성 (호스트 저장)
  - **보안 강화 모드**: 컨테이너 격리 (내부 저장)

## 필요한 의존성

### macOS (Homebrew 사용)
```bash
brew install curl openssl pkg-config cmake
```

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install libcurl4-openssl-dev libssl-dev pkg-config cmake build-essential
```

## 컴파일 및 실행

### 방법 1: Docker 사용 (권장) 🐳

Docker를 사용하면 Mac, Linux, Windows 어느 환경에서도 동일하게 작동합니다.

1. **Docker 및 Docker Compose 설치**
   - [Docker Desktop](https://www.docker.com/products/docker-desktop/) 설치

2. **프로젝트 빌드 및 실행**

**간단한 실행 (권장):**
```bash
# 보안 강화 모드로 실행
# Linux/macOS
./docker-run.sh
# Windows
docker-run.bat
```

**수동 실행:**
```bash
# 보안 강화 모드
docker-compose -f docker-compose-secure.yml up --build
# 또는
docker build -f Dockerfile.secure -t binance-trader-secure .
docker run -it --rm --security-opt no-new-privileges:true --tmpfs /tmp:rw,noexec,nosuid,size=100m binance-trader-secure
```

3. **보안 강화 모드**
   - 암호화된 키 파일이 컨테이너 내부에서만 저장됩니다
   - 컨테이너 삭제 시 데이터도 완전히 삭제됩니다
   - 강력한 보안, 호스트에서 접근 불가능
   - 비밀번호는 화면에 표시되지 않지만 정상적으로 입력됩니다

4. **테스트 완료 환경**
   - ✅ **Windows 10/11**: Docker Desktop 사용
   - ✅ **Ubuntu 20.04**: Docker 컨테이너 내부에서 빌드 및 실행
   - ✅ **의존성 자동 설치**: libcurl, OpenSSL, CMake, build-essential
   - ✅ **코드 호환성**: C++ 17 표준 준수

### 방법 2: 직접 컴파일

1. 프로젝트 빌드
```bash
mkdir build
cd build
cmake ..
make
```

2. 프로그램 실행
```bash
./binance_trader
```

## 바이낸스 API 키 설정

1. [바이낸스](https://www.binance.com)에 로그인
2. API Management로 이동
3. 새 API 키 생성
4. **권한 설정**:
   - **현물거래만 사용**: Spot Trading 권한만 활성화
   - **선물거래 포함**: Spot Trading + Futures Trading 권한 활성화 🚀
   - **권장**: IP 제한을 설정하여 보안 강화
5. API 키와 시크릿 키를 안전하게 보관

### 선물거래 추가 설정 ⚠️
- 바이낸스 선물거래 계정을 먼저 활성화해야 합니다
- 선물거래 계정에 USDT를 입금하세요 (현물과 별도 계정)
- 기본 레버리지는 안전한 1x로 설정됩니다 (필요시 수동으로 변경 가능)
- 선물거래는 높은 리스크를 수반하므로 충분한 학습 후 사용하세요

## 사용법

### 첫 실행 시
1. 프로그램 실행
2. 새 마스터 비밀번호 설정 (2번 입력하여 확인)
3. 바이낸스 API 키와 시크릿 키 입력
4. 키가 암호화되어 안전하게 저장됨

### 이후 실행 시
1. 프로그램 실행
2. 마스터 비밀번호 입력 (24시간 동안 유효)
3. 자동으로 암호화된 키 불러오기

### 메뉴 옵션

**현물거래 기능:**
1. **계정 정보 조회**: 현재 BTC와 USDT 잔고를 확인
2. **암호화폐 현재 가격 조회**: 선택한 암호화폐의 현재 시장 가격 조회
3. **암호화폐 최소수량 구매**: 거래소에서 허용하는 최소수량으로 암호화폐 구매
4. **암호화폐 최소수량 판매**: 거래소에서 허용하는 최소수량으로 암호화폐 판매
5. **사용자 정의 수량으로 구매**: 원하는 수량으로 암호화폐 구매
6. **사용자 정의 수량으로 판매**: 원하는 수량으로 암호화폐 판매

**선물거래 기능:**
11. **선물거래 계정 정보 조회**: 마진 잔고, 미실현 손익, 사용 가능한 잔고 확인
12. **선물거래 포지션 조회**: 선택한 암호화폐의 현재 포지션 상태 및 수익률 확인
13. **레버리지 설정**: 선택한 암호화폐의 1x~125x 레버리지 설정 (기본값: 1x 안전 모드)
14. **롱 포지션 진입**: 선택한 암호화폐로 시장가 롱 포지션 진입 (가격 상승 베팅)
15. **숏 포지션 진입**: 선택한 암호화폐로 시장가 숏 포지션 진입 (가격 하락 베팅)
16. **포지션 종료**: 선택한 암호화폐의 현재 포지션을 즉시 종료하여 손익 실현
17. **선물거래 지정가 주문**: 선택한 암호화폐에 대해 원하는 가격에 지정가 주문 실행
18. **선물거래 가능한 심볼 목록 조회**: 바이낸스에서 실제 거래 가능한 모든 USDT 페어 조회 🆕

**시스템 기능:**
7. **세션 상태 확인**: 현재 세션의 유효성 및 만료 시간 확인
8. **주문 권한 테스트**: 실제 거래 없이 API 키의 거래 권한 확인 🔧
9. **저장된 키 삭제**: 암호화된 키 파일을 완전히 삭제
10. **마스터 비밀번호 변경**: 기존 비밀번호를 새 비밀번호로 변경

## 보안 주의사항

### 강화된 보안 기능
- ✅ **암호화된 저장**: API 키가 암호화되어 로컬에 저장됩니다
- ✅ **마스터 비밀번호**: 하나의 강력한 비밀번호로 모든 키를 보호합니다
- ✅ **세션 관리**: 24시간 후 자동으로 재인증이 필요합니다
- ✅ **메모리 보안**: 민감한 데이터가 메모리에서 안전하게 제거됩니다

### 추가 보안 권장사항
- **절대로** API 키와 시크릿 키를 코드에 하드코딩하지 마세요
- API 키에는 최소한의 권한만 부여하세요 (Spot Trading만)
- IP 제한을 설정하여 보안을 강화하세요
- 마스터 비밀번호는 강력하게 설정하세요 (8자 이상, 특수문자 포함)
- 테스트넷에서 먼저 테스트해보는 것을 권장합니다
- 정기적으로 마스터 비밀번호를 변경하세요

## 최소 주문 수량

- BTCUSDT 쌍의 최소 주문 수량은 일반적으로 0.00001 BTC입니다
- 프로그램이 자동으로 최소 주문 수량을 조회하여 표시합니다

## 주의사항

### 일반 주의사항
- 이 프로그램은 교육 및 테스트 목적으로 작성되었습니다
- 실제 거래 시에는 충분한 테스트와 검증을 거치세요
- 시장 주문(Market Order)을 사용하므로 슬리피지가 발생할 수 있습니다
- 거래 수수료가 적용됩니다

### 선물거래 특별 주의사항 ⚠️
- **높은 리스크**: 선물거래는 레버리지로 인해 큰 손실이 발생할 수 있습니다
- **자금 관리**: 투자할 수 있는 금액의 일부만 사용하세요
- **레버리지 주의**: 높은 레버리지는 큰 수익과 함께 큰 손실 위험을 수반합니다
- **강제 청산**: 손실이 증가하면 포지션이 강제로 청산될 수 있습니다
- **24시간 거래**: 선물시장은 24시간 변동하므로 지속적인 모니터링이 필요합니다
- **교육 필수**: 선물거래에 대한 충분한 이해 없이 사용하지 마세요
- **테스트넷 권장**: 실제 거래 전에 바이낸스 테스트넷에서 먼저 연습하세요

## 문제 해결

### 대화형 입력 문제 (비밀번호 입력) 🔑

#### 문제: 비밀번호 입력 시 반응하지 않음
```
처음 사용하시는군요! 새로운 마스터 비밀번호를 설정하세요.
새 마스터 비밀번호를 입력하세요: [여기서 입력이 안 되는 것처럼 보임]
```

**원인:** Windows PowerShell에서 Docker 컨테이너의 대화형 입력이 제대로 작동하지 않는 경우

**해결 방법:**

1. **대화형 전용 스크립트 사용 (권장):**
   ```bash
   # Windows
   docker-run-secure-interactive.bat
   
   # Linux/macOS
   ./docker-run-secure-interactive.sh
   ```

2. **직접 Docker 명령 사용:**
   ```bash
   docker run -it --rm --name binance-trader-secure \
     --security-opt no-new-privileges:true \
     --tmpfs /tmp:rw,noexec,nosuid,size=100m \
     binance-trader-secure
   ```

3. **Windows Terminal 사용:**
   - PowerShell 대신 Windows Terminal 사용
   - 더 나은 대화형 지원 제공

**⚠️ 중요사항:**
- 🔐 **비밀번호 입력 시 화면에 표시되지 않지만 입력은 됩니다**
- ⌨️ **천천히 정확하게 입력하고 Enter를 누르세요**
- 🔄 **비밀번호 확인 시 동일하게 입력해야 합니다**
- 💡 **입력 중에 커서가 움직이지 않아도 정상입니다**

### Docker 관련 문제

#### 컨테이너 이름 충돌 오류
```
docker: Error response from daemon: Conflict. The container name "/binance-trader-secure" is already in use by container "...". You have to remove (or rename) that container to be able to reuse that name.
```

**원인:** 이전에 실행된 컨테이너가 남아있어서 같은 이름으로 새 컨테이너를 만들 수 없음

**해결 방법:**

1. **기존 컨테이너 삭제 후 새로 실행 (권장):**
   ```bash
   # 기존 컨테이너 강제 삭제
   docker rm -f binance-trader-secure
   
   # 다시 실행
   ./docker-run-secure-interactive.bat
   ```

2. **기존 컨테이너가 실행 중인 경우 접속:**
   ```bash
   # 실행 중인 컨테이너에 접속
   docker exec -it binance-trader-secure /bin/bash
   
   # 또는 프로그램 직접 실행
   docker exec -it binance-trader-secure ./build/binance_trader
   ```

3. **기존 컨테이너 상태 확인:**
   ```bash
   # 컨테이너 상태 확인
   docker ps -a | findstr binance-trader-secure
   
   # 실행 중이면: STATUS가 "Up"
   # 중지됨이면: STATUS가 "Exited"
   ```

4. **중지된 컨테이너 재시작:**
   ```bash
   # 중지된 컨테이너 시작
   docker start binance-trader-secure
   
   # 시작 후 접속
   docker exec -it binance-trader-secure ./build/binance_trader
   ```

5. **완전 정리 후 새로 시작 (추천):**
   ```bash
   # 모든 관련 컨테이너 삭제
   docker rm -f binance-trader-secure
   
   # 이미지도 삭제하고 싶다면
   docker rmi binance-trader-secure
   
   # 새로 실행
   ./docker-run-secure-interactive.bat
   ```

**💡 가장 간단한 해결책:**
```bash
docker rm -f binance-trader-secure && ./docker-run-secure-interactive.bat
```

#### Docker Desktop 실행 오류
```
error during connect: Get "http://%2F%2F.%2Fpipe%2FdockerDesktopLinuxEngine/v1.48/..."
```
**해결 방법:**
1. Docker Desktop이 실행되고 있는지 확인
2. Windows 시작 메뉴에서 "Docker Desktop" 실행
3. 시스템 트레이에서 Docker 고래 아이콘 확인
4. Docker Desktop 완전 시작까지 1-2분 대기

#### Docker Compose 버전 경고
```
the attribute `version` is obsolete
```
**해결 방법:** 이미 수정되었습니다. 최신 Docker Compose에서는 version 속성이 불필요합니다.

### 컴파일 오류

#### C++ 예약어 충돌
```
error: 'signed' specified with 'bool'
```
**해결 방법:** 이미 수정되었습니다. `signed` 매개변수를 `is_signed`로 변경했습니다.

#### OpenSSL 헤더 오류
```
fatal error: openssl/pbkdf2.h: No such file or directory
```
**해결 방법:** 이미 수정되었습니다. 사용하지 않는 헤더를 제거했습니다.

### 직접 컴파일 시 문제
- OpenSSL과 libcurl이 제대로 설치되었는지 확인
- pkg-config가 라이브러리를 찾을 수 있는지 확인
- Visual Studio Build Tools (Windows) 설치 확인

### API 오류 및 거래 실패

#### 문제: 구매/판매 주문이 실행되지 않음
```
정말 구매하시겠습니까? (y/N): y
구매 주문을 실행하는 중...
주문 실패: [오류 메시지]
```

**일반적인 원인과 해결 방법:**

1. **API 키 권한 부족**
   - 바이낸스에서 API 키 설정 확인
   - **Spot Trading** 권한이 활성화되어 있는지 확인
   - **Enable Trading** 옵션이 체크되어 있는지 확인

2. **IP 제한 설정**
   - 바이낸스 API 관리에서 IP 제한 확인
   - 현재 IP 주소가 허용 목록에 있는지 확인
   - 또는 IP 제한을 해제 (보안상 권장하지 않음)

3. **잔고 부족**
   - USDT 잔고가 구매 금액보다 충분한지 확인
   - 최소 구매 금액: 약 $1-2 (현재 BTC 가격에 따라 변동)

4. **최소 주문 수량 미달**
   - 최소 주문 수량: 0.00001000 BTC
   - 현재 BTC 가격 기준 최소 주문 금액 확인

5. **네트워크 연결 문제**
   - 인터넷 연결 상태 확인
   - 방화벽이 바이낸스 API 접근을 차단하는지 확인

6. **시장 상황**
   - 거래 일시 중단 시간이 아닌지 확인
   - 바이낸스 서버 점검 시간이 아닌지 확인

**디버깅 방법:**

1. **주문 권한 테스트 사용 (메뉴 8번)**: 🔧
   ```
   === 주문 권한 테스트 ===
   실제 거래 없이 주문 권한을 테스트합니다.
   
   구매 권한 테스트 중...
   ✅ 구매 권한: 정상
   판매 권한 테스트 중...
   ✅ 판매 권한: 정상
   
   🎉 모든 주문 권한이 정상입니다! 실제 거래가 가능합니다.
   ```

2. **API 응답 확인**:
   - 프로그램 실행 시 "API 응답" 메시지를 확인
   - 구체적인 오류 코드와 메시지 확인
   - 바이낸스 API 문서에서 해당 오류 코드 의미 확인

3. **권한 상세 확인**:
   - 프로그램 시작 시 거래 권한 상태 자동 확인
   - "❌ 거래 권한: 비활성화됨" 메시지 시 바이낸스 설정 확인

**추가 확인사항:**
- API 키와 시크릿 키가 올바른지 확인
- 계정에 충분한 잔고가 있는지 확인
- 테스트넷이 아닌 실제 계정인지 확인

### 네트워크 오류
- 인터넷 연결 상태 확인
- 방화벽 설정 확인
- 바이낸스 API 서버 상태 확인

## 파일 저장 위치

### 암호화된 키 파일

**Docker 일반 모드:**
- 호스트 시스템: `./data/keys.enc`
- 컨테이너 내부: `/app/data/keys.enc`

**Docker 보안 강화 모드:**
- 컨테이너 내부에서만: `/app/secure_data/keys.enc`
- 호스트에서 접근 불가능

**직접 실행 시:**
- **Windows**: `%USERPROFILE%\.binance_trader\keys.enc`
- **Linux/macOS**: `~/.binance_trader/keys.enc`

이 파일은 마스터 비밀번호로 암호화되어 있으며, 비밀번호 없이는 복호화할 수 없습니다.

## Docker 사용 시 추가 명령어

### 컨테이너 관리

**일반 모드:**
```bash
# 백그라운드에서 실행
docker-compose up -d

# 실행 중인 컨테이너에 접속
docker-compose exec binance-trader /bin/bash

# 컨테이너 중지
docker-compose down

# 컨테이너 및 이미지 삭제
docker-compose down --rmi all
```

**보안 강화 모드:**
```bash
# 백그라운드에서 실행
docker-compose -f docker-compose-secure.yml up -d

# 실행 중인 컨테이너에 접속
docker-compose -f docker-compose-secure.yml exec binance-trader-secure /bin/bash

# 컨테이너 중지
docker-compose -f docker-compose-secure.yml down

# 컨테이너 및 이미지 삭제
docker-compose -f docker-compose-secure.yml down --rmi all
```

### 로그 확인

**일반 모드:**
```bash
# 실시간 로그 확인
docker-compose logs -f

# 특정 컨테이너 로그 확인
docker logs binance-trader
```

**보안 강화 모드:**
```bash
# 실시간 로그 확인
docker-compose -f docker-compose-secure.yml logs -f

# 특정 컨테이너 로그 확인
docker logs binance-trader-secure
```

## 빌드 성공 확인

프로그램이 성공적으로 빌드되면 다음과 같은 메시지가 표시됩니다:

```
✅ 빌드 성공!
🚀 프로그램이 시작되었습니다...

=== 바이낸스 비트코인 최소수량 거래 프로그램 (보안 강화) ===

처음 사용하시는군요! 새로운 마스터 비밀번호를 설정하세요.
```

### 해결된 주요 문제들
- ✅ **C++ 예약어 충돌**: `signed` → `is_signed`로 수정
- ✅ **OpenSSL 헤더 호환성**: 불필요한 `pbkdf2.h` 헤더 제거
- ✅ **Docker Compose 버전 경고**: 최신 형식으로 업데이트
- ✅ **크로스 플랫폼 빌드**: Ubuntu 20.04 컨테이너에서 안정적 빌드
- ✅ **대화형 입력 문제**: 보안 강화 모드에서 비밀번호 입력 개선
- ✅ **거래 실패 디버깅**: API 응답 출력 및 권한 확인 기능 추가
- ✅ **수량 포맷팅**: 8자리 소수점 정확한 포맷팅으로 주문 오류 방지
- ✅ **주문 권한 테스트**: 실제 거래 없이 API 키 권한 사전 확인 기능

## 보안 모드 비교

### 일반 모드 vs 보안 강화 모드

| 항목 | 일반 모드 | 보안 강화 모드 |
|------|----------|----------------|
| **데이터 저장** | 호스트 `./data/` | 컨테이너 내부만 |
| **데이터 지속성** | ✅ 컨테이너 삭제 후에도 유지 | ❌ 컨테이너 삭제 시 함께 삭제 |
| **호스트 접근** | ⚠️ 호스트에서 파일 접근 가능 | ✅ 호스트에서 접근 불가능 |
| **컨테이너 격리** | 🔒 기본 격리 | 🔐 강화된 격리 |
| **권한 제한** | 기본 권한 | 최소 권한 (비특권 사용자) |
| **파일시스템** | 읽기/쓰기 | 읽기 전용 (데이터 디렉토리 제외) |
| **보안 옵션** | 기본 설정 | `--security-opt`, `--cap-drop` 적용 |
| **백업 편의성** | ✅ 쉬움 | ⚠️ 컨테이너 실행 중에만 가능 |

### 보안 강화 모드 특징

**적용된 보안 기능:**
- 🔐 **비특권 사용자 실행**: root 권한 없이 실행
- 🔒 **읽기 전용 파일시스템**: 시스템 파일 변경 방지
- 🛡️ **권한 제한**: 불필요한 시스템 권한 제거
- 🔑 **컨테이너 격리**: 호스트 시스템과 완전 분리
- 🚫 **새 권한 획득 방지**: `no-new-privileges` 적용

**권장 사용 시나리오:**
- 🏢 **프로덕션 환경**: 실제 거래 시 최고 보안 필요
- 🔐 **민감한 환경**: 높은 보안 요구사항
- 🧪 **일회성 테스트**: 테스트 후 완전 삭제 필요
- 🌐 **공유 시스템**: 다른 사용자와 시스템 공유 시

## 라이센스

이 프로젝트는 교육 목적으로 제공되며, 사용자의 책임 하에 사용하세요. 