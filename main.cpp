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
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	const int16_t columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	const int16_t rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	try
	{
		auto menu = Menu(columns, rows);
		menu.Start();
	}
	catch (std::exception& ex)
	{
		throw ex;
	}

	return 0;
}
