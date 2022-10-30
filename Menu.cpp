#include "Menu.hpp"

#include <conio.h>
#include <iostream>
#include <string>

#include "Alfabet.hpp"

Menu::Menu(const int16_t cols, const int16_t rows) : cols_(cols), rows_(rows)
{
	auto str1 = StringToBlocks("start");
	auto str2 = StringToBlocks("wyjscie");
	menu_ = {
		{
			"start", COORD(cols / 2, 6), [this]
			{
				this->Display(true);
				this->activeMenu_ = this->difficultyMenu_;
			}
		},
		{
			"wyjscie", COORD(cols / 2, 12), [this]
			{
				exit(0);
			}
		},
	};
	difficultyMenu_ = {
		{
			"latwy", COORD(cols / 2, 6), [this]
			{
				this->level_ = 0;
				this->StartGame();
			}
		},
		{
			"sredni", COORD(cols / 2, 12), [this]
			{
				this->level_ = 1;
				this->StartGame();
			}
		},
		{
			"trudny", COORD(cols / 2, 18), [this]
			{
				this->level_ = 2;
				this->StartGame();
			}
		},
	};
	gameOverMenu_ = {
		{
			"wyjscie", COORD(cols / 2, 18), [this]
			{
				this->Display(true);
				this->PrintGameOver(true, true, INT32_MAX);
				this->activeMenu_ = this->menu_;
			}
		},
	};
	activeMenu_ = menu_;
}

void Menu::Display(const bool clear) const
{
	Print("saper", {int16_t(cols_ / 2), 0}, NULL, clear);

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
		case 'W':
			cursor_ = (cursor_ - 1) % activeMenu_.size();
			break;
		case 's':
		case 'S':
			cursor_ = (cursor_ + 1) % activeMenu_.size();
			break;
		case 'e':
		case 'E':
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

	int16_t Y = 1;
	constexpr char space = Alfabet::SPACE,
	               block = Alfabet::BLOCK;
	const auto blocks = StringToBlocks(str);
	SetConsoleCursorPosition(hConsole, {int16_t(startingPos.X - blocks.front().size() / 2), startingPos.Y});

	for (const auto& row : blocks)
	{
		for (const auto cell : row)
		{
			if (!clear && cell)
				std::cout << block;
			else
				std::cout << space;
		}

		SetConsoleCursorPosition(hConsole, {int16_t(startingPos.X - blocks.front().size() / 2), int16_t(startingPos.Y + Y++)});
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
	auto game = Game(x, y, mines, COORD{int16_t(cols_ / 2), 0});
	game.DisplayBoard(false);
	auto [win, time] = game.Start();
	game.DisplayBoard(true);

	PrintGameOver(false, win, time);
}

void Menu::PrintGameOver(const bool clear, const bool win, const int time)
{
	const std::string result = win ? "rozbrojone" : "game over";
	Print(result, {int16_t(cols_ / 2), 6}, NULL, clear);
	if (win)
		Print("twoj czas " + std::to_string(time), {int16_t(cols_ / 2), 12}, NULL, clear);

	activeMenu_ = gameOverMenu_;
	cursor_ = 0;
}
