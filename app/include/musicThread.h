#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

// Store data of a single wave file read into memory.
// Space is dynamically allocated; must be freed correctly!

#define AUDIOMIXER_MAX_VOLUME 100

#define ENTER_MUSIC "wave-files/enter.wav"
#define OUT_MUSIC "wave-files/out.wav"


typedef struct {
	int numSamples;
	short *pData;
} wavedata_t;

extern wavedata_t enterWave;
extern wavedata_t outWave;

// init() must be called before any other functions,
// cleanup() must be called last to stop playback threads and free memory.
void AudioMixer_init(void);
void AudioMixer_cleanup(void);

// Read the contents of a wave file into the pSound structure. Note that
// the pData pointer in this structure will be dynamically allocated in
// readWaveFileIntoMemory(), and is freed by calling freeWaveFileData().
void AudioMixer_readWaveFileIntoMemory(char *fileName, wavedata_t *pSound);
void AudioMixer_freeWaveFileData(wavedata_t *pSound);

// Queue up another sound bite to play as soon as possible.
void AudioMixer_queueSound(wavedata_t *pSound);

// Get/set the volume.
// setVolume() function posted by StackOverflow user "trenki" at:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
int  AudioMixer_getVolume(void);
void AudioMixer_setVolume(int newVolume);

// void AudioMixer_clearQueue(void);

void* playbackThread(void*);

#endif
