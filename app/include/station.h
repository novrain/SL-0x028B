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
#include "tinydir/tinydir.h"

#include "packet_creator.h"

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
        REPORT = 1,
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
    typedef ev_timer ChannelFilesWatcher;
    typedef ev_async ChannelAsyncWatcher;

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
#define CHANNLE_DEFAULT_KEEPALIVE_INTERVAL 40

    typedef enum
    {
        CHANNEL_STATUS_RUNNING,
        CHANNEL_STATUS_WAITTING_FILESEND_ACK
    } ChannelStatus;
    typedef struct Channel
    {
        struct ChannelVtbl const *vptr;
        uint8_t id; // 04~0B 对应规范里的 master / slave
        ChannelType type;
        bool isConnnected;
        size_t buffSize;
        uint8_t msgSendInterval;
        char *buff;
        uint16_t seq;
        uint8_t keepaliveTimer;
        uint8_t centerAddr;
        ChannelHandlerPtrVector handlers; // 静态引用，不需要释放
        pthread_t *thread;
        ChannelStatus status;
        tinydir_file *currentFile;
        // recordsFile
        char *recordsFile;
        cJSON *recordsFileInJSON;
        pthread_mutex_t cleanUpMutex;
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
        void (*onFilesQuery)(Channel *const me);
        bool (*notifyData)(Channel *const me);
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
        ChannelFilesWatcher *filesWatcher;
        ChannelDataWatcher *dataWatcher;
        ChannelAsyncWatcher *asyncWatcher;
    } IOChannel;

    typedef struct IOChannelVtbl
    {
        struct ChannelVtbl super;
        void (*onConnectTimerEvent)(Reactor *reactor, ev_timer *w, int revents);
        void (*onIOReadEvent)(Reactor *reactor, ev_io *w, int revents);
        void (*onFilesScanTimerEvent)(Reactor *reactor, ev_timer *w, int revents);
        void (*onAsyncEvent)(Reactor *reactor, ev_async *w, int revents);
    } IOChannelVtbl;

    typedef struct SocketChannel
    {
        IOChannel supper;
        char *deviceStr;
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
        // extend config
        cJSON *configInJSON;
        char *configFile;
        char *workDir;
        char *filesDir;
        char *sentFilesDir;
        char *socketDevice;
        size_t *buffSize;
        uint16_t *msgSendInterval;
        char *schemasDir;
        // reference
        Station *station;
    } Config;
    Channel *Config_FindChannel(Config *const me, uint8_t chId);
    int32_t Config_IndexOfChannel(Config *const me, uint8_t chid);
#define CHANNEL_MIN_BUFF_SIZE 200
#define CHANNEL_MAX_BUFF_SIZE 10240
#define CHANNEL_DEFAULT_BUFF_SIZE 256

#define CHANNEL_MIN_MSG_SEND_INTERVAL 1
#define CHANNEL_MAX_MSG_SEND_INTERVAL 5000
#define CHANNEL_DEFAULT_MSG_SEND_INTERVAL 10

    typedef struct
    {
        Package *pkg;
        // 发送时根据当前的channel设置mask位，当全零时，表示可以清除
        uint16_t channelSentMask;
    } Packet;
    typedef vec_t(Packet *) PacketPtrVector;
    void Packet_dtor(Packet *const me);
#define Packet_IsSent(ptr_) (ptr_)->channelSentMask == 0

    struct _station
    {
        // Reactor *reactor;
        Config config;
        PacketCreatorFactory pcFactory;
        PacketPtrVector packets;
        pthread_mutex_t cleanUpMutex;
        pthread_mutex_t sendMutex;
    };
    void Station_ctor(Station *const me);
    bool Station_Start(Station *const me);
    bool Station_StartBy(Station *const me, char const *dir);
    void Station_dtor(Station *const me);
    bool Station_IsFileSentByAllChannel(Station *const me, tinydir_file *const file, Channel *const currentCh);
    // for other thread to call this function
    bool Station_AsyncSend(Station *const me, char *const schemaName, cJSON *const data);
    void Station_SendPacketsToChannel(Station *const me, Channel *const ch);
#define SL651_DEFAULT_WORKDIR "/sl651"
#define SL651_DEFAULT_CONFIG_FILE_NAME_LEN 11
    // #define SL651_DEFAULT_CONFIGFILE "/sl651/config.json"

#define Station_config(_ptr) &(_ptr->config)

#ifdef __cplusplus
}
#endif
#endif