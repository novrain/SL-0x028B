
#include "station.h"

// cJSON read write helper
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
// cJSON END

// Virtual Channel

uint16_t Channel_seq(Channel *const me)
{
    return me->seq++;
}

void Channel_Start(Channel *const me)
{
    assert(0);
}

bool Channel_Open(Channel *const me)
{
    assert(0);
    return false;
}

void Channel_Close(Channel *const me)
{
    assert(0);
}

void Channel_Keepalive(Channel *const me)
{
    assert(me);
    assert(me->station);
    if (!me->isConnnected)
    {
        return;
    }
    Config *config = Station_config(me->station);
    // create keepalive package
    UplinkMessage *msg = NewInstance(UplinkMessage); // 选择是上行还是下行
    UplinkMessage_ctor(msg, 0);                      // 调用构造函数,如果有要素，需要指定要素数量
    Package *pkg = (Package *)msg;                   // 获取父结构Package
    Head *head = &pkg->head;                         // 获取Head结构
    head->centerAddr = me->centerAddr;               // 填写Head结构
    head->funcCode = KEEPALIVE;                      // 心跳功能码功能码
    head->password = *config->password;              // 密码
    head->stationAddr = *config->stationAddr;        // 站地址
    pkg->tail.etxFlag = ETX;                         // 截至符
    msg->messageHead.seq = Channel_seq(me);          // 根据功能码填写报文头
    ByteBuffer *byteOut = pkg->vptr->encode(pkg);    // 编码
    BB_Flip(byteOut);                                // 转为读模式
    me->vptr->send(me, byteOut);                     //
    UplinkMessage_dtor((Package *)msg);              // 析构
    DelInstance(msg);                                // free
    BB_dtor(byteOut);                                // 析构
    DelInstance(byteOut);                            // free
}

ByteBuffer *Channel_OnRead(Channel *const me)
{
    assert(0);
    return NULL;
}

bool Channel_Send(Channel *const me, ByteBuffer *const buff)
{
    assert(0);
    return false;
}

void Channel_dtor(Channel *const me)
{
    assert(me);
}

void Channel_ctor(Channel *me, Station *const station)
{
    assert(me);
    // assert(station);
    static ChannelVtbl const vtbl = {
        &Channel_Start,
        &Channel_Open,
        &Channel_Close,
        &Channel_Keepalive,
        &Channel_OnRead,
        &Channel_Send,
        &Channel_dtor};
    me->vptr = &vtbl;
    me->station = station;
}
// Virtual Channel END

// Abstract IOChannel
void IOChannel_Start(Channel *const me)
{
    assert(me);
    IOChannel *ioCh = (IOChannel *)me;
    ChannleConnectWatcher *connectWatcher = NewInstance(ChannleConnectWatcher); // 启动定时器，连接，如果出错，自动重连
    if (connectWatcher != NULL)
    {
        ev_init(connectWatcher, ((IOChannelVtbl *)me->vptr)->onConnectTimerEvent);
        connectWatcher->repeat = 1; // start as fast as posible
        ioCh->reactor = me->station->reactor;
        connectWatcher->data = (void *)me;
        ioCh->connectWatcher = connectWatcher;
        ev_timer_again(ioCh->reactor, connectWatcher);
    }
}

bool IOChannel_Open(Channel *const me)
{
    assert(0);
    return false;
}

void IOChannel_Close(Channel *const me)
{
    assert(0);
}

void IOChannel_Keepalive(Channel *const me)
{
    Channel_Keepalive(me);
}

ByteBuffer *IOChannel_OnRead(Channel *const me)
{
    assert(0);
    return NULL;
}

bool IOChannel_Send(Channel *const me, ByteBuffer *buff)
{
    assert(0);
    return false;
}

void IOChannel_OnConnectTimerEvent(Reactor *reactor, ev_timer *w, int revents)
{
    IOChannel *ioCh = (IOChannel *)w->data;
    Channel *ch = (Channel *)ioCh;
    if (ch->isConnnected)
    {
        Channel_Keepalive(ch);
        return;
    }
    if (!ch->vptr->open(ch))
    {
        ev_timer_again(reactor, w);
        return;
    }
    int fd = ioCh->fd;
    if (ioCh->dataWatcher != NULL)
    {
        ev_io_stop(reactor, ioCh->dataWatcher);
        ev_io_set(ioCh->dataWatcher, fd, EV_READ);
        ev_io_start(reactor, ioCh->dataWatcher);
    }
    else
    {
        ChannleDataWatcher *dataWatcher = NewInstance(ev_io);
        if (dataWatcher != NULL)
        {
            ev_io_init(dataWatcher, ((IOChannelVtbl *)ch->vptr)->onIOReadEvent, fd, EV_READ);
            ioCh->dataWatcher = dataWatcher;
            dataWatcher->data = ioCh;
            ev_io_start(reactor, ioCh->dataWatcher);
        }
    }
    ioCh->connectWatcher->repeat = ch->keepaliveTimer;
    ch->isConnnected = true;
}

