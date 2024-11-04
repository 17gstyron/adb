#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum SESSION_STATUS {
    EXIT,
    COMMAND,
    INSERT,
    SELECT,
    UPDATE,
    DELETE
};

char* read_input() {
    size_t characters;
    char *buffer = NULL;
    size_t bufsize = 32;

    characters = getline(&buffer, &bufsize, stdin);
    return buffer;
}

void print_prompt(int session_status) {
    if (session_status == COMMAND) {
        printf("db> ");
    } else if(session_status == INSERT) {
        printf("INSERT> ");
    } else if(session_status == SELECT) {
        printf("SELECT> ");
    } else if(session_status == UPDATE) {
        printf("UPDATE> ");
    } else if(session_status == DELETE) {
        printf("DELETE> ");
    }
}

void execute(int* session_status, char* command) {
    // check that command is valid
    // run it if so
    if (strcmp(command, "exit\n") == 0) { 
        *session_status = EXIT;
    } else if (strcmp(command, "insert\n") == 0) { 
        *session_status = INSERT;
    }

}

int main() {
    int session_status = COMMAND;

	while (session_status) {
        print_prompt(session_status);
        char* input = read_input();
        execute(&session_status ,input);
        free(input);
    }
}

// Insert
// Select
// Update
// Delete
//
// Every row has unique id
