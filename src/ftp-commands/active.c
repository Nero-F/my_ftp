/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** active mode
*/

#include "my_ftp.h"

void port_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    char **strtab = my_str_to_word_array(arg, ',');
    int int_tab[6] = {0};
    int index = 0;
    if (!strtab)
        return;
    while (strtab[index] != NULL) {
        if ((int_tab[index] = verif_nb(strtab[index], "IP")) == -1) {
            dprintf(client->fd, "NO\n");
            return;
        }
        ++index;
    }
    if (index != 6) {
        dprintf(client->fd, "NOP\n");
    } else {
        client->data_sock = setup_data_socket();
        if (!client->data_sock)
            return;
        init_data_socket(client->data_sock, \
        int_tab[4] * 256 + int_tab[5], "127.0.0.1");
        dprintf(client->fd, "200 PORT okay.\n");
    }
    if (connect(client->data_sock->socket, \
        (struct sockaddr *)&client->data_sock->addr, \
        client->data_sock->addr_len) == -1) {
        perror("");
        return;
    }
}