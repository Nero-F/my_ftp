#ifndef MY_FTP_H_
#define MY_FTP_H_

#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define NO_AUTH_DISCONNECT          0x40 
#define WRONG_USER_DISCONNECT       0x60 
#define WRIGHT_USER_DISCONNECT      0x62
#define POST_AUTH_DISCONNECT        0x6a


#define OPTIONAL_ARGUMENT   2
#define REQUIRED_ARGUMENT   1
#define NO_ARGUMENT         0

#define MAX_CLIENT          5

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
    struct sockaddr_in cli_addr;
    socklen_t addr_len;
    int socket;
} socket_t;

typedef struct client_list_s
{
    int fd;
    socket_t *data_sock;
    bool_t is_connected;
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
    fd_set rfds;
    fd_set afds;
    socket_t server;
    socket_t clients;
    socket_t data_sock;
    client_list_t *cli_list;
} ftp_t;

char *get_next_line(int fd);
int my_ftp(char *port_str, char *path);
int verif_port(char *port);
int server_pi(ftp_t *ftp);

/* linked list */
int add_node(client_list_t **head, int fd, char *path, int port);
void rm_at_filedesc(client_list_t **head, int fd);
void dump_list(client_list_t *head);
void free_list(client_list_t **head);
client_list_t *get_node_at_filedesc(client_list_t **head, int fd);

#endif /* MY_FTP_H_*/