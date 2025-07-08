# 🚀 Binance Crypto Trading Suite

바이낸스 API를 활용한 고급 암호화폐 거래 프로그램 모음입니다.

## 📁 프로젝트 구조

```
Binance/
├── trade/          # 메인 거래 프로그램
│   ├── src/        # C++ 소스 코드
│   ├── include/    # 헤더 파일
│   ├── Docker 설정 파일들
│   └── README.md   # 상세 사용법
└── simulation/     # 거래 시뮬레이션 도구
```

## 🎯 주요 기능

### 📊 Trade (메인 거래 프로그램)
- **현물거래**: BTC, ETH, BNB, ADA, XRP, SOL, DOT, DOGE, AVAX, MATIC, LTC, LINK 지원
- **선물거래**: 12+ 암호화폐 지원, 1x-125x 레버리지
- **보안**: 암호화된 API 키 저장, 마스터 비밀번호 시스템
- **크로스 플랫폼**: Docker 지원 (Windows, Linux, macOS)
- **실시간 모니터링**: 포지션 관리, P&L 추적
- **안전 기능**: 자동 LOT_SIZE/NOTIONAL 필터 처리

### 🔬 Simulation (시뮬레이션 도구)
- 백테스팅 및 전략 테스트
- 리스크 분석 도구
- 가상 거래 환경

## 🚀 빠른 시작

### 1. Trade 프로그램 실행
```bash
cd trade
./docker-run.sh    # Linux/macOS
docker-run.bat     # Windows
```

### 2. 보안 강화 모드 (권장)
```bash
cd trade
./docker-run.sh    # 보안 강화 모드로 실행
```

## 📋 요구사항

- Docker & Docker Compose
- 바이낸스 API 키 (현물거래 및 선물거래 권한)
- 최소 1GB RAM

## ⚠️ 중요 주의사항

1. **API 키 보안**: 절대 API 키를 공유하지 마세요
2. **선물거래 위험**: 높은 레버리지는 큰 손실을 초래할 수 있습니다
3. **테스트 환경**: 실제 거래 전 테스트 환경에서 충분히 테스트하세요
4. **자금 관리**: 손실 감당 가능한 범위 내에서만 거래하세요

## 🔗 상세 문서

각 프로젝트의 상세한 사용법은 해당 폴더의 README.md를 참조하세요:
- [Trade 프로그램 상세 가이드](trade/README.md)
- [Simulation 도구 가이드](simulation/README.md)

## 🤝 기여하기

이슈 리포트, 기능 제안, 풀 리퀘스트를 환영합니다!

## 📄 라이센스

MIT License - 자유롭게 사용하세요.

## ⚖️ 면책 조항

이 프로그램은 교육 및 연구 목적으로 제공됩니다. 실제 거래로 인한 손실에 대해서는 책임지지 않습니다. 투자는 본인의 판단과 책임 하에 진행하세요.

---

**Made with by Kim Nam-Il** 