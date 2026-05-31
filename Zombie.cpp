#include "Zombie.h"
#include "Player.h"
#include "ThuatToan.h"
#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct StarPoint {
    int x;
    int y;
    int size;
    int phase;
};

static StarPoint stars[24];
static int starsReady = 0;

static void initRandomStars()
{
    int i;

    if(starsReady) {
        return;
    }

    srand((unsigned int)time(NULL));
    for(i = 0; i < 24; i++) {
        stars[i].x = 45 + rand() % 700;
        stars[i].y = 70 + rand() % 125;
        stars[i].size = 1 + rand() % 3;
        stars[i].phase = rand() % 7;

        if(stars[i].x > 645 && stars[i].x < 760 && stars[i].y < 150) {
            stars[i].x -= 150;
        }
    }

    starsReady = 1;
}

static void eraseStar(int x, int y)
{
    drawFilledCircle(x, y, 5, BLACK, BLACK);
}

static void drawStarShape(int x, int y, int size, int brightness)
{
    int color = DARKGRAY;

    if(brightness == 1) color = LIGHTGRAY;
    if(brightness == 2) color = WHITE;

    if(brightness == 0) {
        putpixel(x, y, color);
        return;
    }

    drawFilledCircle(x, y, size, color, color);

    if(brightness == 2) {
        drawLineBresenham(x - size - 3, y, x + size + 3, y, color);
        drawLineBresenham(x, y - size - 3, x, y + size + 3, color);
    }
}

void drawTwinklingStars()
{
    int starCount = sizeof(stars) / sizeof(stars[0]);
    int tick = (int)(clock() / (CLOCKS_PER_SEC / 6));
    int i;

    initRandomStars();

    for(i = 0; i < starCount; i++) {
        int brightness = (tick + stars[i].phase) % 4;

        eraseStar(stars[i].x, stars[i].y);
        if(brightness == 3) {
            brightness = 1;
        }
        drawStarShape(stars[i].x, stars[i].y, stars[i].size, brightness);
    }
}

static void drawGround() {
    drawFilledRect(26, 445, 774, 495, LIGHTGRAY, DARKGRAY);

    drawLineBresenham(45, 468, 210, 468, LIGHTGRAY);
    drawLineBresenham(250, 452, 250, 488, LIGHTGRAY);
    drawLineBresenham(300, 458, 345, 493, LIGHTGRAY);
    drawLineBresenham(410, 490, 460, 452, LIGHTGRAY);
    drawLineBresenham(520, 458, 700, 487, LIGHTGRAY);
    drawLineBresenham(610, 492, 640, 448, LIGHTGRAY);

    setAlgorithmColor(BLACK);
    BLineHorizontal(50, 460, 145);
    BLineVertical(190, 452, 488);
    BLine45(225, 452, 260, 487);
    BLineMinus45(305, 488, 340, 453);
    BLine1(380, 458, 500, 488);
    BLine2(540, 448, 562, 492);
    BLine3(610, 485, 735, 458);
    BLine4(700, 492, 728, 448);
}

static void drawBrokenBuilding(int left, int bottom, int width, int height, int color) {
    int right = left + width;
    int top = bottom - height;

    drawFilledRect(left, top, right, bottom, BLACK, color);
    drawLineBresenham(left, top, left + width / 5, top - 18, LIGHTGRAY);
    drawLineBresenham(left + width / 5, top - 18, left + width / 2, top - 5, LIGHTGRAY);
    drawLineBresenham(left + width / 2, top - 5, right - width / 6, top - 28, LIGHTGRAY);
    drawLineBresenham(right - width / 6, top - 28, right, top, LIGHTGRAY);

    for(int y = top + 24; y < bottom - 18; y += 34) {
        for(int x = left + 14; x < right - 16; x += 30) {
            drawFilledRect(x, y, x + 13, y + 16, BLACK, DARKGRAY);
            if((x + y) % 3 == 0) {
                drawLineBresenham(x, y, x + 13, y + 16, RED);
                drawLineBresenham(x + 13, y, x, y + 16, RED);
            } else if((x + y) % 4 == 0) {
                drawFilledRect(x + 3, y + 4, x + 10, y + 12, YELLOW, YELLOW);
            }
        }
    }
}

