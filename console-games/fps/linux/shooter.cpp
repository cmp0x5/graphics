#include <locale.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <utility>
#include <cmath>

using namespace std;

#include <stdio.h>
#include <ncurses.h>

int nScreenWidth = 120;
int nScreenHeight = 40;

float fPlayerX = 8.0f;
float fPlayerY = 8.0f;
float fPlayerAngle = 0.0f;

int nMapHeight = 16;
int nMapWidth = 16;

float fFOV = 3.14159 / 4.0;
float fDepth = 16.0f;

int main()
{
	setlocale(LC_ALL, "");
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	
	wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
	//HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	//SetConsoleActiveScreenBuffer(hConsole);
	//DWORD dwBytesWritten = 0;
	
	wstring map;

	map += L"################";
	map += L"#..............#";
	map += L"#...######.....#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#####..........#";
	map += L"#..............#";
	map += L"#.........######";
	map += L"#..#...........#";
	map += L"#..#...####....#";
	map += L"#..#...........#";
	map += L"#..............#";
	map += L"#.......########";
	map += L"#..............#";
	map += L"#..............#";
	map += L"################";

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	int ch;

	bool running = true;
	while(running)
	{
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();
		float fPerpendicular = fPlayerAngle += 1.5708f; // strafing

		ch = getch();
		switch(ch)
		{
			case(0x41):
				fPlayerX -= sinf(fPerpendicular) * 3.0f * fElapsedTime;
				fPlayerY -= cosf(fPerpendicular) * 3.0f * fElapsedTime;

				if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
				{
					fPlayerX += sinf(fPerpendicular) * 3.0f * fElapsedTime;
					fPlayerY += cosf(fPerpendicular) * 3.0f * fElapsedTime;
				}
				break;
			
			case(0x44):
				fPlayerX += sinf(fPerpendicular) * 3.0f * fElapsedTime;
				fPlayerY += cosf(fPerpendicular) * 3.0f * fElapsedTime;

				if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
				{
					fPlayerX -= sinf(fPerpendicular) * 3.0f * fElapsedTime;
					fPlayerY -= cosf(fPerpendicular) * 3.0f * fElapsedTime;
				}
				break;

			case(KEY_LEFT):
				fPlayerAngle -= 0.8f * fElapsedTime;
				break;

			case(KEY_RIGHT):
				fPlayerAngle += 0.8f * fElapsedTime;
				break;

			case(0x57):
				fPlayerX += sinf(fPlayerAngle) * 5.0f * fElapsedTime;
				fPlayerY += cosf(fPlayerAngle) * 5.0f * fElapsedTime;

				if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
				{
					fPlayerX -= sinf(fPlayerAngle) * 5.0f * fElapsedTime;
					fPlayerY -= cosf(fPlayerAngle) * 5.0f * fElapsedTime;
				}
				break;

			case(0x53):
				fPlayerX -= sinf(fPlayerAngle) * 5.0f * fElapsedTime;
				fPlayerY -= cosf(fPlayerAngle) * 5.0f * fElapsedTime;

				if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
				{
					fPlayerX += sinf(fPlayerAngle) * 5.0f * fElapsedTime;
					fPlayerY += cosf(fPlayerAngle) * 5.0f * fElapsedTime;
				}
				break;
		}

		// VK_SPACE 0x20

		for (int x=0; x < nScreenWidth; x++)
		{
			// for each column, calculate projected ray angle into world space
			// start from leftmost ray, move along as x increases
			float fRayAngle = (fPlayerAngle - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

			float fDistanceToWall = 0;
			bool bInsideWall = false;
			bool bBoundary = false;

			// unit vector for ray in player space
			float fEyeX = sinf(fRayAngle);
			float fEyeY = cosf(fRayAngle);

			while (!bInsideWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += 0.1f;

				// unit vector grows in direction player is looking at
				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				// test if ray is out of bounds, set distance to max depth
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bInsideWall = true;
					fDistanceToWall = fDepth;
				}
				else
				{
					// Ray is inbounds, test if ray cell is a wall block
					if (map[nTestY * nMapWidth + nTestX] == '#')
					{
						bInsideWall = true;

						vector<pair<float, float>> p; // distance to perfect corner, dot product

						for (int tx = 0; tx < 2; tx++)
						{
							for (int ty = 0; ty < 2; ty++)
							{
								float vy = (float)nTestY + ty - fPlayerY;
								float vx = (float)nTestX + tx - fPlayerX;
								float d = sqrt(vx*vx + vy*vy);
								float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								p.push_back(make_pair(d, dot));
							}
							// sort pairs from closest to farthest
							sort(p.begin(), p.end(), [](const pair<float, float> &left, const pair<float, float> &right) {return left.first < right.first; });

							float fBound = 0.01;
							if (acos(p.at(0).second) < fBound) bBoundary = true;
							if (acos(p.at(1).second) < fBound) bBoundary = true;
							//if (acos(p.at(2).second) < fBound) bBoundary = true;
						}
					}
				}
			}

			// calculate distance to ceiling and floor to create illusion of perspective
			// nCeiling higher as distance increases, floor mirrors ceiling
			int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;

			short nShade = ' ';

			if (fDistanceToWall <= fDepth / 4.0f) // close
				nShade = 0x2588;
			else if (fDistanceToWall < fDepth / 3.0f) 
				nShade = 0x2593;
			else if (fDistanceToWall < fDepth / 2.0f) 
				nShade = 0x2592;
			else if (fDistanceToWall < fDepth) 
				nShade = 0x2591;
			else 
				nShade = ' '; // far

			if (bBoundary)
				nShade = ' ';

			for(int y = 0; y < nScreenHeight; y++)
			{
				if (y < nCeiling) // ceiling
				{
					screen[y*nScreenWidth + x] = ' ';
				}
				else if (y > nCeiling && y <= nFloor) // wall
				{
					screen[y*nScreenWidth + x] = nShade;
				}
				else // floor
				{
					float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
					if (b > 0.25)
						nShade = '#';
					else if (b < 0.5)
						nShade = 'x';
					else if (b < 0.75)
						nShade = '.';
					else if (b < 0.9)
						nShade = '-';
					else
						nShade = ' ';
					screen[y*nScreenWidth + x] = nShade;
				}
			}

		}

		wprintf(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f, Perp=%3.2f, FPS=%3.2f ", fPlayerX, fPlayerY, fPlayerAngle, fPerpendicular, 1.0f / fElapsedTime);
		
		// minimap
		for (int nx = 0; nx < nMapWidth; nx++)
		{
			for (int ny = 0; nx < nMapWidth; ny++)
			{
				screen[(ny + 1) * nScreenWidth + nx] = map[ny * nMapWidth + nx];
			}
		}

		screen[((int)fPlayerY + 1) * nScreenWidth + (int)fPlayerX] = 'P';
	
		// set last char to null
		screen[nScreenWidth * nScreenHeight - 1] = '\0'; 	

		// coord is {0, 0} to stop cursor from scrolling down
		wchar_t *msg = L"生活 自由";
		mvaddwstr(5, 5, msg);
		refresh();
		//WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, {0, 0}, &dwBytesWritten);
		getch();
		endwin();
	}
	return 0;
}

