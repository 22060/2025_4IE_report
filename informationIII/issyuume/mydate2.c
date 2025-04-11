#include <time.h>
#include <stdio.h>
#include <stdlib.h>
struct args
{
    int all;
    int ctime;
    int origin;
};
typedef struct args args_t;
args_t *parse_args(int argc, char *argv[]);
int main(int argc, char *argv[])
{
    args_t *args = parse_args(argc, argv);
    char buf[BUFSIZ];
    time_t tval;
    struct tm *pTime;
    tval = time(NULL);
    if (args->origin > 0)
    {
        pTime = args->origin == 2 ? gmtime(&tval) : localtime(&tval);
        strftime(buf, sizeof(buf), "%a %b %e %H:%M:%S %Z %Y", pTime);
        printf("%s\n", buf);
    }
    else
    {
        if (args->ctime)
        {
            printf("%s", ctime(&tval));
        }
        else
        {
            pTime = localtime(&tval);
            if (args->all)
            {
                strftime(buf, sizeof(buf), "%Y %B %e (%A) %T %p, %Z", pTime);
            }
            else
            {
                strftime(buf, sizeof(buf), "%a %b %e %H:%M:%S %Z %Y", pTime);
            }
            printf("%s\n", buf);
        }
    }
    return 0;
}

args_t *parse_args(int argc, char *argv[])
{
    static args_t argvs;
    argvs.all = 0;
    argvs.ctime = 0;
    argvs.origin = 0;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
            case 'a':
                argvs.all = 1;
                break;
            case 'c':
                argvs.ctime = 1;
                break;
            case 'o':
                if (i + 1 >= argc)
                {
                    argvs.origin = 1;
                }
                else if (argv[i + 1][0] == 'j' || argv[i + 1] == "JST")
                {
                    argvs.origin = 1;
                    i++;
                }
                else if (argv[i + 1][0] == 'g' || argv[i + 1] == "GMT")
                {
                    argvs.origin = 2;
                    i++;
                }
                break;
            case 'h':
                printf("Usage: %s [-a] [-c] [-o]\n", argv[0]);
                printf("Options:\n");
                printf("  -a    Show all date and time information\n");
                printf("  -c    Show ctime format\n");
                printf("  -o    Show original format\n");
                exit(EXIT_SUCCESS);
                break;
            default:
                fprintf(stderr, "Unknown option: %s\n", argv[i]);
                exit(EXIT_FAILURE);
            }
        }
    }
    return &argvs;
}