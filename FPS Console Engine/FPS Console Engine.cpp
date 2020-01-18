// FPS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

#include "olcConsoleGameEngine.h"

class UltimateFPS : public olcConsoleGameEngine
{
public:
    UltimateFPS()
    {
        m_sAppName = L"Ultimate First Person Shooter";
    }

    virtual bool OnUserCreate()
    {
        map += L"#########.......#########.......";
        map += L"#...............#...............";
        map += L"#.......#########.......########";
        map += L"#..............##..............#";
        map += L"#......##......##......##......#";
        map += L"#......##..............##......#";
        map += L"#..............##..............#";
        map += L"###............####............#";
        map += L"##.............###.............#";
        map += L"#............####............###";
        map += L"#..............................#";
        map += L"#..............##..............#";
        map += L"#..............##..............#";
        map += L"#...........#####...........####";
        map += L"#..............................#";
        map += L"###..####....########....#######";
        map += L"####.####.......######..........";
        map += L"#...............#...............";
        map += L"#.......#########.......##..####";
        map += L"#..............##..............#";
        map += L"#......##......##.......#......#";
        map += L"#......##......##......##......#";
        map += L"#..............##..............#";
        map += L"###............####............#";
        map += L"##.............###.............#";
        map += L"#............####............###";
        map += L"#..............................#";
        map += L"#..............................#";
        map += L"#..............##..............#";
        map += L"#...........##..............####";
        map += L"#..............##..............#";
        map += L"################################";

        spriteWall = new olcSprite(L"F:\\Coding\\CPP\\FPS Console Engine\\Debug\\FPSSprites\\fps_wall1.spr");


        return true;
    }

    virtual bool OnUserUpdate(float elapsedTime)
    {
        // controls

        // rotation
        if (m_keys[L'A'].bHeld)
        {
            playerA -= (0.8f) * elapsedTime;
        }
        if (m_keys[L'D'].bHeld)
        {
            playerA += (0.8f) * elapsedTime;
        }

        // forwards
        if (m_keys[L'W'].bHeld)
        {
            playerX += sinf(playerA) * speed * elapsedTime;
            playerY += cosf(playerA) * speed * elapsedTime;

            if (map[(int)playerY * mapWidth + (int)playerX] == '#')
            {
                playerX -= sinf(playerA) * speed * elapsedTime;
                playerY -= cosf(playerA) * speed * elapsedTime;
            }
        }

        // backwards
        if (m_keys[L'S'].bHeld)
        {
            playerX -= sinf(playerA) * speed * elapsedTime;
            playerY -= cosf(playerA) * speed * elapsedTime;

            if (map[(int)playerY * mapWidth + (int)playerX] == '#')
            {
                playerX += sinf(playerA) * speed * elapsedTime;
                playerY += cosf(playerA) * speed * elapsedTime;
            }
        }

        // strafe right
        if (m_keys[L'E'].bHeld)
        {
            playerX += cosf(playerA) * speed * elapsedTime;
            playerY -= sinf(playerA) * speed * elapsedTime;

            if (map.c_str()[(int)playerX * mapWidth + (int)playerY] == '#')
            {
                playerX -= cosf(playerA) * speed * elapsedTime;
                playerY += sinf(playerA) * speed * elapsedTime;
            }
        }

        // strafe left
        if (m_keys[L'Q'].bHeld)
        {
            playerX -= cosf(playerA) * speed * elapsedTime;
            playerY += sinf(playerA) * speed * elapsedTime;

            if (map.c_str()[(int)playerX * mapWidth + (int)playerY] == '#')
            {
                playerX += cosf(playerA) * speed * elapsedTime;
                playerY -= sinf(playerA) * speed * elapsedTime;
            }
        }

        for (int x = 0; x < ScreenWidth(); x++)
        {
            // for each column, calculate the projected ray angle into world space
            float rayAngle = (playerA - fov / 2.0f) + ((float)x / (float)ScreenWidth()) * fov;

            float stepSize = 0.01f;
            float distanceToWall = 0.0f;

            bool hitWall = false;
            bool boundary = false;

            float eyeX = sinf(rayAngle);
            float eyeY = cosf(rayAngle);

            float sampleX = 0.0f;

            while (!hitWall && distanceToWall < depth)
            {
                distanceToWall += stepSize;

                int testX = (int)(playerX + eyeX * distanceToWall);
                int testY = (int)(playerY + eyeY * distanceToWall);

                // test if ray is out of bounds
                if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight)
                {
                    hitWall = true; // just set distance to maximum depth
                    distanceToWall = depth;
                }
                else
                {
                    // ray is inbounds so test to see if the ray cell is a wall block
                    if (map[testY * mapWidth + testX] == '#')
                    {
                        hitWall = true;

                        float blockMidX = (float)testX + 0.5f;
                        float blockMidY = (float)testY + 0.5f;

                        float testPointX = playerX + eyeX * distanceToWall;
                        float testPointY = playerX + eyeY * distanceToWall;

                        float testAngle = atan2f((testPointY - blockMidY), (testPointX - blockMidX));

                        if (testAngle >= -3.14159f * 0.25f && testAngle < 3.14159f * 0.25f)
                            sampleX = testPointY - (float)testY;
                        if (testAngle >= 3.14159f * 0.25f && testAngle < 3.14159f * 0.75f)
                            sampleX = testPointX - (float)testX;
                        if (testAngle < -3.14159f * 0.25f && testAngle >= -3.14159f * 0.75f)
                            sampleX = testPointX - (float)testX;
                        if (testAngle >= 3.14159f * 0.75f || testAngle < -3.14159f * 0.75f)
                            sampleX = testPointY - (float)testY;
                    }
                }
            }

            // calculate distance to ceiling and floor
            int ceiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)distanceToWall);
            int floor = ScreenHeight() - ceiling;

            for (int y = 0; y < ScreenHeight(); y++)
            {
                if (y <= ceiling)
                {
                    Draw(x, y, L' ');
                }
                else if (y > ceiling&& y <= floor)
                {
                    if (distanceToWall < depth)
                    {
                        float sampleY = ((float)y - (float)ceiling) / ((float)floor - (float)ceiling);
                        Draw(x, y, spriteWall->SampleGlyph(sampleX, sampleY), spriteWall->SampleColour(sampleX, sampleY));
                    }
                    else
                    {
                        Draw(x, y, PIXEL_SOLID, 0);
                    }
                }
                else
                {
                    Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
                }
            }
        }

        // display stats
        //swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", playerX, playerY, playerA, 1.0f / elapsedTime);

        // display map
        for (int nx = 0; nx < mapWidth; nx++)
        {
            for (int ny = 0; ny < mapWidth; ny++)
            {
                Draw(nx + 1, ny + 1, map[ny * mapWidth + nx]);
            }
        }

        // show player on map
        Draw(1 + (int)playerY, 1 + (int)playerX, L'P');

        return true;
    }

private:
    float playerX = 8.0f;
    float playerY = 8.0f;
    float playerA = 0.0f;

    int mapHeight = 32;
    int mapWidth = 32;

    float fov = 3.14159 / 4.0;
    float depth = 16.0f;
    float speed = 5.0f;

    wstring map;

    olcSprite *spriteWall;
    olcSprite *spriteLamp;
    olcSprite *spriteFireball;

    struct sObject
    {
        float x;
        float y;
        olcSprite *sprite;
    };

    list<sObject> listObjects;
};

int main()
{
    UltimateFPS game;
    game.ConstructConsole(320, 240, 4, 4);
    game.Start();

    return 0;
}