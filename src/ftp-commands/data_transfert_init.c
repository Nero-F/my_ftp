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

char *disp_port(struct sockaddr_in *addr, char *ip)
{
    int port = ntohs(addr->sin_port);
    char *result = NULL;
    int index = 0;

    while (ip[index]) {
        if (ip[index] == '.') {
            ip[index] = ',';
        }
        index++;
    }
    printf("port ---> %d\n", port);
    asprintf(&result, "%s,%d,%d", ip, port / 256, port % 256);
    return (result);
}

socket_t *setup_data_socket(void)
{
    socket_t *data_sock = malloc(sizeof(socket_t));
    if (!data_sock) {
        fprintf(stderr, "malloc() -> PASV");
        return (NULL);
    }
    data_sock->socket = socket(AF_INET, SOCK_STREAM, \
    getprotobyname("TCP")->p_proto); 
    if (data_sock->socket == -1) {
        fprintf(stderr, "socket() -> PASV", strerror(errno));
        return (NULL);
    }
    return (data_sock);
}

void init_data_socket(socket_t *data_sock, int port, char *ip)
{
    data_sock->addr.sin_addr.s_addr = inet_addr(ip);
    data_sock->addr.sin_family = AF_INET;
    data_sock->addr.sin_port = htons(port);
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
    if (getsockname(data_sock->socket, (struct sockaddr *)&data_sock->addr, \
        &data_sock->addr_len) == -1) {
        fprintf(stderr, "getsockname() -> PASV", strerror(errno));
        return;
    }
}

void get_local_ip(ftp_t *ftp)
{
    int socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr = {0};

    if (socketfd == -1) {
        perror("");
        return;
    }
    addr.sin_addr.s_addr = INADDR_LOOPBACK;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ftp->port);
    int addr_len = sizeof(addr);
    if (connect(socketfd, (struct sockaddr *)&addr, addr_len) == -1) {
        perror("");
        return;
    }
    if (getsockname(socketfd, (struct  sockaddr *)&addr, &addr_len) == -1) {
        perror("");
        return;
    }
    ftp->local_ip = inet_ntoa(addr.sin_addr);
}