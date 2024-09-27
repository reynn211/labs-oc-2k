#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

int is_vowel(char c) {
    c = tolower(c);
    return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}

void trim_newlines(char *str) {
    int len = strlen(str);
    while (len > 0 && str[len-1] == '\n') {
        str[--len] = '\0';
    }
    if (len > 0) {
        str[len] = '\n';
        str[len+1] = '\0';
    }
}

void remove_vowels(char *str) {
    int i, j;
    for (i = 0, j = 0; str[i]; i++) {
        if (!is_vowel(str[i])) {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

int main(int argc, char *argv[]) {
    const int BUFFER_SIZE = 256;

    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("file open fail");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1)) > 1) {
        remove_vowels(buffer);
        trim_newlines(buffer);
        if (strlen(buffer) > 1) {
            write(fd, buffer, strlen(buffer));
        }
    }

    close(fd);
    return 0;
}