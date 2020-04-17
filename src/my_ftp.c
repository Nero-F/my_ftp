/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** ftp server
*/

#include "my_ftp.h"

static int socket_assignation(struct sockaddr_in *addr, int sockfd, int port)
{
	addr->sin_addr.s_addr = INADDR_ANY;
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *)addr, sizeof(struct sockaddr)) == -1) {
        close(sockfd);
        return (84);
    }
    return (0);
}

static int set_queue(int sockfd)
{
    if (listen(sockfd, MAX_CLIENT) == -1) {
        close(sockfd);
        return (84);
    }
    return (0);
}

static int server_start(ftp_t *ftp) // listens all the connextions on the given port
{
    ftp->server.socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
    int ret = 0;
    
    if (ftp->server.socket == -1 || !addr || \
        socket_assignation(addr, ftp->server.socket, ftp->port) != 0 || \
        set_queue(ftp->server.socket) != 0) {
        perror("");
        return (84);
    }
    printf("--> %d\n", ftp->port);
    ret = server_pi(ftp);
    close(ftp->server.socket);
    return (ret);
}

static void init_ftp(ftp_t *ftp, int port, char *path)
{
    ftp->buffer = NULL;
    get_local_ip(ftp);
    ftp->port = port;
    ftp->path = realpath(path, NULL);
    ftp->clients.socket = 0;
    ftp->clients.addr_len = sizeof(ftp->clients.addr);
    memset(&ftp->clients.addr, 0, ftp->clients.addr_len);
    ftp->cli_list = NULL;
    ftp->connexion = 0;
}

int my_ftp(char *port_str, char *path)
{
    ftp_t *ftp = malloc(sizeof(ftp_t));
    int port = 0; 

    if (!ftp || (port = verif_nb(port_str, "Port")) == -1)
        return (84);
    if (chdir(path) == -1) {
        perror("");
        return (84);
    }
    init_ftp(ftp, port, path);
    server_start(ftp);
    return (0);
}