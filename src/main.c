#include <string.h> // strcmp
#include <ctype.h> // ctype
#include <stdlib.h> // atoi
#include "my_ftp.h"

static void display_help(void)
{
	printf("USAGE: ./my_fpt port path\n"
	"       port is the port number on wich the server"
    " socket listents\n"
	"       path is the path to the directory for the"
    " \e[3Anonymous\e[0 user\n");
}

int verif_nb(char *nb, char *elem)
{
    size_t len = strlen(nb);
    size_t i = 0;

    while (i != len) {
        if (isdigit(nb[i]) == 0) {
            fprintf(stderr, "%s is not a number\n", elem);
            return (-1);
        }
        ++i;
    }
    return (atoi(nb));
}

int main(int ac, char *av[])
{
    if (ac == 2 && strcmp(av[1], "-help"))
        display_help();
    else if (ac == 3 && my_ftp(av[1], av[2]) == 0)
        return (0);
    return (84);
}
