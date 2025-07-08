#include "secure_storage.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>
#include <filesystem>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#else
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#endif

SecureStorage::SecureStorage() {
    config_file_path_ = getConfigFilePath();
}

SecureStorage::~SecureStorage() {
    clearSession();
}

bool SecureStorage::initializeSession(const std::string& master_password) {
    if (master_password.empty()) {
        return false;
    }
    
    // 기존 세션 정리
    clearSession();
    
    // 새 세션 키 생성
    session_key_ = deriveKey(master_password, "binance_session_salt_2024");
    session_start_ = std::chrono::system_clock::now();
    
    return true;
}

bool SecureStorage::isSessionValid() const {
    if (session_key_.empty()) {
        return false;
    }
    
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::hours>(now - session_start_);
    
    return duration.count() < SESSION_DURATION_HOURS;
}

bool SecureStorage::storeApiKeys(const std::string& api_key, const std::string& secret_key) {
    if (!isSessionValid()) {
        std::cout << "세션이 만료되었습니다. 다시 로그인하세요." << std::endl;
        return false;
    }
    
    try {
        // 솔트 생성
        std::string salt = generateSalt();
        
        // API 키들을 JSON 형태로 결합
        std::string combined_keys = "{\"api_key\":\"" + api_key + "\",\"secret_key\":\"" + secret_key + "\"}";
        
        // 암호화
        std::string encrypted_data = encrypt(combined_keys, session_key_);
        
        // 파일에 저장 (솔트 + 암호화된 데이터)
        std::ofstream file(config_file_path_, std::ios::binary);
        if (!file.is_open()) {
            std::cout << "설정 파일을 생성할 수 없습니다." << std::endl;
            return false;
        }
        
        file << salt << "\n" << encrypted_data;
        file.close();
        
        // 파일 권한 설정 (소유자만 읽기/쓰기)
#ifndef _WIN32
        std::filesystem::permissions(config_file_path_, 
                                   std::filesystem::perms::owner_read | 
                                   std::filesystem::perms::owner_write);
#endif
        
        std::cout << "API 키가 안전하게 저장되었습니다." << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "키 저장 중 오류 발생: " << e.what() << std::endl;
        return false;
    }
}

