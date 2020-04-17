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
    char *buffer[BUFFER_SIZE + 1];
    int ret_read = 0;
    pid_t pid = 0;
    int fdf = 0;

    if (!fp || (fdf = accept(client->data_sock->socket, (struct sockaddr *)&client->data_sock->addr, &client->data_sock->addr_len) == -1) || \
        (pid = fork()) == -1) {
        perror("");
    }
    if (pid == 0) {
        while ((ret_read = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) {
            fwrite(buffer, sizeof(char), ret_read, fp);
        }
    } else {
        fclose(fp);
    }
    fclose(fp);
}
