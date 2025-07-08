#include "binance_api.h"
#include "json_parser.h"
#include <curl/curl.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <thread>
#include <cmath>

// CURL response를 위한 구조체
struct CurlResponse {
    std::string data;
};

// CURL 응답 콜백 함수
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, CurlResponse* response) {
    size_t totalSize = size * nmemb;
    response->data.append((char*)contents, totalSize);
    return totalSize;
}

BinanceAPI::BinanceAPI(const std::string& api_key, const std::string& secret_key) 
    : api_key_(api_key), secret_key_(secret_key), base_url_("https://api.binance.com"), 
      futures_base_url_("https://fapi.binance.com") {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

std::string BinanceAPI::createSignature(const std::string& query_string) {
    unsigned char* digest = HMAC(EVP_sha256(), 
                                secret_key_.c_str(), secret_key_.length(),
                                (unsigned char*)query_string.c_str(), query_string.length(),
                                nullptr, nullptr);
    
    std::stringstream ss;
    for (int i = 0; i < 32; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
    }
    return ss.str();
}

long long BinanceAPI::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

std::string BinanceAPI::makeRequest(const std::string& endpoint, const std::string& method,
                                   const std::map<std::string, std::string>& params, bool is_signed) {
    CURL* curl = curl_easy_init();
    CurlResponse response;
    
    if (!curl) {
        return "{\"error\":\"Failed to initialize CURL\"}";
    }
    
    // 쿼리 스트링 생성
    std::string query_string;
    for (const auto& param : params) {
        if (!query_string.empty()) query_string += "&";
        query_string += param.first + "=" + param.second;
    }
    
    if (is_signed) {
        if (!query_string.empty()) query_string += "&";
        query_string += "timestamp=" + std::to_string(getCurrentTimestamp());
        
        std::string signature = createSignature(query_string);
        query_string += "&signature=" + signature;
    }
    
    std::string full_url = base_url_ + endpoint;
    
    // POST 요청의 경우 데이터를 body에 넣기
    if (method == "POST") {
        curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query_string.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, query_string.length());
    } else {
        // GET 요청의 경우 URL에 쿼리 스트링 추가
        if (!query_string.empty()) {
            full_url += "?" + query_string;
        }
        curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
    }
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
    // 실제 주문의 경우 더 짧은 타임아웃 사용
    if (endpoint == "/api/v3/order") {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);  // 15초로 단축
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);  // 5초로 단축
    } else {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    }
    
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Binance-Trader/1.0");
    
    // 헤더 설정
    struct curl_slist* headers = nullptr;
    std::string api_key_header = "X-MBX-APIKEY: " + api_key_;
    headers = curl_slist_append(headers, api_key_header.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    CURLcode res = curl_easy_perform(curl);
    
    // HTTP 응답 코드 확인
    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        std::string error_msg = "네트워크 요청 실패: ";
        switch (res) {
            case CURLE_OPERATION_TIMEDOUT:
                error_msg += "타임아웃 (15초 초과)";
                break;
            case CURLE_COULDNT_CONNECT:
                error_msg += "연결 실패";
                break;
            case CURLE_COULDNT_RESOLVE_HOST:
                error_msg += "DNS 해석 실패";
                break;
            case CURLE_SSL_CONNECT_ERROR:
                error_msg += "SSL 연결 실패";
                break;
            default:
                error_msg += curl_easy_strerror(res);
                break;
        }
        return "{\"error\":\"" + error_msg + "\"}";
    }
    
    if (response_code >= 400) {
        return "{\"error\":\"HTTP 오류 " + std::to_string(response_code) + ": " + response.data + "\"}";
    }
    
    return response.data;
}

std::string BinanceAPI::makeFuturesRequest(const std::string& endpoint, const std::string& method,
                                          const std::map<std::string, std::string>& params, bool is_signed) {
    CURL* curl = curl_easy_init();
    CurlResponse response;
    
    if (!curl) {
        return "{\"error\":\"Failed to initialize CURL\"}";
    }
    
    // 쿼리 스트링 생성
    std::string query_string;
    for (const auto& param : params) {
        if (!query_string.empty()) query_string += "&";
        query_string += param.first + "=" + param.second;
    }
    
    if (is_signed) {
        if (!query_string.empty()) query_string += "&";
        query_string += "timestamp=" + std::to_string(getCurrentTimestamp());
        
        std::string signature = createSignature(query_string);
        query_string += "&signature=" + signature;
    }
    
    std::string full_url = futures_base_url_ + endpoint;
    
    // POST 요청의 경우 데이터를 body에 넣기
    if (method == "POST" || method == "PUT") {
        curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
        if (method == "POST") {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
        } else {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        }
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query_string.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, query_string.length());
    } else {
        // GET 요청의 경우 URL에 쿼리 스트링 추가
        if (!query_string.empty()) {
            full_url += "?" + query_string;
        }
        curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
    }
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Binance-Trader/1.0");
    
    // 헤더 설정
    struct curl_slist* headers = nullptr;
    std::string api_key_header = "X-MBX-APIKEY: " + api_key_;
    headers = curl_slist_append(headers, api_key_header.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    CURLcode res = curl_easy_perform(curl);
    
    // HTTP 응답 코드 확인
    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        std::string error_msg = "선물거래 API 요청 실패: ";
        switch (res) {
            case CURLE_OPERATION_TIMEDOUT:
                error_msg += "타임아웃 (30초 초과)";
                break;
            case CURLE_COULDNT_CONNECT:
                error_msg += "연결 실패";
                break;
            case CURLE_COULDNT_RESOLVE_HOST:
                error_msg += "DNS 해석 실패";
                break;
            case CURLE_SSL_CONNECT_ERROR:
                error_msg += "SSL 연결 실패";
                break;
            default:
                error_msg += curl_easy_strerror(res);
                break;
        }
        return "{\"error\":\"" + error_msg + "\"}";
    }
    
    if (response_code >= 400) {
        return "{\"error\":\"HTTP 오류 " + std::to_string(response_code) + ": " + response.data + "\"}";
    }
    
    return response.data;
}

