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

#define BUFFER_SIZE 1024 

static void retrieve(int data_sock, char *path, client_list_t *client)
{
    struct stat statbuf = {0};
    int fd = open(path, O_RDONLY);
    char buffer[BUFFER_SIZE + 1];
    int ret_read = 0;
    pid_t pid = 0;
    off_t offset = 0;

    if (fd == -1 || fstat(fd, &statbuf) == -1) {
        dprintf(client->fd, "550 failed to open file.\n");
        perror("");
        close(fd);
        return;
    }
    dprintf(client->fd, "150 File status okay;\n");
                        // "about to open data connection\r\n");
    int fdf = accept(data_sock, (struct sockaddr *)&client->data_sock->addr, &client->data_sock->addr_len);
    if (fdf == -1) {
        perror("");
        return;
    }
    pid = fork();
    if (pid == -1) {
        return;
    } else if (pid == 0) {
        while ((ret_read = read(fd, buffer, BUFFER_SIZE)) != 0) {
            write(fdf, buffer, ret_read);
        }
        dprintf(client->fd, "226 Closing data connection\n");
        shutdown(fd, SHUT_WR);
        close(fd);
        close(data_sock);
    } else {
        printf("parent\n");
    }
}

void retr_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    int fd = client->data_sock->socket;

    if (client->can_transfer == FALSE) {
        dprintf(client->fd, "425 Use PORT or PASV first.\n");
    } else {
        if (fd == -1) {
            printf("pas ouf\n");
            dprintf(client->fd, "425 problem with connection.\n");
            return;
        }
        retrieve(fd, arg, client);
    }
}