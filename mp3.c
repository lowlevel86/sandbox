#include <stdio.h>
#include <stdlib.h>
#include <lame/lame.h>

void audio_to_mp3(short *audioData, int frameCnt, int sampRate, char *mp3_filename)
{
   #define buff_size 8192
   int i, read_size, write_size;
   unsigned char mp3_buffer[buff_size];
   
   // initialize LAME
   lame_t lame = lame_init();
   lame_set_in_samplerate(lame, sampRate);
   lame_set_num_channels(lame, 1); // mono
   lame_set_out_samplerate(lame, 22050); // mp3 sample rate
   lame_set_VBR(lame, vbr_default);
   lame_init_params(lame);
   
   FILE *mp3_file = fopen(mp3_filename, "wb");
   if (!mp3_file)
   {
      perror("Failed to open MP3 file");
      lame_close(lame);
      return;
   }

   for (i=0; i < frameCnt; i+=buff_size)
   {
      if (frameCnt - i < buff_size)
      read_size = frameCnt - i;
      else
      read_size = buff_size;

      write_size = lame_encode_buffer(lame, &audioData[i], NULL, read_size, mp3_buffer, buff_size);
      fwrite(mp3_buffer, sizeof(unsigned char), write_size, mp3_file);
   }

   // flush and write any remaining frames
   write_size = lame_encode_flush(lame, mp3_buffer, buff_size);
   fwrite(mp3_buffer, sizeof(unsigned char), write_size, mp3_file);
   
   fclose(mp3_file);
   lame_close(lame);
}
