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

float starX[80], starY[80], starSize[80];
float innerStarX[40], innerStarY[40], innerStarB[40], innerStarR[40];
bool  galaxyTheme  = true;
bool  use24h       = false;
bool  isFullscreen = false;
int   savedX, savedY, savedW = 600, savedH = 650;

void initStars() {
    srand(42);
    for (int i = 0; i < 80; i++) {
        starX[i]    = 20  + rand() % 560;
        starY[i]    = 120 + rand() % 500;
        starSize[i] = 0.5f + (rand() % 20) / 10.0f;
    }
    for (int i = 0; i < 40; i++) {
        float angle   = (rand() % 360) * PI / 180;
        float dist    = (float)(rand() % (int)(radius - 30));
        innerStarX[i] = cx + dist * cos(angle);
        innerStarY[i] = cy + dist * sin(angle);
        innerStarB[i] = 0.4f + (rand() % 6)  / 10.0f;
        innerStarR[i] = 0.8f + (rand() % 10) / 10.0f;
    }
}

// ── Bresenham ────────────────────────────────────────────────────────────────
void bresenhamLine(int x0, int y0, int x1, int y1) {
    int dx=abs(x1-x0), dy=abs(y1-y0);
    int sx=x0<x1?1:-1, sy=y0<y1?1:-1, err=dx-dy;
    while(true){
        glVertex2i(x0,y0);
        if(x0==x1&&y0==y1) break;
        int e2=2*err;
        if(e2>-dy){err-=dy;x0+=sx;}
        if(e2< dx){err+=dx;y0+=sy;}
    }
}

void bresenhamLineClipped(int x0,int y0,int x1,int y1,float clipR){
    int dx=abs(x1-x0),dy=abs(y1-y0);
    int sx=x0<x1?1:-1,sy=y0<y1?1:-1,err=dx-dy;
    while(true){
        float ddx=x0-cx,ddy=y0-cy;
        if(ddx*ddx+ddy*ddy<=clipR*clipR) glVertex2i(x0,y0);
        if(x0==x1&&y0==y1) break;
        int e2=2*err;
        if(e2>-dy){err-=dy;x0+=sx;}
        if(e2< dx){err+=dx;y0+=sy;}
    }
}

void drawBresenhamLine(int x0,int y0,int x1,int y1){
    glBegin(GL_POINTS); bresenhamLine(x0,y0,x1,y1); glEnd();
}

void drawCircle(float x,float y,float r){
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<360;i++){float t=i*PI/180; glVertex2f(x+r*cos(t),y+r*sin(t));}
    glEnd();
}

void drawFilledCircle(float x,float y,float r){
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x,y);
    for(int i=0;i<=360;i++){float t=i*PI/180; glVertex2f(x+r*cos(t),y+r*sin(t));}
    glEnd();
}

// ── Stars ────────────────────────────────────────────────────────────────────
void drawStars(){
    for(int i=0;i<80;i++){
        float dx=starX[i]-cx,dy=starY[i]-cy;
        if(sqrt(dx*dx+dy*dy)<radius+20) continue;
        float b=0.5f+starSize[i]*0.3f;
        glColor3f(b,b,b);
        drawFilledCircle(starX[i],starY[i],starSize[i]);
    }
}

// ── Gradient ring ────────────────────────────────────────────────────────────
void drawGradientRing(){
    for(int i=0;i<360;i++){
        float t=(float)i/360.0f, r,g,b;
        if(galaxyTheme){
            if     (t<0.25f){float p=t/0.25f;         r=0.5f+0.2f*p;g=0.0f+0.1f*p;b=0.8f+0.1f*p;}
            else if(t<0.5f) {float p=(t-0.25f)/0.25f; r=0.7f-0.4f*p;g=0.1f+0.3f*p;b=0.9f+0.1f*p;}
            else if(t<0.75f){float p=(t-0.5f)/0.25f;  r=0.3f+0.6f*p;g=0.4f-0.3f*p;b=1.0f-0.4f*p;}
            else            {float p=(t-0.75f)/0.25f;  r=0.9f-0.4f*p;g=0.1f-0.1f*p;b=0.6f+0.2f*p;}
        } else {
            if     (t<0.25f){float p=t/0.25f;         r=0.0f+0.1f*p;g=0.5f+0.4f*p;b=0.5f+0.4f*p;}
            else if(t<0.5f) {float p=(t-0.25f)/0.25f; r=0.1f+0.6f*p;g=0.9f+0.1f*p;b=0.9f+0.1f*p;}
            else if(t<0.75f){float p=(t-0.5f)/0.25f;  r=0.7f-0.7f*p;g=1.0f-0.5f*p;b=1.0f-0.5f*p;}
            else            {float p=(t-0.75f)/0.25f;  r=0.0f;       g=0.5f;       b=0.5f;}
        }
        glColor3f(r,g,b);
        float t1=i*PI/180,t2=(i+1)*PI/180;
        glBegin(GL_QUADS);
        glVertex2f(cx+(radius+3)*cos(t1), cy+(radius+3)*sin(t1));
        glVertex2f(cx+(radius+18)*cos(t1),cy+(radius+18)*sin(t1));
        glVertex2f(cx+(radius+18)*cos(t2),cy+(radius+18)*sin(t2));
        glVertex2f(cx+(radius+3)*cos(t2), cy+(radius+3)*sin(t2));
        glEnd();
    }
}

