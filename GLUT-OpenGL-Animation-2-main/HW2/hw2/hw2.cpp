/*********
----------
STUDENT : Mehmet Enes Çakýr
SECTION : 01
HOMEWORK: HOMEWORK 2
----------
PROBLEMS:   * cannot create 5 ufo targets at the same time
            * a problem about collision function (not working properly)
            * since collision function doesn't work properly;
            *   1) some ufos do not reach the end
            *   2) score count wrongly
----------
ADDITIONAL FEATURES: * shadow effects on the spaceship (details)
                     * showing score to keep how many ufos are crashed (sadly, do not work properly)
                     * background effects and images 
                     * ufos randomly appears at the spesific location of y axis
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

#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 700

#define TIMER_PERIOD  1000 // Period for the timer.
#define TIMER_PERIOD_FIRE 16 // Period for the bullet speed
#define DURATION 20
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

// declaration of max ufo
#define MAX_UFO 5

// ufo speed
#define UFO_SPEED 5
// radius of ufo
#define UFO_RADIUS 40

// bullet speed
#define BULLET_SPEED 12

// max fire at a time
#define MAX_FIRE 10
// each 8 fire frame, user can shoot bullet 
#define FIRE_RATE 32

// the states of the game
bool pause = false;
bool end = false;

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, shoot = false;
int  winWidth, winHeight; // current Window width and height

// score -> user fired the ufo
int score = 0;

// store ammo info
typedef struct {

    int x;
    int y;

    bool status;

} bullet_t;

// store ufo info
typedef struct {
    
    int x;
    int y;

    bool status;

} ufo_t;

ufo_t ufo[MAX_UFO];
bullet_t bullet[MAX_FIRE];

// time counting
int time_count = DURATION;

// location of spaceship
int x_loc = 0, y_loc = -310;

int fire_rate = 0;

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

void spiral(float x, float y, float r, float start, float end, float offset) {
    glColor3f(1, 1, 1);
    glBegin(GL_LINE_STRIP);
    for (float angle = start; angle < end; angle += 10) {
        r -= offset;
        glVertex2f(r * cos(angle * D2R) + x, r * sin(angle * D2R) + y);
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

void ufo_display(ufo_t ufo) {

    //
    glColor3ub(153, 186, 221);
    circle(ufo.x, ufo.y + 20, 20);
    //

    //
    glColor3f(1, 1, 1);
    circle_wire(ufo.x, ufo.y + 20, 21);
    //

    //
    glColor3ub(0, 135, 62);
    glRectf(ufo.x - 20, ufo.y, ufo.x + 20, ufo.y + 20);
    //

    //
    glBegin(GL_LINE_STRIP);
    glColor3ub(120, 121, 118);

    glVertex2f(ufo.x - 30, ufo.y);
    glVertex2f(ufo.x - 30, ufo.y - 30);
    glVertex2f(ufo.x - 35, ufo.y - 30);

    glEnd();
    
    glBegin(GL_LINE_STRIP);
    glColor3ub(120, 121, 118);

    glVertex2f(ufo.x + 30, ufo.y);
    glVertex2f(ufo.x + 30, ufo.y - 30);
    glVertex2f(ufo.x + 35, ufo.y - 30);

    glEnd();
    //

    //
    glBegin(GL_TRIANGLES);

    glColor3ub(0, 135, 62);
    glVertex2f(ufo.x + 20, ufo.y);
    glVertex2f(ufo.x + 20, ufo.y + 20);
    glVertex2f(ufo.x + 50, ufo.y);

    glColor3ub(0, 135, 62);
    glVertex2f(ufo.x - 20, ufo.y);
    glVertex2f(ufo.x - 20, ufo.y + 20);
    glVertex2f(ufo.x - 50, ufo.y);
    
    glColor3ub(67, 70, 75);
    glVertex2f(ufo.x + 30, ufo.y);
    glVertex2f(ufo.x + 30, ufo.y - 10);
    glVertex2f(ufo.x + 40, ufo.y);

    glColor3ub(67, 70, 75);
    glVertex2f(ufo.x - 30, ufo.y);
    glVertex2f(ufo.x - 30, ufo.y - 10);
    glVertex2f(ufo.x - 40, ufo.y);

    glEnd();
    //

    //
    glColor3ub(136, 139, 141);
    glRectf(ufo.x - 17, ufo.y - 13, ufo.x + 17, ufo.y - 30);
    //

    //
    glBegin(GL_POLYGON);
    glColor3ub(67, 70, 75);

    glVertex2f(ufo.x + 30, ufo.y);
    glVertex2f(ufo.x + 35, ufo.y - 18);
    glVertex2f(ufo.x, ufo.y - 27);
    glVertex2f(ufo.x - 35, ufo.y - 18);
    glVertex2f(ufo.x - 30, ufo.y);

    glEnd();
    //

}

void spaceship_display(int x, int y) {

    //
    // main boundary of the spaceship
    glLineWidth(4);
    glColor3f(1, 1, 1);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + 20, y);
    glVertex2f(x + 20, y - 30);
    glVertex2f(x + 74, y + 20);
    glVertex2f(x + 74, y + 60);
    glVertex2f(x + 67, y + 60);
    glVertex2f(x + 67, y + 90);
    glVertex2f(x + 40, y + 120);
    glVertex2f(x + 40, y + 40);
    glVertex2f(x + 30, y + 40);
    glVertex2f(x + 20, y + 60);
    glVertex2f(x + 15, y + 60);
    glVertex2f(x + 15, y + 70);
    glVertex2f(x + 10, y + 70);
    glVertex2f(x + 10, y + 80);
    glVertex2f(x, y + 95);
    glVertex2f(x - 10, y + 80);
    glVertex2f(x - 10, y + 70);
    glVertex2f(x - 15, y + 70);
    glVertex2f(x - 15, y + 70);
    glVertex2f(x - 15, y + 60);
    glVertex2f(x - 20, y + 60);
    glVertex2f(x - 30, y + 40);
    glVertex2f(x - 40, y + 40);
    glVertex2f(x - 40, y + 120);
    glVertex2f(x - 67, y + 90);
    glVertex2f(x - 67, y + 60);
    glVertex2f(x - 74, y + 60);
    glVertex2f(x - 74, y + 20);
    glVertex2f(x - 20, y - 30);
    glVertex2f(x - 20, y);
    glEnd();


    glBegin(GL_QUADS);
    //left part (violet) 
    glColor3ub(155, 38, 182);
    glVertex2f(x - 40, y + 60);
    glVertex2f(x - 40, y + 120);
    glVertex2f(x - 67, y + 90);
    glVertex2f(x - 67, y + 60);

    //right part (advent purple)
    glColor3ub(75, 54, 95);
    glVertex2f(x + 40, y + 60);
    glVertex2f(x + 40, y + 120);
    glVertex2f(x + 67, y + 90);
    glVertex2f(x + 67, y + 60);
    glEnd();


    glBegin(GL_POLYGON);
    //left part (violet)
    glColor3ub(155, 38, 182);
    glVertex2f(x - 74, y + 60);
    glVertex2f(x - 74, y + 20);
    glVertex2f(x - 20, y - 30);
    glVertex2f(x - 20, y + 20);
    glVertex2f(x - 40, y + 41);
    glEnd();

    glBegin(GL_POLYGON);
    //right part (advent purple)
    glColor3ub(75, 54, 95);
    glVertex2f(x + 74, y + 60);
    glVertex2f(x + 74, y + 20);
    glVertex2f(x + 20, y - 30);
    glVertex2f(x + 20, y + 20);
    glVertex2f(x + 40, y + 41);
    glEnd();


    glBegin(GL_POLYGON);
    //left part (violet)
    glColor3ub(155, 38, 182);
    glVertex2f(x - 19, y + 20);
    glVertex2f(x - 19, y + 60);
    glVertex2f(x - 30, y + 40);
    glVertex2f(x - 40, y + 40);
    glEnd();

    glBegin(GL_POLYGON);
    //right part (advent purple)
    glColor3ub(75, 54, 95);
    glVertex2f(x + 19, y + 20);
    glVertex2f(x + 19, y + 60);
    glVertex2f(x + 30, y + 40);
    glVertex2f(x + 40, y + 40);;
    glEnd();


    glBegin(GL_TRIANGLES);
    //left part (violet)
    glColor3ub(155, 38, 182);
    glVertex2f(x - 74, y + 60);
    glVertex2f(x - 40, y + 60);
    glVertex2f(x - 40, y + 40);
    //right part (advent purple)
    glColor3ub(75, 54, 95);
    glVertex2f(x + 74, y + 60);
    glVertex2f(x + 40, y + 60);
    glVertex2f(x + 40, y + 40);
    glEnd();


    glBegin(GL_TRIANGLES);
    //left part (violet)
    glColor3ub(75, 54, 95);
    glVertex2f(x, y + 59);
    glVertex2f(x + 19, y + 59);
    glVertex2f(x + 20, y);
    //right part (advent purple)
    glColor3ub(155, 38, 182);
    glVertex2f(x, y + 59);
    glVertex2f(x - 19, y + 59);
    glVertex2f(x - 20, y);
    glEnd();


    glBegin(GL_TRIANGLES);
    //left part (dark pink grape)
    glColor3ub(128, 49, 167);
    glVertex2f(x - 20, y);
    glVertex2f(x , y);
    glVertex2f(x , y + 60);
    //right part (midnight purple)
    glColor3ub(46, 26, 71);
    glVertex2f(x + 20, y);
    glVertex2f(x, y);
    glVertex2f(x, y + 60);
    glEnd();

    //
    // left spaceship cabin (dark blue ton of window) / azure blue
    glBegin(GL_POLYGON);
    glColor3ub(0, 138, 216);
    glVertex2f(x , y + 65);
    glVertex2f(x + 9, y + 65);
    glVertex2f(x + 9, y + 81);
    glVertex2f(x , y + 92);
    glEnd();

    //
    // right spaceship cabin (light blue ton of window) / aqua blue
    glBegin(GL_POLYGON);
    glColor3ub(5, 195, 221);
    glVertex2f(x, y + 65);
    glVertex2f(x - 9, y + 65);
    glVertex2f(x - 9, y + 81);
    glVertex2f(x, y + 92);
    glEnd();

    //
    // gray cockpit 
    glColor3ub(120, 121, 118);
    glRectf(x - 15, y + 58, x + 15, y + 70);

    //
    // shadows of the right part of spaceship
    glLineWidth(8);
    glBegin(GL_LINE_STRIP);
    glColor3ub(46, 26, 71);
    glVertex2f(x + 18, y);
    glVertex2f(x + 18, y + 4);
    glVertex2f(x + 24, y + 4);
    glVertex2f(x + 24, y);
    glVertex2f(x + 24, y - 22);
    glVertex2f(x + 70, y + 20);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(x + 38, y + 10);
    glVertex2f(x + 60, y + 30);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(x + 22, y + 19);
    glVertex2f(x + 65, y + 45);
    glEnd();
    glLineWidth(5);
    glBegin(GL_LINE_STRIP);
    glColor3ub(46, 26, 71);
    glVertex2f(x + 43, y +  64);
    glVertex2f(x + 43, y + 115);
    glVertex2f(x + 63, y + 90);
    glVertex2f(x + 63, y + 62);
    glEnd();

    //
    // shadows of the left part of spaceship
    glLineWidth(8);
    glBegin(GL_LINE_STRIP);
    glColor3ub(128, 49, 167);
    glVertex2f(x - 18, y);
    glVertex2f(x - 18, y + 4);
    glVertex2f(x - 24, y + 4);
    glVertex2f(x - 24, y);
    glVertex2f(x - 24, y - 22);
    glVertex2f(x - 70, y + 20);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(x - 38, y + 10);
    glVertex2f(x - 60, y + 30);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(x - 22, y + 19);
    glVertex2f(x - 65, y + 45);
    glEnd();
    glLineWidth(5);
    glBegin(GL_LINE_STRIP);
    glColor3ub(128, 49, 167);
    glVertex2f(x - 43, y + 64);
    glVertex2f(x - 43, y + 115);
    glVertex2f(x - 63, y + 90);
    glVertex2f(x - 63, y + 62);
    glEnd();

    //
    // fire of spaceship (rocket)

    glBegin(GL_POLYGON);
    glColor3ub(225, 6, 0);
    glVertex2f(x - 2, y - 2);
    glVertex2f(x - 15, y - 17);
    glVertex2f(x - 7, y - 16);
    glVertex2f(x - 10, y - 28);
    glVertex2f(x , y - 22);
    glVertex2f(x + 10, y - 28);
    glVertex2f(x + 7, y - 16);
    glVertex2f(x + 15, y -17);
    glVertex2f(x + 2, y - 2);

    glColor3ub(255, 133, 15);
    glVertex2f(x - 1, y - 2);
    glVertex2f(x - 11, y - 14);
    glVertex2f(x - 3, y - 13);
    glVertex2f(x - 6, y - 23);
    glVertex2f(x, y - 19);
    glVertex2f(x + 6, y - 23);
    glVertex2f(x + 3, y - 13);
    glVertex2f(x + 11, y - 14);
    glVertex2f(x + 1, y - 2);

    glEnd();

    glBegin(GL_POLYGON);
    glColor3ub(255, 133, 15);
    glVertex2f(x - 1, y - 2);
    glVertex2f(x - 11, y - 14);
    glVertex2f(x - 3, y - 13);
    glVertex2f(x - 6, y - 23);
    glVertex2f(x, y - 19);
    glVertex2f(x + 6, y - 23);
    glVertex2f(x + 3, y - 13);
    glVertex2f(x + 11, y - 14);
    glVertex2f(x + 1, y - 2);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3ub(255, 233, 0);
    glVertex2f(x - 1, y - 2);
    glVertex2f(x - 7, y - 10);
    glVertex2f(x - 2, y - 8);
    glVertex2f(x - 4, y - 15);
    glVertex2f(x, y - 12);
    glVertex2f(x + 4, y - 15);
    glVertex2f(x + 2, y - 8);
    glVertex2f(x + 6, y - 10);
    glVertex2f(x + 1, y - 2);
    glEnd();

    // metal part firing
    glColor3ub(67, 70, 75);
    glRectf(x - 7, y + 2, x + 7, y - 3);


}

void bullet_image(int x, int y)
{
    
    //
    // left part of the rocket 
    glBegin(GL_POLYGON);
    glColor3ub(0, 138, 216);
    glVertex2f(x, y);
    glVertex2f(x + 12,y);
    glVertex2f(x + 12, y + 16);
    glVertex2f(x, y + 27);
    glVertex2f(x, y - 27);
    glEnd();

    //
    // right part of the rocket
    glBegin(GL_POLYGON);
    glColor3ub(5, 195, 221);
    glVertex2f(x, y);
    glVertex2f(x - 12, y);
    glVertex2f(x -12, y + 16);
    glVertex2f(x, y + 27);
    glEnd();
    
    glBegin(GL_POLYGON);
    glColor3ub(1, 115, 92);
    glVertex2f(x - 2, y - 2);
    glVertex2f(x - 15, y - 17);
    glVertex2f(x - 7, y - 16);
    glVertex2f(x - 10, y - 28);
    glVertex2f(x, y - 22);
    glVertex2f(x + 10, y - 28);
    glVertex2f(x + 7, y - 16);
    glVertex2f(x + 15, y - 17);
    glVertex2f(x + 2, y - 2);

    glColor3ub(8, 255, 8);
    glVertex2f(x - 1, y - 2);
    glVertex2f(x - 11, y - 14);
    glVertex2f(x - 3, y - 13);
    glVertex2f(x - 6, y - 23);
    glVertex2f(x, y - 19);
    glVertex2f(x + 6, y - 23);
    glVertex2f(x + 3, y - 13);
    glVertex2f(x + 11, y - 14);
    glVertex2f(x + 1, y - 2);

    glEnd();

    glBegin(GL_POLYGON);
    glColor3ub(8, 255, 8);
    glVertex2f(x - 1, y - 2);
    glVertex2f(x - 11, y - 14);
    glVertex2f(x - 3, y - 13);
    glVertex2f(x - 6, y - 23);
    glVertex2f(x, y - 19);
    glVertex2f(x + 6, y - 23);
    glVertex2f(x + 3, y - 13);
    glVertex2f(x + 11, y - 14);
    glVertex2f(x + 1, y - 2);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3ub(0, 174, 88);
    glVertex2f(x - 1, y - 2);
    glVertex2f(x - 7, y - 10);
    glVertex2f(x - 2, y - 8);
    glVertex2f(x - 4, y - 15);
    glVertex2f(x, y - 12);
    glVertex2f(x + 4, y - 15);
    glVertex2f(x + 2, y - 8);
    glVertex2f(x + 6, y - 10);
    glVertex2f(x + 1, y - 2);
    glEnd();

}
//
// shows bullets in the specified area
void display_bullet_fired() {
    
    for (int i = 0; i < MAX_FIRE; i++) {
        if (bullet[i].status) {
            bullet_image(bullet[i].x, bullet[i].y);
        }
    }
}

//
// it shows the remaining time
void time_display() {

    glColor4f(1, 1, 1, 0.3);
    vprint(325, 265, GLUT_BITMAP_TIMES_ROMAN_24, "TIME: ");

    if (time_count >= 10)
        vprint2(400, 260, 0.25, "00:%2d", time_count);
    else if (time_count >= 0)
        vprint2(400, 260, 0.25, "00:0%d", time_count);
    else
        vprint2(400, 260, 0.25, "00:00");

}

void score_display() {

    glColor4f(1, 1, 1, 0.3);
    vprint(-485, 260, GLUT_BITMAP_TIMES_ROMAN_24, "SCORE: ");
    vprint2(-400, 260, 0.20, "%d", score);
}

void main_background() {
    
    // background (dark tone of blue)
    glColor3f(0.00, 0.02, 0.12);
    glRectf(-490, -340, 490, 300);


    // informative texts
    glColor3f(0, 0, 0);
    vprint(-483, 317, GLUT_BITMAP_8_BY_13, "Mehmet Enes CAKIR");
    vprint(354, 317, GLUT_BITMAP_9_BY_15, "- Homework 2 -");
    vprint(-195, 314, GLUT_BITMAP_TIMES_ROMAN_24, "----SPACE INVADERS GAME----");


    // boundaries of the template
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    glColor3f(0.00, 0.50, 0.33);
    glVertex2f(-490, 300);
    glVertex2f(-490, 346);
    glVertex2f(490, 346);
    glVertex2f(490, 300);
    glEnd();

    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    glColor3f(0.00, 0.50, 0.33);
    glVertex2f(-490, -340);
    glVertex2f(-490, 346);
    glVertex2f(490, 346);
    glVertex2f(490, -340);
    glEnd();


    // background of texts
    glColor4f(0, 0.1, 0, 0.3);
    glRectf(350, 337, 487, 307);

    glColor4f(0, 0.1, 0, 0.3);
    glRectf(-344, 337, -487, 307);

    spiral(250, 120, 60, 0, 1080, 0.5);

    spiral(-270, -90, 60, 0, 1080, 0.5);
}

void direction_display() {
    
    glColor4f(1, 1, 1, 0.35);
    vprint2(-180, 270, 0.135, "<- . -> : Arrow Keys to Move");
    vprint2(-115, 252, 0.135, "Spacebar : Shoot");

}

// To display onto window using OpenGL commands
void display() {

    // clear window to white
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    main_background();

    // if user presses F1 , game be wil paused
    if (pause) {

        // pause symbol
        glColor4f(1, 1, 1, 0.8);
        glRectf(-30, 30, -10, 120);
        glColor4f(1, 1, 1, 0.8);
        glRectf(30, 30, 10, 120);

        // informative text to be shown
        glColor3f(1, 1, 1);
        vprint(-155, -55, GLUT_BITMAP_TIMES_ROMAN_24, "~~THE GAME IS PAUSED~~");
        glColor3f(1, 1, 1);
        vprint(-177, -80, GLUT_BITMAP_TIMES_ROMAN_24, "PRESS F1 TO CONTINUE GAME");
    }
    else {
        direction_display();

        time_display();

        score_display();

        ufo_display(*ufo);

        spaceship_display(x_loc, y_loc + 30);
     
        display_bullet_fired();
    }

    glutSwapBuffers();
}

// In the bullet storing array, check if there is any bullet.
// if there is a bullet that player can fire, returns its index.
int bullet_number_check() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (bullet[i].status == false)
            return i;
    }
    return -1;
}

void add_reset_target(ufo_t *ufo) {
    
    float ufo_y = rand() % 2 ? 150 : 30;
    ufo->x = -480;
    ufo->y = ufo_y;
    ufo->status = true;
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // if SPACEBAR is pressed
    if (key == 32)
        // assign the locations for each bullet fired
        shoot = true;

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    if (key == 32)
        shoot = false;

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
        // if game is not paused, spaceship goes left
        left = true;
        break;
    case GLUT_KEY_RIGHT:
        // if game is not paused, spaceship goes right
        right = true;
        break;
    case GLUT_KEY_F1: // the game pauses, if F1 key is pressed down
        pause = !pause;
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
    int y_loc = winHeight / 2 - y;
    int x_loc = x - winWidth / 2;

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

bool collision_test(ufo_t ufo, bullet_t blt) {
    
    float dx = ufo.x - blt.x;
    float dy = ufo.y - blt.y;
    float d = sqrt(dx * dx + dy * dy);
    return d <= UFO_RADIUS;
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);

    if (pause == false) {

        time_count--;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()
}
#endif

#if TIMER_ON == 1
void onTimerFire(int v) {

    glutTimerFunc(TIMER_PERIOD_FIRE, onTimerFire, 0);
    
    if (pause == false) {

        if (right && x_loc <= 405)
            x_loc += 12;

        if (left && x_loc >= -405)
            x_loc -= 12;


        if (shoot && fire_rate == 0) {
            int availFire = bullet_number_check();

            if (availFire != -1) {
                bullet[availFire].status = true;
                bullet[availFire].x = x_loc;
                bullet[availFire].y = -190;
                fire_rate = FIRE_RATE;
            }
        }

        if (fire_rate > 0)
            fire_rate--;

        for (int i = 0; i < MAX_FIRE; i++) {
            if (bullet[i].status) {
                bullet[i].y += BULLET_SPEED;

                if (bullet[i].y > 270) {
                    bullet[i].status = false;
                }
            }

            for (int k = 0; k < MAX_UFO; k++) {

                if (collision_test(ufo[k], bullet[i])) {
                    add_reset_target(&ufo[k]);
                    score += 10;
                }

                if (ufo[k].x > 450) {
                    add_reset_target(&ufo[k]);
                }
            }
        }

        for (int i = 0; i < MAX_UFO; i++) {
            ufo[i].x += UFO_SPEED;
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
    glutInitWindowPosition(150, 50);
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
    glutTimerFunc(TIMER_PERIOD_FIRE, onTimerFire, 0);
#endif

    Init();
    // generates random numbers in to an array after program starts
    //generateRandomArray();
    glutMainLoop();
}
