#include "includes/consts.h"
#include "includes/helper.h"
#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

map<string, int> update_results(int index, map<string, int> final){
    string fifo_name = string(FIFO) + to_string(index);
    int fd = open(fifo_name.c_str(), O_RDONLY);
    char result[MAX];
    read(fd, result, sizeof(result));
    auto current_result = decode_tokens(string(result));
    for(auto & _pair: current_result){
        final = update_map(final, _pair.first, _pair.second);
    }
    return final;
}

map<string, int> combine_all_results(int ids){
    map<string, int> final;
    for(int i = 0; i < ids; i++){
        final = update_results(i, final);
    }
    return final;
}

void write_final_results(map<string, int> final, int fd){
    auto _token = tokenize(final);
    auto token = _token.c_str();
    write(fd, token, sizeof(token));
    close(fd);
}

int main(int argc, char const *argv[]){
    cout << "na" << endl;
    int write_fd = atoi(argv[1]), ids = atoi(argv[2]);
    auto final = combine_all_results(ids);

}