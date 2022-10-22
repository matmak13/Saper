#pragma once
#include <ChPtrArr.h>
#include <cinttypes>
#include <mutex>
#include <thread>

#include "Board.hpp"

class Game
{
private:
	COORD cursor_;
	COORD checkedCellsPos_;
	COORD timerPos_;
	HANDLE hConsole;

	Board board_;
	int32_t revealedCells_ = 0;
	int16_t minesLeft_;

	bool gameStarted_ = false;
	bool win_ = false;
	std::atomic_bool gameEnded_;

	int32_t time = 0;
	std::mutex displayMutex_;
	std::thread timerThread_;

	Cell getCell(COORD pos);

	void MoveCursor(COORD newPosition);
	void CheckSwitch();
	void RevealCell();
	void PrintCell(COORD pos);
	void PrintMinesLeft(bool clear);

	void GameOver(bool win);
	void BlowUp();

	void StartTimer();
	void Timer();
	void PrintTimer(bool clear);
public:
	Game(int16_t Xsize, int16_t Ysize, int16_t mines);

	void RevealBoard();
	void DisplayBoard(bool clear);
	std::tuple<bool, int32_t> Start();
};
