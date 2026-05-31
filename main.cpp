#include <graphics.h>
#include <conio.h>
#include "Zombie.h"
#include "Player.h"
#include "HowToPlay.h"
#include "Sound.h"
#include <windows.h>

const int ZOMBIE_COUNT = 5;
const int ZOMBIE_HP_MAX = 3;
const int PLAYER_HP_MAX = 5;
const int PLAYER_GROUND_Y = 452;
const int JUMP_DURATION = 24;
const int JUMP_HEIGHT = 82;

static int initialZombieX[ZOMBIE_COUNT] = {485, 640, 795, 910, 1025};
static int spawnZombieX[ZOMBIE_COUNT] = {795, 875, 955, 1035, 1115};

static void drawGameState(int playerX, int playerGroundY, int zombieX[], int zombieHp[], int showZombieHp[],
                          int playerHp, int score, int kill, int elapsedSeconds)
{
    drawZombieSceneManyWithPlayerStats(playerX, playerGroundY, ZOMBIE_COUNT,
                                       zombieX, zombieHp, showZombieHp,
                                       playerHp, score, kill, elapsedSeconds);
}

static int getTargetZombie(int playerX, int zombieX[], int zombieHp[])
{
    int target = -1;
    int targetX = 10000;

    for(int i = 0; i < ZOMBIE_COUNT; i++) {
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

static int getZombieSpeedMode(int zombieStep)
{
    if(zombieStep <= 1) return 0;
    if(zombieStep >= 100) return 2;
    return 1;
}

static void applyZombieSpeedAction(int action, int &zombieStep)
{
    if(action == 1) zombieStep = 1;
    if(action == 2) zombieStep = 10;
    if(action == 3) zombieStep = 100;
}

static int getPlayerGroundY(int jumpTick)
{
    if(jumpTick <= 0) return PLAYER_GROUND_Y;

    int offset = 4 * JUMP_HEIGHT * jumpTick * (JUMP_DURATION - jumpTick) / (JUMP_DURATION * JUMP_DURATION);
    return PLAYER_GROUND_Y - offset;
}

static void spawnZombieFromRight(int index, int zombieX[], int zombieHp[],
                                 int showZombieHp[], int zombieRespawn[])
{
    zombieX[index] = spawnZombieX[index];
    zombieHp[index] = ZOMBIE_HP_MAX;
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

        delay(10);
    }

    return 1;
}

int main()
{
    initwindow(800, 600);
    startBackgroundMusic();

    int running = 1;
    int zombieStep = 10;
    while(running) {
        drawStartScreen();
        int startGame = 0;

        while(!startGame && running) {
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
                        delay(10);
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

                        delay(10);
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
        }

        if(!running) break;

        int playerX;
        int zombieX[ZOMBIE_COUNT];
        int zombieHp[ZOMBIE_COUNT];
        int showZombieHp[ZOMBIE_COUNT];
        int zombieRespawn[ZOMBIE_COUNT];
        double zombieMoveCarry;
        DWORD lastFrameTick;
        int playerHp;
        int score;
        int kills;
        int elapsedSeconds;
        int jumpTick;
        DWORD gameStartTick;

        resetGameState(playerX, zombieX, zombieHp, showZombieHp, zombieRespawn,
                       zombieMoveCarry, lastFrameTick, playerHp, score, kills, elapsedSeconds, gameStartTick,
                       jumpTick);
        drawGameState(playerX, getPlayerGroundY(jumpTick), zombieX, zombieHp, showZombieHp,
                      playerHp, score, kills, elapsedSeconds);

        int returnToMenu = 0;
        while(running) {
            int needRedraw = 0;
            int gameOver = 0;

            delay(20);
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

            zombieMoveCarry += (double)zombieStep * (double)frameDelta / 100.0;
            int zombiePixelsToMove = (int)zombieMoveCarry;
            if(zombiePixelsToMove > 0) {
                zombieMoveCarry -= zombiePixelsToMove;
                for(int i = 0; i < ZOMBIE_COUNT; i++) {
                    if(zombieHp[i] > 0) zombieX[i] -= zombiePixelsToMove;

                    if(zombieHp[i] <= 0 && zombieRespawn[i] > 0) zombieRespawn[i]--;
                    if(zombieHp[i] <= 0 && zombieRespawn[i] <= 0) {
                        spawnZombieFromRight(i, zombieX, zombieHp, showZombieHp, zombieRespawn);
                    }
                }

                needRedraw = 1;
            }

            for(int i = 0; i < ZOMBIE_COUNT; i++) {
                if(zombieReachedPlayer(zombieX[i], zombieHp[i])) {
                    playerHp--;
                    spawnZombieFromRight(i, zombieX, zombieHp, showZombieHp, zombieRespawn);
                    needRedraw = 1;
                }
            }

            if(playerHp <= 0) {
                playerHp = 0;
                gameOver = 1;
                needRedraw = 1;
            }

            if(needRedraw) {
                drawGameState(playerX, getPlayerGroundY(jumpTick), zombieX, zombieHp, showZombieHp,
                              playerHp, score, kills, elapsedSeconds);
            }

            if(gameOver) {
                playZombieDeathSound();
                returnToMenu = handleGameOverScreen(playerX, zombieX, zombieHp, showZombieHp,
                                                    zombieRespawn, zombieMoveCarry, lastFrameTick, playerHp,
                                                    score, kills, elapsedSeconds, gameStartTick,
                                                    running, jumpTick);
                if(returnToMenu) break;
                continue;
            }

            if(ismouseclick(WM_LBUTTONDOWN)) {
                int mouseX, mouseY;
                getmouseclick(WM_LBUTTONDOWN, mouseX, mouseY);
                if(getGameScreenAction(mouseX, mouseY) == 1) {
                    break;
                } else {
                    int target = getTargetZombie(playerX, zombieX, zombieHp);

                    if(target < 0) {
                        continue;
                    }

                    playGunSound();
                    drawShootEffectAt(playerX, zombieX[target]);

                    zombieHp[target]--;
                    showZombieHp[target] = 1;
                    if(zombieHp[target] <= 0) {
                        spawnZombieFromRight(target, zombieX, zombieHp, showZombieHp, zombieRespawn);
                        kills++;
                        score += 100;
                        playZombieDeathSound();
                    }

                    drawGameState(playerX, getPlayerGroundY(jumpTick), zombieX, zombieHp, showZombieHp,
                                  playerHp, score, kills, elapsedSeconds);
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
                }
                if(key == 'i' || key == 'I') {
                    playPickupSound();
                }
                if(key == 'a' || key == 'A') {
                    playerX -= 12;
                    if(playerX < 70) playerX = 70;
                    drawGameState(playerX, getPlayerGroundY(jumpTick), zombieX, zombieHp, showZombieHp,
                                  playerHp, score, kills, elapsedSeconds);
                }
                if(key == 'd' || key == 'D') {
                    playerX += 12;
                    if(playerX > 235) playerX = 235;
                    drawGameState(playerX, getPlayerGroundY(jumpTick), zombieX, zombieHp, showZombieHp,
                                  playerHp, score, kills, elapsedSeconds);
                }
                if((key == 'w' || key == 'W') && jumpTick == 0) {
                    jumpTick = 1;
                    drawGameState(playerX, getPlayerGroundY(jumpTick), zombieX, zombieHp, showZombieHp,
                                  playerHp, score, kills, elapsedSeconds);
                }
            }
        }
    }

    stopBackgroundMusic();
    closegraph();

    return 0;
}
