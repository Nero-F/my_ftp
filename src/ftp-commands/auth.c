/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** auth ftp commands gestion 
*/

#include "my_ftp.h"

void user_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    printf("--> %s\n", arg);
    if (arg == NULL) {
        dprintf(client->fd, "530 Permission denied.\n");
        return;
    }
    if (client->has_auth & 3) {
        dprintf(client->fd, "530 Can't change from guest user.\n");
        return;
    }
    if (strcmp(arg, "Anonymous") == 0) { // Todo: CHecker si on il y'a plus qu'Anonymous
        client->has_auth |= (1 << 1) | (1 << 5);
        printf("HERE-----\n");
    } else
        client->has_auth |= (1 << 5);
    dprintf(client->fd, "331 Please specify the password.\n");
}

void pass_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    if (!client->has_auth & 5)
        dprintf(client->fd, "530 Login with USER first.\n");
    else if (!arg)
        client->has_auth |= (1 << 3);
    if (client->has_auth == 42) {
        dprintf(client->fd, "230 Login successful.\n");
        client->is_connected = TRUE;
    } else {
        dprintf(client->fd, "530 Login incorrect.\n");
        client->has_auth ^= client->has_auth;
    }
}