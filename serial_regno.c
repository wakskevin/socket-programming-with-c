// To write a program that requests a user to enter his/her name and saves it to a file.

#include <stdio.h>
#include <string.h>

int main()
{
    int serial;
    char regno[20];
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

        putchar('\n');
        printf("Your serial number is %d and your registration number is %s\n", serial, regno);
        printf("Is this correct? (type 'yes' or 'no'): ");
        scanf("%s", confirm);

        if (strcmp(confirm, "yes") == 0)
        {
            // Open the file
            fh = fopen("serial_regno.txt", "a");
            if (fh == NULL)
            {
                puts("Failed to create file serial_regno.txt");
                return (1);
            }
            else
            {
                if (ftell(fh) == 0) // start writing at the begining of the file
                {
                    fprintf(fh, "%d %s", serial, regno);
                }
                else // go to the next line and start writing from there
                {
                    fprintf(fh, "\n%d %s", serial, regno);
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