// ── Clock face ───────────────────────────────────────────────────────────────
void drawClockFace(){
    drawGradientRing();
    if(galaxyTheme){
        glColor3f(0.7f,0.4f,1.0f);
        drawCircle(cx,cy,radius+18); drawCircle(cx,cy,radius+3);
        for(int r=radius;r>0;r--){
            float t=(float)r/radius;
            glColor3f(0.02f+0.18f*t,0.0f+0.02f*t,0.05f+0.25f*t);
            drawFilledCircle(cx,cy,r);
        }
        for(int i=0;i<40;i++){
            glColor3f(innerStarB[i],innerStarB[i],innerStarB[i]+0.2f);
            drawFilledCircle(innerStarX[i],innerStarY[i],innerStarR[i]);
        }
        glColor3f(0.6f,0.3f,0.9f);  drawCircle(cx,cy,radius);
        glColor3f(0.3f,0.1f,0.5f);  drawCircle(cx,cy,radius-5);
        glColor3f(0.5f,0.2f,0.75f); drawCircle(cx,cy,radius-12);
        glColor3f(0.2f,0.05f,0.4f); drawCircle(cx,cy,radius-18);
    } else {
        glColor3f(0.6f,1.0f,1.0f);
        drawCircle(cx,cy,radius+18); drawCircle(cx,cy,radius+3);
        for(int r=radius;r>0;r--){
            float t=(float)r/radius;
            glColor3f(0.0f+0.05f*t,0.05f+0.45f*t,0.2f+0.6f*t);
            drawFilledCircle(cx,cy,r);
        }
        glColor3f(0.4f,0.9f,1.0f);  drawCircle(cx,cy,radius);
        glColor3f(0.1f,0.5f,0.6f);  drawCircle(cx,cy,radius-5);
        glColor3f(0.08f,0.4f,0.55f);drawCircle(cx,cy,radius-12);
        glColor3f(0.05f,0.3f,0.45f);drawCircle(cx,cy,radius-18);
    }
}

// ── Markings ─────────────────────────────────────────────────────────────────
void drawHourMarkings(){
    for(int i=0;i<12;i++){
        float angle=(90-i*30)*PI/180;
        int x1=(int)(cx+(radius-10)*cos(angle)),y1=(int)(cy+(radius-10)*sin(angle));
        int x2=(int)(cx+(radius-26)*cos(angle)),y2=(int)(cy+(radius-26)*sin(angle));
        glColor3f(galaxyTheme?0.85f:0.8f, galaxyTheme?0.7f:1.0f, 1.0f);
        drawBresenhamLine(x1,y1,x2,y2);
    }
    for(int i=0;i<60;i++){
        if(i%5==0) continue;
        float angle=(90-i*6)*PI/180;
        int x1=(int)(cx+(radius-10)*cos(angle)),y1=(int)(cy+(radius-10)*sin(angle));
        int x2=(int)(cx+(radius-16)*cos(angle)),y2=(int)(cy+(radius-16)*sin(angle));
        glColor3f(galaxyTheme?0.4f:0.3f, galaxyTheme?0.2f:0.6f, galaxyTheme?0.6f:0.7f);
        drawBresenhamLine(x1,y1,x2,y2);
    }
}

// ── Numbers ──────────────────────────────────────────────────────────────────
void drawNumbers(){
    const char* nums[]={"12","1","2","3","4","5","6","7","8","9","10","11"};
    const float offX[]={-10,-4,-4,-4,-4,-4,-4,-4,-4,-4,-8,-8};
    const float offY[]={ -6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6};
    for(int i=0;i<12;i++){
        float angle=(90-i*30)*PI/180, r=radius-40;
        float x=cx+r*cos(angle), y=cy+r*sin(angle);
        bool major=(i==0||i==3||i==6||i==9);
        if(major){
            glColor3f(galaxyTheme?0.9f:0.8f, galaxyTheme?0.75f:1.0f, 1.0f);
            glRasterPos2f(x+offX[i]-2,y+offY[i]);
            const char* s=nums[i];
            while(*s) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*s++);
        } else {
            glColor3f(galaxyTheme?0.7f:0.6f, galaxyTheme?0.55f:0.9f, galaxyTheme?0.9f:0.95f);
            glRasterPos2f(x+offX[i],y+offY[i]);
            const char* s=nums[i];
            while(*s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*s++);
        }
    }
}

