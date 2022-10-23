#include "Menu.hpp"

#include <conio.h>
#include <iostream>
#include <string>

#include "Alfabet.hpp"

Menu::Menu()
{
	activeMenu_ = menu_;
}

void Menu::Display(bool clear) const
{
	Print("saper", {15, 0}, NULL, clear);

	for (int i = 0; i < activeMenu_.size(); i++)
	{
		const auto& [str, pos, f] = activeMenu_[i];
		WORD atr = i == cursor_ ? FOREGROUND_RED : NULL;
		Print(str, pos, atr, clear);
	}
}

void Menu::Start()
{
	while (true)
	{
		Display(false);
		switch (const int ch = _getch())
		{
		case 'w':
			cursor_ = (cursor_ - 1) % activeMenu_.size();
			break;
		case 's':
			cursor_ = (cursor_ + 1) % activeMenu_.size();
			break;
		case 'e':
			Use();
			break;
		default:
			break;
		}
	}
}

std::vector<std::vector<bool>> Menu::StringToBlocks(const std::string& str) const
{
	std::vector<std::vector<bool>> string(5);

	for (const auto c : str)
	{
		try
		{
			const auto& ch = alfabet_.at(c);
			for (int i = 0; i < ch.size(); i++)
			{
				const auto& row = ch[i];
				string[i].insert(string[i].end(), row.begin(), row.end());
			}

			for (auto& row : string)
				row.push_back(false);
		}
		catch (...)
		{
		}
	}

	return string;
}

void Menu::Print(const std::string& str, const COORD startingPos, WORD consoleAtribute, bool clear) const
{
	const auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (consoleAtribute != NULL)
		SetConsoleTextAttribute(hConsole, consoleAtribute);
	else
		SetConsoleTextAttribute(hConsole, 7);
	SetConsoleCursorPosition(hConsole, startingPos);
	int16_t i = 1;

	char space = Alfabet::SPACE,
	     block = Alfabet::BLOCK;
	if (clear)
		block = Alfabet::SPACE;

	for (const auto& row : StringToBlocks(str))
	{
		for (const auto cell : row)
		{
			if (cell)
				std::cout << block;
			else
				std::cout << space;
		}
		SetConsoleCursorPosition(hConsole, {startingPos.X, startingPos.Y + i++});
	}
}

void Menu::Use() const
{
	std::get<2>(activeMenu_[cursor_])();
}

void Menu::StartGame()
{
	Display(true);

	auto [x, y, mines] = levels_[level_];
	auto game = Game(x, y, mines);
	game.DisplayBoard(false);
	auto [win, time] = game.Start();
	game.DisplayBoard(true);

	PrintGameOver(false, win, time);
}

void Menu::PrintGameOver(bool clear, bool win, int time)
{
	const std::string result = win ? "rozbrojone" : "game over";
	Print(result, {15, 6}, NULL, clear);
	if (win)
		Print("twoj czas " + std::to_string(time), {15, 12}, NULL, clear);

	activeMenu_ = gameOverMenu_;
	cursor_ = 0;
}
