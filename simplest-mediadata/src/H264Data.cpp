// ======================================================================
// [视音频数据处理入门：H.264视频码流解析]
// https://blog.csdn.net/leixiaohua1020/article/details/50534369
// [H264--2--语法及结构]
// https://blog.csdn.net/yangzhongxuan/article/details/8003494
// ======================================================================

#include "util.h"
#include "SimplestMediadata.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NALU 头结构（1Byte）
// forbidden_bit(1bit) + nal_reference_bit(2bit) + nal_unit_type(5bit)

typedef enum
{
    NALU_TYPE_SLICE = 1,
    NALU_TYPE_DPA = 2,
    NALU_TYPE_DPB = 3,
    NALU_TYPE_DPC = 4,
    NALU_TYPE_IDR = 5,
    NALU_TYPE_SEI = 6,
    NALU_TYPE_SPS = 7,
    NALU_TYPE_PPS = 8,
    NALU_TYPE_AUD = 9,
    NALU_TYPE_EOSEQ = 10,
    NALU_TYPE_EOSTREAM = 11,
    NALU_TYPE_FILL = 12,
} NaluType;

typedef enum
{
    NALU_PRIORITY_DISPOSABLE = 0,
    NALU_PRIRITY_LOW = 1,
    NALU_PRIORITY_HIGH = 2,
    NALU_PRIORITY_HIGHEST = 3
} NaluPriority;

typedef struct
{
    int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
    unsigned len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
    unsigned max_size;            //! Nal Unit Buffer size
    int forbidden_bit;            //! should be always FALSE
    int nal_reference_idc;        //! NALU_PRIORITY_xxxx
    int nal_unit_type;            //! NALU_TYPE_xxxx    
    char *buf;                    //! contains the first byte followed by the EBSP
} NALU_t;

typedef struct
{
    unsigned char *buf;
    int length;
} NALUBuffer;

typedef struct  
{
    unsigned pos;
    int startcodeprefix_len;
} NextNALUInfo;

static int FindStartCodePrefix2(const unsigned char *buf, unsigned offset)
{
    if(buf[offset] != 0 || buf[offset + 1] != 0 || buf[offset + 2] != 1)  //0x000001?
    {
        return 0;
    }
    else
    {
        return 3;
    }
}

static int FindStartCodePrefix3(const unsigned char *buf, unsigned offset)
{
    if(buf[offset] != 0 || buf[offset + 1] != 0 || buf[offset + 2] != 0 || buf[offset + 3] != 1) //0x00000001?
    {
        return 0;
    }
    else
    {
        return 4;
    }
}

void ParseNALU(const NALUBuffer &naluBuf, NALU_t &n)
{
    if(naluBuf.buf && naluBuf.length > 0)
    {
        unsigned char h = naluBuf.buf[n.startcodeprefix_len];
        n.forbidden_bit = h & 0x80;     //1 bit
        n.nal_reference_idc = (h & 0x60) >> 5; // 2 bit
        n.nal_unit_type = h & 0x1f;     // 5 bit
    }
}

void PrintNaluInfo(const unsigned &num, const unsigned &offset, const NALU_t &n)
{
    char type_str[20] = {0};
    switch(n.nal_unit_type)
    {
        case NALU_TYPE_SLICE:sprintf(type_str, "SLICE"); break;
        case NALU_TYPE_DPA:sprintf(type_str, "DPA"); break;
        case NALU_TYPE_DPB:sprintf(type_str, "DPB"); break;
        case NALU_TYPE_DPC:sprintf(type_str, "DPC"); break;
        case NALU_TYPE_IDR:sprintf(type_str, "IDR"); break;
        case NALU_TYPE_SEI:sprintf(type_str, "SEI"); break;
        case NALU_TYPE_SPS:sprintf(type_str, "SPS"); break;
        case NALU_TYPE_PPS:sprintf(type_str, "PPS"); break;
        case NALU_TYPE_AUD:sprintf(type_str, "AUD"); break;
        case NALU_TYPE_EOSEQ:sprintf(type_str, "EOSEQ"); break;
        case NALU_TYPE_EOSTREAM:sprintf(type_str, "EOSTREAM"); break;
        case NALU_TYPE_FILL:sprintf(type_str, "FILL"); break;
    }
    char idc_str[20] = {0};
    switch(n.nal_reference_idc)
    {
        case NALU_PRIORITY_DISPOSABLE:sprintf(idc_str, "DISPOS"); break;
        case NALU_PRIRITY_LOW:sprintf(idc_str, "LOW"); break;
        case NALU_PRIORITY_HIGH:sprintf(idc_str, "HIGH"); break;
        case NALU_PRIORITY_HIGHEST:sprintf(idc_str, "HIGHEST"); break;
    }
    fprintf(stdout, "%5d| 0x%6x| %7s| %6s| %8d| %8d|\n", num, offset, idc_str, type_str, n.len, n.startcodeprefix_len);
}

NextNALUInfo GetNextNaluPos(const unsigned char *buf, const unsigned start, unsigned maxsize)
{
    NextNALUInfo info;
    int offset = start;
    int codeLen = 0;
    while(offset < maxsize)
    {
        codeLen = FindStartCodePrefix2(buf, offset);
        if(codeLen)
        {
            break;
        }
        else
        {
            codeLen = FindStartCodePrefix3(buf, offset);
            if(codeLen)
            {
                break;
            }
        }
        ++offset;
    }
    info.pos = offset;
    info.startcodeprefix_len = codeLen;
    return info;
}

void simplest_h264_parser(const std::string &url)
{
    unsigned size = GetFileSize(url);
    unsigned char *h264buf = new unsigned char[size];
    ReadFile(url, h264buf, size);

    printf("-----+-------- NALU Table ------+---------+---------+\n");
    printf(" NUM |    POS  |    IDC |  TYPE |   LEN   |START_LEN|\n");
    printf("-----+---------+--------+-------+---------+---------+\n");

    unsigned num = 0;
    NALUBuffer nalu;
    NextNALUInfo naluInfo = GetNextNaluPos(h264buf, 0, size);
    NextNALUInfo nextNaluInfo;
    while(naluInfo.pos + naluInfo.startcodeprefix_len < size)
    {
        unsigned offset = naluInfo.pos + naluInfo.startcodeprefix_len;
        nextNaluInfo = GetNextNaluPos(h264buf, offset, size);

        NALUBuffer naluBuf;
        naluBuf.buf = h264buf + naluInfo.pos;
        naluBuf.length = nextNaluInfo.pos - naluInfo.pos;
        NALU_t n;
        n.startcodeprefix_len = naluInfo.startcodeprefix_len;
        n.len = nextNaluInfo.pos - naluInfo.pos - naluInfo.startcodeprefix_len;
        ParseNALU(naluBuf, n);
        PrintNaluInfo(num, naluInfo.pos, n);
        ++num;
        naluInfo = nextNaluInfo;
    }

    delete[] h264buf;
}
