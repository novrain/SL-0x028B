
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
void Channel_FillUplinkMessageHead(Channel *const me, UplinkMessage *const upMsg)
{
    assert(me);
    assert(upMsg);
    Package *pkg = (Package *)upMsg;
    Config *config = &me->station->config;
    pkg->head.centerAddr = me->centerAddr;
    pkg->head.stationAddr = *config->stationAddr;
    pkg->head.password = *config->password;
    DateTime_now(&upMsg->messageHead.sendTime);
    ObserveTime_now(&upMsg->messageHead.observeTimeElement.observeTime);
    upMsg->messageHead.stationAddrElement.stationAddr = *config->stationAddr;
}

uint16_t Channel_nextSeq(Channel *const me)
{
    return me->seq++;
}

uint16_t Channel_lastSeq(Channel *const me)
{
    return me->seq;
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
    // create keepalive package
    UplinkMessage *msg = NewInstance(UplinkMessage); // 选择是上行还是下行
    UplinkMessage_ctor(msg, 0);                      // 调用构造函数,如果有要素，需要指定要素数量
    Package *pkg = (Package *)msg;                   // 获取父结构Package
    Head *head = &pkg->head;                         // 获取Head结构
    Channel_FillUplinkMessageHead(me, msg);          // Fill head by config
    head->funcCode = KEEPALIVE;                      // 心跳功能码功能码
    msg->messageHead.seq = Channel_lastSeq(me);      // 根据功能码填写报文头
    pkg->tail.etxFlag = ETX;                         // 截止符
    ByteBuffer *byteOut = pkg->vptr->encode(pkg);    // 编码
    BB_Flip(byteOut);                                // 转为读模式
    me->vptr->send(me, byteOut);                     // 调用发送实现
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

bool Channel_ExpandEncode(Channel *const me, ByteBuffer *const buff)
{
    assert(0);
    return false;
}

void Channel_dtor(Channel *const me)
{
    assert(me);
}

// HANDELERS
bool handleTEST(Channel *const ch, Package *const request)
{
    assert(ch);
    assert(request);
    assert(request->head.funcCode == TEST);
    DownlinkMessage *downMsg = (DownlinkMessage *)request;
    Package *pkg = NULL;
    LinkMessage *linkMsg = NULL;
    UplinkMessage *upMsg = NULL;
    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    // FROM AN FIX TEST PKG
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "01"
                                 "0012345678"
                                 "1234"
                                 "30"
                                 "002B"
                                 "02"
                                 "0003"
                                 "591011154947"
                                 "F1F1"
                                 "0012345678"
                                 "48"
                                 "F0F0"
                                 "5910111549"
                                 "2019"
                                 "000005"
                                 "2619"
                                 "000005"
                                 "3923"
                                 "00000127"
                                 "3812"
                                 "1115"
                                 "03"
                                 "20FA",
                       120);
    BB_Flip(byteBuff);
    pkg = decodePackage(byteBuff);
    linkMsg = (LinkMessage *)pkg;
    upMsg = (UplinkMessage *)pkg;
    // CHANGE VALUE BY CONFIG
    Channel_FillUplinkMessageHead(ch, upMsg);
    upMsg->messageHead.seq = downMsg->messageHead.seq; // @Todo 这里是否要填写请求端对应的流水号 upMsg->messageHead.seq = Channel_nextSeq(ch)
    // encode
    ByteBuffer *sendBuff = pkg->vptr->encode(pkg);
    bool res = false;
    if (sendBuff != NULL)
    {
        BB_Flip(sendBuff);
        res = ch->vptr->send(ch, sendBuff);
        BB_dtor(sendBuff);
        DelInstance(sendBuff);
    }
    //release
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    return res;
}

