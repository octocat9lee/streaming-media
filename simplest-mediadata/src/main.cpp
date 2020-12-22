#include "SimplestMediadata.h"
#include <iostream>

const std::string resourcePath("E:\\github\\streaming-media\\resource\\");

void pcm()
{
    //const std::string pcm16lePath = resourcePath + std::string("NocturneNo2inEflat_44.1k_s16le.pcm");
    //simplest_pcm16le_split(pcm16lePath);

    //simplest_pcm16le_halfvolumeleft(pcm16lePath);

    //simplest_pcm16le_to_pcm8(pcm16lePath);

    //simplest_pcm16le_to_wave(pcm16lePath, 2, 44100);
}

void aac()
{
    simplest_aac_parser(resourcePath + std::string("nocturne.aac"));
}

void flv()
{
    simplest_flv_parser(resourcePath + std::string("cuc_ieschool.flv"));
}

void rtp()
{
    simplest_rtp_parser(resourcePath + std::string("length_udp.rtp"));
}

void h264()
{
    simplest_h264_parser(resourcePath + std::string("sintel.h264"));
}

int main()
{
    rtp();
    return 0;
}
