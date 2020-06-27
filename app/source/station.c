#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#ifdef _WIN32
#include <winsock2.h>
#include <winsock.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <netinet/tcp.h>
#endif

#include "cJSON/cJSON_Utils.h"
#include "cJSON/cJSON_Helper.h"
#include "common/class.h"

#include "station.h"
// string util
void string2hexString(char *input, char *output, size_t size)
{
    int loop;
    int i;

    i = 0;
    loop = 0;

    while (loop < size)
    {
        sprintf((char *)(output + i), "%02X", input[loop]);
        loop += 1;
        i += 2;
    }
    //insert NULL at the end of the output string
    output[i++] = '\0';
}
//

// file system util
typedef struct stat Stat;
#ifdef _WIN32
#define mkdir(D, M) mkdir(D)
#endif

static int do_mkdir(const char *path, mode_t mode)
{
    Stat st;
    int status = 0;

    if (stat(path, &st) != 0)
    {
        /* Directory does not exist. EEXIST for race condition */
        if (mkdir(path, mode) != 0 && errno != EEXIST)
            status = -1;
    }
    else if (!S_ISDIR(st.st_mode))
    {
        errno = ENOTDIR;
        status = -1;
    }

    return status;
}

int mkpath(const char *path, mode_t mode)
{
    char *pp;
    char *sp;
    int status;
    char *copypath = strdup(path);

    status = 0;
    pp = copypath;
    while (status == 0 && (sp = strchr(pp, '/')) != 0)
    {
        if (sp != pp)
        {
            /* Neither root nor double slash in path */
            *sp = '\0';
            status = do_mkdir(copypath, mode);
            *sp = '/';
        }
        pp = sp + 1;
    }
    if (status == 0)
        status = do_mkdir(path, mode);
    DelInstance(copypath);
    return status;
}
// file system util END

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
    upMsg->messageHead.stationCategory = config->stationCategory;
}

void Channel_FillPackageHead(Channel *const me, Package *const pkg)
{
    assert(me);
    assert(pkg);
    Config *config = &me->station->config;
    pkg->head.centerAddr = me->centerAddr;
    pkg->head.stationAddr = *config->stationAddr;
    pkg->head.password = *config->password;
    if (pkg->head.direction == Up)
    {
        UplinkMessage *upMsg = (UplinkMessage *)pkg;
        DateTime_now(&upMsg->messageHead.sendTime);
        upMsg->messageHead.stationAddrElement.stationAddr = *config->stationAddr;
        upMsg->messageHead.stationCategory = config->stationCategory;
    }
    else
    {
        DateTime_now(&((DownlinkMessage *)pkg)->messageHead.sendTime);
    }
}

uint16_t Channel_NextSeq(Channel *const me)
{
    return me->seq++;
}

uint16_t Channel_LastSeq(Channel *const me)
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
    msg->messageHead.seq = Channel_LastSeq(me);      // 根据功能码填写报文头
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

bool Channel_IsConnected(Channel *const me)
{
    assert(me);
    return me->isConnnected;
}

