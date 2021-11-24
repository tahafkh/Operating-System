#include "includes/consts.h"
#include "includes/helper.h"
#include <fstream>
#include <iostream>
#include <experimental/filesystem>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

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

void create_new_map_process(int fd[], string file, int id){
    auto name = file.c_str();
    
    pid_t pid = fork();
    if (pid == 0){ // child process
        close(fd[WRITE]);
        char name[MAX];
        read(fd[READ], name, sizeof(name));
        close(fd[READ]);
        string proc_id = to_string(id);
        char temp[MAX];
        strcpy(temp, proc_id.c_str());
        char* argv[] = {MAP, name, temp, NULL};
        execv(argv[0], argv);
    }

    else{ // parent process
        close(fd[READ]);
        auto name = file.c_str();
        write(fd[WRITE], name, file.size() + 1);
        close(fd[WRITE]);
    }
}

void create_all_map_processes(const vector<string>& files){
    for (int i = 0; i < files.size(); i++){
        int fd[2];
        create_unnamed_pipe(fd);
        create_new_map_process(fd, files[i], i);
    }
}

int create_reduce_process(const vector<string>& files){
    int fd[2];
    create_unnamed_pipe(fd);
    pid_t pid = fork();

    if (pid == 0){ // child process
        close(fd[READ]);
        string write_fd = to_string(fd[WRITE]), ids = to_string(files.size());
        char temp1[MAX], temp2[MAX];
        strcpy(temp1, write_fd.c_str());
        strcpy(temp2, ids.c_str());
        char* argv[] = {REDUCE, temp1, temp2, NULL};
        execv(argv[0], argv);
    }

    else{ // parent process
        close(fd[WRITE]);
    }

    return fd[READ];
}


void write_results(int fd){
    char result[MAX];
    read(fd, result, sizeof(result));
    close(fd);
    auto final = decode_tokens(string(result));
    ofstream file(string(OUTPUT) + string(EXTENSION));
    file << tokenize(final);
}

int main(int argc, char const *argv[])
{
    auto files = find_all_csv_files();
    create_all_map_processes(files);
    auto fd = create_reduce_process(files);
    while(wait(NULL) > 0);
    write_results(fd);
    return 0;
}