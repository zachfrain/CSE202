#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "user.h"

void new_password(user_t *u)
{
    srand(time(0));
    int pw_length = 8 + (rand() % 3);
    char temp[10];
    int lb = 33;
    int ub = 126;
    for (int i = 0; i < pw_length; i++)
    {
        char c = (rand() % (ub - lb + 1)) + lb;
        temp[i] = c;
    }
    if (u->password != NULL)
    {
        free(u->password);
    }
    u->password = (char *)malloc(strlen(temp) + 1);
    if (u->password != NULL)
    {
        strcpy(u->password, temp);
    }
}

int read_users(user_t *user_list, char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Failure opening file.");
        return -1;
    }

    int count = 0;
    char line[50];
    while (fgets(line, sizeof(line), file) != NULL)
    {

        user_list[count].username = (char *)malloc(10);
        user_list[count].password = (char *)malloc(11);
        if (user_list[count].username == NULL || user_list[count].password == NULL)
        {
            perror("Memory allocation failed.");
            fclose(file);
            return -1;
        }
        if (sscanf(line, "%s %s %d", user_list[count].username, user_list[count].password, &user_list[count].access) == 3)
        {
            count++;
        }
        else
        {
            fprintf(stderr, "Error parsing line: %s\n", line);
        }
    }

    fclose(file);
    return count;
}

int save_users(user_t *user_list, char *filename, int size)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Failure opening file.");
        return -1;
    }

    for (int i = 0; i < size; i++)
    {
        fprintf(file, "%s %s %d\n", user_list[i].username, user_list[i].password, user_list[i].access);
    }
    fclose(file);
    return 0;
}

int find_user(user_t *user_list, char *username, char *password, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (strcmp(user_list[i].username, username) == 0)
        {
            return i;
        }
        else if (strcmp(user_list[i].password, password) == 0)
        {
            return i;
        }
    }
    return -1;
}