bool Channel_NotifyData(Channel *const me)
{
    assert(0);
    return false;
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

void Channel_OnFilesQuery(Channel *const me)
{
    assert(0);
}

bool Channel_IsFileSent(Channel *const me, tinydir_file *file)
{
    assert(me);
    assert(file);
    pthread_mutex_lock(&me->cleanUpMutex);
    cJSON *records = cJSON_GetObjectItem(me->recordsFileInJSON, "records");
    bool res = records != NULL && cJSON_HasObjectItem(records, file->path);
    pthread_mutex_unlock(&me->cleanUpMutex);
    return res;
}

void Channel_RecordCurrentSentFile(Channel *const me)
{
    assert(me);
    pthread_mutex_lock(&me->cleanUpMutex);
    cJSON *records = cJSON_GetObjectItem(me->recordsFileInJSON, "records");
    cJSON_AddItemToObject(records, me->currentFile->path, cJSON_CreateObject());
    cJSON_WriteFile(me->recordsFileInJSON, me->recordsFile);
    pthread_mutex_unlock(&me->cleanUpMutex);
}

void Channel_RecordSentFile(Channel *const me, tinydir_file *const file)
{
    assert(me);
    assert(file);
    pthread_mutex_lock(&me->cleanUpMutex);
    cJSON *records = cJSON_GetObjectItem(me->recordsFileInJSON, "records");
    cJSON_AddItemToObject(records, file->path, cJSON_CreateObject());
    cJSON_WriteFile(me->recordsFileInJSON, me->recordsFile);
    pthread_mutex_unlock(&me->cleanUpMutex);
}

void Channel_ClearSentFileRecord(Channel *const me, tinydir_file const *file)
{
    assert(me);
    assert(file);
    pthread_mutex_lock(&me->cleanUpMutex);
    cJSON *records = cJSON_GetObjectItem(me->recordsFileInJSON, "records");
    cJSON_DeleteItemFromObject(records, file->path);
    cJSON_WriteFile(me->recordsFileInJSON, me->recordsFile);
    pthread_mutex_unlock(&me->cleanUpMutex);
}

bool Channel_SendFileByFd(Channel *const me, struct stat *fStat, int fd)
{
    assert(me);
    assert(fStat);
    if (fStat->st_size <= 0)
    {
        return true;
    }
    uint16_t pkgCount = fStat->st_size / me->buffSize;
    if (fStat->st_size % me->buffSize != 0)
    {
        pkgCount++;
    }
    uint16_t pkgNo = 1;
    ssize_t readBytes = -1;
    while ((readBytes = read(fd, me->buff, me->buffSize)) > 0)
    {
        // create package
        UplinkMessage *upMsg = NewInstance(UplinkMessage); // 选择是上行还是下行
        UplinkMessage_ctor(upMsg, 0);                      // 调用构造函数,如果有要素，需要指定要素数量
        Package *pkg = (Package *)upMsg;                   // 获取父结构Package
        Head *head = &pkg->head;                           // 获取Head结构
        Channel_FillUplinkMessageHead(me, upMsg);          // Fill head by config
        head->funcCode = PICTURE;                          // 心跳功能码功能码
        head->stxFlag = SYN;
        head->sequence.count = pkgCount;
        head->sequence.seq = pkgNo;
        upMsg->messageHead.seq = Channel_LastSeq(me); // 根据功能码填写报文头
        if (pkgNo != pkgCount)
        {
            pkg->tail.etxFlag = ETB; // 截止符
        }
        else
        {
            pkg->tail.etxFlag = ETX; // 截止符
        }
        LinkMessage *uplinkMsg = (LinkMessage *)upMsg;
        PictureElement *picEl = NewInstance(PictureElement);
        PictureElement_ctor(picEl, pkgNo);
        ByteBuffer *rawBuff = picEl->buff = NewInstance(ByteBuffer);
        BB_ctor_wrapped(rawBuff, (uint8_t *)me->buff, readBytes);
        BB_Flip(rawBuff);
        LinkMessage_PushElement(uplinkMsg, (Element *const)picEl);
        ByteBuffer *byteOut = pkg->vptr->encode(pkg); // 编码
        BB_Flip(byteOut);                             // 转为读模式
        bool res = me->vptr->send(me, byteOut);       // 调用发送实现
        UplinkMessage_dtor((Package *)upMsg);         // 析构
        DelInstance(upMsg);                           // free
        BB_dtor(byteOut);                             // 析构
        DelInstance(byteOut);                         // free
        if (!res)
        {
            return false;
        }
        usleep(me->msgSendInterval * 1000);
        pkgNo++;
    }
    return pkgNo == pkgCount + 1;
}

void Channel_SendFile(Channel *const me, tinydir_file *file)
{
    assert(me);
    assert(file);
    // if (Channel_isFileSent(me, file))
    // {
    //     return;
    // }
    if (me->status != CHANNEL_STATUS_RUNNING)
    {
        return;
    }
    printf("ch[%2d] pick up file [%s] to send.\r\n", me->id, file->path);
    memset(me->buff, 0, me->buffSize);
    struct stat fStat = {0};
    int fd = 0;
#ifndef WIN32
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif
    if (stat(file->path, &fStat) == SL651_APP_ERROR_SUCCESS &&
        (fd = open(file->path, O_RDONLY | O_BINARY)))
    {
        if (fStat.st_size <= 0) // 0 字节文件
        {
            if (!Station_IsFileSentByAllChannel((Station *const)me->station, (tinydir_file *const)file, me))
            {
                Channel_RecordSentFile(me, (tinydir_file *const)file);
            }
            me->status = CHANNEL_STATUS_RUNNING;
            return;
        }

        if (Channel_SendFileByFd(me, &fStat, fd))
        {
            if (me->station->config.waitFileSendAck) // 等待应答
            {
                me->status = CHANNEL_STATUS_WAITTING_SCAN_FILESEND_ACK;
                me->currentFile = NewInstance(tinydir_file);
                memcpy(me->currentFile, file, sizeof(tinydir_file));
            }
            else // 不等待应答
            {
                me->status = CHANNEL_STATUS_RUNNING;
                Channel_RecordSentFile(me, (tinydir_file *const)file);
            }
        }
        close(fd);
        // @Todo 隔段时间 做一次反向清理同步
    }
}

void Channel_SendFilePkg(Channel *const me, FilePkg *const filePkg)
{
    assert(me);
    assert(filePkg);
    if (me->status != CHANNEL_STATUS_RUNNING)
    {
        return;
    }
    printf("ch[%2d] sending file[%s].\r\n", me->id, filePkg->file);
    memset(me->buff, 0, me->buffSize);
    struct stat fStat = {0};
    int fd = 0;
#ifndef WIN32
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif
    if (stat(filePkg->file, &fStat) == SL651_APP_ERROR_SUCCESS &&
        (fd = open(filePkg->file, O_RDONLY | O_BINARY)))
    {
        if (fStat.st_size <= 0) // 0 字节文件
        {
            Station_MarkFilePkgSent(me->station, me, filePkg, true);
            me->status = CHANNEL_STATUS_RUNNING;
            return;
        }

        if (Channel_SendFileByFd(me, &fStat, fd))
        {
            if (me->station->config.waitFileSendAck) // 等待应答
            {
                me->status = CHANNEL_STATUS_WAITTING_AYNC_FILESEND_ACK;
                me->currentFilePkg = filePkg;
            }
            else // 不等待应答
            {
                me->status = CHANNEL_STATUS_RUNNING;
                Station_MarkFilePkgSent(me->station, me, filePkg, true);
            }
        }
        close(fd);
        // @Todo 隔段时间 做一次反向清理同步
    }
}

void Channel_dtor(Channel *const me)
{
    assert(me);
    if (me->recordsFile != NULL)
    {
        DelInstance(me->recordsFile);
    }
    if (me->recordsFileInJSON != NULL)
    {
        cJSON_Delete(me->recordsFileInJSON);
    }
    pthread_mutex_destroy(&me->cleanUpMutex);
}

// 工作方式 1/2 主动上报
bool Channel_TEST(Channel *const me, uint16_t seq)
{
    assert(me);
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
    Channel_FillUplinkMessageHead(me, upMsg);
    upMsg->messageHead.seq = seq;
    // encode
    ByteBuffer *sendBuff = pkg->vptr->encode(pkg);
    bool res = false;
    if (sendBuff != NULL)
    {
        BB_Flip(sendBuff);
        res = me->vptr->send(me, sendBuff);
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

bool Channel_BASIC_CONFIG(Channel *const me)
{
    assert(me);
    UplinkMessage *upMsg = NewInstance(UplinkMessage);
    UplinkMessage_ctor(upMsg, 10);
    Package *pkg = (Package *)upMsg;              // 获取父结构Package
    Head *head = &pkg->head;                      // 获取Head结构
    Channel_FillUplinkMessageHead(me, upMsg);     // Fill head by config
    head->funcCode = BASIC_CONFIG;                // 心跳功能码功能码
    upMsg->messageHead.seq = Channel_NextSeq(me); // 根据功能码填写报文头 @Todo 这里是否要填写请求端对应的流水号
    LinkMessage *uplinkMsg = (LinkMessage *)upMsg;
    ByteBuffer *rawBuff = uplinkMsg->rawBuff = NewInstance(ByteBuffer);
    BB_ctor(rawBuff, 0);
    Config *config = &me->station->config;
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
    if (config->stationAddr != NULL)
    {
        BB_Expand(rawBuff, ELEMENT_IDENTIFER_LEN + REMOTE_STATION_ADDR_LEN); // 2+5
        BB_PutUInt8(rawBuff, CONFIG_REMOTESTATION_ADDR);
        BB_PutUInt8(rawBuff, 5 << NUMBER_ELEMENT_LEN_OFFSET); // 0x28
        RemoteStationAddr_Encode(config->stationAddr, rawBuff);
    }
    if (config->password != NULL)
    {
        BB_Expand(rawBuff, ELEMENT_IDENTIFER_LEN + 2); // 2+2
        BB_PutUInt8(rawBuff, CONFIG_PASSWORD);
        BB_PutUInt8(rawBuff, 2 << 3); // 0x10
        BB_BE_PutUInt16(rawBuff, *config->password);
    }
    if (config->workMode != NULL)
    {
        BB_Expand(rawBuff, ELEMENT_IDENTIFER_LEN + 1); // 2+1
        BB_PutUInt8(rawBuff, CONFIG_WORK_MODE);
        BB_PutUInt8(rawBuff, 1 << 3); // 0x08
        BB_BCDPutUInt8(rawBuff, *config->workMode);
    }
    size_t i;
    Channel *ch = NULL;
    vec_foreach(&config->channels, ch, i)
    {
        if (ch != NULL &&
            ch->id != CHANNEL_ID_FIXED &&
            ch->type != CHANNEL_DISABLED)
        {
            if (ch->vptr->expandEncode != NULL)
            {
                ch->vptr->expandEncode(ch, rawBuff);
            }
        }
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
//

// HANDELERS
bool handleTEST(Channel *const ch, Package *const request)
{
    assert(ch);
    assert(request);
    assert(request->head.funcCode == TEST);
    if (*ch->station->config.workMode == REPORT || *ch->station->config.workMode == REPORT_CONFIRM)
    {
        return true;
    }
    return Channel_TEST(ch, request->head.sequence.seq);
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
    upMsg->messageHead.seq = Channel_NextSeq(ch); // 根据功能码填写报文头 @Todo 这里是否要填写请求端对应的流水号
    if (reqBuff != NULL)
    {
        LinkMessage *uplinkMsg = (LinkMessage *)upMsg;
        ByteBuffer *rawBuff = uplinkMsg->rawBuff = NewInstance(ByteBuffer);
        BB_ctor(rawBuff, 0);
        Config *config = &ch->station->config;
        while (BB_Available(reqBuff) >= 2) // 标识符两个字节，前一个是标识符，后一个固定为 0；成对，如果多余，就丢弃
        {
            uint8_t identifier = 0;
            Channel *ch = NULL;
            BB_GetUInt8(reqBuff, &identifier);
            // 同时 计算实际内容部分的长度，开ByteBuffer
            switch (identifier)
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
                ch = Config_FindChannel(config, identifier);
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
    if (reqBuff != NULL && BB_Available(reqBuff) > 0) // 空包不处理
    {
        Config *config = &ch->station->config; //
        cJSON *patches = cJSON_CreateArray();  //
        while (BB_Available(reqBuff) >= 2)     // 标识符两个字节，前一个是标识符，后一个固定为 0；成对，如果多余，就丢弃
        {
            uint8_t identifier = 0;
            uint8_t len = 0;
            uint8_t u8 = 0;
            uint16_t u16 = 0;
            BB_GetUInt8(reqBuff, &identifier);
            BB_GetUInt8(reqBuff, &len);
            len = len >> NUMBER_ELEMENT_LEN_OFFSET;
            // 同时 计算实际内容部分的长度，开ByteBuffer
            if (BB_Available(reqBuff) >= len)
            {
                switch (identifier)
                {
                case CONFIG_CENTER_ADDRS:
                    BB_GetUInt8(reqBuff, &u8);
                    cJSONUtils_AddPatchToArray(patches, "replace", "/centerAddrs/addr1", cJSON_CreateNumber(u8));
                    BB_GetUInt8(reqBuff, &u8);
                    cJSONUtils_AddPatchToArray(patches, "replace", "/centerAddrs/addr2", cJSON_CreateNumber(u8));
                    BB_GetUInt8(reqBuff, &u8);
                    cJSONUtils_AddPatchToArray(patches, "replace", "/centerAddrs/addr3", cJSON_CreateNumber(u8));
                    BB_GetUInt8(reqBuff, &u8);
                    cJSONUtils_AddPatchToArray(patches, "replace", "/centerAddrs/addr4", cJSON_CreateNumber(u8));
                    break;
                case CONFIG_REMOTESTATION_ADDR:
                {
                    RemoteStationAddr *stationAddr = NewInstance(RemoteStationAddr);
                    RemoteStationAddr_Decode(stationAddr, reqBuff);
                    cJSONUtils_AddPatchToArray(patches, "replace", "/remoteStationAddr/A5",
                                               cJSON_CreateNumber(stationAddr->A5));
                    cJSONUtils_AddPatchToArray(patches, "replace", "/remoteStationAddr/A4",
                                               cJSON_CreateNumber(stationAddr->A4));
                    cJSONUtils_AddPatchToArray(patches, "replace", "/remoteStationAddr/A3",
                                               cJSON_CreateNumber(stationAddr->A3));
                    cJSONUtils_AddPatchToArray(patches, "replace", "/remoteStationAddr/A2",
                                               cJSON_CreateNumber(stationAddr->A2));
                    cJSONUtils_AddPatchToArray(patches, "replace", "/remoteStationAddr/A1",
                                               cJSON_CreateNumber(stationAddr->A1));
                    cJSONUtils_AddPatchToArray(patches, "replace", "/remoteStationAddr/A0",
                                               cJSON_CreateNumber(stationAddr->A0));
                    DelInstance(stationAddr);
                }
                break;
                case CONFIG_PASSWORD:
                    BB_BCDGetUInt(reqBuff, &u16, 2);
                    cJSONUtils_AddPatchToArray(patches, "replace", "/password", cJSON_CreateNumber(u16));
                    break;
                case CONFIG_WORK_MODE:
                    BB_GetUInt8(reqBuff, &u8);
                    cJSONUtils_AddPatchToArray(patches, "replace", "/workMode", cJSON_CreateNumber(u8));
                    break;
                case CONFIG_CHANNEL_1_MASTER ... CONFIG_CHANNEL_4_SLAVE:
                {
                    uint8_t cType = CHANNEL_DISABLED;
                    BB_GetUInt8(reqBuff, &cType);
                    if (cType != CHANNEL_DOMAIN &&
                        cType != CHANNEL_IPV4 &&
                        cType != CHANNEL_DISABLED)
                    {
                        BB_Skip(reqBuff, len);
                        break;
                    }
                    int32_t chIndex = -1;
                    chIndex = Config_IndexOfChannel(config, identifier);
                    if (cType == CHANNEL_DISABLED)
                    {
                        if (chIndex != -1) // remove
                        {
                            char path[20] = {0}; // enough
                            snprintf(path, 20, "/channels/%d", chIndex);
                            cJSONUtils_AddPatchToArray(patches, "remove", path, NULL);
                        }
                        BB_Skip(reqBuff, len);
                        break;
                    }
                    char path[20] = {0}; // enough
                    if (chIndex != -1)
                    {
                        snprintf(path, 20, "/channels/%d", chIndex);
                    }
                    else
                    {
                        snprintf(path, 20, "/channels/-");
                    }
                    cJSON *ch = NULL;
                    switch (cType)
                    {
                    case CHANNEL_IPV4:
                    {
                        ch = cJSON_CreateObject();
                        cJSON_AddItemToObject(ch, "id", cJSON_CreateNumber(identifier));
                        cJSON_AddItemToObject(ch, "type", cJSON_CreateNumber(cType));
                        cJSON_AddItemToObject(ch, "keepaliveTimer", cJSON_CreateNumber(CHANNLE_DEFAULT_KEEPALIVE_INTERVAL));
                        uint64_t u64 = 0;
                        BB_BCDGetUInt(reqBuff, &u64, 6);
                        char path[20] = {0}; // enough
                        snprintf(path, 20, "%d.%d.%d.%d",
                                 (uint16_t)((u64 / 1000000000) % 1000),
                                 (uint16_t)((u64 / 1000000) % 1000),
                                 (uint16_t)((u64 / 1000) % 1000),
                                 (uint16_t)(u64 % 1000));
                        cJSON_AddItemToObject(ch, "ipv4", cJSON_CreateString(path));
                        BB_BCDGetUInt(reqBuff, &u16, 3);
                        cJSON_AddItemToObject(ch, "port", cJSON_CreateNumber(u16));
                    }
                    break;
                    case CHANNEL_DOMAIN:
                    {
                        char *domainStr = BB_GetString(reqBuff, len);
                        ch = cJSON_CreateObject();
                        cJSON_AddItemToObject(ch, "id", cJSON_CreateNumber(identifier));
                        cJSON_AddItemToObject(ch, "type", cJSON_CreateNumber(cType));
                        cJSON_AddItemToObject(ch, "keepaliveTimer", cJSON_CreateNumber(CHANNLE_DEFAULT_KEEPALIVE_INTERVAL));
                        char *colon = strchr(domainStr, ':');
                        if (colon != NULL)
                        {
                            cJSON_AddItemToObject(ch, "port", cJSON_CreateNumber(atoi(colon + 1)));
                            *colon = '\0'; // make a break here
                        }
                        else
                        {
                            cJSON_AddItemToObject(ch, "port", cJSON_CreateNumber(60338));
                        }
                        cJSON_AddItemToObject(ch, "domain", cJSON_CreateString(domainStr));
                        DelInstance(domainStr);
                    }
                    break;
                    default:
                        BB_Skip(reqBuff, len);
                        break;
                    }
                    if (ch != NULL)
                    {
                        cJSONUtils_AddPatchToArray(patches, chIndex != -1 ? "replace" : "add", path, ch);
                    }
                }
                break;
                default:
                    BB_Skip(reqBuff, len);
                    break;
                }
            }
        }
        char *jsonStr = cJSON_Print(patches);
        printf("ch[%2d] config patches:      \r\n%s\r\n ============================================ \r\n", ch->id, jsonStr);
        DelInstance(jsonStr);
        cJSONUtils_ApplyPatchesCaseSensitive(config->configInJSON, patches);
        jsonStr = cJSON_Print(config->configInJSON);
        printf("ch[%2d] config after patched:\r\n%s\r\n ============================================ \r\n", ch->id, jsonStr);
        DelInstance(jsonStr);
        cJSON_Delete(patches);
        cJSON_WriteFile(config->configInJSON, config->configFile);
        // 应答
        UplinkMessage *upMsg = NewInstance(UplinkMessage);
        UplinkMessage_ctor(upMsg, 10);
        Package *pkg = (Package *)upMsg;              // 获取父结构Package
        Head *head = &pkg->head;                      // 获取Head结构
        Channel_FillUplinkMessageHead(ch, upMsg);     // Fill head by config
        head->funcCode = BASIC_CONFIG;                // 心跳功能码功能码
        upMsg->messageHead.seq = Channel_NextSeq(ch); // 根据功能码填写报文头 @Todo 这里是否要填写请求端对应的流水号
        LinkMessage *uplinkMsg = (LinkMessage *)upMsg;
        uplinkMsg->rawBuff = NewInstance(ByteBuffer);
        BB_ctor_wrappedAnother(uplinkMsg->rawBuff, reqBuff, 0, BB_Limit(reqBuff));
        BB_Flip(uplinkMsg->rawBuff);
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
    return true;
}

bool handleRUNTIME_CONFIG(Channel *const ch, Package *const request)
{
    assert(ch);
    assert(request);
    assert(request->head.funcCode == RUNTIME_CONFIG);
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
    head->funcCode = RUNTIME_CONFIG;              // 心跳功能码功能码
    upMsg->messageHead.seq = Channel_NextSeq(ch); // 根据功能码填写报文头 @Todo 这里是否要填写请求端对应的流水号
    Config *config = &ch->station->config;
    cJSON *params = cJSON_GetObjectItem(config->configInJSON, "runtimeParameters");
    if (reqBuff != NULL && params != NULL)
    {
        LinkMessage *uplinkMsg = (LinkMessage *)upMsg;
        while (BB_Available(reqBuff) >= 2) // 标识符两个字节，前一个是标识符，后一个固定为 0；成对，如果多余，就丢弃
        {
            uint8_t identifierLeader = 0;
            BB_GetUInt8(reqBuff, &identifierLeader);
            char path[30] = {0}; // enough
            snprintf(path, 30, "%04X", identifierLeader);
            cJSON *param = cJSON_GetObjectItem(params, path);
            if (param != NULL)
            {
                cJSON *t = cJSON_GetObjectItem(param, "t");
                cJSON *v = cJSON_GetObjectItem(param, "v");
                if (t != NULL && v != NULL)
                {
                    NumberElement *el = NewInstance(NumberElement);
                    NumberElement_ctor(el, identifierLeader, t->valueint, false);
                    if ((t->valueint & NUMBER_ELEMENT_PRECISION_MASK) == 0)
                    {
                        NumberElement_SetInteger(el, v->valueint);
                    }
                    else
                    {
                        NumberElement_SetDouble(el, v->valuedouble);
                    }
                    LinkMessage_PushElement(uplinkMsg, (Element *)el);
                }
            }
            BB_Skip(reqBuff, 1);
        }
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

bool handleMODIFY_RUNTIME_CONFIG(Channel *const ch, Package *const request)
{
    assert(ch);
    assert(request);
    assert(request->head.funcCode == MODIFY_RUNTIME_CONFIG);
    if (request->tail.etxFlag == ESC)
    {
        return true;
    }
    LinkMessage *reqLinkMsg = (LinkMessage *)request;
    DownlinkMessage *reqDownlinkMsg = (DownlinkMessage *)request;
    ByteBuffer *reqBuff = reqLinkMsg->rawBuff;
    //解析获取对应要查询的配置型标识符
    if (reqBuff != NULL && BB_Available(reqBuff) > 0) // 空包不处理
    {
        Config *config = &ch->station->config; //
        cJSON *patches = cJSON_CreateArray();  //
        while (BB_Available(reqBuff) >= 2)     // 标识符两个字节，前一个是标识符
        {
            uint8_t identifierLeader = 0;
            uint8_t dataDef = 0;
            BB_GetUInt8(reqBuff, &identifierLeader);
            BB_GetUInt8(reqBuff, &dataDef);
            uint8_t dataLen = dataDef >> NUMBER_ELEMENT_LEN_OFFSET;
            uint8_t dataPrecision = dataDef & NUMBER_ELEMENT_PRECISION_MASK;
            // 同时 计算实际内容部分的长度，开ByteBuffer
            if (BB_Available(reqBuff) >= dataLen)
            {
                Element *el = (Element *)NewInstance(NumberElement);
                NumberElement_ctor_nullNumber((NumberElement *)el, identifierLeader, dataDef, false);
                Element_SetDirection(el, Up);
                bool decoded = el->vptr->decode(el, reqBuff); // 解析
                if (!decoded)                                 // 解析失败，需要手动删除指针
                {                                             //
                    if (el->vptr->dtor != NULL)               // 实现了析构函数
                    {                                         //
                        el->vptr->dtor(el);                   // 调用析构，规范步骤
                    }                                         //
                    DelInstance(el);                          // 删除指针
                }
                else
                {
                    cJSON *param = cJSON_CreateObject();
                    double fv = 0;
                    uint64_t iv = 0;
                    cJSON_AddItemToObject(param, "t", cJSON_CreateNumber(dataDef));
                    if (dataPrecision == 0)
                    {
                        NumberElement_GetInteger((NumberElement *)el, &iv);
                        fv = iv;
                    }
                    else
                    {
                        NumberElement_GetDouble((NumberElement *)el, &fv);
                    }
                    cJSON_AddItemToObject(param, "v", cJSON_CreateNumber(fv));
                    char path[30] = {0}; // enough
                    snprintf(path, 30, "/runtimeParameters/%04X", identifierLeader);
                    cJSONUtils_AddPatchToArray(patches, "add", path, param);
                }
            }
        }
        char *jsonStr = cJSON_Print(patches);
        printf("ch[%2d] config patches:      \r\n%s\r\n ============================================ \r\n", ch->id, jsonStr);
        DelInstance(jsonStr);
        cJSONUtils_ApplyPatchesCaseSensitive(config->configInJSON, patches);
        jsonStr = cJSON_Print(config->configInJSON);
        printf("ch[%2d] config after patched:\r\n%s\r\n ============================================ \r\n", ch->id, jsonStr);
        DelInstance(jsonStr);
        cJSON_Delete(patches);
        cJSON_WriteFile(config->configInJSON, config->configFile);
        // 应答
        UplinkMessage *upMsg = NewInstance(UplinkMessage);
        UplinkMessage_ctor(upMsg, 10);
        Package *pkg = (Package *)upMsg;              // 获取父结构Package
        Head *head = &pkg->head;                      // 获取Head结构
        Channel_FillUplinkMessageHead(ch, upMsg);     // Fill head by config
        head->funcCode = RUNTIME_CONFIG;              // 心跳功能码功能码
        upMsg->messageHead.seq = Channel_NextSeq(ch); // 根据功能码填写报文头 @Todo 这里是否要填写请求端对应的流水号
        LinkMessage *uplinkMsg = (LinkMessage *)upMsg;
        uplinkMsg->rawBuff = NewInstance(ByteBuffer);
        BB_ctor_wrappedAnother(uplinkMsg->rawBuff, reqBuff, 0, BB_Limit(reqBuff));
        BB_Flip(uplinkMsg->rawBuff);
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
    return true;
}

bool handlePICTURE(Channel *const ch, Package *const request)
{
    assert(ch);
    assert(request);
    assert(request->head.funcCode == PICTURE);
    if (ch->status == CHANNEL_STATUS_WAITTING_SCAN_FILESEND_ACK) // 扫描式的
    {
        if (request->tail.etxFlag == ENQ)
        {
            return false; // 不能交叉
        }
        if (request->tail.etxFlag == EOT || request->tail.etxFlag == ACK) // 确认成功
        {
            if (ch->currentFile != NULL && !Station_IsFileSentByAllChannel((Station *const)ch->station, (tinydir_file *const)ch->currentFile, ch))
            {
                Channel_RecordCurrentSentFile(ch);
            }
            DelInstance(ch->currentFile);
            ch->status = CHANNEL_STATUS_RUNNING;
        }
        return true;
    }
    else if (ch->status == CHANNEL_STATUS_WAITTING_AYNC_FILESEND_ACK) // 触发式的
    {
        if (request->tail.etxFlag == ENQ)
        {
            return false; // 不能交叉
        }
        if (request->tail.etxFlag == EOT || request->tail.etxFlag == ACK) // 确认成功
        {
            if (ch->currentFilePkg != NULL)
            {
                Station_MarkFilePkgSent(ch->station, ch, ch->currentFilePkg, true);
            }
            ch->status = CHANNEL_STATUS_RUNNING;
        }
        return true;
    }
    else
    {
        if (request->tail.etxFlag == ENQ) // 查询
        {
            // 发起文件查询，由给自Channel实现
            if (ch->vptr->onFilesQuery != NULL)
            {
                ch->vptr->onFilesQuery(ch);
            }
        }
        return true;
    }
}
// HANDLERS END

void Channel_ctor(Channel *me, Station *const station, size_t buffSize, uint8_t msgSendInterval)
{
    assert(me);
    assert(buffSize > 0);
    // assert(station);
    static ChannelVtbl const vtbl = {
        &Channel_Start,
        &Channel_Open,
        &Channel_Close,
        &Channel_Keepalive,
        &Channel_OnRead,
        &Channel_Send,
        &Channel_ExpandEncode,
        &Channel_OnFilesQuery,
        &Channel_NotifyData,
        &Channel_SendFilePkg,
        &Channel_dtor};
    me->vptr = &vtbl;
    me->station = station;
    vec_init(&me->handlers);
    vec_reserve(&me->handlers, CHANNEL_RESERVED_HANDLER_SIZE);
    // init buff
    me->buff = (char *)malloc(buffSize);
    memset(me->buff, '\0', buffSize);
    me->buffSize = buffSize;
    me->msgSendInterval = msgSendInterval;
    // register handler
    static ChannelHandler const h_TEST = {TEST, &handleTEST}; // TEST
    vec_push(&me->handlers, (ChannelHandler *)&h_TEST);
    // BASIC_CONFIG
    static ChannelHandler const h_BASIC_CONFIG = {BASIC_CONFIG, &handleBASIC_CONFIG}; // BASIC_CONFIG
    vec_push(&me->handlers, (ChannelHandler *)&h_BASIC_CONFIG);
    static ChannelHandler const h_MODIFY_BASIC_CONFIG = {MODIFY_BASIC_CONFIG, &handleMODIFY_BASIC_CONFIG}; // MODIFY_BASIC_CONFIG
    vec_push(&me->handlers, (ChannelHandler *)&h_MODIFY_BASIC_CONFIG);
    // RUNTIME_CONFIG
    static ChannelHandler const h_RUNTIME_CONFIG = {RUNTIME_CONFIG, &handleRUNTIME_CONFIG}; // RUNTIME_CONFIG
    vec_push(&me->handlers, (ChannelHandler *)&h_RUNTIME_CONFIG);
    static ChannelHandler const h_MODIFY_RUNTIME_CONFIG = {MODIFY_RUNTIME_CONFIG, &handleMODIFY_RUNTIME_CONFIG}; // MODIFY_RUNTIME_CONFIG
    vec_push(&me->handlers, (ChannelHandler *)&h_MODIFY_RUNTIME_CONFIG);
    // PICTURE
    static ChannelHandler const h_PICTRUE = {PICTURE, &handlePICTURE};
    vec_push(&me->handlers, (ChannelHandler *)&h_PICTRUE);
    me->status = CHANNEL_STATUS_RUNNING;
    pthread_mutexattr_t mutexAttr;
    pthread_mutexattr_init(&mutexAttr);
    pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&me->cleanUpMutex, &mutexAttr);
}
// Virtual Channel END

// Abstract IOChannel
void IOChannel_Start(Channel *const me)
{
    assert(me);
    IOChannel *ioCh = (IOChannel *)me;
    // 启动定时器，连接，如果出错，自动重连
    ChannelConnectWatcher *connectWatcher = NewInstance(ChannelConnectWatcher);
    if (connectWatcher != NULL)
    {
        ev_init(connectWatcher, ((IOChannelVtbl *)me->vptr)->onConnectTimerEvent);
        connectWatcher->repeat = 1.; // start as fast as posible
        // ioCh->reactor = me->station->reactor;
        connectWatcher->data = (void *)me;
        ioCh->connectWatcher = connectWatcher;
        ev_timer_again(ioCh->reactor, connectWatcher);
    }
    // 创建文件夹扫描，不启动
    ChannelFilesWatcher *filesWatcher = NewInstance(ChannelFilesWatcher);
    if (filesWatcher != NULL)
    {
        ev_init(filesWatcher, ((IOChannelVtbl *)me->vptr)->onFilesScanTimerEvent);
        filesWatcher->repeat = 1.; // start as fast as posible
        // ioCh->reactor = me->station->reactor;
        filesWatcher->data = (void *)me;
        ioCh->filesWatcher = filesWatcher;
        // ev_timer_again(ioCh->reactor, filesWatcher);
    }
    // load files index
    char recordsFile[300] = {0};
    uint8_t id = me->id; // @Todo 改为中心站对应的编号，还需要合并主备Channel
    snprintf(recordsFile, 300, "%s/channels/%d/records.json", me->station->config.workDir, id);
    me->recordsFile = strdup(recordsFile);
    cJSON *json = cJSON_FromFile(recordsFile);
    if (json == NULL)
    {
        snprintf(recordsFile, 300, "%s/channels/%d", me->station->config.workDir, id);
        mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
        mkpath(recordsFile, mode);
        json = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "records", cJSON_CreateObject());
    }
    me->recordsFileInJSON = json;
    // start async watcher
    ChannelAsyncWatcher *asyncWatcher = NewInstance(ChannelAsyncWatcher);
    if (asyncWatcher != NULL)
    {
        ev_async_init(asyncWatcher, ((IOChannelVtbl *)me->vptr)->onAsyncEvent);
        asyncWatcher->data = (void *)me;
        ioCh->asyncWatcher = asyncWatcher;
        ev_async_start(ioCh->reactor, ioCh->asyncWatcher);
    }
    ev_run(ioCh->reactor, 0);
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
        ev_io_set(ioCh->dataWatcher, fd, EV_READ | EV_WRITE);
        ev_io_start(reactor, ioCh->dataWatcher);
    }
    else
    {
        ChannelDataWatcher *dataWatcher = NewInstance(ev_io);
        if (dataWatcher != NULL)
        {
            ev_io_init(dataWatcher, ((IOChannelVtbl *)ch->vptr)->onIOReadEvent, fd, EV_READ | EV_WRITE);
            ioCh->dataWatcher = dataWatcher;
            dataWatcher->data = ioCh;
            ev_io_start(reactor, ioCh->dataWatcher);
        }
    }
    ioCh->connectWatcher->repeat = ch->keepaliveTimer;
}

void IOChannel_OnIOReadEvent(Reactor *reactor, ev_io *w, int revents)
{
    IOChannel *ioCh = (IOChannel *)w->data;
    Channel *ch = (Channel *)ioCh;
    if (revents == EV_WRITE)
    {
        ch->isConnnected = true;
        ev_io_set(ioCh->dataWatcher, ioCh->fd, EV_READ);
        ioCh->filesWatcher->repeat = 1;
        ev_timer_start(ioCh->reactor, ioCh->filesWatcher);
        if (ch->station->config.workMode != NULL &&
            (*ch->station->config.workMode == REPORT || *ch->station->config.workMode == REPORT_CONFIRM))
        {
            Channel_TEST(ch, Channel_LastSeq(ch));
            usleep(*ch->station->config.msgSendInterval * 1000);
            // Channel_BASIC_CONFIG(ch);
        }
        return;
    }
    int len = 0;
    ByteBuffer *buff = ch->vptr->onRead(ch);
    if (buff == NULL)
    {
        ev_io_stop(ioCh->reactor, ioCh->dataWatcher);
        ch->isConnnected = false;
        ch->vptr->close(ch);
        ioCh->connectWatcher->repeat = 10;
        ev_timer_again(ioCh->reactor, ioCh->connectWatcher);

        // 停止 文件扫描
        // printf("ch[%2d] stop file scan.\r\n", ch->id);
        ev_timer_stop(ioCh->reactor, ioCh->filesWatcher);
        return;
    }
    while (BB_Available(buff) > 0)
    {
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
            printf("ch[%2d] %7s request[%4X] stx[%4X] ext[%4X] crc[%4x]\r\n", ch->id,
                   handled == true ? "handled" : "droped",
                   pkg->head.funcCode,
                   pkg->head.stxFlag,
                   pkg->tail.etxFlag,
                   pkg->tail.crc);
            if (pkg->vptr->dtor != NULL) // 实现了析构函数
            {                            //
                pkg->vptr->dtor(pkg);    // 调用析构，规范步骤
            }                            //
            DelInstance(pkg);
        }
        else
        {
            int len = BB_Limit(buff);
            char hexStr[(len * 2) + 1];
            string2hexString(ch->buff, hexStr, len);
            printf("ch[%2d] invalid request, errno:[%3d], hex:[%s]\r\n", ch->id, last_error(), hexStr);
            break;
        }
    }
    BB_dtor(buff);
    DelInstance(buff);
}

void IOChannel_OnFilesQuery(Channel *const me)
{
    assert(me);
    IOChannel *ioCh = (IOChannel *)me;
    // pick up one file from filesDir to send
    tinydir_dir dir;
    int i;
    tinydir_open_sorted(&dir, me->station->config.filesDir);
    for (i = 0; i < dir.n_files; i++)
    {
        tinydir_file file;
        tinydir_readfile_n(&dir, &file, i);
        if (!file.is_dir && !Channel_IsFileSent(me, &file))
        {
            ev_suspend(ioCh->reactor);
            Channel_SendFile(me, &file);
            ev_resume(ioCh->reactor);
            if (me->status == CHANNEL_STATUS_WAITTING_SCAN_FILESEND_ACK)
            {
                ioCh->filesWatcher->repeat = 2.; // 复用这个定时器2秒等应答
                ev_timer_again(ioCh->reactor, ioCh->filesWatcher);
            }
            break;
        }
    }
    tinydir_close(&dir);
}

void IOChannel_SendFilePkg(Channel *const me, FilePkg *const filePkg)
{
    assert(me);
    assert(filePkg);
    IOChannel *ioCh = (IOChannel *)me;
    ev_suspend(ioCh->reactor);
    Channel_SendFilePkg(me, filePkg);
    ev_resume(ioCh->reactor);
    if (me->status == CHANNEL_STATUS_WAITTING_AYNC_FILESEND_ACK)
    {
        ioCh->filesWatcher->repeat = 2.; // 复用这个定时器2秒等应答
        ev_timer_again(ioCh->reactor, ioCh->filesWatcher);
    }
}

bool IOChannel_NotifyData(Channel *const me)
{
    assert(me);
    IOChannel *ioCh = (IOChannel *)me;
    ev_async_send(ioCh->reactor, ioCh->asyncWatcher);
    return true;
}

void IOChannel_OnFilesScanEvent(Reactor *reactor, ev_timer *w, int revents)
{
    IOChannel *ioCh = (IOChannel *)w->data;
    Channel *ch = (Channel *)ioCh;
    if (ch->status == CHANNEL_STATUS_WAITTING_AYNC_FILESEND_ACK) // 主动发送没有等到应答
    {
        ch->status = CHANNEL_STATUS_RUNNING;
        if (ch->currentFilePkg != NULL)
        {
            Station_MarkFilePkgSent(ch->station, ch, ch->currentFilePkg, false);
            ch->currentFilePkg = NULL;
        }
        ioCh->filesWatcher->repeat = 10.; // slow down
        ev_timer_again(ioCh->reactor, ioCh->filesWatcher);
        return;
    }
    if (ch->status == CHANNEL_STATUS_WAITTING_SCAN_FILESEND_ACK) // 没有等到应答
    {
        ch->status = CHANNEL_STATUS_RUNNING;
        if (ch->currentFile != NULL)
        {
            DelInstance(ch->currentFile);
        }
        ioCh->filesWatcher->repeat = 10.; // slow down
        ev_timer_again(ioCh->reactor, ioCh->filesWatcher);
        return;
    }
    if (!ch->station->config.scanFiles) // 没有启动扫描
    {
        return;
    }
    // 停止 文件扫描
    ev_timer_stop(ioCh->reactor, ioCh->filesWatcher);
    if (!ch->isConnnected) // protect
    {
        return;
    }
    IOChannel_OnFilesQuery(ch);
    ioCh->filesWatcher->repeat = 10.; // slow down
    ev_timer_again(ioCh->reactor, ioCh->filesWatcher);
}

void IOChannel_OnAsyncEvent(Reactor *reactor, ev_async *w, int revents)
{
    IOChannel *ioCh = (IOChannel *)w->data;
    Channel *ch = (Channel *)ioCh;
    Station_SendPacketsToChannel(ch->station, ch);
    Station_SendFilePkgsToChannel(ch->station, ch);
}

void IOChannel_dtor(Channel *const me)
{
    assert(me);
    IOChannel *ioCh = (IOChannel *)me;
    if (ioCh->reactor)
    {
        if (ioCh->dataWatcher != NULL)
        {
            // ev_io_stop(me->station->reactor, ioCh->dataWatcher);
            ev_io_stop(ioCh->reactor, ioCh->dataWatcher);
        }
        if (ioCh->connectWatcher != NULL)
        {
            // ev_timer_stop(me->station->reactor, ioCh->connectWatcher);
            ev_timer_stop(ioCh->reactor, ioCh->connectWatcher);
        }
        ev_loop_destroy(ioCh->reactor);
    }
    Channel_dtor(me);
}

void IOChannel_ctor(IOChannel *me, Station *const station, size_t buffSize, uint8_t msgSendInterval)
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
         &IOChannel_OnFilesQuery,
         &IOChannel_NotifyData,
         &IOChannel_SendFilePkg,
         &IOChannel_dtor},
        &IOChannel_OnConnectTimerEvent,
        &IOChannel_OnIOReadEvent,
        &IOChannel_OnFilesScanEvent,
        &IOChannel_OnAsyncEvent};
    Channel *super = (Channel *)me;
    Channel_ctor(super, station, buffSize, msgSendInterval);
    super->vptr = (const ChannelVtbl *)(&vtbl);
    me->reactor = ev_loop_new(0);
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
    if (me->id != CHANNEL_ID_FIXED)
    {
        printf("ch[%2d] connecting to %15s:%5d\r\n", ch->id, inet_ntoa(ipv4->addr.sin_addr), ntohs(ipv4->addr.sin_port));
    }
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        return false;
    }
    int on = 1;
#ifdef _WIN32
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (const char *)&on, sizeof(on));
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(on));
#else
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (void *)&on, sizeof(on));
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&on, sizeof(on));
#endif
    setSocketBlockingEnabled(sock, false);
    // if (connect(sock, (struct sockaddr *)ipv4, sizeof(struct sockaddr_in)) < 0)
    // {
    //     close(sock);
    //     return false;
    // }
    connect(sock, (struct sockaddr *)ipv4, sizeof(struct sockaddr_in));
    ioCh->fd = sock;
    // printf("ch[%2d] connected\r\n", ch->id);
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
#ifdef _WIN32
#else
    errno = 0;
