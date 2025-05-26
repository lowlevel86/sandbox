int iniAudioEngine();
void endAudioEngine();
void engineVolume(float);


int loadSound(char *);
void playSound(int);
void loopSound(int);

void soundVolume(int, float);
void panSound(int, float);

void stopSound(int);
void unloadSound(int);


void recMicStart();
void recMicStop();
void playSoundBuffer();
void saveBufferToMp3(char *);
