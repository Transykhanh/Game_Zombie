#include "ThuatToan.h"
#include <graphics.h>
#include <math.h>
#include <stdlib.h>

int c = WHITE;

static const double RADS = 3.14159265 / 180.0;
static const double DRAGON_FACT = 0.70710678;
static const double KOCH_FACT = 1.73205081;
static int fractalX = 0;
static int fractalY = 0;

static void swapInt(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
}

static void plotCircleSymmetry(int x0, int y0, int x, int y) {
    // Ve 8 diem doi xung cua duong tron Midpoint quanh tam (x0, y0).
    putpixel(x0 + x, y0 + y, c);
    putpixel(x0 - x, y0 + y, c);
    putpixel(x0 + x, y0 - y, c);
    putpixel(x0 - x, y0 - y, c);
    putpixel(x0 + y, y0 + x, c);
    putpixel(x0 - y, y0 + x, c);
    putpixel(x0 + y, y0 - x, c);
    putpixel(x0 - y, y0 - x, c);
}

static void drawFractalLine(int x1, int y1, int x2, int y2) {
    // Moi canh fractal deu ve bang duong thang Bresenham.
    drawLineBresenham(x1, y1, x2, y2, c);
}

static void drawFractalLineRel(int dx, int dy) {
    // Ve mot doan fractal theo toa do tuong doi kieu turtle.
    int nextX = fractalX + dx;
    int nextY = fractalY + dy;
    drawFractalLine(fractalX, fractalY, nextX, nextY);
    fractalX = nextX;
    fractalY = nextY;
}

void setAlgorithmColor(int color) {
    // Dat mau dung chung cho cac ham thuat toan ve diem/duong.
    c = color;
}

// Ham dieu phoi: phan loai duong thang roi goi dung 1 trong 8 truong hop.
void drawLineBresenham(int x1, int y1, int x2, int y2, int color) {
    c = color;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    if(dx == 0 && dy == 0) {
        putpixel(x1, y1, c);
    } else if(dy == 0) {
        BLineHorizontal(x1, y1, x2);
    } else if(dx == 0) {
        BLineVertical(x1, y1, y2);
    } else if(dx == dy && (x2 - x1) * (y2 - y1) > 0) {
        BLine45(x1, y1, x2, y2);
    } else if(dx == dy) {
        BLineMinus45(x1, y1, x2, y2);
    } else if((x2 - x1) * (y2 - y1) > 0 && dx > dy) {
        BLine1(x1, y1, x2, y2);
    } else if((x2 - x1) * (y2 - y1) > 0) {
        BLine2(x1, y1, x2, y2);
    } else if(dx > dy) {
        BLine3(x1, y1, x2, y2);
    } else {
        BLine4(x1, y1, x2, y2);
    }
}

// TH1: 0 < m < 1, x tang, y tang cham.
void BLine1(int x1, int y1, int x2, int y2) {
    if(x1 > x2) {
        swapInt(x1, x2);
        swapInt(y1, y2);
    }

    int x, y, dx, dy, e, et, ekt;
    dx = x2 - x1;
    dy = y2 - y1;
    ekt = 2 * dy;
    e = ekt - dx;
    et = e - dx;

    for(x = x1, y = y1; x <= x2; x++) {
        putpixel(x, y, c);
        if(e < 0) {
            e += ekt;
        } else {
            e += et;
            y++;
        }
    }
}

// TH2: m > 1, y tang nhanh hon x.
void BLine2(int x1, int y1, int x2, int y2) {
    if(y1 > y2) {
        swapInt(x1, x2);
        swapInt(y1, y2);
    }

    int x, y, dx, dy, e, et, ekt;
    dx = x2 - x1;
    dy = y2 - y1;
    e = 2 * dx - dy;
    ekt = 2 * dx;
    et = 2 * dx - 2 * dy;

    for(x = x1, y = y1; y <= y2; y++) {
        putpixel(x, y, c);
        if(e < 0) {
            e += ekt;
        } else {
            e += et;
            x++;
        }
    }
}