#endif
    int len = recv(sock, ch->buff, me->buffSize, 0);
    if (len <= 0)
    {
        int err = 0;
        char *errStr = NULL;
#ifdef _WIN32
        err = WSAGetLastError();
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL, err,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      errStr, 0, NULL);
#else
        err = errno;
        errStr = strerror(err);
#endif
        Ipv4 *ipv4 = ((SocketChannelVtbl *)ch->vptr)->ip((SocketChannel *)me);
        if (me->id != CHANNEL_ID_FIXED)
        {
            printf("ch[%2d] breaking with %15s:%5d, error [%02d] %s\r\n", ch->id,
                   inet_ntoa(ipv4->addr.sin_addr),
                   ntohs(ipv4->addr.sin_port),
                   err,
                   errStr);
        }
#ifdef _WIN32
        LocalFree(errStr);
#endif
        return NULL;
    }
    ByteBuffer *buff = NewInstance(ByteBuffer);
    BB_ctor_wrapped(buff, (uint8_t *)ch->buff, len);
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

void SocketChannel_OnFilesQuery(Channel *const me)
{
    IOChannel_OnFilesQuery(me);
}

bool SocketChannel_NotifyData(Channel *const me)
{
    return IOChannel_NotifyData(me);
}

void SocketChannel_SendFilePkg(Channel *const me, FilePkg *const filePkg)
{
    IOChannel_SendFilePkg(me, filePkg);
}

