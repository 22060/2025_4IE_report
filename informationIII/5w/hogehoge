#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>

int main(void)
{
    FTS *ftsp;
    FTSENT *p;
    char *dotav[] = {".", NULL};

    ftsp = fts_open(dotav, FTS_PHYSICAL, NULL);
    while ((p = fts_read(ftsp)) != NULL)
    {
        // printf("path: %s\n", p->fts_path);
        switch (p->fts_info)
        {
        case FTS_F:
            printf("filename: %s, uid: %d\n", p->fts_name, p->fts_statp->st_uid);
            break;
        }
    }
}