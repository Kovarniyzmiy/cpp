#include <GL\glut.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <windows.h>

#define WIDTH 80
#define HEIGHT 45
int score = 0;
const int smooth = 2;
int count = 20 * 4;
int DisplayW = 40, DisplayH = 30;
int W = count * DisplayW, H = count * DisplayH;
int direction;
const int level = 10;
int bullet_count = 0;
int enemy_bullet_count = 0;
GLuint  textura_id;

/*
Игра работает на OpenGL
Создаются объекты классов texture, enemy, ship, asteroid.
Каждый кадр проверяется местоположение всех объектов и нажатие кнопок.
Каждый кадр обновляются и изменяются переменные пололжения объектов на экране.
Каждый кадр создаются новые объекты класса астероид.
Так же каждый кадр проверяется наезд хитбоксов разных объектов друг на друга => наносится урон противнику/убивается игрок.
управление: WASD/стрелки - перемещение, пробел - выстрел, esc - выход.
*/

class Enemy {
public:
    int x, y;
    int hp;

    void New() {
        x = DisplayW + 1;
        y = DisplayH-5;
        hp = 100;
    }

    void DrawNew() {

        glColor3f(1.0, 1.0, 1.0);

        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 1);

        glBegin(GL_QUADS);
        glTexCoord2d(0, 1); glVertex2d((x - 1) * count, (y - 1) * count);
        glTexCoord2d(0, 0); glVertex2d((x - 1) * count, (y + 2) * count);
        glTexCoord2d(1, 0); glVertex2d((x + 2) * count, (y + 2) * count);
        glTexCoord2d(1, 1); glVertex2d((x + 2) * count, (y - 1) * count);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glPopMatrix();


        glutSwapBuffers();
    }
        
} e[2];

class Ship {
public:
    int x, y;
} s[1];

class enemybullet {
public:
    int x, y;

    void Hide() {
        x = H;
        y = W;
    }

    void New() {
        x = e[1].x;
        y = e[1].y - 1;
    }

    void DrawNew() {

        glColor3f(1.0, 1.0, 1.0);

        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 2);
        glRotatef(0.0f, 0.0f, 0.0f, 1.0f);

        glBegin(GL_QUADS);
        glTexCoord2d(0, 0); glVertex2d(x * count, y * count);
        glTexCoord2d(0, 1); glVertex2d(x * count, (y + 1) * count);
        glTexCoord2d(1, 1); glVertex2d((x + 1) * count, (y + 1) * count);
        glTexCoord2d(1, 0); glVertex2d((x + 1) * count, y * count);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glPopMatrix();


        glutSwapBuffers();
    }
}eb[50];

class asteroid {
public:
    int x, y, col;

    void New() {
        x = rand() % DisplayW;
        y = DisplayH + rand() % 30;
        col = 3 + rand() % 2;
    }

    void DrawNew() {

        glColor3f(1.0, 1.0, 1.0);

        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, col);
        glRotatef(0.0f, 0.0f, 0.0f, 1.0f);

        glBegin(GL_QUADS);
        glTexCoord2d(1, 1); glVertex2d(x * count, y * count);
        glTexCoord2d(1, 0); glVertex2d(x * count, (y + 1) * count);
        glTexCoord2d(0, 0); glVertex2d((x + 1) * count, (y + 1) * count);
        glTexCoord2d(0, 1); glVertex2d((x + 1) * count, y * count);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glPopMatrix();


        glutSwapBuffers();
    }
}z[level];

class bullet {
public:
    int x, y;

    void Hide() {
        x = -1;
        y = s[0].y;
    }

    void New() {
        x = s[0].x;
        y = s[0].y;
    }

    void DrawNew() {

        glColor3f(1.0, 1.0, 1.0);

        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 2);
        glRotatef(0.0f, 0.0f, 0.0f, 1.0f);

        glBegin(GL_QUADS);
        glTexCoord2d(1, 1); glVertex2d(x * count, y * count);
        glTexCoord2d(1, 0); glVertex2d(x * count, (y + 1) * count);
        glTexCoord2d(0, 0); glVertex2d((x + 1) * count, (y + 1) * count);
        glTexCoord2d(0, 1); glVertex2d((x + 1) * count, y * count);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glPopMatrix();