void SocketChannel_OnFilesScanEvent(Reactor *reactor, ev_timer *w, int revents)
{
    IOChannel_OnFilesScanEvent(reactor, w, revents);
}

void SocketChannel_OnAsyncEvent(Reactor *reactor, ev_async *w, int revents)
{
    IOChannel_OnAsyncEvent(reactor, w, revents);
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

#define SOCKET_CHANNEL_DEFAULT_BUFF_SIZE 3072
#define SOCKET_CHANNEL_DEFAULT_MSG_SEND_INTREVAL 100

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
          &SocketChannel_OnFilesQuery,
          &SocketChannel_NotifyData,
          &SocketChannel_SendFilePkg,
          &SocketChannel_dtor},
         &SocketChannel_OnConnectTimerEvent,
         &SocketChannel_OnIOReadEvent,
         &SocketChannel_OnFilesScanEvent,
         &SocketChannel_OnAsyncEvent},
        &SocketChannel_Ip};
    IOChannel *super = (IOChannel *)me;
    IOChannel_ctor(super, station,
                   station->config.buffSize == NULL
                       ? SOCKET_CHANNEL_DEFAULT_BUFF_SIZE
                       : *(station->config.buffSize),
                   station->config.msgSendInterval == NULL
                       ? SOCKET_CHANNEL_DEFAULT_MSG_SEND_INTREVAL
                       : *(station->config.msgSendInterval));
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
    BB_Expand(buff, ELEMENT_IDENTIFER_LEN + 6 + 3 + 1); // 2+6
    BB_PutUInt8(buff, me->id);
    BB_PutUInt8(buff, (6 + 3 + 1) << 3); // 0x30
    BB_PutUInt8(buff, CHANNEL_IPV4);
    Ipv4Channel *ipv4Channel = (Ipv4Channel *)me;
    struct in_addr addr = ipv4Channel->ipv4.addr.sin_addr;
