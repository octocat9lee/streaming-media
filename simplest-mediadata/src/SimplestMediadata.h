#ifndef SIMPLEST_MEDIA_DATA_H
#define SIMPLEST_MEDIA_DATA_H

#include <string>

void simplest_pcm16le_split(const std::string &url);

void simplest_pcm16le_halfvolumeleft(const std::string &url);

void simplest_pcm16le_to_pcm8(const std::string &url);

void simplest_pcm16le_to_wave(const std::string &pcmpath, int channels, int sample_rate);

void simplest_aac_parser(const std::string &url);

int simplest_flv_parser(const std::string &url);

#endif
