/**
 * @file libsiren-msnsiren-decode.c
 * @author your name (you@domain.com)
 * @brief 
 * 
 *   gcc -g3 -Wall libsiren-msnsiren-decode.c -o libsiren-msnsiren-decode -Wl,-rpath,.libs -I. -L.libs -lsiren
 *   ./libsiren-msnsiren-decode msnsiren.wav.raw msnsiren.wav.raw.siren7.s16le.ac1.ar16000.pcm
 *   ffmpeg -f s16le -ac 1 -ar 16000 -i msnsiren.wav.raw.siren7.s16le.ac1.ar16000.pcm msnsiren.wav.raw.siren7.pcm.wav
 * 
 * @version 0.1
 * @date 2023-12-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "decoder.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: ./%s SIREN_SRC PCM_DST\n", argv[0]);
        return 1;
    }

    FILE *i_fp = fopen(argv[1], "rb");
    if (!i_fp)
    {
        printf("Failed to open file %s\n", argv[1]);
        return 1;
    }

    FILE *o_fp = fopen(argv[2], "wb");
    if (!o_fp)
    {
        printf("Failed to open file %s\n", argv[2]);
        return 1;
    }

    SirenDecoder decoder = Siren7_NewDecoder(16000);
    if (!decoder)
    {
        printf("Failed to create decoder SirenDecoder\n");
        goto return_fail;
    }

    unsigned char ibuffer[40];
    unsigned char obuffer[640];
    int samples = 320;
    int datasize = 2;
    int channels = 1;
    int frames = 0;
    while (fread(ibuffer, sizeof(ibuffer), 1, i_fp) == 1)
    {
        memset(obuffer, 0, sizeof(obuffer));
        int ret = Siren7_DecodeFrame(decoder, ibuffer, obuffer);
        if (ret != 0)
        {
            printf("Failed to decode frame %d\n", ret);
            break;
        }

        /* s16le */
        int i;
        for (i = 0; i < samples; i++)
        {
            int ch;
            for (ch = 0; ch < channels; ch++)
            {
                fwrite(obuffer + datasize * i, 1, datasize, o_fp);
            }
        }

        ++frames;
    }

    Siren7_CloseDecoder(decoder);

return_fail:
    fclose(i_fp);
    fclose(o_fp);

    printf("Total frames %d\n", frames);

    return 0;
}
