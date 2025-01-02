#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>

#define RED        "\033[0;31m"
#define GREEN      "\033[0;32m"
#define RESET      "\033[0m"
#define YELLOW     "\033[0;33m"

char LEADER_KEY = ':';

enum SESSION_STATUS {
    EXIT,
    COMMAND, // Start, TODO change name
    TABLE,
    CREATETABLE,
    CREATECOLUMN,
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

int setup() {
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

bool startsWith(const char *str, const char *prefix) {
    size_t prefix_len = strlen(prefix);
    size_t str_len = strlen(str);
    if (prefix_len > str_len) {
        return false;
    }
    return strncmp(str, prefix, prefix_len) == 0;
}

void print_prompt(int session_status, const char current_table[64]) {
    if (session_status == COMMAND) {
        printf("> ");
    } else if(session_status == TABLE) {
        printf("Table[%s%s%s]> ", YELLOW, current_table, RESET);
    } else if(session_status == CREATETABLE) {
        printf("CREATE_TABLE> ");
    } else if(session_status == CREATECOLUMN) {
        printf("CREATE_COLUMN[%s%s%s]> ", YELLOW, current_table, RESET);
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

int create_table(const char filename[64]) {
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

uint32_t update_column_count(FILE *file) {
    uint32_t num_columns = 0;
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("fseek error");
        return -1;
    }    

    // Read the number of columns from the start of the file
    fread(&num_columns, sizeof(uint32_t), 1, file);
    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("fseek error");
        return -1;
    }

    num_columns++;
    fwrite(&num_columns, sizeof(uint32_t), 1, file);

    return num_columns;
}

int create_column(const char table_name[64], const char column_name[64]) {
    // open table file
    // 64 char col name
    // 256 char value
    // 320 total
    // no idea if this makes sense
    //
    char path[256];
    strcpy(path, "tables/");
    strcat(path, table_name);
    FILE *file;
    file = fopen(path, "rb+");
    uint32_t num_columns = update_column_count(file);
    
    // seek
    fseek(file, sizeof(uint32_t) + (sizeof(char[64]) * num_columns), SEEK_SET);

    // write
    int results = fwrite(column_name, sizeof(char[64]), 1, file);
    // TODO check results?

    fclose(file);
    return 0;
}

char* build_command(char *command) {
    char *prefix = &LEADER_KEY;
    char *suffix = "\n";
    char *cmd = malloc(strlen(prefix) + strlen(command) + strlen(suffix) + 1);

    strcpy(cmd, prefix);
    strcat(cmd, command);
    strcat(cmd, suffix);
    return cmd;
}

void execute(int *session_status, char *command, char *current_table) {
    // TODO - check for <LEADER_KEY> if so check for list of commands outside of inputs from certain modes
    // like createtable mode, <LEADER_KEY>exit should have priority over the name input of the table
    // dont really care to consider if table name starts with <LEADER_KEY> just dont do that

    // TODO home command? 
    if (strcmp(command, build_command("exit")) == 0) {
        *session_status = EXIT;
    } else if (*session_status == TABLE) {
        if (strcmp(command, build_command("insert")) == 0) {
            *session_status = INSERT;
        }
    } else if (*session_status == CREATETABLE) {
        remove_newline(command);
        int t = create_table(command);
        if (t == 0) {
            printf(GREEN "Table created.\n" RESET);
            strcpy(current_table, command);
            *session_status = CREATECOLUMN;
        } else {
            // error
        }
    } else if (*session_status == CREATECOLUMN) {
        if (strcmp(command, build_command("done")) == 0) {
            memset(current_table, 0, sizeof(&current_table));
            *session_status = COMMAND;
        } else {
            remove_newline(command);
            int t = create_column(current_table, command);
            if (t == 0) {
                printf(GREEN "Column created.\n" RESET);
            }
        }
    } else if (startsWith(command, ":table")) { // TODO build with dynamic leader key
        char *table_name;
        strtok(command, " ");
        table_name = strtok(NULL, " ");
        if (table_name == NULL) { // TODO check if table exists
            printf("Provide existing table name\n");
        } else {
            remove_newline(table_name);
            strcpy(current_table, table_name);
            *session_status = TABLE;
        }
    } else if (strcmp(command, build_command("createtable")) == 0) {
        *session_status = CREATETABLE;
    }
}

int main() {
    setup();
    int session_status = COMMAND;
    char current_table[64];

	while (session_status) {
        print_prompt(session_status, current_table);
        char* input = read_input();
        execute(&session_status ,input, current_table);
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
//
// TODO:
// change session_status to mode
