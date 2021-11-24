#include "include/consts.h"
#include "include/helper.h"
#include <fstream>
#include <iostream>
#include <experimental/filesystem>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>


using namespace std;

vector<string> find_all_csv_files(){
    string path(PATH);
    string ext(EXTENSION);
    vector<string> csv_files;
    for (auto const &p : experimental::filesystem::recursive_directory_iterator(path))
    {
        if (p.path().extension() == ext) {
            csv_files.push_back(p.path().string());
        }
    }
    return csv_files;
}

void create_unnamed_pipe(int fd[]){
    if (pipe(fd) < 0){
        cerr << "Couldn't make new pipe!" << endl;
        exit(1);
    }
}

void create_all_map_processes(const vector<string>& files){
    for (int i = 0; i < files.size(); i++){
        int fd[2];
        create_unnamed_pipe(fd);
        create_new_map_process(fd, files[i], i);
    }
}


void create_new_map_process(int fd[], string file, int id){
    pid_t pid = fork();

    if (pid == 0){ // child process
        close(fd[WRITE]);
        string read_fd = to_string(fd[READ]), proc_id = to_string(id);
        const char* argv[] = {MAP, read_fd.c_str(), proc_id.c_str()};
        execv(argv[0], argv);
    }

    else{ // parent process
        close(fd[READ]);
        auto name = file.c_str();
        write(fd[WRITE], name, sizeof(name));
        close(fd[WRITE]);
    }

}

int create_reduce_process(const vector<string>& files){
    int fd[2];
    create_unnamed_pipe(fd);
    pid_t pid = fork();

    if (pid == 0){ // child process
        close(fd[READ]);
        string write_fd = to_string(fd[WRITE]), ids = to_string(files.size());
        const char* argv[] = {REDUCE, write_fd.c_str(), ids.c_str()};
        execv(argv[0], argv);
    }

    else{ // parent process
        close(fd[WRITE]);
    }

    return fd[READ];
}

vector<string> make_csv(const map<string, int>& final){
    string line_1 = "", line_2 = "";

}

void write_results(int fd){
    char result[MAX];
    read(fd, result, sizeof(result));
    auto final = decode_tokens(string(result));
    ostream file(string(OUTPUT) + string(EXTENSION));
    for(auto line : lines){
        file << line;
    }
}

int main(int argc, char const *argv[])
{
    auto files = find_all_csv_files();
    create_all_map_processes(files);
    auto fd = create_reduce_process(files);
    wait();
    write_results(fd);
    return 0;
}