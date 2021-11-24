#include "include/helper.h"
#include "include/consts.h"
#include <sstream>

using namespace std;

map<string, int> update_map(map<string, int> in_map, string sample, int number){
    if (in_map.find(sample) != in_map.end()){
        in_map[sample] = number;
    }

    else{
        in_map[sample] += number;
    }

    return in_map;
}

vector<string> decode_token(string token){
    stringstream parser(token);
    string result;
    vector<string> results;
    while(getline(parser, result, ':')){
        results.push_back(result);
    }
    return results;
}

map<string, int> decode_tokens(string tokens){
    stringstream parser(tokens);
    string token;
    map<string, int> final;
    while (getline(parser, token, DELIMITER)){
        auto decoded = decode_token(token);
        final = update_map(final, decoded[0], stoi(decoded[1]))
    }
    return final;
}

string tokenize(map<string, int> in_map){
    string final = "";
    for(auto & _pair: in_map){
        final += _pair.first + ":" + to_string(_pair.second);
        final.push_back(DELIMITER);
    }
    if(final.size()){
        final[final.size() - 1] = '\n';
    }
    return final;
}