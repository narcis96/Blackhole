//
//  main.cpp
//  mainPlayer
//
//  Created by Gemene Narcis on 02/08/2017.
//  Copyright © 2017 Gemene Narcis. All rights reserved.
//

#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <cassert>
#include <ctime>
#include <tuple>
#include <string>
enum class CellState
{
	FREE,
	BLOCKED,
	MINE,
	OPPONENT
};

class BlackholeSolver
{
public:
	BlackholeSolver(const int &n, const int &moves) :m_dimension(n), m_turn(0), m_totalMoves(moves)
	{
		assert(n > 0 && moves > 0);
		m_cellState = std::vector <std::vector<CellState>>(m_dimension, std::vector<CellState>(m_dimension, CellState::FREE));
		m_cellValue = std::vector <std::vector<int>>(m_dimension, std::vector<int>(m_dimension, 0));
		m_availableValues = std::vector <int>(m_totalMoves);
		for (int i = 0; i < m_totalMoves; i++)
			m_availableValues[i] = i + 1;
		std::random_shuffle(m_availableValues.begin(), m_availableValues.end());
		for (int i = 0; i < m_dimension; i++)
		{
			for (int j = 0; j <= i; j++)
				m_freeCells.emplace_back(i, j);
		}
	}
	void MarkBlocked(const int &i, const int &j)
	{
		CheckCell(i, j);
		m_cellState[i][j] = CellState::BLOCKED;
		EraseCell(i, j);
	}
	void SetOpponetCell(const int &i, const int &j, const int &val)
	{
		CheckCell(i, j);
		m_cellState[i][j] = CellState::OPPONENT;
		m_cellValue[i][j] = val;
		EraseCell(i, j);
	}

	std::tuple<int, int, int> GetTurn()
	{
		assert(m_turn < m_totalMoves);
		int i, j, val;
/*		if (m_turn == m_totalMoves - 1 && m_availableValues.size() == 2)
		{
			std::tie(i,j,val) = GetLastMove();
		}
		else*/
		{
			int index = rand() % m_availableValues.size();
			i = m_freeCells[index].first;
			j = m_freeCells[index].second;
			val = m_availableValues[index];
		}

		MarkMine(i, j, val);
		m_turn += 1;

		std::tuple<int, int, int> turn{ i, j, val };
		return turn;
	}
private:

	bool IsInterior(const int &i, const int &j)
	{
		return (0 <= i && i < m_dimension && 0 <= j && j <= i);
	}

	int GetWin(const int &i, const int &j)
	{
		assert(m_cellState[i][j] == CellState::FREE);
		int size = sizeof(dx)/sizeof(*dx);
		int Points = 0;
		for (int pos = 0; pos < size; ++pos)
		{
			int x = i + dx[pos];
			int y = j + dy[pos];
			if (IsInterior(x, y))
			{
				if (m_cellState[x][y] == CellState::MINE)
				{
					Points += m_cellValue[x][y];
				}
				else
				{
					if (m_cellState[x][y] == CellState::OPPONENT)
					{

						Points -= m_cellValue[x][y];
					}
				}
			}
		}
		return Points;
	}

	std::tuple < int, int, int > GetLastMove()
	{
		assert(m_freeCells.size() == 2);
		std::pair < int, int > firstFreePozition = m_freeCells.front();
		std::pair < int, int > secondFreePozition = m_freeCells.back();
		int i, j;
		if (GetWin(firstFreePozition.first, firstFreePozition.second) > GetWin(secondFreePozition.first, secondFreePozition.second))
		{
			std::tie(i, j) = firstFreePozition;
		}
		else
		{
			std::tie(i,j) = secondFreePozition;
		}
		return std::make_tuple(i, j, m_availableValues.front());
	}


	void MarkMine(const int &i, const int &j, const int &val)
	{
		CheckCell(i, j);
		assert(1 <= val &&  val <= m_totalMoves);

		m_cellState[i][j] = CellState::MINE;
		m_cellValue[i][j] = val;
		
		EraseCell(i, j);
		EraseVal(val);
	}

	void CheckCell(const int &i, const int &j)
	{
		assert(IsInterior(i,j));
		assert(m_cellState[i][j] == CellState::FREE);
	}

	void EraseCell(const int &i, const int &j)
	{
		auto it = std::find(m_freeCells.begin(), m_freeCells.end(), std::make_pair(i, j));
		assert(it != m_freeCells.end());
		m_freeCells.erase(it);
	}

	void EraseVal(const int &val)
	{
		auto it = std::find(m_availableValues.begin(), m_availableValues.end(), val);
		assert(it != m_availableValues.end());
		m_availableValues.erase(it);
	}
	const int m_dimension;
	const int m_totalMoves;
	int m_turn;
	std::vector <std::vector<CellState>> m_cellState;
	std::vector <std::vector<int>> m_cellValue;
	std::vector <std::pair<int, int>> m_freeCells;
	std::vector <int> m_availableValues;
	
	const int dx[6] = { 0, 0, -1, 1 ,-1, 1};
	const int dy[6] = { 1, -1, 0, 0, -1, 1};
};

int main(int argc, const char * argv[])
{
	srand(time(0));
	const int countBlocked = 5;
	BlackholeSolver solver(8, 15);

	auto ConvertToIndex = [](const char &line, const char &pos) -> std::pair < int, int >
	{
		return{ line - 'A' + pos - '1',pos - '1' };
	};
	auto ConvertToCell = [](const int &i, const int &j) -> std::pair < char, char >
	{
		return{ i + 'A' - j, j + '1' };
	};
	auto OpponentTurn = [ConvertToIndex, &solver](const std::string &command)
	{
		char line, pos;
		int val;
		sscanf(command.c_str(), "%c%c=%d", &line, &pos, &val);

		int i, j;
		std::tie(i, j) = ConvertToIndex(line, pos);

		solver.SetOpponetCell(i, j, val);
	};

	auto MyTurn = [&]()
	{
		int i, j, val;
		std::tuple<int, int, int> answer = solver.GetTurn();
		std::tie(i, j, val) = answer;

		char line, pos;
		std::tie(line, pos) = ConvertToCell(i, j);
		std::cout << line << pos << "=" << val << std::endl << std::flush;

	};
	for (int i = 0; i < countBlocked; i++)
	{
		char line, pos;
		std::cin >> line >> pos;
		auto indx = ConvertToIndex(line, pos);
		solver.MarkBlocked(indx.first, indx.second);
	}
	std::string command;
	std::cin >> command;
	CellState turn;
	if (command == "Start")
	{
		turn = CellState::MINE;
	}
	else
	{
		turn = CellState::OPPONENT;
	}
	for (int i = 1; i <= 30; ++i)
	{
		if (turn == CellState::MINE)
		{
			MyTurn();
			turn = CellState::OPPONENT;
		}
		else
		{
			if (i > 1)
			{
				std::cin >> command;
			}
			OpponentTurn(command);
			turn = CellState::MINE;
		}
	}
	return 0;
}
