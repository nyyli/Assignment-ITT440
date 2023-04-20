#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define maxchild 10
#define maxmessage 100

int pipefd[maxchild][2];
pid_t children[maxchild];

void interrupt(int sig) {
    printf("\nError! Interrupt signal.\n");
    exit(0);
}

void parent() {
    char message[maxmessage];
    printf("Please enter a message to send to the child processes: ");
    fgets(message, maxmessage, stdin);
    for (int i = 0; i < maxchild; i++) {
        write(pipefd[i][1], message, strlen(message)+1);
    }
}

void child(int id) {
    char message[maxmessage];
    close(pipefd[id][1]);
    read(pipefd[id][0], message, maxmessage);
    printf("Child %d received message from parent %d : %s", id, id, message);
    close(pipefd[id][0]);
    exit(0);
}

int main() {
    signal(SIGINT, interrupt);
    for (int i = 0; i < maxchild; i++) {
        if (pipe(pipefd[i]) == -1) {
            perror("pipe");
            exit(1);
        }
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            child(i);
        } else {
            children[i] = pid;
        }
    }
    parent();
    for (int i = 0; i < maxchild; i++) {
        waitpid(children[i], NULL, 0);
    }
    return 0;
}
