// ======================================================================
// [视音频数据处理入门：UDP-RTP协议解析]
// https://blog.csdn.net/leixiaohua1020/article/details/50535230
// [视音频数据处理入门：H.264视频码流解析]
// https://blog.csdn.net/leixiaohua1020/article/details/50534369
// [GB28181的PS流完全分析(封装/分包发送/接收组包/解析)]
// https://www.cnblogs.com/dong1/p/11051708.html
// ======================================================================

#include "SimplestMediadata.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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
    unsigned char marker : 1;         /* expect 1 */
    /* bytes 2, 3 */
    uint16_t seq_no;
    /* bytes 4-7 */
    uint32_t timestamp;
    /* bytes 8-11 */
    uint32_t ssrc;                    /* stream number is used here. */
} RTP_FIXED_HEADER;


typedef struct RTP_SELF_HEADER
{
    uint32_t length;
};

#pragma pack(pop)

void PrintRTPHeader(const RTP_FIXED_HEADER &rtp)
{
    printf("============== RTP Header ==============\n");
    printf("Version:       %d\n", rtp.version);
    printf("Padding:       %d\n", rtp.padding);
    printf("Extension:     %d\n", rtp.extension);
    printf("CCCount:       %d\n", rtp.csrc_len);
    printf("Marker:        %d\n", rtp.marker);
    printf("PayloadType:   %d\n", rtp.payload);
    printf("SN:            %u\n", rtp.seq_no);
    printf("Timestamp:     %u\n", rtp.timestamp);
    printf("SSRC:          %u\n", rtp.ssrc);
    printf("========================================\n");
}

void simplest_rtp_parser(const std::string &url)
{
    unsigned size = GetFileSize(url);
    unsigned char *rtpBuf = new unsigned char[size];
    ReadFile(url, rtpBuf, size);

    // RTP header
    RTP_FIXED_HEADER rtp = *(RTP_FIXED_HEADER*)(rtpBuf + sizeof(RTP_SELF_HEADER));
    rtp.seq_no = ntohs(rtp.seq_no);
    rtp.timestamp = ntohl(rtp.timestamp);
    rtp.ssrc = ntohl(rtp.ssrc);
    PrintRTPHeader(rtp);

    printf("---------+---------|-- RTP --|---------+---------+\n");
    printf("   seq   |   type  |  stamp  |  marker |  length |\n");
    printf("---------+---------+---------+---------+---------|\n");
    uint32_t offset = 0;
    while(offset < size)
    {
        // | rtp length (4Byte) | rtp data | rtp length (4Byte) | rtp data | .....
        RTP_SELF_HEADER self = *(RTP_SELF_HEADER*)(rtpBuf + offset);
        RTP_FIXED_HEADER rtp = *(RTP_FIXED_HEADER*)(rtpBuf + offset + sizeof(RTP_SELF_HEADER));

        rtp.seq_no = ntohs(rtp.seq_no);
        rtp.timestamp = ntohl(rtp.timestamp);
        rtp.ssrc = ntohl(rtp.ssrc);

        fprintf(stdout, "%9d| %8d| %8d| %8d| %8d|\n", rtp.seq_no, rtp.payload,
            rtp.timestamp, rtp.marker, self.length);

        offset += (self.length + sizeof(RTP_SELF_HEADER));
    }

    delete[] rtpBuf;
}