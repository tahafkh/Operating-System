#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/stat.h>
#include"const.h"

int available_port;

char buffer[BUFFER_SIZE] = {0};

int numbers_in_rooms[ROOMS] = {0};
int fds_in_rooms[ROOMS][PLAYERS_PER_ROOM] = {{0}};

int setup(int port) {
    const char message[] = "Server is running!\n";
    struct sockaddr_in address;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    
    listen(server_fd, MAX_CONNECTION);

    write(STDOUT, message, sizeof(message));
    return server_fd;
}

int accept_client(int server_fd) {
    char message[BUFFER_SIZE] = {0};
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr *) &client_address, (socklen_t*) &address_len);
    sprintf(message ,"New client connected. fd = %d\n", client_fd);
    write(STDOUT, message, sizeof(message));
    return client_fd;
}

void set_fds(fd_set* set, int server_fd){
    FD_ZERO(set);
    FD_SET(server_fd, set);
}

void close_fd(int client_fd){
    char message[BUFFER_SIZE] = {0};
    sprintf(message, "Client fd = %d closed\n", client_fd);
    write(STDOUT, message, sizeof(message));
    close(client_fd);
}

void make_new_room(int index){
    available_port++;
    char message[BUFFER_SIZE];
    for(int id = 0; id < PLAYERS_PER_ROOM; id++){
        sprintf(message, "%d%d", id, available_port);
        write(fds_in_rooms[index][id], message, sizeof(message));
        memset(message, 0, BUFFER_SIZE);
    }
    sprintf(message, "New room %d created\n", index);
    write(STDOUT, message, sizeof(message));
    numbers_in_rooms[index] = 0;
}

void add_client_to_room(int client_fd){
    char message[BUFFER_SIZE] = {0};
    int index = buffer[1] - '0';
    fds_in_rooms[index][numbers_in_rooms[index]] = client_fd;
    numbers_in_rooms[index]++;
    sprintf(message, "Client fd = %d added to the room %d\n", client_fd, index);
    write(STDOUT, message, sizeof(message));
    if(numbers_in_rooms[index] == PLAYERS_PER_ROOM){
        make_new_room(index);
    }
}

void submit_answers(){
    char message[BUFFER_SIZE] = {0};
    sprintf(message, "%c.txt", buffer[1]);
    int file_fd = open(message, O_APPEND | O_WRONLY);
    if(file_fd < 0){
        file_fd = open(message, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
    }
    write(file_fd, buffer+2, strlen(buffer+2));
    close(file_fd);
}

int main(int argc, char const *argv[]) {
    int server_fd, new_fd, max_fd;
    int server_port = atoi(argv[1]);
    available_port = server_port;
    fd_set master_set, working_set;

    server_fd = setup(server_port);
    FD_ZERO(&master_set);
    max_fd = server_fd;
    FD_SET(server_fd, &master_set);

    while(1){
        working_set = master_set;
        select(max_fd + 1, &working_set, NULL, NULL, NULL);
        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &working_set)) {           
                if (i == server_fd) {  
                    new_fd = accept_client(server_fd);
                    FD_SET(new_fd, &master_set);
                    if (new_fd > max_fd)
                        max_fd = new_fd;  
                }
                
                else{
                    memset(buffer, 0, BUFFER_SIZE); 
                    int bytes_received = read(i, buffer, BUFFER_SIZE);
                    
                    if (bytes_received == 0) { 
                        close_fd(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }

                    else{
                        if(buffer[0] == ROOM_REQ){
                            add_client_to_room(i);
                        }

                        else if(buffer[0] == SUBMIT_REQ){
                            submit_answers();
                        }
                    }                    
                }
            }
        }
    }

    return 0;
}