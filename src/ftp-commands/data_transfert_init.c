/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** initialisation of the data transfert protocole (active|passive)
*/

#define _GNU_SOURCE

#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include "my_ftp.h"

static char *disp_port(struct sockaddr_in *addr, struct in_addr c_ip)
{
    char *ip = inet_ntoa(c_ip);
    int port = ntohs(addr->sin_port);
    char *result = NULL;
    int index = 0;

    while (ip[index]) {
        if (ip[index] == '.') {
            ip[index] = ',';
        }
        index++;
    }
    asprintf(&result, "%s,%d,%d", ip, port / 256, port % 256);
    return (result);
}

void port_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    // int port = itoa(arg);
    dprintf(client->fd, "200 PORT okay.\n");
}

static void init_data_socket(socket_t *data_sock)
{
    data_sock->addr.sin_addr.s_addr = INADDR_ANY;
    data_sock->addr.sin_family = AF_INET;
    data_sock->addr.sin_port = htons(0); // supposely bind any availiable port
    data_sock->addr_len = sizeof(data_sock->addr);
    if (bind(data_sock->socket, \
        (struct sockaddr *)&data_sock->addr, \
        data_sock->addr_len) == -1) {
        fprintf(stderr, "bind() -> PASV", strerror(errno));
        return;
    }
    if (listen(data_sock->socket, 1) == -1) {
        fprintf(stderr, "listen() -> PASV", strerror(errno));
        return;
    }
    if (getsockname(data_sock->socket, (struct sockaddr *)&data_sock->addr, &data_sock->addr_len) == -1) {
        fprintf(stderr, "getsockname() -> PASV", strerror(errno));
        return;
    }
}
void pasv_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    client->data_sock = malloc(sizeof(socket_t));
    if (!client->data_sock) {
        fprintf(stderr, "malloc() -> PASV");
        return;
    }
    client->data_sock->socket = socket(AF_INET, SOCK_STREAM, \
    getprotobyname("TCP")->p_proto); 
    if (client->data_sock->socket == -1) {
        fprintf(stderr, "socket() -> PASV", strerror(errno));
        return;
    }
    init_data_socket(client->data_sock);
    dprintf(client->fd, "227 Entering Passive Mode %s.\n", \
    disp_port(&client->data_sock->addr, ftp->clients.addr.sin_addr));
    client->can_transfer = TRUE;
}