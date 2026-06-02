#ifndef ZOMBIE_H
#define ZOMBIE_H

void drawZombieScene();
void drawZombieSceneAt(int playerX, int zombie1X, int zombie2X, int zombie3X);
void drawZombieSceneWithHealth(int playerX, int zombie1X, int zombie2X, int zombie3X,
                               int zombie1Hp, int zombie2Hp, int zombie3Hp,
                               int showZombie1Hp, int showZombie2Hp, int showZombie3Hp);
void drawZombieSceneWithStats(int playerX, int zombie1X, int zombie2X, int zombie3X,
                              int zombie1Hp, int zombie2Hp, int zombie3Hp,
                              int showZombie1Hp, int showZombie2Hp, int showZombie3Hp,
                              int playerHp, int score, int kill, int elapsedSeconds);
void drawZombieSceneManyWithStats(int playerX, int zombieCount,
                                  int zombieX[], int zombieHp[], int showZombieHp[],
                                  int playerHp, int score, int kill, int elapsedSeconds);
void drawZombieSceneManyWithPlayerStats(int playerX, int playerGroundY, int zombieCount,
                                        int zombieX[], int zombieHp[], int showZombieHp[],
                                        int playerHp, int score, int kill, int elapsedSeconds);
void drawZombieSceneManyWithEffects(int playerX, int playerGroundY, int zombieCount,
                                    int zombieX[], int zombieHp[], int showZombieHp[],
                                    int zombieHitEffect[], int zombieDeathEffect[],
                                    int playerHp, int score, int kill, int elapsedSeconds);
void drawZombieSceneManyWithWaveEffects(int playerX, int playerGroundY, int zombieCount,
                                        int zombieX[], int zombieHp[], int showZombieHp[],
                                        int zombieHitEffect[], int zombieDeathEffect[],
                                        int playerHp, int score, int kill, int elapsedSeconds,
                                        int currentWave, int waveIntroFrames);
void drawStartScreen();
void drawSettingsScreen(int zombieSpeedMode);
void drawSettingsSpeedButtons(int zombieSpeedMode);
void drawGameOverScreen(int score, int zombiesKilled);
void drawPauseMenuScreen();
int getStartMenuAction(int mouseX, int mouseY);
int getSettingsAction(int mouseX, int mouseY);
int getGameScreenAction(int mouseX, int mouseY);
int getGameOverAction(int mouseX, int mouseY);
int getPauseMenuAction(int mouseX, int mouseY);
void drawShootEffect(int targetX, int targetY);
void drawShootEffectAt(int playerX, int zombieX);
void drawZombie(int x, int groundY);
void drawTwinklingStars();

#endif
