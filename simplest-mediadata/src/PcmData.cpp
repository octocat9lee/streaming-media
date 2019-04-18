// ==============================================================
// [视音频数据处理入门：PCM音频采样数据处理]
// https://blog.csdn.net/leixiaohua1020/article/details/50534316
// ==============================================================

#include "SimplestMediadata.h"
#include "util.h"

#include <stdint.h>
#include <iostream>

#ifdef __GNUC__
#define GNUC_PACKED  __attribute__ ((packed))
#else
#define GNUC_PACKED
#endif

typedef struct
{
    int8_t   fccID[4];
    uint32_t dwSize;
    int8_t   fccType[4];
}GNUC_PACKED WAVE_HEADER;

typedef struct
{
    int8_t    fccID[4];
    uint32_t  dwSize;
    uint16_t  wFormatTag;
    uint16_t  wChannels;
    uint32_t  dwSamplesPerSec;
    uint32_t  dwAvgBytesPerSec;
    uint16_t  wBlockAlign;
    uint16_t  uiBitsPerSample;
}GNUC_PACKED WAVE_FMT;

typedef struct WAVE_DATA
{
    int8_t   fccID[4];
    uint32_t dwSize;
}WAVE_DATA;

#ifndef __GNUC__
#pragma pack(1)
#endif

#ifndef __GNUC__
#pragma pack()
#endif

void simplest_pcm16le_split(const std::string &url)
{
    FILE *fp = fopen(url.c_str(), "rb");
    std::string outpath = GetFilename(url);
    FILE *left = fopen((outpath + "_split_l.pcm").c_str(), "wb+");
    FILE *right = fopen((outpath + "_split_r.pcm").c_str(), "wb+");

    unsigned char sample[4];;
    while(!feof(fp))
    {
        fread(sample, 1, 4, fp);
        //L
        fwrite(sample, 1, 2, left);
        //R
        fwrite(sample + 2, 1, 2, right);
    }

    fclose(fp);
    fclose(left);
    fclose(right);
}


void simplest_pcm16le_halfvolumeleft(const std::string &url)
{
    FILE *fp = fopen(url.c_str(), "rb");
    std::string outpath = GetFilename(url);
    FILE *halfleft = fopen((outpath + "_halfleft.pcm").c_str(), "wb+");

    unsigned char sample[4];
    while(!feof(fp))
    {
        fread(sample, 1, 4, fp);
        int16_t *leftsample = (int16_t*)sample;
        *leftsample = (*leftsample) / 2;
        //L
        fwrite(sample, 1, 2, halfleft);
        //R
        fwrite(sample + 2, 1, 2, halfleft);
    }

    fclose(fp);
    fclose(halfleft);
}

void simplest_pcm16le_to_pcm8(const std::string &url)
{
    FILE *fp = fopen(url.c_str(), "rb");
    std::string outpath = GetFilename(url);
    FILE *pcm8 = fopen((outpath + "_us8le.pcm").c_str(), "wb+");

    unsigned char sample[4];
    while(!feof(fp))
    {
        fread(sample, 1, 4, fp);
        //(-32768-32767)
        int16_t *samplenum16 = (int16_t*)sample;
        int8_t samplenum8 = (*samplenum16) >> 8;
        //(0-255)
        uint8_t samplenum8_u = samplenum8 + 128;
        //L
        fwrite(&samplenum8_u, 1, 1, pcm8);

        samplenum16 = (int16_t*)(sample + 2);
        samplenum8 = (*samplenum16) >> 8;
        samplenum8_u = samplenum8 + 128;
        //R
        fwrite(&samplenum8_u, 1, 1, pcm8);
    }
    fclose(fp);
    fclose(pcm8);
}


void simplest_pcm16le_to_wave(const std::string &pcmpath, int channels, int sample_rate)
{
    std::string wavepath = GetFilename(pcmpath) + ".wav";

    if(channels == 0 || sample_rate == 0)
    {
        channels = 2;
        sample_rate = 44100;
    }
    int bits = 16;    
    FILE *wav = fopen(wavepath.c_str(), "wb+");
    if(wav == NULL)
    {
        return;
    }

    // WAVE_HEADER
    WAVE_HEADER pcmHEADER;
    memcpy(pcmHEADER.fccID, "RIFF", strlen("RIFF"));
    memcpy(pcmHEADER.fccType, "WAVE", strlen("WAVE"));
    fseek(wav, sizeof(WAVE_HEADER), SEEK_CUR);

    // WAVE_FMT
    uint16_t pcmData;
    WAVE_FMT pcmFMT;
    pcmFMT.dwSamplesPerSec = sample_rate;
    pcmFMT.dwAvgBytesPerSec = pcmFMT.dwSamplesPerSec * sizeof(pcmData);
    pcmFMT.uiBitsPerSample = bits;
    memcpy(pcmFMT.fccID, "fmt ", strlen("fmt "));
    pcmFMT.dwSize = 16;
    pcmFMT.wBlockAlign = 2;
    pcmFMT.wChannels = channels;
    pcmFMT.wFormatTag = 1;
    fwrite(&pcmFMT, sizeof(WAVE_FMT), 1, wav);

    // WAVE_DATA
    WAVE_DATA pcmDATA;
    memcpy(pcmDATA.fccID, "data", strlen("data"));
    pcmDATA.dwSize = 0;
    fseek(wav, sizeof(WAVE_DATA), SEEK_CUR);

    FILE *pcm = fopen(pcmpath.c_str(), "rb");
    if(pcm == NULL)
    {
        return;
    }
    fread(&pcmData, sizeof(uint16_t), 1, pcm);
    while(!feof(pcm))
    {
        pcmDATA.dwSize += 2;
        fwrite(&pcmData, sizeof(uint16_t), 1, wav);
        fread(&pcmData, sizeof(uint16_t), 1, pcm);
    }

    pcmHEADER.dwSize = 36 + pcmDATA.dwSize;

    rewind(wav);
    fwrite(&pcmHEADER, sizeof(WAVE_HEADER), 1, wav);

    fseek(wav, sizeof(WAVE_FMT), SEEK_CUR);
    fwrite(&pcmDATA, sizeof(WAVE_DATA), 1, wav);

    fclose(pcm);
    fclose(wav);
}
