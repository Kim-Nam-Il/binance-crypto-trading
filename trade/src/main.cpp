#include "binance_api.h"
#include "secure_storage.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <vector>

#ifdef _WIN32
#include <conio.h>
#else
#include <cstdlib>
#endif

void printAccountInfo(const AccountInfo& info) {
    if (info.success) {
        std::cout << "=== 계정 정보 ===" << std::endl;
        std::cout << "BTC 잔고: " << std::fixed << std::setprecision(8) << info.btcBalance << " BTC" << std::endl;
        std::cout << "USDT 잔고: " << std::fixed << std::setprecision(2) << info.usdtBalance << " USDT" << std::endl;
    } else {
        std::cout << "계정 정보 조회 실패: " << info.error << std::endl;
    }
}

void printPrice(const MarketPrice& price) {
    if (price.success) {
        std::cout << "현재 " << price.symbol << " 가격: $" 
                  << std::fixed << std::setprecision(2) << price.price << std::endl;
    } else {
        std::cout << "가격 조회 실패: " << price.error << std::endl;
    }
}

void printOrderResult(const OrderResponse& order) {
    if (order.success) {
        std::cout << "주문 성공!" << std::endl;
        std::cout << "주문 ID: " << order.orderId << std::endl;
        std::cout << "상태: " << order.status << std::endl;
        std::cout << "실행된 수량: " << std::fixed << std::setprecision(8) << order.quantity << std::endl;
        std::cout << "방향: " << order.side << std::endl;
    } else {
        std::cout << "주문 실패: " << order.error << std::endl;
    }
}

void printFuturesAccountInfo(const FuturesAccountInfo& info) {
    if (info.success) {
        std::cout << "=== 선물거래 계정 정보 ===" << std::endl;
        std::cout << "총 지갑 잔고: " << std::fixed << std::setprecision(2) << info.totalWalletBalance << " USDT" << std::endl;
        std::cout << "총 마진 잔고: " << std::fixed << std::setprecision(2) << info.totalMarginBalance << " USDT" << std::endl;
        std::cout << "사용 가능한 잔고: " << std::fixed << std::setprecision(2) << info.availableBalance << " USDT" << std::endl;
        std::cout << "총 미실현 손익: " << std::fixed << std::setprecision(2) << info.totalUnrealizedPnl << " USDT";
        if (info.totalUnrealizedPnl > 0) {
            std::cout << " (✅ 수익)";
        } else if (info.totalUnrealizedPnl < 0) {
            std::cout << " (❌ 손실)";
        }
        std::cout << std::endl;
        std::cout << "최대 출금 가능: " << std::fixed << std::setprecision(2) << info.maxWithdrawAmount << " USDT" << std::endl;
    } else {
        std::cout << "선물거래 계정 정보 조회 실패: " << info.error << std::endl;
    }
}

void printFuturesPosition(const FuturesPosition& position) {
    if (position.success) {
        if (position.positionAmt == 0) {
            std::cout << "현재 " << position.symbol << " 포지션이 없습니다." << std::endl;
        } else {
            std::cout << "=== " << position.symbol << " 포지션 정보 ===" << std::endl;
            std::cout << "포지션 수량: " << std::fixed << std::setprecision(3) << position.positionAmt;
            if (position.positionAmt > 0) {
                std::cout << " (🔵 롱 포지션)";
            } else {
                std::cout << " (🔴 숏 포지션)";
            }
            std::cout << std::endl;
            std::cout << "진입 가격: $" << std::fixed << std::setprecision(2) << position.entryPrice << std::endl;
            std::cout << "현재 가격: $" << std::fixed << std::setprecision(2) << position.markPrice << std::endl;
            std::cout << "미실현 손익: " << std::fixed << std::setprecision(2) << position.unRealizedProfit << " USDT";
            if (position.unRealizedProfit > 0) {
                std::cout << " (✅ 수익)";
            } else if (position.unRealizedProfit < 0) {
                std::cout << " (❌ 손실)";
            }
            std::cout << std::endl;
            std::cout << "수익률: " << std::fixed << std::setprecision(2) << position.percentage << "%" << std::endl;
            std::cout << "레버리지: " << position.leverage << "x" << std::endl;
        }
    } else {
        std::cout << "포지션 조회 실패: " << position.error << std::endl;
    }
}

void printFuturesOrderResult(const FuturesOrderResponse& order) {
    if (order.success) {
        std::cout << "선물거래 주문 성공!" << std::endl;
        std::cout << "주문 ID: " << order.orderId << std::endl;
        std::cout << "상태: " << order.status << std::endl;
        std::cout << "실행된 수량: " << std::fixed << std::setprecision(3) << order.quantity << std::endl;
        std::cout << "방향: " << order.side << " (" << order.positionSide << ")" << std::endl;
        std::cout << "평균 가격: $" << std::fixed << std::setprecision(2) << order.price << std::endl;
    } else {
        std::cout << "선물거래 주문 실패: " << order.error << std::endl;
    }
}

