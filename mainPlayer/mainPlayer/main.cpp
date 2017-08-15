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
#include <cassert>
#include <tuple>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <sys/types.h>
#include <unistd.h>

#ifdef __APPLE__
#define CLIENT "Client (" << getpid() << ") : "
//#define MY_DEBUG
#endif
enum class CellState {
    EMPTY,
    BLOCKED,
    MINE,
    OPPONENT
};

class BlackholeSolver {
public:
    BlackholeSolver(const int& n, const int& moves)
    : m_dimension(n)
    , m_turn(0)
    , m_totalMoves(moves)
    , m_codif(0LL)
    {
        assert(n > 0 && moves > 0);
        m_cellState = std::vector<std::vector<CellState> >(m_dimension, std::vector<CellState>(m_dimension, CellState::EMPTY));
        m_cellValue = std::vector<std::vector<int> >(m_dimension, std::vector<int>(m_dimension, 0));
        for (int i = 0; i < m_totalMoves; i++)
            m_availableValues.insert(i + 1);
        
        m_opponentAvailableValues = m_availableValues;
        
        for (int i = 0; i < m_dimension; i++) {
            for (int j = 0; j <= i; j++) {
                m_freeCells.insert({ i, j });
            }
        }
    }
    void MarkBlocked(const int& i, const int& j)
    {
        CheckCell(i, j);
        SetCell(i, j, CellState::BLOCKED, 0);
    }
    void SetOpponetCell(const int& i, const int& j, const int& val)
    {
        CheckCell(i, j);
        SetCell(i, j, CellState::OPPONENT, val);
        EraseVal(m_opponentAvailableValues, val);
    }
    
    std::tuple<int, int, int> GetTurn()
    {
        assert(m_turn < m_totalMoves);
        m_turn += 1;
        
        std::tuple<int, int, int> answer(-1, -1, -1);
        if (m_turn <= threshold1) {
            auto maxPos = std::max_element(m_freeCells.begin(), m_freeCells.end(), [&](const std::pair<int, int> &pos1, const std::pair<int, int> &pos2)->bool
            {
                return GetCost(pos1.first, pos1.second) < GetCost(pos2.first,pos2.second);
            });
            answer = std::tuple<int, int, int>(maxPos->first, maxPos->second, *m_availableValues.rbegin());
        }
        else {
            if (IsFinal() == true) {
#ifdef MY_DEBUG
                std::cerr << CLIENT << "reach final" << std::endl << std::flush;
#endif
                auto positions = GetLastPositions();
                auto minPos = std::min_element(positions.begin(), positions.end(), [&](const std::pair<int, int> &pos1, const std::pair<int, int> &pos2)->bool
                {
                    return GetWin(pos1.first, pos1.second) < GetWin(pos2.first,pos2.second);
                });
                answer = std::make_tuple(minPos->first, minPos->second, *m_availableValues.begin());
            }
            else {
                int steps = 2;
                if (m_turn >= 8) {
                    steps = 3;
                }
                bool stopFinal = false;
                if (m_freeCells.size()  <= 9) {
                    stopFinal = true;
                }
#ifdef MY_DEBUG
                std::cerr << CLIENT << "starts computing mix-max tree size = " << m_freeCells.size() << " stopFinal = " << stopFinal << std::endl << std::flush;
#endif
                answer = GetMinMaxTree(1, m_freeCells.size() - steps, stopFinal).second;
#ifdef MY_DEBUG
                std::cerr << CLIENT << "finished computing mix-max tree" << std::endl << std::flush;
#endif
            }
        }
        
        int i, j, val;
        std::tie(i, j, val) = answer;
#ifdef MY_DEBUG
        std::cerr << CLIENT << "found " << i << "," << j << " " << val << std::endl << std::flush;
#endif
        MarkMine(i, j, val);
        return answer;
    }
    
private:
    int GetWin(const int& i, const int& j)
    {
        assert(m_cellState[i][j] == CellState::EMPTY);
        int size = sizeof(dx) / sizeof(*dx);
        int points = 0;
        for (int pos = 0; pos < size; ++pos) {
            int x = i + dx[pos];
            int y = j + dy[pos];
            if (IsInterior(x, y)) {
                if (m_cellState[x][y] == CellState::MINE) {
                    points += m_cellValue[x][y];
                }
                else {
                    if (m_cellState[x][y] == CellState::OPPONENT) {
                        
                        points -= m_cellValue[x][y];
                    }
                }
            }
        }
        return points;
    }
    
    int GetNeighbors(const int& i, const int& j)
    {
        assert(m_cellState[i][j] == CellState::EMPTY);
        int size = sizeof(dx) / sizeof(*dx);
        int neighbors = 0;
        for (int pos = 0; pos < size; ++pos) {
            int x = i + dx[pos];
            int y = j + dy[pos];
            if (IsInterior(x, y) && m_cellState[x][y] == CellState::EMPTY) {
                neighbors += 1;
            }
        }
        return neighbors;
    }
    