#ifdef _WIN32
    unsigned long addrInLong = addr.S_un.S_addr;
#else
    unsigned long addrInLong = addr.s_addr;
#endif
    uint64_t ipIn64 = (addrInLong & 0xFF) * 10e8 +
                      ((addrInLong >> 8) & 0xFF) * 10e5 +
                      ((addrInLong >> 16) & 0xFF) * 10e2 +
                      ((addrInLong >> 24) & 0xFF);
    BB_BE_BCDPutUInt(buff, &ipIn64, 6);
    uint32_t port = ntohs(ipv4Channel->ipv4.addr.sin_port);
    BB_BE_BCDPutUInt(buff, &port, 3);
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
          &SocketChannel_OnFilesQuery,
          &SocketChannel_NotifyData,
          &SocketChannel_SendFilePkg,
          &Ipv4Channel_dtor},
         &SocketChannel_OnConnectTimerEvent,
         &SocketChannel_OnIOReadEvent,
         &SocketChannel_OnFilesScanEvent,
         &SocketChannel_OnAsyncEvent},
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
    char portInStr[10] = {0};
    snprintf(portInStr, 10, "%d", ntohs(domainCh->domain.ipv4.addr.sin_port));
    // itoa(ntohs(domainCh->domain.ipv4.addr.sin_port), portInStr, 10);
    uint8_t domainLen = strlen(domainCh->domain.domainStr) + strlen(portInStr) + 1; // one for :
    char domainStr[domainLen + 1];
    BB_Expand(buff, ELEMENT_IDENTIFER_LEN + domainLen + 1); // one more for type
    BB_PutUInt8(buff, me->id);
    BB_PutUInt8(buff, domainLen << 3);
    BB_PutUInt8(buff, CHANNEL_DOMAIN);
    snprintf(domainStr, domainLen, "%s:%s", domainCh->domain.domainStr, portInStr);
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
          &SocketChannel_OnFilesQuery,
          &SocketChannel_NotifyData,
          &SocketChannel_SendFilePkg,
          &DomainChannel_dtor},
         &SocketChannel_OnConnectTimerEvent,
         &SocketChannel_OnIOReadEvent,
         &SocketChannel_OnFilesScanEvent,
         &SocketChannel_OnAsyncEvent},
        &DomainChannel_Ip};
    SocketChannel *super = (SocketChannel *)me;
    SocketChannel_ctor(super, station);
    Channel *ch = (Channel *)me;
    ch->vptr = (const ChannelVtbl *)&vtbl;
}
// DomainChannel END

