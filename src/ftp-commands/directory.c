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

static char *copy_to_buff(char *arg, char *path_dist)
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
    if (!path) {
        perror("realpath() -> cdup()");
        return;
    }
    client->path_dist = path;
    dprintf(client->fd, "200 Directory successfully changed.\n");
}

static char *get_rpath(char *arg, char *path_dist, DIR *dirp, int fd)
{
    char *path = NULL;
    char *buffer = copy_to_buff(arg, path_dist);

    if (!buffer)
        return (NULL);
    if ((dirp = opendir(buffer)) == NULL) {
        perror("");
        dprintf(fd, "550 Failed to change directory\n");
        free(buffer);
        return (NULL);
    }
    path = realpath(buffer, NULL);
    free(buffer);
    if (!path) {
        perror("realpath() -> cdup()");
        return (NULL);
    }
    return (path);
}

void cwd_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    char *path = NULL;
    DIR *dirp = NULL;

    if  (!arg) {
        dprintf(client->fd, "550 Failed to change directory\n");
        return;
    }
    if ((path = get_rpath(arg, client->path_dist, dirp, \
        client->fd)) == NULL) {
        closedir(dirp);
        return;
    }
    client->path_dist = path;
    closedir(dirp);
    dprintf(client->fd, "250 Directory successfully changed.\n");
}

void dele_f(ftp_t *ftp, char *arg, client_list_t *client)
{
}