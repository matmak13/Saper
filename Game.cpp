#include "Game.hpp"

#include <conio.h>
#include <iostream>
#include <random>
#include "Alfabet.hpp"


Game::Game(int16_t Xsize, int16_t Ysize, int16_t mines, COORD leftUpperCorner) :
	cursor_({int16_t(Xsize / 2 + leftUpperCorner.X), int16_t(Ysize / 2 + leftUpperCorner.Y)}),
	checkedCellsPos_({int16_t(leftUpperCorner.X - 10), 1}),
	timerPos_({int16_t(leftUpperCorner.X - 10), 5}),
	hConsole_(GetStdHandle(STD_OUTPUT_HANDLE)),
	board_(Xsize, Ysize, mines),
	leftUpperCorner_(leftUpperCorner),
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
			SetConsoleCursorPosition(hConsole_, {j + leftUpperCorner_.X, i + leftUpperCorner_.Y});
			if (clear)
			{
				std::cout << " ";
				continue;
			}

			if (i == 0 || i == board_.ysize_ + 1 || j == 0 || j == board_.xsize_ + 1)
			{
				SetConsoleTextAttribute(hConsole_, FOREGROUND_RED);
				std::cout << Alfabet::BLOCK;
				continue;
			}

			PrintCell({j + leftUpperCorner_.X, i + leftUpperCorner_.Y});
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
			return {win_, time_};

		switch (_getch())
		{
		case 'w':
		case 'W':
			MoveCursor({cursor_.X, static_cast<int16_t>(cursor_.Y - 1)});
			break;
		case 'a':
		case 'A':
			MoveCursor({static_cast<int16_t>(cursor_.X - 1), cursor_.Y});
			break;
		case 's':
		case 'S':
			MoveCursor({cursor_.X, static_cast<int16_t>(cursor_.Y + 1)});
			break;
		case 'd':
		case 'D':
			MoveCursor({static_cast<int16_t>(cursor_.X + 1), cursor_.Y});
			break;
		case 'q':
		case 'Q':
			CheckSwitch();
			break;
		case 'e':
		case 'E':
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
	if (newPosition.X > board_.xsize_ + leftUpperCorner_.X || newPosition.X == leftUpperCorner_.X ||
		newPosition.Y > board_.ysize_ + leftUpperCorner_.Y || newPosition.Y == leftUpperCorner_.Y)
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

	board_.CheckSwitch(cursor_.X - 1 - leftUpperCorner_.X, cursor_.Y - 1 - leftUpperCorner_.Y) ? minesLeft_-- : minesLeft_++;
	if (minesLeft_ == -1)
	{
		board_.CheckSwitch(cursor_.X - 1 - leftUpperCorner_.X, cursor_.Y - 1 - leftUpperCorner_.Y) ? minesLeft_-- : minesLeft_++;
		return;
	}

	displayMutex_.lock();
	PrintCell(cursor_);
	PrintMinesLeft(false);
	displayMutex_.unlock();
}

void Game::RevealCell()
{
	auto cells = board_.RevealCell(cursor_.X - 1 - leftUpperCorner_.X, cursor_.Y - 1 - leftUpperCorner_.Y);

	for (const auto& cell : cells)
		if (cell.isMine)
		{
			GameOver(false);
			return;
		}

	displayMutex_.lock();
	for (auto cell : cells)
		PrintCell({cell.x + 1 + leftUpperCorner_.X, cell.y + 1 + leftUpperCorner_.Y});
	revealedCells_ += cells.size();

	SetConsoleCursorPosition(hConsole_, {cursor_.X + 1, cursor_.Y});
	displayMutex_.unlock();

	if (revealedCells_ == board_.ysize_ * board_.xsize_ - board_.mines_)
		GameOver(true);
}

Cell Game::getCell(COORD pos)
{
	return board_.board_[pos.Y - 1 - leftUpperCorner_.Y][pos.X - 1 - leftUpperCorner_.X];
}

