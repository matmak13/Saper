#include <iostream>

#include "Game.hpp"
#include "Menu.hpp"

void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

int main()
{
	ShowConsoleCursor(false);

	try
	{
		auto menu = Menu();
		menu.Start();
	}
	catch (std::exception& ex)
	{
		throw ex;
	}

	return 0;
}
