#include "include/consts.h"
#include "include/helper.h"
#include <stdio.h>
#include <fcntl.h>
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

map<string, int> decode_csv(int fd){
    char name[MAX];
    read(fd, name, sizeof(name));
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
    mkfifo(name, 0777);
    auto _token = tokenize(result);
    auto token = _token.c_str();
    int fd = open(name, O_WRONLY);
    write(fd, token, sizeof(token));
    close(fd);
}

int main(int argc, char const *argv[]){
    int read_fd = atoi(read_fd);
    auto result = decode_csv(read_fd);
    string fifo_name = string(FIFO) + string(argv[2]);
    write_results(result, fifo_name);
    exit();
}