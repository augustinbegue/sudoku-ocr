#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        errx(1, "missing operand.\nTry --help for more information.");
    }

    char const *param1 = argv[1];

    if (strcmp(param1, "--help") == 0)
    {
        // --help argument -> print help message

        // TODO: Write the help message
        printf("Help message.\n");
    }
    else
    {
        // param1 contains the file path.
        if (access(param1, F_OK) == 0)
        {
        }
        else
        {
            // File doesn't exist.
            errx(1, "cannot open '%s': No such file or directory.", param1);
        }
    }

    return 0;
}
