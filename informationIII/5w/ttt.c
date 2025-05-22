#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>

int main(void)
{
    FTS *ftsp;
    FTSENT *p;
    char *dotav[] = {"./", NULL};

    ftsp = fts_open(dotav, FTS_PHYSICAL, NULL);
    while ((p = fts_read(ftsp)) != NULL)
    {
        printf("fts_info: %d\n", p->fts_info);
        printf(",hardlink: %ld\n", p->fts_statp->st_nlink);
        switch (p->fts_info)
        {
        case FTS_F:
            printf("filename: %s, uid: %d\n", p->fts_name, p->fts_statp->st_uid);
            break;
        case FTS_D:
            printf("directory: %s, uid: %d\n", p->fts_name, p->fts_statp->st_uid);
            break;
        case FTS_SL:
            printf("symbolic link: %s, uid: %d\n", p->fts_name, p->fts_statp->st_uid);
            break;
        case FTS_SLNONE:
            printf("symbolic link not dereferenced: %s, uid: %d\n", p->fts_name, p->fts_statp->st_uid);
            break;
        }
        printf("\n");
    }
}