#include "Menu.hpp"

#include <conio.h>
#include <iostream>
#include <string>

#include "Alfabet.hpp"

Menu::Menu()
{
	activeMenu_ = menu_;
}

void Menu::Display(const bool clear) const
{
	Print("saper", {15, 0}, NULL, clear);

	for (uint32_t i = 0; i < activeMenu_.size(); i++)
	{
		const auto& [str, pos, f] = activeMenu_[i];
		const WORD atr = i == cursor_ ? FOREGROUND_RED : NULL;
		Print(str, pos, atr, clear);
	}
}

void Menu::Start()
{
	while (true)
	{
		Display(false);

		switch (_getch())
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
	Alfabet::Letter string(5);

	for (const auto c : str)
	{
		Alfabet::Letter ch;

		try
		{
			ch = alfabet_.at(c);
		}
		catch (...)
		{
			ch = alfabet_.at(NULL);
		}

		for (uint32_t i = 0; i < ch.size(); i++)
		{
			const auto& row = ch[i];
			string[i].insert(string[i].end(), row.begin(), row.end());
		}

		for (auto& row : string)
			row.push_back(false);
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

	int16_t Y = 1;
	constexpr char space = Alfabet::SPACE,
	               block = Alfabet::BLOCK;

	for (const auto& row : StringToBlocks(str))
	{
		for (const auto cell : row)
		{
			if (!clear && cell)
				std::cout << block;
			else
				std::cout << space;
		}

		SetConsoleCursorPosition(hConsole, {startingPos.X, startingPos.Y + Y++});
	}
}

void Menu::Use() const
{
	std::get<2>(activeMenu_[cursor_])();
}

void Menu::StartGame()
{
	Display(true);

	auto& [x, y, mines] = levels_[level_];
	auto game = Game(x, y, mines);
	game.DisplayBoard(false);
	auto [win, time] = game.Start();
	game.DisplayBoard(true);

	PrintGameOver(false, win, time);
}

void Menu::PrintGameOver(const bool clear, const bool win, const int time)
{
	const std::string result = win ? "rozbrojone" : "game over";
	Print(result, {5, 6}, NULL, clear);
	if (win)
		Print("twoj czas " + std::to_string(time), {3, 12}, NULL, clear);

	activeMenu_ = gameOverMenu_;
	cursor_ = 0;
}