void IOChannel_OnIOReadEvent(Reactor *reactor, ev_io *w, int revents)
{
    int len = 0;
    IOChannel *ioCh = (IOChannel *)w->data;
    Channel *ch = (Channel *)ioCh;
    ByteBuffer *buff = ch->vptr->onRead(ch);
    if (buff == NULL)
    {
        ev_io_stop(ioCh->reactor, ioCh->dataWatcher);
        ch->isConnnected = false;
        ch->vptr->close(ch);
        ioCh->connectWatcher->repeat = 10;
        ev_timer_again(ioCh->reactor, ioCh->connectWatcher);
        return;
    }
    Package *pkg = decodePackage(buff);
    if (pkg != NULL)
    {
        Package_dtor(pkg);
        DelInstance(pkg);
    }
    else
    {
        printf("recv :%s\n", ch->readBuff);
    }
    BB_dtor(buff);
    DelInstance(buff);
}

void IOChannel_dtor(Channel *const me)
{
    assert(me);
    Channel_dtor(me);
}

void IOChannel_ctor(IOChannel *me, Station *const station)
{
    assert(me);
    // assert(station);
    static IOChannelVtbl const vtbl = {
        {&IOChannel_Start,
         &IOChannel_Open,
         &IOChannel_Close,
         &IOChannel_Keepalive,
         &IOChannel_OnRead,
         &IOChannel_Send,
         &IOChannel_dtor},
        &IOChannel_OnConnectTimerEvent,
        &IOChannel_OnIOReadEvent};
    Channel *super = (Channel *)me;
    Channel_ctor(super, station);
    super->vptr = (const ChannelVtbl *)(&vtbl);
}
// Abstract IOChannel END

// SocketChannel
void SocketChannel_Start(Channel *const me)
{
    assert(me);
    IOChannel_Start(me);
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

bool SocketChannel_Connect(Channel *const me)
{
    assert(me);
    IOChannel *ioCh = (IOChannel *)me;
    Channel *ch = (Channel *)ioCh;
    Ipv4 *ipv4 = ((SocketChannelVtbl *)ch->vptr)->ip((SocketChannel *)me);
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        return false;
    }
    if (connect(sock, (struct sockaddr *)ipv4, sizeof(struct sockaddr_in)) < 0)
    {
        close(sock);
        return false;
    }
    setSocketBlockingEnabled(sock, false);
    ioCh->fd = sock;
    return true;
}

void SocketChannel_Close(Channel *const me)
{
    IOChannel *ioCh = (IOChannel *)me;
    int sock = ioCh->fd;
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
}

void SocketChannel_Keepalive(Channel *const me)
{
    assert(0);
}

ByteBuffer *SocketChannel_OnRead(Channel *const me)
{
    IOChannel *ioCh = (IOChannel *)me;
    Channel *ch = (Channel *)ioCh;
    int sock = ioCh->fd;
    int len = recv(sock, ch->readBuff, 1024, 0);
    if (len <= 0)
    {
        return NULL;
    }
    // printf("recv :%s\n", ch->readBuff);
    ByteBuffer *buff = NewInstance(ByteBuffer);
    BB_ctor_wrapped(buff, (uint8_t *)ch->readBuff, len);
    return buff;
}

bool SocketChannel_Send(Channel *const me, ByteBuffer *const buff)
{
    assert(me);
    assert(buff);
    if (me->isConnnected)
    {
        IOChannel *ioCh = (IOChannel *)me;
        Channel *ch = (Channel *)ioCh;
        int sock = ioCh->fd;
        int len = BB_Available(buff);
        return len == send(sock, (const char *)buff->buff, len, 0);
    }
    else
    {
        return false;
    }
}

void SocketChannel_OnConnectTimerEvent(Reactor *reactor, ev_timer *w, int revents)
{
    IOChannel_OnConnectTimerEvent(reactor, w, revents);
}

void SocketChannel_OnIOReadEvent(Reactor *reactor, ev_io *w, int revents)
{
    IOChannel_OnIOReadEvent(reactor, w, revents);
}

Ipv4 *SocketChannel_Ip(SocketChannel *me)
{
    assert(0);
    return NULL;
}

void SocketChannel_dtor(Channel *const me)
{
    assert(me);
    IOChannel_dtor(me);
}

