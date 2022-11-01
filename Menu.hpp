#pragma once
#include <vector>
#include <functional>
#include "Alfabet.hpp"
#include "Game.hpp"

class Menu
{
private:
	using MenuItem = std::tuple<std::string*, COORD, std::function<void()>>;

	bool stop_ = false;
	uint32_t cols_;
	uint32_t rows_;
	Alfabet alfabet_{};
	uint32_t level_ = 0;
	std::vector<std::tuple<int16_t, int16_t, int16_t>> levels_{
		{8, 8, 10},
		{16, 16, 40},
		{30, 16, 99}
	};

	uint32_t cursor_ = 0;
	std::vector<MenuItem> activeMenu_;
	std::vector<MenuItem> menu_;
	std::vector<MenuItem> difficultyMenu_;
	std::vector<MenuItem> gameOverMenu_;

	std::vector<std::vector<bool>> StringToBlocks(const std::string& str) const;
	void Print(const std::string& str, COORD startingPos, WORD consoleAtribute, bool clear) const;
	void Display(bool clear) const;
	void Use() const;
	void StartGame(std::tuple<int16_t, int16_t, int16_t> level);
	void PrintGameOver(bool clear, bool win, int time);
	std::tuple<int16_t, int16_t, int16_t> CustomLevel();
public:
	Menu(int16_t cols, int16_t rows);
	~Menu();

	void Start();
};
