#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <memory>

class SecureStorage {
public:
    SecureStorage();
    ~SecureStorage();
    
    // 마스터 비밀번호로 세션 초기화 (24시간 유효)
    bool initializeSession(const std::string& master_password);
    
    // 세션이 유효한지 확인
    bool isSessionValid() const;
    
    // 암호화된 API 키 저장
    bool storeApiKeys(const std::string& api_key, const std::string& secret_key);
    
    // 암호화된 API 키 불러오기
    bool loadApiKeys(std::string& api_key, std::string& secret_key);
    
    // 저장된 키가 있는지 확인
    bool hasStoredKeys() const;
    
    // 세션 종료 및 메모리 정리
    void clearSession();
    
    // 저장된 키 파일 삭제
    bool deleteStoredKeys();

private:
    std::string config_file_path_;
    std::string session_key_;
    std::chrono::system_clock::time_point session_start_;
    static const int SESSION_DURATION_HOURS = 24;
    
    // 암호화/복호화 함수들
    std::string encrypt(const std::string& plaintext, const std::string& key);
    std::string decrypt(const std::string& ciphertext, const std::string& key);
    
    // 키 파생 함수
    std::string deriveKey(const std::string& password, const std::string& salt);
    
    // 랜덤 솔트 생성
    std::string generateSalt();
    
    // 안전한 메모리 정리
    void secureMemoryClear(std::string& str);
    
    // 설정 파일 경로 생성
    std::string getConfigFilePath();
}; 