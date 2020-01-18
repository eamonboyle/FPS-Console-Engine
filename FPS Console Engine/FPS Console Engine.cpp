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

            float distanceToWall = 0.0f;

            bool hitWall = false;
            bool boundary = false;

            float eyeX = sinf(rayAngle);
            float eyeY = cosf(rayAngle);

            while (!hitWall && distanceToWall < depth)
            {
                distanceToWall += 0.1f;

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

                        vector<pair<float, float>> p; // distance, dot

                        for (int tx = 0; tx < 2; tx++)
                        {
                            for (int ty = 0; ty < 2; ty++)
                            {
                                float vy = (float)testY + ty - playerY;
                                float vx = (float)testX + tx - playerX;
                                float d = sqrt(vx * vx + vy * vy);
                                float dot = (eyeX * vx / d) + (eyeY * vy / d);
                                p.push_back(make_pair(d, dot));
                            }
                        }

                        // sort pairs from closest to farthest
                        sort(p.begin(), p.end(), [](const pair<float, float>& left, const pair<float, float>& right) { return left.first < right.first; });

                        float bound = 0.01f;
                        if (acos(p.at(0).second) < bound) boundary = true;
                        if (acos(p.at(1).second) < bound) boundary = true;
                        //if (acos(p.at(2).second) < bound) boundary = true;
                    }
                }
            }

            // calculate distance to ceiling and floor
            int ceiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)distanceToWall);
            int floor = ScreenHeight() - ceiling;

            // calculate wall distance
            short shade = ' ';

            if (distanceToWall <= depth / 4.0f)         shade = 0x2588; // close
            else if (distanceToWall < depth / 3.0f)     shade = 0x2593;
            else if (distanceToWall < depth / 2.0f)     shade = 0x2592;
            else if (distanceToWall < depth)            shade = 0x2591;
            else                                        shade = ' ';    // far away

            // shade boundary
            if (boundary)
            {
                shade = ' ';
            }

            for (int y = 0; y < ScreenHeight(); y++)
            {
                if (y <= ceiling)
                {
                    Draw(x, y, L' ');
                }
                else if (y > ceiling&& y <= floor)
                {
                    Draw(x, y, shade);
                }
                else
                {
                    float b = 1.0f - (((float)y - ScreenHeight() / 2.0f) / ((float)ScreenHeight() / 2.0f));
                    if (b < 0.25) shade = '#';
                    else if (b < 0.25) shade = 'x';
                    else if (b < 0.75) shade = '.';
                    else if (b < 0.9) shade = '-';
                    else shade = ' ';

                    Draw(x, y, shade);
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
};

int main()
{
    UltimateFPS game;
    game.ConstructConsole(320, 240, 4, 4);
    game.Start();

    return 0;
}