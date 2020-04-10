/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** server protocole interpreter
*/

#include <sys/types.h>
#include <sys/select.h>
#include "my_ftp.h"
#include <errno.h>

const ftp_cmd_t ftp_cmd[] = {
    {"NOOP", NO_ARGUMENT, 200},
    {"CDUP", NO_ARGUMENT, 200},
    {"PORT", NO_ARGUMENT, 200},
    {"USER", REQUIRED_ARGUMENT, 331},
    {"PASS", REQUIRED_ARGUMENT, 332},
    {"CWD", REQUIRED_ARGUMENT, 250},
    {"DELE", REQUIRED_ARGUMENT, 250},
    {"QUIT", NO_ARGUMENT, 221},
    {"PASV", NO_ARGUMENT, 227}, // missing h1,h2,h3,h4,p1,p2
    {"LIST", OPTIONAL_ARGUMENT, 150}, // TODO: for the next three complete the message status
    {"STOR", REQUIRED_ARGUMENT, 150}, // and 226
    {"RETR", REQUIRED_ARGUMENT, 214}, 
    {"HELP", OPTIONAL_ARGUMENT, 214}, 
};

const return_obj_t object_ret[] = {
    {120, "Servie ready in nnn minutes.\n"},
    {125, "Data connection already open; transfert starting/\n"},
    {150, "File status okay; about to open data connextion.\n"},
    {200, " okay.\n"},
    {214, ""}, // help msg
    {220, "Servie ready for new user"},
    {221, "Servie closing control connection.\nLogged out if appropriate.\n"},
    {226, "Closing data connection.\n"},
    {230, "User logged in proceed.\n"},
    {250, "Requested file action successful.\n"},
    {257, "Entering Passive Mode "},
    {331, "User name okay, completed.\n"},
    {332, "Need account for login."},
};

void noop_f(char *cmd, char *arg, client_list_t *client)
{
    dprintf(client->fd, "%s%s", cmd, object_ret[3]);
}

void cdup_f(char *cmd, char *arg, client_list_t *client)
{
    dprintf(client->fd, "%s%s", cmd, object_ret[3]);
}

void port_f(char *cmd, char *arg, client_list_t *client)
{
    dprintf(client->fd, "%s%s", cmd, object_ret[3]);
}

void user_f(char *cmd, char *arg, client_list_t *client)
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

void pass_f(char *cmd, char *arg, client_list_t *client)
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
void cwd_f(char *cmd, char *arg, client_list_t *client)
{
}
void dele_f(char *cmd, char *arg, client_list_t *client)
{
}
void quit_f(char *cmd, char *arg, client_list_t *client)
{
    dprintf(client->fd, "221 Goodbye.\n");
    client->has_auth |= (1 << 6);
}

char *disp_port(uint32_t port)
{

    printf("==++> %d\n", port);
    return ("EFZEF");
}

void pasv_f(char *cmd, char *arg, client_list_t *client)
{
    client->data_sock = malloc(sizeof(socket_t));
    if (!client->data_sock) {
        fprintf(stderr, "malloc() -> PASV");
        return;
    }
    client->data_sock->socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (client->data_sock->socket == -1) {
        fprintf(stderr, "socket() -> PASV", strerror(errno));
        return;
    }
    client->data_sock->cli_addr.sin_addr.s_addr = INADDR_ANY;
    client->data_sock->cli_addr.sin_family = AF_INET;
    client->data_sock->cli_addr.sin_port = 0; // supposely bind any availiable port
    client->data_sock->addr_len = sizeof(client->data_sock->cli_addr);
    if (bind(client->fd, (struct sockaddr *)&client->data_sock, \
        client->data_sock->addr_len) == -1) {
        fprintf(stderr, "bind() -> PASV", strerror(errno));
        return;
    }
    dprintf(client->fd, "227 Entering Passive Mode %s.\n", disp_port(client->data_sock->cli_addr.sin_port));
}
void list_f(char *cmd, char *arg, client_list_t *client)
{
}
void ret_f(char *cmd, char *arg, client_list_t *client)
{
}
void help_f(char *cmd, char *arg, client_list_t *client)
{
}

void pwd_f(char *cmd, char *arg, client_list_t *client)
{
    dprintf(client->fd, "257 \"%s\"", client->path_dist);
}

void disconnect_client(int fd, fd_set *rfds, client_list_t **list, int *connexion)
{
    close(fd);
    FD_CLR(fd, rfds);
    rm_at_filedesc(list,fd);
    --(*connexion);
}

void protocole_interpreter(ftp_t *ftp, int fd)
{
    client_list_t *client = get_node_at_filedesc(&ftp->cli_list, fd);
    void (*fct_ptr[13])(char *, char *, client_list_t *) = { // TODO: deplacer ça dans init_struct
        &noop_f, &cdup_f, port_f, \
        &user_f, &pass_f, cwd_f, \
        &dele_f, &quit_f, &pasv_f, \
        &list_f, &ret_f, &help_f, &pwd_f
    };
    int i = 0;
    char *cmd = strtok(ftp->buffer, " ");
    if (!client || !cmd)
        return;
    if (client->is_connected == FALSE && strcmp(cmd, "QUIT") != 0 && \
        strcmp(cmd, "USER") != 0 && strcmp(cmd, "PASS") != 0) { // check if the user is connected or not
        dprintf(client->fd, "530 Please login with USER and PASS\n");
    } else {
        while (i != 12) {
            if (strcmp(cmd, ftp_cmd[i].cmd) == 0) {
                fct_ptr[i](ftp_cmd[i].cmd, strtok(NULL, " "), client);
                break;
            }
            i++;
        }
        if (client->has_auth == NO_AUTH_DISCONNECT ||
            client->has_auth == WRONG_USER_DISCONNECT ||
            client->has_auth == WRIGHT_USER_DISCONNECT ||
            client->has_auth == POST_AUTH_DISCONNECT) {
            disconnect_client(client->fd, &ftp->rfds, &ftp->cli_list, &ftp->connexion);
        }
    }
    printf("connextion %d\n", ftp->connexion);
    dump_list(ftp->cli_list);
}

int new_clients(ftp_t *ftp)
{
    printf("someone's trying to connect\n");
    if ((ftp->clients.socket = accept(ftp->server.socket, \
        (struct sockaddr *)&ftp->clients.cli_addr, \
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


static int clients_connexion(int i, ftp_t *ftp)
{
    if (i == ftp->server.socket) {
        if (new_clients(ftp) != 0)
            return (84);
    } else {
        ftp->buffer = get_next_line(i);
        if (ftp->buffer == NULL) {
            printf("####pre disco\n");
            disconnect_client(i, &ftp->rfds, &ftp->cli_list, &ftp->connexion);
            return (0);
        }
        printf("socket number #%d said: %s\n", i, ftp->buffer);
        protocole_interpreter(ftp, i);
        free(ftp->buffer);
    }
}

int server_pi(ftp_t *ftp)
{
    int nfds = 0;

    FD_ZERO(&ftp->rfds);
    FD_SET(ftp->server.socket, &ftp->rfds);
    while (1) {
        ftp->afds = ftp->rfds;
        if (select(FD_SETSIZE, &ftp->afds, NULL, NULL, NULL) == -1) {
            perror("select");
            break;
        }
        for (int i = 0; i < FD_SETSIZE; ++i) {
            if (FD_ISSET(i, &ftp->afds) && clients_connexion(i, ftp) == 84)
                return (84);
        }
    }
    close(ftp->server.socket);
    return (0);
}