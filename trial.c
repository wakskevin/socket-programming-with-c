// To write a program that requests a user to enter their RegNo, name and save them to a file.

#include <stdio.h>

int main()
{

    char regno[20];
    char fname[15];
    char lname[15];
    int confirm;

    printf("Your registration number? ");
    scanf("%s", regno);
    printf("Your first name? ");
    scanf("%s", fname);
    printf("Your last name? ");
    scanf("%s", lname);

    putchar('\n');
    printf("You are %s %s of registration %s.\n", fname, lname,regno);

    putchar('\n');
    printf("Is this correct? (type 'y' or 'n'): ");
    confirm = getchar();

    printf("%c\n\n", confirm);

    return (0);
}
