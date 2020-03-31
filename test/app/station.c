#include "station.h"

int main()
{
    Station station = {0};
    char const *dir = "./";
    Station_ctor(&station);
    int res = Station_StartBy(&station, dir);
    // loop exit
    Station_dtor(&station);
    return res;
}