AccountInfo BinanceAPI::getAccountInfo() {
    AccountInfo info;
    
    std::string response = makeRequest("/api/v3/account", "GET", {}, true);
    
    if (response.find("\"error\"") != std::string::npos) {
        info.success = false;
        info.error = JSONParser::extractString(response, "error");
        return info;
    }
    
    // balances 배열에서 BTC와 USDT 잔고 찾기
    size_t balances_pos = response.find("\"balances\"");
    if (balances_pos != std::string::npos) {
        size_t btc_pos = response.find("\"asset\":\"BTC\"", balances_pos);
        if (btc_pos != std::string::npos) {
            size_t free_pos = response.find("\"free\":", btc_pos);
            if (free_pos != std::string::npos) {
                info.btcBalance = JSONParser::extractDouble(response.substr(free_pos - 20, 100), "free");
            }
        }
        
        size_t usdt_pos = response.find("\"asset\":\"USDT\"", balances_pos);
        if (usdt_pos != std::string::npos) {
            size_t free_pos = response.find("\"free\":", usdt_pos);
            if (free_pos != std::string::npos) {
                info.usdtBalance = JSONParser::extractDouble(response.substr(free_pos - 20, 100), "free");
            }
        }
    }
    
    info.success = true;
    return info;
}

MarketPrice BinanceAPI::getCurrentPrice(const std::string& symbol) {
    MarketPrice price_info;
    price_info.symbol = symbol;
    
    std::map<std::string, std::string> params;
    params["symbol"] = symbol;
    
    std::string response = makeRequest("/api/v3/ticker/price", "GET", params, false);
    
    if (response.find("\"error\"") != std::string::npos) {
        price_info.success = false;
        price_info.error = JSONParser::extractString(response, "error");
        return price_info;
    }
    
    price_info.price = JSONParser::extractDouble(response, "price");
    price_info.success = true;
    
    return price_info;
}

double BinanceAPI::getMinOrderQuantity(const std::string& symbol) {
    std::map<std::string, std::string> params;
    params["symbol"] = symbol;
    
    std::string response = makeRequest("/api/v3/exchangeInfo", "GET", params, false);
    
    // symbols 배열에서 해당 심볼의 필터 정보 찾기
    size_t symbol_pos = response.find("\"symbol\":\"" + symbol + "\"");
    if (symbol_pos != std::string::npos) {
        size_t filters_pos = response.find("\"filters\"", symbol_pos);
        if (filters_pos != std::string::npos) {
            size_t lot_size_pos = response.find("\"filterType\":\"LOT_SIZE\"", filters_pos);
            if (lot_size_pos != std::string::npos) {
                return JSONParser::extractDouble(response.substr(lot_size_pos, 200), "minQty");
            }
        }
    }
    
    return 0.00001; // 기본값
}

double BinanceAPI::adjustQuantityForLotSize(const std::string& symbol, double quantity) {
    std::map<std::string, std::string> params;
    params["symbol"] = symbol;
    
    std::string response = makeRequest("/api/v3/exchangeInfo", "GET", params, false);
    
    // LOT_SIZE 필터 정보 찾기
    size_t symbol_pos = response.find("\"symbol\":\"" + symbol + "\"");
    if (symbol_pos != std::string::npos) {
        size_t filters_pos = response.find("\"filters\"", symbol_pos);
        if (filters_pos != std::string::npos) {
            size_t lot_size_pos = response.find("\"filterType\":\"LOT_SIZE\"", filters_pos);
            if (lot_size_pos != std::string::npos) {
                std::string filter_section = response.substr(lot_size_pos, 300);
                
                double minQty = JSONParser::extractDouble(filter_section, "minQty");
                double stepSize = JSONParser::extractDouble(filter_section, "stepSize");
                
                std::cout << "LOT_SIZE 필터 정보:" << std::endl;
                std::cout << "  최소 수량: " << std::fixed << std::setprecision(8) << minQty << std::endl;
                std::cout << "  단위 크기: " << std::fixed << std::setprecision(8) << stepSize << std::endl;
                
                // stepSize가 0이면 기본값 사용
                if (stepSize <= 0) {
                    stepSize = 0.00001;
                }
                
                // 최소 수량보다 작으면 최소 수량 사용
                if (quantity < minQty) {
                    quantity = minQty;
                }
                
                // stepSize의 배수로 조정
                double adjusted = std::floor(quantity / stepSize) * stepSize;
                
                // 조정된 수량이 최소 수량보다 작으면 한 단계 올림
                if (adjusted < minQty) {
                    adjusted = std::ceil(minQty / stepSize) * stepSize;
                }
                
                // 원래 수량보다 작아졌다면 한 단계 올림 (NOTIONAL 필터 고려)
                if (adjusted < quantity) {
                    adjusted = std::ceil(quantity / stepSize) * stepSize;
                }
                
                std::cout << "  원래 수량: " << std::fixed << std::setprecision(8) << quantity << std::endl;
                std::cout << "  조정된 수량: " << std::fixed << std::setprecision(8) << adjusted << std::endl;
                
                return adjusted;
            }
        }
    }
    
    return quantity; // 필터 정보를 찾을 수 없으면 원래 수량 반환
}

