#include "includes/consts.h"
#include "includes/helper.h"
#include <stdio.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

map<string, int> decode_line(map<string, int> decoded, string line){
    stringstream decode(line);
    string word;
    while(getline(decode, word, DELIMITER)){
        decoded = update_map(decoded, word, 1);
    }
    return decoded;
}

map<string, int> decode_csv(const char name[]){
    string _name = name, line;
    ifstream file(_name);
    map<string, int> results;
    while(getline(file, line)){
        results = decode_line(results, line);
    }
    return results;
}

void write_results(map<string, int> result, string fifo_name){
    auto name = fifo_name.c_str();
    if(mkfifo(name, 0666) == -1 && errno != EEXIST){
        cerr << "Couldn't create fifo!" << endl;
        return;
    }
    auto _token = tokenize(result);
    auto token = _token.c_str();
    int fd = open(fifo_name.c_str(), O_WRONLY);
    write(fd, token, MAX);
    close(fd);
}

int main(int argc, char const *argv[]){
    auto result = decode_csv(argv[1]);
    string fifo_name = FIFO + string(argv[2]);
    write_results(result, fifo_name);
    return 0;
}