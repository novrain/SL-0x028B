
#ifndef H_STATION
#define H_STATION

#ifdef __cplusplus
extern "C"
{
#endif
#include <pthread.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "libev/ev.h"
#include "cJSON/cJSON.h"
#include "vec/vec.h"
#include "common/error.h"
#include "sl651/sl651.h"

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
        CONFIG_CENTER_ADDRS = 1,
        CONFIG_REMOTESTATION_ADDR,
        CONFIG_PASSWORD,
        CONFIG_CHANNEL_1_MASTER,
        CONFIG_CHANNEL_1_SLAVE,
        CONFIG_CHANNEL_2_MASTER,
        CONFIG_CHANNEL_2_SLAVE,
        CONFIG_CHANNEL_3_MASTER,
        CONFIG_CHANNEL_3_SLAVE,
        CONFIG_CHANNEL_4_MASTER,
        CONFIG_CHANNEL_4_SLAVE,
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
    } Ipv4;

    typedef struct
    {
        Ipv4 ipv4;
        char *domainStr;
    } Domain;

    typedef struct ev_loop Reactor;
    typedef ev_timer ChannelConnectWatcher;
    typedef ev_io ChannelDataWatcher;

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

    struct ChannelVtbl;
    struct Channel;

    typedef struct
    {
        FunctionCode code;
        bool (*cb)(struct Channel *const ch, Package *const request);
    } ChannelHandler;

    typedef vec_t(ChannelHandler *) ChannelHandlerPtrVector;
#define CHANNEL_RESERVED_HANDLER_SIZE 10
#define CHANNLE_RECIVE_BUFF_SIZE 2048
#define CHANNLE_DEFAULT_KEEPALIVE_INTERVAL 40
    typedef struct Channel
    {
        struct ChannelVtbl const *vptr;
        uint8_t id; // 04~0B 对应规范里的 master / slave
        ChannelType type;
        bool isConnnected;
        char readBuff[CHANNLE_RECIVE_BUFF_SIZE];
        uint16_t seq;
        uint8_t keepaliveTimer;
        uint8_t centerAddr;
        ChannelHandlerPtrVector handlers; // 静态引用，不需要释放
        pthread_t *thread;
        // reference
        Station *station;
    } Channel;

    typedef struct ChannelVtbl
    {
        void (*start)(Channel *const me);
        bool (*open)(Channel *const me);
        void (*close)(Channel *const me);
        void (*keepalive)(Channel *const me);
        ByteBuffer *(*onRead)(Channel *const me);
        bool (*send)(Channel *const me, ByteBuffer *const buff);
        bool (*expandEncode)(Channel *const me, ByteBuffer *const buff);
        void (*dtor)(Channel *const me);
    } ChannelVtbl;

    typedef struct IOChannel
    {
        Channel supper;
        int fd;
        // libev
        Reactor *reactor;
        // Watcher for IO
        ChannelConnectWatcher *connectWatcher;
        ChannelDataWatcher *dataWatcher;
    } IOChannel;

    typedef struct IOChannelVtbl
    {
        struct ChannelVtbl super;
        void (*onConnectTimerEvent)(Reactor *reactor, ev_timer *w, int revents);
        void (*onIOReadEvent)(Reactor *reactor, ev_io *w, int revents);
    } IOChannelVtbl;

    typedef struct SocketChannel
    {
        IOChannel supper;
    } SocketChannel;

    typedef struct SocketChannelVtbl
    {
        struct IOChannelVtbl super;
        Ipv4 *(*ip)(SocketChannel *me);
    } SocketChannelVtbl;

    typedef struct Ipv4Channel
    {
        SocketChannel super;
        Ipv4 ipv4;
    } Ipv4Channel;

    typedef struct DomainChannel
    {
        SocketChannel super;
        Domain domain;
    } DomainChannel;

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
        cJSON *configInJSON;
        char *configFile;
        char *workDir;
        char *filesDir;
    } Config;
    Channel *Config_FindChannel(Config *const me, uint8_t chId);
    int32_t Config_IndexOfChannel(Config *const me, uint8_t chid);

    struct _station
    {
        // Reactor *reactor;
        Config config;
    };
    void Station_ctor(Station *const me);
    bool Station_Start(Station *const me);
    bool Station_StartBy(Station *const me, char const *dir);
    void Station_dtor(Station *const me);
#define SL651_DEFAULT_WORKDIR "/sl651"
#define SL651_DEFAULT_CONFIG_FILE_NAME_LEN 11
    // #define SL651_DEFAULT_CONFIGFILE "/sl651/config.json"

#define Station_config(_ptr) &(_ptr->config)

#ifdef __cplusplus
}
#endif
#endif