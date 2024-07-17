/*********
----------
STUDENT : Mehmet Enes Çakýr
SECTION : 01
HOMEWORK: HOMEWORK 3
----------
PROBLEMS:   * No problems
----------
ADDITIONAL FEATURES: * pause game
                     * one sinusodial, orbital, lineer movement
                     * restart the game with click button (mouse click) 
                     * when game paused user cannot restart the game (only in game continuing)
                     * when mouse is on the restart button, it colors to the red, otherwise white
                     * restarting the game change;
                        * red -> speed, direction, orbit radius, location
                        * green -> speed, location
                        * blue -> velocity, speed, location
*********/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "vec.h"

#define WINDOW_WIDTH  700
#define WINDOW_HEIGHT 500

#define GAME_BOUNDARY_LEFT      ((WINDOW_WIDTH / 2) - 10)
#define GAME_BOUNDARY_RIGHT     (-(WINDOW_WIDTH / 2) + 10)
#define GAME_BOUNDARY_UP        ((WINDOW_HEIGHT / 2) - 10)
#define GAME_BOUNDARY_DOWN      (-(WINDOW_HEIGHT / 2) + 10)

#define TIMER_PERIOD   16 // Period for the timer.
#define TIMER_ON        1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

#define RADIUS 50

#define NUM 3

/* Global Variables */
int  winWidth, winHeight; // current Window width and height
int red_orbit = 100;
int maxV = 5, minV = 1; // max & min velocity of the light sources, to be used in rand() func
int direction = 1; // ( -1 * cosX ) will change the direction of the movement on the orbit

bool up = false, down = false, right = false, left = false;
bool pause = false; // pause the game (state machine)
bool restart = false; // restart game with diff values
bool click = false;

float angle = 0.0f;
float angle_sin = 0.0f;
float angle_inc = 0.70f;
float speed = 1.0f;
float A = 50, //amplitude
      fq = 1,  //frequency
      C = 0,   //horizontal phase shift
      B = 0;   //vertical phase shift

typedef struct {
    float r, g, b;
} color_t;

typedef struct {
    char name[8];
    vec_t   pos;
    vec_t   vel;
    color_t color;
    bool active;
} light_t;

typedef struct {
    vec_t pos;
    vec_t N;
} vertex_t;

/* to change the center of the black planet */
vec_t center = { 0, 0 };

light_t light[3] =
{
   { "Red", {  0, 0 }, { 0, 0 }, {1, 0, 0}, true},     /* No need for vel for red light source (orbital) */
   { "Green", {  200, 0 },{ 0, 0 }, {0, 1, 0}, true},  /* No need for vel for green light source (sinusodial) */
   { "Blue", { -200, 0 }, { 3, -3 }, {0, 0, 1}, true}
};

color_t mulColor(float k, color_t c) {
    color_t tmp = { k * c.r, k * c.g, k * c.b };
    return tmp;
}

color_t addColor(color_t c1, color_t c2) {
    color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
    return tmp;
}

// To add distance into calculation
// when distance is 0  => its impact is 1.0
// when distance is 350 => impact is 0.0
// Linear impact of distance on light calculation.
double distanceImpact(double d) {
    return (-1.0 / sqrt(pow(WINDOW_HEIGHT, 2) + pow(WINDOW_WIDTH, 2))) * d + 1.0; /* limited in the game area */
}

color_t calculateColor(light_t source, vertex_t v) {
    vec_t L = subV(source.pos, v.pos);
    vec_t uL = unitV(L);
    float factor = dotP(uL, v.N) * distanceImpact(magV(L));
    return mulColor(factor, source.color);
}

int randomInteger(int lower, int upper) {

    int x;

    if (restart) {
        
        x = (rand() % (upper - lower + 1)) + lower;
    }

    return(x);
}

float randomFloat(float min, float max) {

    float f_num;

    f_num = rand() / (float)RAND_MAX;
    
    return min + f_num * (max - min);
}

