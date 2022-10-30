#include "Game.hpp"

#include <conio.h>
#include <iostream>
#include <random>
#include "Alfabet.hpp"


Game::Game(int16_t Xsize, int16_t Ysize, int16_t mines) :
	cursor_({static_cast<int16_t>(Xsize / 2), static_cast<int16_t>(Ysize / 2)}),
	checkedCellsPos_({Xsize + 3, 1}),
	timerPos_({Xsize + 3, 5}),
	hConsole(GetStdHandle(STD_OUTPUT_HANDLE)),
	board_(Xsize, Ysize, mines),
	minesLeft_(mines)
{
}

void Game::RevealBoard()
{
	board_.RevealBoard();
}

void Game::DisplayBoard(bool clear)
{
	displayMutex_.lock();
	for (int16_t i = 0; i <= board_.ysize_ + 1; i++)
	{
		for (int16_t j = 0; j <= board_.xsize_ + 1; j++)
		{
			SetConsoleCursorPosition(hConsole, {j, i});
			if (clear)
			{
				std::cout << " ";
				continue;
			}

			if (i == 0 || i == board_.ysize_ + 1 || j == 0 || j == board_.xsize_ + 1)
			{
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
				std::cout << Alfabet::BLOCK;
				continue;
			}

			PrintCell({j, i});
		}
	}

	PrintMinesLeft(clear);
	displayMutex_.unlock();
	PrintTimer(clear);
}

std::tuple<bool, int32_t> Game::Start()
{
	while (true)
	{
		if (gameEnded_)
			return {win_, time};

		switch (_getch())
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
			if (!gameStarted_)
			{
				gameStarted_ = true;
				StartTimer();
			}
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
	if (const auto& cell = getCell(cursor_); cell.isRevealed)
		return;

	board_.CheckSwitch(cursor_.X - 1, cursor_.Y - 1) ? minesLeft_-- : minesLeft_++;
	if (minesLeft_ == -1)
	{
		board_.CheckSwitch(cursor_.X - 1, cursor_.Y - 1) ? minesLeft_-- : minesLeft_++;
		return;
	}

	displayMutex_.lock();
	PrintCell(cursor_);
	PrintMinesLeft(false);
	displayMutex_.unlock();
}

void Game::RevealCell()
{
	auto cells = board_.RevealCell(cursor_.X - 1, cursor_.Y - 1);

	for (const auto& cell : cells)
		if (cell.isMine)
			GameOver(false);

	displayMutex_.lock();
	for (auto cell : cells)
		PrintCell({cell.x + 1, cell.y + 1});
	revealedCells_ += cells.size();

	SetConsoleCursorPosition(hConsole, {cursor_.X + 1, cursor_.Y});
	displayMutex_.unlock();

	if (revealedCells_ == board_.ysize_ * board_.xsize_ - board_.mines_)
		GameOver(true);
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
		std::cout << Alfabet::BLOCK;
}

void Game::GameOver(bool win)
{
	gameEnded_ = true;
	timerThread_.join();

	win_ = win;
	if (!win)
		BlowUp();
}

void Game::PrintMinesLeft(bool clear)
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
	SetConsoleCursorPosition(hConsole, checkedCellsPos_);
	if (clear)
		std::cout << "    ";
	else
		std::cout << "MINY";
	SetConsoleCursorPosition(hConsole, {checkedCellsPos_.X, checkedCellsPos_.Y + 1});
	std::cout << "  ";
	SetConsoleCursorPosition(hConsole, {checkedCellsPos_.X, checkedCellsPos_.Y + 1});
	if (!clear)
		std::cout << minesLeft_;

	SetConsoleTextAttribute(hConsole, 7);
	SetConsoleCursorPosition(hConsole, cursor_);
}

void Game::Timer()
{
	while (!gameEnded_)
	{
		time++;
		PrintTimer(false);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1000ms);
	}

	PrintTimer(true);
}

void Game::PrintTimer(bool clear)
{
	displayMutex_.lock();
	SetConsoleTextAttribute(hConsole, 7);
	SetConsoleCursorPosition(hConsole, timerPos_);
	if (clear)
		std::cout << "    ";
	else
		std::cout << "Czas";

	SetConsoleCursorPosition(hConsole, {timerPos_.X, timerPos_.Y + 1});
	if (clear)
		std::cout << "     ";
	else
		std::cout << time;

	SetConsoleCursorPosition(hConsole, cursor_);
	displayMutex_.unlock();
}

void Game::StartTimer()
{
	timerThread_ = std::thread(&Game::Timer, this);
}

void Game::BlowUp()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(25, 50);
	SetConsoleTextAttribute(hConsole, 7);
	const auto mines = board_.allMines_;

	COORD leftTop = cursor_, rightBot = cursor_;
	int booms = 0;
	std::vector boomed(board_.ysize_, std::vector<bool>(board_.xsize_));

	while (booms < board_.xsize_ * board_.ysize_)
	{
		const auto explode = [this, &booms, &boomed](const int16_t start, const int16_t end, const bool horizontal, const int16_t cord)
		{
			for (int16_t i = start; i <= end; i++)
			{
				auto bomed = horizontal ? boomed[cord - 1][i - 1] : boomed[i - 1][cord - 1];
				if (bomed)
					continue;

				const Cell& cell = horizontal ? board_.board_[cord - 1][i - 1] : board_.board_[i - 1][cord - 1];
				SetConsoleCursorPosition(hConsole, {cell.x + 1, cell.y + 1});
				std::cout << "*";
				booms++;
				bomed = true;
			}
		};

		explode(leftTop.X, rightBot.X, true, leftTop.Y);
		explode(leftTop.X, rightBot.X, true, rightBot.Y);
		explode(leftTop.Y, rightBot.Y, false, leftTop.X);
		explode(leftTop.Y, rightBot.Y, false, rightBot.X);

		using namespace std::chrono_literals;

		std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));

		if (leftTop.X - 1 > 0)leftTop.X--;
		if (leftTop.Y - 1 > 0)leftTop.Y--;
		if (rightBot.X + 1 <= board_.xsize_)rightBot.X++;
		if (rightBot.Y + 1 <= board_.ysize_)rightBot.Y++;
	}
}
