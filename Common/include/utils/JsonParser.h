#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <json/json.h>
#include <string>

using namespace std;

/**
 *  JSON数据的序列化和反序列化
 */
class JsonParser {
private:
    string m_str;
    Json::Value m_json;
public:
    JsonParser(string s);
    JsonParser(Json::Value j);
    bool toJson(Json::Value& j);
    string toString();
};

#endif
