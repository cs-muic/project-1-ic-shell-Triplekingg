#include <stdio.h>
#include "string.h"

char cmd[100];


void getCommand(){
    printf("icsh $\t");
    fgets(cmd,100,stdin);
    int len = strlen(cmd);
    cmd[len-1] = '\0';
}

void startShell(){
    while(1){
        getCommand();
        if(!strcmp(NULL, cmd)){
            continue;
        }
        if(!strcmp("exit", cmd)){
            printf("bye\n");
            break;
        }
    }
}

int main() {
    printf("Starting IC shell\n");
    startShell();
    return 0;
}