    long double GetCost(const int& i, const int& j)
    {
        assert(m_cellState[i][j] == CellState::EMPTY);
        int size = sizeof(dx) / sizeof(*dx);
        long double cost = 0;
        int opponentMaxValue = *std::max_element(m_opponentAvailableValues.begin(), m_opponentAvailableValues.end());
        for (int pos = 0; pos < size; ++pos) {
            int x = i + dx[pos];
            int y = j + dy[pos];
            if (IsInterior(x, y)) {
                if (m_cellState[x][y] == CellState::MINE) {
                    cost -= m_cellValue[x][y];
                }
                else {
                    if (m_cellState[x][y] == CellState::OPPONENT) {
                        
                        cost += m_cellValue[x][y];
                    }
                    else if (m_cellState[x][y] == CellState::EMPTY) {
                        long double weight = 0.85;
                        int points = GetWin(x, y);
                        if (points > 0) {
                            weight = 0.7;
                        }
                        else if (points < 0) {
                            weight = 1;
                        }
                        cost += opponentMaxValue * weight;
                    }
                }
            }
        }
        return cost;
    }
    
    std::pair<double, std::tuple<int, int, int> > GetMinMaxTree(const bool& turn, const unsigned long& stopSize, const bool &stopFinal)
    {
#ifdef MY_DEBUG
        if(stopSize <= 2)
            std::cerr << CLIENT <<"GetMinMaxTree() turn = "<< turn << " size = " << m_freeCells.size() << " stop = " << stopSize << " stopFinal = " << stopFinal << std::endl << std::flush;
#endif
        if(stopFinal == true)
        {
#ifdef MY_DEBUG
            std::cerr << CLIENT <<"GetMinMaxTree() IsFinal() = "<< IsFinal() << std::endl << std::flush;
#endif
            if(IsFinal() == true)
            {
                bool currentTurn = turn;
                auto positions = GetLastPositions();
                std::sort(positions.begin(), positions.end(), [&](const std::pair<int, int> &pos1, const std::pair<int, int> &pos2)->bool
                {
                        return GetWin(pos1.first, pos1.second) < GetWin(pos2.first,pos2.second);
                });
                while (positions.size() > 1) {
                    if(currentTurn == 1)
                        positions.erase(positions.begin());
                    else
                        positions.pop_back();
                    currentTurn = currentTurn ^ 1;
                }
                return std::make_pair(GetWin(positions.back().first,positions.back().second), std::make_tuple(-1, -1, -1));
            }
        }
        else
        {
            if (m_freeCells.size() == stopSize) {
                double cost = 0;
                auto magic = [](const int x) -> double {
                    return log(x);
                };
                for (int i = 0; i < m_dimension; i++) {
                    for (int j = 0; j <= i; j++) {
                        if (m_cellState[i][j] == CellState::EMPTY) {
                            int win = GetWin(i, j);
                            if (win != 0) {
                                if (win > 0)
                                    cost += magic(win);
                                else
                                    cost -= magic(-win);
                            }
                        }
                    }
                }
                return { cost, std::tuple<int, int, int>(-1, -1, -1) };
            }
        }
        const auto cells = m_freeCells;
        
        std::pair<double, std::tuple<int, int, int> > answer(0, std::make_tuple(-1, -1, -1));
        std::set<int> availableValues;
        
        std::set<int>* from;
        if (turn == 1) //my turn
        {
            answer.first = -1000;
            from = &m_availableValues;
        }
        else //opponent turn
        {
            answer.first = 1000;
            from = &m_opponentAvailableValues;
        }
        for (const auto& cell : cells) {
            if (GetNeighbors(cell.first, cell.second) == 0) {
                if (turn == 1) {
                    availableValues.clear();
                    availableValues.insert(*m_availableValues.begin());
                }
                else{
                    availableValues.clear();
                    availableValues.insert(*m_opponentAvailableValues.begin());
                }
            }
            else {
                if(turn ==1)
                    availableValues = m_availableValues;
                else
                    availableValues = m_opponentAvailableValues;
            }
            
            for (const auto& value : availableValues) {
                if (turn == 1) {
                    MarkMine(cell.first, cell.second, value);
                }
                else {
                    SetOpponetCell(cell.first, cell.second, value);
                }
                
                auto sonBest = GetMinMaxTree(turn ^ 1, stopSize, stopFinal);
                
                from->insert(value);
                m_cellState[cell.first][cell.second] = CellState::EMPTY;
                m_cellValue[cell.first][cell.second] = 0;
                m_freeCells.insert(cell);
                
                if (turn == 1) {
                    if (answer.first < sonBest.first) {
                        answer.first = sonBest.first;
                        answer.second = std::make_tuple(cell.first, cell.second, value);
                    }
                }
                else if (answer.first > sonBest.first) {
                    answer.first = sonBest.first;
                }
            }
        }
        return answer;
    }
    
