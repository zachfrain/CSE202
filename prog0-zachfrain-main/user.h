#ifndef USER_H
#define USER_H
#define NULL 0
#define MAX_USERS 150

enum ACCESS
{
    USER,
    ADMIN
};

typedef struct User
{
    char *username;
    char *password;
    enum ACCESS access;
} user_t;

/*
Generates a random password of 8-10 char (alphabet, digit, and punctuations)
in the member 'password' of the struct 'u'
*/
void new_password(user_t *u);

/*
Reads the user credentials from the file filename and stores them in the array user_list
The function returns the number of users read from the file, or -1 if the file cannot be read.
*/
int read_users(user_t *user_list, char *filename);

/*
Saves the user credentials from the array user_list to the file filename.
size is the number of users in the array user_list
The function returns 0 if writing to the file was successful, -1 otherwise.
*/
int save_users(user_t *user_list, char *filename, int size);

/*
Returns the index of the element in the array user_list that has the username equal to username if password is NULL.
If the argument password is not null, the function returns the index of the user that has the username
and password match the arguments username and password.
If the username is not found or the username is found, but doesn't' match the password, function returns -1
size is the number of users in the array user_list
*/
int find_user(user_t *user_list, char *username, char *password, int size);

#endif /* USER_H */
