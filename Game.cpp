#include "Game.hpp"

#include <conio.h>
#include <iostream>
#include <random>


Game::Game(int16_t Xsize, int16_t Ysize, int16_t mines) :
	cursor_({static_cast<int16_t>(Xsize / 2), static_cast<int16_t>(Ysize / 2)}),
	checkedCellsPos_({Xsize + 3, 1}),
	hConsole(GetStdHandle(STD_OUTPUT_HANDLE)),
	board_(Xsize, Ysize, mines),
	minesLeft_(mines)
{
}

void Game::RevealBoard()
{
	board_.RevealBoard();
}

void Game::GenerateBoard()
{
	for (int16_t i = 0; i <= board_.ysize_ + 1; i++)
	{
		for (int16_t j = 0; j <= board_.xsize_ + 1; j++)
		{
			SetConsoleCursorPosition(hConsole, {j, i});

			if (i == 0 || i == board_.ysize_ + 1 || j == 0 || j == board_.xsize_ + 1)
			{
				SetConsoleTextAttribute(hConsole, 7);
				std::cout << '#';
				continue;
			}

			PrintCell({j, i});
		}
	}

	PrintMinesLeft();
}

bool Game::Start()
{
	while (true)
	{
		switch (const int ch = _getch())
		{
		case 'w':
			MoveCursor({cursor_.X, static_cast<int16_t>(cursor_.Y - 1)});
			break;
		case 'a':
			MoveCursor({static_cast<int16_t>(cursor_.X - 1), cursor_.Y});
			break;
		case 's':
			MoveCursor({cursor_.X, static_cast<int16_t>(cursor_.Y + 1)});
			break;
		case 'd':
			MoveCursor({static_cast<int16_t>(cursor_.X + 1), cursor_.Y});
			break;
		case 'q':
			CheckSwitch();
			break;
		case 'e':
			RevealCell();
			break;
		default:
			break;
		}
	}
}

void Game::MoveCursor(COORD newPosition)
{
	if (newPosition.X > board_.xsize_ || newPosition.X == 0 ||
		newPosition.Y > board_.ysize_ || newPosition.Y == 0)
		return;

	const auto oldCursor = cursor_;
	cursor_ = newPosition;

	PrintCell(oldCursor);
	PrintCell(cursor_);
}

void Game::CheckSwitch()
{
	board_.CheckSwitch(cursor_.X - 1, cursor_.Y - 1) ? minesLeft_-- : minesLeft_++;
	if (minesLeft_ == -1)
	{
		board_.CheckSwitch(cursor_.X - 1, cursor_.Y - 1) ? minesLeft_-- : minesLeft_++;
		return;
	}

	PrintCell(cursor_);
	PrintMinesLeft();
}

void Game::RevealCell()
{
	auto cells = board_.RevealCell(cursor_.X - 1, cursor_.Y - 1);
	if (!cells.empty() && cells[0].isMine)
		GameOver(false);

	for (auto cell : cells)
		PrintCell({cell.x + 1, cell.y + 1});
	revealedCells_ += cells.size();

	if (revealedCells_ == board_.ysize_ * board_.xsize_ - board_.mines_)
		GameOver(true);

	SetConsoleCursorPosition(hConsole, {cursor_.X + 1, cursor_.Y});
}

Cell Game::getCell(COORD pos)
{
	return board_.board_[pos.Y - 1][pos.X - 1];
}

void Game::PrintCell(COORD pos)
{
	const auto cell = getCell(pos);

	SetConsoleCursorPosition(hConsole, pos);

	if (cell.isChecked)
	{
		if (cursor_.X == pos.X && cursor_.Y == pos.Y)
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		else
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
	}
	else
	{
		if (cursor_.X == pos.X && cursor_.Y == pos.Y)
			SetConsoleTextAttribute(hConsole, 13);
		else if (!cell.isRevealed)
			SetConsoleTextAttribute(hConsole, 7);
		else
		{
			if (cell.minesAround == 0)
				SetConsoleTextAttribute(hConsole, 7);
			else if (cell.minesAround == 1)
				SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_BLUE);
			else if (cell.minesAround == 2)
				SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			else if (cell.minesAround == 3)
				SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED);
			else if (cell.minesAround == 4)
				SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
			else if (cell.minesAround == 5)
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
			else if (cell.minesAround == 6)
				SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE);
			else if (cell.minesAround == 7)
				SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
			else if (cell.minesAround == 8)
				SetConsoleTextAttribute(
					hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
	}

	if (cell.isRevealed)
	{
		if (cell.isMine)
			std::cout << '*';
		if (cell.minesAround == 0 && cursor_.X == pos.X && cursor_.Y == pos.Y)
			std::cout << '-';
		else
			cell.minesAround > 0 ? std::cout << cell.minesAround : std::cout << ' ';
	}
	else
		std::cout << 'Û';
}

void Game::GameOver(bool win)
{
	throw std::exception("konec");
}

void Game::PrintMinesLeft()
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
	SetConsoleCursorPosition(hConsole, checkedCellsPos_);
	std::cout << "MINY";
	SetConsoleCursorPosition(hConsole, {checkedCellsPos_.X, checkedCellsPos_.Y + 1});
	std::cout << minesLeft_;

	SetConsoleTextAttribute(hConsole, 7);
	SetConsoleCursorPosition(hConsole, cursor_);
}
