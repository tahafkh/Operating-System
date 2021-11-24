#ifndef OPERATING_SYSTEM_HELPER_H
#define OPERATING_SYSTEM_HELPER_H

#include <map>
#include <string>
#include <vector>
#include <sstream>

std::map<std::string, int> update_map(std::map<std::string, int> in_map, std::string sample, int number);
std::map<std::string, int> decode_tokens(std::string tokens);
std::string tokenize(std::map<std::string, int> in_map);

#endif
