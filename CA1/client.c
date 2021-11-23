#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/ioctl.h>
#include"const.h"

int last_question_id = IDLE, last_answer_id = IDLE;

char buffer[BUFFER_SIZE] = {0};

int qa = 0;
char qas[PLAYERS_PER_ROOM][BUFFER_SIZE];

struct sockaddr_in bc_address;

int connect_to_server(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    
    const char error[] = "Error in connecting\n";
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(port); 
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        write(STDOUT, error, sizeof(error));
    }

    return fd;
}

char send_room(int server_fd){
    char message[BUFFER_SIZE] = "Choose your room in number: (0)Computer Engineering, (1)Eletrical Engineering, (2)Civil Engineering and (3)Mechanical Engineering\n";
    write(STDOUT, message, sizeof(message));
    memset(message, 0, BUFFER_SIZE);

    read(STDIN, message, BUFFER_SIZE);
    sprintf(buffer, "%c%c", ROOM_REQ, message[0]);
    write(server_fd, buffer, sizeof(buffer));
    memset(buffer, 0, BUFFER_SIZE);
    return message[0];
}

int recieve_id(int server_fd){
    read(server_fd, buffer, BUFFER_SIZE);
    return buffer[0] - '0';
}

int recieve_port(){
    int port = atoi(buffer + 1);
    memset(buffer, 0, BUFFER_SIZE);
    return port;
}

int setup(int port) {
    int fd, broadcast = 1, opt = 1;
    char buffer[1024] = {0};

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    bc_address.sin_family = AF_INET; 
    bc_address.sin_port = htons(port); 
    bc_address.sin_addr.s_addr = inet_addr("192.168.1.255");

    bind(fd, (struct sockaddr *)&bc_address, sizeof(bc_address));
    

    return fd;
}

void ask_question(int id, int fd){
    char message[BUFFER_SIZE] = "Your Turn! Write your question:\n";
    write(STDOUT, message, sizeof(message));
    memset(message, 0, BUFFER_SIZE);

    read(STDIN, message, BUFFER_SIZE);
    sprintf(buffer, "%c%d: %s", QUESTION_REQ, id,message);
    sendto(fd, buffer, strlen(buffer), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
    last_question_id = id;
}

int choose_best_answer(int fd){
    char message[BUFFER_SIZE] = "Choose best answer: (1) or (2)?\n";
    write(STDOUT, message, sizeof(message));
    memset(message, 0, BUFFER_SIZE);
    memset(buffer, 0, BUFFER_SIZE);

    read(STDIN, message, BUFFER_SIZE);
    int index = message[0] - '0';
    sprintf(buffer, "Best answer: %s", qas[index]);
    sendto(fd, buffer, strlen(buffer), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
    return index;
}

void exit_room(int fd){
    char message[BUFFER_SIZE] = "Goodbye! See you later\n";
    sendto(fd, message, strlen(message), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
}

void submit_results(int index, int server_fd, char room){
    char message[BUFFER_SIZE] = {0};
    char result[3*BUFFER_SIZE] = {0};
    sprintf(message, "*%s", qas[index]);
    strcpy(qas[index], message);
    sprintf(result, "%c%c%s%s%s", SUBMIT_REQ, room, qas[0], qas[1], qas[2]);
    write(server_fd, result, sizeof(result));
}

void answer_question(int id, int fd){
    char message[BUFFER_SIZE] = "Your turn! Answer the question:\n";
    write(STDOUT, message, sizeof(message));
    memset(message, 0, BUFFER_SIZE);
    memset(buffer, 0, BUFFER_SIZE);

    alarm(MAXIMUM_DELAY);
    int read_bytes = read(STDIN, message, BUFFER_SIZE);
    alarm(0);

    if(read_bytes == -1){
        memset(message, 0, BUFFER_SIZE);
        strcpy(message, "NO ANSWER!\n");
    }

    sprintf(buffer, "%c%d: %s", ANSWER_REQ, id, message);
    sendto(fd, buffer, strlen(buffer), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
}

int next_answer_id(){
    return (last_question_id == 0 && last_answer_id == IDLE) ? 1 :\
            (last_question_id == 0 && last_answer_id == 1) ? 2 : \
            (last_question_id == 1 && last_answer_id == IDLE) ? 0 :\
            (last_question_id == 1 && last_answer_id == 0) ? 2 :\
            (last_question_id == 2 && last_answer_id == IDLE) ? 0 :\
            (last_question_id == 2 && last_answer_id == 0) ? 1 : IDLE;
}

void alarm_handler(int signal) {
    char message[BUFFER_SIZE] = "Your turn has passed! Too late\n";
    write(STDOUT, message, sizeof(message));
}

int main(int argc, char const *argv[]) {
    signal(SIGALRM, alarm_handler);
    siginterrupt(SIGALRM, 1);
    int server_fd, id, port, fd;
    server_fd = connect_to_server(atoi(argv[1]));
    char room = send_room(server_fd);
    
    id = recieve_id(server_fd);
    port = recieve_port();
    sprintf(buffer, "Welcome! Your id is %d. IDs start at 0 and end at 3, so make sure you know your turn.\n", id);
    write(STDOUT, buffer, sizeof(buffer));
    memset(buffer, 0, BUFFER_SIZE);

    fd = setup(port);
    if(id == 0){
        ask_question(id, fd);
    }

    while(1){
        memset(buffer, 0, BUFFER_SIZE);
        read(fd, buffer, BUFFER_SIZE);
        write(STDOUT, buffer, sizeof(buffer));

        if(buffer[0] == GOODBYE_REQ){
            break;
        }
        if(id == last_question_id && buffer[0] != BEST_REQ){
            strcpy(qas[qa], buffer);
            qa++;
            if(qa == PLAYERS_PER_ROOM){
                submit_results(choose_best_answer(fd), server_fd, room);
            }
            continue;
        }

        if(id == last_question_id && buffer[0] == BEST_REQ){
            if(id + 1 == PLAYERS_PER_ROOM){
                exit_room(fd);
            }
            else{
                last_question_id = IDLE;
            }
        }
        else{
            if(buffer[0] == BEST_REQ){
                if(last_question_id + 1 == id){
                    ask_question(id, fd);
                }
                continue;
            }
            if(buffer[0] == QUESTION_REQ){
                last_question_id = buffer[1] - '0';
                last_answer_id = IDLE;
                if(id == next_answer_id()){
                    answer_question(id, fd);
                }
                continue;
            }
            if(buffer[0] == ANSWER_REQ){
                last_answer_id = buffer[1] - '0';
                if(id == next_answer_id()){
                    answer_question(id, fd);
                }
                continue;
            }
        }
    }


    return 0;
}