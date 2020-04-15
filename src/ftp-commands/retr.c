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
        close(fd);
        return;
    }
    pid = fork();
    if (pid == -1) {
        printf("oupsi\n");
        return;
    } else if (pid == 0) {
        printf("children\n");
        if (sendfile(data_sock, fd, &offset, statbuf.st_size) == -1) {
            perror("sendfile() -> retrieve()");
            close(fd);
            return;
        }
        // client->in_use = TRUE;
        // while ((ret_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        //     write(data_sock, buffer, BUFFER_SIZE);
        // }
        shutdown(data_sock, SHUT_WR);
    } else {
        printf("buffer ====++> %s\n", buffer);
        dprintf(client->fd, buffer);
        dprintf(client->fd, "File send successfully");
        close(fd);
    }
}

void retr_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    int fd = client->data_sock->socket;
    printf("ahaha\n");
    if (client->can_transfer == FALSE) {
        dprintf(client->fd, "425 Use PORT or PASV first.\n");
        printf("sfefzef\n");
    } else {
        // fd = accept(client->data_sock->socket, \
        (struct sockaddr *)&client->data_sock->addr, &client->data_sock->addr_len);
        if (fd == -1) {
            printf("pas ouf\n");
            dprintf(client->fd, "425 problem with connection.\n");
            return;
        }
        printf("hohoho\n");
        retrieve(fd, arg, client);
    }
}