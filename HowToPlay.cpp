#include "HowToPlay.h"
#include "ThuatToan.h"
#include <graphics.h>

static void drawKeyBox(int left, int top, char text[]) {
    drawFilledRect(left, top, left + 58, top + 34, LIGHTGRAY, DARKGRAY);
    setcolor(WHITE);
    outtextxy(left + 18, top + 12, text);
}

static void drawMiniZombie(int x, int y) {
    drawFilledCircle(x, y, 18, GREEN, LIGHTGREEN);
    drawFilledCircle(x - 7, y - 4, 3, BLACK, RED);
    drawFilledCircle(x + 7, y - 4, 3, BLACK, RED);
    drawLineBresenham(x - 10, y + 8, x + 10, y + 8, BLACK);
    drawLineBresenham(x - 4, y + 8, x - 4, y + 14, WHITE);
    drawLineBresenham(x + 4, y + 8, x + 4, y + 14, WHITE);
}

static void drawPanel(int left, int top, int right, int bottom) {
    drawFilledRect(left, top, right, bottom, LIGHTGRAY, BLACK);
    drawLineBresenham(left + 8, top + 8, right - 8, top + 8, LIGHTGREEN);
    drawLineBresenham(left + 8, bottom - 8, right - 8, bottom - 8, LIGHTGREEN);
    drawLineBresenham(left + 8, top + 8, left + 8, bottom - 8, LIGHTGREEN);
    drawLineBresenham(right - 8, top + 8, right - 8, bottom - 8, LIGHTGREEN);
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

void drawHowToPlayScreen() {
    setbkcolor(BLACK);
    cleardevice();

    drawFilledRect(0, 0, 799, 599, BLACK, BLACK);
    drawFilledCircle(705, 82, 34, RED, RED);
    drawFilledCircle(90, 80, 2, WHITE, WHITE);
    drawFilledCircle(210, 118, 2, LIGHTGRAY, LIGHTGRAY);
    drawFilledCircle(345, 76, 2, WHITE, WHITE);
    drawFilledCircle(520, 122, 2, LIGHTGRAY, LIGHTGRAY);
    drawLineBresenham(430, 92, 436, 92, WHITE);
    drawLineBresenham(433, 89, 433, 95, WHITE);
    drawKochCloud(135, 155, 125, LIGHTGRAY);
    drawKochCloud(455, 145, 105, DARKGRAY);
    drawFractalTree(80, 520, 78, 90, 6, BROWN);
    drawFractalTree(720, 520, 68, 100, 5, BROWN);
    drawFilledRect(0, 520, 799, 599, LIGHTGRAY, DARKGRAY);

    drawPanel(110, 70, 690, 500);
    drawMiniZombie(625, 145);

    char title[] = "HOW TO PLAY";
    char moveText[] = "MOVE";
    char shootText[] = "SHOOT";
    char weaponText[] = "CHANGE WEAPON";
    char pauseText[] = "PAUSE";
    char targetText[] = "GAME OBJECTIVE";
    char line1[] = "Use A / D to move backward and forward.";
    char line2[] = "Press SPACE or click mouse to shoot.";
    char line3[] = "Press Q / E to change weapon.";
    char line4[] = "Press P or ESC to pause the game.";
    char line5[] = "Kill zombies before they reach you.";
    char line6[] = "Survive as long as possible.";
    char back[] = "Press any key or click to return";

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
    setcolor(LIGHTGREEN);
    outtextxy(280, 105, title);

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    setcolor(YELLOW);
    outtextxy(150, 170, moveText);
    char keyA[] = "A";
    char keyD[] = "D";
    drawKeyBox(150, 220, keyA);
    drawKeyBox(220, 220, keyD);

    setcolor(WHITE);
    outtextxy(370, 205, line1);

    setcolor(YELLOW);
    outtextxy(150, 300, shootText);
    setcolor(WHITE);
    outtextxy(150, 325, line2);

    setcolor(YELLOW);
    outtextxy(150, 355, weaponText);
    setcolor(WHITE);
    outtextxy(150, 380, line3);

    setcolor(YELLOW);
    outtextxy(150, 410, pauseText);
    setcolor(WHITE);
    outtextxy(150, 435, line4);

    setcolor(LIGHTRED);
    outtextxy(410, 300, targetText);
    setcolor(WHITE);
    outtextxy(410, 325, line5);
    outtextxy(410, 350, line6);

    setcolor(LIGHTGRAY);
    outtextxy(285, 470, back);
}
