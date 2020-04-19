/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** simple commands
*/

#include "my_ftp.h"

void dele_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    if (!arg)
        dprintf(client->fd, "501 Errors in parameters or Arguments\r\n");
    else {
        if (remove(arg) == -1) {
            perror("remove() -> dele_f()");
            dprintf(client->fd, "550 Requested action not taken. "
                    "File not found or is not a file\r\n");
        } else
            dprintf(client->fd, "250 Directory successfully removed.\r\n");
    }
}

void noop_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    printf("[%s]\n", arg);
    dprintf(client->fd, "200 NOOP okay.\n");
}

void quit_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    dprintf(client->fd, "221 Goodbye.\n");
    client->has_auth |= (1 << 6);
}

void help_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    dprintf(client->fd, "214 LOL.\n");
}