Channel *Channel_Ipv4FromJson(cJSON *const channelInJson, Config *const config)
{
    assert(channelInJson);
    cJSON_GET_VALUE(id, uint8_t, channelInJson, valuedouble, 0);
    if (!Config_IsChannelEnableById(config, id))
    {
        return NULL;
    }
    cJSON_GET_VALUE(ipv4, char *, channelInJson, valuestring, NULL);
    cJSON_GET_VALUE(port, uint16_t, channelInJson, valuedouble, 60338);
    cJSON_GET_VALUE(keepalive, uint8_t, channelInJson, valuedouble, CHANNLE_DEFAULT_KEEPALIVE_INTERVAL);
    if (ipv4 != NULL && port > 0 &&
        id >= CHANNEL_ID_MASTER_01 && id <= CHANNEL_ID_SLAVE_04)
    {
        Ipv4Channel *ch = NewInstance(Ipv4Channel);
        Ipv4Channel_ctor(ch, config->station); // 初始化 station 为 NULL
        Channel *super = (Channel *)ch;
        super->id = id;
        super->type = CHANNEL_IPV4;
        super->keepaliveTimer = keepalive;
        ch->ipv4.addr.sin_family = AF_INET;
        ch->ipv4.addr.sin_port = htons(port);
#ifdef _WIN32
        if (inet_pton(AF_INET, ipv4, &ch->ipv4.addr.sin_addr) == 1)
#else
        if (inet_aton(ipv4, &ch->ipv4.addr.sin_addr) == 1)
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

Channel *Channel_DomainFromJson(cJSON *const channelInJson, Config *const config)
{
    assert(channelInJson);
    cJSON_GET_VALUE(id, uint8_t, channelInJson, valuedouble, 0);
    if (!Config_IsChannelEnableById(config, id))
    {
        return NULL;
    }
    cJSON_GET_VALUE(domain, char *, channelInJson, valuestring, NULL);
    cJSON_GET_VALUE(port, uint16_t, channelInJson, valuedouble, 60338);
    cJSON_GET_VALUE(keepalive, uint8_t, channelInJson, valuedouble, CHANNLE_DEFAULT_KEEPALIVE_INTERVAL);
    if (domain != NULL && strlen(domain) > 0 && port > 0 &&
        id >= CHANNEL_ID_MASTER_01 && id <= CHANNEL_ID_SLAVE_04)
    {
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            return NULL;
        }
#else

#endif
        struct hostent *hosts = gethostbyname(domain);
        if (hosts == NULL || hosts->h_addrtype != AF_INET)
        {
            return NULL;
        }
        DomainChannel *ch = NewInstance(DomainChannel);
        DomainChannel_ctor(ch, config->station); // 初始化 station 为 NULL
        Channel *super = (Channel *)ch;
        super->id = id;
        super->type = CHANNEL_DOMAIN;
        super->keepaliveTimer = keepalive;
        ch->domain.domainStr = domain;
        ch->domain.ipv4.addr.sin_family = AF_INET;
        ch->domain.ipv4.addr.sin_port = htons(port);
        // GET THE FIRST IP
        ch->domain.ipv4.addr.sin_addr = *(struct in_addr *)hosts->h_addr_list[0];
        return (Channel *)ch;
    }
    else
    {
        return NULL;
    }
}

Channel *Channel_ToiOTA(Config *const config)
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
    DomainChannel_ctor(ch, config->station); // 初始化 station 为 NULL
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

uint8_t Config_CenterAddr(Config *const me, uint8_t id)
{
    assert(me);
    if (me->centerAddrs == NULL || id < 1 || id > 4)
    {
        return 0;
    }
    uint8_t *p = (uint8_t *)me->centerAddrs;
    return *(p + id - 1);
}

bool Config_IsCenterAddrEnable(Config *const me, uint8_t id)
{
    return Config_CenterAddr(me, id) != 0;
}

uint8_t Config_CenterAddrOfChannel(Config *const me, Channel *const ch)
{
    assert(me);
    if (ch == NULL)
    {
        return 0;
    }
    uint8_t id = (ch->id / 2) - 1; //
    return Config_CenterAddr(me, id);
}

bool Config_IsChannelEnable(Config *const me, Channel *const ch)
{
    assert(me);
    return ch->id == CHANNEL_ID_FIXED || Config_CenterAddrOfChannel(me, ch) != CENTER_DISABLED;
}

bool Config_IsChannelEnableById(Config *const me, uint8_t id)
{
    assert(me);
    return id == CHANNEL_ID_FIXED || Config_CenterAddr(me, (id / 2) - 1) != CENTER_DISABLED;
}

Channel *Config_FindChannel(Config *const me, uint8_t chId)
{
    size_t i = 0;
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

int32_t Config_IndexOfChannel(Config *const me, uint8_t chId)
{
    size_t i = 0;
    Channel *ch;
    vec_foreach(&me->channels, ch, i)
    {
        if (ch->id == chId)
        {
            return i;
        }
    }
    return -1;
}

bool Config_InitFromJSON(Config *const me, cJSON *const json)
{
    assert(me);
    assert(json);
    // 中心地址
    cJSON *centerAddrs = cJSON_GetObjectItem(json, "centerAddrs");
    // 站地址
    cJSON *remoteStationAddr = cJSON_GetObjectItem(json, "remoteStationAddr");
    if (centerAddrs == NULL || remoteStationAddr == NULL)
    {
        return false;
    }
    // init channel array
    vec_init(&me->channels);
    vec_reserve(&me->channels, 4); // channel 04~07
    // 中心地址
    me->centerAddrs = NewInstance(CenterAddrs); // we DO NOT care whether it is NULL
    cJSON_COPY_VALUE(me->centerAddrs->addr1, addr1, centerAddrs, valuedouble);
    cJSON_COPY_VALUE(me->centerAddrs->addr2, addr2, centerAddrs, valuedouble);
    cJSON_COPY_VALUE(me->centerAddrs->addr3, addr3, centerAddrs, valuedouble);
    cJSON_COPY_VALUE(me->centerAddrs->addr4, addr4, centerAddrs, valuedouble);
    // 站地址
    me->stationAddr = NewInstance(RemoteStationAddr);
    cJSON_COPY_VALUE(me->stationAddr->A5, A5, remoteStationAddr, valuedouble);
    cJSON_COPY_VALUE(me->stationAddr->A4, A4, remoteStationAddr, valuedouble);
    cJSON_COPY_VALUE(me->stationAddr->A3, A3, remoteStationAddr, valuedouble);
    cJSON_COPY_VALUE(me->stationAddr->A2, A2, remoteStationAddr, valuedouble);
    cJSON_COPY_VALUE(me->stationAddr->A1, A1, remoteStationAddr, valuedouble);
    cJSON_COPY_VALUE(me->stationAddr->A0, A0, remoteStationAddr, valuedouble);
    // 密码
    me->password = NewInstance(uint16_t);
    *me->password = 0; // default but maybe not valid
    cJSON_COPY_VALUE(*me->password, password, json, valuedouble);
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
    cJSON_COPY_VALUE(*me->workMode, workMode, json, valuedouble);
    // StationCategory
    cJSON_GET_NUMBER(stationCategory, StationCategory, json, RIVER_STATION, 16);
    me->stationCategory = stationCategory;
    // filesDir
    cJSON_COPY_VALUE(me->filesDir, filesDir, json, valuestring);
    if (me->filesDir == NULL)
    {
        size_t len = strlen(me->workDir) + 6;
        me->filesDir = (char *)malloc(len); // /pics\0
        memset(me->filesDir, '\0', len);
        snprintf(me->filesDir, len, "%s/pics", me->workDir);
    }
    mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
    mkpath(me->filesDir, mode);
    // sentFilesDir
    cJSON_COPY_VALUE(me->sentFilesDir, sentFilesDir, json, valuestring);
    if (me->sentFilesDir == NULL)
    {
        size_t len = strlen(me->workDir) + 11;
        me->sentFilesDir = (char *)malloc(len); // /pics\0
        memset(me->sentFilesDir, '\0', len);
        snprintf(me->sentFilesDir, len, "%s/pics_sent", me->workDir);
    }
    mkpath(me->sentFilesDir, mode);
    // socketDevice
    cJSON_COPY_VALUE(me->socketDevice, socketDevice, json, valuestring);
    if (me->socketDevice == NULL)
    {
        me->socketDevice = strdup("pp");
    }
    // buffSize
    if (cJSON_HasObjectItem(json, "buffSize"))
    {
        me->buffSize = NewInstance(size_t);
        cJSON_COPY_VALUE(*me->buffSize, buffSize, json, valuedouble);
        if (*me->buffSize < CHANNEL_MIN_BUFF_SIZE || *me->buffSize > CHANNEL_MAX_BUFF_SIZE)
        {
            *me->buffSize = CHANNEL_DEFAULT_BUFF_SIZE;
        }
    }
    // msgSendInterval
    if (cJSON_HasObjectItem(json, "msgSendInterval"))
    {
        me->msgSendInterval = NewInstance(uint16_t);
        cJSON_COPY_VALUE(*me->msgSendInterval, msgSendInterval, json, valuedouble);
        if (*me->msgSendInterval < CHANNEL_MIN_MSG_SEND_INTERVAL || *me->msgSendInterval > CHANNEL_MAX_MSG_SEND_INTERVAL)
        {
            *me->msgSendInterval = CHANNEL_DEFAULT_MSG_SEND_INTERVAL;
        }
    }
    // wait send file ack: default false
    me->waitFileSendAck = cJSON_IsTrue(cJSON_GetObjectItem(json, "waitFileSendAck"));

    // enable scan file to send: default false
    me->scanFiles = cJSON_IsTrue(cJSON_GetObjectItem(json, "scanFiles"));

    // channels
    cJSON *channels = cJSON_GetObjectItem(json, "channels");
    cJSON *channel;
    cJSON_ArrayForEach(channel, channels)
    {
        cJSON_GET_VALUE(type, ChannelType, channel, valuedouble, CHANNEL_DISABLED);
        Channel *ch = NULL;
        switch (type)
        {
        case CHANNEL_IPV4: // 目前只处理IPV4, 其他认为无效
            ch = Channel_Ipv4FromJson(channel, me);
            break;
        case CHANNEL_DOMAIN: // 自定义的域名方式
            ch = Channel_DomainFromJson(channel, me);
            break;
        default:
            break;
        }
        if (ch != NULL && Config_FindChannel(me, ch->id) == NULL)
        {
            vec_push(&me->channels, ch);
        }
    }
    // add a fixed channel to make it reachable
    Channel *ch = Channel_ToiOTA(me);
    if (ch != NULL)
    {
        vec_push(&me->channels, ch);
    }
    return true;
}

bool Config_IsValid(Config *const config)
{
    return config != NULL && config->channels.length > 0;
}

void Config_dtor(Config *const me)
{
    int i;
    Channel *ch = NULL;
    vec_foreach(&me->channels, ch, i)
    {
        if (ch != NULL)
        {
            if (ch->vptr->dtor != NULL)
            {
                ch->vptr->dtor(ch);
            }
            DelInstance(ch);
        }
    }
    if (me->centerAddrs != NULL)
    {
        DelInstance(me->centerAddrs);
    }
    if (me->stationAddr != NULL)
    {
        DelInstance(me->stationAddr);
    }
    if (me->password != NULL)
    {
        DelInstance(me->password);
    }
    ChannelPtrVector channels;
    if (me->workMode != NULL)
    {
        DelInstance(me->workMode);
    }
    if (me->configInJSON != NULL)
    {
        cJSON_Delete(me->configInJSON);
    }
    if (me->configFile != NULL)
    {
        DelInstance(me->configFile);
    }
    if (me->workDir != NULL)
    {
        DelInstance(me->workDir);
    }
    if (me->filesDir != NULL)
    {
        DelInstance(me->filesDir);
    }
    if (me->sentFilesDir != NULL)
    {
        DelInstance(me->sentFilesDir);
    }
    if (me->socketDevice != NULL)
    {
        DelInstance(me->socketDevice);
    }
}

void Config_ctor(Config *const me, Station *const station)
{
    assert(me);
    me->centerAddrs = NULL;
    me->configInJSON = NULL;
    me->configFile = NULL;
    me->workDir = NULL;
    me->password = NULL;
    me->stationAddr = NULL;
    me->workMode = NULL;
    me->station = station;
}

// Packet
void Packet_ctor(Packet *const me, Package *const pkg)
{
    assert(me);
    me->pkg = pkg;
    me->channelSentMask = 0;
}

void Packet_dtor(Packet *const me)
{
    assert(me);
    if (me->pkg != NULL)
    {
        me->pkg->vptr->dtor(me->pkg);
        DelInstance(me->pkg);
    }
}

void Packet_Marking(Packet *const me, uint8_t chId)
{
    assert(me);
    assert(chId > 0 && chId <= 16);
    me->channelSentMask |= (1 << chId);
}

void Packet_MarkingByChannel(Packet *const me, Channel *const ch)
{
    assert(me);
    assert(ch);
    Packet_Marking(me, ch->id);
}

void Packet_Unmark(Packet *const me, uint8_t chId)
{
    assert(me);
    assert(chId > 0 && chId <= 16);
    me->channelSentMask &= ~(1 << chId);
}

void Packet_UnmarkByChannel(Packet *const me, Channel *const ch)
{
    assert(me);
    assert(ch);
    Packet_Unmark(me, ch->id);
}

bool Packet_ShouldSend(Packet *const me, uint8_t chId)
{
    assert(me);
    assert(chId > 0 && chId <= 16);
    return me->channelSentMask & (1 << chId);
}

bool Packet_ShouldSendByChannel(Packet *const me, Channel *const ch)
{
    assert(me);
    assert(ch);
    return Packet_ShouldSend(me, ch->id);
}
// Packet END

// FilePkg
void FilePkg_ctor(FilePkg *const me, const char *file)
{
    assert(me);
    me->file = strdup(file);
    me->channelSentMask = 0;
}

void FilePkg_dtor(FilePkg *const me)
{
    assert(me);
    if (me->file != NULL)
    {
        DelInstance(me->file);
    }
}

void FilePkg_Marking(FilePkg *const me, uint8_t chId)
{
    assert(me);
    assert(chId > 0 && chId <= 16);
    me->channelSentMask |= (1 << chId);
}

void FilePkg_MarkingByChannel(FilePkg *const me, Channel *const ch)
{
    assert(me);
    assert(ch);
    FilePkg_Marking(me, ch->id);
}

void FilePkg_Unmark(FilePkg *const me, uint8_t chId)
{
    assert(me);
    assert(chId > 0 && chId <= 16);
    me->channelSentMask &= ~(1 << chId);
}

void FilePkg_UnmarkByChannel(FilePkg *const me, Channel *const ch)
{
    assert(me);
    assert(ch);
    FilePkg_Unmark(me, ch->id);
}

bool FilePkg_ShouldSend(FilePkg *const me, uint8_t chId)
{
    assert(me);
    assert(chId > 0 && chId <= 16);
    return me->channelSentMask & (1 << chId);
}

bool FilePkg_ShouldSendByChannel(FilePkg *const me, Channel *const ch)
{
    assert(me);
    assert(ch);
    return FilePkg_ShouldSend(me, ch->id);
}
// FilePkg END

void Station_ctor(Station *const me)
{
    assert(me);
    // me->reactor = NULL;
    Config_ctor(&me->config, me);
    pthread_mutexattr_t mutexAttr;
    pthread_mutexattr_init(&mutexAttr);
    pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&me->cleanUpMutex, &mutexAttr);
    pthread_mutex_init(&me->sendMutex, &mutexAttr);
    vec_init(&me->packets);
    vec_reserve(&me->packets, 20);
}

bool Station_StartBy(Station *const me, char const *workDir)
{
    assert(me);
    assert(workDir);
    int len = strlen(workDir) + 1;
    assert(len > 0);
    assert(me->config.centerAddrs == NULL); // assert all config is empty
    int fileLen = len + SL651_DEFAULT_CONFIG_FILE_NAME_LEN + 1;
    char *file = (char *)malloc(fileLen); // config.json
    memset(file, '\0', fileLen);
    snprintf(file, fileLen, "%s/config.json", workDir);
    me->config.workDir = strdup(workDir);
    cJSON *json = cJSON_FromFile(file);
    if (json != NULL &&
        Config_InitFromJSON(&me->config, json) &&
        Config_IsValid(&me->config))
    {
        me->config.configInJSON = json;
        me->config.configFile = file;
        // me->reactor = ev_loop_new(0);
        // if (me->reactor != NULL)
        // {
        int i;
        Channel *ch = NULL;
        vec_foreach(&me->config.channels, ch, i)
        {
            if (Config_IsChannelEnable(&me->config, ch) &&
                (ch->type == CHANNEL_IPV4 || ch->type == CHANNEL_DOMAIN))
            {
                ch->station = me;
                ch->centerAddr = Config_CenterAddrOfChannel(&me->config, ch);
                // ch->vptr->start(ch);
                pthread_t *thread = NewInstance(pthread_t);
                pthread_attr_t attr;
                pthread_attr_init(&attr);
                pthread_attr_setstacksize(&attr, 1024 * 10);
                int res = pthread_create(thread, &attr, (void *(*)(void *))ch->vptr->start, ch);
                if (res)
                {
                    printf("ch[%2d] start failed.", ch->id);
                    DelInstance(thread);
                }
                else
                {
                    ch->thread = thread;
                }
                pthread_attr_destroy(&attr);
            }
        }
        vec_foreach(&me->config.channels, ch, i)
        {
            if (ch != NULL && ch->thread != NULL)
            {
                pthread_join(*ch->thread, NULL);
                DelInstance(ch->thread);
            }
        }
        // // dead loop until no event to be handle
        // if (ev_run(me->reactor, 0))
        // {
        //     return true;
        // }
        return true;
    }
    //     else
    //     {
    //         return false;
    //     }
    // }
    else
    {
        return false;
    }
}

bool Station_Start(Station *const me)
{
    assert(me);
    char const *defaultDir = SL651_DEFAULT_WORKDIR;
    return Station_StartBy(me, defaultDir);
}

bool Station_IsFileSentByAllChannel(Station *const me, tinydir_file *const file, Channel *const currentCh)
{
    assert(me);
    assert(file);
    pthread_mutex_lock(&me->cleanUpMutex);
    // 找文件记录，比较计数，是否移动文件
    size_t i = 0;
    Channel *ch;
    vec_foreach(&me->config.channels, ch, i)
    {
        if (ch != NULL &&
            ch->id != CHANNEL_ID_FIXED &&
            (currentCh != NULL ? ch->id != currentCh->id : true) &&
            Config_IsChannelEnable(&me->config, ch) &&
            !Channel_IsFileSent(ch, file))
        {
            pthread_mutex_unlock(&me->cleanUpMutex);
            return false;
        }
    }
    // clear
    // move file
    char dir[40] = {0};
    time_t nSeconds;
    struct tm *pTM;
    time(&nSeconds);
    pTM = localtime(&nSeconds);
    snprintf(dir, 300, "%s/%04d%02d%02d", me->config.sentFilesDir, pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday);
    mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
    mkpath(dir, mode);
    char newFile[300] = {0};
    snprintf(newFile, 300, "%s/%s", dir, file->name);
    i = 1;
    while (access(newFile, 0) == SL651_APP_ERROR_SUCCESS)
    {
        snprintf(newFile, 300, "%s/%s.(%d)", dir, file->name, (int)i);
        i++;
    }
    if (rename(file->path, newFile) != SL651_APP_ERROR_SUCCESS)
    {
        remove(file->path);
    }
    vec_foreach(&me->config.channels, ch, i)
    {
        if (ch != NULL &&
            (currentCh != NULL ? ch->id != currentCh->id : true) &&
            Config_IsChannelEnable(&me->config, ch))
        {
            Channel_ClearSentFileRecord(ch, file);
        }
    }
    pthread_mutex_unlock(&me->cleanUpMutex);
    return true;
}

bool Station_AsyncSend(Station *const me, cJSON *const data)
{
    assert(me);
    if (data == NULL)
    {
        return true;
    }
    pthread_mutex_lock(&me->sendMutex);
    // 生成
    Package *pkg = createPackage(data);
    if (pkg == NULL)
    {
        return false;
    }
    Packet *packet = NewInstance(Packet);
    Packet_ctor(packet, pkg);
    size_t i = 0;
    Channel *ch;
    vec_foreach(&me->config.channels, ch, i)
    {
        if (ch != NULL &&
            ch->id != CHANNEL_ID_FIXED &&
            Config_IsChannelEnable(&me->config, ch) &&
            Channel_IsConnected(ch) && // 尽可能不导致数据无法清理 isConnected是不安全的
            ch->vptr->notifyData(ch))
        {
            Packet_MarkingByChannel(packet, ch);
        }
    }
    if (packet->channelSentMask != 0)
    {
        vec_push(&me->packets, packet);
    }
    else
    {
        Packet_dtor(packet);
        DelInstance(packet);
    }
    pthread_mutex_unlock(&me->sendMutex);
    return true;
}

void Station_SendPacketsToChannel(Station *const me, Channel *const ch)
{
    assert(me);
    pthread_mutex_lock(&me->sendMutex);
    size_t i = 0;
    Packet *packet;
    vec_foreach(&me->packets, packet, i)
    {
        if (packet == NULL)
        {
            vec_remove(&me->packets, packet);
            continue;
        }
        else
        {
            if (packet->pkg != NULL && Packet_ShouldSendByChannel(packet, ch))
            {
                Package *pkg = packet->pkg;
                if (pkg->head.direction == Up)
                {
                    UplinkMessage *msg = (UplinkMessage *)pkg;
                    msg->messageHead.seq = Channel_NextSeq(ch);
                }
                Channel_FillPackageHead(ch, pkg); // 每个channel不同
                ByteBuffer *buff = pkg->vptr->encode(pkg);
                if (buff == NULL)
                {
                    Packet_UnmarkByChannel(packet, ch);
                    continue;
                }
                BB_Flip(buff);
                bool res = ch->vptr->send(ch, buff);
                BB_dtor(buff);
                DelInstance(buff);
                Packet_UnmarkByChannel(packet, ch); // @Todo 发送失败的重试机制
                // if (res)
                // {
                //     Packet_UnmarkByChannel(packet, ch);
                // }
                // else
                // {
                //     ch->vptr->notifyData(ch); // try again later. 次数
                // }
            }
            if (Packet_IsSent(packet))
            {
                vec_remove(&me->packets, packet);
                Packet_dtor(packet);
                DelInstance(packet);
            }
        }
    }
    // 压缩掉
    vec_compact(&me->packets);
    pthread_mutex_unlock(&me->sendMutex);
}

void Station_ClearPackets(Station *const me)
{
    assert(me);
    size_t i = 0;
    Packet *packet;
    vec_foreach(&me->packets, packet, i)
    {
        if (packet != NULL)
        {
            Packet_dtor(packet);
            DelInstance(packet);
        }
    }
    vec_deinit(&me->packets);
}

bool Station_AsyncSendFilePkg(Station *const me, const char *file)
{
    assert(me);
    if (file == NULL)
    {
        return true;
    }
    struct stat fStat = {0};
    if (stat(file, &fStat) != SL651_APP_ERROR_SUCCESS)
    {
        return true;
    }
    if (fStat.st_size <= 0)
    {
        return true;
    }
    pthread_mutex_lock(&me->sendMutex);
    // 生成
    FilePkg *f = NewInstance(FilePkg);
    FilePkg_ctor(f, file);
    size_t i = 0;
    Channel *ch;
    vec_foreach(&me->config.channels, ch, i)
    {
        if (ch != NULL &&
            ch->id != CHANNEL_ID_FIXED &&
            Config_IsChannelEnable(&me->config, ch) &&
            Channel_IsConnected(ch) && // 尽可能不导致数据无法清理 isConnected是不安全的
            ch->vptr->notifyData(ch))
        {
            FilePkg_MarkingByChannel(f, ch);
        }
    }
    if (f->channelSentMask != 0)
    {
        vec_push(&me->files, f);
    }
    else
    {
        FilePkg_dtor(f);
        DelInstance(f);
    }
    pthread_mutex_unlock(&me->sendMutex);
    return true;
}

void Station_SendFilePkgsToChannel(Station *const me, Channel *const ch)
{
    assert(me);
    pthread_mutex_lock(&me->sendMutex);
    size_t i = 0;
    FilePkg *f;
    vec_foreach(&me->files, f, i)
    {
        if (f == NULL)
        {
            vec_remove(&me->files, f);
            continue;
        }
        else
        {
            if (f->file != NULL && FilePkg_ShouldSendByChannel(f, ch))
            {
                ch->vptr->sendFilePkg(ch, f);
            }
        }
    }
    pthread_mutex_unlock(&me->sendMutex);
}

void Station_MarkFilePkgSent(Station *const me, Channel *const ch, FilePkg *const filePkg, bool result)
{
    assert(me);
    assert(ch);
    assert(filePkg);
    pthread_mutex_lock(&me->sendMutex);
    vec_remove(&me->files, filePkg);
    pthread_mutex_unlock(&me->sendMutex);
}

void Station_ClearFilePkgs(Station *const me)
{
    assert(me);
    size_t i = 0;
    FilePkg *f;
    vec_foreach(&me->files, f, i)
    {
        if (f != NULL)
        {
            FilePkg_dtor(f);
            DelInstance(f);
        }
    }
    vec_deinit(&me->files);
}

void Station_dtor(Station *const me)
{
    assert(me);
    Config_dtor(&me->config);
    pthread_mutex_destroy(&me->cleanUpMutex);
    pthread_mutex_destroy(&me->sendMutex);
    Station_ClearPackets(me);
    Station_ClearFilePkgs(me);
    // if (me->reactor)
    // {
    //     ev_loop_destroy(me->reactor);
    // }
}
