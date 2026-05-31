#include <graphics.h>
#include <conio.h>
#include "Zombie.h"
#include "Player.h"
#include "HowToPlay.h"
#include "Sound.h"
#include <windows.h>

const int ZOMBIE_COUNT = 5;
const int ZOMBIE_HP_MAX = 3;
const int BOSS_HP_MAX = 9;
const int PLAYER_HP_MAX = 5;
const int PLAYER_GROUND_Y = 452;
const int JUMP_DURATION = 24;
const int JUMP_HEIGHT = 82;
const int TARGET_FRAME_MS = 16;
const int ZOMBIE_DEATH_FALL_FRAMES = 14;
const int WAVE_INTRO_FRAMES = 120;

static int initialZombieX[ZOMBIE_COUNT] = {485, 640, 795, 910, 1025};
static int spawnZombieX[ZOMBIE_COUNT] = {795, 875, 955, 1035, 1115};

static void drawGameState(int playerX, int playerGroundY, int zombieX[], int zombieHp[], int showZombieHp[],
                          int playerHp, int score, int kill, int elapsedSeconds)
{
    drawZombieSceneManyWithPlayerStats(playerX, playerGroundY, ZOMBIE_COUNT,
                                       zombieX, zombieHp, showZombieHp,
                                       playerHp, score, kill, elapsedSeconds);
}

static void drawGameStateWithEffects(int playerX, int playerGroundY, int activeZombieCount,
                                     int zombieX[], int zombieHp[],
                                     int showZombieHp[], int zombieHitEffect[], int zombieDeathEffect[],
                                     int playerHp, int score, int kill, int elapsedSeconds,
                                     int currentWave, int waveIntroFrames)
{
    drawZombieSceneManyWithWaveEffects(playerX, playerGroundY, activeZombieCount,
                                       zombieX, zombieHp, showZombieHp, zombieHitEffect, zombieDeathEffect,
                                       playerHp, score, kill, elapsedSeconds, currentWave, waveIntroFrames);
}

static int getTargetZombie(int playerX, int activeZombieCount, int zombieX[], int zombieHp[])
{
    int target = -1;
    int targetX = 10000;

    for(int i = 0; i < activeZombieCount; i++) {
        if(zombieHp[i] > 0 && zombieX[i] > playerX && zombieX[i] < targetX && zombieX[i] < 780) {
            target = i;
            targetX = zombieX[i];
        }
    }

    return target;
}

static int getElapsedSeconds(DWORD startTick)
{
    return (int)((GetTickCount() - startTick) / 1000);
}