void SocketChannel_ctor(SocketChannel *me, Station *const station)
{
    assert(me);
    // assert(station);
    static SocketChannelVtbl const vtbl = {
        {{&SocketChannel_Start,
          &SocketChannel_Connect,
          &SocketChannel_Close,
          &SocketChannel_Keepalive,
          &SocketChannel_OnRead,
          &SocketChannel_Send,
          &SocketChannel_dtor},
         &SocketChannel_OnConnectTimerEvent,
         &SocketChannel_OnIOReadEvent},
        &SocketChannel_Ip};
    IOChannel *super = (IOChannel *)me;
    IOChannel_ctor(super, station);
    Channel *ch = (Channel *)me;
    ch->vptr = (const ChannelVtbl *)(&vtbl);
}
// SocketChannel END

// Ipv4Channel
Ipv4 *Ipv4Channel_Ip(SocketChannel *me)
{
    assert(me);
    return &((Ipv4Channel *)me)->ipv4;
}

void Ipv4Channel_dtor(Channel *me)
{
    assert(me);
    SocketChannel_dtor(me);
}

void Ipv4Channel_ctor(Ipv4Channel *me, Station *const station)
{
    assert(me);
    // assert(station);
    static SocketChannelVtbl const vtbl = {
        {{&SocketChannel_Start,
          &SocketChannel_Connect,
          &SocketChannel_Close,
          &SocketChannel_Keepalive,
          &SocketChannel_OnRead,
          &SocketChannel_Send,
          &Ipv4Channel_dtor},
         &SocketChannel_OnConnectTimerEvent,
         &SocketChannel_OnIOReadEvent},
        &Ipv4Channel_Ip};
    SocketChannel *super = (SocketChannel *)me;
    SocketChannel_ctor(super, station);
    Channel *ch = (Channel *)me;
    ch->vptr = (const ChannelVtbl *)&vtbl;
}
// Ipv4Channel END

// DomainChannel
Ipv4 *DomainChannel_Ip(SocketChannel *me)
{
    assert(me);
    return &((DomainChannel *)me)->domain.ipv4;
}

void DomainChannel_ctor(DomainChannel *me, Station *const station)
{
    assert(me);
    // assert(station);
    static SocketChannelVtbl const vtbl = {
        {{&SocketChannel_Start,
          &SocketChannel_Connect,
          &SocketChannel_Close,
          &SocketChannel_Keepalive,
          &SocketChannel_OnRead,
          &SocketChannel_Send,
          &SocketChannel_dtor},
         &SocketChannel_OnConnectTimerEvent,
         &SocketChannel_OnIOReadEvent},
        &DomainChannel_Ip};
    SocketChannel *super = (SocketChannel *)me;
    SocketChannel_ctor(super, station);
    Channel *ch = (Channel *)me;
    ch->vptr = (const ChannelVtbl *)&vtbl;
}
// DomainChannel END

Channel *Channel_ipv4FromJson(cJSON *const channelInJson)
{
    assert(channelInJson);
    uint8_t idU8 = 0;
    char *ipv4Str = NULL;
    uint16_t ipv4Port = 0;
    uint8_t keepaliveU8 = 40; // default value
    cJSON *id = NULL;
    cJSON *ipv4 = NULL;
    cJSON *port = NULL;
    cJSON *keepalive = NULL;
    GET_VALUE(idU8, channelInJson, id, id->valuedouble);
    GET_VALUE(ipv4Str, channelInJson, ipv4, ipv4->valuestring);
    GET_VALUE(ipv4Port, channelInJson, port, port->valuedouble);
    GET_VALUE(keepaliveU8, channelInJson, keepalive, keepalive->valuedouble);
    if (ipv4Str != NULL && port > 0 &&
        idU8 >= CHANNEL_ID_MASTER_01 && idU8 <= CHANNEL_ID_SLAVE_04)
    {
        Ipv4Channel *ch = NewInstance(Ipv4Channel);
        Ipv4Channel_ctor(ch, NULL); // 初始化 station 为 NULL
        Channel *super = (Channel *)ch;
        super->id = idU8;
        super->type = CHANNEL_IPV4;
        super->keepaliveTimer = keepaliveU8;
        ch->ipv4.addr.sin_family = AF_INET;
        ch->ipv4.addr.sin_port = htons(ipv4Port);
#ifdef _WIN32
        if (inet_pton(AF_INET, ipv4Str, &ch->ipv4.addr.sin_addr) == 1)
#else
        if (inet_aton(ipv4Str, &ch->ipv4.sin_addr) == 1)
#endif
        {
            return (Channel *)ch;
        }
        else
        {
            Ipv4Channel_dtor(super);
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
    uint8_t keepaliveU8 = 40; // default value
    cJSON *id = NULL;
    cJSON *domain = NULL;
    cJSON *port = NULL;
    cJSON *keepalive = NULL;
    GET_VALUE(idU8, channelInJson, id, id->valuedouble);
    GET_VALUE(domainStr, channelInJson, domain, domain->valuestring);
    GET_VALUE(ipv4Port, channelInJson, port, port->valuedouble);
    GET_VALUE(keepaliveU8, channelInJson, keepalive, keepalive->valuedouble);
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
            return NULL;
        }
        DomainChannel *ch = NewInstance(DomainChannel);
        DomainChannel_ctor(ch, NULL); // 初始化 station 为 NULL
        Channel *super = (Channel *)ch;
        super->id = idU8;
        super->type = CHANNEL_DOMAIN;
        super->keepaliveTimer = keepaliveU8;
        ch->domain.domainStr = domainStr;
        ch->domain.ipv4.addr.sin_family = AF_INET;
        ch->domain.ipv4.addr.sin_port = htons(ipv4Port);
        // GET THE FIRST IP
        ch->domain.ipv4.addr.sin_addr = *(struct in_addr *)hosts->h_addr_list[0];
        return (Channel *)ch;
    }
    else
    {
        return NULL;
    }
}

