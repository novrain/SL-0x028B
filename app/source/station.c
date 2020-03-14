#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef _WIN32
#include <winsock2.h>
#include <winsock.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "libev/ev.h"
#include "cJSON/cJSON.h"
#include "common/class.h"
#include "common/error.h"
#include "sl651/sl651.h"
#include "vec/vec.h"
#include "station.h"

typedef enum
{
    SL651_APP_ERROR_SUCCESS = ERROR_ENUM_BEGIN_RANGE(0),
} SL651AppError;

typedef enum
{
    CONFIG_CENTER_ADDRS,
    CONFIG_REMOTESTATION_ADDR,
    CONFIG_PASSWORD,
    CONFIG_CHANNEL_1,
    CONFIG_CHANNEL_2,
    CONFIG_CHANNEL_3,
    CONFIG_CHANNEL_4,
    CONFIG_WORK_MODE
} ConfigIdentifer;

typedef enum
{
    CHANNEL_DISABLED,
    CHANNEL_SMS,
    CHANNEL_IPV4,
    CHANNEL_BEIDOU,
    CHANNEL_INMARSAT,
    CHANNEL_PSTN,
    CHANNEL_ULTRASHORT_WAVE,
    CHANNEL_DOMAIN = 99 // 实现按照域名的方式，规范中没有定义
} ChannelType;

typedef enum
{
    REPORT,
    REPORT_CONFIRM,
    QUERY_ACK,
    MAINTAIN,
} WorkMode;

typedef struct
{
    uint8_t addr1;
    uint8_t addr2;
    uint8_t addr3;
    uint8_t addr4;
} CenterAddrs;

typedef union {
    struct in_addr ipv4;
    char *domain;
} ChannelParams;

typedef struct
{
    uint8_t id; // 04~0B 对应规范里的 master / slave
    ChannelType type;
    ChannelParams params;
} Channel;

// Dynamic Array for Element @see https://github.com/rxi/vec
typedef vec_t(Channel *) ChannelPtrVector;
/**
 * 都使用指针方式，解码编码都是可选项
 */
typedef struct
{
    CenterAddrs *centerAddrs;
    RemoteStationAddr *stationAddr;
    uint16_t *password;
    ChannelPtrVector channels;
    uint8_t *workMode;
} Config;

typedef struct
{
    Config config;
} Station;

cJSON *cJSON_fromFile(char const *const file)
{
    cJSON *json = NULL;
    struct stat fStat = {0};
    int fd = 0;
    if (stat(file, &fStat) == SL651_APP_ERROR_SUCCESS &&
        (fd = open(file, O_RDONLY), fd))
    {
        char *fContent = (char *)malloc(fStat.st_size);
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

bool Config_initFromJSON(Config *const me, cJSON *const json)
{
    assert(me);
    assert(json);
    // init channel array
    vec_init(&me->channels);
    vec_reserve(&me->channels, 4); // channel 04~07
    // 中心地址
    cJSON *centerAddrs = cJSON_GetObjectItemCaseSensitive(json, "centerAddrs");
    if (centerAddrs != NULL)
    {
        me->centerAddrs = NewInstance(CenterAddrs); // we DO NOT care whether it is NULL
        cJSON *addr1 = NULL;
        GET_VALUE(me->centerAddrs->addr1, centerAddrs, addr1, addr1->valuedouble);
        cJSON *addr2 = NULL;
        GET_VALUE(me->centerAddrs->addr2, centerAddrs, addr2, addr2->valuedouble);
        cJSON *addr3 = NULL;
        GET_VALUE(me->centerAddrs->addr3, centerAddrs, addr3, addr3->valuedouble);
        cJSON *addr4 = NULL;
        GET_VALUE(me->centerAddrs->addr4, centerAddrs, addr4, addr4->valuedouble);
    }
    else
    {
        return false;
    }
    // 密码
    me->password = NewInstance(uint16_t);
    *me->password = 0; // default but maybe not valid
    cJSON *password = NULL;
    GET_VALUE(*me->password, json, password, (uint16_t)password->valuedouble);
    // 工作模式
    me->workMode = NewInstance(uint8_t);
    *me->workMode = QUERY_ACK; //
    cJSON *workMode = NULL;
    GET_VALUE(*me->workMode, json, workMode, workMode->valuedouble);
    // channels
    cJSON *channels = cJSON_GetObjectItemCaseSensitive(json, "channels");
    cJSON *channel;
    cJSON_ArrayForEach(channel, channels)
    {
        ChannelType cType = CHANNEL_DISABLED;
        cJSON *type = NULL;
        GET_VALUE(cType, channel, type, (ChannelType)type->valuedouble);
        // pre define
        char *ipv4Str = NULL;
        cJSON *ipv4 = NULL;
        switch (cType)
        {
        case CHANNEL_IPV4:
            GET_VALUE(ipv4Str, channel, ipv4, ipv4->valuestring);
            if (ipv4Str != NULL)
            {
                Channel *ch = NewInstance(Channel);
                ch->type = cType;
#ifdef _WIN32
                if (inet_pton(AF_INET, ipv4Str, &ch->params.ipv4.s_addr) == SL651_APP_ERROR_SUCCESS)
#else
                if (inet_aton(ipv4Str, &ch->params.ipv4) != SL651_APP_ERROR_SUCCESS)
#endif
                {
                    vec_push(&me->channels, ch);
                }
                else
                {
                    DelInstance(ch);
                }
            }
            break;

        default:
            break;
        }
    }
    return false;
}

bool Station_startBy(Station *const me, char const *file)
{
    assert(me);
    assert(file && strlen(file) > 0);
    assert(me->config.centerAddrs == NULL); // assert all config is empty
    cJSON *json = cJSON_fromFile(file);
    if (json != NULL)
    {
        Config_initFromJSON(&me->config, json);
    }
    return false;
}

#define SL651_DEFAULT_CONFIGFILE "/sl651/config.json"

bool Station_start(Station *const me)
{
    char const *defaultFile = SL651_DEFAULT_CONFIGFILE;
    return Station_startBy(me, defaultFile);
}

int main()
{
    Station station = {0};
    char const *configFile = "./config.json";

    return Station_startBy(&station, configFile);
}
