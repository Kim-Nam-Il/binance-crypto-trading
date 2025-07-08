#!/bin/bash

echo "🚀 바이낸스 비트코인 거래 프로그램"
echo ""

echo "📋 보안 강화 모드로 실행됩니다:"
echo "   - 저장된 키는 컨테이너 삭제 시 완전히 제거됩니다"
echo "   - 호스트 접근이 차단되어 더 안전합니다"
echo "   - 비밀번호는 화면에 표시되지 않지만 입력됩니다"
echo ""

read -p "Continue? (y/N): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Cancelled."
    exit 1
fi

echo ""
echo "🔨 Building Docker image..."
docker build -f Dockerfile.secure -t binance-trader-secure . --quiet

if [ $? -eq 0 ]; then
    echo "✅ Build completed!"
    echo ""
    echo "🚀 Starting container..."
    echo "💡 TIP: Passwords will not be displayed on screen but input is working."
    echo ""
    
    docker run -it --rm \
        --name binance-trader-secure \
        --security-opt no-new-privileges:true \
        --tmpfs /tmp:rw,noexec,nosuid,size=100m \
        binance-trader-secure
else
    echo "❌ Build failed. Please check if Docker is properly installed."
    exit 1
fi

echo ""
echo "✅ Program terminated" 