bool BinanceAPI::checkApiPermissions() {
    // API 키 권한 확인을 위해 계정 정보 조회 시도
    std::string response = makeRequest("/api/v3/account", "GET", {}, true);
    
    if (response.find("\"error\"") != std::string::npos || response.find("\"code\"") != std::string::npos) {
        std::cout << "API 권한 확인 실패: " << response << std::endl;
        return false;
    }
    
    // 계정 권한 상세 확인
    std::cout << "API 권한 확인 성공" << std::endl;
    
    // 권한 정보 추출
    if (response.find("\"canTrade\":true") != std::string::npos) {
        std::cout << "✅ 거래 권한: 활성화됨" << std::endl;
    } else if (response.find("\"canTrade\":false") != std::string::npos) {
        std::cout << "❌ 거래 권한: 비활성화됨 - 바이낸스에서 Spot Trading 권한을 활성화하세요!" << std::endl;
        return false;
    } else {
        std::cout << "⚠️  거래 권한 상태를 확인할 수 없습니다." << std::endl;
    }
    
    if (response.find("\"canWithdraw\":true") != std::string::npos) {
        std::cout << "✅ 출금 권한: 활성화됨" << std::endl;
    } else {
        std::cout << "ℹ️  출금 권한: 비활성화됨 (거래에는 영향 없음)" << std::endl;
    }
    
    return true;
}

OrderResponse BinanceAPI::testOrder(const std::string& side, double quantity) {
    OrderResponse order;
    order.symbol = "BTCUSDT";
    order.side = side;
    
    std::cout << "테스트 주문 요청 준비 중... (" << side << " " << std::fixed << std::setprecision(8) << quantity << " BTC)" << std::endl;
    
    // 먼저 현재 가격 조회로 API 연결 확인
    std::cout << "현재 가격 조회 중..." << std::endl;
    MarketPrice price = getCurrentPrice("BTCUSDT");
    if (!price.success) {
        order.success = false;
        order.error = "가격 조회 실패: " + price.error;
        return order;
    }
    
    std::cout << "현재 BTC 가격: $" << std::fixed << std::setprecision(2) << price.price << std::endl;
    
    // 계정 정보 조회로 거래 권한 확인
    std::cout << "계정 권한 확인 중..." << std::endl;
    AccountInfo account = getAccountInfo();
    if (!account.success) {
        order.success = false;
        order.error = "계정 정보 조회 실패: " + account.error;
        return order;
    }
    
    // 거래 권한이 있는지 확인 (계정 정보 조회 성공 = 거래 권한 있음)
    std::cout << "거래 권한 확인 완료" << std::endl;
    
    // 잔고 확인
    if (side == "BUY") {
        double requiredUsdt = quantity * price.price;
        std::cout << "필요 USDT: " << std::fixed << std::setprecision(2) << requiredUsdt << std::endl;
        std::cout << "보유 USDT: " << std::fixed << std::setprecision(2) << account.usdtBalance << std::endl;
        
        if (account.usdtBalance < requiredUsdt) {
            order.success = false;
            order.error = "잔고 부족: " + std::to_string(requiredUsdt - account.usdtBalance) + " USDT 부족";
            return order;
        }
    } else if (side == "SELL") {
        std::cout << "필요 BTC: " << std::fixed << std::setprecision(8) << quantity << std::endl;
        std::cout << "보유 BTC: " << std::fixed << std::setprecision(8) << account.btcBalance << std::endl;
        
        if (account.btcBalance < quantity) {
            order.success = false;
            order.error = "잔고 부족: " + std::to_string(quantity - account.btcBalance) + " BTC 부족";
            return order;
        }
    }
    
    // 테스트 성공
    order.success = true;
    order.status = "TEST_SUCCESS";
    order.quantity = quantity;
    
    return order;
}

bool BinanceAPI::testConnection() {
    std::cout << "바이낸스 서버 연결 테스트 중..." << std::endl;
    
    // 단순한 서버 시간 조회 (인증 불필요)
    std::string response = makeRequest("/api/v3/time", "GET", {}, false);
    
    if (response.empty()) {
        std::cout << "❌ 연결 실패: 응답 없음" << std::endl;
        return false;
    }
    
    if (response.find("\"error\"") != std::string::npos) {
        std::cout << "❌ 연결 실패: " << response << std::endl;
        return false;
    }
    
    if (response.find("serverTime") != std::string::npos) {
        std::cout << "✅ 바이낸스 서버 연결 성공" << std::endl;
        return true;
    }
    
    std::cout << "⚠️  예상하지 못한 응답: " << response << std::endl;
    return false;
}

