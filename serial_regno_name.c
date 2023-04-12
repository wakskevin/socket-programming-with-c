// To write a program that requests a user to enter his/her regno and name and saves to a file.

#include <stdio.h>
#include <string.h>

int main()
{
    int serial;
    char regno[20];
    char fname[15];
    char lname[15];
    char confirm[5];
    int trial = 1;

    FILE *fh;

    while (trial <= 3)
    {
        puts("*************************************");
        printf("Your serial number? ");
        scanf("%d", &serial);
        printf("Your registration number? ");
        scanf("%s", regno);
        printf("Your first name? ");
        scanf("%s", fname);
        printf("Your last name? ");
        scanf("%s", lname);

        putchar('\n');
        printf("You are %s %s of registration %s. Your serial number is %d\n", fname, lname, regno, serial);
        printf("Is this correct? (type 'yes' or 'no'): ");
        scanf("%s", confirm);

        if (strcmp(confirm, "yes") == 0)
        {
            // Open the file
            fh = fopen("serial_regno_name.txt", "a");
            if (fh == NULL)
            {
                puts("Failed to create file serial_regno_name.txt");
                return (1);
            }
            else
            {
                if (ftell(fh) == 0) // start writing at the begining of the file
                {
                    fprintf(fh, "%d %s %s %s", serial, regno, fname, lname);
                }
                else // go to the next line and start writing from there
                {
                    fprintf(fh, "\n%d %s %s %s", serial, regno, fname, lname);
                }
                puts("Record added to file ✅");
                // Close the file
                fclose(fh);
                break;
            }
        }
        else if (strcmp(confirm, "no") == 0)
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
