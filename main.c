// To write a program that requests a user to enter their RegNo, name and save them to a file.

#include <stdio.h>

int main()
{

    char regno[20];
    char fname[15];
    char lname[15];
    char confirm;
    int trial = 1;

    while (trial <= 3)
    {
        puts("*************************************");
        printf("Your registration number? ");
        scanf("%s", regno);
        printf("Your first name? ");
        scanf("%s", fname);
        printf("Your last name? ");
        scanf("%s", lname);

        putchar('\n');
        printf("You are %s %s of registration %s.\n", fname, lname, regno);
        printf("Is this correct? (type 'y' or 'n'): ");
        scanf(" %c", &confirm);   // don't remove the whitespace before %c

        if (confirm == 'y')
        {
            puts("Record added to file ✅");
            break;
        }
        else if (confirm == 'n')
        {
            printf("trials done: %d of 3\n", trial);
            if (trial == 3)
            {
                puts("Trial limit reached! Exiting program...");
            }
            putchar('\n');
            trial++;
        }
        else
        {
            puts("Invalid input (Allowed values are 'yes' or 'no')");
            printf("trials done: %d of 3\n", trial);
            if (trial == 3)
            {
                puts("Trial limit reached! Exiting program...");
            }
            putchar('\n');
            trial++;
        }
    }

    putchar('\n');

    return (0);
}
