#include "Board.hpp"

#include <random>

Board::Board(int16_t Xsize, int16_t Ysize, int16_t mines) : xsize_(Xsize), ysize_(Ysize), mines_(mines)
{
	// generate board
	for (int16_t i = 0; i < Ysize; i++)
	{
		std::vector<Cell> temp;
		for (int16_t j = 0; j < Xsize; j++)
			temp.emplace_back(j, i);

		board_.push_back(temp);
	}

	// generete random mines
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distX(0, Xsize - 1), distY(0, Ysize - 1);

	int numOfMines = 0;
	while (numOfMines < mines)
	{
		const auto x = distX(gen), y = distY(gen);

		if (auto& cell = board_[y][x]; !cell.isMine)
		{
			numOfMines++;
			cell.isMine = true;
			allMines_.push_back(cell);

			// set number of mines in the cells around the mine
			for (int16_t Y = y - 1; Y <= y + 1; Y++)
			{
				if (Y < 0 || Y >= Ysize)
					continue;

				for (int16_t X = x - 1; X <= x + 1; X++)
				{
					if (X < 0 || X >= Xsize || (Y == y && X == x))
						continue;

					auto& cellAround = board_[Y][X];
					cellAround.minesAround++;
				}
			}
		}
	}
}

bool Board::CheckSwitch(int16_t x, int16_t y)
{
	return board_[y][x].CheckSwitch();
}

std::vector<Cell> Board::RevealCell(int16_t x, int16_t y)
{
	auto& cell = board_[y][x];
	std::vector<Cell> revealedCells{};

	if (cell.isChecked)
		return revealedCells;

	if (cell.isRevealed)
	{
		auto checkedCellsAround = GetCellsAround(x, y, [](Cell cell) { return cell.isChecked; }).size();
		if (checkedCellsAround != cell.minesAround)
			return revealedCells;

		auto uncheckedCellsAround = GetCellsAround(x, y, [](Cell cell) { return !cell.isChecked && !cell.isRevealed; });
		for (auto& cellAround : uncheckedCellsAround)
		{
			auto cells = RevealCell(cellAround.x, cellAround.y);
			revealedCells.insert(revealedCells.end(), cells.begin(), cells.end());
		}

		return revealedCells;
	}

	cell.Reveal();
	revealedCells.push_back(cell);

	if (cell.isMine)
		return revealedCells;

	if (cell.minesAround > 0)
		return revealedCells;

	auto cellsAround = GetCellsAround(x, y, [](Cell cell) { return !cell.isRevealed && !cell.isChecked; });
	for (auto& cellAround : cellsAround)
	{
		if (cellAround.isChecked)
			continue;

		auto cells = RevealCell(cellAround.x, cellAround.y);
		revealedCells.insert(revealedCells.end(), cells.begin(), cells.end());
	}

	return revealedCells;
}

std::vector<Cell> Board::GetCellsAround(int16_t x, int16_t y, bool (*predicate)(Cell)) const
{
	std::vector<Cell> cells{};

	for (int16_t Y = y - 1; Y <= y + 1; Y++)
	{
		if (Y < 0 || Y >= ysize_)
			continue;

		for (int16_t X = x - 1; X <= x + 1; X++)
		{
			if (X < 0 || X >= xsize_ || (Y == y && X == x))
				continue;

			auto& cell = board_[Y][X];
			if (predicate(cell))
				cells.push_back(cell);
		}
	}

	return cells;
}

void Board::RevealBoard()
{
	for (auto& row : board_)
		for (auto& cell : row)
			cell.Reveal();
}
