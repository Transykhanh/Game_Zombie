#include "Sound.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define SAMPLE_RATE 22050
#define PI_VALUE 3.14159265358979323846

struct SoundNote {
    double frequency;
    int durationMs;
};

static int soundFilesReady = 0;
static char backgroundFile[MAX_PATH] = "";
static char gunFile[MAX_PATH] = "";
static char deathFile[MAX_PATH] = "";
static char pickupFile[MAX_PATH] = "";

static void writeU16(FILE *file, unsigned short value)
{
    fputc(value & 255, file);
    fputc((value >> 8) & 255, file);
}

static void writeU32(FILE *file, unsigned int value)
{
    fputc(value & 255, file);
    fputc((value >> 8) & 255, file);
    fputc((value >> 16) & 255, file);
    fputc((value >> 24) & 255, file);
}

static void writeWavHeader(FILE *file, int sampleCount)
{
    int dataSize = sampleCount * 2;

    fwrite("RIFF", 1, 4, file);
    writeU32(file, 36 + dataSize);
    fwrite("WAVE", 1, 4, file);

    fwrite("fmt ", 1, 4, file);
    writeU32(file, 16);
    writeU16(file, 1);
    writeU16(file, 1);
    writeU32(file, SAMPLE_RATE);
    writeU32(file, SAMPLE_RATE * 2);
    writeU16(file, 2);
    writeU16(file, 16);

    fwrite("data", 1, 4, file);
    writeU32(file, dataSize);
}

static int countSamples(const SoundNote notes[], int noteCount)
{
    int total = 0;
    int i;

    for(i = 0; i < noteCount; i++) {
        total += notes[i].durationMs * SAMPLE_RATE / 1000;
    }

    return total;
}

static short makeSoftSample(double phase, double volume, double envelope)
{
    double value = sin(phase) * 0.78 + sin(phase * 2.0) * 0.12;
    int sample = (int)(value * volume * envelope * 32767.0);

    if(sample > 32767) sample = 32767;
    if(sample < -32768) sample = -32768;

    return (short)sample;
}

static void createMelodyWav(const char *fileName, const SoundNote notes[], int noteCount, double volume)
{
    FILE *file = fopen(fileName, "wb");
    int totalSamples = countSamples(notes, noteCount);
    int i, j;

    if(file == NULL) {
        return;
    }

    writeWavHeader(file, totalSamples);

    for(i = 0; i < noteCount; i++) {
        int noteSamples = notes[i].durationMs * SAMPLE_RATE / 1000;
        double phase = 0.0;
        double step = 2.0 * PI_VALUE * notes[i].frequency / SAMPLE_RATE;

        for(j = 0; j < noteSamples; j++) {
            double envelope = 1.0;
            int fadeSamples = SAMPLE_RATE / 80;
            short sample = 0;

            if(j < fadeSamples) {
                envelope = (double)j / fadeSamples;
            }
            if(noteSamples - j < fadeSamples) {
                envelope = (double)(noteSamples - j) / fadeSamples;
            }

            if(notes[i].frequency > 0.0) {
                sample = makeSoftSample(phase, volume, envelope);
                phase += step;
            }

            writeU16(file, (unsigned short)sample);
        }
    }

    fclose(file);
}

static int fileExists(const char *fileName)
{
    return GetFileAttributesA(fileName) != INVALID_FILE_ATTRIBUTES;
}

static void buildSoundPath(char *target, const char *fileName)
{
    char exePath[MAX_PATH];
    char *lastSlash;

    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    lastSlash = strrchr(exePath, '\\');

    if(lastSlash != NULL) {
        *(lastSlash + 1) = '\0';
        lstrcpyA(target, exePath);
        lstrcatA(target, fileName);
    } else {
        lstrcpyA(target, fileName);
    }
}