static void pumpWindowsMessages()
{
    MSG message;

    while(PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

static void waitForFrameEnd(DWORD frameStartTick, int frameMs)
{
    while((int)(GetTickCount() - frameStartTick) < frameMs) {
        pumpWindowsMessages();
        Sleep(1);
    }
}

static int getZombieSpeedMode(int zombieStep)
{
    if(zombieStep <= 6) return 0;
    if(zombieStep >= 30) return 2;
    return 1;
}

static void applyZombieSpeedAction(int action, int &zombieStep)
{
    if(action == 0) zombieStep = 0;
    if(action == 1) zombieStep = 6;
    if(action == 2) zombieStep = 18;
    if(action == 3) zombieStep = 30;
}

static int getWaveForKills(int kills)
{
    if(kills >= 12) return 3;
    if(kills >= 5) return 2;
    return 1;
}

static int getWaveZombieCount(int wave)
{
    if(wave <= 1) return 3;
    if(wave == 2) return 4;
    return ZOMBIE_COUNT;
}

static int getWaveMoveStep(int baseStep, int wave)
{
    if(baseStep <= 0) return 0;
    return baseStep + (wave - 1) * 4;
}

static int getZombieMaxHpForWave(int index, int activeZombieCount, int wave)
{
    if(wave >= 3 && index == activeZombieCount - 1) {
        return BOSS_HP_MAX;
    }
    return ZOMBIE_HP_MAX;
}

static int getPlayerGroundY(int jumpTick)
{
    if(jumpTick <= 0) return PLAYER_GROUND_Y;

    int offset = 4 * JUMP_HEIGHT * jumpTick * (JUMP_DURATION - jumpTick) / (JUMP_DURATION * JUMP_DURATION);
    return PLAYER_GROUND_Y - offset;
}

static void spawnZombieFromRight(int index, int activeZombieCount, int currentWave,
                                 int zombieX[], int zombieHp[], int showZombieHp[], int zombieRespawn[])
{
    zombieX[index] = spawnZombieX[index];
    zombieHp[index] = getZombieMaxHpForWave(index, activeZombieCount, currentWave);
    showZombieHp[index] = 0;
    zombieRespawn[index] = 0;
}

static int zombieReachedPlayer(int zombieX, int zombieHp)
{
    return zombieHp > 0 && zombieX < 250;
}

static void resetZombies(int zombieX[], int zombieHp[], int showZombieHp[], int zombieRespawn[])
{
    for(int i = 0; i < ZOMBIE_COUNT; i++) {
        zombieX[i] = initialZombieX[i];
        zombieHp[i] = ZOMBIE_HP_MAX;
        showZombieHp[i] = 0;
        zombieRespawn[i] = 0;
    }
}

static void setupWaveZombies(int activeZombieCount, int currentWave, int zombieX[], int zombieHp[],
                             int showZombieHp[], int zombieRespawn[], int zombieHitEffect[],
                             int zombieDeathEffect[])
{
    for(int i = 0; i < activeZombieCount; i++) {
        int maxHp = getZombieMaxHpForWave(i, activeZombieCount, currentWave);

        if(zombieHp[i] <= 0 && zombieDeathEffect[i] <= 0) {
            zombieHp[i] = maxHp;
            zombieX[i] = spawnZombieX[i];
            showZombieHp[i] = 0;
            zombieRespawn[i] = 0;
            zombieHitEffect[i] = 0;
        } else if(zombieHp[i] > 0 && zombieHp[i] < maxHp) {
            zombieHp[i] = maxHp;
        }
    }
}

static void resetZombieEffects(int zombieHitEffect[], int zombieDeathEffect[])
{
    for(int i = 0; i < ZOMBIE_COUNT; i++) {
        zombieHitEffect[i] = 0;
        zombieDeathEffect[i] = 0;
    }
}

static void resetGameState(int &playerX, int zombieX[], int zombieHp[], int showZombieHp[],
                           int zombieRespawn[], double &zombieMoveCarry, DWORD &lastFrameTick, int &playerHp,
                           int &score, int &kills, int &elapsedSeconds, DWORD &gameStartTick,
                           int &jumpTick)
{
    playerX = 105;
    resetZombies(zombieX, zombieHp, showZombieHp, zombieRespawn);
    zombieMoveCarry = 0.0;
    lastFrameTick = GetTickCount();
    jumpTick = 0;
    playerHp = PLAYER_HP_MAX;
    score = 0;
    kills = 0;
    elapsedSeconds = 0;
    gameStartTick = GetTickCount();
}

static int handleGameOverScreen(int &playerX, int zombieX[], int zombieHp[], int showZombieHp[],
                                int zombieRespawn[], double &zombieMoveCarry, DWORD &lastFrameTick, int &playerHp,
                                int &score, int &kills, int &elapsedSeconds,
                                DWORD &gameStartTick, int &running, int &jumpTick)
{
    drawGameOverScreen(score, kills);

    while(running) {
        if(ismouseclick(WM_LBUTTONDOWN)) {
            int mouseX, mouseY;
            getmouseclick(WM_LBUTTONDOWN, mouseX, mouseY);
            int action = getGameOverAction(mouseX, mouseY);

            if(action == 1) {
                resetGameState(playerX, zombieX, zombieHp, showZombieHp, zombieRespawn,
                               zombieMoveCarry, lastFrameTick, playerHp, score, kills, elapsedSeconds, gameStartTick,
                               jumpTick);
                drawGameState(playerX, getPlayerGroundY(jumpTick), zombieX, zombieHp, showZombieHp,
                              playerHp, score, kills, elapsedSeconds);
                return 0;
            }
            if(action == 2) {
                return 1;
            }
        }

        if(kbhit()) {
            int key = getch();
            if(key == 13) {
                resetGameState(playerX, zombieX, zombieHp, showZombieHp, zombieRespawn,
                               zombieMoveCarry, lastFrameTick, playerHp, score, kills, elapsedSeconds, gameStartTick,
                               jumpTick);
                drawGameState(playerX, getPlayerGroundY(jumpTick), zombieX, zombieHp, showZombieHp,
                              playerHp, score, kills, elapsedSeconds);
                return 0;
            }
            if(key == 27) {
                return 1;
            }
        }

        waitForFrameEnd(GetTickCount(), 10);
    }

    return 1;
}

int main()
{
    initwindow(800, 600);
    startBackgroundMusic();

    int running = 1;
    int zombieStep = 6;
    while(running) {
        drawStartScreen();
        int startGame = 0;

        while(!startGame && running) {
            DWORD menuFrameStart = GetTickCount();
            pumpWindowsMessages();

            if(ismouseclick(WM_LBUTTONDOWN)) {
                int mouseX, mouseY;
                getmouseclick(WM_LBUTTONDOWN, mouseX, mouseY);

                int action = getStartMenuAction(mouseX, mouseY);
                if(action == 1) {
                    startGame = 1;
                }
                if(action == 2) {
                    drawHowToPlayScreen();
                    while(!kbhit() && !ismouseclick(WM_LBUTTONDOWN)) {
                        waitForFrameEnd(GetTickCount(), 10);
                    }
                    if(kbhit()) getch();
                    if(ismouseclick(WM_LBUTTONDOWN)) {
                        int tempX, tempY;
                        getmouseclick(WM_LBUTTONDOWN, tempX, tempY);
                    }
                    drawStartScreen();
                }
                if(action == 3) {
                    int inSettings = 1;
                    drawSettingsScreen(getZombieSpeedMode(zombieStep));
                    while(inSettings && running) {
                        DWORD settingsFrameStart = GetTickCount();
                        pumpWindowsMessages();

                        if(ismouseclick(WM_LBUTTONDOWN)) {
                            int settingsX, settingsY;
                            getmouseclick(WM_LBUTTONDOWN, settingsX, settingsY);
                            int settingsAction = getSettingsAction(settingsX, settingsY);
                            if(settingsAction >= 1 && settingsAction <= 3) {
                                applyZombieSpeedAction(settingsAction, zombieStep);
                                drawSettingsSpeedButtons(getZombieSpeedMode(zombieStep));
                            }
                            if(settingsAction == 4) {
                                inSettings = 0;
                            }
                        }

                        if(kbhit()) {
                            int settingsKey = getch();
                            if(settingsKey == '0') {
                                applyZombieSpeedAction(0, zombieStep);
                                drawSettingsSpeedButtons(getZombieSpeedMode(zombieStep));
                            }
                            if(settingsKey == '1') {
                                applyZombieSpeedAction(1, zombieStep);
                                drawSettingsSpeedButtons(getZombieSpeedMode(zombieStep));
                            }
                            if(settingsKey == '2') {
                                applyZombieSpeedAction(2, zombieStep);
                                drawSettingsSpeedButtons(getZombieSpeedMode(zombieStep));
                            }
                            if(settingsKey == '3') {
                                applyZombieSpeedAction(3, zombieStep);
                                drawSettingsSpeedButtons(getZombieSpeedMode(zombieStep));
                            }
                            if(settingsKey == 27 || settingsKey == 13) {
                                inSettings = 0;
                            }
                        }

                        waitForFrameEnd(settingsFrameStart, 10);
                    }
                    drawStartScreen();
                }
                if(action == 4) {
                    running = 0;
                }
            }

            if(kbhit()) {
                int key = getch();
                if(key == 13) startGame = 1;
                if(key == 27) running = 0;
            }

            waitForFrameEnd(menuFrameStart, 10);
        }

        if(!running) break;

        int playerX;
        int zombieX[ZOMBIE_COUNT];
        int zombieHp[ZOMBIE_COUNT];
        int showZombieHp[ZOMBIE_COUNT];
        int zombieHitEffect[ZOMBIE_COUNT];
        int zombieDeathEffect[ZOMBIE_COUNT];
        int zombieRespawn[ZOMBIE_COUNT];
        double zombieMoveCarry;
        DWORD lastFrameTick;
        int playerHp;
        int score;
        int kills;
        int elapsedSeconds;
        int jumpTick;
        DWORD gameStartTick;
        int currentWave = 1;
        int activeZombieCount = getWaveZombieCount(currentWave);
        int waveIntroFrames = WAVE_INTRO_FRAMES;

        resetGameState(playerX, zombieX, zombieHp, showZombieHp, zombieRespawn,
                       zombieMoveCarry, lastFrameTick, playerHp, score, kills, elapsedSeconds, gameStartTick,
                       jumpTick);
        resetZombieEffects(zombieHitEffect, zombieDeathEffect);
        setupWaveZombies(activeZombieCount, currentWave, zombieX, zombieHp, showZombieHp,
                         zombieRespawn, zombieHitEffect, zombieDeathEffect);
        drawGameStateWithEffects(playerX, getPlayerGroundY(jumpTick), activeZombieCount,
                                 zombieX, zombieHp, showZombieHp, zombieHitEffect, zombieDeathEffect,
                                 playerHp, score, kills, elapsedSeconds, currentWave, waveIntroFrames);

        int returnToMenu = 0;
        while(running) {
            DWORD frameStartTick = GetTickCount();
            int needRedraw = 0;
            int gameOver = 0;

            pumpWindowsMessages();
            DWORD nowTick = GetTickCount();
            DWORD frameDelta = nowTick - lastFrameTick;
            lastFrameTick = nowTick;
            if(frameDelta > 100) frameDelta = 100;

            int currentElapsedSeconds = getElapsedSeconds(gameStartTick);
            if(currentElapsedSeconds != elapsedSeconds) {
                elapsedSeconds = currentElapsedSeconds;
                needRedraw = 1;
            }

            if(jumpTick > 0) {
                jumpTick++;
                if(jumpTick >= JUMP_DURATION) {
                    jumpTick = 0;
                }
                needRedraw = 1;
            }

            if(waveIntroFrames > 0) {
                waveIntroFrames--;
                needRedraw = 1;
            }

            for(int i = 0; i < ZOMBIE_COUNT; i++) {
                if(zombieHitEffect[i] > 0) {
                    zombieHitEffect[i]--;
                    needRedraw = 1;
                }
                if(zombieDeathEffect[i] > 0) {
                    zombieDeathEffect[i]--;
                    if(zombieDeathEffect[i] <= 0) {
                        spawnZombieFromRight(i, activeZombieCount, currentWave, zombieX, zombieHp, showZombieHp, zombieRespawn);
                    }
                    needRedraw = 1;
                }
            }

            int effectiveZombieStep = getWaveMoveStep(zombieStep, currentWave);
            zombieMoveCarry += (double)effectiveZombieStep * (double)frameDelta / 100.0;
            int zombiePixelsToMove = (int)zombieMoveCarry;
            if(zombiePixelsToMove > 0) {
                zombieMoveCarry -= zombiePixelsToMove;
                for(int i = 0; i < activeZombieCount; i++) {
                    if(zombieHp[i] > 0) zombieX[i] -= zombiePixelsToMove;

                    if(zombieHp[i] <= 0 && zombieDeathEffect[i] <= 0 && zombieRespawn[i] > 0) zombieRespawn[i]--;
                    if(zombieHp[i] <= 0 && zombieDeathEffect[i] <= 0 && zombieRespawn[i] <= 0) {
                        spawnZombieFromRight(i, activeZombieCount, currentWave, zombieX, zombieHp, showZombieHp, zombieRespawn);
                        zombieHitEffect[i] = 0;
                    }
                }

                needRedraw = 1;
            }

            for(int i = 0; i < activeZombieCount; i++) {
                if(zombieReachedPlayer(zombieX[i], zombieHp[i])) {
                    playerHp--;
                    spawnZombieFromRight(i, activeZombieCount, currentWave, zombieX, zombieHp, showZombieHp, zombieRespawn);
                    zombieHitEffect[i] = 0;
                    zombieDeathEffect[i] = 0;
                    needRedraw = 1;
                }
            }

            if(playerHp <= 0) {
                playerHp = 0;
                gameOver = 1;
                needRedraw = 1;
            }

            if(gameOver) {
                if(needRedraw) {
                    drawGameStateWithEffects(playerX, getPlayerGroundY(jumpTick), activeZombieCount,
                                             zombieX, zombieHp, showZombieHp,
                                             zombieHitEffect, zombieDeathEffect,
                                             playerHp, score, kills, elapsedSeconds, currentWave, waveIntroFrames);
                }
                playZombieDeathSound();
                returnToMenu = handleGameOverScreen(playerX, zombieX, zombieHp, showZombieHp,
                                                    zombieRespawn, zombieMoveCarry, lastFrameTick, playerHp,
                                                    score, kills, elapsedSeconds, gameStartTick,
                                                    running, jumpTick);
                if(returnToMenu) break;
                resetZombieEffects(zombieHitEffect, zombieDeathEffect);
                currentWave = 1;
                activeZombieCount = getWaveZombieCount(currentWave);
                waveIntroFrames = WAVE_INTRO_FRAMES;
                continue;
            }

            if(ismouseclick(WM_LBUTTONDOWN)) {
                int mouseX, mouseY;
                getmouseclick(WM_LBUTTONDOWN, mouseX, mouseY);
                if(getGameScreenAction(mouseX, mouseY) == 1) {
                    break;
                } else {
                    int target = getTargetZombie(playerX, activeZombieCount, zombieX, zombieHp);

                    if(target >= 0) {
                        playGunSound();
                        drawShootEffectAt(playerX, zombieX[target]);

                        zombieHp[target]--;
                        showZombieHp[target] = 1;
                        zombieHitEffect[target] = 8;
                        if(zombieHp[target] <= 0) {
                            zombieHitEffect[target] = 0;
                            zombieDeathEffect[target] = ZOMBIE_DEATH_FALL_FRAMES;
                            showZombieHp[target] = 0;
                            kills++;
                            score += 100;
                            playZombieDeathSound();

                            int nextWave = getWaveForKills(kills);
                            if(nextWave > currentWave) {
                                currentWave = nextWave;
                                activeZombieCount = getWaveZombieCount(currentWave);
                                waveIntroFrames = WAVE_INTRO_FRAMES;
                                setupWaveZombies(activeZombieCount, currentWave, zombieX, zombieHp, showZombieHp,
                                                 zombieRespawn, zombieHitEffect, zombieDeathEffect);
                            }
                        }

                        needRedraw = 1;
                    }
                }
            }

            if(kbhit()) {
                int key = getch();
                if(key == 27) {
                    break;
                }
                if(key == 'g' || key == 'G') {
                    playZombieDeathSound();
                    returnToMenu = handleGameOverScreen(playerX, zombieX, zombieHp, showZombieHp,
                                                    zombieRespawn, zombieMoveCarry, lastFrameTick, playerHp,
                                                    score, kills, elapsedSeconds, gameStartTick,
                                                    running, jumpTick);
                    if(returnToMenu) break;
                    resetZombieEffects(zombieHitEffect, zombieDeathEffect);
                    currentWave = 1;
                    activeZombieCount = getWaveZombieCount(currentWave);
                    waveIntroFrames = WAVE_INTRO_FRAMES;
                }
                if(key == 'i' || key == 'I') {
                    playPickupSound();
                }
                if(key == '0') {
                    applyZombieSpeedAction(0, zombieStep);
                    zombieMoveCarry = 0.0;
                }
                if(key == '1') {
                    applyZombieSpeedAction(1, zombieStep);
                    zombieMoveCarry = 0.0;
                }
                if(key == '2') {
                    applyZombieSpeedAction(2, zombieStep);
                    zombieMoveCarry = 0.0;
                }
                if(key == '3') {
                    applyZombieSpeedAction(3, zombieStep);
                    zombieMoveCarry = 0.0;
                }
                if(key == 'a' || key == 'A') {
                    playerX -= 12;
                    if(playerX < 70) playerX = 70;
                    needRedraw = 1;
                }
                if(key == 'd' || key == 'D') {
                    playerX += 12;
                    if(playerX > 235) playerX = 235;
                    needRedraw = 1;
                }
                if((key == 'w' || key == 'W') && jumpTick == 0) {
                    jumpTick = 1;
                    needRedraw = 1;
                }
            }

            if(needRedraw) {
                drawGameStateWithEffects(playerX, getPlayerGroundY(jumpTick), activeZombieCount,
                                         zombieX, zombieHp, showZombieHp,
                                         zombieHitEffect, zombieDeathEffect,
                                         playerHp, score, kills, elapsedSeconds, currentWave, waveIntroFrames);
            }

            waitForFrameEnd(frameStartTick, TARGET_FRAME_MS);
        }
    }

    stopBackgroundMusic();
    closegraph();

    return 0;
}
