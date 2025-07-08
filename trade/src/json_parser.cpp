#include "json_parser.h"
#include <algorithm>
#include <sstream>

std::string JSONParser::extractValue(const std::string& json, const std::string& key) {
    std::string search_key = "\"" + key + "\"";
    size_t key_pos = json.find(search_key);
    
    if (key_pos == std::string::npos) {
        return "";
    }
    
    size_t colon_pos = json.find(":", key_pos);
    if (colon_pos == std::string::npos) {
        return "";
    }
    
    size_t value_start = colon_pos + 1;
    while (value_start < json.length() && (json[value_start] == ' ' || json[value_start] == '\t')) {
        value_start++;
    }
    
    size_t value_end;
    if (json[value_start] == '"') {
        // String value
        value_start++; // Skip opening quote
        value_end = json.find('"', value_start);
    } else {
        // Number or boolean value
        value_end = json.find_first_of(",}", value_start);
    }
    
    if (value_end == std::string::npos) {
        value_end = json.length();
    }
    
    return trim(json.substr(value_start, value_end - value_start));
}

double JSONParser::extractDouble(const std::string& json, const std::string& key) {
    std::string value = extractValue(json, key);
    if (value.empty()) return 0.0;
    
    try {
        return std::stod(removeQuotes(value));
    } catch (...) {
        return 0.0;
    }
}

std::string JSONParser::extractString(const std::string& json, const std::string& key) {
    return removeQuotes(extractValue(json, key));
}

bool JSONParser::extractBool(const std::string& json, const std::string& key) {
    std::string value = removeQuotes(extractValue(json, key));
    return value == "true";
}

std::string JSONParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string JSONParser::removeQuotes(const std::string& str) {
    if (str.length() >= 2 && str.front() == '"' && str.back() == '"') {
        return str.substr(1, str.length() - 2);
    }
    return str;
} 