#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <cstdio>
#include <cstdlib>
using namespace std;

const float PI = 3.14159265f;
int winW = 600, winH = 650;
float cx = 300, cy = 330;
float radius = 220;

// Stars
float starX[80], starY[80], starSize[80];

// Inner clock face stars (pre-computed, fix #3)
float innerStarX[40], innerStarY[40], innerStarB[40], innerStarR[40];

// Theme toggle (fix #5)
bool galaxyTheme = true;

void initStars() {
    srand(42);
    for (int i = 0; i < 80; i++) {
        starX[i] = 20 + rand() % 560;
        starY[i] = 120 + rand() % 500;
        starSize[i] = 0.5f + (rand() % 20) / 10.0f;
    }
    // Pre-compute inner stars
    for (int i = 0; i < 40; i++) {
        float angle = (rand() % 360) * PI / 180;
        float dist  = (rand() % (int)(radius - 30));
        innerStarX[i] = cx + dist * cos(angle);
        innerStarY[i] = cy + dist * sin(angle);
        innerStarB[i] = 0.4f + (rand() % 6) / 10.0f;
        innerStarR[i] = 0.8f + (rand() % 10) / 10.0f;
    }
}

// ── Bresenham with circular clipping (fix #1) ──────────────────────────────
void bresenhamLineClipped(int x0, int y0, int x1, int y1, float clipR) {
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    while (true) {
        float ddx = x0 - cx, ddy = y0 - cy;
        if (ddx*ddx + ddy*ddy <= clipR*clipR)
            glVertex2i(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}

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
        if (e2 <  dx) { err += dx; y0 += sy; }
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

void drawStars() {
    for (int i = 0; i < 80; i++) {
        float dx = starX[i] - cx, dy = starY[i] - cy;
        if (sqrt(dx*dx + dy*dy) < radius + 20) continue;
        float b = 0.5f + starSize[i] * 0.3f;
        glColor3f(b, b, b);
        drawFilledCircle(starX[i], starY[i], starSize[i]);
    }
}

// ── Ocean gradient ring ─────────────────────────────────────────────────────
void drawOceanRing() {
    for (int i = 0; i < 360; i++) {
        float t = (float)i / 360.0f;
        float r, g, b;
        if (t < 0.25f)      { float p=t/0.25f;       r=0.0f+0.1f*p; g=0.5f+0.4f*p; b=0.5f+0.4f*p; }
        else if (t < 0.5f)  { float p=(t-0.25f)/0.25f; r=0.1f+0.6f*p; g=0.9f+0.1f*p; b=0.9f+0.1f*p; }
        else if (t < 0.75f) { float p=(t-0.5f)/0.25f;  r=0.7f-0.7f*p; g=1.0f-0.5f*p; b=1.0f-0.5f*p; }
        else                { float p=(t-0.75f)/0.25f;  r=0.0f;        g=0.5f;        b=0.5f; }
        glColor3f(r, g, b);
        float t1 = i * PI / 180, t2 = (i+1) * PI / 180;
        glBegin(GL_QUADS);
        glVertex2f(cx+(radius+3)*cos(t1),  cy+(radius+3)*sin(t1));
        glVertex2f(cx+(radius+18)*cos(t1), cy+(radius+18)*sin(t1));
        glVertex2f(cx+(radius+18)*cos(t2), cy+(radius+18)*sin(t2));
        glVertex2f(cx+(radius+3)*cos(t2),  cy+(radius+3)*sin(t2));
        glEnd();
    }
}

// ── Galaxy gradient ring ────────────────────────────────────────────────────
void drawGalaxyRing() {
    for (int i = 0; i < 360; i++) {
        float t = (float)i / 360.0f;
        float r, g, b;
        if (t < 0.25f)      { float p=t/0.25f;         r=0.5f+0.2f*p; g=0.0f+0.1f*p; b=0.8f+0.1f*p; }
        else if (t < 0.5f)  { float p=(t-0.25f)/0.25f; r=0.7f-0.4f*p; g=0.1f+0.3f*p; b=0.9f+0.1f*p; }
        else if (t < 0.75f) { float p=(t-0.5f)/0.25f;  r=0.3f+0.6f*p; g=0.4f-0.3f*p; b=1.0f-0.4f*p; }
        else                { float p=(t-0.75f)/0.25f;  r=0.9f-0.4f*p; g=0.1f-0.1f*p; b=0.6f+0.2f*p; }
        glColor3f(r, g, b);
        float t1 = i * PI / 180, t2 = (i+1) * PI / 180;
        glBegin(GL_QUADS);
        glVertex2f(cx+(radius+3)*cos(t1),  cy+(radius+3)*sin(t1));
        glVertex2f(cx+(radius+18)*cos(t1), cy+(radius+18)*sin(t1));
        glVertex2f(cx+(radius+18)*cos(t2), cy+(radius+18)*sin(t2));
        glVertex2f(cx+(radius+3)*cos(t2),  cy+(radius+3)*sin(t2));
        glEnd();
    }
}

void drawClockFace() {
    if (galaxyTheme) {
        drawGalaxyRing();
        glColor3f(0.7f, 0.4f, 1.0f);
        drawCircle(cx, cy, radius + 18);
        drawCircle(cx, cy, radius + 3);

        for (int r = radius; r > 0; r--) {
            float t = (float)r / radius;
            glColor3f(0.02f+0.18f*t, 0.0f+0.02f*t, 0.05f+0.25f*t);
            drawFilledCircle(cx, cy, r);
        }

        // Pre-computed inner stars (fix #3)
        for (int i = 0; i < 40; i++) {
            glColor3f(innerStarB[i], innerStarB[i], innerStarB[i] + 0.2f);
            drawFilledCircle(innerStarX[i], innerStarY[i], innerStarR[i]);
        }

        glColor3f(0.6f, 0.3f, 0.9f);
        drawCircle(cx, cy, radius);
        glColor3f(0.3f, 0.1f, 0.5f);
        drawCircle(cx, cy, radius - 5);
        // Two extra concentric circles for more boundary detail
        glColor3f(0.5f, 0.2f, 0.75f);
        drawCircle(cx, cy, radius - 12);
        glColor3f(0.2f, 0.05f, 0.4f);
        drawCircle(cx, cy, radius - 18);

    } else {
        // Ocean theme face
        drawOceanRing();
        glColor3f(0.6f, 1.0f, 1.0f);
        drawCircle(cx, cy, radius + 18);
        drawCircle(cx, cy, radius + 3);

        for (int r = radius; r > 0; r--) {
            float t = (float)r / radius;
            glColor3f(0.0f+0.05f*t, 0.05f+0.45f*t, 0.2f+0.6f*t);
            drawFilledCircle(cx, cy, r);
        }

        glColor3f(0.4f, 0.9f, 1.0f);
        drawCircle(cx, cy, radius);
        glColor3f(0.1f, 0.5f, 0.6f);
        drawCircle(cx, cy, radius - 5);
        glColor3f(0.08f, 0.4f, 0.55f);
        drawCircle(cx, cy, radius - 12);
        glColor3f(0.05f, 0.3f, 0.45f);
        drawCircle(cx, cy, radius - 18);
    }
}

void drawHourMarkings() {
    for (int i = 0; i < 12; i++) {
        float angle = (90 - i * 30) * PI / 180;
        float x1 = cx + (radius - 10) * cos(angle);
        float y1 = cy + (radius - 10) * sin(angle);
        float x2 = cx + (radius - 26) * cos(angle);
        float y2 = cy + (radius - 26) * sin(angle);
        if (galaxyTheme) glColor3f(0.85f, 0.7f, 1.0f);
        else             glColor3f(0.8f,  1.0f, 1.0f);
        drawBresenhamLine((int)x1, (int)y1, (int)x2, (int)y2);
    }
    for (int i = 0; i < 60; i++) {
        if (i % 5 == 0) continue;
        float angle = (90 - i * 6) * PI / 180;
        float x1 = cx + (radius - 10) * cos(angle);
        float y1 = cy + (radius - 10) * sin(angle);
        float x2 = cx + (radius - 16) * cos(angle);
        float y2 = cy + (radius - 16) * sin(angle);
        if (galaxyTheme) glColor3f(0.4f, 0.2f, 0.6f);
        else             glColor3f(0.3f, 0.6f, 0.7f);
        drawBresenhamLine((int)x1, (int)y1, (int)x2, (int)y2);
    }
}

// ── Number drawing with proper centering (fix #2) ──────────────────────────
void drawNumbers() {
    // Widths: 1-digit ~8px (Helvetica12), 2-digit ~14px; ROMAN24 ~1.5x
    const char* nums[] = {"12","1","2","3","4","5","6","7","8","9","10","11"};
    // offsets to visually center each label
    const float offX[] = {-10,-4,-4,-4,-4,-4,-4,-4,-4,-4,-8,-8};
    const float offY[] = { -6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6};

    for (int i = 0; i < 12; i++) {
        float angle = (90 - i * 30) * PI / 180;
        float r = radius - 40;
        float x = cx + r * cos(angle);
        float y = cy + r * sin(angle);

        bool isMajor = (i == 0 || i == 3 || i == 6 || i == 9);
        if (isMajor) {
            if (galaxyTheme) glColor3f(0.9f, 0.75f, 1.0f);
            else             glColor3f(0.8f, 1.0f,  1.0f);
            glRasterPos2f(x + offX[i] - 2, y + offY[i]);
            const char* s = nums[i];
            while (*s) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *s++);
        } else {
            if (galaxyTheme) glColor3f(0.7f, 0.55f, 0.9f);
            else             glColor3f(0.6f, 0.9f,  0.95f);
            glRasterPos2f(x + offX[i], y + offY[i]);
            const char* s = nums[i];
            while (*s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *s++);
        }
    }
}

// ── Hand drawing with perpendicular offset thickness + clipping (fix #1 #4) ─
void drawHand(float angle, float length, float tailLength,
              float r, float g, float b, int thickness, bool clip) {
    float rad = angle * PI / 180;
    int x1 = (int)cx, y1 = (int)cy;
    int x2 = (int)(cx + length * cos(rad));
    int y2 = (int)(cy + length * sin(rad));
    int xt = (int)(cx - tailLength * cos(rad));
    int yt = (int)(cy - tailLength * sin(rad));

    // Perpendicular direction for clean thick lines (fix #4)
    float perpX = -sin(rad);
    float perpY =  cos(rad);
    int half = thickness / 2;

    glColor3f(r, g, b);
    glBegin(GL_POINTS);
    for (int t = -half; t <= half; t++) {
        int ox = (int)(t * perpX);
        int oy = (int)(t * perpY);
        if (clip)
            bresenhamLineClipped(x1+ox, y1+oy, x2+ox, y2+oy, radius - 2);
        else
            bresenhamLine(x1+ox, y1+oy, x2+ox, y2+oy);
        // tail always unclipped (stays inside anyway)
        bresenhamLine(x1+ox, y1+oy, xt+ox, yt+oy);
    }
    glEnd();
}

void drawClockHands() {
    time_t now = time(0);
    struct tm* t = localtime(&now);

    int h = t->tm_hour % 12;
    int m = t->tm_min;
    int s = t->tm_sec;

    float secAngle  = 90 - s * 6;
    float minAngle  = 90 - m * 6  - s * 0.1f;
    float hourAngle = 90 - h * 30 - m * 0.5f;

    if (galaxyTheme) {
        drawHand(hourAngle, radius*0.5f,  20, 0.9f, 0.8f, 1.0f, 7, true);
        drawHand(minAngle,  radius*0.75f, 25, 0.7f, 0.6f, 1.0f, 4, true);
        drawHand(secAngle,  radius*0.9f,  35, 1.0f, 0.3f, 0.8f, 1, true);

        glColor3f(0.05f, 0.0f, 0.1f);  drawFilledCircle(cx, cy, 10);
        glColor3f(0.7f,  0.4f, 1.0f);  drawFilledCircle(cx, cy, 6);
        glColor3f(1.0f,  0.3f, 0.8f);  drawFilledCircle(cx, cy, 3);
    } else {
        drawHand(hourAngle, radius*0.5f,  20, 0.85f, 1.0f, 1.0f, 7, true);
        drawHand(minAngle,  radius*0.75f, 25, 0.5f,  0.9f, 1.0f, 4, true);
        drawHand(secAngle,  radius*0.9f,  35, 0.0f,  0.9f, 0.8f, 1, true);

        glColor3f(0.0f,  0.08f, 0.15f); drawFilledCircle(cx, cy, 10);
        glColor3f(0.4f,  0.9f,  1.0f);  drawFilledCircle(cx, cy, 6);
        glColor3f(0.0f,  0.9f,  0.8f);  drawFilledCircle(cx, cy, 3);
    }
}

void drawDigitalTime() {
    time_t now = time(0);
    struct tm* t = localtime(&now);

    if (galaxyTheme) {
        glColor3f(0.03f, 0.0f, 0.08f);
    } else {
        glColor3f(0.0f, 0.05f, 0.1f);
    }
    glBegin(GL_QUADS);
    glVertex2f(155,50); glVertex2f(445,50);
    glVertex2f(445,115); glVertex2f(155,115);
    glEnd();

    if (galaxyTheme) glColor3f(0.7f, 0.4f, 1.0f);
    else             glColor3f(0.4f, 0.9f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(155,50); glVertex2f(445,50);
    glVertex2f(445,115); glVertex2f(155,115);
    glEnd();

    if (galaxyTheme) glColor3f(0.3f, 0.1f, 0.5f);
    else             glColor3f(0.1f, 0.5f, 0.6f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(159,54); glVertex2f(441,54);
    glVertex2f(441,111); glVertex2f(159,111);
    glEnd();

    char timeStr[20];
    sprintf(timeStr, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
    if (galaxyTheme) glColor3f(0.85f, 0.7f, 1.0f);
    else             glColor3f(0.7f,  1.0f, 1.0f);
    glRasterPos2f(180, 72);
    const char* s = timeStr;
    while (*s) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *s++);

    char dateStr[30];
    const char* days[]   = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    const char* months[] = {"Jan","Feb","Mar","Apr","May","Jun",
                            "Jul","Aug","Sep","Oct","Nov","Dec"};
    sprintf(dateStr, "%s, %d %s %d",
        days[t->tm_wday], t->tm_mday, months[t->tm_mon], 1900+t->tm_year);
    if (galaxyTheme) glColor3f(0.65f, 0.5f, 0.85f);
    else             glColor3f(0.5f,  0.8f, 0.9f);
    glRasterPos2f(180, 92);
    s = dateStr;
    while (*s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *s++);

    // Theme hint label
    if (galaxyTheme) glColor3f(0.5f, 0.3f, 0.7f);
    else             glColor3f(0.3f, 0.6f, 0.7f);
    glRasterPos2f(180, 57);
    const char* hint = galaxyTheme ? "[G] Galaxy  [O] Ocean" : "[G] Galaxy  [O] Ocean";
    while (*hint) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *hint++);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (!galaxyTheme) {
        // Ocean: plain dark background
    } else {
        drawStars();
    }
    drawClockFace();
    drawHourMarkings();
    drawNumbers();
    drawClockHands();
    drawDigitalTime();
    glutSwapBuffers();
}

// ── Keyboard interaction (fix #5) ──────────────────────────────────────────
void keyboard(unsigned char key, int x, int y) {
    if (key == 'g' || key == 'G') {
        galaxyTheme = true;
        glClearColor(0.0f, 0.0f, 0.04f, 1);
        glutPostRedisplay();
    } else if (key == 'o' || key == 'O') {
        galaxyTheme = false;
        glClearColor(0.0f, 0.04f, 0.08f, 1);
        glutPostRedisplay();
    }
}

void timer(int val) {
    glutPostRedisplay();
    glutTimerFunc(50, timer, 0);
}

void init() {
    glClearColor(0.0f, 0.0f, 0.04f, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, winW, 0, winH);
    initStars();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Interactive Graphics Clock - Galaxy / Ocean");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);   // interactivity registered here
    glutTimerFunc(50, timer, 0);
    glutMainLoop();
}