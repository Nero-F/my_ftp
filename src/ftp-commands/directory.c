/*
** EPITECH PROJECT, 2020
** ftp
** File description:
** directory managements ftp commands
*/

#include <dirent.h>
#include "my_ftp.h"

void pwd_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    dprintf(client->fd, "257 \"%s\"\n", client->path_dist);
}

char *copy_to_buff(char *arg, char *path_dist)
{
    char *buffer = malloc(sizeof(char) * (strlen(path_dist) + \
    strlen(arg) + 2));

    if (!buffer)
        return (NULL);
    buffer[strlen(path_dist) + strlen(arg) + 1] = '\0';
    strcpy(buffer, path_dist);
    strcat(buffer, "/");
    strcat(buffer, arg);
    return (buffer);
}

void cdup_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    char *buffer = copy_to_buff("/../", client->path_dist);
    char *path = NULL;

    if (!buffer)
        return;
    path = realpath(buffer, NULL);
    if (!path || chdir(path) == -1 ) {
        perror("realpath()  or chdir() -> cdup()");
        return;
    }
    client->path_dist = path;
    dprintf(client->fd, "200 Directory successfully changed.\r\n");
}

static char *get_rpath(char *arg, char *path_dist, int fd)
{
    char *path = NULL;
    char *buffer = arg[0] == '/' ? arg : copy_to_buff(arg, path_dist);

    if (!buffer)
        return (NULL);
    path = realpath(buffer, NULL);
    if (chdir(buffer) == -1) {
        perror("");
        dprintf(fd, "550 Failed to change directory\r\n");
        free(buffer);
        return (NULL);
    }
    buffer == arg ? 0 : free(buffer);
    if (!path) {
        perror("realpath() -> cdup()");
        return (NULL);
    }
    return (path);
}

void cwd_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    char *path = NULL;
    char *tmp = NULL;

    if  (!arg) {
        dprintf(client->fd, "550 Errors in parameters or Arguments\r\n");
        return;
    }
    if ((tmp = strstr(arg, "\r")) != NULL || \
        (tmp = strstr(arg, "\r\n")) != NULL || \
        (tmp = strstr(arg, "\n")) != NULL ) {
        *tmp = '\0';
    }
    if ((path = get_rpath(arg, client->path_dist, \
        client->fd)) == NULL) {
        return;
    }
    client->path_dist = path;
    dprintf(client->fd, "250 Directory successfully changed.\r\n");
}
