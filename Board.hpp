#pragma once
#include <cinttypes>
#include <functional>
#include <vector>
#include "Cell.hpp"

class Board
{
public:
	int16_t xsize_;
	int16_t ysize_;
	/**
	 * \brief ilość min
	 */
	int16_t mines_;
	std::vector<std::vector<Cell>> board_;
	/**
	 * \brief tablica wszystkich min
	 */
	std::vector<Cell> allMines_;

	Board(int16_t Xsize, int16_t Ysize, int16_t mines);

	bool CheckSwitch(int16_t x, int16_t y);
	std::vector<Cell> RevealCell(int16_t x, int16_t y);
	/**
	 * \brief Przeszukuje komórki dookoła podanej pozycji 
	 * \param x X
	 * \param y Y
	 * \param predicate warunek jaki musi spełniać komórka
	 * \return komórki spełniające ten warunek
	 */
	std::vector<Cell> GetCellsAround(int16_t x, int16_t y, std::function<bool(Cell)> predicate) const;
};
