#pragma once
#include <map>
#include <vector>
#include <functional>
#include "Alfabet.hpp"
#include "Game.hpp"

class Menu
{
private:
	Alfabet alfabet_{};
	uint32_t level_ = 0;
	std::vector<std::tuple<int16_t, int16_t, int16_t>> levels_{
		{8, 8, 10},
		{16, 16, 40},
		{30, 16, 99}
	};

	uint32_t cursor_ = 0;
	std::vector<std::tuple<std::string, COORD, std::function<void()>>>& activeMenu_ = menu_;
	std::vector<std::tuple<std::string, COORD, std::function<void()>>> menu_{
		{
			"start", COORD(15, 6), [this]
			{
				this->Display(true);
				this->activeMenu_ = this->difficultyMenu;
			}
		},
		{
			"opcje", COORD(15, 12), [this]
			{
				this->Display(true);
				//this->activeMenu_ = this->settingsMenu;
			}
		},
		{
			"wyjscie", COORD(12, 18), [this]
			{
				exit(0);
			}
		},
	};
	std::vector<std::tuple<std::string, COORD, std::function<void()>>> difficultyMenu{
		{
			"latwy", COORD(14, 6), [this]
			{
				this->level_ = 0;
				this->StartGame();
			}
		},
		{
			"sredni", COORD(13, 12), [this]
			{
				this->level_ = 1;
				this->StartGame();
			}
		},
		{
			"trudny", COORD(12, 18), [this]
			{
				this->level_ = 2;
				this->StartGame();
			}
		},
	};
	std::vector<std::tuple<std::string, COORD, std::function<void()>>> settingsMenu{};

	std::vector<std::vector<bool>> StringToBlocks(const std::string& str) const;
	void Print(const std::string& str, COORD startingPos, WORD consoleAtribute, bool clear) const;
	void Display(bool clear) const;
	void Use() const;
	void StartGame();
public:
	Menu() = default;

	void Start();
};
