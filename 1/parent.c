#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    const int BUFFER_SIZE = 256;
    int pipe1[2], pipe2[2];
    pid_t child1, child2;
    char filename1[BUFFER_SIZE], filename2[BUFFER_SIZE];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        error("pipe fail");
    }

    printf("filename for child1: ");
    scanf("%s", filename1);
    printf("filename for child2: ");
    scanf("%s", filename2);

    if ((child1 = fork()) == -1) {
        error("fork fail");
    } else if (child1 == 0) {
        close(pipe1[1]);
        dup2(pipe1[0], STDIN_FILENO);
        execlp("./child", "child", filename1, NULL);
        error("execlp fail");
    }

    if ((child2 = fork()) == -1) {
        error("fork fail");
    } else if (child2 == 0) {
        close(pipe2[1]);
        dup2(pipe2[0], STDIN_FILENO);
        execlp("./child", "child", filename2, NULL);
        error("execlp fail");
    }

    close(pipe1[0]);
    close(pipe2[0]);

    char buffer[BUFFER_SIZE];

    while (getchar() != '\n');
    memset(buffer, 0, sizeof(buffer));

    while (fgets(buffer, sizeof(buffer), stdin)) {
        if (buffer[strlen(buffer) - 1] == '\n' && strlen(buffer) == 1) { // exit
            write(pipe1[1], buffer, strlen(buffer));
            write(pipe2[1], buffer, strlen(buffer));
            break;
        } else if (strlen(buffer) > 10) {
            write(pipe2[1], buffer, strlen(buffer));
        } else {
            write(pipe1[1], buffer, strlen(buffer));
        }
    }

    close(pipe1[1]);
    close(pipe2[1]);

    wait(NULL);
    wait(NULL);

    return 0;
}