static void drawFence(int left, int y, int count) {
    for(int i = 0; i < count; i++) {
        int x = left + i * 28;
        drawFilledRect(x, y - 42, x + 8, y, BROWN, BROWN);
        drawLineBresenham(x, y - 42, x + 4, y - 50, BROWN);
        drawLineBresenham(x + 8, y - 42, x + 4, y - 50, BROWN);
    }

    drawLineBresenham(left - 8, y - 30, left + count * 28, y - 30, BROWN);
    drawLineBresenham(left - 8, y - 14, left + count * 28, y - 14, BROWN);
}

static void drawWoodCrate(int left, int bottom) {
    drawFilledRect(left, bottom - 34, left + 42, bottom, BROWN, BROWN);
    drawLineBresenham(left, bottom - 34, left + 42, bottom, BLACK);
    drawLineBresenham(left + 42, bottom - 34, left, bottom, BLACK);
    drawLineBresenham(left + 21, bottom - 34, left + 21, bottom, BLACK);
}

static void drawBrokenCar(int x, int y) {
    int bodyX[] = {x, x + 28, x + 118, x + 148, x + 138, x + 12};
    int bodyY[] = {y, y - 22, y - 22, y, y + 22, y + 22};
    drawFilledPolygon(bodyX, bodyY, 6, x + 70, y + 5, LIGHTGRAY, DARKGRAY);

    int roofX[] = {x + 38, x + 62, x + 100, x + 118};
    int roofY[] = {y - 22, y - 52, y - 52, y - 22};
    drawFilledPolygon(roofX, roofY, 4, x + 78, y - 32, LIGHTGRAY, BLUE);

    drawFilledCircle(x + 34, y + 22, 13, BLACK, DARKGRAY);
    drawFilledCircle(x + 112, y + 22, 13, BLACK, DARKGRAY);
    drawLineBresenham(x + 12, y + 6, x + 50, y - 12, RED);
    drawLineBresenham(x + 122, y - 2, x + 145, y - 25, LIGHTGRAY);
}

static void drawCurvedLeaf(int baseX, int baseY, int tipX, int tipY, int color) {
    setAlgorithmColor(color);
    D1(3, baseX, baseY, tipX, tipY);
    D1(3, tipX, tipY, baseX + 3, baseY + 4);
    drawLineBresenham(baseX, baseY, tipX, tipY, GREEN);
}

static void drawLeafCluster(int x, int y) {
    drawCurvedLeaf(x, y, x - 20, y - 12, GREEN);
    drawCurvedLeaf(x, y, x + 20, y - 14, LIGHTGREEN);
    drawCurvedLeaf(x, y + 5, x - 16, y + 12, GREEN);
    drawCurvedLeaf(x, y + 5, x + 17, y + 10, LIGHTGREEN);
    drawCurvedLeaf(x - 4, y - 4, x, y - 24, GREEN);
}

static void drawKochCloud(int x, int y, int length, int color) {
    setAlgorithmColor(color);
    setFractalStart(x, y);
    K(3, (float)length, 0);
    setFractalStart(x + length / 3, y - 12);
    K(2, (float)(length / 2), 0);
    setFractalStart(x + length * 2 / 3, y - 8);
    K(2, (float)(length / 3), 0);
}

static void drawBackground() {
    drawFilledRect(20, 58, 780, 510, BLACK, BLACK);
    drawFilledCircle(705, 105, 34, RED, RED);
    drawTwinklingStars();
    drawKochCloud(330, 165, 120, LIGHTGRAY);
    drawKochCloud(520, 178, 95, DARKGRAY);

    drawBrokenBuilding(42, 445, 90, 235, DARKGRAY);
    drawBrokenBuilding(150, 445, 120, 285, LIGHTGRAY);
    drawBrokenBuilding(590, 445, 88, 210, DARKGRAY);
    drawBrokenBuilding(690, 445, 70, 250, LIGHTGRAY);

    drawFence(292, 445, 7);
    drawBrokenCar(345, 420);
    drawWoodCrate(535, 445);
    drawWoodCrate(570, 445);

    drawFractalTree(75, 445, 72, 90, 6, BROWN);
    drawFractalTree(735, 445, 64, 100, 5, BROWN);
    drawFractalTree(280, 445, 42, 85, 4, BROWN);
    drawLeafCluster(75, 285);
    drawLeafCluster(50, 322);
    drawLeafCluster(105, 326);
    drawLeafCluster(735, 304);
    drawLeafCluster(710, 333);
    drawLeafCluster(760, 340);
    drawLeafCluster(280, 356);
}

