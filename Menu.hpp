#pragma once
#include <vector>
#include <functional>
#include "Alfabet.hpp"
#include <windows.h>

class Menu
{
private:
	enum controls;

	// Ustawione kontrolki
	// 296, 299, 301, 304 - strzałki
	// 32, 12 - spacja, enter
	std::vector<std::tuple<controls, std::vector<int>>> controls_{
		{up, {296, 'w'}},
		{down, {304, 's'}},
		{left, {299, 'a'}},
		{right, {301, 'd'}},
		{use, {32, 13, 'e'}},
		{check, {'q'}},
	};
	// Sprawdza czy naciśnięty przycisk jest zbindowany do jednej z kontrolek
	bool checkIfControlUsed(controls control, int ch) const;

	bool stop_ = false;
	uint32_t cols_;
	uint32_t rows_;
	Alfabet alfabet_{};
	std::vector<std::tuple<int16_t, int16_t, int16_t>> levels_{
		{8, 8, 10},
		{16, 16, 40},
		{30, 16, 99}
	};

	using MenuItem = std::tuple<std::string*, COORD, std::function<void()>>;
	std::vector<MenuItem> activeMenu_;
	std::vector<MenuItem> menu_;
	std::vector<MenuItem> difficultyMenu_;
	std::vector<MenuItem> gameOverMenu_;
	// Aktualna pozycja w menu
	uint32_t cursor_ = 0;

	std::vector<std::vector<bool>> StringToBlocks(const std::string& str) const;
	void Print(const std::string& str, COORD startingPos, WORD consoleAtribute, bool clear) const;
	void Display(bool clear) const;
	void Use() const;
	void StartGame(std::tuple<int16_t, int16_t, int16_t> level);
	void PrintGameOver(bool clear, bool win, int time);
	std::tuple<int16_t, int16_t, int16_t> CustomLevel();
public:
	// Oznaczenia możliwych ruchów użytkownika
	enum controls { up, down, left, right, use, check, none };

	// Zwraca naciśnięty numer przycisku przystosowany do vektora kontrolek (strzałki mają podwójne numery)
	static int getKey();
	// Sprawdza czy naciśnięty przycisk jest zbindowany do jednej z kontrolek
	static bool checkIfControlUsed(controls control, int ch, std::vector<std::tuple<controls, std::vector<int>>> controls);

	Menu(int16_t cols, int16_t rows);
	~Menu();

	void Start();
};