// ── Hands ────────────────────────────────────────────────────────────────────
void drawHand(float angle,float length,float tailLength,
              float r,float g,float b,int thickness,bool clip){
    float rad=angle*PI/180;
    int x1=(int)cx,y1=(int)cy;
    int x2=(int)(cx+length*cos(rad)),y2=(int)(cy+length*sin(rad));
    int xt=(int)(cx-tailLength*cos(rad)),yt=(int)(cy-tailLength*sin(rad));
    float perpX=-sin(rad),perpY=cos(rad);
    int half=thickness/2;
    glColor3f(r,g,b);
    glBegin(GL_POINTS);
    for(int t=-half;t<=half;t++){
        int ox=(int)(t*perpX),oy=(int)(t*perpY);
        if(clip) bresenhamLineClipped(x1+ox,y1+oy,x2+ox,y2+oy,radius-2);
        else     bresenhamLine(x1+ox,y1+oy,x2+ox,y2+oy);
        bresenhamLine(x1+ox,y1+oy,xt+ox,yt+oy);
    }
    glEnd();
}

// ── Text shadow helper ───────────────────────────────────────────────────────
// Draws a string twice: dark offset shadow, then bright main text
void drawShadowText(float x, float y, const char* str,
                    void* font,
                    float tr, float tg, float tb) {
    // Shadow pass — dark, slightly offset
    glColor3f(0.0f, 0.0f, 0.05f);
    glRasterPos2f(x + 1.5f, y - 1.5f);
    const char* s = str;
    while (*s) glutBitmapCharacter(font, *s++);
    // Main text pass
    glColor3f(tr, tg, tb);
    glRasterPos2f(x, y);
    s = str;
    while (*s) glutBitmapCharacter(font, *s++);
}

// ── Inline timestamp — no boxes ───────────────────────────────────────────────
void drawInlineTimestamp(){
    time_t now=time(0);
    struct tm* t=localtime(&now);

    // ── Time — above center, ~2/3 up between center and 12 ──
    char timeStr[16];
    if(use24h){
        sprintf(timeStr,"%02d:%02d:%02d",t->tm_hour,t->tm_min,t->tm_sec);
    } else {
        int h12=t->tm_hour%12; if(h12==0)h12=12;
        const char* ap=t->tm_hour<12?"AM":"PM";
        sprintf(timeStr,"%02d:%02d %s",h12,t->tm_min,ap);
    }

    // Center the time string roughly (TIMES_ROMAN_24 ~13px/char)
    int tLen = 0; const char* tmp = timeStr; while(*tmp++) tLen++;
    float tW = tLen * 13.0f;
    float tX = cx - tW/2;
    float tY = cy + 68;

    if(galaxyTheme)
        drawShadowText(tX, tY, timeStr, GLUT_BITMAP_TIMES_ROMAN_24,
                       0.95f, 0.82f, 1.0f);
    else
        drawShadowText(tX, tY, timeStr, GLUT_BITMAP_TIMES_ROMAN_24,
                       0.8f, 1.0f, 1.0f);

    // ── Date — below center ──
    char dateStr[32];
    const char* days[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    const char* months[]={"Jan","Feb","Mar","Apr","May","Jun",
                          "Jul","Aug","Sep","Oct","Nov","Dec"};
    sprintf(dateStr,"%s, %d %s %d",
        days[t->tm_wday],t->tm_mday,months[t->tm_mon],1900+t->tm_year);

    int dLen=0; tmp=dateStr; while(*tmp++) dLen++;
    float dW = dLen * 7.5f;   // HELVETICA_12 ~7.5px/char
    float dX = cx - dW/2;
    float dY = cy - 100;

    if(galaxyTheme)
        drawShadowText(dX, dY, dateStr, GLUT_BITMAP_HELVETICA_12,
                       0.78f, 0.62f, 0.98f);
    else
        drawShadowText(dX, dY, dateStr, GLUT_BITMAP_HELVETICA_12,
                       0.6f, 0.9f, 1.0f);

    // ── Hint — bottom of window, always ──
    glColor3f(galaxyTheme?0.35f:0.25f, galaxyTheme?0.15f:0.45f,
              galaxyTheme?0.55f:0.6f);
    glRasterPos2f(68, 12);
    const char* hint="[G] Galaxy  [O] Ocean  [T] 12h/24h  [F] Fullscreen";
    while(*hint) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*hint++);
}

