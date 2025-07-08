#pragma once

#include <string>
#include <map>
#include <vector>

struct OrderResponse {
    std::string symbol;
    std::string orderId;
    std::string status;
    double price;
    double quantity;
    std::string side;
    std::string error;
    bool success;
};

struct AccountInfo {
    double btcBalance;
    double usdtBalance;
    bool success;
    std::string error;
};

struct MarketPrice {
    std::string symbol;
    double price;
    bool success;
    std::string error;
};

// 선물거래 계정 정보
struct FuturesAccountInfo {
    double totalWalletBalance;    // 총 지갑 잔고
    double totalUnrealizedPnl;    // 총 미실현 손익
    double totalMarginBalance;    // 총 마진 잔고
    double availableBalance;      // 사용 가능한 잔고
    double maxWithdrawAmount;     // 최대 출금 가능 금액
    bool success;
    std::string error;
};

// 선물거래 포지션 정보
struct FuturesPosition {
    std::string symbol;           // 심볼 (예: BTCUSDT)
    double positionAmt;           // 포지션 수량 (양수: 롱, 음수: 숏)
    double entryPrice;            // 진입 가격
    double markPrice;             // 마크 가격
    double unRealizedProfit;      // 미실현 손익
    double percentage;            // 수익률 (%)
    std::string positionSide;     // 포지션 방향 (LONG/SHORT/BOTH)
    int leverage;                 // 레버리지
    bool success;
    std::string error;
};

// 선물거래 주문 응답
struct FuturesOrderResponse {
    std::string symbol;
    std::string orderId;
    std::string clientOrderId;
    std::string status;
    double price;
    double quantity;
    std::string side;             // BUY/SELL
    std::string positionSide;     // LONG/SHORT/BOTH
    std::string type;             // MARKET/LIMIT 등
    std::string timeInForce;      // GTC/IOC/FOK
    bool reduceOnly;              // 포지션 감소 전용
    bool success;
    std::string error;
};

// 선물거래 심볼 정보
struct FuturesSymbolInfo {
    std::string symbol;           // 심볼 (예: BTCUSDT)
    std::string baseAsset;        // 기본 자산 (예: BTC)
    std::string quoteAsset;       // 견적 자산 (예: USDT)
    std::string status;           // 거래 상태 (TRADING/BREAK 등)
    double minQty;                // 최소 주문 수량
    double maxQty;                // 최대 주문 수량
    double stepSize;              // 수량 단위
    double minNotional;           // 최소 주문 금액
    int pricePrecision;           // 가격 정밀도
    int quantityPrecision;        // 수량 정밀도
};

// 선물거래 심볼 목록 응답
struct FuturesSymbolsResponse {
    std::vector<FuturesSymbolInfo> symbols;
    bool success;
    std::string error;
};

class BinanceAPI {
public:
    BinanceAPI(const std::string& api_key, const std::string& secret_key);
    
    // 계정 정보 조회
    AccountInfo getAccountInfo();
    
    // 현재 시장 가격 조회
    MarketPrice getCurrentPrice(const std::string& symbol = "BTCUSDT");
    
    // 비트코인 구매
    OrderResponse buyBitcoin(double quantity);
    
    // 비트코인 판매
    OrderResponse sellBitcoin(double quantity);
    
    // 최소 주문 수량 조회
    double getMinOrderQuantity(const std::string& symbol = "BTCUSDT");
    
    // LOT_SIZE 필터에 맞게 수량 조정
    double adjustQuantityForLotSize(const std::string& symbol, double quantity);
    
    // API 키 권한 확인
    bool checkApiPermissions();
    
    // 테스트 주문 (실제 실행 안함)
    OrderResponse testOrder(const std::string& side, double quantity);
    
    // 네트워크 연결 테스트
    bool testConnection();

    // === 선물거래 기능 ===
    
    // 선물거래 계정 정보 조회
    FuturesAccountInfo getFuturesAccountInfo();
    
    // 선물거래 포지션 조회
    std::vector<FuturesPosition> getFuturesPositions();
    
    // 특정 심볼의 포지션 조회
    FuturesPosition getFuturesPosition(const std::string& symbol = "BTCUSDT");
    
    // 레버리지 설정
    bool setLeverage(const std::string& symbol, int leverage);
    
    // 마진 타입 설정 (ISOLATED/CROSSED)
    bool setMarginType(const std::string& symbol, const std::string& marginType);
    
    // 선물거래 롱 포지션 진입
    FuturesOrderResponse openLongPosition(const std::string& symbol, double quantity);
    
    // 선물거래 숏 포지션 진입
    FuturesOrderResponse openShortPosition(const std::string& symbol, double quantity);
    
    // 선물거래 포지션 종료
    FuturesOrderResponse closePosition(const std::string& symbol, const std::string& positionSide = "BOTH");
    
    // 선물거래 시장가 주문
    FuturesOrderResponse futuresMarketOrder(const std::string& symbol, const std::string& side, 
                                           double quantity, const std::string& positionSide = "BOTH");
    
    // 선물거래 지정가 주문
    FuturesOrderResponse futuresLimitOrder(const std::string& symbol, const std::string& side, 
                                          double quantity, double price, const std::string& positionSide = "BOTH");
    
    // 선물거래 가능한 심볼 목록 조회
    FuturesSymbolsResponse getFuturesSymbols();
    
    // 선물거래 최소주문수량 검증 및 조정
    struct FuturesOrderValidation {
        bool isValid;
        double adjustedQuantity;
        double minQuantity;
        double minNotional;
        double currentPrice;
        std::string warning;
        std::string error;
    };
    
    FuturesOrderValidation validateFuturesOrderQuantity(const std::string& symbol, double quantity);

private:
    std::string api_key_;
    std::string secret_key_;
    std::string base_url_;
    std::string futures_base_url_;
    
    std::string createSignature(const std::string& query_string);
    std::string makeRequest(const std::string& endpoint, const std::string& method = "GET", 
                          const std::map<std::string, std::string>& params = {}, bool is_signed = false);
    std::string makeFuturesRequest(const std::string& endpoint, const std::string& method = "GET", 
                                 const std::map<std::string, std::string>& params = {}, bool is_signed = false);
    long long getCurrentTimestamp();
}; 