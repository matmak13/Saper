#include "Menu.hpp"

#include <conio.h>
#include <iostream>
#include <string>

#include "Alfabet.hpp"
#include "Game.hpp"

Menu::Menu(const int16_t cols, const int16_t rows) : cols_(cols), rows_(rows)
{
	menu_ = {
		{
			new std::string{"start"}, COORD(cols / 2, 6), [this]
			{
				this->Display(true);
				this->activeMenu_ = this->difficultyMenu_;
			}
		},
		{
			new std::string{"wyjscie"}, COORD(cols / 2, 12), [this]
			{
				stop_ = true;
			}
		},
	};
	difficultyMenu_ = {
		{
			new std::string{"latwy"}, COORD(cols / 2, 6), [this]
			{
				Display(true);
				this->StartGame(levels_[0]);
			}
		},
		{
			new std::string{"sredni"}, COORD(cols / 2, 12), [this]
			{
				Display(true);
				this->StartGame(levels_[1]);
			}
		},
		{
			new std::string{"trudny"}, COORD(cols / 2, 18), [this]
			{
				Display(true);
				this->StartGame(levels_[2]);
			}
		},
		{
			new std::string{"niestandardowy"}, COORD(cols / 2, 24), [this]
			{
				Display(true);
				this->StartGame(CustomLevel());
			}
		},
	};
	gameOverMenu_ = {
		{
			new std::string{"wyjscie"}, COORD(cols / 2, 18), [this]
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
		Print(*str, pos, atr, clear);
	}
}

bool Menu::checkIfControlUsed(controls control, int ch, std::vector<std::tuple<controls, std::vector<int>>> controls)
{
	const auto& vec = std::get<1>(controls.at(control));
	return std::ranges::find(vec, ch) != vec.end();
}


bool Menu::checkIfControlUsed(controls control, const int ch) const
{
	return checkIfControlUsed(control, ch, controls_);
}

int Menu::getKey()
{
	auto ch = _getch();
	if (ch == 224)
		ch += _getch();

	return ch;
}

void Menu::Start()
{
	while (!stop_)
	{
		Display(false);

		const auto ch = getKey();

		controls key = none;
		if (checkIfControlUsed(up, ch))
			key = up;
		else if (checkIfControlUsed(down, ch))
			key = down;
		else if (checkIfControlUsed(use, ch))
			key = use;

		switch (key)
		{
		case up:
			cursor_ = (cursor_ - 1) % activeMenu_.size();
			break;
		case down:
			cursor_ = (cursor_ + 1) % activeMenu_.size();
			break;
		case use:
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
	const auto blocks = StringToBlocks(str);
	SetConsoleCursorPosition(hConsole, {int16_t(startingPos.X - blocks.front().size() / 2), startingPos.Y});

	for (const auto& row : blocks)
	{
		for (const auto cell : row)
		{
			if (!clear && cell)
				std::cout << Alfabet::BLOCK;
			else
				std::cout << Alfabet::SPACE;
		}

		SetConsoleCursorPosition(hConsole, {int16_t(startingPos.X - blocks.front().size() / 2), int16_t(startingPos.Y + Y++)});
	}
}

void Menu::Use() const
{
	std::get<2>(activeMenu_[cursor_])();
}

void Menu::StartGame(std::tuple<int16_t, int16_t, int16_t> level)
{
	auto& [x, y, mines] = level;
	auto game = Game(x, y, mines, COORD{int16_t(cols_ / 2), 0}, controls_);
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

std::tuple<int16_t, int16_t, int16_t> Menu::CustomLevel()
{
	int32_t col = 8, row = 8, mine = 10;
	const std::vector variables{&col, &row, &mine};
	const auto limit = [this, &col, &row, &mine]
	{
		if (cursor_ != 3)
		{
			if (col + 1 > 30)
				col = 30;
			if (col < 8)
				col = 8;
			if (row + 1 > 24)
				row = 24;
			if (row < 8)
				row = 8;
			if (mine + 1 > col * row - 1)
				mine = col * row - 1;
			if (mine < col * row * 0.1)
				mine = col * row * 0.1;
			Use();
		}
	};
	std::string cols{"kolumny " + std::to_string(col)},
	            rows{"wiersze " + std::to_string(row)},
	            mines{"miny " + std::to_string(mine)},
	            start{"start"};
	auto ready = false, cursorMoved = true;
	cursor_ = 0;
	std::vector<std::tuple<std::string*, COORD, std::function<void()>>> customLevelMenu = {
		{
			&cols, COORD(cols_ / 2, 6), [this, &cols, &col, &customLevelMenu]
			{
				Print(cols, std::get<1>(customLevelMenu[0]), NULL, true);
				cols = "kolumny " + std::to_string(col);
			}
		},
		{
			&rows, COORD(cols_ / 2, 12), [this, &rows, &row, &customLevelMenu]
			{
				Print(rows, std::get<1>(customLevelMenu[1]), NULL, true);
				rows = "wiersze " + std::to_string(row);
			}
		},
		{
			&mines, COORD(cols_ / 2, 18), [this, &mines, &mine, &col, &row, &customLevelMenu]
			{
				Print(mines, std::get<1>(customLevelMenu[2]), NULL, true);
				mines = "miny " + std::to_string(mine);
			}
		},
		{
			&start, COORD(cols_ / 2, 24), [&ready] { ready = true; }
		}
	};
	activeMenu_ = customLevelMenu;

	while (!ready)
	{
		Display(false);
		const auto ch = getKey();
		if ('0' <= ch && ch <= '9')
		{
			const auto value = ch - 48;

			const auto val = variables[cursor_];
			if (cursorMoved)
				*val = 0;
			if (*val * 10 >= 0)
			{
				*val *= 10;
				*val += value;
			}

			cursorMoved = false;
			Use();
		}
		controls key = none;
		if (checkIfControlUsed(up, ch))
			key = up;
		else if (checkIfControlUsed(down, ch))
			key = down;
		else if (checkIfControlUsed(use, ch))
			key = use;
		switch (key)
		{
		case up:
			limit();
			cursor_ = (cursor_ - 1) % activeMenu_.size();
			cursorMoved = true;
			break;
		case down:
			limit();
			cursor_ = (cursor_ + 1) % activeMenu_.size();
			cursorMoved = true;
			break;
		case use:
			Use();
			break;
		default:
			break;
		}
	}

	Display(true);
	return {col, row, mine};
}

Menu::~Menu()
{
	for (auto& [ptr, coord, func] : menu_)
		delete ptr;
	for (auto& [ptr, coord, func] : difficultyMenu_)
		delete ptr;
	for (auto& [ptr, coord, func] : gameOverMenu_)
		delete ptr;
}
