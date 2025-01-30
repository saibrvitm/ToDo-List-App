#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "task_manager.h"

// Function: Take string input safely
void takeInput(const char *prompt, char *input) {
    printf("%s", prompt);
    fgets(input, 50, stdin);
    input[strcspn(input, "\n")] = '\0';
}

// Function: Securely take password input
void takePassword(const char *prompt, char *password) {
    printf("%s", prompt);
    int i = 0;
    char ch;

    while (1) {
        ch = getch();
        if (ch == ENTER) {
            password[i] = '\0';
            break;
        } else if (ch == BCKSPC) {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (i < 49) {
            password[i++] = ch;
            printf("*");
        }
    }
    printf("\n");
}


// Function: Sign up a new user
void signUp() {
    FILE *fp;
    User newUser;
    char confirmPassword[50];

    printf("\n--- Sign Up ---\n");

    takeInput("Enter your username: ", newUser.username);
    takePassword("Enter your password: ", newUser.password);
    takePassword("Confirm your password: ", confirmPassword);

    if (strcmp(newUser.password, confirmPassword) != 0) {
        printf("\nPasswords do not match. Registration failed.\n");
        return;
    }

    fp = fopen(DATA_FILE, "ab+");
    if (fp == NULL) {
        printf("\nError: Could not open file %s\n", DATA_FILE);
        return;
    }

    fwrite(&newUser, sizeof(User), 1, fp);
    fclose(fp);

    printf("\nUser registration successful! Your username is '%s'.\n", newUser.username);
}

// Function: Login an existing user
int login() {
    FILE *fp;
    User existingUser;
    char username[50], password[50];
    int userFound = 0;

    printf("\n--- Login ---\n");

    takeInput("Enter your username: ", username);
    takePassword("Enter your password: ", password);

    fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open file %s. No users registered yet.\n", DATA_FILE);
        return 0;
    }

    while (fread(&existingUser, sizeof(User), 1, fp)) {
        if (strcmp(existingUser.username, username) == 0) {
            userFound = 1;
            if (strcmp(existingUser.password, password) == 0) {
                fclose(fp);
                printf("\nLogin successful! Welcome, %s.\n", existingUser.username);
                strcpy(GLOBAL_USERNAME, existingUser.username);
                return 1;
            } else {
                printf("\nIncorrect password.\n");
                fclose(fp);
                return 0;
            }
        }
    }

    if (!userFound) {
        printf("\nUsername not found.\n");
    }

    fclose(fp);
    return 0;
}

// Function: Authentication System
char* auth() {
    int choice;

    while (1) {
        printf("\n--- Authentication System ---\n");
        printf("1. Sign Up\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Your choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("\nInvalid input. Please enter a number.\n");
            continue;
        }
        getchar();

        switch (choice) {
            case 1:
                signUp();
                break;
            case 2:
                if (login()) {
                    return GLOBAL_USERNAME;
                }
                break;
            case 3:
                printf("\nExiting... Goodbye!\n");
                exit(0);
            default:
                printf("\nInvalid choice. Try again.\n");
        }
    }
    return "null";
}