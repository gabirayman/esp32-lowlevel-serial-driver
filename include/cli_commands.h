#ifndef CLI_COMMANDS_H
#define CLI_COMMANDS_H

typedef void (*cmd_handler_t)(void); // Define what a command function looks like

typedef struct {
    const char* name;
    cmd_handler_t handler;
    const char* help_text; // for describing the command in a help menu
} cli_cmd_t;

// Declare the command list (defined in cli_commands.c)
extern cli_cmd_t cli_commands[];


#endif