/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** list utilities
*/

#include "my_ftp.h"

void dump_list(client_list_t *head)
{
    client_list_t *tmp = head;

    while (tmp) {
        printf("fd -> %d | ", tmp->fd);
        tmp = tmp->nxt;
    }
}

void free_list(client_list_t **head)
{
    client_list_t *tmp = *head;
    client_list_t *rmvr = *head;

    while (tmp != NULL) {
        tmp = rmvr->nxt;
        free(rmvr);
        rmvr = tmp;
    }
}