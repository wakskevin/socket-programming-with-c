// a program that requests a user to enter his/her regno and saves it to a file.

#include <stdio.h>
#include <string.h>

int main()
{
    char regno[20];
    char confirm[5];
    int trial = 1;

    FILE *fh;

    while (trial <= 3)
    {
        puts("*************************************");
        printf("Your registration number? ");
        scanf("%s", regno);

        putchar('\n');
        printf("Your registration number is %s\n", regno);
        printf("Is this correct? (type 'yes' or 'no'): ");
        scanf("%s", confirm);

        if (strcmp(confirm, "yes") == 0)
        {
            // Open the file
            fh = fopen("1_regno.txt", "a");
            if (fh == NULL)
            {
                puts("Failed to create file 1_regno.txt");
                return (1);
            }
            else
            {
                if (ftell(fh) == 0) // start writing at the begining of the file
                {
                    fprintf(fh, "%s", regno);
                }
                else // go to the next line and start writing from there
                {
                    fprintf(fh, "\n%s", regno);
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
