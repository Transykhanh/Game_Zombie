#include "Player.h"
#include "ThuatToan.h"
#include <graphics.h>

static void drawGun(int x, int y) {
    // Ve sung cua nguoi choi: than sung bang hinh chu nhat, tia lua bang Bresenham.
    drawFilledRect(x + 13, y - 40, x + 55, y - 33, BLACK, DARKGRAY);
    drawFilledRect(x + 52, y - 38, x + 74, y - 35, BLACK, LIGHTGRAY);
    drawFilledRect(x + 21, y - 33, x + 29, y - 20, BLACK, DARKGRAY);

    drawLineBresenham(x + 74, y - 36, x + 90, y - 36, YELLOW);
    drawLineBresenham(x + 74, y - 36, x + 86, y - 45, YELLOW);
    drawLineBresenham(x + 74, y - 36, x + 86, y - 27, YELLOW);
}

void drawPlayer(int x, int groundY) {
    // Ve nhan vat nguoi choi bang cac ham tu cai dat: hinh tron, hinh chu nhat, Bresenham.
    int bodyTop = groundY - 72;
    int bodyBottom = groundY - 24;

    // Dau, mat va mieng: drawFilledCircle + drawLineBresenham.
    drawFilledCircle(x, groundY - 94, 15, WHITE, LIGHTGRAY);
    drawFilledCircle(x - 5, groundY - 97, 2, BLACK, BLACK);
    drawFilledCircle(x + 5, groundY - 97, 2, BLACK, BLACK);
    drawLineBresenham(x - 5, groundY - 88, x + 8, groundY - 88, BLACK);

    // Than ao: drawFilledRect; duong cheo ao: drawLineBresenham.
    drawFilledRect(x - 15, bodyTop, x + 15, bodyBottom, WHITE, BLUE);
    drawLineBresenham(x - 15, bodyTop + 10, x + 15, bodyTop + 20, WHITE);

    // Tay nguoi choi: cac doan thang Bresenham.
    drawLineBresenham(x + 13, bodyTop + 10, x + 42, bodyTop + 34, LIGHTGRAY);
    drawLineBresenham(x + 12, bodyTop + 12, x + 40, bodyTop + 36, WHITE);
    drawLineBresenham(x - 13, bodyTop + 10, x - 32, bodyTop + 33, LIGHTGRAY);

    drawGun(x, groundY);

    // Chan va giay: chan bang Bresenham, giay bang hinh chu nhat to mau.
    drawLineBresenham(x - 7, bodyBottom, x - 19, groundY, BLUE);
    drawLineBresenham(x + 7, bodyBottom, x + 18, groundY, BLUE);
    drawFilledRect(x - 28, groundY, x - 10, groundY + 6, BLACK, BROWN);
    drawFilledRect(x + 10, groundY, x + 29, groundY + 6, BLACK, BROWN);
}