static void createSoundFiles()
{
    SoundNote backgroundNotes[] = {
        {261.63, 420}, {329.63, 420}, {392.00, 520}, {329.63, 420},
        {293.66, 420}, {349.23, 520}, {392.00, 620}, {0.00, 180},
        {329.63, 420}, {392.00, 420}, {440.00, 520}, {392.00, 420},
        {349.23, 420}, {329.63, 520}, {293.66, 700}, {0.00, 220}
    };
    SoundNote gunNotes[] = {
        {1250.00, 24}, {760.00, 28}, {0.00, 18}, {410.00, 35}
    };
    SoundNote deathNotes[] = {
        {240.00, 100}, {190.00, 120}, {145.00, 160}, {95.00, 220}
    };
    SoundNote pickupNotes[] = {
        {523.25, 55}, {659.25, 60}, {783.99, 80}, {1046.50, 100}
    };

    if(soundFilesReady) {
        return;
    }

    buildSoundPath(backgroundFile, "sound_background.wav");
    buildSoundPath(gunFile, "sound_gun.wav");
    buildSoundPath(deathFile, "sound_death.wav");
    buildSoundPath(pickupFile, "sound_pickup.wav");

    if(!fileExists(backgroundFile)) {
        createMelodyWav(backgroundFile, backgroundNotes, sizeof(backgroundNotes) / sizeof(backgroundNotes[0]), 0.20);
    }
    if(!fileExists(gunFile)) {
        createMelodyWav(gunFile, gunNotes, sizeof(gunNotes) / sizeof(gunNotes[0]), 0.55);
    }
    if(!fileExists(deathFile)) {
        createMelodyWav(deathFile, deathNotes, sizeof(deathNotes) / sizeof(deathNotes[0]), 0.35);
    }
    if(!fileExists(pickupFile)) {
        createMelodyWav(pickupFile, pickupNotes, sizeof(pickupNotes) / sizeof(pickupNotes[0]), 0.32);
    }

    soundFilesReady = 1;
}

void startBackgroundMusic()
{
    char command[512];
    MCIERROR openError;
    MCIERROR playError;

    createSoundFiles();

    mciSendStringA("close bgm", NULL, 0, NULL);
    wsprintfA(command, "open \"%s\" type waveaudio alias bgm", backgroundFile);
    openError = mciSendStringA(command, NULL, 0, NULL);
    playError = mciSendStringA("play bgm from 0 repeat", NULL, 0, NULL);

    if(openError != 0 || playError != 0) {
        mciSendStringA("close bgm", NULL, 0, NULL);
        PlaySoundA(backgroundFile, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
    }
}

void stopBackgroundMusic()
{
    mciSendStringA("stop bgm", NULL, 0, NULL);
    mciSendStringA("close bgm", NULL, 0, NULL);
    PlaySoundA(NULL, NULL, 0);
}

void playGunSound()
{
    char command[512];

    createSoundFiles();
    mciSendStringA("close gunSound", NULL, 0, NULL);
    wsprintfA(command, "open \"%s\" type waveaudio alias gunSound", gunFile);
    mciSendStringA(command, NULL, 0, NULL);
    mciSendStringA("play gunSound from 0", NULL, 0, NULL);
}

void playZombieDeathSound()
{
    char command[512];

    createSoundFiles();
    mciSendStringA("close deathSound", NULL, 0, NULL);
    wsprintfA(command, "open \"%s\" type waveaudio alias deathSound", deathFile);
    mciSendStringA(command, NULL, 0, NULL);
    mciSendStringA("play deathSound from 0", NULL, 0, NULL);
}

void playPickupSound()
{
    char command[512];

    createSoundFiles();
    mciSendStringA("close pickupSound", NULL, 0, NULL);
    wsprintfA(command, "open \"%s\" type waveaudio alias pickupSound", pickupFile);
    mciSendStringA(command, NULL, 0, NULL);
    mciSendStringA("play pickupSound from 0", NULL, 0, NULL);
}
