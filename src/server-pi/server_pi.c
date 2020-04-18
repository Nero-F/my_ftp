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
    {"NOOP", NO_ARGUMENT, 200},
    {"CDUP", NO_ARGUMENT, 200},
    {"PORT", NO_ARGUMENT, 200},
    {"USER", REQUIRED_ARGUMENT, 331},
    {"PASS", REQUIRED_ARGUMENT, 332},
    {"CWD", REQUIRED_ARGUMENT, 250},
    {"DELE", REQUIRED_ARGUMENT, 250},
    {"QUIT", NO_ARGUMENT, 221},
    {"PASV", NO_ARGUMENT, 227},
    {"LIST", OPTIONAL_ARGUMENT, 150},
    {"RETR", REQUIRED_ARGUMENT, 214},
    {"STOR", REQUIRED_ARGUMENT, 150},
    {"HELP", OPTIONAL_ARGUMENT, 214},
    {"PWD", NO_ARGUMENT, 257},
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

    while (cmd[i] != '\0') {
        if (cmd[i] != ' ')
            return (cmd);
        ++i;
    }
    return (strdup("BAD CMD\n"));
}

static void check_ftp_cmd(char *cmd, void (*fct_ptr[])\
(ftp_t *, char *, client_list_t *), ftp_t *ftp, client_list_t *client)
{
    int i = 0;
    char *tmp = NULL;

    while (i < 14) {
        if ((tmp = strstr(cmd, "\r")) != NULL || \
            (tmp = strstr(cmd, "\r\n")) != NULL || \
            (tmp = strstr(cmd, "\n")) != NULL ) {
            *tmp = '\0';
        }
        if (strcmp(cmd, ftp_cmd[i].cmd) == 0) {
            fct_ptr[i](ftp, ftp->cmd_parse[1], client);
            break;
        }
        i++;
    }
    i == 14 ? dprintf(client->fd, "500 Unknown command.\n") : 0;
    check_disconnect(ftp, client);
}

static void protocole_interpreter(ftp_t *ftp, int fd, \
void (*fct_ptr[])(ftp_t *, char *, client_list_t *))
{
    client_list_t *client = get_node_at_filedesc(&ftp->cli_list, fd);
    ftp->cmd_parse = my_str_to_word_array(ftp->buffer, ' ');

    if (!ftp->cmd_parse)
        return;
    char *cmd = parse_cmd(ftp->cmd_parse[0]);
    if (!client || !cmd)
        return;
    if (client->is_connected == FALSE && strcmp(cmd, "QUIT") != 0 && \
        strcmp(cmd, "USER") != 0 && strcmp(cmd, "PASS") != 0) {
        dprintf(client->fd, "530 Please login with USER and PASS\n");
    } else {
        check_ftp_cmd(cmd, fct_ptr, ftp, client);
    }
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