bool handleBASIC_CONFIG(Channel *const ch, Package *const request)
{
    assert(ch);
    assert(request);
    assert(request->head.funcCode == BASIC_CONFIG);
    if (request->tail.etxFlag == ESC)
    {
        return true;
    }
    LinkMessage *reqLinkMsg = (LinkMessage *)request;
    DownlinkMessage *reqDownlinkMsg = (DownlinkMessage *)request;
    UplinkMessage *upMsg = NewInstance(UplinkMessage);
    //解析获取对应要查询的配置型标识符
    ByteBuffer *reqBuff = reqLinkMsg->rawBuff;
    UplinkMessage_ctor(upMsg, 10);
    Package *pkg = (Package *)upMsg;              // 获取父结构Package
    Head *head = &pkg->head;                      // 获取Head结构
    Channel_FillUplinkMessageHead(ch, upMsg);     // Fill head by config
    head->funcCode = BASIC_CONFIG;                // 心跳功能码功能码
    upMsg->messageHead.seq = Channel_nextSeq(ch); // 根据功能码填写报文头 @Todo 这里是否要填写请求端对应的流水号
    if (reqBuff != NULL)
    {
        LinkMessage *uplinkMsg = (LinkMessage *)upMsg;
        ByteBuffer *rawBuff = uplinkMsg->rawBuff = NewInstance(ByteBuffer);
        BB_ctor(rawBuff, 0);
        Config *config = &ch->station->config;
        while (BB_Available(reqBuff) >= 2) // 标识符两个字节，前一个是标识符，后一个固定为 0；成对，如果多余，就丢弃
        {
            uint8_t u8 = 0;
            Channel *ch = NULL;
            BB_GetUInt8(reqBuff, &u8);
            // 同时 计算实际内容部分的长度，开ByteBuffer
            switch (u8)
            {
            case CONFIG_CENTER_ADDRS:
                if (config->centerAddrs != NULL)
                {
                    BB_Expand(rawBuff, ELEMENT_IDENTIFER_LEN + 4); // 2+4
                    BB_PutUInt8(rawBuff, CONFIG_CENTER_ADDRS);
                    BB_PutUInt8(rawBuff, 4 << NUMBER_ELEMENT_LEN_OFFSET); // 0x20
                    BB_PutUInt8(rawBuff, config->centerAddrs->addr1);
                    BB_PutUInt8(rawBuff, config->centerAddrs->addr2);
                    BB_PutUInt8(rawBuff, config->centerAddrs->addr3);
                    BB_PutUInt8(rawBuff, config->centerAddrs->addr4);
                }
                break;
            case CONFIG_REMOTESTATION_ADDR:
                if (config->stationAddr != NULL)
                {
                    BB_Expand(rawBuff, ELEMENT_IDENTIFER_LEN + REMOTE_STATION_ADDR_LEN); // 2+5
                    BB_PutUInt8(rawBuff, CONFIG_REMOTESTATION_ADDR);
                    BB_PutUInt8(rawBuff, 5 << NUMBER_ELEMENT_LEN_OFFSET); // 0x28
                    RemoteStationAddr_Encode(config->stationAddr, rawBuff);
                }
                break;
            case CONFIG_PASSWORD:
                if (config->password != NULL)
                {
                    BB_Expand(rawBuff, ELEMENT_IDENTIFER_LEN + 2); // 2+2
                    BB_PutUInt8(rawBuff, CONFIG_PASSWORD);
                    BB_PutUInt8(rawBuff, 2 << 3); // 0x10
                    BB_BE_PutUInt16(rawBuff, *config->password);
                }
                break;
            case CONFIG_WORK_MODE:
                if (config->workMode != NULL)
                {
                    BB_Expand(rawBuff, ELEMENT_IDENTIFER_LEN + 1); // 2+1
                    BB_PutUInt8(rawBuff, CONFIG_WORK_MODE);
                    BB_PutUInt8(rawBuff, 1 << 3); // 0x08
                    BB_BCDPutUInt8(rawBuff, *config->workMode);
                }
            case CONFIG_CHANNEL_1_MASTER ... CONFIG_CHANNEL_4_SLAVE:
                ch = Config_findChannel(config, u8);
                if (ch != NULL &&
                    ch->id != CHANNEL_ID_FIXED &&
                    ch->type != CHANNEL_DISABLED)
                {
                    if (ch->vptr->expandEncode != NULL)
                    {
                        ch->vptr->expandEncode(ch, rawBuff);
                    }
                }
                break;
            default:
                break;
            }
            BB_Skip(reqBuff, 1);
        }
        BB_Flip(rawBuff);
    }
    pkg->tail.etxFlag = ETX; // 截止符
    // encode
    ByteBuffer *sendBuff = pkg->vptr->encode(pkg);
    bool res = false;
    if (sendBuff != NULL)
    {
        BB_Flip(sendBuff);
        res = ch->vptr->send(ch, sendBuff);
        BB_dtor(sendBuff);
        DelInstance(sendBuff);
    }
    //release
    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    return res;
}