        glutSwapBuffers();
    }
}b[50];

void shoot() {
    b[bullet_count].New();
    b[bullet_count].DrawNew();
    bullet_count = bullet_count + 1;
    if (bullet_count >= 50) { bullet_count = 0; }
}

void enemyshoot() {
    eb[enemy_bullet_count].New();
    eb[enemy_bullet_count].DrawNew();
    enemy_bullet_count = enemy_bullet_count + 1;
    if (enemy_bullet_count >= 50) { enemy_bullet_count = 0; }
}

struct textura_struct
{
    int W;
    int H;
    unsigned char* Image;
}get_textura;

int LoadTexture(const char* FileName)
{
    FILE* F;
    if ((F = fopen(FileName, "rb")) == NULL)
        return 0;
    fseek(F, 18, SEEK_SET);
    fread(&(get_textura.W), 2, 1, F);
    fseek(F, 2, SEEK_CUR);
    fread(&(get_textura.H), 2, 1, F);

    printf("%d x %d\n", get_textura.W, get_textura.H);

    if ((get_textura.Image = (unsigned char*)malloc(sizeof(unsigned char) * 3 * get_textura.W * get_textura.H)) == NULL)
    {
        fclose(F);
        return 0;
    }

    fseek(F, 30, SEEK_CUR);
    fread(get_textura.Image, 3, get_textura.W * get_textura.H, F);


    glGenTextures(1, &textura_id);
    glBindTexture(GL_TEXTURE_2D, textura_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, get_textura.W, get_textura.H, GL_BGR_EXT, GL_UNSIGNED_BYTE, get_textura.Image);
    free(get_textura.Image);
    printf("%d tx", textura_id);
    fclose(F);

    return 1;
}

