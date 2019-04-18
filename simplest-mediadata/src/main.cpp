#include "SimplestMediadata.h"

#include <iostream>

const std::string resourcePath("E:\\github\\streaming-media\\resource\\");

int main()
{
    const std::string pcm16lePath = resourcePath + std::string("NocturneNo2inEflat_44.1k_s16le.pcm");
    simplest_pcm16le_split(pcm16lePath);

    simplest_pcm16le_halfvolumeleft(pcm16lePath);

    simplest_pcm16le_to_pcm8(pcm16lePath);

    simplest_pcm16le_to_wave(pcm16lePath, 2, 44100);

    return 0;
}
