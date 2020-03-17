
#ifndef H_STATION
#define H_STATION

#ifdef __cplusplus
extern "C"
{
#endif

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

#define GET_VALUE(target, jParent, jChild, vField)                   \
    (jChild) = cJSON_GetObjectItemCaseSensitive((jParent), #jChild); \
    if ((jChild) != NULL)                                            \
    {                                                                \
        (target) = (vField);                                         \
    }

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

#define CENTER_DISABLED 0

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

    struct _station;
    typedef struct _station Station;

    typedef struct
    {
        uint8_t id; // 04~0B 对应规范里的 master / slave
        ChannelType type;
        bool isConnnected;
        char readBuff[1024];
        uint16_t seq;
        uint8_t keepaliveTimer;
        ChannelParams params;
        uint8_t centerAddr;
        //reference
        Station *station;
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

    struct _station
    {
        Reactor *reactor;
        Config config;
    };

#define Station_config(_ptr) &(_ptr->config)

#ifdef __cplusplus
}
#endif
#endif