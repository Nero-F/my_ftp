/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** server protocole interpreter
*/

#include <sys/types.h>
#include <sys/select.h>
#include "my_ftp.h"

const ftp_cmd_t ftp_cmd[] = {
    {"NOOP", "NOOP\r\n", NO_ARGUMENT, 200},
    {"CDUP", "CDUP\r\n", NO_ARGUMENT, 200},
    {"PORT", "PORT\r\n", NO_ARGUMENT, 200},
    {"USER", "USER\r\n", REQUIRED_ARGUMENT, 331},
    {"PASS", "PASS\r\n", REQUIRED_ARGUMENT, 332},
    {"CWD", "CWD\r\n", REQUIRED_ARGUMENT, 250},
    {"DELE", "DELE\r\n", REQUIRED_ARGUMENT, 250},
    {"QUIT", "QUIT\r\n", NO_ARGUMENT, 221},
    {"PASV", "PASV\r\n", NO_ARGUMENT, 227}, // missing h1,h2,h3,h4,p1,p2
    {"LIST", "LIST\r\n", OPTIONAL_ARGUMENT, 150}, // TODO: for the next three complete the message status
    {"RETR", "RETR\r\n", REQUIRED_ARGUMENT, 214}, 
    {"STOR", "STOR\r\n", REQUIRED_ARGUMENT, 150}, // and 226
    {"HELP", "HELP\r\n", OPTIONAL_ARGUMENT, 214}, 
    {"PWD", "PWD\r\n", NO_ARGUMENT, 257}, 
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

static char *parse_cmd(char *cmd)
{
    int i = 0;

    printf("[%s]\n", cmd);
    while (cmd[i] != '\0') {
        if (cmd[i] != ' ')
            return (strtok(cmd, " "));
        ++i;
    }
    return (strdup("BAD CMD\n"));
}

static void protocole_interpreter(ftp_t *ftp, int fd, \
void (*fct_ptr[])(ftp_t *, char *, client_list_t *))
{
    client_list_t *client = get_node_at_filedesc(&ftp->cli_list, fd);
    int i = 0;
    
    char *cmd = parse_cmd(ftp->buffer);
    if (!client || !cmd)
        return;
    if (client->is_connected == FALSE && strcmp(cmd, "QUIT") != 0 && \
        strcmp(cmd, "USER") != 0 && strcmp(cmd, "PASS") != 0) { // check if the user is connected or not
        dprintf(client->fd, "530 Please login with USER and PASS\n");
    } else {
        while (i < 14) {
            if (strcmp(cmd, ftp_cmd[i].cmd) == 0 || \
                strcmp(cmd, ftp_cmd[i].cmd_p) == 0) {
                printf("COMMAND %s\n", ftp_cmd[i].cmd);
                fct_ptr[i](ftp, strtok(NULL, " "), client);
                break;
            }
            i++;
        }
        i == 14 ? dprintf(client->fd, "500 Unknown command.\n") : 0;
        check_disconnect(ftp, client);
    }
    printf("connextion %d\n", ftp->connexion);
    dump_list(ftp->cli_list);
}

static int clients_connexion(int i, ftp_t *ftp, \
void (*fct_ptr[])(ftp_t *, char *, client_list_t *))
{
    if (i == ftp->server.socket) {
        if (new_clients(ftp) != 0)
            return (84);
    } else {
        ftp->buffer = get_next_line(i);
        if (ftp->buffer == NULL) {
            disconnect_client(i, &ftp->rfds, &ftp->cli_list, &ftp->connexion);
            return (0);
        }
        printf("socket number #%d said: %s\n", i, ftp->buffer);
        protocole_interpreter(ftp, i, fct_ptr);
        free(ftp->buffer);
    }
}

int server_pi(ftp_t *ftp)
{
    void (*fct_ptr[14])(ftp_t *, char *, client_list_t *) = { // TODO: deplacer ça dans init_struct
        &noop_f, &cdup_f, &port_f, &user_f, &pass_f, cwd_f, \
        &dele_f, &quit_f, &pasv_f, &list_f, &retr_f, &stor_f, &help_f, &pwd_f
    };

    FD_ZERO(&ftp->rfds);
    FD_SET(ftp->server.socket, &ftp->rfds);
    while (1) {
        ftp->afds = ftp->rfds;
        if (select(FD_SETSIZE, &ftp->afds, NULL, NULL, NULL) == -1) {
            perror("select");
            break;
        }
        for (int i = 0; i < FD_SETSIZE; ++i) {
            if (FD_ISSET(i, &ftp->afds) && \
                clients_connexion(i, ftp, fct_ptr) == 84)
                return (84);
        }
    }
    return (0);
}