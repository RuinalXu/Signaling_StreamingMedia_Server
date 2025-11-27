#ifndef XML_PARSER_H
#define XML_PARSER_H

#include <string>
#include <tinyxml2.h>

#define XMLTITLE "<?xml version=\"1.0\"?>\n"

const int max_title = 64;

/**
 *  1. 插入根节点，指定根节点的名称
 *  2. 在指定的节点下再插入子节点并设置节点名称和相应的值
 *  3. 添加xml的申明部分并返回整个xml的char*数据
 */
class XmlParser {
private:
    tinyxml2::XMLDocument* m_document;
    tinyxml2::XMLElement* m_rootElement;
    char* m_xmlTitle;
public:
    XmlParser();
    ~XmlParser();

    tinyxml2::XMLElement* addRootNode(const char* rootName);
    tinyxml2::XMLElement* insertSubNode(tinyxml2::XMLElement* parentNode, const char* itemName, const char* value);
    void setNodeAttributes(tinyxml2::XMLElement* node, char* attrName, char* attrValue);
    void getXmlData(char* xmlBuf);
};

#endif
