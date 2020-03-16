#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

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

typedef struct
{
    struct sockaddr_in addr;
    int sock;
} Ipv4;

typedef struct
{
    Ipv4 ipv4;
    char *domainStr;
} Domain;

typedef union {
    Ipv4 ipv4;
    Domain domain;
} ChannelParams;

typedef struct ev_loop Reactor;
typedef ev_timer ChannleConnectWatcher;
typedef ev_io ChannleDataWatcher;

typedef enum
{
    CHANNEL_ID_MASTER_01 = 4,
    CHANNEL_ID_SLAVE_01,
    CHANNEL_ID_MASTER_02,
    CHANNEL_ID_SLAVE_02,
    CHANNEL_ID_MASTER_03,
    CHANNEL_ID_SLAVE_03,
    CHANNEL_ID_MASTER_04,
    CHANNEL_ID_SLAVE_04,
    CHANNEL_ID_FIXED = 99, // internal private id
} ChannelId;

typedef struct
{
    uint8_t id; // 04~0B 对应规范里的 master / slave
    ChannelType type;
    bool isConnnected;
    char readBuff[1024];
    ChannelParams params;
    //libev
    Reactor *reactor; // just a reference NO NEED TO FREE by channel
    //
    ChannleConnectWatcher *connectWatcher;
    ChannleDataWatcher *dataWatcher;
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
    Reactor *reactor;
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

void Channel_dtor(Channel *const me)
{
    assert(me);
    // stop and release watcher
}

Channel *Channel_ipv4FromJson(cJSON *const channelInJson)
{
    assert(channelInJson);
    uint8_t idU8 = 0;
    char *ipv4Str = NULL;
    uint16_t ipv4Port = 0;
    cJSON *id = NULL;
    cJSON *ipv4 = NULL;
    cJSON *port = NULL;
    GET_VALUE(idU8, channelInJson, id, id->valuedouble);
    GET_VALUE(ipv4Str, channelInJson, ipv4, ipv4->valuestring);
    GET_VALUE(ipv4Port, channelInJson, port, port->valuedouble);
    if (ipv4Str != NULL && port > 0 &&
        idU8 >= CHANNEL_ID_MASTER_01 && idU8 <= CHANNEL_ID_SLAVE_04)
    {
        Channel *ch = NewInstance(Channel);
        ch->id = idU8;
        ch->type = CHANNEL_IPV4;
        ch->params.ipv4.addr.sin_family = AF_INET;
        ch->params.ipv4.addr.sin_port = htons(ipv4Port);
#ifdef _WIN32
        if (inet_pton(AF_INET, ipv4Str, &ch->params.ipv4.addr.sin_addr) == 1)
#else
        if (inet_aton(ipv4Str, &ch->params.ipv4.sin_addr) == 1)
#endif
        {
            return ch;
        }
        else
        {
            Channel_dtor(ch);
            DelInstance(ch);
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

Channel *Channel_domainFromJson(cJSON *const channelInJson)
{
    assert(channelInJson);
    uint8_t idU8 = 0;
    char *domainStr = NULL;
    uint16_t ipv4Port = 0;
    cJSON *id = NULL;
    cJSON *domain = NULL;
    cJSON *port = NULL;
    GET_VALUE(idU8, channelInJson, id, id->valuedouble);
    GET_VALUE(domainStr, channelInJson, domain, domain->valuestring);
    GET_VALUE(ipv4Port, channelInJson, port, port->valuedouble);
    if (domainStr != NULL && strlen(domainStr) > 0 && port > 0 &&
        idU8 >= CHANNEL_ID_MASTER_01 && idU8 <= CHANNEL_ID_SLAVE_04)
    {
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            return NULL;
        }
#else

#endif
        struct hostent *hosts = gethostbyname(domainStr);
        if (hosts == NULL || hosts->h_addrtype != AF_INET)
        {
            uint16_t err = WSAGetLastError();
            return NULL;
        }
        Channel *ch = NewInstance(Channel);
        ch->id = idU8;
        ch->type = CHANNEL_DOMAIN;
        ch->params.domain.domainStr = domainStr;
        ch->params.domain.ipv4.addr.sin_family = AF_INET;
        ch->params.domain.ipv4.addr.sin_port = htons(ipv4Port);
        // GET THE FIRST IP
        ch->params.domain.ipv4.addr.sin_addr = *(struct in_addr *)hosts->h_addr_list[0];
        return ch;
    }
    else
    {
        return NULL;
    }
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
        Channel *ch = NULL;
        switch (cType)
        {
        case CHANNEL_IPV4: // 目前只处理IPV4, 其他认为无效
            ch = Channel_ipv4FromJson(channel);
            break;
        case CHANNEL_DOMAIN: // 自定义的域名方式
            ch = Channel_domainFromJson(channel);
            break;
        default:
            break;
        }
        if (ch != NULL)
        {
            vec_push(&me->channels, ch);
        }
    }
    return true;
}

bool Config_isValid(Config *const config)
{
    return config != NULL && config->channels.length > 0;
}

void onIPV4ChannelData(Reactor *reactor, ev_io *w, int revents)
{
    int len = 0;
    Channel *ch = (Channel *)w->data;
    len = recv(w->fd, ch->readBuff, 1024, 0);
    if (len <= 0)
    {
        ev_io_stop(reactor, ch->dataWatcher);
        ch->isConnnected = false;
#ifdef _WIN32
        closesocket(w->fd);
#else
        close(w->fd);
#endif
        ev_timer_again(reactor, ch->connectWatcher);
        return;
    }
    printf("recv :%s\n", ch->readBuff);
}

bool setSocketBlockingEnabled(int fd, bool blocking)
{
    if (fd < 0)
        return false;

#ifdef _WIN32
    unsigned long mode = blocking ? 0 : 1;
    return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
#else
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return false;
    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
#endif
}

void startAIPV4Channel(Reactor *reactor, ev_timer *w, int revents)
{
    w->repeat = 10; // retry every ten seconds.
    Channel *ch = (Channel *)w->data;
    if (ch->isConnnected)
    {
        return;
    }
    Ipv4 *ipv4 = NULL;
    switch (ch->type)
    {
    case CHANNEL_IPV4:
        ipv4 = &ch->params.ipv4;
        break;
    case CHANNEL_DOMAIN:
        ipv4 = &ch->params.domain.ipv4;
        printf("connect: %s\n", ch->params.domain.domainStr);
        break;
    default:
        return;
    }
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        ev_timer_again(reactor, w);
        return;
    }
    if (connect(sock, (struct sockaddr *)ipv4, sizeof(struct sockaddr_in)) < 0)
    {
        close(sock);
        ev_timer_again(reactor, w);
        return;
    }
    // 开始一个io
    setSocketBlockingEnabled(sock, false);
    ipv4->sock = sock;
    if (ch->dataWatcher != NULL)
    {
        ev_io_stop(reactor, ch->dataWatcher);
        ev_io_set(ch->dataWatcher, sock, EV_READ);
        ev_io_start(reactor, ch->dataWatcher);
    }
    else
    {
        ChannleDataWatcher *dataWatcher = NewInstance(ev_io);
        if (dataWatcher != NULL)
        {
            ev_io_init(dataWatcher, onIPV4ChannelData, sock, EV_READ);
            ch->dataWatcher = dataWatcher;
            dataWatcher->data = ch;
            ev_io_start(reactor, ch->dataWatcher);
        }
    }
    ch->isConnnected = true;
}

bool Station_startBy(Station *const me, char const *file)
{
    assert(me);
    assert(file && strlen(file) > 0);
    assert(me->config.centerAddrs == NULL); // assert all config is empty
    cJSON *json = cJSON_fromFile(file);
    if (json != NULL &&
        Config_initFromJSON(&me->config, json) &&
        Config_isValid(&me->config))
    {
        me->reactor = ev_loop_new(0);
        if (me->reactor != NULL)
        {
            int i;
            Channel *ch = NULL;
            vec_foreach(&me->config.channels, ch, i)
            {
                if (ch->type == CHANNEL_IPV4 || ch->type == CHANNEL_DOMAIN)
                {
                    ChannleConnectWatcher *connectWatcher = NewInstance(ChannleConnectWatcher); // 启动定时器，连接，如果出错，自动重连
                    if (connectWatcher != NULL)
                    {
                        ev_init(connectWatcher, startAIPV4Channel);
                        connectWatcher->repeat = 1; // start as fast as posible
                        ch->reactor = me->reactor;
                        connectWatcher->data = (void *)ch;
                        ch->connectWatcher = connectWatcher;
                        ev_timer_again(me->reactor, connectWatcher);
                    }
                }
            }
            // dead loop until no event to be handle
            if (ev_run(me->reactor, 0))
            {
                return true;
            }
            return false;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

#define SL651_DEFAULT_CONFIGFILE "/sl651/config.json"

bool Station_start(Station *const me)
{
    char const *defaultFile = SL651_DEFAULT_CONFIGFILE;
    return Station_startBy(me, defaultFile);
}

void Station_dtor(Station *const me)
{
    int i;
    Channel *ch = NULL;
    vec_foreach(&me->config.channels, ch, i)
    {
        if (ch != NULL)
        {
            Channel_dtor(ch);
        }
    }

    if (me->reactor)
    {
        ev_loop_destroy(me->reactor);
    }
}

int main()
{
    Station station = {0};
    char const *configFile = "./config.json";

    int res = Station_startBy(&station, configFile);
    // loop exit
    Station_dtor(&station);
    return res;
}