bool handleMODIFY_BASIC_CONFIG(Channel *const ch, Package *const request)
{
    assert(ch);
    assert(request);
    assert(request->head.funcCode == MODIFY_BASIC_CONFIG);
    if (request->tail.etxFlag == ESC)
    {
        return true;
    }
    LinkMessage *reqLinkMsg = (LinkMessage *)request;
    DownlinkMessage *reqDownlinkMsg = (DownlinkMessage *)request;
    ByteBuffer *reqBuff = reqLinkMsg->rawBuff;
    //解析获取对应要查询的配置型标识符
    if (reqBuff != NULL && BB_Available(reqBuff) > 0) // 应答包或者空包不处理
    {
        UplinkMessage *upMsg = NewInstance(UplinkMessage);
        UplinkMessage_ctor(upMsg, 10);
        Package *pkg = (Package *)upMsg;              // 获取父结构Package
        Head *head = &pkg->head;                      // 获取Head结构
        Channel_FillUplinkMessageHead(ch, upMsg);     // Fill head by config
        head->funcCode = BASIC_CONFIG;                // 心跳功能码功能码
        upMsg->messageHead.seq = Channel_nextSeq(ch); // 根据功能码填写报文头 @Todo 这里是否要填写请求端对应的流水号
    }
    return true;
}
// HANDLERS END

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
        &Channel_ExpandEncode,
        &Channel_dtor};
    me->vptr = &vtbl;
    me->station = station;
    vec_init(&me->handlers);
    vec_reserve(&me->handlers, CHANNEL_RESERVED_HANDLER_SIZE);
    // register handler
    static ChannelHandler const h_TEST = {TEST, &handleTEST}; // TEST
    vec_push(&me->handlers, (ChannelHandler *)&h_TEST);
    static ChannelHandler const h_BASIC_CONFIG = {BASIC_CONFIG, &handleBASIC_CONFIG}; // BASIC_CONFIG
    vec_push(&me->handlers, (ChannelHandler *)&h_BASIC_CONFIG);
    static ChannelHandler const h_MODIFY_BASIC_CONFIG = {MODIFY_BASIC_CONFIG, &handleMODIFY_BASIC_CONFIG}; // MODIFY_BASIC_CONFIG
    vec_push(&me->handlers, (ChannelHandler *)&h_MODIFY_BASIC_CONFIG);
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

bool IOChannel_ExpandEncode(Channel *const me, ByteBuffer *const buff)
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
        w->repeat = 10; // slow down
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
        int i = 0;
        ChannelHandler *h = NULL;
        bool handled = false;
        vec_foreach(&ch->handlers, h, i)
        {
            if (h != NULL && h->cb != NULL && h->code == pkg->head.funcCode)
            {
                handled = true;
                h->cb(ch, pkg);
                break;
            }
        }
        printf("ch[%2d] %7s request[%4X]\r\n", ch->id, handled == true ? "handled" : "droped", pkg->head.funcCode);
        Package_dtor(pkg);
        DelInstance(pkg);
    }
    else
    {
        printf("ch[%2d] invalid request, errno:[%3d], hex:[%s]\r\n", ch->id, last_error(), ch->readBuff);
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
         &IOChannel_ExpandEncode,
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
    printf("ch[%2d] connecting to %15s:%5d\r\n", ch->id, inet_ntoa(ipv4->addr.sin_addr), ntohs(ipv4->addr.sin_port));
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
    printf("ch[%2d] connected\r\n", ch->id);
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
    int len = recv(sock, ch->readBuff, CHANNLE_RECIVE_BUFF_SIZE, 0);
    if (len <= 0)
    {
        return NULL;
    }
    ByteBuffer *buff = NewInstance(ByteBuffer);
    BB_ctor_wrapped(buff, (uint8_t *)ch->readBuff, len);
    BB_Flip(buff);
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
        return send(sock, (const char *)buff->buff, len, 0) == len;
    }
    else
    {
        return false;
    }
}