// ── Clock hands ──────────────────────────────────────────────────────────────
void drawClockHands(){
    time_t now=time(0);
    struct tm* t=localtime(&now);
    int h=t->tm_hour%12,m=t->tm_min,s=t->tm_sec;
    float secAngle =90-s*6;
    float minAngle =90-m*6  -s*0.1f;
    float hourAngle=90-h*30 -m*0.5f;

    if(galaxyTheme){
        drawHand(hourAngle,radius*0.5f, 20,0.9f,0.8f,1.0f,7,true);
        drawHand(minAngle, radius*0.75f,25,0.7f,0.6f,1.0f,4,true);
        drawHand(secAngle, radius*0.9f, 35,1.0f,0.3f,0.8f,1,true);
        glColor3f(0.05f,0.0f,0.1f); drawFilledCircle(cx,cy,10);
        glColor3f(0.7f,0.4f,1.0f);  drawFilledCircle(cx,cy,6);
        glColor3f(1.0f,0.3f,0.8f);  drawFilledCircle(cx,cy,3);
    } else {
        drawHand(hourAngle,radius*0.5f, 20,0.85f,1.0f,1.0f,7,true);
        drawHand(minAngle, radius*0.75f,25,0.5f,0.9f,1.0f,4,true);
        drawHand(secAngle, radius*0.9f, 35,0.0f,0.9f,0.8f,1,true);
        glColor3f(0.0f,0.08f,0.15f); drawFilledCircle(cx,cy,10);
        glColor3f(0.4f,0.9f,1.0f);   drawFilledCircle(cx,cy,6);
        glColor3f(0.0f,0.9f,0.8f);   drawFilledCircle(cx,cy,3);
    }
}

// ── Display ──────────────────────────────────────────────────────────────────
void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    if(galaxyTheme) drawStars();
    drawClockFace();
    drawHourMarkings();
    drawNumbers();
    drawClockHands();
    drawInlineTimestamp();
    glutSwapBuffers();
}

// ── Reshape — fixes fullscreen squash ────────────────────────────────────────
void reshape(int w, int h){
    winW = w; winH = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);

    // Recentre clock and scale radius to fit window
    cx = w / 2.0f;
    cy = h / 2.0f + 20.0f;          // slight upward offset for hint bar
    float margin = 60.0f;
    float maxR = (min(w, h) / 2.0f) - margin;
    radius = (maxR < 220) ? maxR : 220.0f;

    // Recompute inner stars relative to new cx/cy
    srand(42);
    // burn through outer star randoms first
    for(int i=0;i<80;i++){rand();rand();rand();}
    for(int i=0;i<40;i++){
        float angle=(rand()%360)*PI/180;
        float dist=(float)(rand()%(int)(radius-30));
        innerStarX[i]=cx+dist*cos(angle);
        innerStarY[i]=cy+dist*sin(angle);
        innerStarB[i]=0.4f+(rand()%6)/10.0f;
        innerStarR[i]=0.8f+(rand()%10)/10.0f;
    }
}

// ── Keyboard ─────────────────────────────────────────────────────────────────
void keyboard(unsigned char key,int x,int y){
    switch(key){
        case 'g':case 'G':
            galaxyTheme=true;
            glClearColor(0.0f,0.0f,0.04f,1); break;
        case 'o':case 'O':
            galaxyTheme=false;
            glClearColor(0.0f,0.04f,0.08f,1); break;
        case 't':case 'T':
            use24h=!use24h; break;
        case 'f':case 'F':
            if(!isFullscreen){
                savedX=glutGet(GLUT_WINDOW_X);
                savedY=glutGet(GLUT_WINDOW_Y);
                savedW=glutGet(GLUT_WINDOW_WIDTH);
                savedH=glutGet(GLUT_WINDOW_HEIGHT);
                glutFullScreen();
                isFullscreen=true;
            } else {
                glutReshapeWindow(savedW,savedH);
                glutPositionWindow(savedX,savedY);
                isFullscreen=false;
            }
            break;
        case 27:   // ESC also exits fullscreen
            if(isFullscreen){
                glutReshapeWindow(savedW,savedH);
                glutPositionWindow(savedX,savedY);
                isFullscreen=false;
            }
            break;
    }
    glutPostRedisplay();
}

void timer(int val){
    glutPostRedisplay();
    glutTimerFunc(50,timer,0);
}

void init(){
    glClearColor(0.0f,0.0f,0.04f,1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,winW,0,winH);
    initStars();
}

int main(int argc,char** argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(winW,winH);
    glutCreateWindow("Interactive Graphics Clock");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);      // <-- this is what fixes fullscreen
    glutKeyboardFunc(keyboard);
    glutTimerFunc(50,timer,0);
    glutMainLoop();
}