// TH3: -1 < m < 0, x tang, y giam cham.
void BLine3(int x1, int y1, int x2, int y2) {
    if(x1 > x2) {
        swapInt(x1, x2);
        swapInt(y1, y2);
    }

    int x, y, dx, dy, e, eg, ekg;
    dx = x2 - x1;
    dy = y2 - y1;
    e = -2 * dy - dx;
    ekg = -2 * dy;
    eg = -2 * dy - 2 * dx;

    for(x = x1, y = y1; x <= x2; x++) {
        putpixel(x, y, c);
        if(e < 0) {
            e += ekg;
        } else {
            e += eg;
            y--;
        }
    }
}

// TH4: m < -1, y giam nhanh hon x.
void BLine4(int x1, int y1, int x2, int y2) {
    if(x1 > x2) {
        swapInt(x1, x2);
        swapInt(y1, y2);
    }

    int x, y, dx, dy, e, et, ekt;
    dx = x2 - x1;
    dy = y2 - y1;
    e = 2 * dx + dy;
    ekt = 2 * dx;
    et = 2 * dx + 2 * dy;

    for(x = x1, y = y1; y >= y2; y--) {
        putpixel(x, y, c);
        if(e < 0) {
            e += ekt;
        } else {
            e += et;
            x++;
        }
    }
}

// TH5: duong ngang, dy = 0.
void BLineHorizontal(int x1, int y, int x2) {
    if(x1 > x2) swapInt(x1, x2);

    for(int x = x1; x <= x2; x++) {
        putpixel(x, y, c);
    }
}

// TH6: duong dung, dx = 0.
void BLineVertical(int x, int y1, int y2) {
    if(y1 > y2) swapInt(y1, y2);

    for(int y = y1; y <= y2; y++) {
        putpixel(x, y, c);
    }
}

// TH7: duong cheo 45 do, m = 1.
void BLine45(int x1, int y1, int x2, int y2) {
    int sx = (x2 >= x1) ? 1 : -1;
    int sy = (y2 >= y1) ? 1 : -1;
    int step = abs(x2 - x1);

    for(int i = 0; i <= step; i++) {
        putpixel(x1 + i * sx, y1 + i * sy, c);
    }
}

// TH8: duong cheo -45 do, m = -1.
void BLineMinus45(int x1, int y1, int x2, int y2) {
    int sx = (x2 >= x1) ? 1 : -1;
    int sy = (y2 >= y1) ? 1 : -1;
    int step = abs(x2 - x1);

    for(int i = 0; i <= step; i++) {
        putpixel(x1 + i * sx, y1 + i * sy, c);
    }
}

void MPCircle(int x0, int y0, int r) {
    // Thuat toan Midpoint ve duong tron; dung cho dau, mat, mat trang, banh xe, hieu ung no.
    if(r < 0) return;

    int x = 0;
    int y = r;
    int p = 1 - r;

    while(x <= y) {
        plotCircleSymmetry(x0, y0, x, y);
        x++;
        if(p < 0) {
            p += 2 * x + 1;
        } else {
            y--;
            p += 2 * (x - y) + 1;
        }
    }
}

void drawCircleMidpoint(int x0, int y0, int r, int color) {
    // Ham boc ngoai: chon mau roi ve duong tron bang Midpoint.
    c = color;
    MPCircle(x0, y0, r);
}

void drawFilledCircle(int x0, int y0, int r, int borderColor, int fillColor) {
    // Ve hinh tron co to mau: quet cac day ngang, sau do vien bang Midpoint.
    if(r < 0) return;

    setcolor(fillColor);
    for(int y = -r; y <= r; y++) {
        int xLimit = (int)sqrt((double)(r * r - y * y));
        line(x0 - xLimit, y0 + y, x0 + xLimit, y0 + y);
    }

    drawCircleMidpoint(x0, y0, r, borderColor);
}

