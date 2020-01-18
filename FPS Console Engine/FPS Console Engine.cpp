// FPS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace std;

int screenWidth = 120;
int screenHeight = 40;

float playerX = 8.0f;
float playerY = 8.0f;
float playerA = 0.0f;

int mapHeight = 16;
int mapWidth = 16;

float fov = 3.14159 / 4.0;
float depth = 16.0f;

int main()
{
    // create screen buffer
    wchar_t* screen = new wchar_t[screenWidth * screenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD bytesWritten = 0;

    wstring map;

    map += L"################";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..........#...#";
    map += L"#..........#...#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#.......########";
    map += L"#..............#";
    map += L"#..............#";
    map += L"################";

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    // game loop
    while (1)
    {
        // calculate frame time
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTimeChrono = tp2 - tp1;
        tp1 = tp2;
        float elapsedTime = elapsedTimeChrono.count();

        // controls
        // handle counter clockwise rotation
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
        {
            playerA -= (0.8f) * elapsedTime;
        }
        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
        {
            playerA += (0.8f) * elapsedTime;
        }

        if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
        {
            playerX += sinf(playerA) * 5.0f * elapsedTime;
            playerY += cosf(playerA) * 5.0f * elapsedTime;

            if (map[(int)playerY * mapWidth + (int)playerX] == '#')
            {
                playerX -= sinf(playerA) * 5.0f * elapsedTime;
                playerY -= cosf(playerA) * 5.0f * elapsedTime;
            }
        }

        if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
        {
            playerX -= sinf(playerA) * 5.0f * elapsedTime;
            playerY -= cosf(playerA) * 5.0f * elapsedTime;

            if (map[(int)playerY * mapWidth + (int)playerX] == '#')
            {
                playerX += sinf(playerA) * 5.0f * elapsedTime;
                playerY += cosf(playerA) * 5.0f * elapsedTime;
            }
        }

        for (int x = 0; x < screenWidth; x++)
        {
            // for each column, calculate the projected ray angle into world space
            float rayAngle = (playerA - fov / 2.0f) + ((float)x / (float)screenWidth) * fov;

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
                        sort(p.begin(), p.end(), [](const pair<float, float>& left, const pair<float, float> &right) { return left.first < right.first; });

                        float bound = 0.01f;
                        if (acos(p.at(0).second) < bound) boundary = true;
                        if (acos(p.at(1).second) < bound) boundary = true;
                        //if (acos(p.at(2).second) < bound) boundary = true;
                    }
                }
            }

            // calculate distance to ceiling and floor
            int ceiling = (float)(screenHeight / 2.0) - screenHeight / ((float)distanceToWall);
            int floor = screenHeight - ceiling;

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

            for (int y = 0; y < screenHeight; y++)
            {
                if (y <= ceiling)
                {
                    screen[y * screenWidth + x] = ' ';
                }
                else if (y > ceiling&& y <= floor)
                {
                    screen[y * screenWidth + x] = shade;
                }
                else
                {
                    float b = 1.0f - (((float)y - screenHeight / 2.0f) / ((float)screenHeight / 2.0f));
                    if (b < 0.25) shade = '#';
                    else if (b < 0.25) shade = 'x';
                    else if (b < 0.75) shade = '.';
                    else if (b < 0.9) shade = '-';
                    else shade = ' ';

                    screen[y * screenWidth + x] = shade;
                }
            }
        }

        // display stats
        swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", playerX, playerY, playerA, 1.0f / elapsedTime);

        // display map
        for (int nx = 0; nx < mapWidth; nx++)
        {
            for (int ny = 0; ny < mapWidth; ny++)
            {
                screen[(ny + 1) * screenWidth + nx] = map[ny * mapWidth + nx];
            }
        }

        // show player on map
        screen[((int)playerY + 1) * screenWidth + (int)playerX] = 'P';

        screen[screenWidth * screenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, { 0,0 }, &bytesWritten);
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