void Game::PrintCell(COORD pos)
{
	const auto cell = getCell(pos);

	SetConsoleCursorPosition(hConsole_, pos);

	if (cell.isChecked)
	{
		if (cursor_.X == pos.X && cursor_.Y == pos.Y)
			SetConsoleTextAttribute(hConsole_, FOREGROUND_RED);
		else
			SetConsoleTextAttribute(hConsole_, FOREGROUND_RED | FOREGROUND_INTENSITY);
	}
	else
	{
		if (cursor_.X == pos.X && cursor_.Y == pos.Y)
			SetConsoleTextAttribute(hConsole_, 13);
		else if (!cell.isRevealed)
			SetConsoleTextAttribute(hConsole_, 7);
		else
		{
			if (cell.minesAround == 0)
				SetConsoleTextAttribute(hConsole_, 7);
			else if (cell.minesAround == 1)
				SetConsoleTextAttribute(hConsole_, FOREGROUND_INTENSITY | FOREGROUND_BLUE);
			else if (cell.minesAround == 2)
				SetConsoleTextAttribute(hConsole_, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			else if (cell.minesAround == 3)
				SetConsoleTextAttribute(hConsole_, FOREGROUND_INTENSITY | FOREGROUND_RED);
			else if (cell.minesAround == 4)
				SetConsoleTextAttribute(hConsole_, FOREGROUND_BLUE);
			else if (cell.minesAround == 5)
				SetConsoleTextAttribute(hConsole_, FOREGROUND_RED);
			else if (cell.minesAround == 6)
				SetConsoleTextAttribute(hConsole_, FOREGROUND_GREEN | FOREGROUND_BLUE);
			else if (cell.minesAround == 7)
				SetConsoleTextAttribute(hConsole_, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
			else if (cell.minesAround == 8)
				SetConsoleTextAttribute(
					hConsole_, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
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

	std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void Game::PrintMinesLeft(bool clear)
{
	SetConsoleTextAttribute(hConsole_, FOREGROUND_RED);
	SetConsoleCursorPosition(hConsole_, checkedCellsPos_);
	if (clear)
		std::cout << "    ";
	else
		std::cout << "MINY";
	SetConsoleCursorPosition(hConsole_, {checkedCellsPos_.X, checkedCellsPos_.Y + 1});
	std::cout << "   ";
	SetConsoleCursorPosition(hConsole_, {checkedCellsPos_.X, checkedCellsPos_.Y + 1});
	if (!clear)
		std::cout << minesLeft_;

	SetConsoleTextAttribute(hConsole_, 7);
	SetConsoleCursorPosition(hConsole_, cursor_);
}

void Game::Timer()
{
	while (!gameEnded_)
	{
		time_++;
		PrintTimer(false);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1000ms);
	}

	PrintTimer(true);
}

void Game::PrintTimer(bool clear)
{
	displayMutex_.lock();
	SetConsoleTextAttribute(hConsole_, 7);
	SetConsoleCursorPosition(hConsole_, timerPos_);
	if (clear)
		std::cout << "    ";
	else
		std::cout << "Czas";

	SetConsoleCursorPosition(hConsole_, {timerPos_.X, timerPos_.Y + 1});
	if (clear)
		std::cout << "     ";
	else
		std::cout << time_;

	SetConsoleCursorPosition(hConsole_, cursor_);
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
	SetConsoleTextAttribute(hConsole_, 7);
	const auto mines = board_.allMines_;

	COORD leftTop = {cursor_.X - leftUpperCorner_.X, cursor_.Y - leftUpperCorner_.Y},
	      rightBot = {cursor_.X - leftUpperCorner_.X, cursor_.Y - leftUpperCorner_.Y};
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
				SetConsoleCursorPosition(hConsole_, {cell.x + 1 + leftUpperCorner_.X, cell.y + 1 + leftUpperCorner_.Y});
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