void Fill4(int x, int y, int borderColor, int fillColor) {
    // To mau mien kin theo 4 huong bang stack/scanline, tranh tran ngan xep khi vung lon.
    struct FillPoint {
        int x;
        int y;
    };

    int maxX = getmaxx();
    int maxY = getmaxy();
    int targetColor;
    int stackLimit;
    int stackTop = 0;
    FillPoint *stack;

    if(fillColor == borderColor) return;
    if(x < 0 || y < 0 || x > maxX || y > maxY) return;

    targetColor = getpixel(x, y);
    if(targetColor == borderColor || targetColor == fillColor) return;

    stackLimit = (maxX + 1) * (maxY + 1);
    stack = (FillPoint*)malloc(sizeof(FillPoint) * stackLimit);
    if(stack == NULL) return;

    stack[stackTop].x = x;
    stack[stackTop].y = y;
    stackTop++;

    setcolor(fillColor);
    while(stackTop > 0) {
        FillPoint point = stack[--stackTop];
        int left;
        int right;

        if(point.x < 0 || point.y < 0 || point.x > maxX || point.y > maxY) continue;
        if(getpixel(point.x, point.y) != targetColor) continue;

        left = point.x;
        while(left >= 0 && getpixel(left, point.y) == targetColor) {
            left--;
        }
        left++;

        right = point.x;
        while(right <= maxX && getpixel(right, point.y) == targetColor) {
            right++;
        }
        right--;

        line(left, point.y, right, point.y);

        if(point.y > 0) {
            int inSpan = 0;
            for(int scanX = left; scanX <= right; scanX++) {
                if(getpixel(scanX, point.y - 1) == targetColor) {
                    if(!inSpan && stackTop < stackLimit) {
                        stack[stackTop].x = scanX;
                        stack[stackTop].y = point.y - 1;
                        stackTop++;
                    }
                    inSpan = 1;
                } else {
                    inSpan = 0;
                }
            }
        }

        if(point.y < maxY) {
            int inSpan = 0;
            for(int scanX = left; scanX <= right; scanX++) {
                if(getpixel(scanX, point.y + 1) == targetColor) {
                    if(!inSpan && stackTop < stackLimit) {
                        stack[stackTop].x = scanX;
                        stack[stackTop].y = point.y + 1;
                        stackTop++;
                    }
                    inSpan = 1;
                } else {
                    inSpan = 0;
                }
            }
        }
    }

    free(stack);
}

void Fill8(int x, int y, int borderColor, int fillColor) {
    // To mau de quy 8 huong cho mien kin nho; dung de minh hoa thuat toan to mau de quy.
    if(fillColor == borderColor) return;
    if(x < 0 || y < 0 || x > getmaxx() || y > getmaxy()) return;

    int current = getpixel(x, y);
    if(current == borderColor || current == fillColor) return;

    putpixel(x, y, fillColor);
    Fill8(x + 1, y, borderColor, fillColor);
    Fill8(x - 1, y, borderColor, fillColor);
    Fill8(x, y + 1, borderColor, fillColor);
    Fill8(x, y - 1, borderColor, fillColor);
    Fill8(x + 1, y + 1, borderColor, fillColor);
    Fill8(x - 1, y + 1, borderColor, fillColor);
    Fill8(x + 1, y - 1, borderColor, fillColor);
    Fill8(x - 1, y - 1, borderColor, fillColor);
}

void floodFill(int x, int y, int fillColor, int borderColor) {
    // Ham goi to mau mien kin tu diem hat giong (x, y).
    Fill4(x, y, borderColor, fillColor);
}

void drawFilledRect(int left, int top, int right, int bottom, int borderColor, int fillColor) {
    // Ve hinh chu nhat co to mau; ap dung cho nen, mat dat, HUD, nut bam, than nguoi, thung go.
    if(left > right) swapInt(left, right);
    if(top > bottom) swapInt(top, bottom);

    setfillstyle(SOLID_FILL, fillColor);
    bar(left, top, right, bottom);
    setcolor(borderColor);
    rectangle(left, top, right, bottom);
}

void drawPolygonBresenham(const int x[], const int y[], int n, int color) {
    // Ve bien da giac bang cach noi cac dinh bang Bresenham.
    if(n < 2) return;

    for(int i = 0; i < n; i++) {
        int next = (i + 1) % n;
        drawLineBresenham(x[i], y[i], x[next], y[next], color);
    }
}

