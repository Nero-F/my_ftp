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
    {"NOOP", "NOOP\n", "NOOP\r\n", NO_ARGUMENT, 200},
    {"CDUP", "CDUP\n", "CDUP\r\n", NO_ARGUMENT, 200},
    {"PORT", "PORT\n", "PORT\r\n", NO_ARGUMENT, 200},
    {"USER", "USER\n", "USER\r\n", REQUIRED_ARGUMENT, 331},
    {"PASS", "PASS\n", "PASS\r\n", REQUIRED_ARGUMENT, 332},
    {"CWD", "CWD\n", "CWD\r\n", REQUIRED_ARGUMENT, 250},
    {"DELE", "DELE\n", "DELE\r\n", REQUIRED_ARGUMENT, 250},
    {"QUIT", "QUIT\n", "QUIT\r\n", NO_ARGUMENT, 221},
    {"PASV", "PASV\n", "PASV\r\n", NO_ARGUMENT, 227},
    {"LIST", "LIST\n", "LIST\r\n", OPTIONAL_ARGUMENT, 150},
    {"RETR", "RETR\n", "RETR\r\n", REQUIRED_ARGUMENT, 214},
    {"STOR", "STOR\n", "STOR\r\n", REQUIRED_ARGUMENT, 150},
    {"HELP", "HELP\n", "HELP\r\n", OPTIONAL_ARGUMENT, 214},
    {"PWD", "PWD\n", "PWD\r\n", NO_ARGUMENT, 257},
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

    printf("####[%s]\n", cmd);
    while (cmd[i] != '\0') {
        if (cmd[i] != ' ')
            return (cmd);
        ++i;
    }
    return (strdup("BAD CMD\n"));
}

static void protocole_interpreter(ftp_t *ftp, int fd, \
void (*fct_ptr[])(ftp_t *, char *, client_list_t *))
{
    client_list_t *client = get_node_at_filedesc(&ftp->cli_list, fd);
    int i = 0;
    char *tmp;
    char **cmd_p = my_str_to_word_array(ftp->buffer, ' ');

    if (!cmd_p)
        return;
    char *cmd = parse_cmd(cmd_p[0]);
    if (!client || !cmd)
        return;
    if (client->is_connected == FALSE && strcmp(cmd, "QUIT") != 0 && \
        strcmp(cmd, "USER") != 0 && strcmp(cmd, "PASS") != 0) {
        dprintf(client->fd, "530 Please login with USER and PASS\n");
    } else {
        while (i < 14) {
            if ((tmp = strstr(cmd, "\r")) != NULL || \
                (tmp = strstr(cmd, "\r\n")) != NULL || \
                (tmp = strstr(cmd, "\n")) != NULL ) {
                *tmp = '\0';
            }
            if (strcmp(cmd, ftp_cmd[i].cmd) == 0) {
                fct_ptr[i](ftp, cmd_p[1], client);
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
    void (*fct_ptr[14])(ftp_t *, char *, client_list_t *) = {
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