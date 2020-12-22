// ======================================================================
// [视音频数据处理入门：UDP-RTP协议解析]
// https://blog.csdn.net/leixiaohua1020/article/details/50535230
// [视音频数据处理入门：H.264视频码流解析]
// https://blog.csdn.net/leixiaohua1020/article/details/50534369
// [GB28181的PS流完全分析(封装/分包发送/接收组包/解析)]
// https://www.cnblogs.com/dong1/p/11051708.html
// ======================================================================

#include "SimplestMediadata.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#pragma pack(1)

typedef struct RTP_FIXED_HEADER
{
    /* byte 0 */
    unsigned char csrc_len : 4;       /* expect 0 */
    unsigned char extension : 1;      /* expect 1 */
    unsigned char padding : 1;        /* expect 0 */
    unsigned char version : 2;        /* expect 2 */
    /* byte 1 */
    unsigned char payload : 7;
    unsigned char marker : 1;        /* expect 1 */
    /* bytes 2, 3 */
    unsigned short seq_no;
    /* bytes 4-7 */
    unsigned  long timestamp;
    /* bytes 8-11 */
    unsigned long ssrc;            /* stream number is used here. */
} RTP_FIXED_HEADER;

#pragma pack(pop)

void simplest_rtp_parser(const std::string &url)
{
    printf("RTP Header: %u\n", sizeof(RTP_FIXED_HEADER));
    FILE *rtpFile = fopen(url.c_str(), "rb");
    if(rtpFile == nullptr)
    {
        printf("Failed to open files!");
        return;
    }

    //RTP header
    RTP_FIXED_HEADER rtp;
    fread((char *)&rtp, 1, sizeof(RTP_FIXED_HEADER), rtpFile);
    unsigned int seq_no = ntohs(rtp.seq_no);
    unsigned int timestamp = ntohl(rtp.timestamp);
    unsigned int ssrc = ntohl(rtp.ssrc);

    printf("============== RTP Header ==============\n");
    printf("Version:       %d\n", rtp.version);
    printf("Padding:       %d\n", rtp.padding);
    printf("Extension:     %d\n", rtp.extension);
    printf("CCCount:       %d\n", rtp.csrc_len);
    printf("Marker:        %d\n", rtp.marker);
    printf("PayloadType:   %d\n", rtp.payload);
    printf("SN:            %u\n", seq_no);
    printf("Timestamp:     %u\n", timestamp);
    printf("SSRC:          %u\n", ssrc);
    printf("========================================\n");
}