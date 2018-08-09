
#ifndef CMD_H
#define CMD_H

typedef struct {
    char *command;
    char *description;
    void (*func)(int argc, char **argv);
} command_table_t;

extern const command_table_t cmdlist[];
void cmd_i(void);

#endif // #ifndef CMD_H

