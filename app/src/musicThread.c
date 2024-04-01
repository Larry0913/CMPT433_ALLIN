#include <alsa/asoundlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <limits.h>
#include <alloca.h> // needed for mixer

#include "musicThread.h"
#include "threadController.h"
#include "trafficControl.h"


static snd_pcm_t *handle;

#define DEFAULT_VOLUME 100

#define SAMPLE_RATE 44100
#define NUM_CHANNELS 1
#define SAMPLE_SIZE (sizeof(short)) 			// bytes per sample
// Sample size note: This works for mono files because each sample ("frame') is 1 value.
// If using stereo files then a frame would be two samples.

static unsigned long playbackBufferSize = 0;
static short *playbackBuffer = NULL;


// Currently active (waiting to be played) sound bites
#define MAX_SOUND_BITES 30
typedef struct {
	// A pointer to a previously allocated sound bite (wavedata_t struct).
	// Note that many different sound-bite slots could share the same pointer
	// (overlapping cymbal crashes, for example)
	wavedata_t *pSound;

	// The offset into the pData of pSound. Indicates how much of the
	// sound has already been played (and hence where to start playing next).
	int location;
} playbackSound_t;
static playbackSound_t soundBites[MAX_SOUND_BITES];

static pthread_t playbackThreadId;
static pthread_mutex_t audioMutex = PTHREAD_MUTEX_INITIALIZER;

static int volume = 0;



// all sounds raw data
wavedata_t enterWave;
wavedata_t outWave;




void AudioMixer_init(void)
{
	AudioMixer_setVolume(DEFAULT_VOLUME);

	// Initialize the currently active sound-bites being played
	// REVISIT:- Implement this. Hint: set the pSound pointer to NULL for each
	//     sound bite.

	// read into the program
	AudioMixer_readWaveFileIntoMemory(ENTER_MUSIC, &enterWave);
	AudioMixer_readWaveFileIntoMemory(OUT_MUSIC, &outWave);

	// init soundBite
	for(int i = 0; i < MAX_SOUND_BITES; i++){
		soundBites[i].pSound = NULL;
		soundBites[i].location = 0;
	}

	// Open the PCM output
	int err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}


	// Configure parameters of PCM output
	err = snd_pcm_set_params(handle,
			SND_PCM_FORMAT_S16_LE,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			NUM_CHANNELS,
			SAMPLE_RATE,
			1,			// Allow software resampling
			50000);		// 0.05 seconds per buffer
	if (err < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Allocate this software's playback buffer to be the same size as the
	// the hardware's playback buffers for efficient data transfers.
	// ..get info on the hardware buffers:
 	unsigned long unusedBufferSize = 0;
	snd_pcm_get_params(handle, &unusedBufferSize, &playbackBufferSize);
	// ..allocate playback buffer:
	playbackBuffer = malloc(playbackBufferSize * sizeof(*playbackBuffer));

	// Launch playback thread:
	pthread_create(&playbackThreadId, NULL, playbackThread, NULL);
}


// Client code must call AudioMixer_freeWaveFileData to free dynamically allocated data.
void AudioMixer_readWaveFileIntoMemory(char *fileName, wavedata_t *pSound)
{
	assert(pSound);

	// The PCM data in a wave file starts after the header:
	const int PCM_DATA_OFFSET = 44;

	// Open the wave file
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		fprintf(stderr, "ERROR: Unable to open file %s.\n", fileName);
		exit(EXIT_FAILURE);
	}

	// Get file size
	fseek(file, 0, SEEK_END);
	int sizeInBytes = ftell(file) - PCM_DATA_OFFSET;
	pSound->numSamples = sizeInBytes / SAMPLE_SIZE;

	// Search to the start of the data in the file
	fseek(file, PCM_DATA_OFFSET, SEEK_SET);

	// Allocate space to hold all PCM data
	pSound->pData = malloc(sizeInBytes);
	if (pSound->pData == 0) {
		fprintf(stderr, "ERROR: Unable to allocate %d bytes for file %s.\n",
				sizeInBytes, fileName);
		exit(EXIT_FAILURE);
	}

	// Read PCM data from wave file into memory
	int samplesRead = fread(pSound->pData, SAMPLE_SIZE, pSound->numSamples, file);
	if (samplesRead != pSound->numSamples) {
		fprintf(stderr, "ERROR: Unable to read %d samples from file %s (read %d).\n",
				pSound->numSamples, fileName, samplesRead);
		exit(EXIT_FAILURE);
	}

	// Close file
	fclose(file);
}

void AudioMixer_freeWaveFileData(wavedata_t *pSound)
{
	pSound->numSamples = 0;
	free(pSound->pData);
	pSound->pData = NULL;
}