void printFuturesSymbols(const FuturesSymbolsResponse& response) {
    if (!response.success) {
        std::cout << "❌ 심볼 목록 조회 실패: " << response.error << std::endl;
        return;
    }
    
    std::cout << "\n=== 선물거래 가능한 심볼 목록 ===" << std::endl;
    std::cout << "총 " << response.symbols.size() << "개의 USDT 페어 거래 가능" << std::endl;
    std::cout << std::endl;
    
    // 인기 코인들을 먼저 표시
    std::vector<std::string> popular_coins = {
        "BTC", "ETH", "BNB", "ADA", "XRP", "SOL", "DOT", "DOGE", "AVAX", "MATIC", "LTC", "LINK",
        "UNI", "ATOM", "FIL", "TRX", "ETC", "BCH", "ALGO", "VET", "ICP", "THETA", "FTM", "AAVE",
        "NEAR", "SAND", "MANA", "AXS", "SHIB", "CRV", "COMP", "MKR", "SUSHI", "SNX", "YFI", "1INCH"
    };
    
    std::cout << "🔥 인기 코인:" << std::endl;
    int count = 0;
    for (const auto& coin : popular_coins) {
        for (const auto& symbol : response.symbols) {
            if (symbol.baseAsset == coin) {
                std::cout << std::left << std::setw(12) << symbol.symbol;
                count++;
                if (count % 6 == 0) std::cout << std::endl;
                break;
            }
        }
    }
    if (count % 6 != 0) std::cout << std::endl;
    
    std::cout << "\n💡 전체 목록 (알파벳 순):" << std::endl;
    
    // 심볼을 알파벳 순으로 정렬
    std::vector<FuturesSymbolInfo> sorted_symbols = response.symbols;
    std::sort(sorted_symbols.begin(), sorted_symbols.end(), 
              [](const FuturesSymbolInfo& a, const FuturesSymbolInfo& b) {
                  return a.baseAsset < b.baseAsset;
              });
    
    count = 0;
    for (const auto& symbol : sorted_symbols) {
        std::cout << std::left << std::setw(12) << symbol.symbol;
        count++;
        if (count % 8 == 0) std::cout << std::endl;
    }
    if (count % 8 != 0) std::cout << std::endl;
    
    std::cout << "\n📋 상세 정보 (상위 20개):" << std::endl;
    std::cout << std::left << std::setw(12) << "심볼" 
              << std::setw(8) << "자산" 
              << std::setw(12) << "최소수량" 
              << std::setw(12) << "최소금액" 
              << std::setw(8) << "상태" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (int i = 0; i < std::min(20, (int)sorted_symbols.size()); i++) {
        const auto& symbol = sorted_symbols[i];
        std::cout << std::left << std::setw(12) << symbol.symbol
                  << std::setw(8) << symbol.baseAsset
                  << std::setw(12) << std::fixed << std::setprecision(6) << symbol.minQty
                  << std::setw(12) << std::fixed << std::setprecision(2) << symbol.minNotional
                  << std::setw(8) << symbol.status << std::endl;
    }
    
    std::cout << "\n💰 이 모든 심볼들로 선물거래가 가능합니다!" << std::endl;
}

std::string getHiddenInput(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    
#ifdef _WIN32
    // Windows에서 비밀번호 숨기기
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!input.empty()) {
                input.pop_back();
                std::cout << "\b \b";
            }
        } else {
            input += ch;
            std::cout << '*';
        }
    }
    std::cout << std::endl;
#else
    // Unix/Linux에서 비밀번호 숨기기
    system("stty -echo");
    std::getline(std::cin, input);
    system("stty echo");
    std::cout << std::endl;
#endif
    
    return input;
}

std::string selectAsset() {
    std::cout << "\n=== 거래할 자산을 선택하세요 ===" << std::endl;
    std::cout << "1. BTC (Bitcoin)" << std::endl;
    std::cout << "2. ETH (Ethereum)" << std::endl;
    std::cout << "3. BNB (Binance Coin)" << std::endl;
    std::cout << "4. ADA (Cardano)" << std::endl;
    std::cout << "5. XRP (Ripple)" << std::endl;
    std::cout << "6. SOL (Solana)" << std::endl;
    std::cout << "7. DOT (Polkadot)" << std::endl;
    std::cout << "8. DOGE (Dogecoin)" << std::endl;
    std::cout << "9. AVAX (Avalanche)" << std::endl;
    std::cout << "10. MATIC (Polygon)" << std::endl;
    std::cout << "11. LTC (Litecoin)" << std::endl;
    std::cout << "12. LINK (Chainlink)" << std::endl;
    std::cout << "선택 (1-12): ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore();
    
    switch (choice) {
        case 1: return "BTCUSDT";
        case 2: return "ETHUSDT";
        case 3: return "BNBUSDT";
        case 4: return "ADAUSDT";
        case 5: return "XRPUSDT";
        case 6: return "SOLUSDT";
        case 7: return "DOTUSDT";
        case 8: return "DOGEUSDT";
        case 9: return "AVAXUSDT";
        case 10: return "MATICUSDT";
        case 11: return "LTCUSDT";
        case 12: return "LINKUSDT";
        default:
            std::cout << "잘못된 선택입니다. BTC를 기본값으로 사용합니다." << std::endl;
            return "BTCUSDT";
    }
}

std::string getAssetName(const std::string& symbol) {
    if (symbol == "BTCUSDT") return "Bitcoin (BTC)";
    if (symbol == "ETHUSDT") return "Ethereum (ETH)";
    if (symbol == "BNBUSDT") return "Binance Coin (BNB)";
    if (symbol == "ADAUSDT") return "Cardano (ADA)";
    if (symbol == "XRPUSDT") return "Ripple (XRP)";
    if (symbol == "SOLUSDT") return "Solana (SOL)";
    if (symbol == "DOTUSDT") return "Polkadot (DOT)";
    if (symbol == "DOGEUSDT") return "Dogecoin (DOGE)";
    if (symbol == "AVAXUSDT") return "Avalanche (AVAX)";
    if (symbol == "MATICUSDT") return "Polygon (MATIC)";
    if (symbol == "LTCUSDT") return "Litecoin (LTC)";
    if (symbol == "LINKUSDT") return "Chainlink (LINK)";
    return symbol;
}

