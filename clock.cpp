#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <cstdio>
using namespace std;

const float PI = 3.14159265f;
int winW = 600, winH = 650;
float cx = 300, cy = 330;
float radius = 220;

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

void drawClockFace() {
    glColor3f(1.0f, 0.4f, 0.0f);
    drawCircle(cx, cy, radius + 10);
    glColor3f(1.0f, 0.6f, 0.0f);
    drawCircle(cx, cy, radius + 6);
    glColor3f(1.0f, 0.8f, 0.0f);
    drawCircle(cx, cy, radius + 3);

    for (int r = radius; r > 0; r -= 2) {
        float t = (float)r / radius;
        glColor3f(0.1f * t, 0.1f * t, 0.8f - 0.3f * t);
        drawFilledCircle(cx, cy, r);
    }

    glColor3f(1, 1, 1);
    drawCircle(cx, cy, radius);
    drawCircle(cx, cy, radius - 5);
}

void drawHourMarkings() {
    for (int i = 0; i < 12; i++) {
        float angle = (90 - i * 30) * PI / 180;
        float x1 = cx + (radius - 8) * cos(angle);
        float y1 = cy + (radius - 8) * sin(angle);
        float x2 = cx + (radius - 22) * cos(angle);
        float y2 = cy + (radius - 22) * sin(angle);
        glColor3f(1, 1, 1);
        glLineWidth(3);
        drawBresenhamLine((int)x1, (int)y1, (int)x2, (int)y2);
    }

    for (int i = 0; i < 60; i++) {
        if (i % 5 == 0) continue;
        float angle = (90 - i * 6) * PI / 180;
        float x1 = cx + (radius - 8) * cos(angle);
        float y1 = cy + (radius - 8) * sin(angle);
        float x2 = cx + (radius - 14) * cos(angle);
        float y2 = cy + (radius - 14) * sin(angle);
        glColor3f(0.8f, 0.8f, 0.8f);
        glLineWidth(1);
        drawBresenhamLine((int)x1, (int)y1, (int)x2, (int)y2);
    }
}

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

void drawHand(float angle, float length, float tailLength,
              float r, float g, float b, int thickness) {
    float rad = angle * PI / 180;

    // Main hand
    int x1 = (int)cx;
    int y1 = (int)cy;
    int x2 = (int)(cx + length * cos(rad));
    int y2 = (int)(cy + length * sin(rad));

    // Tail (opposite direction)
    int xt = (int)(cx - tailLength * cos(rad));
    int yt = (int)(cy - tailLength * sin(rad));

    glColor3f(r, g, b);
    glLineWidth(thickness);

    // Draw main hand
    drawBresenhamLine(x1, y1, x2, y2);
    for (int t = 1; t < thickness; t++) {
        drawBresenhamLine(x1+t, y1, x2+t, y2);
        drawBresenhamLine(x1, y1+t, x2, y2+t);
    }

    // Draw tail
    drawBresenhamLine(x1, y1, xt, yt);
}

void drawDigitalTime() {
    time_t now = time(0);
    struct tm* t = localtime(&now);

    glColor3f(0.1f, 0.1f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(160, 55);
    glVertex2f(440, 55);
    glVertex2f(440, 110);
    glVertex2f(160, 110);
    glEnd();

    glColor3f(1.0f, 0.6f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(160, 55);
    glVertex2f(440, 55);
    glVertex2f(440, 110);
    glVertex2f(160, 110);
    glEnd();

    char timeStr[20];
    sprintf(timeStr, "%02d:%02d:%02d",
        t->tm_hour, t->tm_min, t->tm_sec);

    glColor3f(0.0f, 1.0f, 0.8f);
    glRasterPos2f(185, 75);
    const char* s = timeStr;
    while(*s)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *s++);

    char dateStr[30];
    const char* days[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    const char* months[] = {"Jan","Feb","Mar","Apr","May","Jun",
                            "Jul","Aug","Sep","Oct","Nov","Dec"};
    sprintf(dateStr, "%s, %d %s %d",
        days[t->tm_wday],
        t->tm_mday,
        months[t->tm_mon],
        1900 + t->tm_year);

    glColor3f(0.8f, 0.8f, 0.8f);
    glRasterPos2f(185, 95);
    s = dateStr;
    while(*s)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *s++);
}

void drawClockHands() {
    time_t now = time(0);
    struct tm* t = localtime(&now);

    int h = t->tm_hour % 12;
    int m = t->tm_min;
    int s = t->tm_sec;

    float secAngle  = 90 - s * 6;
    float minAngle  = 90 - m * 6 - s * 0.1f;
    float hourAngle = 90 - h * 30 - m * 0.5f;

    // Hour hand — thick white, small tail
    drawHand(hourAngle, radius*0.5f, 20, 1, 1, 1, 5);

    // Minute hand — medium white, small tail
    drawHand(minAngle, radius*0.75f, 25, 1, 1, 1, 3);

    // Second hand — thin red, longer tail
    drawHand(secAngle, radius*0.9f, 35, 1, 0, 0, 1);

    // Center dots
    glColor3f(0.2f, 0.2f, 0.2f);
    drawFilledCircle(cx, cy, 8);
    glColor3f(1, 0, 0);
    drawFilledCircle(cx, cy, 5);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawClockFace();
    drawHourMarkings();
    drawNumbers();
    drawClockHands();
    drawDigitalTime();
    glutSwapBuffers();
}

void timer(int val) {
    glutPostRedisplay();
    glutTimerFunc(50, timer, 0);
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
    glutTimerFunc(50, timer, 0);
    glutMainLoop();
}