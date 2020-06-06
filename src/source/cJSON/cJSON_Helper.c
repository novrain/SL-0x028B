
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "cJSON/cJSON_Helper.h"
#include "common/class.h"

// cJSON read write helper
cJSON *cJSON_FromFile(char const *const file)
{
    cJSON *json = NULL;
    struct stat fStat = {0};
    int fd = 0;
    if (stat(file, &fStat) == 0 &&
        (fd = open(file, O_RDONLY), fd))
    {
        char *fContent = (char *)malloc(fStat.st_size + 1);
        memset(fContent, '\0', fStat.st_size + 1);
        if (read(fd, fContent, fStat.st_size) > 0)
        {
            json = cJSON_Parse(fContent);
        }
        free(fContent);
        if (fd > 0)
        {
            close(fd);
        }
    }
    return json;
}

int cJSON_WriteFile(cJSON *const json, char const *const file)
{
    int fd = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    int lenWrite = 0;
    if (fd > 0)
    {
        char *jstr = cJSON_Print(json);
        int len = strlen(jstr);
        lenWrite = write(fd, jstr, len);
        DelInstance(jstr);
        close(fd);
    }
    return lenWrite;
}
// cJSON END