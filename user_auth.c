#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define ENTER 13
#define BCKSPC 8
#define DATA_FILE "data/users.dat"

struct user {
    char username[50];
    char password[50];
};

void takeInput(const char *prompt, char *input);
void takePassword(const char *prompt, char *password);
void signUp();
int login();
void auth();

// Function: Take string input safely
void takeInput(const char *prompt, char *input) {
    printf("%s", prompt);
    fgets(input, 50, stdin);
    input[strcspn(input, "\n")] = '\0'; // Remove trailing newline
}

// Function: Securely take password input
void takePassword(const char *prompt, char *password) {
    printf("%s", prompt);
    int i = 0;
    char ch;

    while (1) {
        ch = getch(); // Read a character without echoing
        if (ch == ENTER) {
            password[i] = '\0';
            break;
        } else if (ch == BCKSPC) {
            if (i > 0) {
                i--;                     // Move one step back in the password buffer
                printf("\b \b");         // Erase the last `*` visually
            }
        } else if (i < 49) { // Prevent buffer overflow
            password[i++] = ch;
            printf("*");     // Print `*` for every character entered
        }
    }
    printf("\n");
}


// Function: Sign up a new user
void signUp() {
    FILE *fp;
    struct user newUser;
    char confirmPassword[50];

    printf("\n--- Sign Up ---\n");

    takeInput("Enter your username: ", newUser.username);
    takePassword("Enter your password: ", newUser.password);
    takePassword("Confirm your password: ", confirmPassword);

    if (strcmp(newUser.password, confirmPassword) != 0) {
        printf("\nPasswords do not match. Registration failed.\n");
        return;
    }

    // Open file to append user data
    fp = fopen(DATA_FILE, "ab+");
    if (fp == NULL) {
        printf("\nError: Could not open file %s\n", DATA_FILE);
        return;
    }

    fwrite(&newUser, sizeof(struct user), 1, fp);
    fclose(fp);

    printf("\nUser registration successful! Your username is '%s'.\n", newUser.username);
}

// Function: Login an existing user
int login() {
    FILE *fp;
    struct user existingUser;
    char username[50], password[50];
    int userFound = 0;

    printf("\n--- Login ---\n");

    takeInput("Enter your username: ", username);
    takePassword("Enter your password: ", password);

    // Open file to read user data
    fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open file %s. No users registered yet.\n", DATA_FILE);
        return 0;
    }

    while (fread(&existingUser, sizeof(struct user), 1, fp)) {
        if (strcmp(existingUser.username, username) == 0) {
            userFound = 1;
            if (strcmp(existingUser.password, password) == 0) {
                fclose(fp);
                printf("\nLogin successful! Welcome, %s.\n", existingUser.username);
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
void auth() {
    int choice;

    while (1) {
        printf("\n--- Authentication System ---\n");
        printf("1. Sign Up\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Your choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // Clear invalid input
            printf("\nInvalid input. Please enter a number.\n");
            continue;
        }
        getchar(); // Consume newline

        switch (choice) {
            case 1:
                signUp();
                break;
            case 2:
                if (login()) {
                    return; // Exit the auth system after successful login
                }
                break;
            case 3:
                printf("\nExiting... Goodbye!\n");
                exit(0);
            default:
                printf("\nInvalid choice. Try again.\n");
        }
    }
}