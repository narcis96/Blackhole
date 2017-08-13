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
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <limits>
#include <sys/types.h>
#include <unistd.h>

#ifdef __APPLE__
#define CLIENT "Client ("<<getpid()<<") : "
#endif

enum class CellState
{
	EMPTY,
	BLOCKED,
	MINE,
	OPPONENT
};

class BlackholeSolver
{
public:
	BlackholeSolver(const int &n, const int &moves) :m_dimension(n), m_turn(0), m_totalMoves(moves), m_codif(0LL)
	{
		assert(n > 0 && moves > 0);
		m_cellState = std::vector <std::vector<CellState>>(m_dimension, std::vector<CellState>(m_dimension, CellState::EMPTY));
		m_cellValue = std::vector <std::vector<int>>(m_dimension, std::vector<int>(m_dimension, 0));
		for (int i = 0; i < m_totalMoves; i++)
            m_availableValues.insert(i + 1);
        
        m_opponentAvailableValues = m_availableValues;
		
		for (int i = 0; i < m_dimension; i++)
		{
			for (int j = 0; j <= i; j++)
            {
                m_freeCells.insert({i, j});
            }
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
        SetCell(i,j, CellState::OPPONENT, val);
        m_opponentAvailableValues.erase(val);
        m_codif = m_codif*Base + GetHash(i, j, val);
	}

	std::tuple<int, int, int> GetTurn()
	{
		assert(m_turn < m_totalMoves);
        m_turn += 1;
        std::tuple<int,int,int> answer(-1, -1, -1);
        if(m_turn <= threshold1)
        {
            long double costMax = std::numeric_limits<long double>::min();
            int val =  *std::max_element(m_availableValues.begin(), m_availableValues.end());
            for (int i = 0; i < m_dimension; i++)
            {
                for (int j = 0; j <= i; j++)
                {
                    if (m_cellState[i][j]==CellState::EMPTY)
                    {
                        long double cost = GetCost(i, j);
                        if(cost > costMax)
                        {
                            costMax = cost;
                            answer = std::tuple<int,int,int>(i, j, val);
                        }
                    }
                }
            }
        }
        else
        {
            if (m_turn == 5)
            {
                GetMinMaxTree(1, 0LL, 15);
                m_codif = 0;
            }
            else
                if (m_turn == 9)
                {
                    m_dp.clear();
                    m_tree.clear();
                    GetMinMaxTree(1, 0LL, 1);
                    m_codif = 0;
                }
            assert(m_tree.find(m_codif) != m_tree.end() && "configuration was not found");
            answer = m_tree[m_codif];

        }
        
        int i, j, val;
        std::tie(i,j,val) = answer;
#ifdef __APPLE__
        std::cerr << CLIENT <<"found"<< std::get<0>(answer) << "," << std::get<1>(answer) << " " << std::get<2>(answer)<< std::endl << std::flush;
#endif
        MarkMine(i, j, val);
        m_codif = m_codif*Base + GetHash(i, j, val);

        return answer;
	}
private:

	int GetWin(const int &i, const int &j)
	{
		assert(m_cellState[i][j] == CellState::EMPTY);
		int size = sizeof(dx)/sizeof(*dx);
		int points = 0;
		for (int pos = 0; pos < size; ++pos)
		{
			int x = i + dx[pos];
			int y = j + dy[pos];
			if (IsInterior(x, y))
			{
				if (m_cellState[x][y] == CellState::MINE)
				{
					points += m_cellValue[x][y];
				}
				else
				{
					if (m_cellState[x][y] == CellState::OPPONENT)
					{

						points -= m_cellValue[x][y];
					}
				}
			}
		}
		return points;
	}
    
    long double GetCost(const int &i, const int &j)
    {
        assert(m_cellState[i][j] == CellState::EMPTY);
        int size = sizeof(dx)/sizeof(*dx);
        long double cost = 0;
        int opponentMaxValue = *std::max_element(m_opponentAvailableValues.begin(), m_opponentAvailableValues.end());
        for (int pos = 0; pos < size; ++pos)
        {
            int x = i + dx[pos];
            int y = j + dy[pos];
            if (IsInterior(x, y))
            {
                if (m_cellState[x][y] == CellState::MINE)
                {
                    cost -= m_cellValue[x][y];
                }
                else
                {
                    if (m_cellState[x][y] == CellState::OPPONENT)
                    {
                        
                        cost += m_cellValue[x][y];
                    }
                    else
                        if(m_cellState[x][y] == CellState::EMPTY)
                        {
                            long double weight = 0.85;
                            int points = GetWin(x, y);
                            if(points > 0)
                            {
                                weight = 0.7;
                            }
                            else
                                if(points < 0)
                                {
                                    weight = 1;
                                }
                            cost += opponentMaxValue * weight;
                        }
                }
            }
        }
        return cost;
    }
    
    void GetMinMaxTree(const bool &turn,const unsigned long long &conf, const int &stopSize)
    {
        if (m_freeCells.size() == stopSize)
        {
            if (stopSize == 1)
            {
                m_dp[conf] = GetWin(m_freeCells.begin()->first, m_freeCells.begin()->second);
            }
            else
            {
                for (int i = 0; i < m_dimension; i++)
                {
                    for (int j = 0; j <= i; j++)
                    {
                        if (m_cellState[i][j] == CellState::EMPTY)
                        {
                            m_dp[conf] += GetWin(i, j);
                        }
                    }
                }
            }
            return ;
        }
        const auto cells = m_freeCells;
        std::unordered_set<int> availableValues;
        std::unordered_set<int>*from;
        if (turn == 1)//my turn
        {
            availableValues = m_availableValues;
            from = &m_availableValues;
        }
        else//opponent turn
        {
            availableValues = m_opponentAvailableValues;
            from = &m_opponentAvailableValues;
        }
        for (const auto &cell:cells)
        {
            for (const auto &value : availableValues)
            {
                unsigned long long sonConf = conf * Base + GetHash(cell.first, cell.second, value);

                assert(m_dp.find(sonConf) == m_dp.end() && "Configurations are not unique");
                
                if(turn == 1)
                {
                    MarkMine(cell.first, cell.second, value);
                }
                else
                {
                    SetOpponetCell(cell.first, cell.second, value);
                }
                
                GetMinMaxTree(turn^1, sonConf, stopSize);
                
                
                from->insert(value);
                m_cellState[cell.first][cell.second] = CellState::EMPTY;
                m_cellValue[cell.first][cell.second] = 0;
                m_freeCells.insert(cell);
                
                
                if (m_dp.find(conf) == m_dp.end())
                {
                    m_dp[conf] = m_dp[sonConf];
                    if(turn == 1)
                        m_tree[conf] = std::tuple<int,int,int>(cell.first,cell.second,value);
                }
                else
                {
                    if (turn == 1)
                    {
                        if (m_dp[conf] < m_dp[sonConf])
                        {
                            m_dp[conf] = m_dp[sonConf];
                            m_tree[conf] = std::tuple<int,int,int>(cell.first,cell.second,value);
                        }
                    }
                    else
                    {
                        if(m_dp[conf] > m_dp[sonConf])
                        {
                            m_dp[conf] = m_dp[sonConf];
                        }
                    }

                }
            }
        }

    }
    unsigned long long GetHash(int i, int j, int val)
    {
        return (m_totalMoves+1)*(m_totalMoves+1)*i + (m_totalMoves+1)*j + val;
    }

	void MarkMine(const int &i, const int &j, const int &val)
	{
		CheckCell(i, j);
		assert(1 <= val &&  val <= m_totalMoves);
        SetCell(i,j, CellState::MINE, val);
		EraseVal(val);
    }

    void SetCell(const int &i,const int &j, const CellState &state, const int &value)
    {
        m_cellState[i][j] = state;
        m_cellValue[i][j] = value;
        EraseCell(i, j);

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
    
    void CheckCell(const int &i, const int &j)
    {
#ifdef __APPLE__
        if(IsInterior(i, j) == false)
        {
            std::cerr << CLIENT << i << "," << j << "is out of boundaries" << std::endl << std::flush;
        }
        if(m_cellState[i][j] != CellState::EMPTY)
        {
            std::cerr << CLIENT << i << "," << j << " is not empty" << std::endl << std::flush;
        }
#endif
        assert(IsInterior(i,j));
        assert(m_cellState[i][j] == CellState::EMPTY);
    }
    
    bool IsInterior(const int &i, const int &j)
    {
        return (0 <= i && i < m_dimension && 0 <= j && j <= i);
    }
    
	const int m_dimension;
	const int m_totalMoves;
	int m_turn;
	std::vector <std::vector<CellState>> m_cellState;
	std::vector <std::vector<int>> m_cellValue;
	std::set <std::pair<int, int>> m_freeCells;
	std::unordered_set <int> m_availableValues;
    std::unordered_set <int> m_opponentAvailableValues;
    std::unordered_map<unsigned long long, std::tuple<int,int,int>> m_tree;
    std::unordered_map<unsigned long long, int> m_dp;
    unsigned long long m_codif;
	const int dx[6] = { 0, 0, -1, 1 ,-1, 1};
	const int dy[6] = { 1, -1, 0, 0, -1, 1};
    static const int Base = 2111;
    static const int threshold1 = 4;
};

int main(int argc, const char * argv[])
{
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
#ifdef __APPLE__
        std::cerr << CLIENT <<"OpponentTurn() "<< command << std::endl << std::flush;
#endif
		solver.SetOpponetCell(i, j, val);
	};

	auto MyTurn = [&]()
	{
		int i, j, val;
		std::tuple<int, int, int> answer = solver.GetTurn();
		std::tie(i, j, val) = answer;

		char line, pos;
		std::tie(line, pos) = ConvertToCell(i, j);
#ifdef __APPLE__
        std::cerr << CLIENT << line << pos << "=" << val << std::endl << std::flush;
#endif
		std::cout << line << pos << "=" << val << std::endl << std::flush;

	};
	for (int i = 0; i < countBlocked; i++)
	{
		char line, pos;
		std::cin >> line >> pos;
		auto indx = ConvertToIndex(line, pos);
		solver.MarkBlocked(indx.first, indx.second);
#ifdef __APPLE__
        std::cerr << CLIENT << line << pos << std::endl << std::flush;
#endif

	}
	std::string command;
	std::cin >> command;
	CellState turn;
	if (command == "Start")
	{
		turn = CellState::MINE;
#ifdef __APPLE__
        std::cerr << CLIENT << "Start" << std::endl << std::flush;
#endif
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
