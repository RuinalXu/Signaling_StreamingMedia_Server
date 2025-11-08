#include "config_reader.h"

#define MAX_INI_FILE_SIZE 1024*10

ConfigReader::ConfigReader(const std::string& fileName) {
	m_filename = fileName;
}

ConfigReader::~ConfigReader(){}

void ConfigReader::setSection(const std::string& section) {
    m_section = section;
}

std::string ConfigReader::readStr(const std::string& key) const {
    const int buff_size = 1024;
	std::vector<char> buff(buff_size);
    read_profile_string(m_section.c_str(), key.c_str(), buff.data(), sizeof(buff), m_filename.c_str());
    for (int i = 0; i < buff_size; i++) {
        if (buff[i] == '\t' || buff[i] == ' ') {
            buff[i] = '\0';
        }
        if (buff[i] == '\0') {
            break;
        }
    }
    return std::string(buff.data());
}

int ConfigReader::readInt(const std::string& key) const {
    return read_profile_int(m_section.c_str(), key.c_str(), m_filename.c_str());
}

int ConfigReader::load_ini_file(const char* file, char* buff, int* file_size) {
    FILE *in = NULL;
	int i = 0;
	*file_size = 0;
	assert(file != NULL);
	assert(buff != NULL);
	in = fopen(file, "r");
	if (NULL == in) {
		return 0;
	}
	buff[i] = fgetc(in);
	while (buff[i] != (char)EOF) {
		i++;
		assert(i < MAX_INI_FILE_SIZE); 
		buff[i] = fgetc(in);
	}
	buff[i] = '\0';
	*file_size = i;
	fclose(in);
	return 1;
}

int ConfigReader::read_profile_string(const char* section, const char* key, char* value, int size, const char* file) {
    char buff[MAX_INI_FILE_SIZE] = { 0 };
	int file_size;
	int sec_s, sec_e, key_s, key_e, value_s, value_e;
	assert(section != NULL && strlen(section));
	assert(key != NULL && strlen(key));
	assert(value != NULL);
	assert(size > 0);
	assert(file != NULL &&strlen(key));
	if (!load_ini_file(file, buff, &file_size)) {
		return 0;
	}
	if (!parse_file(section, key, buff, &sec_s, &sec_e, &key_s, &key_e, &value_s, &value_e)) {
		return 0; //not find the key
	} else {
		int cpcount = value_e - value_s;
		if (size - 1 < cpcount) {
			cpcount = size - 1;
		}
		//memset(value, 0, size);
		memcpy(value, buff + value_s, cpcount);
		value[cpcount] = '\0';
		return 1;
	}
}

int ConfigReader::read_profile_int(const char* section, const char* key, const char* file) {
	char value[32] = { 0 };
	if (!read_profile_string(section, key, value, sizeof(value), file)) {
		return -1;
	} else {
		return atoi(value);
	}
}

int ConfigReader::newline(char c) {
	return ('\n' == c || '\r' == c) ? 1 : 0;
}

int ConfigReader::end_of_string(char c) {
	return '\0' == c ? 1 : 0;
}

int ConfigReader::left_barce(char c) {
	return '[' == c ? 1 : 0;
}

int ConfigReader::right_brace(char c) {
	return ']' == c ? 1 : 0;
}

int ConfigReader::parse_file(const char *section, const char *key, const char *buf, int *sec_s, int *sec_e,
	    int *key_s, int *key_e, int *value_s, int *value_e) {
	const char* p = buf;
	int i = 0;

	assert(buf != NULL);
	assert(section != NULL && strlen(section));
	assert(key != NULL && strlen(key));

	*sec_e = *sec_s = *key_e = *key_s = *value_s = *value_e = -1;

	while (!end_of_string(p[i])) {
		//find the section
		if ((0 == i || newline(p[i - 1])) && left_barce(p[i])) {
			int section_start = i + 1;
			//find the ']'
			do {
				i++;
			} while (!right_brace(p[i]) && !end_of_string(p[i]));
			if (0 == strncasecmp(p+section_start,section, i-section_start)) {
				int newline_start = 0;
				i++;
				//over space char after ']'
				while (isspace(p[i])) {
					i++;
				}
				//find the section
				*sec_s = section_start;
				*sec_e = i;
				while (!(newline(p[i - 1]) && left_barce(p[i])) && !end_of_string(p[i])) {
					int j = 0;
					//get a new line
					newline_start = i;
					while (!newline(p[i]) && !end_of_string(p[i])) {
						i++;
					}
					j = newline_start;
					int valid = j;
					if ('#' != p[j]) {
						while (j < i && p[j] != '=') {
							j++;
							if (' ' != p[j] && '\t' != p[j] && '=' != p[j]) {
                                valid = j;
                            }
							if ('=' == p[j]) {
								if(strncasecmp(key,p+newline_start,valid-newline_start+1)==0){
									*key_s = newline_start;
									*key_e = j - 1;
									valid = j + 1;
									while (' ' == p[valid] || '\t' == p[valid])
										valid++;
									*value_s = valid;
									*value_e = i;
									return 1;
								}
							}
						}
					}

					i++;
				}
			}
		}else{
			i++;
		}
	}
	return 0;
}