OrderResponse BinanceAPI::buyBitcoin(double quantity) {
    OrderResponse order;
    order.symbol = "BTCUSDT";
    order.side = "BUY";
    
    std::cout << "=== 실제 구매 주문 시작 ===" << std::endl;
    std::cout << "주문 수량: " << std::fixed << std::setprecision(8) << quantity << " BTC" << std::endl;
    
    // 현재 시간 확인
    long long timestamp = getCurrentTimestamp();
    std::cout << "현재 타임스탬프: " << timestamp << std::endl;
    
    std::map<std::string, std::string> params;
    params["symbol"] = "BTCUSDT";
    params["side"] = "BUY";
    params["type"] = "MARKET";
    
    // 수량을 8자리 소수점으로 포맷팅
    std::stringstream ss;
    ss << std::fixed << std::setprecision(8) << quantity;
    params["quantity"] = ss.str();
    
    std::cout << "주문 파라미터:" << std::endl;
    for (const auto& param : params) {
        std::cout << "  " << param.first << " = " << param.second << std::endl;
    }
    
    std::cout << "\n바이낸스 API 호출 시작..." << std::endl;
    
    // 한 번만 시도 (재시도 제거)
    std::string response = makeRequest("/api/v3/order", "POST", params, true);
    
    std::cout << "API 호출 완료!" << std::endl;
    std::cout << "응답 수신됨 (길이: " << response.length() << " 바이트)" << std::endl;
    
    // 응답이 비어있는지 확인
    if (response.empty()) {
        std::cout << "❌ 빈 응답 수신" << std::endl;
        order.success = false;
        order.error = "API 응답 없음";
        return order;
    }
    
    // 응답 내용 출력 (처음 500자만)
    std::cout << "API 응답 내용: " << response.substr(0, std::min(500, (int)response.length())) << std::endl;
    
    // 오류 응답 확인
    if (response.find("\"error\"") != std::string::npos || response.find("\"code\"") != std::string::npos) {
        std::cout << "❌ 오류 응답 감지" << std::endl;
        order.success = false;
        
        // 오류 메시지 추출
        std::string error_msg = JSONParser::extractString(response, "msg");
        if (error_msg.empty()) {
            error_msg = JSONParser::extractString(response, "error");
        }
        
        // 오류 코드 확인
        std::string error_code = JSONParser::extractString(response, "code");
        if (!error_code.empty()) {
            error_msg = "오류 코드 " + error_code + ": " + error_msg;
        }
        
        if (error_msg.empty()) {
            error_msg = "알 수 없는 오류: " + response;
        }
        
        order.error = error_msg;
        std::cout << "오류 내용: " << order.error << std::endl;
        return order;
    }
    
    // 성공 응답 처리
    std::cout << "✅ 주문 성공적으로 처리됨!" << std::endl;
    
    order.orderId = JSONParser::extractString(response, "orderId");
    order.status = JSONParser::extractString(response, "status");
    order.quantity = JSONParser::extractDouble(response, "executedQty");
    order.success = true;
    
    std::cout << "주문 ID: " << order.orderId << std::endl;
    std::cout << "주문 상태: " << order.status << std::endl;
    std::cout << "실행 수량: " << std::fixed << std::setprecision(8) << order.quantity << " BTC" << std::endl;
    
    return order;
}

