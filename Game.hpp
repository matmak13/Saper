#pragma once
#include <ChPtrArr.h>
#include <cinttypes>

#include "Board.hpp"

class Game
{
private:
	COORD cursor_;
	COORD checkedCellsPos_;
	HANDLE hConsole;

	Board board_;
	int32_t revealedCells_ = 0;
	int16_t minesLeft_;

	Cell getCell(COORD pos);

	void MoveCursor(COORD newPosition);
	void CheckSwitch();
	void RevealCell();
	void PrintCell(COORD pos);
	void PrintMinesLeft();

	void GameOver(bool win);

public:
	Game(int16_t Xsize, int16_t Ysize, int16_t mines);

	void RevealBoard();
	void GenerateBoard();
	bool Start();
};
