/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** list ftp command
*/

#include "my_ftp.h"

void list_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    FILE *fp = popen("ls -l", "r");
    char buffer[BUFFER_SIZE + 1];
    ssize_t ret_read = 0;
    pid_t pid = 0;
    int fdf = 0;

    if (!fp || (fdf = accept(client->data_sock->socket, \
                (struct sockaddr *)&client->data_sock->addr, \
                &client->data_sock->addr_len)) == -1) {
        perror("");
        return;
    }
    dprintf(client->fd, "150 File status okay;\n");
    if ((pid = fork()) == -1) {
        perror("");
        return;
    } else if (pid == 0) {
        while ((ret_read = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) != 0) {
            write(fdf, buffer, ret_read);
        }
        fclose(fp);
        dprintf(client->fd, "226 Closing data connection\n");
    }
}
