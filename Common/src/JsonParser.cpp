#include "utils/JsonParser.h"
#include "log/LogManager.h"

JsonParser::JsonParser(string s)
:m_str(s) {}

JsonParser::JsonParser(Json::Value j)
:m_json(j) {}

bool JsonParser::toJson(Json::Value& j) {
    bool bret = false;
    Json::CharReaderBuilder builder;
    Json::CharReaderBuilder::strictMode(&builder.settings_);
    builder["collectComents"] = true;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    JSONCPP_STRING errs;
    bret = reader->parse(m_str.data(), m_str.data() + m_str.size(), &j, &errs);
    if (!bret || !errs.empty()) {
        LOG(ERROR) << "json parse error:" << errs.c_str();
    }
    return bret;
}

string JsonParser::toString() {
    Json::StreamWriterBuilder builder;
    char* indent = "";
    builder["indentation"] = indent;
    return Json::writeString(builder, m_json);
}
