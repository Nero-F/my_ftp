/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** passive mod
*/

#include "my_ftp.h"

void pasv_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    client->data_sock = setup_data_socket();
    if (!client->data_sock)
        return;
    init_data_socket(client->data_sock, 0, ftp->local_ip);
    dprintf(client->fd, "227 Entering Passive Mode %s.\n", \
    disp_port(&client->data_sock->addr, ftp->local_ip));
    client->can_transfer = TRUE;
}