void AudioMixer_queueSound(wavedata_t *pSound)
{
	// Ensure we are only being asked to play "good" sounds:
	if (running_flag)
	{
		//assert(pSound->numSamples > 0);
		//assert(pSound->pData);
	}

	pthread_mutex_lock(&audioMutex); // Lock for thread safety
    // Search for an empty sound bite slot:
    int i;
    for (i = 0; i < MAX_SOUND_BITES; i++) {
        if (soundBites[i].pSound == NULL) { // Check if the slot is unused
            soundBites[i].pSound = pSound; // Place the sound in the slot
            soundBites[i].location = 0; // Set starting location to 0
            break; // Exit the loop once a slot is filled
        }
    }

    if (i == MAX_SOUND_BITES) { // No free slot found
        fprintf(stderr, "ERROR: No free slots to queue sound.\n");
    }

	pthread_mutex_unlock(&audioMutex);
}

void AudioMixer_cleanup(void)
{
	// printf("Stopping audio...\n");

	pthread_join(playbackThreadId, NULL);

	// Shutdown the PCM output, allowing any pending sound to play out (drain)
	snd_pcm_drain(handle);
	snd_pcm_close(handle);

	// Free playback buffer
	// (note that any wave files read into wavedata_t records must be freed
	//  in addition to this by calling AudioMixer_freeWaveFileData() on that struct.)
	free(playbackBuffer);
	playbackBuffer = NULL;

	AudioMixer_freeWaveFileData(&enterWave);
	AudioMixer_freeWaveFileData(&outWave);

	printf("Done stopping audio...\n");
	fflush(stdout);
}


int AudioMixer_getVolume()
{
	return volume;
}

// Function copied from:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
// Written by user "trenki".
void AudioMixer_setVolume(int newVolume)
{
	// Ensure volume is reasonable; If so, cache it for later getVolume() calls.
	if (newVolume < 0 || newVolume > AUDIOMIXER_MAX_VOLUME) {
		printf("ERROR: Volume must be between 0 and 100.\n");
		return;
	}
	volume = newVolume;

    long min, max;
    snd_mixer_t *volHandle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "PCM";

    snd_mixer_open(&volHandle, 0);
    snd_mixer_attach(volHandle, card);
    snd_mixer_selem_register(volHandle, NULL, NULL);
    snd_mixer_load(volHandle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(volHandle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

    snd_mixer_close(volHandle);
}


// Fill the `buff` array with new PCM values to output.
//    `buff`: buffer to fill with new PCM data from sound bites.
//    `size`: the number of values to store into playbackBuffer
static void fillPlaybackBuffer(short *buff, int size)
{
	memset(buff, 0, size * SAMPLE_SIZE);

    pthread_mutex_lock(&audioMutex); // Lock for thread safety

    // 3. Loop through each slot in soundBites[]
    for (int i = 0; i < MAX_SOUND_BITES; i++) {

        if (soundBites[i].pSound != NULL) { // Check if sound bite slot is used
            int soundDataLeft = soundBites[i].pSound->numSamples - soundBites[i].location;

            // Calculate how many samples to mix into the buffer
            int samplesToMix = (soundDataLeft < size) ? soundDataLeft : size;

            for (int j = 0; j < samplesToMix; j++) {
                // Add sound bite's data to the playback buffer
                int bufferIndex = j;
                int soundIndex = soundBites[i].location + j;
                short sampleToAdd = soundBites[i].pSound->pData[soundIndex];
                
                // Calculate new sample value and prevent overflow/underflow
                int newSample = buff[bufferIndex] + sampleToAdd;
                if (newSample > SHRT_MAX) newSample = SHRT_MAX;
                else if (newSample < SHRT_MIN) newSample = SHRT_MIN;
                
                buff[bufferIndex] = (short)newSample;
            }

            // Update the sound bite's location for next playback
            soundBites[i].location += samplesToMix;

            // If the sound bite has been fully played, free the slot
            if (soundBites[i].location >= soundBites[i].pSound->numSamples) {
                soundBites[i].pSound = NULL;
                soundBites[i].location = 0;
            }
        }
		
    }

    pthread_mutex_unlock(&audioMutex); // Unlock after modifications

}

// void AudioMixer_clearQueue() {
//     pthread_mutex_lock(&audioMutex);
//     for (int i = 0; i < MAX_SOUND_BITES; i++) {
//         soundBites[i].pSound = NULL;
//         soundBites[i].location = 0;
//     }
//     pthread_mutex_unlock(&audioMutex);
// }


void* playbackThread(void *_arg){

	(void)_arg; // address -Werror

	while (running_flag) {
		// Generate next block of audio
		fillPlaybackBuffer(playbackBuffer, playbackBufferSize);
		// Output the audio
		snd_pcm_sframes_t frames = snd_pcm_writei(handle,
				playbackBuffer, playbackBufferSize);

		// Check for (and handle) possible error conditions on output
		if (frames < 0) {
			fprintf(stderr, "AudioMixer: writei() returned %li\n", frames);
			frames = snd_pcm_recover(handle, frames, 1);
		}
		if (frames < 0) {
			fprintf(stderr, "ERROR: Failed writing audio with snd_pcm_writei(): %li\n",
					frames);
			exit(EXIT_FAILURE);
		}
		if (frames > 0 && frames < (snd_pcm_sframes_t)playbackBufferSize) {
			printf("Short write (expected %li, wrote %li)\n",
					playbackBufferSize, frames);
		}
	}

	return NULL;
}