#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#define PI 3.14159265359

float playerX, playerY;
float playerDeltaX, playerDeltaY;
float playerAngle;

float fFOV = PI / 4.0;

int mapX = 8;
int mapY = 8;
int mapSize = 64;
int map[] = 
{
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 1, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 1, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
};

void drawMap2D()
{
    int x, y, xo, yo;
    for (y = 0; y < mapY; y++)
    {
        for (x=0; x < mapX; x++)
        {
            if (map[y*mapX+x] == 1)
            {
                glColor3f(1, 1, 1);
            }
            else
            {
                glColor3f(0, 0, 0);
            }
            xo = x * mapSize;
            yo = y * mapSize;
            glBegin(GL_QUADS);
            glVertex2i(xo + 1, yo + 1);
            glVertex2i(xo + 1, yo + mapSize - 1);
            glVertex2i(xo + mapSize - 1, yo + mapSize - 1);
            glVertex2i(xo + mapSize - 1, yo + 1);
            glEnd();
        }
    }
}


void drawPlayer()
{
    glColor3f(1,1,0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(playerX, playerY);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(playerX, playerY);
    glVertex2i(playerX + playerDeltaX * 5, playerY + playerDeltaY * 5);
    glEnd();
}

void drawRays3D()
{
    int r, mx, my, mp, dof;
    float rayX, rayY, rayAngle, xOffset, yOffset;
    rayAngle = playerAngle;
    for (r = 0; r < 1; r++)
    {
        /* check horizontal lines */
        dof = 0;
        float atan = -1 / tan(rayAngle);
        if (rayAngle > PI)
        {
            rayY = (((int)playerY >> 6) << 6) - 0.0001; // round rayY to nearest 64th value
            rayX = (playerY - rayY) * atan + playerX;
            yOffset = -64;
            xOffset = -yOffset * atan;
        }
        if (rayAngle < PI)
        {
            rayY = (((int)playerY >> 6) << 6) + 64;
            rayX = (playerY - rayY) * atan + playerX;
            yOffset = 64;
            xOffset = -yOffset * atan;
        }
        if (rayAngle == 0 || rayAngle == PI) // looking straight left or right
        {
            rayX = playerX;
            rayY = playerY;
            dof = 8;
        }
        while (dof < 8)
        {
            mx = (int) (rayX) >> 6;
            my = (int) (rayY) >> 6;
            mp = my * mapX + mx;
            if (mp < mapX * mapY && map[mp] == 1) // hit wall
            {
                dof = 8;
            }
            else // next line
            {
                rayX += xOffset;
                rayY += yOffset;
                dof += 1;
            }
        }
        glColor3f(0, 1, 0);
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2i(playerX, playerY);
        glVertex2i(rayX, rayY);
        glEnd();

        /* check vertical lines */
        dof = 0;
        atan = -tan(rayAngle);
        if (rayAngle > (PI/2) && rayAngle < (3*PI/2))
        {
            rayX = (((int)playerX >> 6) << 6) - 0.0001; 
            rayY = (playerX - rayX) * atan + playerY;
            xOffset = -64;
            yOffset = -xOffset * atan;
        }
        if (rayAngle < (PI/2) || rayAngle > (3*PI/2))
        {
            rayX = (((int)playerX >> 6) << 6) + 64;
            rayY = (playerX - rayX) * atan + playerY;
            xOffset = 64;
            yOffset = -xOffset * atan;
        }
        if (rayAngle == 0 || rayAngle == PI) // looking straight left or right
        {
            rayX = playerX;
            rayY = playerY;
            dof = 8;
        }
        while (dof < 8)
        {
            mx = (int) (rayX) >> 6;
            my = (int) (rayY) >> 6;
            mp = my * mapX + mx;
            if (mp < mapX * mapY && map[mp] == 1) // hit wall
            {
                dof = 8;
            }
            else // next line
            {
                rayX += xOffset;
                rayY += yOffset;
                dof += 1;
            }
        }
        glColor3f(1, 0, 0);
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2i(playerX, playerY);
        glVertex2i(rayX, rayY);
        glEnd();
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap2D();
    drawPlayer();
    glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y)
{
    if (key == 'a')
    {
        playerAngle += 0.1;
        if (playerAngle > 2 * PI)
        {
            playerAngle -= 2 * PI;
        }
        playerDeltaX = cos(playerAngle) * 5;
        playerDeltaY = sin(playerAngle) * 5;
    }
    if (key == 'd')
    {
        playerAngle -= 0.1;
        if (playerAngle < 0)
        {
            playerAngle += 2 * PI;
        }
        playerDeltaX = cos(playerAngle) * 5;
        playerDeltaY = sin(playerAngle) * 5;
    }
    if (key == 'w')
    {
        playerX += playerDeltaX;
        playerY += playerDeltaY;
    }
    if (key == 's')
    {
        playerX -= playerDeltaX;
        playerY -= playerDeltaY;
    }
    glutPostRedisplay();
}

void init()
{
    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0, 1024, 512, 0);
    playerX=300;
    playerY=300;
    playerDeltaX = cos(playerAngle) * 5;
    playerDeltaY = sin(playerAngle) * 5;
}

int main(int argc, char * argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1024, 512);
    glutCreateWindow("Raycasting");
    glutFullScreen();
    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);
    glutMainLoop();
}