bool SocketChannel_ExpandEncode(Channel *const me, ByteBuffer *const buff)
{
    assert(0);
    return false;
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
          &SocketChannel_ExpandEncode,
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

bool Ipv4Channel_ExpandEncode(Channel *const me, ByteBuffer *const buff)
{
    BB_Expand(buff, ELEMENT_IDENTIFER_LEN + 6); // 2+6
    BB_PutUInt8(buff, me->id);
    BB_PutUInt8(buff, 6 << 3); // 0x30
    Ipv4Channel *ipv4Channel = (Ipv4Channel *)me;
    struct in_addr addr = ipv4Channel->ipv4.addr.sin_addr;
    uint64_t ipIn64 = addr.s_net * 10e9 +
                      addr.s_host * 10e6 +
                      addr.s_lh * 10e3 +
                      addr.s_impno;
    BB_BE_BCDPutUInt(buff, &ipIn64, 6);
    return true;
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
          &Ipv4Channel_ExpandEncode,
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

bool DomainChannel_ExpandEncode(Channel *const me, ByteBuffer *const buff)
{
    DomainChannel *domainCh = (DomainChannel *)me;
    uint8_t domainLen = strlen(domainCh->domain.domainStr) + 10; // 10 more for port
    char domainStr[domainLen];
    BB_Expand(buff, ELEMENT_IDENTIFER_LEN + domainLen); // 2+6
    BB_PutUInt8(buff, me->id);
    BB_PutUInt8(buff, domainLen << 3);
    snprintf(domainStr, domainLen, "%s:%d", domainCh->domain.domainStr, domainCh->domain.ipv4.addr.sin_port);
    BB_PutString(buff, domainStr);
    return true;
}

void DomainChannel_dtor(Channel *me)
{
    assert(me);
    SocketChannel_dtor(me);
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
          &DomainChannel_ExpandEncode,
          &DomainChannel_dtor},
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

Channel *Channel_toiOTA()
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return NULL;
    }
#else

#endif
    static char const *iOTA = "console.theiota.cn";
    struct hostent *hosts = gethostbyname(iOTA);
    if (hosts == NULL || hosts->h_addrtype != AF_INET)
    {
        return NULL;
    }
    DomainChannel *ch = NewInstance(DomainChannel);
    DomainChannel_ctor(ch, NULL); // 初始化 station 为 NULL
    Channel *super = (Channel *)ch;
    super->id = CHANNEL_ID_FIXED;
    super->type = CHANNEL_DOMAIN;
    super->keepaliveTimer = 60;
    ch->domain.domainStr = (char *)iOTA;
    ch->domain.ipv4.addr.sin_family = AF_INET;
    ch->domain.ipv4.addr.sin_port = htons(60338);
    // GET THE FIRST IP
    ch->domain.ipv4.addr.sin_addr = *(struct in_addr *)hosts->h_addr_list[0];
    return (Channel *)ch;
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

Channel *Config_findChannel(Config *const me, uint8_t chId)
{
    int i = 0;
    Channel *ch;
    vec_foreach(&me->channels, ch, i)
    {
        if (ch->id == chId)
        {
            return ch;
        }
    }
    return NULL;
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
    // 处理为 BCD 方式，便于用户配置在配置文件里
    int pwdInBcd = 0;
    int shift = 0;
    while (*me->password > 0)
    {
        pwdInBcd |= (*me->password % 10) << (shift++ << 2);
        *me->password /= 10;
    }
    *me->password = pwdInBcd;
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
    // add a fixed channel to make it reachable
    Channel *ch = Channel_toiOTA();
    if (ch != NULL)
    {
        vec_push(&me->channels, ch);
    }
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