static void drawStartZombieIcon(int x, int y) {
    drawFilledCircle(x, y, 24, GREEN, LIGHTGREEN);
    drawFilledCircle(x - 9, y - 5, 4, BLACK, RED);
    drawFilledCircle(x + 9, y - 5, 4, BLACK, RED);
    drawLineBresenham(x - 12, y + 11, x + 12, y + 11, BLACK);
    drawLineBresenham(x - 6, y + 11, x - 6, y + 18, WHITE);
    drawLineBresenham(x + 6, y + 11, x + 6, y + 18, WHITE);
}

static void drawDashedHorizontal(int x1, int y, int x2, int color) {
    for(int x = x1; x <= x2; x += 16) {
        drawLineBresenham(x, y, x + 8, y, color);
    }
}

static void drawDashedVertical(int x, int y1, int y2, int color) {
    for(int y = y1; y <= y2; y += 16) {
        drawLineBresenham(x, y, x, y + 8, color);
    }
}

static void drawMenuButton(int left, int top, int right, int bottom, char text[], int active) {
    int borderColor = active ? LIGHTGREEN : LIGHTGRAY;
    int textColor = active ? LIGHTGREEN : WHITE;

    drawLineBresenham(left, top, right, top, borderColor);
    drawLineBresenham(right, top, right, bottom, borderColor);
    drawLineBresenham(right, bottom, left, bottom, borderColor);
    drawLineBresenham(left, bottom, left, top, borderColor);

    setcolor(textColor);
    outtextxy(left + 18, top + 9, text);
}

void drawStartScreen() {
    setbkcolor(BLACK);
    cleardevice();

    drawFilledRect(0, 0, 799, 599, BLACK, BLACK);
    drawFilledCircle(660, 95, 38, RED, RED);
    drawFilledCircle(90, 75, 2, WHITE, WHITE);
    drawFilledCircle(190, 120, 2, LIGHTGRAY, LIGHTGRAY);
    drawFilledCircle(310, 90, 2, WHITE, WHITE);
    drawFilledCircle(500, 70, 2, LIGHTGRAY, LIGHTGRAY);
    drawLineBresenham(410, 115, 416, 115, WHITE);
    drawLineBresenham(413, 112, 413, 118, WHITE);
    drawKochCloud(135, 160, 130, LIGHTGRAY);
    drawKochCloud(455, 150, 110, DARKGRAY);
    drawFractalTree(80, 515, 80, 90, 6, BROWN);
    drawFractalTree(720, 515, 70, 100, 5, BROWN);
    drawLeafCluster(80, 348);
    drawLeafCluster(52, 390);
    drawLeafCluster(112, 392);
    drawLeafCluster(720, 365);
    drawLeafCluster(690, 400);
    drawLeafCluster(752, 405);
    drawFilledRect(0, 515, 799, 599, LIGHTGRAY, DARKGRAY);

    drawFilledRect(150, 85, 650, 430, LIGHTGRAY, BLACK);
    drawDashedHorizontal(170, 105, 630, LIGHTGREEN);
    drawDashedHorizontal(170, 410, 630, LIGHTGREEN);
    drawDashedVertical(170, 105, 410, LIGHTGREEN);
    drawDashedVertical(630, 105, 410, LIGHTGREEN);

    drawStartZombieIcon(220, 170);
    drawStartZombieIcon(580, 170);

    char title[] = "ZOMBIE SURVIVAL";
    char start[] = "[ START GAME ]";
    char how[] = "[ HOW TO PLAY ]";
    char settings[] = "[ SETTINGS ]";
    char exitText[] = "[ EXIT ]";
    char background[] = "Background zombie city";
    char hint[] = "Click START GAME to play";

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    setcolor(LIGHTGREEN);
    outtextxy(286, 135, title);

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    drawMenuButton(315, 205, 485, 235, start, 1);
    drawMenuButton(315, 245, 485, 275, how, 0);
    drawMenuButton(315, 285, 485, 315, settings, 0);
    drawMenuButton(315, 325, 485, 355, exitText, 0);

    setcolor(WHITE);
    outtextxy(305, 380, background);
    setcolor(LIGHTGRAY);
    outtextxy(300, 455, hint);
}

