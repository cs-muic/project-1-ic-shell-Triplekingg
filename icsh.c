#include <stdio.h>
#include "string.h"

char cmd[100]; //command
char *args[32]; //commands and args split into array of strings

void startShell();

void getCommand();

void splitCmd();


void getCommand(){
    printf("icsh $\t");
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


