/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** client gestion
*/

#include <sys/select.h>
#include "my_ftp.h"

int new_clients(ftp_t *ftp)
{
    printf("someone's trying to connect\n");
    if ((ftp->clients.socket = accept(ftp->server.socket, \
        (struct sockaddr *)&ftp->clients.addr, \
        &ftp->clients.addr_len)) == -1) {
        perror("");
        return (84);
    }
    if (ftp->connexion == MAX_CLIENT) {
        dprintf(ftp->clients.socket, "Occupied.\n");
        return (0);
    }
    ftp->connexion++;
    add_node(&ftp->cli_list, ftp->clients.socket, ftp->path, ftp->port);
    dprintf(ftp->clients.socket, "220\n");
    FD_SET(ftp->clients.socket, &ftp->rfds);
    return (0);
}

void disconnect_client(int fd, fd_set *rfds, client_list_t **list, \
int *connexion)
{
    close(fd);
    FD_CLR(fd, rfds);
    rm_at_filedesc(list, fd);
    --(*connexion);
}

void check_disconnect(ftp_t *ftp, client_list_t *client)
{
    if (client->has_auth == NO_AUTH_DISCONNECT ||
        client->has_auth == WRONG_USER_DISCONNECT ||
        client->has_auth == WRIGHT_USER_DISCONNECT ||
        client->has_auth == POST_AUTH_DISCONNECT)
        disconnect_client(client->fd, &ftp->rfds, &ftp->cli_list, \
        &ftp->connexion);
}