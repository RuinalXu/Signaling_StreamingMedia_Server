#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include <string>
#include <string.h>
#include <fstream>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <vector>

class ConfigReader{
private:
    std::string m_filename;
    std::string m_section;
    static int load_ini_file(const char* file, char* buff, int* file_size);
    static int newline(char c);
    static int end_of_string(char c);
    static int left_barce(char c);
    static int right_brace(char c);
    static int parse_file(const char* section, const char* key, const char* buf, int* sec_s, int* sec_e,
		int* key_s, int* key_e, int* value_s, int* value_e);
public:
    ConfigReader(const std::string& filename);
    ~ConfigReader(void);
    void setSection(const std::string& section);
    std::string readStr(const std::string& key) const;
    int readInt(const std::string& key) const;
    static int read_profile_string(const char* section, const char* key, char* value, int size, const char* file);
    static int read_profile_int(const char* section, const char* key, const char* file);
};

#endif