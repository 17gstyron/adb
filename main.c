#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

enum SESSION_STATUS {
    EXIT,
    COMMAND, // Start, TODO change name
    TABLE,
    CREATETABLE,
    USETABLE,
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

int init() {
    const char *table_dir_name = "tables";
    if (mkdir(table_dir_name, 0755) == -1) {
        perror("Error creating directory");
        return 1;
    }
    return 0;
}

void remove_newline(char *command) {
    // Find the position of the newline character, if it exists
    size_t len = strcspn(command, "\n");
    
    // If a newline was found, replace it with a null terminator
    if (command[len] == '\n') {
        command[len] = '\0';
    }
}

void print_prompt(int session_status) {
    if (session_status == COMMAND) {
        printf("db> ");
    } else if(session_status == TABLE) {
        printf("TABLE> ");
    } else if(session_status == CREATETABLE) {
        printf("CREATE_TABLE> ");
    }  else if(session_status == INSERT) {
        printf("INSERT> ");
    } else if(session_status == SELECT) {
        printf("SELECT> ");
    } else if(session_status == UPDATE) {
        printf("UPDATE> ");
    } else if(session_status == DELETE) {
        printf("DELETE> ");
    }
}

int create_table(const char *filename) {
    char path[256];
    strcpy(path, "tables/");
    strcat(path, filename);

    FILE *file;
    file = fopen(path, "w");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    fclose(file);
    return 0;
}

void execute(int *session_status, char *command) {
  // check that command is valid
  // run it if so
  if (strcmp(command, "exit\n") == 0) {
    *session_status = EXIT;
  } else if (*command == 27) { // escape + enter to return to start
    *session_status = COMMAND;
  } else if (*session_status == TABLE) {
        if (strcmp(command, "insert\n") == 0) {
            *session_status = INSERT;
        }
  } else if (*session_status == CREATETABLE) {
    remove_newline(command);
    int t = create_table(command);
    if (t) {
        *session_status = COMMAND;
    }
  } else if (strcmp(command, "table\n") == 0) {
    *session_status = TABLE;
  } else if (strcmp(command, "createtable\n") == 0) {
    *session_status = CREATETABLE;
  }
}

int main() {
    init();
    int session_status = COMMAND;
	while (session_status) {
        print_prompt(session_status);
        char* input = read_input();
        execute(&session_status ,input);
        free(input);
    }
}

// File for table names
// > TABLE table_name
// table_name> INSERT
// col1> input for col1
// col2> input for col2

// Insert
// Select
// Update
// Delete
//
// Every row has unique id
//
