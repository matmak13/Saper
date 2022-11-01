#pragma once
#include <cinttypes>
#include <mutex>
#include <thread>

#include "Board.hpp"
#include "Menu.hpp"

class Game
{
private:
	COORD cursor_;
	/**
	 * \brief Pozycja wypisywania min
	 */
	COORD checkedCellsPos_;
	/**
	 * \brief Pozycja timera
	 */
	COORD timerPos_;
	/**
	 * \brief Pozycja lewego górnego rogu planszy
	 */
	COORD leftUpperCorner_;
	HANDLE hConsole_;
	/**
	 * \brief Kontrolki otrzymywane z menu
	 */
	std::vector<std::tuple<Menu::controls, std::vector<int>>> controls_;

	Board board_;
	int32_t revealedCells_ = 0;
	int16_t minesLeft_;

	bool gameStarted_ = false;
	bool win_ = false;
	std::atomic_bool gameEnded_;

	int32_t time_ = 0;
	std::mutex displayMutex_;
	std::thread timerThread_;

	Cell getCell(COORD pos);

	void MoveCursor(COORD newPosition);
	/**
	 * \brief Przełączenie zaznaczenia komórki
	 */
	void CheckSwitch();
	/**
	 * \brief Odkrycie komórki
	 */
	void RevealCell();
	/**
	 * \brief Wypisanie komórki na ekran
	 * \param pos pozycja komórki
	 */
	void PrintCell(COORD pos);
	void PrintMinesLeft(bool clear);

	void GameOver(bool win);
	void BlowUp();

	void StartTimer();
	void Timer();
	void PrintTimer(bool clear);
public:
	Game(int16_t Xsize, int16_t Ysize, int16_t mines, COORD leftUpperCorner, std::vector<std::tuple<Menu::controls, std::vector<int>>> controls);
	
	/**
	 * \brief Wyświetlenie lub wyczyszczenie planszy z ekranu
	 * \param clear false - wyświetlenie true - wyczyszczenie
	 */
	void DisplayBoard(bool clear);
	/**
	 * \brief Rozpoczyna rozgrywkę
	 * \return bool - czy gracz wygrał, int - czas gry
	 */
	std::tuple<bool, int32_t> Start();
};
