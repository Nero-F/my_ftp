/*
** EPITECH PROJECT, 2019
** put a char * on char **
** File description:
** my_str_to_word_array
*/

#include <stdlib.h>
#include <stdio.h>

static int nbr_sep(char *str, char sep)
{
    int space = 0;
    int n = 0;

    while (str[n] != '\0') {
        if (str[n] == sep)
            space += 1;
        n += 1;
    }
    return (space + 1);
}

static char **malloc_dest(char *str, char sep)
{
    int n = 0;
    int count = 0;
    int count_char = 0;
    int space = nbr_sep(str, sep);
    char **dest = NULL;

    dest = malloc(sizeof(char *) * (space + 1));
    dest[space] = NULL;
    while (n < space) {
        if (str[count] == sep || str[count] == '\0') {
            dest[n] = malloc(sizeof(char) * (count_char + 1));
            dest[n][count_char] = '\0';
            count += 1;
            count_char = 0;
            ++n;
        }
        count += 1;
        count_char += 1;
    }
    return (dest);
}

char **my_str_to_word_array(char *str, char sep)
{
    int n = 0;
    int x = 0;
    int y = 0;
    char **dest = malloc_dest(str, sep);

    while (str[n] != '\0') {
        if (str[n] == sep) {
            dest[y++][x] = '\0';
            x = 0;
        } else
            dest[y][x++] = str[n];
        n += 1;
    }
    dest[++y] = NULL;
    return (dest);
}