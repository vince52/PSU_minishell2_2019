/*
** EPITECH PROJECT, 2019
** minishell.c
** File description:
** Minishell
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "include/my.h"
#include "include/env.h"
#include "include/error.h"

char *my_cat(char *str1, char *str2);

int char_in_array(char c, char *str);

int minishell_execute3(char **argv, char *final_path, char **env)
{
    struct stat st;
    int exit_code = 0;

    if (stat(argv[0], &st) == 0 && char_in_array('/', argv[0])) {
        if ((st.st_mode & S_IXUSR) && (st.st_mode & __S_IFREG)) {
            pid_t childpid = fork();
            if (childpid == 0)
                exit_code = execve(final_path, argv, env);
            else {
                wait(&exit_code);
                exit_code = WEXITSTATUS(exit_code);
            }
        }
    }
    return errno;
}

int minishell_execute2(char **argv, char *path, char **env)
{
    int exit_code = 0;

    char *newenviron[] = { NULL };
    char *final_path;
    errno = 0;
    if (argv[0][0] == '/') 
        final_path = argv[0];
    else {
        final_path = my_cat(my_cat(path, "/"), argv[0]);
    }
    exit_code = minishell_execute3(argv, final_path, env);
    return exit_code;
}

int minishell_execute(char **argv, char *path, char **env)
{
    char **tmp = searsh_in_env(env, "PATH");
    int continu = 127;
    char *str1 = my_strdup(argv[0]);
    int exit_code = 0;
    errno = 0;
    if (minishell_execute2(argv, path, env) != 0) {
        for (int i = 0; tmp[i] != NULL && continu != 0; i++) {
            errno = 0;
            my_strcat(tmp[i], "/");
            my_strcat(tmp[i], str1);
            argv[0] = my_strdup(tmp[i]);
            continu = minishell_execute2(argv, path, env);
        }
    }
    if (errno != 0)
            my_error_handle("", argv[0], errno);
    return errno;
}