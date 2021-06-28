#ifndef UNICODE
#define UNICODE
#endif


#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
using namespace std;

#include <math.h>
#include <stdio.h>
#include <Windows.h>
#include <winuser.h>
#include <algorithm>
//Console Variables
int nScreenWidth = 120;
int nScreenHeight = 40;

//X, Y, Angle
float fPlayerX = 8.0f;
float fPlayerY = 8.0f;
float fPlayerA = 0.0f;

//Map Variables
int nMapHeight = 16;
int nMapWidth = 16;

//player variables
float fFOV = 3.14159f / 4.0f;
float fDepth = 16.0f;
int main()
{

    //create buffer screen
    wchar_t* screen = new wchar_t[nScreenHeight * nScreenWidth];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;


    wstring map;

    map += L"################";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..########....#";
    map += L"#..#..#####....#";
    map += L"#..#..#........#";
    map += L"#..#..#........#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#....##........#";
    map += L"#....##........#";
    map += L"#..............#";
    map += L"#.........######";
    map += L"#..............#";
    map += L"################";

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    //Gameloop
    while (1)
    {

        //We need to control time to get a consistent rotation rate and move rate
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fElapsedTime = elapsedTime.count();


        //Controls

        //Translation movemtn
        if (GetAsyncKeyState((unsigned)'W') & 0x8000)
        {
            fPlayerX += sinf(fPlayerA) * 6.0f * fElapsedTime;
            fPlayerY += cosf(fPlayerA) * 6.0f * fElapsedTime;
            //collision detection
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
            {
                fPlayerX -= sinf(fPlayerA) * 6.0f * fElapsedTime;
                fPlayerY -= cosf(fPlayerA) * 6.0f * fElapsedTime;
            }
        }

        if (GetAsyncKeyState((unsigned)'S') & 0x8000)
        {
            fPlayerX -= sinf(fPlayerA) * 6.0f * fElapsedTime;
            fPlayerY -= cosf(fPlayerA) * 6.0f * fElapsedTime;
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
            {
                fPlayerX += sinf(fPlayerA) * 6.0f * fElapsedTime;
                fPlayerY += cosf(fPlayerA) * 6.0f * fElapsedTime;
            }
        }

        //strafing crashes game for some reason
//        if (GetAsyncKeyState((unsigned)'A') & 0x8000)
//        {
//            fPlayerX -= acosf(fPlayerA) * 4.0f * fElapsedTime;
//            fPlayerY += asinf(fPlayerA) * 4.0f * fElapsedTime;
//            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
//            {
//                fPlayerX += acosf(fPlayerA) * 4.0f * fElapsedTime;
//                fPlayerY -= asinf(fPlayerA) * 4.0f * fElapsedTime;
//            }
//        }
//        if (GetAsyncKeyState((unsigned)'D') & 0x8000)
//        {
//            fPlayerX += acosf(fPlayerA) * 4.0f * fElapsedTime;
//            fPlayerY -= asinf(fPlayerA) * 4.0f * fElapsedTime;
//            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
//            {
//                fPlayerX -= acosf(fPlayerA) * 4.0f * fElapsedTime;
//                fPlayerY += asinf(fPlayerA) * 4.0f * fElapsedTime;
//            }
//        }


        //Rotation movement

        if (GetAsyncKeyState((unsigned)'Q') & 0x8000)
        {
            fPlayerA -= (0.1) * 10.0f * fElapsedTime;
        }

        if (GetAsyncKeyState((unsigned)'E') & 0x8000)
        {
            fPlayerA += (0.1) * 10.0f * fElapsedTime;
        }





        for (int x = 0; x < nScreenWidth; x++)
        {
            //for every column we cast a ray, and we find out the projected angle
            //from -FOV/2 to +FOV/2 with steps of the screen width (120 in this case)
            float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

            //Distance from player to wall for current angle
            float fDistancetoWall = 0;
            bool bHitWall = false;
            //Boundary as in edge of cell
            bool bBoundary = false;

            float fEyeX = sinf(fRayAngle); // unit vector for ray in player space
            float fEyeY = cosf(fRayAngle);

            //We slowly increment the distance from the player until the ray hits a tile was a wall block
            while (!bHitWall && fDistancetoWall < fDepth)
            {
                fDistancetoWall += 0.01f;

                int nTestX = (int)(fPlayerX + fEyeX * fDistancetoWall);
                int nTestY = (int)(fPlayerY + fEyeY * fDistancetoWall);

                //check if ray goes out of bounds
                if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
                {
                    bHitWall = true;
                    fDistancetoWall = fDepth;

                }
                //check if hits wall
                else
                {
                    if (map[nTestY * nMapWidth + nTestX] == '#')
                    {
                        bHitWall = true;
                        //Highlighting corners, if the ray cast has a very close angle to the perfect corner
                        //of the cell it'll be highlighted, but we want the two closest, only
                        vector<pair<float, float>> p; //distance, dot
                        for (int tx = 0; tx < 2; tx++)
                        {
                            for (int ty = 0; ty < 2; ty++)
                            {
                                float vy = (float)nTestY + ty - fPlayerY;
                                float vx = (float)nTestX + tx - fPlayerX;
                                float d = sqrt(vx * vx + vy * vy);
                                float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
                                p.push_back(make_pair(d, dot));

                            }

                            //sort pairs from closest to furthers
                            sort(p.begin(), p.end(), [](const pair<float, float>& left, const pair<float, float>& right) {return left.first < right.first; });
                            float fBound = 0.005;
                            if (acos(p.at(0).second) < fBound) bBoundary = true;
                            if (acos(p.at(1).second) < fBound) bBoundary = true;
                        }

                    }
                }
            }
            //calculate distance to ceiling and floor, to give illusion of perspective
            int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistancetoWall);
            int nFloor = nScreenHeight - nCeiling;

            short nShade = ' ';

            //shade to give illusion of distance, using extended unicode
            if (fDistancetoWall <= fDepth / 4.0f) nShade = 0x2588; //very close
            else if (fDistancetoWall <= fDepth / 3.0f) nShade = 0x2593;
            else if (fDistancetoWall <= fDepth / 2.0f) nShade = 0x2592;
            else if (fDistancetoWall <= fDepth)        nShade = 0x2591;
            else                                  nShade = ' '; //too far

            if (bBoundary) nShade = ' '; //Highlight boundary

            for (int y = 0; y < nScreenHeight; y++)
            {
                if (y <= nCeiling)
                {
                    //drawing sky as blank space
                    screen[y * nScreenWidth + x] = ' ';
                }
                else if (y > nCeiling && y <= nFloor)
                {
                    // drawing wall
                    screen[y * nScreenWidth + x] = nShade;
                }
                else
                {
                    //if not wall or sky, it must be floor
                    //shade floor based on distance
                    float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
                    if (b < 0.25)     nShade = '#';
                    else if (b < 0.5)  nShade = 'x';
                    else if (b < 0.75) nShade = '-';
                    else if (b < 0.5)  nShade = '.';
                    else               nShade = ' ';
                    screen[y * nScreenWidth + x] = nShade;
                }
            }
        }


        //Display Stats
        swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f, FPS=%5.2f", fPlayerX, fPlayerY, fPlayerA, 1.0f / fElapsedTime);

        //Display map
        for (int nx = 0; nx < nMapWidth; nx++)
        {
            for (int ny = 0; ny < nMapWidth; ny++)
            {
                screen[(ny + 1) * nScreenWidth + nx] = map[ny * nMapWidth + nx];
            }
        }
        screen[((int)fPlayerY + 1) * nScreenWidth + (int)fPlayerX] = 'P';

        //Display Frame
        screen[nScreenWidth * nScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

    }

    return 0;
}
