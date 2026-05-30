#include <GL/glut.h>
#include <cmath>
#include <ctime>
using namespace std;

const float PI = 3.14159265f;
int winW = 600, winH = 600;
float cx = 300, cy = 300;
float radius = 220;

// Bresenham's Line Algorithm
void bresenhamLine(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        glVertex2i(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx)  { err += dx; y0 += sy; }
    }
}

void drawBresenhamLine(int x0, int y0, int x1, int y1) {
    glBegin(GL_POINTS);
    bresenhamLine(x0, y0, x1, y1);
    glEnd();
}

// Circle drawing
void drawCircle(float x, float y, float r) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++) {
        float theta = i * PI / 180;
        glVertex2f(x + r * cos(theta), y + r * sin(theta));
    }
    glEnd();
}

void drawFilledCircle(float x, float y, float r) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 360; i++) {
        float theta = i * PI / 180;
        glVertex2f(x + r * cos(theta), y + r * sin(theta));
    }
    glEnd();
}

// Clock face
void drawClockFace() {
    // Outer glow circles
    glColor3f(1.0f, 0.4f, 0.0f);
    drawCircle(cx, cy, radius + 10);
    glColor3f(1.0f, 0.6f, 0.0f);
    drawCircle(cx, cy, radius + 6);
    glColor3f(1.0f, 0.8f, 0.0f);
    drawCircle(cx, cy, radius + 3);

    // Main clock face - gradient effect
    for (int r = radius; r > 0; r -= 2) {
        float t = (float)r / radius;
        glColor3f(0.1f * t, 0.1f * t, 0.8f - 0.3f * t);
        drawFilledCircle(cx, cy, r);
    }

    // White border circle
    glColor3f(1, 1, 1);
    drawCircle(cx, cy, radius);
    drawCircle(cx, cy, radius - 5);
}

// Hour markings
void drawHourMarkings() {
    for (int i = 0; i < 12; i++) {
        float angle = (90 - i * 30) * PI / 180;
        float outerR = radius - 8;
        float innerR = radius - 22;

        float x1 = cx + outerR * cos(angle);
        float y1 = cy + outerR * sin(angle);
        float x2 = cx + innerR * cos(angle);
        float y2 = cy + innerR * sin(angle);

        glColor3f(1, 1, 1);
        glLineWidth(3);
        drawBresenhamLine((int)x1, (int)y1, (int)x2, (int)y2);
    }

    // Minute markings
    for (int i = 0; i < 60; i++) {
        if (i % 5 == 0) continue; // skip hour marks
        float angle = (90 - i * 6) * PI / 180;
        float outerR = radius - 8;
        float innerR = radius - 14;

        float x1 = cx + outerR * cos(angle);
        float y1 = cy + outerR * sin(angle);
        float x2 = cx + innerR * cos(angle);
        float y2 = cy + innerR * sin(angle);

        glColor3f(0.8f, 0.8f, 0.8f);
        glLineWidth(1);
        drawBresenhamLine((int)x1, (int)y1, (int)x2, (int)y2);
    }
}

// Hour numbers
void drawNumbers() {
    const char* nums[] = {"12","1","2","3","4","5","6","7","8","9","10","11"};
    for (int i = 0; i < 12; i++) {
        float angle = (90 - i * 30) * PI / 180;
        float r = radius - 38;
        float x = cx + r * cos(angle) - 5;
        float y = cy + r * sin(angle) - 5;

        glColor3f(1, 1, 1);
        glRasterPos2f(x, y);
        const char* s = nums[i];
        while (*s)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *s++);
    }
}

// Clock hands using Bresenham
void drawHand(float angle, float length, float r, float g, float b, int thickness) {
    float rad = angle * PI / 180;
    int x1 = (int)cx;
    int y1 = (int)cy;
    int x2 = (int)(cx + length * cos(rad));
    int y2 = (int)(cy + length * sin(rad));

    glColor3f(r, g, b);
    glLineWidth(thickness);
    drawBresenhamLine(x1, y1, x2, y2);

    // Thickness simulation — draw parallel lines
    for (int t = 1; t < thickness; t++) {
        drawBresenhamLine(x1+t, y1, x2+t, y2);
        drawBresenhamLine(x1, y1+t, x2, y2+t);
    }
}

void drawClockHands() {
    time_t now = time(0);
    struct tm* t = localtime(&now);

    int h = t->tm_hour % 12;
    int m = t->tm_min;
    int s = t->tm_sec;

    // Angles (90 = 12 o'clock position)
    float secAngle  = 90 - s * 6;
    float minAngle  = 90 - m * 6 - s * 0.1f;
    float hourAngle = 90 - h * 30 - m * 0.5f;

    // Hour hand — thick white
    drawHand(hourAngle, radius * 0.5f, 1, 1, 1, 5);

    // Minute hand — medium white
    drawHand(minAngle, radius * 0.75f, 1, 1, 1, 3);

    // Second hand — thin red
    drawHand(secAngle, radius * 0.9f, 1, 0, 0, 1);

    // Center dot
    glColor3f(1, 0, 0);
    drawFilledCircle(cx, cy, 6);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawClockFace();
    drawHourMarkings();
    drawNumbers();
    drawClockHands();

    glutSwapBuffers();
}

void timer(int val) {
    glutPostRedisplay();
    glutTimerFunc(1000, timer, 0);
}

void init() {
    glClearColor(0, 0, 0, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, winW, 0, winH);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Interactive Graphics Clock");
    init();
    glutDisplayFunc(display);
    glutTimerFunc(1000, timer, 0);
    glutMainLoop();
}