float sinusodial(float x) {
    return A * sin((fq * x + C) * D2R) + B;
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

void displayLightSources() {

    // orbit of red light source
    glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
    circle_wire(0, 0, red_orbit);

    // route of the green light source (sinusodial)
    glColor4f(1.0f, 1.0f, 1.0f, 0.15f);
    glBegin(GL_LINE_STRIP);
    for (float x = 0; x <= 360; x += 0.70) {
        glVertex2f(x * 1.85 - 335, sinusodial(x * 9) - 175);
    }
    glEnd();


    // light sources --
    for (int i = 0; i < NUM; i++) {
        if (light[i].active) {
            glColor3f(light[i].color.r, light[i].color.g, light[i].color.b);
            circle(light[i].pos.x, light[i].pos.y, 10);
            glColor3f(light[i].color.r, light[i].color.g, light[i].color.b);
            circle_wire(light[i].pos.x, light[i].pos.y, 11);
        }
    }
}

void displayEffectsAndPlanet() {

    // black planet (circle) & light sources effects on it
    for (float i = 0; i < 360; i += 0.5) {

        float angle = i * D2R;

        vec_t position = addV({ RADIUS * cos(angle), RADIUS * sin(angle) }, center);
        vertex_t P = { position, unitV(subV(position, center)) };

        color_t res = { 0, 0, 0 };
        for (int k = 0; k < NUM; k++)
            if (light[k].active)
                res = addColor(res, calculateColor(light[k], P));

        glBegin(GL_LINES);
        glColor3f(res.r, res.g, res.b);
        glVertex2f(center.x, center.y);
        glVertex2f(P.pos.x, P.pos.y);
    }
    glEnd();

    glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
    circle_wire(center.x, center.y, RADIUS + 1);
}

void displayTexts() {
    
    // boundaries
    glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-347, 248);
    glVertex2f(-347, -248);
    glVertex2f(347, -247);
    glVertex2f(347, 247);
    glEnd();


    int location[2] = { -340 , 230 };

    glColor3f(1, 1, 1);

    for (int i = 0; i < NUM; i++) {

        if (light[i].active) {
            vprint(location[0], location[1], GLUT_BITMAP_HELVETICA_12, "F%d <--> %s : MOVE", i + 1, light[i].name);
        }
        else {
            vprint(location[0], location[1], GLUT_BITMAP_HELVETICA_12, "F%d <--> %s : DISAPPEAR - STOP", i + 1, light[i].name);
        }

        location[1] -= 20;
    }

    glColor3f(1, 1, 1);

    if (pause) {
        glColor4f(1.0f, 0.0f, 0.0f, 0.9f);
        vprint(245, 228, GLUT_BITMAP_HELVETICA_12, "PAUSED");
    }
    else {
        glColor4f(0.0f, 1.0f, 0.0f, 0.9f);
        vprint(240, 228, GLUT_BITMAP_HELVETICA_12, "CONTINUE");
    }

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    vprint(location[0] + 260, location[1] + 60, GLUT_BITMAP_HELVETICA_12, "Mehmet Enes Cakir - Homework #3");

    vprint(217, 205, GLUT_BITMAP_HELVETICA_12, "CLICK TO RESTART");

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(212, 220);
    glVertex2f(340, 220);
    glVertex2f(340, 198);
    glVertex2f(212, 198);
    glEnd();

    if (click && !pause)
        glColor4f(1.0f, 0.1f, 0.0f, 0.3f);
    else
        glColor4f(1.0f, 1.0f, 1.0f, 0.3f);

    glRectf(340, 220, 212, 198);
}

//
// To display onto window using OpenGL commands
//
void display() {

    //
    // clear window to black
    //
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    displayLightSources();

    displayEffectsAndPlanet();

    displayTexts();

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
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    case GLUT_KEY_F1: light[0].active = !light[0].active; break; // red
    case GLUT_KEY_F2: light[1].active = !light[1].active; break; // green
    case GLUT_KEY_F3: light[2].active = !light[2].active; break; // blue
    case GLUT_KEY_F4: pause = !pause; break;
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
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
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
    int yR = winHeight / 2 - y;
    int xR = x - winWidth / 2;

    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {
        if (xR < 340 && xR > 212 && yR > 198 && yR < 220) {
            if (pause == false) restart = true;
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
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

void onMove(int x, int y) {
    
    int yR = winHeight / 2 - y;
    int xR = x - winWidth / 2;

    if (xR < 340 && xR > 212 && yR > 198 && yR < 220)
        click = true;
    else
        click = false;
    

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);

    if (pause == false) {

        /* Randomly Restart Part (F5 key) */
        if (restart) {

            /* Red Light Source Restart Part (speed, radius & direction) */
            red_orbit = randomInteger(110, 210); // max - min
            light[0].pos.x = red_orbit;
            double orbit = (double)red_orbit;
            double x = (double)light[0].pos.x;
            float ang = randomFloat(0.0f, 360.0f);
            light[0].pos.y = sqrt(pow(orbit, 2) - pow(x, 2)); /*  x^2 + y^2 = r^2  ->  y = square root of (r^2 - y^2)  */
            angle = ang;
            angle_sin = ang;
            speed = randomFloat(0.9f, 1.8f);

            /* Blue Light Source Restart Part (speed & velocity) */
            light[2].pos.x = randomFloat(GAME_BOUNDARY_LEFT, GAME_BOUNDARY_RIGHT);
            light[2].pos.y = randomFloat(GAME_BOUNDARY_DOWN, GAME_BOUNDARY_UP);
            light[2].vel.x = randomInteger(minV, maxV);
            light[2].vel.y = randomInteger(minV, maxV);
            direction = rand() % 2 ? 1 : -1; // random generator ( 1 or -1 ) -- changes the direction randomly

            /* Since angle & angle increment is changed, Green Light Source location & speed change randomly */
            angle_inc = randomFloat(0.5f, 0.9f);

            restart = false;
        }

        /* red light source movement (ORBITAL) */
        light[0].pos = { direction * cos(angle * speed * D2R) * red_orbit, sin(angle * speed * D2R) * red_orbit };

        /* green light source movement (SINUSODIAL) */
        light[1].pos = { angle_sin * 1.85 - 335, sinusodial(angle_sin * 9) - 175 };

        /* blue light source movement (LINEER) */
        if (light[2].active)
            light[2].pos = addV(light[2].pos, light[2].vel);

        if (light[2].pos.x > GAME_BOUNDARY_LEFT || light[2].pos.x < GAME_BOUNDARY_RIGHT) {
            light[2].vel.x *= -1;
        }

        if (light[2].pos.y > GAME_BOUNDARY_UP || light[2].pos.y < GAME_BOUNDARY_DOWN) {
            light[2].vel.y *= -1;
        }
        // ------------------------------------------


        // angle & angle increment of movements (0 - 360) trigonometrical
        if (light[0].active) angle += angle_inc;        // if red is active, red's angle is increasing - orbital
        if (light[1].active) angle_sin += angle_inc;    // if green is active, green's angle is increasing - sinusodial

        if (angle_sin >= 360.0f)
            angle_sin -= 360.0f;

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
    glutCreateWindow("Lighting Simulation");

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
    srand(time(0));
    glutMainLoop();
}
