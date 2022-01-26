#include <stdio.h>
#include "string.h"

char cmd[100]; //command
char *args[32]; //commands and args split into array of strings

void startShell();
void getCommand();
void splitCmd();
void echo();

void echo(){
    int length = sizeof(args)/sizeof(args[0]);
    for (int i = 1; i < length; i++) {
        if(args[i]==NULL) break;
        printf("%s ", args[i]);
    }
}

void getCommand(){
    printf("\nicsh $\t");
    fgets(cmd,100,stdin);
    int len = strlen(cmd);
    cmd[len-1] = '\0';
}

void startShell(){
    while(1){
        getCommand();
        if(!strcmp("", cmd)){
            continue;
        }
        if(!strcmp("exit", cmd)){
            printf("bye\n");
            break;
        }
        splitCmd();
        if(!strcmp("echo", args[0])){
            echo(args);
        }
    }
}



void splitCmd(){
    int i = 0;
    char *p = strtok (cmd, " ");
    while (p != NULL)
    {
        args[i++] = p;
        p = strtok (NULL, " ");
    }
}


int main() {
    printf("Starting IC shell\n");
    startShell();
    return 0;
}