bool SecureStorage::loadApiKeys(std::string& api_key, std::string& secret_key) {
    if (!isSessionValid()) {
        std::cout << "세션이 만료되었습니다. 다시 로그인하세요." << std::endl;
        return false;
    }
    
    if (!hasStoredKeys()) {
        return false;
    }
    
    try {
        std::ifstream file(config_file_path_, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        std::string salt, encrypted_data;
        std::getline(file, salt);
        std::getline(file, encrypted_data);
        file.close();
        
        // 복호화
        std::string decrypted_data = decrypt(encrypted_data, session_key_);
        
        // JSON 파싱 (간단한 파싱)
        size_t api_start = decrypted_data.find("\"api_key\":\"") + 11;
        size_t api_end = decrypted_data.find("\"", api_start);
        size_t secret_start = decrypted_data.find("\"secret_key\":\"") + 14;
        size_t secret_end = decrypted_data.find("\"", secret_start);
        
        if (api_start != std::string::npos && api_end != std::string::npos &&
            secret_start != std::string::npos && secret_end != std::string::npos) {
            api_key = decrypted_data.substr(api_start, api_end - api_start);
            secret_key = decrypted_data.substr(secret_start, secret_end - secret_start);
            
            // 복호화된 데이터 안전하게 제거
            secureMemoryClear(decrypted_data);
            
            return true;
        }
        
        return false;
        
    } catch (const std::exception& e) {
        std::cout << "키 불러오기 중 오류 발생: " << e.what() << std::endl;
        return false;
    }
}

bool SecureStorage::hasStoredKeys() const {
    return std::filesystem::exists(config_file_path_);
}

void SecureStorage::clearSession() {
    secureMemoryClear(session_key_);
    session_start_ = std::chrono::system_clock::time_point{};
}

bool SecureStorage::deleteStoredKeys() {
    try {
        if (std::filesystem::exists(config_file_path_)) {
            std::filesystem::remove(config_file_path_);
            std::cout << "저장된 키 파일이 삭제되었습니다." << std::endl;
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        std::cout << "키 파일 삭제 중 오류 발생: " << e.what() << std::endl;
        return false;
    }
}

std::string SecureStorage::encrypt(const std::string& plaintext, const std::string& key) {
    // 간단한 XOR 암호화 (실제 운영환경에서는 AES 사용 권장)
    std::string result = plaintext;
    for (size_t i = 0; i < result.length(); ++i) {
        result[i] ^= key[i % key.length()];
    }
    
    // Base64 인코딩 (간단한 구현)
    std::string encoded;
    const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    for (size_t i = 0; i < result.length(); i += 3) {
        uint32_t tmp = 0;
        for (int j = 0; j < 3 && i + j < result.length(); ++j) {
            tmp |= (static_cast<uint8_t>(result[i + j]) << (8 * (2 - j)));
        }
        
        for (int j = 0; j < 4; ++j) {
            if (i * 4 / 3 + j < (result.length() + 2) / 3 * 4) {
                encoded += chars[(tmp >> (6 * (3 - j))) & 0x3F];
            } else {
                encoded += '=';
            }
        }
    }
    
    return encoded;
}

std::string SecureStorage::decrypt(const std::string& ciphertext, const std::string& key) {
    // Base64 디코딩
    std::string decoded;
    std::vector<int> T(128, -1);
    for (int i = 0; i < 64; i++) {
        T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
    }
    
    for (size_t i = 0; i < ciphertext.length(); i += 4) {
        uint32_t tmp = 0;
        for (int j = 0; j < 4; ++j) {
            if (i + j < ciphertext.length() && ciphertext[i + j] != '=') {
                tmp |= (T[static_cast<int>(ciphertext[i + j])] << (6 * (3 - j)));
            }
        }
        
        for (int j = 0; j < 3; ++j) {
            if (i * 3 / 4 + j < ciphertext.length() * 3 / 4) {
                decoded += static_cast<char>((tmp >> (8 * (2 - j))) & 0xFF);
            }
        }
    }
    
    // XOR 복호화
    std::string result = decoded;
    for (size_t i = 0; i < result.length(); ++i) {
        result[i] ^= key[i % key.length()];
    }
    
    return result;
}

std::string SecureStorage::deriveKey(const std::string& password, const std::string& salt) {
    // 간단한 키 파생 (실제로는 PBKDF2 사용 권장)
    std::string combined = password + salt;
    std::hash<std::string> hasher;
    size_t hash_value = hasher(combined);
    
    std::stringstream ss;
    ss << std::hex << hash_value;
    std::string result = ss.str();
    
    // 32바이트로 확장
    while (result.length() < 32) {
        result += result;
    }
    
    return result.substr(0, 32);
}

std::string SecureStorage::generateSalt() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    std::string salt;
    for (int i = 0; i < 16; ++i) {
        salt += static_cast<char>(dis(gen));
    }
    
    return salt;
}

void SecureStorage::secureMemoryClear(std::string& str) {
    if (!str.empty()) {
        std::fill(str.begin(), str.end(), 0);
        str.clear();
    }
}

std::string SecureStorage::getConfigFilePath() {
    std::string config_dir;
    
    // Docker 환경에서 데이터 경로 환경 변수 확인
    const char* data_path = getenv("BINANCE_DATA_PATH");
    if (data_path) {
        config_dir = data_path;
    } else {
        // 기본 경로 설정
        std::string home_dir;
        
#ifdef _WIN32
        char* userprofile = nullptr;
        size_t len = 0;
        if (_dupenv_s(&userprofile, &len, "USERPROFILE") == 0 && userprofile != nullptr) {
            home_dir = userprofile;
            free(userprofile);
        } else {
            home_dir = ".";
        }
#else
        const char* home = getenv("HOME");
        home_dir = home ? home : ".";
#endif
        
        config_dir = home_dir + "/.binance_trader";
    }
    
    // 디렉토리 생성
    try {
        std::filesystem::create_directories(config_dir);
    } catch (const std::exception& e) {
        std::cout << "설정 디렉토리 생성 실패: " << e.what() << std::endl;
        return "./binance_keys.enc";
    }
    
    return config_dir + "/keys.enc";
} 