int getStartMenuAction(int mouseX, int mouseY) {
    if(mouseX >= 315 && mouseX <= 485 && mouseY >= 205 && mouseY <= 235) return 1;
    if(mouseX >= 315 && mouseX <= 485 && mouseY >= 245 && mouseY <= 275) return 2;
    if(mouseX >= 315 && mouseX <= 485 && mouseY >= 285 && mouseY <= 315) return 3;
    if(mouseX >= 315 && mouseX <= 485 && mouseY >= 325 && mouseY <= 355) return 4;
    return 0;
}

void drawSettingsScreen(int zombieSpeedMode) {
    char title[] = "SETTINGS";
    char back[] = "[ BACK ]";
    char speedText[] = "Zombie speed";

    setbkcolor(BLACK);
    cleardevice();

    drawFilledRect(0, 0, 799, 599, BLACK, BLACK);
    drawFilledCircle(675, 84, 30, RED, RED);
    drawKochCloud(135, 158, 130, LIGHTGRAY);
    drawKochCloud(455, 145, 110, DARKGRAY);
    drawFractalTree(80, 520, 80, 90, 6, BROWN);
    drawFractalTree(720, 520, 70, 100, 5, BROWN);
    drawLeafCluster(80, 353);
    drawLeafCluster(720, 370);
    drawFilledRect(0, 520, 799, 599, LIGHTGRAY, DARKGRAY);

    drawFilledRect(155, 95, 645, 430, LIGHTGRAY, BLACK);
    drawDashedHorizontal(180, 118, 620, LIGHTGREEN);
    drawDashedHorizontal(180, 407, 620, LIGHTGREEN);
    drawDashedVertical(180, 118, 407, LIGHTGREEN);
    drawDashedVertical(620, 118, 407, LIGHTGREEN);

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 4);
    setcolor(LIGHTGREEN);
    outtextxy(270, 135, title);

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    setcolor(WHITE);
    outtextxy(348, 205, speedText);

    drawSettingsSpeedButtons(zombieSpeedMode);
    drawMenuButton(300, 345, 500, 377, back, 0);
}

void drawSettingsSpeedButtons(int zombieSpeedMode) {
    char slow[] = "[ SLOW ]";
    char normal[] = "[ NORMAL ]";
    char fast[] = "[ FAST ]";

    drawFilledRect(210, 245, 590, 302, BLACK, BLACK);
    drawMenuButton(210, 255, 320, 288, slow, zombieSpeedMode == 0);
    drawMenuButton(345, 255, 455, 288, normal, zombieSpeedMode == 1);
    drawMenuButton(480, 255, 590, 288, fast, zombieSpeedMode == 2);
}

int getSettingsAction(int mouseX, int mouseY) {
    if(mouseX >= 210 && mouseX <= 320 && mouseY >= 255 && mouseY <= 288) return 1;
    if(mouseX >= 345 && mouseX <= 455 && mouseY >= 255 && mouseY <= 288) return 2;
    if(mouseX >= 480 && mouseX <= 590 && mouseY >= 255 && mouseY <= 288) return 3;
    if(mouseX >= 300 && mouseX <= 500 && mouseY >= 345 && mouseY <= 377) return 4;
    return 0;
}

