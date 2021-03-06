/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** header file
*/


#ifndef MY_FTP_H_
#define MY_FTP_H_

#define _GNU_SOURCE

#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define READ_SIZE 32

#define NO_AUTH_DISCONNECT          0x40
#define WRONG_USER_DISCONNECT       0x60
#define WRIGHT_USER_DISCONNECT      0x62
#define POST_AUTH_DISCONNECT        0x6a

#define BUFFER_SIZE 1024

#define OPTIONAL_ARGUMENT   2
#define REQUIRED_ARGUMENT   1
#define NO_ARGUMENT         0

#define MAX_CLIENT          42

#define MAX(x, y) ((x) > (y) ? (x) : (y))

typedef unsigned short REPLY_CODE;

typedef struct return_obj_s
{
    REPLY_CODE code;
    char *message;
} return_obj_t;

typedef enum bool_e
{
    FALSE,
    TRUE
} bool_t;

typedef struct ftp_cmd_s
{
    char *cmd;
    bool_t has_argument;
    REPLY_CODE code;
} ftp_cmd_t;

typedef struct socket_s
{
    struct sockaddr_in addr;
    socklen_t addr_len;
    int socket;
} socket_t;

typedef struct client_list_s
{
    int fd;
    socket_t *data_sock;
    bool_t is_connected;
    bool_t can_transfer;
    bool_t in_use;
    unsigned int has_auth;
    char *path_dist;
    int port;
    struct client_list_s *nxt;
    struct client_list_s *prev;
} client_list_t;

typedef struct fpt_s
{
    int connexion;
    int port;
    char *path;
    char *buffer;
    char *local_ip;
    fd_set rfds;
    fd_set wfds;
    fd_set afds;
    socket_t server;
    socket_t clients;
    socket_t data_sock;
    char **cmd_parse;
    client_list_t *cli_list;
} ftp_t;

char *get_next_line(int fd);
char **my_str_to_word_array(char *str, char sep);
int my_ftp(char *port_str, char *path);
int verif_nb(char *nb, char *elem);
int server_pi(ftp_t *ftp);
void get_local_ip(ftp_t *ftp);
char *copy_to_buff(char *arg, char *path_dist);

/* linked list */
int add_node(client_list_t **head, int fd, char *path, int port);
void rm_at_filedesc(client_list_t **head, int fd);
void dump_list(client_list_t *head);
void free_list(client_list_t **head);
client_list_t *get_node_at_filedesc(client_list_t **head, int fd);

/* ftp commands */

void user_f(ftp_t *ftp, char *arg, client_list_t *client);
void pass_f(ftp_t *ftp, char *arg, client_list_t *client);
void pasv_f(ftp_t *ftp, char *arg, client_list_t *client);
void port_f(ftp_t *ftp, char *arg, client_list_t *client);
void retr_f(ftp_t *ftp, char *arg, client_list_t *client);
void stor_f(ftp_t *ftp, char *arg, client_list_t *client);
void dele_f(ftp_t *ftp, char *arg, client_list_t *client);
void noop_f(ftp_t *ftp, char *arg, client_list_t *client);
void quit_f(ftp_t *ftp, char *arg, client_list_t *client);
void help_f(ftp_t *ftp, char *arg, client_list_t *client);
void list_f(ftp_t *ftp, char *arg, client_list_t *client);
void cdup_f(ftp_t *ftp, char *arg, client_list_t *client);
void pwd_f(ftp_t *ftp, char *arg, client_list_t *client);
void cwd_f(ftp_t *ftp, char *arg, client_list_t *client);

socket_t *setup_data_socket(void);
void init_data_socket(socket_t *data_sock, int port, char *ip);
char *disp_port(struct sockaddr_in *addr, char *ip);

/* client gestion */

int new_clients(ftp_t *ftp);
void disconnect_client(int fd, fd_set *rfds, client_list_t **list, \
int *connexion);
void check_disconnect(ftp_t *ftp, client_list_t *client);
int accept_data_sock(int fd, socket_t *data_sock);

#endif /* MY_FTP_H_*/
