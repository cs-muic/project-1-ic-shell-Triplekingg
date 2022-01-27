#include <stdio.h>
#include "string.h"
#include "stdlib.h"

char cmd[100]; //command
char *args[32]; //commands and args split into array of strings
char prev[100]; //stores previous command

void startShell();

void getCommand();

void splitCmd();

void echo();

void systemExit();

void systemExit(){
    int num = atoi(args[1]);
    printf("bye\n");
    exit(num);
}

void echo() {
    int length = sizeof(args) / sizeof(args[0]);
    for (int i = 1; i < length; i++) {
        if (args[i] == NULL) break;
        printf("%s ", args[i]);
    }
    printf("\n");
}

void getCommand() {
    memset(cmd,0,sizeof(cmd));
    printf("icsh $\t");
    fgets(cmd, 100, stdin);
    int len = strlen(cmd);
    cmd[len - 1] = '\0';
}

void startShell() {
    while (1) {
        getCommand();
        if (strcmp("!!", cmd)) {
            memset(prev,0,sizeof(prev));
            memcpy(prev, cmd, strlen(cmd));
        }
        splitCmd();
        if (!strcmp("", cmd)) {
            continue;
        }
        else if (!strcmp("exit", cmd)) {
            systemExit();
            break;
        }
        else if (!strcmp("echo", args[0])) {
            echo(args);
        }
        else if (!strcmp("!!", args[0])) {
            printf("%s\n", prev);
        }
        else{
            printf("bad command\n");
        }
    }
}


void splitCmd() {
    int i = 0;
    char *p = strtok(cmd, " ");
    while (p != NULL) {
        args[i++] = p;
        p = strtok(NULL, " ");
    }
}


int main() {
    printf("Starting IC shell\n");
    startShell();
    return 0;
}