void drawGameOverScreen(int score, int zombiesKilled) {
    char title[] = "GAME OVER";
    char scoreText[40];
    char killText[40];
    char playAgain[] = "[ PLAY AGAIN ]";
    char backText[] = "[ BACK TO MENU ]";
    char hintText[] = "Click PLAY AGAIN or press ENTER";

    sprintf(scoreText, "Score: %d", score);
    sprintf(killText, "Zombies killed: %d", zombiesKilled);

    setbkcolor(BLACK);
    cleardevice();

    drawFilledRect(0, 0, 799, 599, BLACK, BLACK);
    drawFilledCircle(665, 90, 40, RED, RED);
    drawFilledCircle(105, 80, 2, WHITE, WHITE);
    drawFilledCircle(230, 126, 2, LIGHTGRAY, LIGHTGRAY);
    drawFilledCircle(530, 94, 2, WHITE, WHITE);
    drawLineBresenham(390, 112, 396, 112, WHITE);
    drawLineBresenham(393, 109, 393, 115, WHITE);
    drawKochCloud(135, 158, 130, LIGHTGRAY);
    drawKochCloud(455, 145, 110, DARKGRAY);
    drawFractalTree(80, 520, 80, 90, 6, BROWN);
    drawFractalTree(720, 520, 70, 100, 5, BROWN);
    drawLeafCluster(80, 353);
    drawLeafCluster(50, 395);
    drawLeafCluster(113, 397);
    drawLeafCluster(720, 370);
    drawLeafCluster(690, 405);
    drawLeafCluster(752, 408);
    drawFilledRect(0, 520, 799, 599, LIGHTGRAY, DARKGRAY);

    drawFilledRect(170, 105, 630, 430, LIGHTGRAY, BLACK);
    drawDashedHorizontal(190, 125, 610, LIGHTRED);
    drawDashedHorizontal(190, 410, 610, LIGHTRED);
    drawDashedVertical(190, 125, 410, LIGHTRED);
    drawDashedVertical(610, 125, 410, LIGHTRED);

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 4);
    setcolor(RED);
    outtextxy(255, 155, title);

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    setcolor(WHITE);
    outtextxy(295, 235, scoreText);
    outtextxy(260, 270, killText);

    drawMenuButton(260, 330, 540, 362, playAgain, 1);
    drawMenuButton(260, 372, 540, 404, backText, 0);

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    setcolor(LIGHTGRAY);
    outtextxy(295, 455, hintText);
}

int getGameOverAction(int mouseX, int mouseY) {
    if(mouseX >= 260 && mouseX <= 540 && mouseY >= 330 && mouseY <= 362) return 1;
    if(mouseX >= 260 && mouseX <= 540 && mouseY >= 372 && mouseY <= 404) return 2;
    return 0;
}

void drawZombie(int x, int groundY) {
    int bodyTop = groundY - 62;
    int bodyBottom = groundY - 19;
    int headY = groundY - 80;

    drawLineBresenham(x - 14, bodyTop + 12, x - 43, bodyTop - 4, LIGHTGREEN);
    drawLineBresenham(x + 14, bodyTop + 12, x + 42, bodyTop - 9, LIGHTGREEN);
    drawFilledCircle(x - 48, bodyTop - 7, 5, GREEN, LIGHTGREEN);
    drawFilledCircle(x + 48, bodyTop - 11, 5, GREEN, LIGHTGREEN);

    drawFilledRect(x - 14, bodyTop, x + 14, bodyBottom, GREEN, LIGHTGREEN);
    drawFilledRect(x - 9, bodyTop - 8, x + 9, bodyTop, GREEN, LIGHTGREEN);

    int shirtX[] = {x - 14, x + 14, x + 10, x - 10};
    int shirtY[] = {bodyTop + 22, bodyTop + 22, bodyBottom, bodyBottom};
    drawFilledPolygon(shirtX, shirtY, 4, x, bodyTop + 28, WHITE, BLUE);

    drawFilledCircle(x, headY, 17, GREEN, LIGHTGREEN);
    drawFilledCircle(x - 7, headY - 4, 3, BLACK, RED);
    drawFilledCircle(x + 7, headY - 4, 3, BLACK, RED);
    drawLineBresenham(x - 9, headY + 8, x + 9, headY + 8, BLACK);
    drawLineBresenham(x - 5, headY + 8, x - 5, headY + 13, WHITE);
    drawLineBresenham(x + 5, headY + 8, x + 5, headY + 13, WHITE);

    drawLineBresenham(x - 10, bodyBottom, x - 22, groundY, LIGHTGREEN);
    drawLineBresenham(x + 10, bodyBottom, x + 21, groundY, LIGHTGREEN);
    drawFilledRect(x - 30, groundY, x - 14, groundY + 5, BLACK, BROWN);
    drawFilledRect(x + 14, groundY, x + 31, groundY + 5, BLACK, BROWN);
}

static void drawZombieHealthBar(int x, int groundY, int hp) {
    int left = x - 24;
    int top = groundY - 122;
    int width = hp * 16;

    if(hp < 0) hp = 0;
    if(hp > 3) hp = 3;
    width = hp * 16;

    drawFilledRect(left - 2, top - 2, left + 50, top + 8, WHITE, BLACK);
    if(hp > 0) {
        int color = GREEN;
        if(hp == 2) color = YELLOW;
        if(hp == 1) color = RED;
        drawFilledRect(left, top, left + width, top + 6, color, color);
    }
}

