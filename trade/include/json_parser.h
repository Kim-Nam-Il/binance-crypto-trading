#pragma once

#include <string>
#include <map>

class JSONParser {
public:
    static std::string extractValue(const std::string& json, const std::string& key);
    static double extractDouble(const std::string& json, const std::string& key);
    static std::string extractString(const std::string& json, const std::string& key);
    static bool extractBool(const std::string& json, const std::string& key);
    
private:
    static std::string trim(const std::string& str);
    static std::string removeQuotes(const std::string& str);
}; 