uint8_t Config_centerAddr(Config *const me, uint8_t id)
{
    assert(me);
    if (me->centerAddrs == NULL || id < 1 || id > 4)
    {
        return 0;
    }
    uint8_t *p = (uint8_t *)me->centerAddrs;
    return *(p + id - 1);
}

bool Config_isCenterAddrEnable(Config *const me, uint8_t id)
{
    return Config_centerAddr(me, id) != 0;
}

uint8_t Config_centerAddrOfChannel(Config *const me, Channel *const ch)
{
    assert(me);
    if (ch == NULL)
    {
        return 0;
    }
    uint8_t id = ch->id - CHANNEL_ID_MASTER_01 + 1; //
    return Config_centerAddr(me, id);
}

bool Config_isChannelEnable(Config *const me, Channel *const ch)
{
    assert(me);
    return ch->id == CHANNEL_ID_FIXED || Config_centerAddrOfChannel(me, ch) != CENTER_DISABLED;
}

bool Config_initFromJSON(Config *const me, cJSON *const json)
{
    assert(me);
    assert(json);
    // 中心地址
    cJSON *centerAddrs = cJSON_GetObjectItemCaseSensitive(json, "centerAddrs");
    // 站地址
    cJSON *remoteStationAddr = cJSON_GetObjectItemCaseSensitive(json, "remoteStationAddr");
    if (centerAddrs == NULL || remoteStationAddr == NULL)
    {
        return false;
    }
    // init channel array
    vec_init(&me->channels);
    vec_reserve(&me->channels, 4); // channel 04~07
    // 中心地址
    me->centerAddrs = NewInstance(CenterAddrs); // we DO NOT care whether it is NULL
    cJSON *addr1 = NULL;
    GET_VALUE(me->centerAddrs->addr1, centerAddrs, addr1, addr1->valuedouble);
    cJSON *addr2 = NULL;
    GET_VALUE(me->centerAddrs->addr2, centerAddrs, addr2, addr2->valuedouble);
    cJSON *addr3 = NULL;
    GET_VALUE(me->centerAddrs->addr3, centerAddrs, addr3, addr3->valuedouble);
    cJSON *addr4 = NULL;
    GET_VALUE(me->centerAddrs->addr4, centerAddrs, addr4, addr4->valuedouble);
    // 站地址
    me->stationAddr = NewInstance(RemoteStationAddr);
    cJSON *A5 = NULL;
    GET_VALUE(me->stationAddr->A5, remoteStationAddr, A5, A5->valuedouble);
    cJSON *A4 = NULL;
    GET_VALUE(me->stationAddr->A4, remoteStationAddr, A4, A4->valuedouble);
    cJSON *A3 = NULL;
    GET_VALUE(me->stationAddr->A3, remoteStationAddr, A3, A3->valuedouble);
    cJSON *A2 = NULL;
    GET_VALUE(me->stationAddr->A2, remoteStationAddr, A2, A2->valuedouble);
    cJSON *A1 = NULL;
    GET_VALUE(me->stationAddr->A1, remoteStationAddr, A1, A1->valuedouble);
    cJSON *A0 = NULL;
    GET_VALUE(me->stationAddr->A0, remoteStationAddr, A0, A0->valuedouble);
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
    // @Todo add a fixed channel to make it reachalbe
    return true;
}

bool Config_isValid(Config *const config)
{
    return config != NULL && config->channels.length > 0;
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
                if (Config_isChannelEnable(&me->config, ch) &&
                    (ch->type == CHANNEL_IPV4 || ch->type == CHANNEL_DOMAIN))
                {
                    ch->station = me;
                    ch->centerAddr = Config_centerAddrOfChannel(&me->config, ch);
                    ch->vptr->start(ch);
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
