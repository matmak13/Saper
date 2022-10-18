#pragma once
#include <iostream>
#include <Windows.h>
#include <conio.h>

constexpr short X = 25, Y = 25;
COORD cursor = { X / 2,Y / 2 };
const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void setCursor(COORD newCursor)
{
	if (newCursor.X > X - 2 || newCursor.X == 0 ||
		newCursor.Y > Y - 2 || newCursor.Y == 0)
		return;

	SetConsoleTextAttribute(hConsole, 7);
	SetConsoleCursorPosition(hConsole, cursor);
	std::cout << '?';

	SetConsoleCursorPosition(hConsole, newCursor);
	SetConsoleTextAttribute(hConsole, 12);
	std::cout << '?';

	cursor = newCursor;
}

void dosmth()
{
	SetConsoleTextAttribute(hConsole, 7);
	SetConsoleCursorPosition(hConsole, cursor);
	std::cout << '.';
}

void Board()
{
	for (int16_t i = 0; i < Y; i++)
	{
		for (int16_t j = 0; j < X; j++)
		{
			SetConsoleCursorPosition(hConsole, { j,i });

			if (i == 0 || i == Y - 1 || j == 0 || j == X - 1)
				std::cout << '#';
			else
				std::cout << '?';
		}
	}

	while (true) {
		switch (const int ch = _getch())
		{
		case 'w':
			setCursor({ cursor.X, cursor.Y - 1 });
			break;
		case 'a':
			setCursor({ cursor.X - 1, cursor.Y });
			break;
		case 's':
			setCursor({ cursor.X, cursor.Y + 1 });
			break;
		case 'd':
			setCursor({ cursor.X + 1, cursor.Y });
			break;
		case ' ':
			dosmth();
			break;
		default:
			break;
		}
	}
}