/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** linked list utilities
*/

#include <dirent.h>
#include "my_ftp.h"

void init_client_node(client_list_t *node, int fd, char *path, int port)
{
    node->fd = fd;
    node->is_connected = FALSE;
    node->has_auth = 0;
    node->path_dist = strdup(path);
    node->nxt = NULL;
    node->data_sock = NULL;
    node->port = port;
    node->can_transfer = FALSE;
    node->in_use = FALSE;
}

int add_node(client_list_t **head, int fd, char *path, int port)
{
    client_list_t *elem = malloc(sizeof(client_list_t));
    client_list_t *tmp = *head;

    if (!elem)
        return (84);
    init_client_node(elem, fd, path, port);
    if (!*head) {
        elem->prev = NULL;
        *head = elem;
    } else {
        while (tmp->nxt != NULL) {
            tmp = tmp->nxt;
        }
        tmp->nxt = elem;
        elem->prev = tmp;
    }
    return (0);
}

void delete_node(client_list_t **head, client_list_t *del)
{
    if (*head == NULL || del == NULL)
        return;
    if (*head == del)
        *head = del->nxt;
    if (del->nxt != NULL)
        del->nxt->prev = del->prev;
    if (del->prev != NULL)
        del->prev->nxt = del->nxt;
    free(del);
    del = NULL;
}

void rm_at_filedesc(client_list_t **head, int fd)
{
    client_list_t *tmp = *head;

    while (tmp != NULL && tmp->fd != fd) {
        tmp = tmp->nxt;
    }
    if (!tmp)
        return;
    delete_node(head, tmp);
}

client_list_t *get_node_at_filedesc(client_list_t **head, int fd)
{
    client_list_t *tmp = *head;

    while (tmp) {
        if (tmp->fd == fd)
            return (tmp);
        tmp = tmp->nxt;
    }
    return (NULL);
}