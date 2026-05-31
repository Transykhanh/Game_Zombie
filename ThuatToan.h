#ifndef THUATTOAN_H
#define THUATTOAN_H

extern int c;

void setAlgorithmColor(int color);

// Bresenham line algorithm.
void drawLineBresenham(int x1, int y1, int x2, int y2, int color);
void BLine1(int x1, int y1, int x2, int y2);       // 0 < m < 1
void BLine2(int x1, int y1, int x2, int y2);       // m > 1
void BLine3(int x1, int y1, int x2, int y2);       // -1 < m < 0
void BLine4(int x1, int y1, int x2, int y2);       // m < -1
void BLineHorizontal(int x1, int y, int x2);       // m = 0
void BLineVertical(int x, int y1, int y2);         // dx = 0
void BLine45(int x1, int y1, int x2, int y2);      // m = 1
void BLineMinus45(int x1, int y1, int x2, int y2); // m = -1

// Midpoint circle algorithm.
void MPCircle(int x0, int y0, int r);
void drawCircleMidpoint(int x0, int y0, int r, int color);
void drawFilledCircle(int x0, int y0, int r, int borderColor, int fillColor);

// Recursive boundary fill.
void Fill4(int x, int y, int borderColor, int fillColor);
void Fill8(int x, int y, int borderColor, int fillColor);
void floodFill(int x, int y, int fillColor, int borderColor);

// Shapes built from the required algorithms.
void drawFilledRect(int left, int top, int right, int bottom, int borderColor, int fillColor);
void drawPolygonBresenham(const int x[], const int y[], int n, int color);
void drawFilledPolygon(const int x[], const int y[], int n, int seedX, int seedY, int borderColor, int fillColor);

// Recursive fractal.
void drawFractalTree(int x, int y, int length, int angleDeg, int depth, int color);

// Cac thuat toan fractal de quy.
void setFractalStart(int x, int y);
void D(int n, float len, float d, int s);                 // Duong rong dang turtle
void D1(int n, int x1, int y1, int x2, int y2);           // Duong rong theo 2 diem
void C(int n, float len, float d);                        // Duong cong C dang turtle
void C(int n, int a, int b, int c, int d);                // Duong cong C theo 2 diem
void Snow(int n, float x, float y, float R);              // Bong tuyet Koch
void K(int n, float a, float b, float c, float d);        // Duong Koch theo 2 diem
void K(int n, float len, float d);                        // Duong Koch dang turtle
void K1(int n, float a, float b, float c, float d);       // Bien the duong Koch theo 2 diem

#endif
