/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** active mode
*/

#define _GNU_SOURCE
#include "my_ftp.h"

static int *parse_arg(char *arg, client_list_t *client, int *index)
{
    char *tmp = NULL;
    char **strtab = NULL;
    int *int_tab = malloc(sizeof(int) * (6));

    if (!int_tab)
        return (NULL);
    if ((tmp = strstr(arg, "\r")) != NULL || (tmp = strstr(arg, "\r\n")) \
        != NULL || (tmp = strstr(arg, "\n")) != NULL )
        *tmp = '\0';
    if ((strtab = my_str_to_word_array(arg, ',')) == NULL)
        return (NULL);
    while (strtab[*index] != NULL) {
        if ((int_tab[*index] = verif_nb(strtab[*index], "IP")) == -1) {
            dprintf(client->fd, "NO\n");
            return (NULL);
        }
        ++*index;
    }
    free(strtab);
    return (int_tab);
}

void port_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    int index = 0;
    int *tab = parse_arg(arg, client, &index);
    char *ip = NULL;

    if (!tab)
        return;
    asprintf(&ip, "%d.%d.%d.%d", tab[0], tab[1], tab[2], tab[3]);
    free(tab);
    if (index != 6) {
        dprintf(client->fd, "501 Errors in parameters or Arguments\r\n");
    } else {
        client->data_sock = setup_data_socket();
        if (!client->data_sock)
            return;
        init_data_socket(client->data_sock, \
        tab[4] * 256 + tab[5], ip);
        dprintf(client->fd, "200 PORT okay.\n");
    }
    if (connect(client->data_sock->socket, \
        (struct sockaddr *)&client->data_sock->addr, \
        client->data_sock->addr_len) == -1)
        perror("");
}