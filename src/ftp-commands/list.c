/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** list ftp command
*/

#include "my_ftp.h"

static int exec_listing(client_list_t *client, int fdf, FILE *fp)
{
    pid_t pid = 0;
    char buffer[BUFFER_SIZE + 1];
    ssize_t ret_read = 0;

    dprintf(client->fd, "150 File status okay;\n");
    if ((pid = fork()) == -1) {
        perror("");
        return (84);
    } else if (pid == 0) {
        while ((ret_read = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) != 0) {
            write(fdf, buffer, ret_read);
        }
        fclose(fp);
        dprintf(client->fd, "226 Closing data connection\n");
    }
    return (0);
}

FILE *get_real_arg(char *arg)
{
    char *rarg = NULL;
    char *tmp = NULL;
    FILE *fp = NULL;

    if ((tmp = strstr(arg, "\r")) != NULL || \
        (tmp = strstr(arg, "\r\n")) != NULL || \
        (tmp = strstr(arg, "\n")) != NULL )
            *tmp = '\0';
    rarg = realpath(arg, NULL);
    asprintf(&rarg, "ls -l %s", arg);
    fp = popen(rarg, "r");
    return (fp);
}

void list_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    FILE *fp = NULL;
    int fdf = 0;

    if (client->can_transfer == FALSE) {
        dprintf(client->fd, "425 Use PORT or PASV first.\n");
        return;
    }
    if (!arg)
        fp = popen("ls -l", "r");
    else
        fp = get_real_arg(arg);
    if (!fp || (fdf = accept(client->data_sock->socket, \
                (struct sockaddr *)&client->data_sock->addr, \
                &client->data_sock->addr_len)) == -1) {
        perror("");
        return;
    }
    exec_listing(client, fdf, fp);
}