static void drawHudValues(int hp, int score, int kill, int minute, int second) {
    int hpWidth;
    char hpText[12];
    char scoreText[24];
    char killText[24];
    char timeText[24];

    if(hp < 0) hp = 0;
    if(hp > 5) hp = 5;
    hpWidth = hp * 148 / 5;

    sprintf(hpText, "HP %d/5", hp);
    sprintf(scoreText, "SCORE: %d", score);
    sprintf(killText, "KILL: %d", kill);
    sprintf(timeText, "TIME: %02d:%02d", minute, second);

    drawFilledRect(30, 24, 186, 36, BLACK, BLACK);
    if(hpWidth > 0) {
        drawFilledRect(32, 26, 32 + hpWidth, 34, RED, RED);
    }
    drawLineBresenham(32, 30, 180, 30, LIGHTRED);

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    drawFilledRect(36, 39, 92, 49, DARKGRAY, DARKGRAY);
    setcolor(WHITE);
    outtextxy(38, 40, hpText);

    drawFilledRect(245, 24, 410, 40, BLACK, BLACK);
    drawFilledRect(460, 24, 575, 40, BLACK, BLACK);
    drawFilledRect(610, 24, 715, 40, BLACK, BLACK);

    setcolor(LIGHTGREEN);
    outtextxy(245, 26, scoreText);
    setcolor(YELLOW);
    outtextxy(460, 26, killText);
    setcolor(LIGHTCYAN);
    outtextxy(610, 26, timeText);
}

static void drawGameFrame(int hp, int score, int kill, int minute, int second) {
    drawFilledRect(0, 0, 799, 599, BLACK, BLACK);

    drawFilledRect(12, 10, 787, 50, LIGHTGRAY, DARKGRAY);
    drawFilledRect(12, 518, 787, 562, LIGHTGRAY, DARKGRAY);
    drawLineBresenham(12, 56, 787, 56, LIGHTGRAY);
    drawLineBresenham(12, 512, 787, 512, LIGHTGRAY);
    drawLineBresenham(12, 56, 12, 512, LIGHTGRAY);
    drawLineBresenham(787, 56, 787, 512, LIGHTGRAY);

    drawFilledRect(28, 22, 188, 38, WHITE, BLACK);

    char weaponText[] = "Weapon";
    char ammoText[] = "Ammo";
    char bombText[] = "Bomb";
    char weaponValue[] = "AK47";
    char ammoValue[] = "30 / 120";
    char bombValue[] = "x2";

    drawFilledRect(720, 19, 772, 42, WHITE, BLACK);
    char backText[] = "BACK";
    setcolor(WHITE);
    outtextxy(730, 27, backText);

    setcolor(LIGHTGRAY);
    outtextxy(62, 532, weaponText);
    outtextxy(300, 532, ammoText);
    outtextxy(550, 532, bombText);

    setcolor(WHITE);
    outtextxy(70, 548, weaponValue);
    outtextxy(292, 548, ammoValue);
    outtextxy(558, 548, bombValue);

    drawHudValues(hp, score, kill, minute, second);
}

int getGameScreenAction(int mouseX, int mouseY) {
    if(mouseX >= 720 && mouseX <= 772 && mouseY >= 19 && mouseY <= 42) return 1;
    return 0;
}

static void drawMuzzleFlash(int x, int y) {
    drawFilledCircle(x, y, 10, YELLOW, YELLOW);
    drawFilledCircle(x + 9, y, 6, LIGHTRED, LIGHTRED);
    drawLineBresenham(x, y, x + 28, y, YELLOW);
    drawLineBresenham(x, y, x + 20, y - 12, YELLOW);
    drawLineBresenham(x, y, x + 20, y + 12, YELLOW);
}

static int clampValue(int value, int minValue, int maxValue) {
    if(value < minValue) return minValue;
    if(value > maxValue) return maxValue;
    return value;
}

