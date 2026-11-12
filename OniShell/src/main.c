
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define INPUT_SIZE 1024
#define ARGS_SIZE 64

/* OniShell - Custom Shell for OniOS
 * Version: 0.1.0
 */

void show_prompt(void) {
    char cwd[INPUT_SIZE];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf(
            "\033[1;34m┌──("
            "\033[1;32mjoojb"
            "\033[1;35m✧"
            "\033[1;32monios"
            "\033[1;34m)-["
            "\033[1;36m%s"
            "\033[1;34m]\n"
            "\033[1;34m└─"
            "\033[1;32m$ "
            "\033[0m",
            cwd
        );
    } else {
        printf("┌──(joojb✧onios)\n└─$ ");
    }

    fflush(stdout);
}

int execute_builtin(char **args) {
    if (args[0] == NULL)
        return 1;

    /* Exit */
    if (strcmp(args[0], "exit") == 0) {
        printf("Leaving OniShell...\n");
        exit(0);
    }

    /* Change directory */
    if (strcmp(args[0], "cd") == 0) {
        const char *path = args[1];

        if (path == NULL)
            path = getenv("HOME");

        if (path == NULL || chdir(path) != 0)
            perror("OniShell: cd");

        return 1;
    }

    /* Print working directory */
    if (strcmp(args[0], "pwd") == 0) {
        char cwd[INPUT_SIZE];

        if (getcwd(cwd, sizeof(cwd)))
            puts(cwd);

        return 1;
    }

    /* Help */
    if (strcmp(args[0], "help") == 0) {
        puts("\nOniShell 0.1.0");
        puts("Custom Shell for OniOS\n");

        puts("Built-in commands:");
        puts("  cd [path]  - Change directory");
        puts("  pwd        - Show current directory");
        puts("  help       - Show this help");
        puts("  exit       - Exit OniShell\n");

        return 1;
    }

    return 0;
}

void execute_command(char **args) {
    if (args[0] == NULL)
        return;

    if (execute_builtin(args))
        return;

    pid_t pid = fork();

    if (pid == 0) {
        execvp(args[0], args);

        perror("OniShell");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        int status;

        waitpid(pid, &status, 0);
    }
    else {
        perror("OniShell: fork");
    }
}

int main(void) {
    char input[INPUT_SIZE];

    printf("\033[1;35m");
    printf("  ___       _ ____  _          _ _ \n");
    printf(" / _ \\ _ __(_) ___|| |__   ___| | |\n");
    printf("| | | | '_ \\ \\___ \\| '_ \\ / _ \\ | |\n");
    printf("| |_| | | | | |__) | | | |  __/ | |\n");
    printf(" \\___/|_| |_|_|____/|_| |_|\\___|_|_|\n");
    printf("\033[0m");

    printf("OniShell 0.1.0 - Custom Shell for OniOS\n");
    printf("Type 'help' for available commands.\n\n");

    while (1) {
        show_prompt();

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }

        char *args[ARGS_SIZE];
        int argc = 0;

        char *token = strtok(input, " \t\n");

        while (token != NULL && argc < ARGS_SIZE - 1) {
            args[argc++] = token;
            token = strtok(NULL, " \t\n");
        }

        args[argc] = NULL;

        execute_command(args);
    }

    return 0;
}
