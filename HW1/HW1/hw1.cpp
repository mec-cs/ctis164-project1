/*********
   CTIS164 - Template Source Program
----------
STUDENT : Mehmet Enes Çakýr
SECTION : 01
HOMEWORK: HOMEWORK 1
----------
PROBLEMS: There is no problem about the mechanism
----------
ADDITIONAL FEATURES: * 3D effects of lake (shade), mountain (shade) and also ground (by cheesy grasses), 
                     * Effects on the axe (color transition on the wooden part)
                     * Color transition on the ground (dark to the light green)
                     * Random speed for each axes
                     * Show user how many axes they put
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  10 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

// number of clicks counted, >MAX 10
int click_count = 0;

// store the locations
int locationX_array[10];
int locationY_array[10];

// check whether or not F1 key is pressed
bool move = false;

// random number array
int random_array[10];

// generattes random array to be used for random speed
void generateRandomArray() {
    int i;

    for (i = 0; i < 10; i++) {
        int num = (rand() % (3 - 2 + 1)) + 2;
        random_array[i] = num;
    }
}


//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}

// add (few transparent) clouds in different positions
void cloud() {

    glColor4f(1, 1, 1, 0.90);
    circle(0, 175, 30);
    glColor4f(1, 1, 1, 0.90);
    circle(0, 165, 30);
    glColor4f(1, 1, 1, 0.90);
    circle(30, 165, 30);
    glColor4f(1, 1, 1, 0.90);
    circle(-30, 165, 30);

    glColor4f(1, 1, 1, 0.90);
    circle(300, 170, 30);
    glColor4f(1, 1, 1, 0.90);
    circle(300, 160, 30);
    glColor4f(1, 1, 1, 0.90);
    circle(330, 160, 30);
    glColor4f(1, 1, 1, 0.90);
    circle(270, 160, 30);

    glColor4f(1, 1, 1, 0.90);
    circle(-300, 170, 30);
    glColor4f(1, 1, 1, 0.90);
    circle(-300, 160, 30);
    glColor4f(1, 1, 1, 0.90);
    circle(-270, 160, 30);
    glColor4f(1, 1, 1, 0.90);
    circle(-330, 160, 30);

    glColor4f(1, 1, 1, 0.90);
    circle(-150, 190, 30);
    glColor4f(1, 1, 1, 0.90);
    circle(-150, 180, 30);
    glColor4f(1, 1, 1, 0.90);
    circle(-120, 180, 30);
    glColor4f(1, 1, 1, 0.90);
    circle(-180, 180, 30);

    glColor4f(1, 1, 1, 0.95);
    circle(150, 190, 30);
    glColor4f(1, 1, 1, 0.90);
    circle(150, 180, 30);
    glColor4f(1, 1, 1, 0.90);
    circle(180, 180, 30);
    glColor4f(1, 1, 1, 0.90);
    circle(120, 180, 30);
}

void display_object(int x, int y) {
    
    // wooden part of axe
    glColor3ub(160, 82, 45);
    glRectf(x, y, x + 20, y + 75);

    glColor3ub(160, 82, 45);
    glRectf(x - 10, y + 75, x + 30, y + 90);

    glBegin(GL_TRIANGLES);
    glColor3ub(160, 82, 45);
    glVertex2f(x + 10, y + 105);
    glVertex2f(x + 25, y + 75);
    glVertex2f(x - 5, y + 75);
    glEnd();
    
    glBegin(GL_TRIANGLES);
    glColor3ub(160, 82, 45);
    glVertex2f(x + 25, y + 82);
    glVertex2f(x + 35, y + 65);
    glVertex2f(x + 35, y + 100);
    glEnd();


    // metal part of axe
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(219/255., 226/255., 233/255.);
    glVertex2f(x + 30, y + 82);
    glVertex2f(x + 30, y + 105);
    glVertex2f(x + 40, y + 95);
    glVertex2f(x + 43, y + 88);
    glVertex2f(x + 43, y + 76);
    glVertex2f(x + 40, y + 69);
    glVertex2f(x + 30, y + 59);
    glEnd();


    // shade of the metal part of axe
    glBegin(GL_LINE_STRIP);
    glColor3f(99 /255., 102 /255., 106 /255.);
    glVertex2f(x + 38, y + 80);
    glVertex2f(x + 40, y + 86);
    glVertex2f(x + 39, y + 92);
    glVertex2f(x + 40, y + 80);
    glVertex2f(x + 40, y + 75);
    glVertex2f(x + 38, y + 70);
    glVertex2f(x + 39, y + 95);
    glEnd();


    // boundary of wooden part of axe
    glBegin(GL_LINE_LOOP);
    glColor3ub(160, 110, 80);
    glVertex2f(x, y);
    glVertex2f(x + 20, y);
    glVertex2f(x + 20, y + 75);
    glVertex2f(x + 30, y + 75);
    glVertex2f(x + 30, y + 90);
    glVertex2f(x + 19, y + 90);
    glVertex2f(x + 10, y + 105);
    glVertex2f(x + 1, y + 90);
    glVertex2f(x - 10, y + 90);
    glVertex2f(x  - 10, y + 75);
    glVertex2f(x, y + 75);
    glEnd();

    // boundary of metal part of axe
    glBegin(GL_LINE_LOOP);
    glColor3ub(83, 86, 91);
    glVertex2f(x + 30, y + 82);
    glVertex2f(x + 30, y + 105);
    glVertex2f(x + 40, y + 95);
    glVertex2f(x + 43, y + 88);
    glVertex2f(x + 43, y + 76);
    glVertex2f(x + 40, y + 69);
    glVertex2f(x + 30, y + 59);
    glEnd();


    // shadow of the wooden part of axe
    glBegin(GL_LINES);
    glColor3ub(130, 82, 45);
    glVertex2f(x + 3, y);
    glColor3ub(133, 82, 45);
    glVertex2f(x + 3, y + 75);
    glColor3ub(136, 82, 45);
    glVertex2f(x + 6, y);
    glColor3ub(139, 82, 45);
    glVertex2f(x + 6, y + 75);
    glColor3ub(142, 82, 45);
    glVertex2f(x + 9, y);
    glColor3ub(145, 82, 45);
    glVertex2f(x + 9, y + 75);
    glColor3ub(148, 82, 45);
    glVertex2f(x + 12, y);
    glColor3ub(152, 82, 45);
    glVertex2f(x + 12, y + 75);
    glColor3ub(156, 82, 45);
    glVertex2f(x + 14, y);
    glColor3ub(160, 82, 45);
    glVertex2f(x + 14, y + 75);
    glEnd();


    // joint part of axe
    glColor3ub(137, 142, 140);
    glRectf(x, y + 75, x + 20, y + 90);
    glColor3ub(83, 86, 91);
    circle(x + 10, y + 82, 3);
}



// To display onto window using OpenGL commands
void display() {

    // clear window to white
    glClearColor(1, 1, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // informative texts
    glColor3f(0, 0, 0);
    vprint(-385, 280, GLUT_BITMAP_8_BY_13, "Mehmet Enes CAKIR");
    vprint(-360, 260, GLUT_BITMAP_8_BY_13, "22103869");
    vprint(-195, 280, GLUT_BITMAP_9_BY_15, "- Homework 1 -");


    // main parts
    glColor3ub(140, 203, 255);
    glRectf(-380, -270, 380, 240);

    // set colors as variables so as to prevent time waste
    int r_dark = 113, g_dark = 169, b_dark = 44;
    int r_light = 196, g_light = 211, b_light = 0;
    

    // color transmission on the ground by using triangles (2 triangle = 1 rectangle)
    
    glBegin(GL_TRIANGLES);

    glColor3ub(r_dark, g_dark, b_dark);
    glVertex2f(-380, -120);
    glColor3ub(r_light, g_light, b_light);
    glVertex2f(-380, -270);
    glColor3ub(r_dark, g_dark, b_dark);
    glVertex2f(+380, -120);

    glColor3ub(r_light, g_light, b_light);
    glVertex2f(-380, -270);
    glColor3ub(r_light, g_light, b_light);
    glVertex2f(+380, -270);
    glColor3ub(r_dark, g_dark, b_dark);
    glVertex2f(+380, -120);

    glEnd();


    // add cloud
    cloud();


    // little mountain
    glBegin(GL_TRIANGLES);

    glColor3ub(177, 201, 232);
    glVertex2f(170, -120);
    glVertex2f(-170, -120);
    glVertex2f(0, 50);

    // shadow of the little mountain
    // to prevent conflict between mountains

    glColor3ub(170, 199, 228);
    glVertex2f(0, 50);
    glVertex2f(170, -120);
    glVertex2f(100, -120);


    // other two big mountains
    glColor3ub(140, 146, 172);
    glVertex2f(-380, -120);
    glVertex2f(-40, -120);
    glVertex2f(-210, 80);
    
    glColor3ub(140, 146, 172);
    glVertex2f(+380, -120);
    glVertex2f(40, -120);
    glVertex2f(210, 80);
    
    glEnd();


    // big mountains' shadows
    glBegin(GL_TRIANGLES);

    glColor3ub(130, 143, 170);
    glVertex2f(-100, -120);
    glVertex2f(-40, -120);
    glVertex2f(-210, 80);

    glColor3ub(130, 143, 170);
    glVertex2f(380, -120);
    glVertex2f(320, -120);
    glVertex2f(210, 80);

    glEnd();

    // snow on top of the all mountains
    glBegin(GL_POLYGON);
    
    glColor3ub(255, 255, 255);
    glVertex2f(0, 50);
    glVertex2f(14, 40);
    glVertex2f(14, 35);
    glVertex2f(0, 37);
    glVertex2f(-14, 35);
    glVertex2f(-14, 40);
    
    glEnd();

    glBegin(GL_POLYGON);
    
    glColor3ub(255, 255, 255);
    glVertex2f(-210, 80);
    glVertex2f(-261, 20);
    glVertex2f(-230, 30);
    glVertex2f(-210, 57);
    glVertex2f(-230, 30);
    glVertex2f(-159, 20);
   
    glEnd();

    glBegin(GL_POLYGON);
    
    glColor3ub(255, 255, 255);
    glVertex2f(210, 80);
    glVertex2f(261, 20);
    glVertex2f(230, 30);
    glVertex2f(210, 37);
    glVertex2f(230, 30);
    glVertex2f(159, 20);
    
    glEnd();


    // lake 
    glColor3f(123/255., 178/255., 221/255.);
    glRectf(-380, -240, -120, -270);
    glRectf(-120, -235, -80, -270);
    glRectf(-80, -230, -40, -270);
    glRectf(-40, -225, 0, -270);
    glRectf(-0, -220, +40, -270);
    glRectf(+40, -215, 100, -270);
    glRectf(+100, -215, +380, -270);


    // 3D effects of lake
    glLineWidth(5);
    glColor3ub(70, 130, 180);
    glBegin(GL_LINES);

    glVertex2f(-380, -243);
    glVertex2f(-108, -243);

    glVertex2f(-65, -238);
    glVertex2f(-123, -238);

    glVertex2f(-80, -233);
    glVertex2f(-28, -233);

    glVertex2f(-40, -228);
    glVertex2f(12, -228);

    glVertex2f(0, -223);
    glVertex2f(52, -223);

    glVertex2f(40, -218);
    glVertex2f(100, -218);

    glVertex2f(100, -218);
    glVertex2f(380, -218);

    glEnd();


    // shore of lake (3D effect)
    glColor3f(129/255., 97/255., 62/255.);
    glRectf(-380, -233, -120, -240);
    
    glColor3f(112/255., 66/255., 20/255.);
    glRectf(-120, -228, -80, -235);
    glRectf(-80, -230, -40, -222);
    glRectf(-40, -218, 0, -225);
    glRectf(-0, -220, +40, -213);

    glColor3f(129 / 255., 97 / 255., 62 / 255.);
    glRectf(+40, -215, 100, -208);
    glRectf(+100, -215, +380, -208);


    // line of shores (3D effect)
    glLineWidth(3);
    glColor3ub(50, 26, 24);
    glBegin(GL_LINES);
    
    glVertex2f(-380, -233);
    glVertex2f(-120, -233);
    
    glVertex2f(-80, -228);
    glVertex2f(-120, -228);
    
    glVertex2f(-80, -222);
    glVertex2f(-40, -222);
    
    glVertex2f(-40, -218);
    glVertex2f(0, -218);

    glVertex2f(0, -213);
    glVertex2f(40, -213);

    glVertex2f(40, -208);
    glVertex2f(100, -208);

    glVertex2f(100, -208);
    glVertex2f(380, -208);

    glEnd();


    // lines boundaring the main parts
    glLineWidth(3);
    glColor3ub(64, 64, 64);
    glBegin(GL_LINES);
    glVertex2f(-380, -271);
    glVertex2f(380, -271);
    glEnd();
    
    glLineWidth(3);
    glColor3ub(64, 64, 64);
    glBegin(GL_LINES);
    glVertex2f(-380, -271);
    glVertex2f(-380, +241);
    glEnd();

    glLineWidth(3);
    glColor3ub(64, 64, 64);
    glBegin(GL_LINES);
    glVertex2f(-381, 241);
    glVertex2f(380, 241);
    glEnd();

    glLineWidth(3);
    glColor3ub(64, 64, 64);
    glBegin(GL_LINES);
    glVertex2f(380, 241);
    glVertex2f(380, -271);
    glEnd();

    glLineWidth(3);
    glColor3ub(64, 64, 64);
    glBegin(GL_LINES);
    glVertex2f(-380, -119);
    glVertex2f(+380, -119);
    glEnd();
    

    // some grass to build 3D perspective
    glLineWidth(3);
    glColor3ub(205, 150, 0);
    glBegin(GL_LINES);
    
    glVertex2f(0, -150);
    glVertex2f(0, -135);
    
    
    glVertex2f(80, -130);
    glVertex2f(80, -142);

    glVertex2f(190, -170);
    glVertex2f(190, -200);

    glVertex2f(-130, -160);
    glVertex2f(-130, -187);

    glVertex2f(-290, -187);
    glVertex2f(-290, -227);

    glVertex2f(-210, -128);
    glVertex2f(-210, -139);

    glVertex2f(280, -130);
    glVertex2f(280, -138);
    
    glEnd();


    // strings to be shown while they are not moving
    if (!move) {
        glColor3f(0, 0, 0);
        vprint(70, 260, GLUT_BITMAP_8_BY_13, "Number of objects you created: %d", click_count);
        vprint(0, 280, GLUT_BITMAP_8_BY_13, "Click on the screen to create an object (MAX 10)");
        vprint(-215, 260, GLUT_BITMAP_8_BY_13, "Press F1 to move them!");
    }
    // while objects are moving, it will be printed
    else {
        glColor3f(0, 0, 0);
        vprint(-65, 280, GLUT_BITMAP_8_BY_13, "Objects are moving to the nearest side, Can't create now!");
    }
    

    // for each loop this count variable checks whether objects are in the limited borders or not
    bool check = false;

    // while in the limited borders, they are being displayed 
    for (int i = 0; i < click_count; i++)
    {       
            // if they pass the boundaries, they wont be displayed, and check will be false bc the main display func is executed again 
            if (locationX_array[i] > -370 && locationX_array[i] < 370) {
                display_object(locationX_array[i], locationY_array[i]);
                check = true;
            }
    }
    
    // whenever the objects pass the boundaries and check returns false, move will be false and counts of clicks will be 0.
    if (check == false) {
        click_count = 0;
        move = false;
    }

    glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: 
        up = true; 
        break;
    case GLUT_KEY_DOWN:
        down = true; 
        break;
    case GLUT_KEY_LEFT: 
        left = true; 
        break;
    case GLUT_KEY_RIGHT: 
        right = true; 
        break;
    // if user press F1, move is true so objects will move
    case GLUT_KEY_F1:
        move = true;
        break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: 
        up = false; 
        break;
    case GLUT_KEY_DOWN: 
        down = false; 
        break;
    case GLUT_KEY_LEFT: 
        left = false; 
        break;
    case GLUT_KEY_RIGHT: 
        right = false; 
        break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
    // if they are not moving, user can add new objects
    if (!move) {
        // if user clicks and if the number of objects is not more than 10, it will new create object
        if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {
            if (click_count < 10) {
                
                // GLUT / openGL coordinate system conversion
                int y_loc = winHeight / 2 - y;
                int x_loc = x - winWidth / 2;

                // if the location is in the limited boundaries, it will be stored in array to be used in display function
                // and clicks will be counted
                if (x_loc < 360 && x_loc > -370) {
                    if (y_loc < 160 && y_loc > -270) {
                        locationX_array[click_count] = x_loc;
                        locationY_array[click_count] = y_loc;
                        click_count++;
                    }
                }
            }
        }
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // refresh window
    display();
}

void onMoveDown(int x, int y) {

    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
    


    // to refresh the window it calls display() function
    glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
 
    // if F1 is down   
    if (move) {
        // each of the object will move to the nearest side (left or right)
        for (int i = 0; i < click_count; i++) {
            if (locationX_array[i] >= 0)
                locationX_array[i] += random_array[i];
            else
                locationX_array[i] -= random_array[i];
        }
    }

    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()
}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Template File");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();
    // generates random numbers in to an array after program starts
    generateRandomArray();
    glutMainLoop();
}
