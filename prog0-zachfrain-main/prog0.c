#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "user.h"

int main(int argc, char *argv[])
{
    user_t temp[MAX_USERS];
    char *filename = "users.txt";
    int i = read_users(temp, filename);
    int run_count = 0;
    int acct_index;
    if (i == -1)
    {
        perror("Error Reading File.\n");
        return -1;
    }
    else
    {
        printf("Success Opening File.\n");
        while (run_count != 3)
        {
            printf("Please enter your username:\n");
            char name[50];
            scanf("%s", name);
            printf("Please enter your password:\n");
            char pass[50];
            scanf("%s", pass);
            acct_index = find_user(temp, name, pass, i);
            if (acct_index != -1)
            {
                int ac = temp[acct_index].access;
                if (ac == 1)
                {
                    printf("Logged in successfully.\n");
                    break;
                }
                else
                {
                    perror("Do not have access.\n");
                    return -1;
                }
            }
            else
            {
                printf("Username or password not found. Please try again\n");
                run_count++;
            }
            return -1;
        }
    }
    int running = 1;
    int modified = 0;
    while (running == 1)
    {
        char choice;
        printf("Please select an option (Enter just the lowercase letter).\n");
        printf("a) Add a new user.\nb) Reset the password of an existing user.\nc) Log out of the program.\n");
        scanf("%c", &choice);
        if (choice == 'a')
        {
            char new_name[50];
            printf("Please enter the username for the new account:\n");
            scanf("%s", new_name);

            int new_access;
            printf("Please enter the access privilege (0 or 1):\n");
            scanf("%d", &new_access);

            int exist = find_user(temp, new_name, "", i);
            if (exist != -1)
            {
                printf("This user already exists.\n");
                continue;
            }
            else
            {
                if (i < MAX_USERS)
                {
                    temp[i].username = (char *)malloc(strlen(new_name) + 1);
                    if (temp[i].username != NULL)
                    {
                        strcpy(temp[i].username, new_name);
                    }
                    else
                    {
                        perror("Memory allocation for username failed.");
                        return -1;
                    }

                    temp[i].access = new_access;
                    new_password(&temp[i]);

                    i++;
                    modified = 1;
                }
                else
                {
                    printf("Maximum number of users reached.\n");
                }
            }
        }
        else if (choice == 'b')
        {
            char reset_name[50];
            printf("Please enter the username of the account.\n");
            scanf("%s", reset_name);
            int reset_exist = find_user(temp, reset_name, "", i);
            if (reset_exist != -1)
            {
                new_password(&temp[reset_exist]);
                modified = 1;
            }
            else
            {
                perror("Username not found.\n");
                return -1;
            }
        }
        else if (choice == 'c')
        {
            if (modified == 1)
            {
                int ggg = save_users(temp, filename, i);
                
                    printf("New list written successfully.\n");
                    running++;
                
            }
        }
        else
        {
            printf("Option not recognized, please try again.\n\n");
        }
    }

    return 0;
}