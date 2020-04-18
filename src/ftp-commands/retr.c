/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** retrieve ftp command
*/

#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <fcntl.h>
#include "my_ftp.h"

int accept_data_sock(int fd, socket_t *data_sock)
{
    printf("foo\n");
    int fdf = accept(data_sock->socket, (struct sockaddr *)&data_sock->addr, \
                    &data_sock->addr_len);
    printf("bar\n");

    if (fdf == -1) {
        perror("");
        close(fd);
        return (84);
    }
    printf("gar\n");
    return (fdf);
}

static int verif_file(char *path, int data_sock, client_list_t *client)
{
    // char *rpath = NULL;
    int fd = 0;
    int i = 0;

    char *tmp = path[0] == '/' ? path : realpath(path, NULL);
    printf("FPATH -> %s\n", tmp);
    // rpath = copy_to_buff(path, client->path_dist);
    // printf("RPATH -> %s\n", rpath);
    if (!tmp || (fd = open(tmp, O_RDONLY)) == -1) {
        dprintf(client->fd, "550 failed to open file.\n");
        perror("");
        return (84);
    }
    // free(rpath);
    dprintf(client->fd, "150 File status okay;\n");
    return (fd);
}

static void retrieve(int data_sock, char *path, client_list_t *client)
{
    char buffer[BUFFER_SIZE + 1];
    int ret_read = 0;
    pid_t pid = 0;
    int fd = verif_file(path, data_sock, client);
    int fdf = 0;

    if (fd == 84 || (fdf = accept_data_sock(fd, client->data_sock)) == 84)
        return;
    if ((pid = fork()) == -1) {
        close(fd);
        perror("");
        return;
    } else if (pid == 0) {
        while ((ret_read = read(fd, buffer, BUFFER_SIZE)) != 0) {
            write(fdf, buffer, ret_read);
        }
        close(fd);
        close(data_sock);
        printf("yolo\n");
        dprintf(client->fd, "226 Closing data connection\r\n");
        client->can_transfer = FALSE;
    }
}

void retr_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    int fd = 0;
    char *tmp = NULL;

    if (client->can_transfer == FALSE || !client->data_sock) {
        dprintf(client->fd, "425 Use PORT or PASV first.\n");
    } else {
        if (!arg) {
            dprintf(client->fd, "501 Syntax error in arguments.\n");
            return;
        } else if ((tmp = strstr(arg, "\r")) != NULL || \
                    (tmp = strstr(arg, "\r\n")) != NULL || \
                    (tmp = strstr(arg, "\n")) != NULL )
            *tmp = '\0';
        fd = client->data_sock->socket;
        if (fd == -1) {
            dprintf(client->fd, "425 problem with connection.\n");
            return;
        }
        retrieve(fd, arg,client);
    }
}