void drawFilledPolygon(const int x[], const int y[], int n, int seedX, int seedY, int borderColor, int fillColor) {
    // To da giac bang scanline, sau do ve vien bang Bresenham; dung cho xe hong, ao zombie, hinh bien doi.
    int minY;
    int maxY;
    int intersections[64];

    (void)seedX;
    (void)seedY;

    if(n < 3) {
        drawPolygonBresenham(x, y, n, borderColor);
        return;
    }

    minY = y[0];
    maxY = y[0];
    for(int i = 1; i < n; i++) {
        if(y[i] < minY) minY = y[i];
        if(y[i] > maxY) maxY = y[i];
    }

    setcolor(fillColor);
    for(int scanY = minY; scanY <= maxY; scanY++) {
        int count = 0;

        for(int i = 0; i < n; i++) {
            int next = (i + 1) % n;
            int y1 = y[i];
            int y2 = y[next];
            int x1 = x[i];
            int x2 = x[next];

            if(y1 == y2) continue;
            if((scanY >= y1 && scanY < y2) || (scanY >= y2 && scanY < y1)) {
                if(count < 64) {
                    intersections[count] = x1 + (scanY - y1) * (x2 - x1) / (y2 - y1);
                    count++;
                }
            }
        }

        for(int i = 0; i < count - 1; i++) {
            for(int j = i + 1; j < count; j++) {
                if(intersections[i] > intersections[j]) {
                    swapInt(intersections[i], intersections[j]);
                }
            }
        }

        for(int i = 0; i + 1 < count; i += 2) {
            line(intersections[i], scanY, intersections[i + 1], scanY);
        }
    }

    drawPolygonBresenham(x, y, n, borderColor);
}

void drawFractalTree(int x, int y, int length, int angleDeg, int depth, int color) {
    // Ve cay fractal de quy trong nen game: than va nhanh deu la Bresenham.
    if(depth <= 0 || length < 4) return;

    double angle = angleDeg * 3.14159265 / 180.0;
    int x2 = x + (int)(cos(angle) * length);
    int y2 = y - (int)(sin(angle) * length);

    drawLineBresenham(x, y, x2, y2, color);
    if(length > 18) {
        drawLineBresenham(x + 1, y, x2 + 1, y2, color);
    }

    drawFractalTree(x2, y2, length * 2 / 3, angleDeg + 25, depth - 1, color);
    drawFractalTree(x2, y2, length * 2 / 3, angleDeg - 25, depth - 1, color);
    if(depth > 2) {
        drawFractalTree(x2, y2, length / 2, angleDeg + 5, depth - 1, color);
    }
}

// Dat vi tri bat dau cho cac fractal dang turtle dung linerel.
void setFractalStart(int x, int y) {
    // Dat toa do bat dau cho cac fractal dang turtle nhu Koch, C, Dragon.
    fractalX = x;
    fractalY = y;
}

// Ham D: ve duong rong dang turtle, moi doan cuoi ve bang Bresenham.
void D(int n, float len, float d, int s) {
    // Ve duong rong Dragon de quy dang turtle; co the dung lam la/canh cong.
    if(n > 0) {
        d += 45 * s;
        D(n - 1, (float)(len * DRAGON_FACT), d, 1);
        d -= 90 * s;
        D(n - 1, (float)(len * DRAGON_FACT), d, -1);
    } else {
        drawFractalLineRel((int)(len * cos(d * RADS)), (int)(len * sin(d * RADS)));
    }
}

// Ham D1: ve duong rong theo 2 diem dau cuoi.
void D1(int n, int x1, int y1, int x2, int y2) {
    // Ve duong rong Dragon theo 2 diem; trong game dung de ve la cong.
    if(n > 0) {
        int midX = (x1 + x2 + y1 - y2) / 2;
        int midY = (y1 + y2 + x2 - x1) / 2;
        D1(n - 1, x1, y1, midX, midY);
        D1(n - 1, x2, y2, midX, midY);
    } else {
        drawFractalLine(x1, y1, x2, y2);
    }
}

// Ham C: ve duong cong C dang turtle, chia nhanh trai phai 45 do.
void C(int n, float len, float d) {
    // Ve duong cong C de quy dang turtle.
    if(n > 0) {
        d += 45;
        C(n - 1, (float)(len * DRAGON_FACT), d);
        d -= 90;
        C(n - 1, (float)(len * DRAGON_FACT), d);
        d += 45;
    } else {
        drawFractalLineRel((int)(len * cos(d * RADS)), (int)(len * sin(d * RADS)));
    }
}

