#include <stdio.h>
#include <stdlib.h>

/*NAME
    hexaPrint - prints the hexdecimal value of the input bytes from a given file
SYNOPSIS
    hexaPrint FILE
DESCRIPTION
    hexaPrint receives, as a command-line argument, the name of a "binary" file, and prints the hexadecimal value of each byte to the standard output, separated by spaces.
*/

void printHex(char *buffer, int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("%02hhx ", buffer[i]);
    }
}

int main(int argc, char **argv)
{
    FILE *file;
    char *arg = argv[1];
    file = fopen(arg, "r");
    char buffer[100];
    int result;
    if (file == NULL)
    {
        printf("Error: File not found\n");
        return 1;
    }
    int length = 0;
    while (result > 0){
        result = fread(&buffer[length], 1, 1, file);
        length++;
    }
    printHex(buffer, length);
    printf("\n");
    fclose(file);
    return 0;
}