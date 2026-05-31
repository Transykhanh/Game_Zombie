#include "Sound.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
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

struct SoundBuffer {
    char *data;
    DWORD size;
};

static SoundBuffer gunBuffer = {NULL, 0};
static SoundBuffer deathBuffer = {NULL, 0};
static SoundBuffer pickupBuffer = {NULL, 0};

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
        {220.00, 520}, {0.00, 120}, {261.63, 520}, {0.00, 120},
        {293.66, 680}, {261.63, 520}, {0.00, 180},
        {196.00, 560}, {0.00, 120}, {246.94, 560}, {0.00, 120},
        {261.63, 720}, {246.94, 560}, {0.00, 220},
        {174.61, 620}, {0.00, 140}, {220.00, 620}, {246.94, 760},
        {220.00, 620}, {196.00, 820}, {0.00, 260}
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

    buildSoundPath(backgroundFile, "sound_background_light.wav");
    buildSoundPath(gunFile, "sound_gun.wav");
    buildSoundPath(deathFile, "sound_death.wav");
    buildSoundPath(pickupFile, "sound_pickup.wav");

    if(!fileExists(backgroundFile)) {
        createMelodyWav(backgroundFile, backgroundNotes, sizeof(backgroundNotes) / sizeof(backgroundNotes[0]), 0.10);
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

static void freeSoundBuffer(SoundBuffer *buffer)
{
    if(buffer->data != NULL) {
        free(buffer->data);
        buffer->data = NULL;
        buffer->size = 0;
    }
}

static void loadSoundBuffer(const char *fileName, SoundBuffer *buffer)
{
    HANDLE file;
    DWORD fileSize;
    DWORD bytesRead = 0;

    if(buffer->data != NULL) {
        return;
    }

    file = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(file == INVALID_HANDLE_VALUE) {
        return;
    }

    fileSize = GetFileSize(file, NULL);
    if(fileSize == INVALID_FILE_SIZE || fileSize == 0) {
        CloseHandle(file);
        return;
    }

    buffer->data = (char*)malloc(fileSize);
    if(buffer->data == NULL) {
        CloseHandle(file);
        return;
    }

    if(!ReadFile(file, buffer->data, fileSize, &bytesRead, NULL) || bytesRead != fileSize) {
        freeSoundBuffer(buffer);
    } else {
        buffer->size = fileSize;
    }

    CloseHandle(file);
}

static void loadSoundBuffers()
{
    createSoundFiles();
    loadSoundBuffer(gunFile, &gunBuffer);
    loadSoundBuffer(deathFile, &deathBuffer);
    loadSoundBuffer(pickupFile, &pickupBuffer);
}

static void playBufferedSound(const char *fileName, SoundBuffer *buffer)
{
    createSoundFiles();
    loadSoundBuffer(fileName, buffer);

    if(buffer->data != NULL) {
        PlaySoundA(buffer->data, NULL, SND_MEMORY | SND_ASYNC | SND_NODEFAULT);
    } else {
        PlaySoundA(fileName, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
    }
}

void startBackgroundMusic()
{
    char command[512];
    MCIERROR openError;
    MCIERROR playError;

    createSoundFiles();
    loadSoundBuffers();

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
    freeSoundBuffer(&gunBuffer);
    freeSoundBuffer(&deathBuffer);
    freeSoundBuffer(&pickupBuffer);
}

void playGunSound()
{
    playBufferedSound(gunFile, &gunBuffer);
}

void playZombieDeathSound()
{
    playBufferedSound(deathFile, &deathBuffer);
}

void playPickupSound()
{
    playBufferedSound(pickupFile, &pickupBuffer);
}