void draw() {

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glRotatef(0.0f, 0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0); glVertex2d(0, 0);
    glTexCoord2d(0, 1); glVertex2d(0, 300);
    glTexCoord2d(1, 1); glVertex2d(500, 300);
    glTexCoord2d(1, 0); glVertex2d(500, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();


    glutSwapBuffers();
}

void Movement() {
    if (e[1].hp < 1) {
    printf("%d", e[1].hp);
        e[1].New();
    }
    for (int i = 0; i < level; i++) {
        if (z[i].y < 0) { z[i].New(); }
        if (z[i].x >= s[0].x - 1 && z[i].x <= s[0].x + 1 && s[0].y == z[i].y) {exit(0); break;}
    }

    for (int i = 0; i < 50; i++) {
        if (b[i].y > H) { b[i].Hide();}
        for (int j = 0; j < level; j++) {
            if (b[i].y >= z[j].y && b[i].y <= z[j].y + count && b[i].y + count >= z[j].y + count && b[i].y <= z[j].y + count && b[i].x == z[j].x) { b[i].Hide(); z[j].New(); }
            }
    }
    for (int i = 0; i < 50; i++) {
        if (eb[i].y == s[0].y && eb[i].x >= s[0].x - 1 && eb[i].x <= s[0].x + 1) { eb[i].Hide(); exit(0); }
        }

    for (int i = 0; i < 50; i++) {
        if (e[1].y == b[i].y && e[1].x >= b[i].x - 1 && e[1].x <= b[i].x + 1) { e[1].hp = e[1].hp - 20; }
    }
}

void Movebullets() {
    for (int i = 0; i < 50; i++) {
        b[i].y = b[i].y + 2 / smooth;
    }
}

void MoveEnemybullets() {
    
    for (int i = 0; i < 50; i++) {
        eb[i].y = eb[i].y - 2 / smooth;
    }
}

void MoveAsteroids() {
    for (int i = 0; i < level; i++) {
        z[i].y = z[i].y - 2 / smooth;
    }
}

void DrawLines() { //Поле
    glColor3f(0.0, 1.0, 1.0);
    glBegin(GL_LINES);

    for (int i = 0; i < W; i = i + count) {
        glVertex2f(i, 0); glVertex2f(i, H);
    }
    for (int j = 0; j < H; j = j + count) {
        glVertex2f(0, j); glVertex2f(W, j);
    }

    glEnd();
}

void DrawPlayer() {
    glColor3f(1.0, 1.0, 1.0);

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 1);
    glRotatef(0.0f, 0.0f, 0.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2d(0, 0); glVertex2d((s[0].x - 1) * count, (s[0].y - 1)* count);
    glTexCoord2d(0, 1); glVertex2d((s[0].x - 1)* count, (s[0].y + 2) * count);
    glTexCoord2d(1, 1); glVertex2d((s[0].x + 2) * count, (s[0].y + 2) * count);
    glTexCoord2d(1, 0); glVertex2d((s[0].x + 2) * count, (s[0].y - 1) * count);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();


    glutSwapBuffers();
    //glRectf(s[i].x * count, s[i].y * count, (s[i].x + 1) * count, (s[i].y + 1) * count); //(x1, y1, x2, y2)
    
}

void MoveEnemy() {
    if (e[1].x == s[0].x) { enemyshoot(); }
    if (e[1].x < s[0].x) { e[1].x = e[1].x + 1; }
    if (e[1].x > s[0].x) { e[1].x = e[1].x - 1; }
}

void Display() {

    MoveEnemy();
    MoveEnemybullets();
    MoveAsteroids();
    Movebullets();
    glClear(GL_COLOR_BUFFER_BIT);
    DrawPlayer();
    e[1].DrawNew();

    for (int i = 0; i < 50; i++) {
        b[i].DrawNew();
    }
    for (int i = 0; i < 50; i++) {
        eb[i].DrawNew();
    }
    //draw();
    //DrawLines();

    for (int i = 0; i < level; i++) {
        z[i].DrawNew();
    }


    glFlush();
}

void KeyboardEvent(unsigned char key, int a, int b) {
    //printf("%d \n", key);
    switch (key) {
    case 32: shoot(); break;
    case 119: if (s[0].y <= (DisplayH - 3)) { s[0].y += 1; break; }; break; //вверх
    case 100: if (s[0].x <= (DisplayW - 3)) { s[0].x += 1; break; }; break; //направо
    case 97:  if (s[0].x >= 2) { s[0].x -= 1; break; }; break; //налево
    case 115: if (s[0].y >= 2) { s[0].y -= 1; break; }; break; //вниз
    case 27:  exit(0); break;

    }
}

void Arrows(int key, int a, int b) {
    switch (key) {
    case 101:  if (s[0].y <= (DisplayH - 3)) { s[0].y += 1; break; }; break; //вверх
    case 102:  if (s[0].x <= (DisplayW - 3)) { s[0].x += 1; break; }; break; //направо
    case 100:  if (s[0].x >= 2) { s[0].x -= 1; break; }; break; //налево
    case 103:  if(s[0].y >= 2) { s[0].y -= 1; break; }; break; //вниз
    }
}

void Random() {
    e[1].New();
    for (int i = 0; i < level; i++) {
        z[i].New();
    }
}

void timer(int i = 0) {
    Display();
    Movement();
    glutTimerFunc(100, timer, 0);
}

int main(int argc, char** argv) {
    s[0].x = 6; s[0].y = 6;

    glutInit(&argc, argv);
    glutInitWindowPosition(100, 50);
    glutInitWindowSize(W/4, H/4);
    glutCreateWindow("game");
    glutDisplayFunc(draw);
    glMatrixMode(GL_PROJECTION);

    if (LoadTexture("ship.bmp") != 1) { printf("Error! Cannot open file\n"); }
    if (LoadTexture("Fireball.bmp") != 1) { printf("Error! Cannot open file\n"); }
    if (LoadTexture("asteroid1.bmp") != 1) { printf("Error! Cannot open file\n"); }
    if (LoadTexture("asteroid2.bmp") != 1) { printf("Error! Cannot open file\n"); }

    glLoadIdentity();
    gluOrtho2D(0, W, 0, H);

    Random();
    glutDisplayFunc(Display);
    glutTimerFunc(50, timer, 0);
    glutSpecialFunc(Arrows);
    glutKeyboardUpFunc(KeyboardEvent);

    glutMainLoop();
    return 0;
}