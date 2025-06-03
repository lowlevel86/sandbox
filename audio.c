#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "mp3.h"

int iniCapturePlayback();
void endCapturePlayback();

#define TRUE 1
#define FALSE 0
#define NO_SOUND_ID -1
#define MAX_SND_OBJS 1024

int noAudio = FALSE;
ma_result result;
ma_engine engine;
ma_sound ma_snd[MAX_SND_OBJS];
char ma_snd_occupied[MAX_SND_OBJS] = {FALSE};

int iniAudioEngine()
{
   ma_engine_config config = ma_engine_config_init();

   config.periodSizeInFrames = 1024 * 4;
   config.sampleRate = 22050;
   config.channels = 2;
   
   result = ma_engine_init(&config, &engine);
   if (result != MA_SUCCESS)
   {
      printf("Failed to initialize audio engine.");
      noAudio = TRUE;
      return -1;
   }
   
   iniCapturePlayback();
   
   return 0;
}

void endAudioEngine()
{
   if (noAudio)
   return;
   
   int i;
   for (i=0; i < MAX_SND_OBJS; i++)
   if (ma_snd_occupied[i])
   {
      ma_sound_stop(&ma_snd[i]);
      ma_sound_uninit(&ma_snd[i]);
      ma_snd_occupied[i] = FALSE;
   }
   
   endCapturePlayback();
   ma_engine_uninit(&engine);
}

void engineVolume(float vol)
{
   if (noAudio)
   return;
   
   ma_engine_set_volume(&engine, vol);
}

int loadSound(char *filename)
{
   if (noAudio)
   return NO_SOUND_ID;
   
   int i;
   for (i=0; i < MAX_SND_OBJS; i++)
   if (ma_snd_occupied[i] == FALSE)
   break;
   
   result = ma_sound_init_from_file(&engine, filename, 0, NULL, NULL, &ma_snd[i]);
   if (result != MA_SUCCESS)
   return NO_SOUND_ID;
      
   ma_snd_occupied[i] = TRUE;
   return i;
}

void playSound(int id)
{
   if (noAudio)
   return;
   
   if ((id < 0) || (id >= MAX_SND_OBJS))
   return;
   
   if (ma_snd_occupied[id] == FALSE)
   return;
   
   //ma_sound_seek_to_pcm_frame(&ma_snd[id], 0);
   ma_sound_start(&ma_snd[id]);
}

void soundVolume(int id, float vol)
{
   if (noAudio)
   return;
   
   if ((id < 0) || (id >= MAX_SND_OBJS))
   return;
   
   if (ma_snd_occupied[id] == FALSE)
   return;
   
   ma_sound_set_volume(&ma_snd[id], vol);
}

void loopSound(int id)
{
   if (noAudio)
   return;
   
   if ((id < 0) || (id >= MAX_SND_OBJS))
   return;
   
   if (ma_snd_occupied[id] == FALSE)
   return;
   
   ma_sound_set_looping(&ma_snd[id], MA_TRUE);
}

void stopSound(int id)
{
   if (noAudio)
   return;
   
   if ((id < 0) || (id >= MAX_SND_OBJS))
   return;
   
   if (ma_snd_occupied[id] == FALSE)
   return;
   
   ma_sound_stop(&ma_snd[id]);
}

void unloadSound(int id)
{
   if (noAudio)
   return;
   
   if ((id < 0) || (id >= MAX_SND_OBJS))
   return;
   
   if (ma_snd_occupied[id] == FALSE)
   return;
   
   ma_sound_stop(&ma_snd[id]);
   ma_sound_uninit(&ma_snd[id]);
   
   ma_snd_occupied[id] = FALSE;
}


///////////////////////////////////////////////////////////////////////////////
//
//////////////////////// record and playback functions ////////////////////////

#define CAPTURE_DURATION 10 // in seconds
short *capture_buffer = 0;
int duration_frame_count;
int bytes_per_frame;
int frame_count;

int capture_frame_count;
ma_device rec_dev;
ma_device play_dev;

void rec_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 framesRead)
{
   if (frame_count+framesRead >= duration_frame_count)
   return;
   
   memcpy(&capture_buffer[frame_count], pInput, framesRead * bytes_per_frame);
   frame_count += framesRead;
}

void play_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 framesRead)
{
   if (frame_count+framesRead >= duration_frame_count)
   return;
   
   memcpy(pOutput, &capture_buffer[frame_count], framesRead * bytes_per_frame); // copy audio data to output
   frame_count += framesRead;
}

int iniCapturePlayback()
{
   ma_result result;
   ma_device_config rec_dev_config;
   ma_device_config play_dev_config;
   
   rec_dev_config = ma_device_config_init(ma_device_type_capture);
   rec_dev_config.capture.format = ma_format_s16;
   rec_dev_config.capture.channels = 1; // mono
   rec_dev_config.dataCallback = rec_callback;

   result = ma_device_init(NULL, &rec_dev_config, &rec_dev);
   if (result != MA_SUCCESS)
   {
      printf("Capture device error\n");
      return 1;
   }

   play_dev_config = ma_device_config_init(ma_device_type_playback);
   play_dev_config.playback.format = ma_format_s16;
   play_dev_config.playback.channels = 1; // mono
   play_dev_config.sampleRate = rec_dev.sampleRate; // sample rate
   play_dev_config.dataCallback = play_callback;

   result = ma_device_init(NULL, &play_dev_config, &play_dev);
   if (result != MA_SUCCESS)
   {
      printf("Playback device error\n");
      return 1;
   }
   
   bytes_per_frame = ma_get_bytes_per_frame(rec_dev.capture.format, rec_dev.capture.channels);
   duration_frame_count = rec_dev.sampleRate * CAPTURE_DURATION;
   capture_buffer = malloc(duration_frame_count * sizeof(short));
   
   return 0;
}

void endCapturePlayback()
{
   ma_device_uninit(&play_dev);
   ma_device_uninit(&rec_dev);
   
   if (capture_buffer)
   free(capture_buffer);
   
   capture_buffer = 0;
}

void recMicStop()
{
   capture_frame_count = frame_count;
   ma_device_stop(&rec_dev);
   ma_device_stop(&play_dev);
}

void recMicStart()
{
   recMicStop();
   frame_count = 0; // reset
   ma_device_start(&rec_dev);
}

void playSoundBuffer()
{
   recMicStop();
   frame_count = 0; // reset
   ma_device_start(&play_dev);
}

void saveBufferToMp3(char *filename)
{
   ma_device_stop(&rec_dev);
   ma_device_stop(&play_dev);
   
   if (capture_buffer)
   audio_to_mp3(capture_buffer, capture_frame_count, rec_dev.sampleRate, filename);
}