static void* saveArea(int left, int top, int right, int bottom) {
    left = clampValue(left, 0, getmaxx());
    top = clampValue(top, 0, getmaxy());
    right = clampValue(right, 0, getmaxx());
    bottom = clampValue(bottom, 0, getmaxy());

    int size = imagesize(left, top, right, bottom);
    void *buffer = malloc(size);
    if(buffer != NULL) {
        getimage(left, top, right, bottom, buffer);
    }
    return buffer;
}

static void restoreArea(int left, int top, void *buffer) {
    if(buffer != NULL) {
        putimage(left, top, buffer, COPY_PUT);
        free(buffer);
    }
}

static void drawStraightBullet(int startX, int y, int endX) {
    int x;

    for(x = startX; x <= endX; x += 30) {
        drawFilledCircle(x, y, 3, WHITE, YELLOW);
    }
    drawLineBresenham(startX, y, endX, y, YELLOW);
}

static void drawImpactSparks(int x, int y) {
    drawLineBresenham(x, y, x + 22, y, YELLOW);
    drawLineBresenham(x, y, x - 18, y, YELLOW);
    drawLineBresenham(x, y, x, y - 20, YELLOW);
    drawLineBresenham(x, y, x, y + 16, YELLOW);
    drawLineBresenham(x, y, x + 16, y - 13, LIGHTRED);
    drawLineBresenham(x, y, x - 14, y + 12, LIGHTRED);
    drawFilledCircle(x, y, 5, WHITE, YELLOW);
}

static void drawExplosion(int x, int y) {
    drawFilledCircle(x, y, 18, RED, RED);
    drawFilledCircle(x, y, 12, LIGHTRED, LIGHTRED);
    drawFilledCircle(x, y, 6, YELLOW, YELLOW);
    drawLineBresenham(x - 28, y - 18, x + 28, y + 18, YELLOW);
    drawLineBresenham(x - 24, y + 20, x + 24, y - 20, YELLOW);
    drawLineBresenham(x - 34, y, x + 34, y, LIGHTRED);
}

static int pseudoRandomRange(int seed, int minValue, int maxValue) {
    int span = maxValue - minValue + 1;
    int value = seed * 1103515245 + 12345;

    if(value < 0) value = -value;
    return minValue + value % span;
}

static void drawLightningSegment(int x1, int y1, int x2, int y2, int color) {
    drawLineBresenham(x1, y1, x2, y2, color);
    drawLineBresenham(x1 + 1, y1, x2 + 1, y2, color);
}

static void drawLightningBolt(int startX) {
    int x1 = startX;
    int y1 = 60;
    int x2 = startX - 18;
    int y2 = 118;
    int x3 = startX + 16;
    int y3 = 174;
    int x4 = startX - 9;
    int y4 = 238;
    int x5 = startX + 28;
    int y5 = 306;

    drawLightningSegment(x1, y1, x2, y2, WHITE);
    drawLightningSegment(x2, y2, x3, y3, LIGHTCYAN);
    drawLightningSegment(x3, y3, x4, y4, WHITE);
    drawLightningSegment(x4, y4, x5, y5, LIGHTCYAN);

    drawLineBresenham(x3, y3, x3 + 42, y3 + 36, LIGHTCYAN);
    drawLineBresenham(x4, y4, x4 - 36, y4 + 28, WHITE);
    drawLineBresenham(x2, y2, x2 - 30, y2 + 24, LIGHTCYAN);
}

void drawShootEffect(int targetX, int targetY) {
    (void)targetX;
    (void)targetY;

    drawShootEffectAt(105, 485);
}

void drawShootEffectAt(int playerX, int zombieX) {
    int muzzleX = playerX + 74;
    int muzzleY = 416;
    int hitX = zombieX - 30;
    int hitY = muzzleY;
    int effectLeft = muzzleX - 35;
    int effectTop = 348;
    int effectRight = zombieX + 70;
    int effectBottom = 462;
    void *effectArea = saveArea(effectLeft, effectTop, effectRight, effectBottom);

    drawMuzzleFlash(muzzleX, muzzleY);
    drawStraightBullet(muzzleX + 20, muzzleY, hitX);
    drawImpactSparks(hitX, hitY);
    drawExplosion(hitX, hitY);

    delay(5);

    restoreArea(clampValue(effectLeft, 0, getmaxx()), clampValue(effectTop, 0, getmaxy()), effectArea);
}

