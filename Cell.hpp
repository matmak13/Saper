#pragma once
#include <cstdint>

class Cell
{
public:
	bool isRevealed = false;
	bool isMine = false;
	bool isChecked = false;
	int16_t minesAround{};
	int16_t x{};
	int16_t y{};

	Cell(int16_t x, int16_t y);

	bool CheckSwitch();
	void Reveal();
};