OrderResponse BinanceAPI::sellBitcoin(double quantity) {
    OrderResponse order;
    order.symbol = "BTCUSDT";
    order.side = "SELL";
    
    std::cout << "실제 판매 주문을 준비하는 중..." << std::endl;
    
    std::map<std::string, std::string> params;
    params["symbol"] = "BTCUSDT";
    params["side"] = "SELL";
    params["type"] = "MARKET";
    
    // 수량을 8자리 소수점으로 포맷팅
    std::stringstream ss;
    ss << std::fixed << std::setprecision(8) << quantity;
    params["quantity"] = ss.str();
    
    std::cout << "바이낸스 API 호출 중... (최대 15초 대기)" << std::endl;
    std::cout << "주문 정보: " << params["side"] << " " << params["quantity"] << " " << params["symbol"] << std::endl;
    
    // 재시도 메커니즘 (최대 2번 시도)
    std::string response;
    int max_retries = 2;
    
    for (int attempt = 1; attempt <= max_retries; attempt++) {
        if (attempt > 1) {
            std::cout << "재시도 중... (" << attempt << "/" << max_retries << ")" << std::endl;
        }
        
        response = makeRequest("/api/v3/order", "POST", params, true);
        
        // 타임아웃이 아닌 경우 재시도 중단
        if (response.find("타임아웃") == std::string::npos) {
            break;
        }
        
        if (attempt < max_retries) {
            std::cout << "⚠️  타임아웃 발생, 2초 후 재시도..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
    
    std::cout << "API 호출 완료!" << std::endl;
    
    // 응답이 비어있는지 확인
    if (response.empty()) {
        std::cout << "❌ API 응답이 비어있습니다." << std::endl;
        order.success = false;
        order.error = "API 응답 없음 - 네트워크 연결을 확인하세요";
        return order;
    }
    
    // 타임아웃 확인
    if (response.find("타임아웃") != std::string::npos) {
        std::cout << "❌ 주문 타임아웃 발생" << std::endl;
        order.success = false;
        order.error = "주문 타임아웃 - 바이낸스 서버 응답 지연";
        return order;
    }
    
    // 디버깅: 응답 출력
    std::cout << "API 응답 길이: " << response.length() << " 바이트" << std::endl;
    std::cout << "API 응답: " << response << std::endl;
    
    // 오류 응답 확인
    if (response.find("\"error\"") != std::string::npos || response.find("\"code\"") != std::string::npos) {
        order.success = false;
        
        // 오류 메시지 추출
        std::string error_msg = JSONParser::extractString(response, "msg");
        if (error_msg.empty()) {
            error_msg = JSONParser::extractString(response, "error");
        }
        
        // 오류 코드 확인
        std::string error_code = JSONParser::extractString(response, "code");
        if (!error_code.empty()) {
            error_msg = "오류 코드 " + error_code + ": " + error_msg;
        }
        
        if (error_msg.empty()) {
            error_msg = "API 오류: " + response;
        }
        
        order.error = error_msg;
        std::cout << "❌ 주문 실패: " << order.error << std::endl;
        return order;
    }
    
    // 성공 응답 처리
    std::cout << "✅ 주문 성공!" << std::endl;
    
    order.orderId = JSONParser::extractString(response, "orderId");
    order.status = JSONParser::extractString(response, "status");
    order.quantity = JSONParser::extractDouble(response, "executedQty");
    order.success = true;
    
    return order;
}

// === 선물거래 기능 구현 ===

FuturesAccountInfo BinanceAPI::getFuturesAccountInfo() {
    FuturesAccountInfo info;
    
    std::string response = makeFuturesRequest("/fapi/v2/account", "GET", {}, true);
    
    if (response.find("\"error\"") != std::string::npos) {
        info.success = false;
        info.error = JSONParser::extractString(response, "error");
        return info;
    }
    
    info.totalWalletBalance = JSONParser::extractDouble(response, "totalWalletBalance");
    info.totalUnrealizedPnl = JSONParser::extractDouble(response, "totalUnrealizedPnl");
    info.totalMarginBalance = JSONParser::extractDouble(response, "totalMarginBalance");
    info.availableBalance = JSONParser::extractDouble(response, "availableBalance");
    info.maxWithdrawAmount = JSONParser::extractDouble(response, "maxWithdrawAmount");
    info.success = true;
    
    return info;
}

std::vector<FuturesPosition> BinanceAPI::getFuturesPositions() {
    std::vector<FuturesPosition> positions;
    
    std::string response = makeFuturesRequest("/fapi/v2/positionRisk", "GET", {}, true);
    
    if (response.find("\"error\"") != std::string::npos) {
        FuturesPosition error_pos;
        error_pos.success = false;
        error_pos.error = JSONParser::extractString(response, "error");
        positions.push_back(error_pos);
        return positions;
    }
    
    // JSON 배열 파싱 (간단한 구현)
    size_t pos = 0;
    while ((pos = response.find("\"symbol\":", pos)) != std::string::npos) {
        FuturesPosition position;
        
        // 해당 포지션 데이터 추출
        size_t end_pos = response.find("}", pos);
        if (end_pos == std::string::npos) break;
        
        std::string position_data = response.substr(pos - 1, end_pos - pos + 2);
        
        position.symbol = JSONParser::extractString(position_data, "symbol");
        position.positionAmt = JSONParser::extractDouble(position_data, "positionAmt");
        position.entryPrice = JSONParser::extractDouble(position_data, "entryPrice");
        position.markPrice = JSONParser::extractDouble(position_data, "markPrice");
        position.unRealizedProfit = JSONParser::extractDouble(position_data, "unRealizedProfit");
        position.positionSide = JSONParser::extractString(position_data, "positionSide");
        position.leverage = static_cast<int>(JSONParser::extractDouble(position_data, "leverage"));
        
        // 수익률 계산
        if (position.entryPrice > 0) {
            position.percentage = ((position.markPrice - position.entryPrice) / position.entryPrice) * 100.0;
            if (position.positionAmt < 0) position.percentage *= -1; // 숏 포지션의 경우 반전
        } else {
            position.percentage = 0.0;
        }
        
        position.success = true;
        
        // 포지션이 있는 경우만 추가
        if (position.positionAmt != 0) {
            positions.push_back(position);
        }
        
        pos = end_pos;
    }
    
    return positions;
}

FuturesPosition BinanceAPI::getFuturesPosition(const std::string& symbol) {
    FuturesPosition position;
    position.symbol = symbol;
    
    std::map<std::string, std::string> params;
    params["symbol"] = symbol;
    
    std::string response = makeFuturesRequest("/fapi/v2/positionRisk", "GET", params, true);
    
    if (response.find("\"error\"") != std::string::npos) {
        position.success = false;
        position.error = JSONParser::extractString(response, "error");
        return position;
    }
    
    // 첫 번째 포지션 데이터 추출 (BOTH 모드의 경우)
    size_t pos = response.find("\"symbol\":\"" + symbol + "\"");
    if (pos != std::string::npos) {
        size_t end_pos = response.find("}", pos);
        if (end_pos != std::string::npos) {
            std::string position_data = response.substr(pos - 1, end_pos - pos + 2);
            
            position.positionAmt = JSONParser::extractDouble(position_data, "positionAmt");
            position.entryPrice = JSONParser::extractDouble(position_data, "entryPrice");
            position.markPrice = JSONParser::extractDouble(position_data, "markPrice");
            position.unRealizedProfit = JSONParser::extractDouble(position_data, "unRealizedProfit");
            position.positionSide = JSONParser::extractString(position_data, "positionSide");
            position.leverage = static_cast<int>(JSONParser::extractDouble(position_data, "leverage"));
            
            // 수익률 계산
            if (position.entryPrice > 0) {
                position.percentage = ((position.markPrice - position.entryPrice) / position.entryPrice) * 100.0;
                if (position.positionAmt < 0) position.percentage *= -1; // 숏 포지션의 경우 반전
            } else {
                position.percentage = 0.0;
            }
            
            position.success = true;
        }
    }
    
    return position;
}

bool BinanceAPI::setLeverage(const std::string& symbol, int leverage) {
    std::map<std::string, std::string> params;
    params["symbol"] = symbol;
    params["leverage"] = std::to_string(leverage);
    
    std::string response = makeFuturesRequest("/fapi/v1/leverage", "POST", params, true);
    
    if (response.find("\"error\"") != std::string::npos || response.find("\"code\"") != std::string::npos) {
        std::cout << "레버리지 설정 실패: " << response << std::endl;
        return false;
    }
    
    std::cout << "레버리지 설정 성공: " << leverage << "x" << std::endl;
    return true;
}

bool BinanceAPI::setMarginType(const std::string& symbol, const std::string& marginType) {
    std::map<std::string, std::string> params;
    params["symbol"] = symbol;
    params["marginType"] = marginType;
    
    std::string response = makeFuturesRequest("/fapi/v1/marginType", "POST", params, true);
    
    if (response.find("\"error\"") != std::string::npos || response.find("\"code\"") != std::string::npos) {
        std::cout << "마진 타입 설정 실패: " << response << std::endl;
        return false;
    }
    
    std::cout << "마진 타입 설정 성공: " << marginType << std::endl;
    return true;
}

FuturesOrderResponse BinanceAPI::futuresMarketOrder(const std::string& symbol, const std::string& side, 
                                                   double quantity, const std::string& positionSide) {
    // openLongPosition/openShortPosition에서 이미 검증했으므로 여기서는 검증하지 않음
    // 직접 호출되는 경우에만 검증
    
    FuturesOrderResponse order;
    order.symbol = symbol;
    order.side = side;
    order.positionSide = positionSide;
    order.type = "MARKET";
    
    std::map<std::string, std::string> params;
    params["symbol"] = symbol;
    params["side"] = side;
    params["type"] = "MARKET";
    params["positionSide"] = positionSide;
    
    // 수량을 적절한 소수점으로 포맷팅
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3) << quantity;
    params["quantity"] = ss.str();
    
    std::string response = makeFuturesRequest("/fapi/v1/order", "POST", params, true);
    
    if (response.find("\"error\"") != std::string::npos || response.find("\"code\"") != std::string::npos) {
        order.success = false;
        
        std::string error_msg = JSONParser::extractString(response, "msg");
        if (error_msg.empty()) {
            error_msg = JSONParser::extractString(response, "error");
        }
        
        std::string error_code = JSONParser::extractString(response, "code");
        if (!error_code.empty()) {
            error_msg = "오류 코드 " + error_code + ": " + error_msg;
        }
        
        order.error = error_msg;
        return order;
    }
    
    order.orderId = JSONParser::extractString(response, "orderId");
    order.clientOrderId = JSONParser::extractString(response, "clientOrderId");
    order.status = JSONParser::extractString(response, "status");
    order.quantity = JSONParser::extractDouble(response, "executedQty");
    order.price = JSONParser::extractDouble(response, "avgPrice");
    order.success = true;
    
    return order;
}

FuturesOrderResponse BinanceAPI::futuresLimitOrder(const std::string& symbol, const std::string& side, 
                                                  double quantity, double price, const std::string& positionSide) {
    // 최소주문수량 검증
    FuturesOrderValidation validation = validateFuturesOrderQuantity(symbol, quantity);
    
    if (!validation.error.empty()) {
        FuturesOrderResponse order;
        order.success = false;
        order.error = validation.error;
        return order;
    }
    
    if (!validation.warning.empty()) {
        std::cout << "\n" << validation.warning << std::endl;
        
        if (!validation.isValid) {
            std::cout << "\n❌ 주문을 실행할 수 없습니다. 수량을 " << validation.adjustedQuantity << "로 조정하시겠습니까? (y/N): ";
            std::string response;
            std::getline(std::cin, response);
            
            if (response != "y" && response != "Y") {
                FuturesOrderResponse order;
                order.success = false;
                order.error = "사용자가 주문을 취소했습니다";
                return order;
            }
            
            quantity = validation.adjustedQuantity;
        } else {
            std::cout << "\n🔧 수량이 자동으로 조정되었습니다: " << validation.adjustedQuantity << std::endl;
            quantity = validation.adjustedQuantity;
        }
    }
    
    FuturesOrderResponse order;
    order.symbol = symbol;
    order.side = side;
    order.positionSide = positionSide;
    order.type = "LIMIT";
    
    std::map<std::string, std::string> params;
    params["symbol"] = symbol;
    params["side"] = side;
    params["type"] = "LIMIT";
    params["positionSide"] = positionSide;
    params["timeInForce"] = "GTC";
    
    // 수량과 가격을 적절한 소수점으로 포맷팅
    std::stringstream ss_qty, ss_price;
    ss_qty << std::fixed << std::setprecision(3) << quantity;
    ss_price << std::fixed << std::setprecision(2) << price;
    params["quantity"] = ss_qty.str();
    params["price"] = ss_price.str();
    
    std::string response = makeFuturesRequest("/fapi/v1/order", "POST", params, true);
    
    if (response.find("\"error\"") != std::string::npos || response.find("\"code\"") != std::string::npos) {
        order.success = false;
        
        std::string error_msg = JSONParser::extractString(response, "msg");
        if (error_msg.empty()) {
            error_msg = JSONParser::extractString(response, "error");
        }
        
        std::string error_code = JSONParser::extractString(response, "code");
        if (!error_code.empty()) {
            error_msg = "오류 코드 " + error_code + ": " + error_msg;
        }
        
        order.error = error_msg;
        return order;
    }
    
    order.orderId = JSONParser::extractString(response, "orderId");
    order.clientOrderId = JSONParser::extractString(response, "clientOrderId");
    order.status = JSONParser::extractString(response, "status");
    order.quantity = JSONParser::extractDouble(response, "origQty");
    order.price = JSONParser::extractDouble(response, "price");
    order.success = true;
    
    return order;
}

FuturesOrderResponse BinanceAPI::openLongPosition(const std::string& symbol, double quantity) {
    std::cout << "롱 포지션 진입: " << quantity << " " << symbol << std::endl;
    
    // 최소주문수량 검증
    FuturesOrderValidation validation = validateFuturesOrderQuantity(symbol, quantity);
    
    if (!validation.error.empty()) {
        FuturesOrderResponse order;
        order.success = false;
        order.error = validation.error;
        return order;
    }
    
    if (!validation.warning.empty()) {
        std::cout << "\n" << validation.warning << std::endl;
        
        if (!validation.isValid) {
            std::cout << "\n❌ 주문을 실행할 수 없습니다. 수량을 " << validation.adjustedQuantity << "로 조정하시겠습니까? (y/N): ";
            std::string response;
            std::getline(std::cin, response);
            
            if (response != "y" && response != "Y") {
                FuturesOrderResponse order;
                order.success = false;
                order.error = "사용자가 주문을 취소했습니다";
                return order;
            }
            
            quantity = validation.adjustedQuantity;
        } else {
            std::cout << "\n🔧 수량이 자동으로 조정되었습니다: " << validation.adjustedQuantity << std::endl;
            quantity = validation.adjustedQuantity;
        }
    }
    
    return futuresMarketOrder(symbol, "BUY", quantity, "BOTH");
}

FuturesOrderResponse BinanceAPI::openShortPosition(const std::string& symbol, double quantity) {
    std::cout << "숏 포지션 진입: " << quantity << " " << symbol << std::endl;
    
    // 최소주문수량 검증
    FuturesOrderValidation validation = validateFuturesOrderQuantity(symbol, quantity);
    
    if (!validation.error.empty()) {
        FuturesOrderResponse order;
        order.success = false;
        order.error = validation.error;
        return order;
    }
    
    if (!validation.warning.empty()) {
        std::cout << "\n" << validation.warning << std::endl;
        
        if (!validation.isValid) {
            std::cout << "\n❌ 주문을 실행할 수 없습니다. 수량을 " << validation.adjustedQuantity << "로 조정하시겠습니까? (y/N): ";
            std::string response;
            std::getline(std::cin, response);
            
            if (response != "y" && response != "Y") {
                FuturesOrderResponse order;
                order.success = false;
                order.error = "사용자가 주문을 취소했습니다";
                return order;
            }
            
            quantity = validation.adjustedQuantity;
        } else {
            std::cout << "\n🔧 수량이 자동으로 조정되었습니다: " << validation.adjustedQuantity << std::endl;
            quantity = validation.adjustedQuantity;
        }
    }
    
    return futuresMarketOrder(symbol, "SELL", quantity, "BOTH");
}

FuturesOrderResponse BinanceAPI::closePosition(const std::string& symbol, const std::string& positionSide) {
    // 현재 포지션 조회
    FuturesPosition position = getFuturesPosition(symbol);
    
    if (!position.success) {
        FuturesOrderResponse order;
        order.success = false;
        order.error = "포지션 조회 실패: " + position.error;
        return order;
    }
    
    if (position.positionAmt == 0) {
        FuturesOrderResponse order;
        order.success = false;
        order.error = "종료할 포지션이 없습니다";
        return order;
    }
    
    // 포지션 방향에 따라 반대 주문 실행
    std::string side = (position.positionAmt > 0) ? "SELL" : "BUY";
    double quantity = std::abs(position.positionAmt);
    
    std::cout << "포지션 종료: " << side << " " << quantity << " " << symbol << std::endl;
    
    FuturesOrderResponse order = futuresMarketOrder(symbol, side, quantity, positionSide);
    order.reduceOnly = true;
    
    return order;
}

FuturesSymbolsResponse BinanceAPI::getFuturesSymbols() {
    FuturesSymbolsResponse response;
    
    std::string api_response = makeFuturesRequest("/fapi/v1/exchangeInfo", "GET", {}, false);
    
    std::cout << "API 응답 길이: " << api_response.length() << " 바이트" << std::endl;
    
    if (api_response.find("\"error\"") != std::string::npos) {
        response.success = false;
        response.error = JSONParser::extractString(api_response, "error");
        return response;
    }
    
    if (api_response.length() < 100) {
        response.success = false;
        response.error = "API 응답이 너무 짧습니다: " + api_response;
        return response;
    }
    
    // symbols 배열 찾기
    size_t symbols_start = api_response.find("\"symbols\":[");
    if (symbols_start == std::string::npos) {
        response.success = false;
        response.error = "심볼 정보를 찾을 수 없습니다";
        return response;
    }
    
    size_t pos = symbols_start + 11; // "symbols":[의 길이
    int total_symbols = 0;
    int usdt_symbols = 0;
    
    while (pos < api_response.length()) {
        size_t symbol_start = api_response.find("{", pos);
        if (symbol_start == std::string::npos) break;
        
        // 중첩된 JSON 구조를 고려한 올바른 끝 위치 찾기
        size_t symbol_end = symbol_start + 1;
        int brace_count = 1;
        while (symbol_end < api_response.length() && brace_count > 0) {
            if (api_response[symbol_end] == '{') {
                brace_count++;
            } else if (api_response[symbol_end] == '}') {
                brace_count--;
            }
            symbol_end++;
        }
        
        if (brace_count > 0) break; // 매칭되는 } 를 찾지 못함
        
        std::string symbol_data = api_response.substr(symbol_start, symbol_end - symbol_start);
        
        FuturesSymbolInfo symbol_info;
        symbol_info.symbol = JSONParser::extractString(symbol_data, "symbol");
        symbol_info.baseAsset = JSONParser::extractString(symbol_data, "baseAsset");
        symbol_info.quoteAsset = JSONParser::extractString(symbol_data, "quoteAsset");
        symbol_info.status = JSONParser::extractString(symbol_data, "status");
        
        total_symbols++;
        
        // USDT 페어이고 거래 가능한 상태인 것만 추가
        if (symbol_info.quoteAsset == "USDT" && symbol_info.status == "TRADING") {
            usdt_symbols++;
            // 필터 정보 추출
            size_t filters_start = symbol_data.find("\"filters\":[");
            if (filters_start != std::string::npos) {
                size_t filters_end = symbol_data.find("]", filters_start);
                std::string filters_data = symbol_data.substr(filters_start, filters_end - filters_start + 1);
                
                // LOT_SIZE 필터 찾기
                size_t lot_size_pos = filters_data.find("\"filterType\":\"LOT_SIZE\"");
                if (lot_size_pos != std::string::npos) {
                    size_t lot_size_end = filters_data.find("}", lot_size_pos);
                    std::string lot_size_data = filters_data.substr(lot_size_pos, lot_size_end - lot_size_pos + 1);
                    
                    symbol_info.minQty = JSONParser::extractDouble(lot_size_data, "minQty");
                    symbol_info.maxQty = JSONParser::extractDouble(lot_size_data, "maxQty");
                    symbol_info.stepSize = JSONParser::extractDouble(lot_size_data, "stepSize");
                }
                
                // MIN_NOTIONAL 필터 찾기
                size_t notional_pos = filters_data.find("\"filterType\":\"MIN_NOTIONAL\"");
                if (notional_pos != std::string::npos) {
                    size_t notional_end = filters_data.find("}", notional_pos);
                    std::string notional_data = filters_data.substr(notional_pos, notional_end - notional_pos + 1);
                    
                    symbol_info.minNotional = JSONParser::extractDouble(notional_data, "notional");
                }
            }
            
            // 정밀도 정보 추출
            symbol_info.pricePrecision = static_cast<int>(JSONParser::extractDouble(symbol_data, "pricePrecision"));
            symbol_info.quantityPrecision = static_cast<int>(JSONParser::extractDouble(symbol_data, "quantityPrecision"));
            
            response.symbols.push_back(symbol_info);
        }
        
        pos = symbol_end + 1;
        
        // 배열 끝 확인
        if (api_response.find("]", pos) < api_response.find("{", pos)) {
            break;
        }
    }
    
    std::cout << "파싱 완료: 전체 " << total_symbols << "개 심볼 중 " << usdt_symbols << "개 USDT 페어 발견" << std::endl;
    std::cout << "최종 결과: " << response.symbols.size() << "개 심볼 추가됨" << std::endl;
    
    response.success = true;
    return response;
}

BinanceAPI::FuturesOrderValidation BinanceAPI::validateFuturesOrderQuantity(const std::string& symbol, double quantity) {
    FuturesOrderValidation validation;
    validation.isValid = false;
    validation.adjustedQuantity = quantity;
    validation.minQuantity = 0.0;
    validation.minNotional = 0.0;
    validation.currentPrice = 0.0;
    
    // 1. 현재 가격 조회
    MarketPrice priceInfo = getCurrentPrice(symbol);
    if (!priceInfo.success) {
        validation.error = "가격 조회 실패: " + priceInfo.error;
        return validation;
    }
    validation.currentPrice = priceInfo.price;
    
    // 2. 심볼 정보 조회 (필터 정보 포함)
    FuturesSymbolsResponse symbolsResponse = getFuturesSymbols();
    if (!symbolsResponse.success) {
        validation.error = "심볼 정보 조회 실패: " + symbolsResponse.error;
        return validation;
    }
    
    // 3. 해당 심볼 찾기
    FuturesSymbolInfo symbolInfo;
    bool found = false;
    for (const auto& sym : symbolsResponse.symbols) {
        if (sym.symbol == symbol) {
            symbolInfo = sym;
            found = true;
            break;
        }
    }
    
    if (!found) {
        validation.error = "심볼 " + symbol + "을(를) 찾을 수 없습니다";
        return validation;
    }
    
    validation.minQuantity = symbolInfo.minQty;
    validation.minNotional = symbolInfo.minNotional;
    
    // 4. 바이낸스 공식 최소주문수량 계산
    // 최소주문수량 = MAX(LOT_SIZE.minQty, MIN_NOTIONAL.minNotional/price)
    double minQtyFromLotSize = symbolInfo.minQty;
    double minQtyFromNotional = symbolInfo.minNotional / validation.currentPrice;
    double actualMinQuantity = std::max(minQtyFromLotSize, minQtyFromNotional);
    
    // 5. 입력된 수량 검증
    if (quantity < actualMinQuantity) {
        validation.isValid = false;
        validation.adjustedQuantity = actualMinQuantity;
        
        // stepSize에 맞게 조정
        if (symbolInfo.stepSize > 0) {
            validation.adjustedQuantity = std::ceil(actualMinQuantity / symbolInfo.stepSize) * symbolInfo.stepSize;
        }
        
        std::stringstream ss;
        ss << "⚠️ 주문 수량 부족 경고!\n"
           << "📊 현재 입력: " << std::fixed << std::setprecision(6) << quantity << " " << symbol << "\n"
           << "💰 현재 가격: $" << std::fixed << std::setprecision(2) << validation.currentPrice << "\n"
           << "📏 LOT_SIZE 최소수량: " << std::fixed << std::setprecision(6) << minQtyFromLotSize << "\n"
           << "💵 MIN_NOTIONAL 최소수량: " << std::fixed << std::setprecision(6) << minQtyFromNotional << " (최소 $" << std::fixed << std::setprecision(2) << symbolInfo.minNotional << ")\n"
           << "✅ 실제 최소수량: " << std::fixed << std::setprecision(6) << actualMinQuantity << "\n"
           << "🔧 권장 조정수량: " << std::fixed << std::setprecision(6) << validation.adjustedQuantity << "\n"
           << "💡 조정 후 주문금액: $" << std::fixed << std::setprecision(2) << (validation.adjustedQuantity * validation.currentPrice);
        
        validation.warning = ss.str();
        return validation;
    }
    
    // 6. stepSize 검증 및 조정
    if (symbolInfo.stepSize > 0) {
        double remainder = fmod(quantity, symbolInfo.stepSize);
        if (remainder > 1e-8) { // 부동소수점 오차 고려
            validation.adjustedQuantity = quantity - remainder;
            if (validation.adjustedQuantity < actualMinQuantity) {
                validation.adjustedQuantity = std::ceil(quantity / symbolInfo.stepSize) * symbolInfo.stepSize;
            }
            
            std::stringstream ss;
            ss << "🔧 수량 조정 알림\n"
               << "📊 입력 수량: " << std::fixed << std::setprecision(6) << quantity << "\n"
               << "📏 stepSize: " << std::fixed << std::setprecision(6) << symbolInfo.stepSize << "\n"
               << "✅ 조정 수량: " << std::fixed << std::setprecision(6) << validation.adjustedQuantity << "\n"
               << "💰 조정 후 주문금액: $" << std::fixed << std::setprecision(2) << (validation.adjustedQuantity * validation.currentPrice);
            
            validation.warning = ss.str();
        }
    }
    
    // 7. 최종 검증
    double finalOrderValue = validation.adjustedQuantity * validation.currentPrice;
    if (finalOrderValue < symbolInfo.minNotional) {
        validation.isValid = false;
        validation.error = "최종 주문 금액이 최소 주문 금액보다 작습니다";
        return validation;
    }
    
    validation.isValid = true;
    return validation;
} 