void drawZombieScene() {
    drawZombieSceneAt(105, 485, 610, 720);
}

void drawZombieSceneAt(int playerX, int zombie1X, int zombie2X, int zombie3X) {
    drawZombieSceneWithHealth(playerX, zombie1X, zombie2X, zombie3X, 3, 3, 3, 0, 0, 0);
}

void drawZombieSceneWithHealth(int playerX, int zombie1X, int zombie2X, int zombie3X,
                               int zombie1Hp, int zombie2Hp, int zombie3Hp,
                               int showZombie1Hp, int showZombie2Hp, int showZombie3Hp) {
    drawZombieSceneWithStats(playerX, zombie1X, zombie2X, zombie3X,
                             zombie1Hp, zombie2Hp, zombie3Hp,
                             showZombie1Hp, showZombie2Hp, showZombie3Hp,
                             5, 0, 0, 0);
}

void drawZombieSceneWithStats(int playerX, int zombie1X, int zombie2X, int zombie3X,
                              int zombie1Hp, int zombie2Hp, int zombie3Hp,
                              int showZombie1Hp, int showZombie2Hp, int showZombie3Hp,
                              int playerHp, int score, int kill, int elapsedSeconds) {
    int zombieX[] = {zombie1X, zombie2X, zombie3X};
    int zombieHp[] = {zombie1Hp, zombie2Hp, zombie3Hp};
    int showZombieHp[] = {showZombie1Hp, showZombie2Hp, showZombie3Hp};

    drawZombieSceneManyWithStats(playerX, 3, zombieX, zombieHp, showZombieHp,
                                 playerHp, score, kill, elapsedSeconds);
}

void drawZombieSceneManyWithStats(int playerX, int zombieCount,
                                  int zombieX[], int zombieHp[], int showZombieHp[],
                                  int playerHp, int score, int kill, int elapsedSeconds) {
    drawZombieSceneManyWithPlayerStats(playerX, 452, zombieCount, zombieX, zombieHp, showZombieHp,
                                       playerHp, score, kill, elapsedSeconds);
}

void drawZombieSceneManyWithPlayerStats(int playerX, int playerGroundY, int zombieCount,
                                        int zombieX[], int zombieHp[], int showZombieHp[],
                                        int playerHp, int score, int kill, int elapsedSeconds) {
    static int gamePage = 0;
    static void *worldScene = NULL;
    static int worldSceneReady = 0;
    static int lastLightningSlot = 0;
    static int lightningFrames = 0;
    static int lightningX = 400;
    int minute = elapsedSeconds / 60;
    int second = elapsedSeconds % 60;
    int lightningSlot = elapsedSeconds / 5;
    int i;

    gamePage = 1 - gamePage;
    setactivepage(gamePage);
    setbkcolor(BLACK);
    cleardevice();

    if(!worldSceneReady) {
        drawFilledRect(20, 58, 780, 510, BLACK, BLACK);
        drawBackground();
        drawGround();

        int size = imagesize(20, 58, 780, 510);
        worldScene = malloc(size);
        if(worldScene != NULL) {
            getimage(20, 58, 780, 510, worldScene);
            worldSceneReady = 1;
        }
    }

    drawGameFrame(playerHp, score, kill, minute, second);

    if(worldSceneReady) {
        putimage(20, 58, worldScene, COPY_PUT);
    } else {
        drawFilledRect(20, 58, 780, 510, BLACK, BLACK);
        drawBackground();
        drawGround();
    }

    if(elapsedSeconds > 0 && elapsedSeconds % 5 == 0 && lightningSlot != lastLightningSlot) {
        lastLightningSlot = lightningSlot;
        lightningFrames = 3;
        lightningX = pseudoRandomRange(lightningSlot, 95, 705);
    }

    if(lightningFrames > 0) {
        drawLightningBolt(lightningX);
        lightningFrames--;
    }

    drawPlayer(playerX, playerGroundY);
    for(i = 0; i < zombieCount; i++) {
        if(zombieHp[i] > 0 && zombieX[i] > 230 && zombieX[i] < 780) {
            drawZombie(zombieX[i], 452);
            if(showZombieHp[i]) drawZombieHealthBar(zombieX[i], 452, zombieHp[i]);
        }
    }

    setvisualpage(gamePage);
    setactivepage(gamePage);
}
