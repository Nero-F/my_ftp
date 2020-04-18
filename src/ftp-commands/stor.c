/*
** EPITECH PROJECT, 2020
** my_ftp
** File description:
** upload
*/

#include <sys/stat.h>
#include <fcntl.h>
#include "my_ftp.h"

static bool_t check_data_sock(client_list_t *client)
{
    if (client->can_transfer == FALSE) {
        dprintf(client->fd, "425 Use PORT or PASV first.\n");
        return (FALSE);
    }
    return (TRUE);
}

static int verif_file(char *path, int data_sock, client_list_t *client)
{
    int fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

    if (check_data_sock(client) == FALSE) {
        printf("MES couilles\n");
        return (84);
    }
    if ( fd == -1) {
        dprintf(client->fd, "550 failed to open file.\n");
        perror("");
        return (84);
    }
    dprintf(client->fd, "150 File status okay;\n");
    return (fd);
}

static void send_datas(int fd, int fdf, int socket, client_list_t *client)
{
    char buffer[BUFFER_SIZE + 1];
    int ret_read = 0;

    while ((ret_read = read(fdf, buffer, BUFFER_SIZE)) > 0) {
        write(fd, buffer, ret_read);
    }
    close(socket);
    dprintf(client->fd, "226 Closing data connection\n");
    client->can_transfer = FALSE;
    close(fd);
}

void stor_f(ftp_t *ftp, char *arg, client_list_t *client)
{
    socket_t *s = client->data_sock;
    int fdf = 0;
    int fd = 0;;
    pid_t pid = 0;

    if (!arg && client->can_transfer == FALSE || !s) {
        dprintf(client->fd, "425 Use PORT or PASV first.\n");
        return;
    } else if (!arg) {
        dprintf(client->fd, "501 Syntax error in parameter or arguments.\n");
        return;
    }
    fd = verif_file(arg, s->socket, client);
    if (fd == 84 || (fdf = accept_data_sock(fd, s)) == 84)
        return;
    if ((pid = fork()) == -1)
        perror("");
    else if (pid == 0)
        send_datas(fd, fdf, s->socket, client);
}