#include <stdio.h>
#include <string.h>

int main()
{
    FILE *fp, *fp2, *fp3;
    char *fileName = "plaintext.txt";
    char ch;

    fp = fopen(fileName, "r");
    fp2 = fopen("cipher.txt", "w");
    // fp3 = fopen("dec.txt", "w");

    while ((ch = fgetc(fp)) != EOF)
    {

        if (ch != ' ')
        {
            int temp = ch;
            char ch1;
            printf("%d -> %c\n", temp, temp);
            fputc(~temp, fp2);
        }
        else
        {
            fputc(ch, fp2);
        }
    }

    // fclose(fp2);

    // fp2 = fopen("enc.txt", "r");
    // while ((ch = fgetc(fp2)) != EOF)
    // {
    //     printf("%c", ch);

    //     if (ch != ' ')
    //     {
    //         fputc(~ch, fp3);
    //     }
    //     else
    //     {
    //         fputc(ch, fp3);
    //     }
    // }

    fclose(fp);
    fclose(fp2);
    // fclose(fp3);

    return 0;
}