int main() {
    std::cout << "=== 바이낸스 비트코인 최소수량 거래 프로그램 (보안 강화) ===" << std::endl;
    
    SecureStorage storage;
    std::string api_key, secret_key;
    
    // 저장된 키가 있는지 확인
    if (storage.hasStoredKeys()) {
        std::cout << "\n저장된 API 키를 발견했습니다." << std::endl;
        
        // 마스터 비밀번호 입력
        std::string master_password = getHiddenInput("마스터 비밀번호를 입력하세요: ");
        
        if (!storage.initializeSession(master_password)) {
            std::cout << "세션 초기화 실패" << std::endl;
            return 1;
        }
        
        if (!storage.loadApiKeys(api_key, secret_key)) {
            std::cout << "키 불러오기 실패. 새로운 키를 입력하세요." << std::endl;
            
            // 새 키 입력
            std::cout << "\n바이낸스 API 키를 입력하세요: ";
            std::getline(std::cin, api_key);
            
            secret_key = getHiddenInput("바이낸스 시크릿 키를 입력하세요: ");
            
            if (api_key.empty() || secret_key.empty()) {
                std::cout << "API 키와 시크릿 키가 필요합니다." << std::endl;
                return 1;
            }
            
            // 새 키 저장
            if (!storage.storeApiKeys(api_key, secret_key)) {
                std::cout << "키 저장 실패" << std::endl;
                return 1;
            }
        } else {
            std::cout << "API 키를 성공적으로 불러왔습니다." << std::endl;
        }
    } else {
        std::cout << "\n처음 사용하시는군요! 새로운 마스터 비밀번호를 설정하세요." << std::endl;
        
        // 새 마스터 비밀번호 설정
        std::string master_password = getHiddenInput("새 마스터 비밀번호를 입력하세요: ");
        std::string confirm_password = getHiddenInput("비밀번호를 다시 입력하세요: ");
        
        if (master_password != confirm_password) {
            std::cout << "비밀번호가 일치하지 않습니다." << std::endl;
            return 1;
        }
        
        if (!storage.initializeSession(master_password)) {
            std::cout << "세션 초기화 실패" << std::endl;
            return 1;
        }
        
        // API 키 입력
        std::cout << "\n바이낸스 API 키를 입력하세요: ";
        std::getline(std::cin, api_key);
        
        secret_key = getHiddenInput("바이낸스 시크릿 키를 입력하세요: ");
        
        if (api_key.empty() || secret_key.empty()) {
            std::cout << "API 키와 시크릿 키가 필요합니다." << std::endl;
            return 1;
        }
        
        // 키 저장
        if (!storage.storeApiKeys(api_key, secret_key)) {
            std::cout << "키 저장 실패" << std::endl;
            return 1;
        }
    }
    
    // BinanceAPI 객체 생성
    BinanceAPI binance(api_key, secret_key);
    
    // API 권한 확인
    std::cout << "\nAPI 권한을 확인하는 중..." << std::endl;
    if (!binance.checkApiPermissions()) {
        std::cout << "API 키 권한이 부족합니다. 다음을 확인하세요:" << std::endl;
        std::cout << "1. API 키가 올바른지 확인" << std::endl;
        std::cout << "2. Spot Trading 권한이 활성화되어 있는지 확인" << std::endl;
        std::cout << "3. IP 제한 설정이 올바른지 확인" << std::endl;
        return 1;
    }
    
    // 최소 주문 수량 조회
    double minQuantity = binance.getMinOrderQuantity("BTCUSDT");
    std::cout << "\nBTCUSDT 최소 주문 수량: " << std::fixed << std::setprecision(8) 
              << minQuantity << " BTC" << std::endl;
    
    while (true) {
        // 세션 유효성 검사
        if (!storage.isSessionValid()) {
            std::cout << "\n세션이 만료되었습니다. 다시 로그인하세요." << std::endl;
            std::string master_password = getHiddenInput("마스터 비밀번호를 입력하세요: ");
            
            if (!storage.initializeSession(master_password)) {
                std::cout << "인증 실패. 프로그램을 종료합니다." << std::endl;
                return 1;
            }
            
            if (!storage.loadApiKeys(api_key, secret_key)) {
                std::cout << "키 불러오기 실패. 프로그램을 종료합니다." << std::endl;
                return 1;
            }
            
            // API 객체 재생성
            binance = BinanceAPI(api_key, secret_key);
            std::cout << "세션이 갱신되었습니다." << std::endl;
        }
        
        std::cout << "\n=== 메뉴 ===" << std::endl;
        std::cout << "=== 현물거래 ===" << std::endl;
        std::cout << "1. 계정 정보 조회" << std::endl;
        std::cout << "2. 암호화폐 현재 가격 조회" << std::endl;
        std::cout << "3. 암호화폐 최소수량 구매" << std::endl;
        std::cout << "4. 암호화폐 최소수량 판매" << std::endl;
        std::cout << "5. 사용자 정의 수량으로 구매" << std::endl;
        std::cout << "6. 사용자 정의 수량으로 판매" << std::endl;
        std::cout << "\n=== 선물거래 ===" << std::endl;
        std::cout << "11. 선물거래 계정 정보 조회" << std::endl;
        std::cout << "12. 선물거래 포지션 조회" << std::endl;
        std::cout << "13. 레버리지 설정" << std::endl;
        std::cout << "14. 롱 포지션 진입" << std::endl;
        std::cout << "15. 숏 포지션 진입" << std::endl;
        std::cout << "16. 포지션 종료" << std::endl;
        std::cout << "17. 선물거래 지정가 주문" << std::endl;
        std::cout << "18. 선물거래 가능한 심볼 목록 조회" << std::endl;
        std::cout << "\n=== 시스템 ===" << std::endl;
        std::cout << "7. 세션 상태 확인" << std::endl;
        std::cout << "8. 주문 권한 테스트" << std::endl;
        std::cout << "9. 저장된 키 삭제" << std::endl;
        std::cout << "10. 마스터 비밀번호 변경" << std::endl;
        std::cout << "0. 종료" << std::endl;
        std::cout << "선택: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(); // 개행문자 제거
        
        switch (choice) {
            case 1: {
                std::cout << "\n계정 정보를 조회중..." << std::endl;
                AccountInfo info = binance.getAccountInfo();
                printAccountInfo(info);
                break;
            }
            
            case 2: {
                std::string symbol = selectAsset();
                std::cout << "\n" << getAssetName(symbol) << " 가격을 조회중..." << std::endl;
                MarketPrice price = binance.getCurrentPrice(symbol);
                printPrice(price);
                break;
            }
            
            case 3: {
                std::string symbol = selectAsset();
                std::string assetSymbol = symbol.substr(0, symbol.find("USDT"));
                
                // 현재 가격 조회
                MarketPrice price = binance.getCurrentPrice(symbol);
                if (!price.success) {
                    std::cout << "가격 조회 실패: " << price.error << std::endl;
                    break;
                }
                
                // 최소 주문 금액 (NOTIONAL 필터) 고려
                double minNotional = 5.0; // 바이낸스 최소 주문 금액 $5
                double calculatedMinQuantity = minNotional / price.price;
                
                // 심볼별 최소 수량 조회
                double symbolMinQuantity = binance.getMinOrderQuantity(symbol);
                
                // 기존 최소 수량과 비교하여 더 큰 값 사용
                double actualMinQuantity = std::max(symbolMinQuantity, calculatedMinQuantity);
                
                // LOT_SIZE 필터에 맞게 수량 조정
                std::cout << "\nLOT_SIZE 필터 확인 중..." << std::endl;
                actualMinQuantity = binance.adjustQuantityForLotSize(symbol, actualMinQuantity);
                
                // 조정된 수량이 NOTIONAL 필터를 만족하는지 재확인
                double finalCost = actualMinQuantity * price.price;
                if (finalCost < minNotional) {
                    std::cout << "\n⚠️  LOT_SIZE 조정 후 NOTIONAL 필터 재확인 필요" << std::endl;
                    std::cout << "현재 비용: $" << std::fixed << std::setprecision(2) << finalCost << std::endl;
                    std::cout << "필요 비용: $" << std::fixed << std::setprecision(2) << minNotional << std::endl;
                    
                    // NOTIONAL을 만족하도록 수량 증가
                    double requiredQuantity = minNotional / price.price;
                    std::cout << "NOTIONAL을 위한 최소 수량: " << std::fixed << std::setprecision(8) << requiredQuantity << std::endl;
                    
                    // LOT_SIZE 필터에 맞게 다시 조정 (올림)
                    actualMinQuantity = binance.adjustQuantityForLotSize(symbol, requiredQuantity + 0.00001);
                    finalCost = actualMinQuantity * price.price;
                    
                    std::cout << "최종 조정된 수량: " << std::fixed << std::setprecision(8) << actualMinQuantity << std::endl;
                    std::cout << "최종 비용: $" << std::fixed << std::setprecision(2) << finalCost << std::endl;
                }
                
                std::cout << "\n=== " << getAssetName(symbol) << " 구매 정보 ===" << std::endl;
                std::cout << "현재 " << getAssetName(symbol) << " 가격: $" << std::fixed << std::setprecision(6) << price.price << std::endl;
                std::cout << "최소 주문 수량: " << std::fixed << std::setprecision(8) << actualMinQuantity << " " << assetSymbol << std::endl;
                std::cout << "예상 비용: $" << std::fixed << std::setprecision(2) << (actualMinQuantity * price.price) << std::endl;
                
                // 계정 정보 조회
                AccountInfo account = binance.getAccountInfo();
                if (account.success) {
                    std::cout << "현재 USDT 잔고: $" << std::fixed << std::setprecision(2) << account.usdtBalance << std::endl;
                    
                    // 잔고 확인
                    double requiredUsdt = actualMinQuantity * price.price;
                    if (account.usdtBalance < requiredUsdt) {
                        std::cout << "❌ 잔고 부족: $" << std::fixed << std::setprecision(2) << (requiredUsdt - account.usdtBalance) << " USDT 부족" << std::endl;
                        break;
                    }
                }
                
                std::cout << "\n" << getAssetName(symbol) << "을(를) 정말 구매하시겠습니까? (y/N): ";
                std::string confirm;
                std::getline(std::cin, confirm);
                
                if (confirm == "y" || confirm == "Y") {
                    // 먼저 테스트 주문으로 권한 확인
                    std::cout << "주문 권한을 테스트하는 중..." << std::endl;
                    OrderResponse testOrder = binance.testOrder("BUY", actualMinQuantity);
                    
                    if (!testOrder.success) {
                        std::cout << "❌ 테스트 주문 실패: " << testOrder.error << std::endl;
                        std::cout << "\n가능한 해결 방법:" << std::endl;
                        std::cout << "1. 바이낸스 API 키 설정에서 'Spot Trading' 권한 활성화" << std::endl;
                        std::cout << "2. 'Enable Trading' 옵션 체크" << std::endl;
                        std::cout << "3. IP 제한 설정 확인 (현재 IP 주소 허용)" << std::endl;
                        std::cout << "4. API 키가 올바른지 확인" << std::endl;
                        break;
                    }
                    
                    std::cout << "✅ 테스트 주문 성공! 실제 주문을 실행합니다..." << std::endl;
                    // 현재 buyBitcoin은 BTCUSDT만 지원하므로 일반화된 구매 함수가 필요
                    // 임시로 BTC만 지원하고 나중에 개선
                    if (symbol == "BTCUSDT") {
                        OrderResponse order = binance.buyBitcoin(actualMinQuantity);
                        printOrderResult(order);
                    } else {
                        std::cout << "⚠️  현재 " << getAssetName(symbol) << " 현물거래는 개발 중입니다. BTC만 지원됩니다." << std::endl;
                    }
                } else {
                    std::cout << "구매가 취소되었습니다." << std::endl;
                }
                break;
            }
            
            case 4: {
                std::string symbol = selectAsset();
                std::string assetSymbol = symbol.substr(0, symbol.find("USDT"));
                
                // 현재는 BTC만 지원하므로 BTC 외의 자산은 제한
                if (symbol != "BTCUSDT") {
                    std::cout << "⚠️  현재 " << getAssetName(symbol) << " 현물거래는 개발 중입니다. BTC만 지원됩니다." << std::endl;
                    break;
                }
                
                double symbolMinQuantity = binance.getMinOrderQuantity(symbol);
                std::cout << "\n최소수량(" << std::fixed << std::setprecision(8) 
                          << symbolMinQuantity << " " << assetSymbol << ")으로 " << getAssetName(symbol) << "을(를) 판매합니다..." << std::endl;
                
                std::cout << "정말 판매하시겠습니까? (y/N): ";
                std::string confirm;
                std::getline(std::cin, confirm);
                
                if (confirm == "y" || confirm == "Y") {
                    OrderResponse order = binance.sellBitcoin(symbolMinQuantity);
                    printOrderResult(order);
                } else {
                    std::cout << "판매가 취소되었습니다." << std::endl;
                }
                break;
            }
            
            case 5: {
                std::string symbol = selectAsset();
                std::string assetSymbol = symbol.substr(0, symbol.find("USDT"));
                
                // 현재는 BTC만 지원하므로 BTC 외의 자산은 제한
                if (symbol != "BTCUSDT") {
                    std::cout << "⚠️  현재 " << getAssetName(symbol) << " 현물거래는 개발 중입니다. BTC만 지원됩니다." << std::endl;
                    break;
                }
                
                double symbolMinQuantity = binance.getMinOrderQuantity(symbol);
                std::cout << "\n구매할 " << getAssetName(symbol) << " 수량을 입력하세요 (최소: " 
                          << std::fixed << std::setprecision(8) << symbolMinQuantity << " " << assetSymbol << "): ";
                double quantity;
                std::cin >> quantity;
                std::cin.ignore();
                
                if (quantity < symbolMinQuantity) {
                    std::cout << "최소 주문 수량보다 작습니다." << std::endl;
                    break;
                }
                
                std::cout << quantity << " " << assetSymbol << "를 구매하시겠습니까? (y/N): ";
                std::string confirm;
                std::getline(std::cin, confirm);
                
                if (confirm == "y" || confirm == "Y") {
                    OrderResponse order = binance.buyBitcoin(quantity);
                    printOrderResult(order);
                } else {
                    std::cout << "구매가 취소되었습니다." << std::endl;
                }
                break;
            }
            
            case 6: {
                std::string symbol = selectAsset();
                std::string assetSymbol = symbol.substr(0, symbol.find("USDT"));
                
                // 현재는 BTC만 지원하므로 BTC 외의 자산은 제한
                if (symbol != "BTCUSDT") {
                    std::cout << "⚠️  현재 " << getAssetName(symbol) << " 현물거래는 개발 중입니다. BTC만 지원됩니다." << std::endl;
                    break;
                }
                
                double symbolMinQuantity = binance.getMinOrderQuantity(symbol);
                std::cout << "\n판매할 " << getAssetName(symbol) << " 수량을 입력하세요 (최소: " 
                          << std::fixed << std::setprecision(8) << symbolMinQuantity << " " << assetSymbol << "): ";
                double quantity;
                std::cin >> quantity;
                std::cin.ignore();
                
                if (quantity < symbolMinQuantity) {
                    std::cout << "최소 주문 수량보다 작습니다." << std::endl;
                    break;
                }
                
                std::cout << quantity << " " << assetSymbol << "를 판매하시겠습니까? (y/N): ";
                std::string confirm;
                std::getline(std::cin, confirm);
                
                if (confirm == "y" || confirm == "Y") {
                    OrderResponse order = binance.sellBitcoin(quantity);
                    printOrderResult(order);
                } else {
                    std::cout << "판매가 취소되었습니다." << std::endl;
                }
                break;
            }
            
            case 7: {
                std::cout << "\n=== 세션 상태 ===" << std::endl;
                if (storage.isSessionValid()) {
                    auto now = std::chrono::system_clock::now();
                    auto time_t = std::chrono::system_clock::to_time_t(now);
                    std::cout << "세션 활성화 상태: 유효" << std::endl;
                    std::cout << "현재 시간: " << std::ctime(&time_t);
                    std::cout << "세션 만료까지: 24시간 이내" << std::endl;
                } else {
                    std::cout << "세션 상태: 만료됨" << std::endl;
                }
                break;
            }
            
            case 8: {
                std::cout << "\n=== 주문 권한 테스트 ===" << std::endl;
                std::cout << "실제 거래 없이 주문 권한을 테스트합니다." << std::endl;
                
                // 1단계: 네트워크 연결 테스트
                std::cout << "\n1단계: 네트워크 연결 확인" << std::endl;
                if (!binance.testConnection()) {
                    std::cout << "\n❌ 네트워크 연결에 실패했습니다." << std::endl;
                    std::cout << "해결 방법:" << std::endl;
                    std::cout << "- 인터넷 연결 상태 확인" << std::endl;
                    std::cout << "- 방화벽 설정 확인" << std::endl;
                    std::cout << "- VPN 사용 시 해제 후 재시도" << std::endl;
                    break;
                }
                
                // 2단계: 구매 권한 테스트
                std::cout << "\n2단계: 구매 권한 테스트" << std::endl;
                OrderResponse buyTest = binance.testOrder("BUY", minQuantity);
                if (buyTest.success) {
                    std::cout << "✅ 구매 권한: 정상 (잔고 및 권한 확인 완료)" << std::endl;
                } else {
                    std::cout << "❌ 구매 권한 문제: " << buyTest.error << std::endl;
                }
                
                // 3단계: 판매 권한 테스트
                std::cout << "\n3단계: 판매 권한 테스트" << std::endl;
                OrderResponse sellTest = binance.testOrder("SELL", minQuantity);
                if (sellTest.success) {
                    std::cout << "✅ 판매 권한: 정상 (잔고 및 권한 확인 완료)" << std::endl;
                } else {
                    std::cout << "❌ 판매 권한 문제: " << sellTest.error << std::endl;
                }
                
                // 결과 요약
                std::cout << "\n=== 테스트 결과 요약 ===" << std::endl;
                if (buyTest.success && sellTest.success) {
                    std::cout << "🎉 모든 주문 권한이 정상입니다! 실제 거래가 가능합니다." << std::endl;
                } else if (buyTest.success && !sellTest.success) {
                    std::cout << "⚠️  구매만 가능합니다. 판매하려면 BTC 잔고가 필요합니다." << std::endl;
                } else if (!buyTest.success && sellTest.success) {
                    std::cout << "⚠️  판매만 가능합니다. 구매하려면 USDT 잔고가 필요합니다." << std::endl;
                } else {
                    std::cout << "⚠️  주문 권한에 문제가 있습니다." << std::endl;
                    std::cout << "\n권장 해결 방법:" << std::endl;
                    std::cout << "1. 바이낸스 API 키 설정에서 'Spot Trading' 권한 활성화" << std::endl;
                    std::cout << "2. 'Enable Trading' 옵션 체크" << std::endl;
                    std::cout << "3. IP 제한 설정 확인 (현재 IP 주소 허용)" << std::endl;
                    std::cout << "4. API 키가 올바른지 확인" << std::endl;
                    std::cout << "5. 충분한 잔고 확인 (USDT 또는 BTC)" << std::endl;
                }
                break;
            }
            
            case 9: {
                std::cout << "\n저장된 키를 삭제하시겠습니까?" << std::endl;
                std::cout << "이 작업은 되돌릴 수 없습니다. (y/N): ";
                std::string confirm;
                std::getline(std::cin, confirm);
                
                if (confirm == "y" || confirm == "Y") {
                    if (storage.deleteStoredKeys()) {
                        std::cout << "저장된 키가 삭제되었습니다." << std::endl;
                        std::cout << "프로그램을 종료합니다." << std::endl;
                        return 0;
                    } else {
                        std::cout << "키 삭제에 실패했습니다." << std::endl;
                    }
                } else {
                    std::cout << "삭제가 취소되었습니다." << std::endl;
                }
                break;
            }
            
            case 10: {
                std::cout << "\n마스터 비밀번호를 변경합니다." << std::endl;
                std::string current_password = getHiddenInput("현재 마스터 비밀번호를 입력하세요: ");
                
                // 현재 비밀번호 확인
                SecureStorage temp_storage;
                if (!temp_storage.initializeSession(current_password)) {
                    std::cout << "현재 비밀번호가 올바르지 않습니다." << std::endl;
                    break;
                }
                
                std::string temp_api_key, temp_secret_key;
                if (!temp_storage.loadApiKeys(temp_api_key, temp_secret_key)) {
                    std::cout << "키 불러오기 실패" << std::endl;
                    break;
                }
                
                // 새 비밀번호 설정
                std::string new_password = getHiddenInput("새 마스터 비밀번호를 입력하세요: ");
                std::string confirm_password = getHiddenInput("새 비밀번호를 다시 입력하세요: ");
                
                if (new_password != confirm_password) {
                    std::cout << "새 비밀번호가 일치하지 않습니다." << std::endl;
                    break;
                }
                
                // 새 비밀번호로 세션 초기화 및 키 재저장
                if (storage.initializeSession(new_password)) {
                    if (storage.storeApiKeys(temp_api_key, temp_secret_key)) {
                        std::cout << "마스터 비밀번호가 성공적으로 변경되었습니다." << std::endl;
                    } else {
                        std::cout << "비밀번호 변경 중 오류가 발생했습니다." << std::endl;
                    }
                } else {
                    std::cout << "새 세션 초기화에 실패했습니다." << std::endl;
                }
                
                // 임시 키 정리
                std::fill(temp_api_key.begin(), temp_api_key.end(), 0);
                std::fill(temp_secret_key.begin(), temp_secret_key.end(), 0);
                break;
            }
            
            // === 선물거래 케이스들 ===
            case 11: {
                std::cout << "\n선물거래 계정 정보를 조회중..." << std::endl;
                FuturesAccountInfo info = binance.getFuturesAccountInfo();
                printFuturesAccountInfo(info);
                break;
            }
            
            case 12: {
                std::string symbol = selectAsset();
                std::cout << "\n" << getAssetName(symbol) << " 선물거래 포지션을 조회중..." << std::endl;
                FuturesPosition position = binance.getFuturesPosition(symbol);
                printFuturesPosition(position);
                break;
            }
            
            case 13: {
                std::string symbol = selectAsset();
                std::cout << "\n" << getAssetName(symbol) << " 레버리지를 설정합니다." << std::endl;
                std::cout << "현재 " << symbol << " 포지션 정보:" << std::endl;
                FuturesPosition position = binance.getFuturesPosition(symbol);
                if (position.success) {
                    std::cout << "현재 레버리지: " << position.leverage << "x" << std::endl;
                }
                
                std::cout << "새 레버리지를 입력하세요 (1-125): ";
                int leverage;
                std::cin >> leverage;
                std::cin.ignore();
                
                if (leverage < 1 || leverage > 125) {
                    std::cout << "레버리지는 1~125 사이여야 합니다." << std::endl;
                    break;
                }
                
                std::cout << getAssetName(symbol) << " 레버리지를 " << leverage << "x로 설정하시겠습니까? (y/N): ";
                std::string confirm;
                std::getline(std::cin, confirm);
                
                if (confirm == "y" || confirm == "Y") {
                    if (binance.setLeverage(symbol, leverage)) {
                        std::cout << "✅ " << getAssetName(symbol) << " 레버리지가 " << leverage << "x로 설정되었습니다." << std::endl;
                    } else {
                        std::cout << "❌ 레버리지 설정에 실패했습니다." << std::endl;
                    }
                } else {
                    std::cout << "레버리지 설정이 취소되었습니다." << std::endl;
                }
                break;
            }
            
            case 14: {
                std::string symbol = selectAsset();
                std::cout << "\n" << getAssetName(symbol) << " 롱 포지션 진입" << std::endl;
                
                // 기본 레버리지를 1x로 설정
                std::cout << "안전한 거래를 위해 레버리지를 1x로 설정합니다..." << std::endl;
                if (!binance.setLeverage(symbol, 1)) {
                    std::cout << "⚠️  레버리지 설정에 실패했지만 계속 진행합니다." << std::endl;
                }
                
                // 현재 가격 조회
                MarketPrice price = binance.getCurrentPrice(symbol);
                if (!price.success) {
                    std::cout << "가격 조회 실패: " << price.error << std::endl;
                    break;
                }
                
                // 계정 정보 조회
                FuturesAccountInfo account = binance.getFuturesAccountInfo();
                if (!account.success) {
                    std::cout << "계정 정보 조회 실패: " << account.error << std::endl;
                    break;
                }
                
                std::string assetSymbol = symbol.substr(0, symbol.find("USDT"));
                std::cout << "현재 " << getAssetName(symbol) << " 가격: $" << std::fixed << std::setprecision(6) << price.price << std::endl;
                std::cout << "사용 가능한 잔고: $" << std::fixed << std::setprecision(2) << account.availableBalance << std::endl;
                std::cout << "레버리지: 1x (안전 모드)" << std::endl;
                
                std::cout << "주문 수량을 입력하세요 (" << assetSymbol << "): ";
                double quantity;
                std::cin >> quantity;
                std::cin.ignore();
                
                if (quantity <= 0) {
                    std::cout << "올바른 수량을 입력하세요." << std::endl;
                    break;
                }
                
                double cost = quantity * price.price;
                std::cout << "예상 비용: $" << std::fixed << std::setprecision(2) << cost << std::endl;
                
                std::cout << quantity << " " << assetSymbol << " 롱 포지션을 진입하시겠습니까? (레버리지 1x) (y/N): ";
                std::string confirm;
                std::getline(std::cin, confirm);
                
                if (confirm == "y" || confirm == "Y") {
                    FuturesOrderResponse order = binance.openLongPosition(symbol, quantity);
                    printFuturesOrderResult(order);
                } else {
                    std::cout << "롱 포지션 진입이 취소되었습니다." << std::endl;
                }
                break;
            }
            
            case 15: {
                std::string symbol = selectAsset();
                std::cout << "\n" << getAssetName(symbol) << " 숏 포지션 진입" << std::endl;
                
                // 기본 레버리지를 1x로 설정
                std::cout << "안전한 거래를 위해 레버리지를 1x로 설정합니다..." << std::endl;
                if (!binance.setLeverage(symbol, 1)) {
                    std::cout << "⚠️  레버리지 설정에 실패했지만 계속 진행합니다." << std::endl;
                }
                
                // 현재 가격 조회
                MarketPrice price = binance.getCurrentPrice(symbol);
                if (!price.success) {
                    std::cout << "가격 조회 실패: " << price.error << std::endl;
                    break;
                }
                
                // 계정 정보 조회
                FuturesAccountInfo account = binance.getFuturesAccountInfo();
                if (!account.success) {
                    std::cout << "계정 정보 조회 실패: " << account.error << std::endl;
                    break;
                }
                
                std::string assetSymbol = symbol.substr(0, symbol.find("USDT"));
                std::cout << "현재 " << getAssetName(symbol) << " 가격: $" << std::fixed << std::setprecision(6) << price.price << std::endl;
                std::cout << "사용 가능한 잔고: $" << std::fixed << std::setprecision(2) << account.availableBalance << std::endl;
                std::cout << "레버리지: 1x (안전 모드)" << std::endl;
                
                std::cout << "주문 수량을 입력하세요 (" << assetSymbol << "): ";
                double quantity;
                std::cin >> quantity;
                std::cin.ignore();
                
                if (quantity <= 0) {
                    std::cout << "올바른 수량을 입력하세요." << std::endl;
                    break;
                }
                
                double cost = quantity * price.price;
                std::cout << "예상 비용: $" << std::fixed << std::setprecision(2) << cost << std::endl;
                
                std::cout << quantity << " " << assetSymbol << " 숏 포지션을 진입하시겠습니까? (레버리지 1x) (y/N): ";
                std::string confirm;
                std::getline(std::cin, confirm);
                
                if (confirm == "y" || confirm == "Y") {
                    FuturesOrderResponse order = binance.openShortPosition(symbol, quantity);
                    printFuturesOrderResult(order);
                } else {
                    std::cout << "숏 포지션 진입이 취소되었습니다." << std::endl;
                }
                break;
            }
            
            case 16: {
                std::string symbol = selectAsset();
                std::cout << "\n" << getAssetName(symbol) << " 포지션 종료" << std::endl;
                
                // 현재 포지션 조회
                FuturesPosition position = binance.getFuturesPosition(symbol);
                if (!position.success) {
                    std::cout << "포지션 조회 실패: " << position.error << std::endl;
                    break;
                }
                
                if (position.positionAmt == 0) {
                    std::cout << getAssetName(symbol) << " 종료할 포지션이 없습니다." << std::endl;
                    break;
                }
                
                printFuturesPosition(position);
                
                std::cout << "\n" << getAssetName(symbol) << " 포지션을 종료하시겠습니까? (y/N): ";
                std::string confirm;
                std::getline(std::cin, confirm);
                
                if (confirm == "y" || confirm == "Y") {
                    FuturesOrderResponse order = binance.closePosition(symbol);
                    printFuturesOrderResult(order);
                } else {
                    std::cout << "포지션 종료가 취소되었습니다." << std::endl;
                }
                break;
            }
            
            case 17: {
                std::string symbol = selectAsset();
                std::cout << "\n" << getAssetName(symbol) << " 선물거래 지정가 주문" << std::endl;
                
                // 현재 가격 조회
                MarketPrice price = binance.getCurrentPrice(symbol);
                if (!price.success) {
                    std::cout << "가격 조회 실패: " << price.error << std::endl;
                    break;
                }
                
                std::string assetSymbol = symbol.substr(0, symbol.find("USDT"));
                std::cout << "현재 " << getAssetName(symbol) << " 가격: $" << std::fixed << std::setprecision(6) << price.price << std::endl;
                
                std::cout << "주문 방향을 선택하세요 (1: 롱/매수, 2: 숏/매도): ";
                int direction;
                std::cin >> direction;
                std::cin.ignore();
                
                if (direction != 1 && direction != 2) {
                    std::cout << "올바른 방향을 선택하세요." << std::endl;
                    break;
                }
                
                std::string side = (direction == 1) ? "BUY" : "SELL";
                std::string directionStr = (direction == 1) ? "롱(매수)" : "숏(매도)";
                
                std::cout << "주문 수량을 입력하세요 (" << assetSymbol << "): ";
                double quantity;
                std::cin >> quantity;
                std::cin.ignore();
                
                if (quantity <= 0) {
                    std::cout << "올바른 수량을 입력하세요." << std::endl;
                    break;
                }
                
                std::cout << "지정가를 입력하세요 ($): ";
                double orderPrice;
                std::cin >> orderPrice;
                std::cin.ignore();
                
                if (orderPrice <= 0) {
                    std::cout << "올바른 가격을 입력하세요." << std::endl;
                    break;
                }
                
                std::cout << "\n=== 주문 정보 ===" << std::endl;
                std::cout << "자산: " << getAssetName(symbol) << std::endl;
                std::cout << "방향: " << directionStr << std::endl;
                std::cout << "수량: " << std::fixed << std::setprecision(6) << quantity << " " << assetSymbol << std::endl;
                std::cout << "지정가: $" << std::fixed << std::setprecision(6) << orderPrice << std::endl;
                std::cout << "총 금액: $" << std::fixed << std::setprecision(2) << (quantity * orderPrice) << std::endl;
                
                std::cout << "\n지정가 주문을 실행하시겠습니까? (y/N): ";
                std::string confirm;
                std::getline(std::cin, confirm);
                
                if (confirm == "y" || confirm == "Y") {
                    FuturesOrderResponse order = binance.futuresLimitOrder(symbol, side, quantity, orderPrice);
                    printFuturesOrderResult(order);
                } else {
                    std::cout << "지정가 주문이 취소되었습니다." << std::endl;
                }
                break;
            }
            
            case 18: {
                std::cout << "\n선물거래 가능한 심볼 목록을 조회중..." << std::endl;
                FuturesSymbolsResponse symbolsResponse = binance.getFuturesSymbols();
                printFuturesSymbols(symbolsResponse);
                break;
            }
            
            case 0:
                std::cout << "프로그램을 종료합니다." << std::endl;
                storage.clearSession();
                return 0;
                
            default:
                std::cout << "잘못된 선택입니다." << std::endl;
                break;
        }
        
        std::cout << "\n계속하려면 Enter를 누르세요...";
        std::cin.get();
    }
    
    return 0;
} 