// Ham C: ve duong cong C theo 2 diem dau cuoi.
void C(int n, int a, int b, int c2, int d2) {
    // Ve duong cong C de quy theo 2 diem dau cuoi.
    if(n > 0) {
        int midX = (a + b + c2 - d2) / 2;
        int midY = (b + c2 + d2 - a) / 2;
        C(n - 1, a, b, midX, midY);
        C(n - 1, midX, midY, c2, d2);
    } else {
        drawFractalLine(a, b, c2, d2);
    }
}

// Ham Snow: ve bong tuyet Koch tu tam va ban kinh R.
void Snow(int n, float x, float y, float R) {
    // Ve bong tuyet Koch tu tam va ban kinh; la fractal trang tri co the dua vao nen.
    float a, b, c2, d2, u, v;

    a = (float)(x - R * KOCH_FACT);
    b = y - R / 2;
    c2 = (float)(x + R * KOCH_FACT);
    d2 = y - R / 2;
    u = x;
    v = y + R;

    drawFractalLine((int)a, (int)b, (int)c2, (int)d2);
    drawFractalLine((int)c2, (int)d2, (int)u, (int)v);
    drawFractalLine((int)u, (int)v, (int)a, (int)b);

    K(n, u, v, a, b);
    K(n, a, b, c2, d2);
    K(n, c2, d2, u, v);
}

// Ham K: ve duong Koch theo 2 diem dau cuoi.
void K(int n, float a, float b, float c2, float d2) {
    // Ve duong Koch theo 2 diem; dung de tao canh may/duong gap khuc fractal.
    float u, v;

    if(n > 0) {
        u = (float)(a + (c2 - a) / 2 + KOCH_FACT * (d2 - b) / 3);
        v = (float)(b + (d2 - b) / 2 + KOCH_FACT * (a - c2) / 3);
        K(n - 1, a, b, a + (c2 - a) / 3, b + (d2 - b) / 3);
        K(n - 1, a + (c2 - a) / 3, b + (d2 - b) / 3, u, v);
        K(n - 1, u, v, a + 2 * (c2 - a) / 3, b + 2 * (d2 - b) / 3);
        K(n - 1, a + 2 * (c2 - a) / 3, b + 2 * (d2 - b) / 3, c2, d2);
    } else {
        drawFractalLine((int)a, (int)b, (int)c2, (int)d2);
    }
}

// Ham K: ve duong Koch dang turtle.
void K(int n, float len, float d) {
    // Ve duong Koch dang turtle; trong game dung de ve dam may Koch.
    if(n > 0) {
        K(n - 1, len / 3, d);
        d -= 60;
        K(n - 1, len / 3, d);
        d += 120;
        K(n - 1, len / 3, d);
        d -= 60;
        K(n - 1, len / 3, d);
    } else {
        drawFractalLineRel((int)(len * cos(d * RADS)), (int)(len * sin(d * RADS)));
    }
}

// Ham K1: bien the duong Koch theo 2 diem dau cuoi.
void K1(int n, float a, float b, float c2, float d2) {
    // Bien the Koch theo 2 diem, giu lai de mo rong cac vat the fractal.
    float u, v;

    if(n > 0) {
        u = (float)(a + (c2 - a) / 2 + KOCH_FACT * (d2 - b) / 3);
        v = (float)(b + (d2 - b) / 2 + KOCH_FACT * (a - c2) / 3);
        K1(n - 1, a, b, a + (c2 - a) / 3, b + (d2 - b) / 3);
        K1(n - 1, a + (c2 - a) / 3, b + (d2 - b) / 3, u, v);
        K1(n - 1, u, v, a + 2 * (c2 - a) / 3, b + 2 * (d2 - b) / 3);
        K1(n - 1, a + 2 * (c2 - a) / 3, b + 2 * (d2 - b) / 3, c2, d2);
    } else {
        drawFractalLine((int)a, (int)b, (int)c2, (int)d2);
    }
}