    std::vector<std::pair<int, int> > GetLastPositions()
    {
        std::vector<std::pair<int, int> > positions;
        for(const auto &position : m_freeCells)
            if (GetNeighbors(position.first, position.second) == 0) {
                positions.emplace_back(position);
            }
        return positions;
    }
    
    bool IsFinal()
    {
        for(const auto &position : m_freeCells)
            if (GetNeighbors(position.first, position.second) > 0) {
                return false;
            }
        return true;
    }
    
    void MarkMine(const int& i, const int& j, const int& val)
    {
        CheckCell(i, j);
        assert(1 <= val && val <= m_totalMoves);
        SetCell(i, j, CellState::MINE, val);
        EraseVal(m_availableValues, val);
    }
    
    void SetCell(const int& i, const int& j, const CellState& state, const int& value)
    {
        m_cellState[i][j] = state;
        m_cellValue[i][j] = value;
        EraseCell(i, j);
    }
    
    void EraseCell(const int& i, const int& j)
    {
        auto it = m_freeCells.find(std::make_pair(i, j));
        assert(it != m_freeCells.end());
        m_freeCells.erase(it);
    }
    
    void EraseVal(std::set<int>&availableValues,const int& val)
    {
        auto it = availableValues.find(val);
        assert(it != availableValues.end());
        availableValues.erase(it);
    }
    
    void CheckCell(const int& i, const int& j)
    {
#ifdef MY_DEBUG
        if (IsInterior(i, j) == false) {
            std::cerr << CLIENT << i << "," << j << "is out of boundaries" << std::endl << std::flush;
        }
        if (m_cellState[i][j] != CellState::EMPTY) {
            std::cerr << CLIENT << i << "," << j << " is not empty" << std::endl << std::flush;
        }
#endif
        assert(IsInterior(i, j));
        assert(m_cellState[i][j] == CellState::EMPTY);
    }
    
    bool IsInterior(const int& i, const int& j)
    {
        return (0 <= i && i < m_dimension && 0 <= j && j <= i);
    }
    
    const int m_dimension;
    const int m_totalMoves;
    int m_turn;
    std::vector<std::vector<CellState> > m_cellState;
    std::vector<std::vector<int> > m_cellValue;
    std::set<std::pair<int, int> > m_freeCells;
    std::set<int> m_availableValues;
    std::set<int> m_opponentAvailableValues;
    unsigned long long m_codif;
    const int dx[6] = { 0, 0, -1, 1, -1, 1 };
    const int dy[6] = { 1, -1, 0, 0, -1, 1 };
    static const int threshold1 = 4;
};

int main(int argc, const char* argv[])
{
    const int countBlocked = 5;
    BlackholeSolver solver(8, 15);
    
    auto ConvertToIndex = [](const char& line, const char& pos) -> std::pair<int, int> {
        return { line - 'A' + pos - '1', pos - '1' };
    };
    auto ConvertToCell = [](const int& i, const int& j) -> std::pair<char, char> {
        return { i + 'A' - j, j + '1' };
    };
    auto OpponentTurn = [ConvertToIndex, &solver](const std::string& command) {
        char line, pos;
        int val;
        sscanf(command.c_str(), "%c%c=%d", &line, &pos, &val);
        
        int i, j;
        std::tie(i, j) = ConvertToIndex(line, pos);
#ifdef MY_DEBUG
        std::cerr << CLIENT << "OpponentTurn() " << command << std::endl << std::flush;
#endif
        solver.SetOpponetCell(i, j, val);
    };
    
    auto MyTurn = [&]() {
        int i, j, val;
        std::tuple<int, int, int> answer = solver.GetTurn();
        std::tie(i, j, val) = answer;
        
        char line, pos;
        std::tie(line, pos) = ConvertToCell(i, j);
#ifdef MY_DEBUG
        std::cerr << CLIENT << line << pos << "=" << val << std::endl << std::flush;
#endif
        std::cout << line << pos << "=" << val << std::endl << std::flush;
        
    };
    for (int i = 0; i < countBlocked; i++) {
        char line, pos;
        std::cin >> line >> pos;
        auto indx = ConvertToIndex(line, pos);
        solver.MarkBlocked(indx.first, indx.second);
#ifdef MY_DEBUG
        std::cerr << CLIENT << line << pos << std::endl << std::flush;
#endif
    }
    std::string command;
    std::cin >> command;
    CellState turn;
    if (command == "Start") {
        turn = CellState::MINE;
#ifdef MY_DEBUG
        std::cerr << CLIENT << "Start" << std::endl << std::flush;
#endif
    }
    else {
        turn = CellState::OPPONENT;
    }
    for (int i = 1; i <= 30; ++i) {
        if (turn == CellState::MINE) {
            MyTurn();
            turn = CellState::OPPONENT;
        }
        else {
            if (i > 1) {
                std::cin >> command;
            }
            OpponentTurn(command);
            turn = CellState::MINE;
        }
    }
    return 0;
}
