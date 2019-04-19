// ======================================================================
// [视音频数据处理入门：AAC音频码流解析]
// https://blog.csdn.net/leixiaohua1020/article/details/50535042
// [ADTS]
// https://wiki.multimedia.cx/index.php/ADTS
// ======================================================================

#include "SimplestMediadata.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getADTSframe(unsigned char *buffer, int buf_size, unsigned char *data, int *data_size)
{
    int size = 0;

    if(!buffer || !data || !data_size)
    {
        return -1;
    }

    while(true)
    {
        if(buf_size < 7)
        {
            return -1;
        }
        //Sync words
        if((buffer[0] == 0xff) && ((buffer[1] & 0xf0) == 0xf0))
        {
            size |= ((buffer[3] & 0x03) << 11);     //high 2 bit
            size |= buffer[4] << 3;                 //middle 8 bit
            size |= ((buffer[5] & 0xe0) >> 5);      //low 3 bit
            break;
        }
        --buf_size;
        ++buffer;
    }

    if(buf_size < size)
    {
        return 1;
    }

    memcpy(data, buffer, size);
    *data_size = size;

    return 0;
}

void simplest_aac_parser(const std::string &url)
{
    int data_size = 0;
    int size = 0;
    int cnt = 0;
    int offset = 0;

    unsigned char *aacframe = (unsigned char *)malloc(1024 * 5);
    unsigned char *aacbuffer = (unsigned char *)malloc(1024 * 1024);

    FILE *aac = fopen(url.c_str(), "rb");
    if(!aac)
    {
        printf("Open file error");
        return;
    }

    printf("-----+- ADTS Frame Table -+------+\n");
    printf(" NUM | Profile | Frequency| Size |\n");
    printf("-----+---------+----------+------+\n");

    while(!feof(aac))
    {
        data_size = fread(aacbuffer + offset, 1, 1024 * 1024 - offset, aac);
        unsigned char* input_data = aacbuffer;

        while(1)
        {
            int ret = getADTSframe(input_data, data_size, aacframe, &size);
            if(ret == -1)
            {
                break;
            }
            else if(ret == 1)
            {
                memcpy(aacbuffer, input_data, data_size);
                offset = data_size;
                break;
            }

            char profile_str[10] = {0};
            char frequence_str[10] = {0};

            unsigned char profile = aacframe[2] & 0xC0;
            profile = profile >> 6;
            switch(profile)
            {
                case 0: sprintf(profile_str, "Main"); break;
                case 1: sprintf(profile_str, "LC"); break;
                case 2: sprintf(profile_str, "SSR"); break;
                default:sprintf(profile_str, "unknown"); break;
            }

            unsigned char sampling_frequency_index = aacframe[2] & 0x3C;
            sampling_frequency_index = sampling_frequency_index >> 2;
            switch(sampling_frequency_index)
            {
                case 0: sprintf(frequence_str, "96000Hz"); break;
                case 1: sprintf(frequence_str, "88200Hz"); break;
                case 2: sprintf(frequence_str, "64000Hz"); break;
                case 3: sprintf(frequence_str, "48000Hz"); break;
                case 4: sprintf(frequence_str, "44100Hz"); break;
                case 5: sprintf(frequence_str, "32000Hz"); break;
                case 6: sprintf(frequence_str, "24000Hz"); break;
                case 7: sprintf(frequence_str, "22050Hz"); break;
                case 8: sprintf(frequence_str, "16000Hz"); break;
                case 9: sprintf(frequence_str, "12000Hz"); break;
                case 10: sprintf(frequence_str, "11025Hz"); break;
                case 11: sprintf(frequence_str, "8000Hz"); break;
                default:sprintf(frequence_str, "unknown"); break;
            }
            data_size -= size;
            input_data += size;
            cnt++;
            printf("%5d| %8s|  %8s| %5d|\n", cnt, profile_str, frequence_str, size);
        }
    }
    fclose(aac);
    free(aacbuffer);
    free(aacframe);
}
