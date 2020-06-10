// #include <unistd.h>
// #include "common/class.h"
#include "station.h"

// void *entry(void *arg)
// {
//     while (true)
//     {
//         usleep(5000 * 1000);
//         char *schemaName = "test";
//         Station_send(&station, schemaName, NULL);
//     }
// }

int main()
{
    char const *dir = ".";
    Station station = {0};
    Station_ctor(&station);
    // pthread_t *thread = NewInstance(pthread_t);
    // pthread_attr_t attr;
    // pthread_attr_init(&attr);
    // pthread_attr_setstacksize(&attr, 1024 * 10);
    // pthread_create(thread, &attr, &entry, NULL);
    int res = Station_StartBy(&station, dir);
    // loop exit
    Station_dtor(&station);
    return res;
}