#include "Cell.hpp"

bool Cell::CheckSwitch()
{
	if (!isRevealed)
		isChecked = !isChecked;

	return isChecked;
}

void Cell::Reveal()
{
	if (!isChecked)
		isRevealed = true;
}

Cell::Cell(int16_t x, int16_t y) : x(x), y(y)
{
}
