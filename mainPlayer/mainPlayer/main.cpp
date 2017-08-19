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
#include <deque>
#include <cassert>
#include <tuple>
#include <string>
#include <cstring>
#include <set>
#include <unordered_set>
#include <unordered_map>
#ifdef __APPLE__
#include <sys/types.h>
#include <unistd.h>
#define CLIENT "Client (" << getpid() << ") : "
#endif
//#define MY_DEBUG
//#define USE_ASSERT
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
    BlackholeSolver(const unsigned long& n, const int& moves,
                    const long double* weights, const unsigned long& stopFinal,
                    const int& startMoves, const int& step3, const int& step4,
                    const int& toErase)
    : m_dimension(n)
    , m_totalMoves(moves)
    , m_turn(0)
    , m_weights(weights)
    , m_stopFinal(stopFinal)
    , m_startMoves(startMoves)
    , m_erase(toErase)
    , m_step3(step3)
    , m_step4(step4)
    {
#ifdef USE_ASSERT
        assert(n > 0 && moves > 0);
#endif
        m_cellState = std::vector<std::vector<CellState>>(m_dimension, std::vector<CellState>(m_dimension, CellState::EMPTY));
        m_cellValue = std::vector<std::vector<int>>(m_dimension, std::vector<int>(m_dimension, 0));
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
#ifdef USE_ASSERT
        CheckCell(i, j);
#endif
        SetCell(i, j, CellState::BLOCKED, 0);
    }
    void SetOpponetCell(const int& i, const int& j, const int& val)
    {
#ifdef USE_ASSERT
        CheckCell(i, j);
#endif
        SetCell(i, j, CellState::OPPONENT, val);
        EraseVal(m_opponentAvailableValues, val);
    }
    
    std::tuple<int, int, int> GetTurn()
    {
#ifdef USE_ASSERT
        assert(m_turn < m_totalMoves);
#endif
        m_turn += 1;
        
        std::tuple<int, int, int> answer(-1, -1, -1);
        if (m_turn <= m_startMoves) {
            auto maxPos = std::max_element(m_freeCells.begin(), m_freeCells.end(),
                             [&](const std::pair<int, int>& pos1,
                                 const std::pair<int, int>& pos2) -> bool {
                                 return GetCost(pos1.first, pos1.second) <
                                 GetCost(pos2.first, pos2.second);
                             });
            answer = std::tuple<int, int, int>(maxPos->first, maxPos->second,
                                               *m_availableValues.rbegin());
        } else {
            if (IsFinal() == true) {
#ifdef MY_DEBUG
                std::cerr << CLIENT << "reach final" << std::endl << std::flush;
#endif
                auto positions = GetLastPositions();
                auto minPos = std::min_element(positions.begin(), positions.end(),
                                 [&](const std::pair<int, int>& pos1,
                                     const std::pair<int, int>& pos2) -> bool {
                                     return GetWin(pos1.first, pos1.second) <
                                     GetWin(pos2.first, pos2.second);
                                 });
                answer = std::make_tuple(minPos->first, minPos->second,
                                         *m_availableValues.begin());
            } else {
                int steps = 2;
                
                if (m_freeCells.size() <= m_step3)
                    steps = 3;
                if (m_freeCells.size() <= m_step4)
                    steps = 4;
                
                bool stopFinal = false;
                if (m_freeCells.size() <= m_stopFinal) {
                    stopFinal = true;
                }
#ifdef MY_DEBUG
                std::cerr << CLIENT << "starts computing mix-max tree size = "
                << m_freeCells.size() << " stopFinal = " << stopFinal
                << std::endl
                << std::flush;
#endif
                answer = GetMinMaxTree(1, m_freeCells.size() - steps, stopFinal).second;
#ifdef MY_DEBUG
                std::cerr << CLIENT << "finished computing mix-max tree" << std::endl
                << std::flush;
#endif
            }
        }
        
        int i, j, val;
        std::tie(i, j, val) = answer;
#ifdef MY_DEBUG
        std::cerr << CLIENT << "found " << i << "," << j << " " << val << std::endl
        << std::flush;
#endif
        MarkMine(i, j, val);
        return answer;
    }
    
private:
    int GetWin(const int& i, const int& j)
    {
#ifdef USE_ASSERT
        assert(m_cellState[i][j] == CellState::EMPTY);
#endif
        int size = sizeof(dx) / sizeof(*dx);
        int points = 0;
        for (int pos = 0; pos < size; ++pos) {
            int x = i + dx[pos];
            int y = j + dy[pos];
            if (IsInterior(x, y)) {
                if (m_cellState[x][y] == CellState::MINE) {
                    points += m_cellValue[x][y];
                } else {
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
#ifdef USE_ASSERT
        assert(m_cellState[i][j] == CellState::EMPTY);
#endif
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
#ifdef USE_ASSERT
        assert(m_cellState[i][j] == CellState::EMPTY);
#endif
        int size = sizeof(dx) / sizeof(*dx);
        long double cost = 0;
        int opponentMaxValue = *std::max_element(m_opponentAvailableValues.begin(),
                                                 m_opponentAvailableValues.end());
        for (int pos = 0; pos < size; ++pos) {
            int x = i + dx[pos];
            int y = j + dy[pos];
            if (IsInterior(x, y)) {
                if (m_cellState[x][y] == CellState::MINE) {
                    cost -= m_cellValue[x][y];
                } else {
                    if (m_cellState[x][y] == CellState::OPPONENT) {
                        cost += m_cellValue[x][y];
                    } else if (m_cellState[x][y] == CellState::EMPTY) {
                        long double weight = m_weights[1];
                        int points = GetWin(x, y);
                        if (points > 0) {
                            weight = m_weights[0];
                        } else if (points < 0) {
                            weight = m_weights[2];
                        }
                        cost += opponentMaxValue * weight;
                    }
                }
            }
        }
        return cost;
    }
    
    std::pair<double, std::tuple<int, int, int>> GetMinMaxTree(
                                                               const bool& turn, const unsigned long& stopSize, const bool& stopFinal)
    {
#ifdef MY_DEBUG
        if (stopSize <= 2)
            std::cerr << CLIENT << "GetMinMaxTree() turn = " << turn
            << " size = " << m_freeCells.size() << " stop = " << stopSize
            << " stopFinal = " << stopFinal << std::endl
            << std::flush;
#endif
        if (stopFinal == true) {
#ifdef MY_DEBUG
            std::cerr << CLIENT << "GetMinMaxTree() IsFinal() = " << IsFinal()
            << std::endl
            << std::flush;
#endif
            if (IsFinal() == true) {
                bool currentTurn = turn;
                auto positions = GetLastPositions();
                std::sort(positions.begin(), positions.end(),
                          [&](const std::pair<int, int>& pos1,
                              const std::pair<int, int>& pos2) -> bool {
                              return GetWin(pos1.first, pos1.second) <
                              GetWin(pos2.first, pos2.second);
                          });
                while (positions.size() > 1) {
                    if (currentTurn == 1)
                        positions.pop_front();
                    else
                        positions.pop_back();
                    currentTurn = currentTurn ^ 1;
                }
                return std::make_pair(
                                      GetWin(positions.back().first, positions.back().second),
                                      std::make_tuple(-1, -1, -1));
            }
        } else {
            if (m_freeCells.size() == stopSize) {
                double cost = 0;
                int count = 1;
                auto magic = [](const int x) -> double { return log(x); };
                for (const auto& cell : m_freeCells) {
                    int i = cell.first;
                    int j = cell.second;
                    if (m_cellState[i][j] == CellState::EMPTY) {
                        int win = GetWin(i, j);
                        if (win != 0) {
                            if (win > 0) {
                                cost += magic(win);
                                count += 1;
                            } else {
                                cost -= magic(-win);
                                count -= 1;
                            }
                        }
                    }
                }
                cost += count;
                return { cost, std::tuple<int, int, int>(-1, -1, -1) };
            }
        }
        std::deque<std::pair<int, int>> cells;
        for (auto cell : m_freeCells)
            cells.push_back(cell);
        
        std::pair<double, std::tuple<int, int, int>> answer(
                                                            0, std::make_tuple(-1, -1, -1));
        std::set<int> availableValues;
        
        std::set<int>* from;
        if (turn == 1) // my turn
        {
            answer.first = -1000;
            from = &m_availableValues;
        } else // opponent turn
        {
            answer.first = 1000;
            from = &m_opponentAvailableValues;
        }
        if (m_erase != -1 && stopFinal == false) {
            std::sort(cells.begin(), cells.end(),
                      [&](const std::pair<int, int>& pos1,
                          const std::pair<int, int>& pos2) -> bool {
                          return GetWin(pos1.first, pos1.second) <
                          GetWin(pos2.first, pos2.second);
                      });
            if (cells.size() >= m_erase) {
                if (turn == 1) {
                    cells.pop_back();
                } else {
                    cells.pop_front();
                }
            }
            if (turn == 1)
                cells.pop_back();
            else
                cells.pop_front();
        }
        for (const auto& cell : cells) {
            if (GetNeighbors(cell.first, cell.second) == 0) {
                if (turn == 1) {
                    availableValues.clear();
                    availableValues.insert(*m_availableValues.begin());
                } else {
                    availableValues.clear();
                    availableValues.insert(*m_opponentAvailableValues.begin());
                }
            } else {
                if (turn == 1)
                    availableValues = m_availableValues;
                else
                    availableValues = m_opponentAvailableValues;
            }
            
            for (const auto& value : availableValues) {
                if (turn == 1) {
                    MarkMine(cell.first, cell.second, value);
                } else {
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
                } else if (answer.first > sonBest.first) {
                    answer.first = sonBest.first;
                }
            }
        }
        return answer;
    }
    
    std::deque<std::pair<int, int>> GetLastPositions()
    {
        std::deque<std::pair<int, int>> positions;
        for (const auto& position : m_freeCells)
            if (GetNeighbors(position.first, position.second) == 0) {
                positions.emplace_back(position);
            }
        return positions;
    }
    
    bool IsFinal()
    {
        for (const auto& position : m_freeCells)
            if (GetNeighbors(position.first, position.second) > 0) {
                return false;
            }
        return true;
    }
    
    void MarkMine(const int& i, const int& j, const int& val)
    {
#ifdef USE_ASSERT
        CheckCell(i, j);
        assert(1 <= val && val <= m_totalMoves);
#endif
        SetCell(i, j, CellState::MINE, val);
        EraseVal(m_availableValues, val);
    }
    
    void SetCell(const int& i, const int& j, const CellState& state,
                 const int& value)
    {
        m_cellState[i][j] = state;
        m_cellValue[i][j] = value;
        EraseCell(i, j);
    }
    
    void EraseCell(const int& i, const int& j)
    {
        auto it = m_freeCells.find(std::make_pair(i, j));
#ifdef USE_ASSERT
        assert(it != m_freeCells.end());
#endif
        m_freeCells.erase(it);
    }
    
    void EraseVal(std::set<int>& availableValues, const int& val)
    {
        auto it = availableValues.find(val);
#ifdef USE_ASSERT
        assert(it != availableValues.end());
#endif
        availableValues.erase(it);
    }
    
    void CheckCell(const int& i, const int& j)
    {
#ifdef MY_DEBUG
        if (IsInterior(i, j) == false) {
            std::cerr << CLIENT << i << "," << j << "is out of boundaries"
            << std::endl
            << std::flush;
        }
        if (m_cellState[i][j] != CellState::EMPTY) {
            std::cerr << CLIENT << i << "," << j << " is not empty" << std::endl
            << std::flush;
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
    std::vector<std::vector<CellState>> m_cellState;
    std::vector<std::vector<int>> m_cellValue;
    std::set<std::pair<int, int>> m_freeCells;
    std::set<int> m_availableValues;
    std::set<int> m_opponentAvailableValues;
    const int dx[6] = { 0, 0, -1, 1, -1, 1 };
    const int dy[6] = { 1, -1, 0, 0, -1, 1 };
    
    const long double* const m_weights;
    const unsigned long m_stopFinal;
    const int m_startMoves;
    const int m_erase;
    const int m_step3;
    const int m_step4;
};

int main(int argc, const char* argv[])
{
    std::vector<long double> weights(3, -1);
    int stopFinal = -1, startMoves = -1, toErase = -1, step3 = -1, step4 = -1;
    if (argc > 1) {
        bool weightsSetted = false;
        for (int i = 1; i < argc - 1; i++) {
            if (strcmp(argv[i], "-weights") == 0) {
                weightsSetted = true;
                sscanf(argv[i + 1], "[%Lf, %Lf, %Lf]", &weights[0], &weights[1],
                       &weights[2]);
            }
            if (strcmp(argv[i], "-startMoves") == 0) {
                startMoves = atoi(argv[i + 1]);
            }
            if (strcmp(argv[i], "-stopFinal") == 0) {
                stopFinal = atoi(argv[i + 1]);
            }
            if (strcmp(argv[i], "-erase") == 0) {
                toErase = atoi(argv[i + 1]);
            }
            if (strcmp(argv[i], "-step3") == 0) {
                step3 = atoi(argv[i + 1]);
            }
            if (strcmp(argv[i], "-step4") == 0) {
                step4 = atoi(argv[i + 1]);
            }
        }
        assert(step3 != -1);
        assert(step4 != -1);
        assert(stopFinal != -1);
        assert(startMoves != -1);
        assert(weightsSetted);
    } else {
        weights[0] = 0.7;  // I winn
        weights[1] = 0.85; // zero
        weights[2] = 1;    // opponent
        step3 = 14;
        step4 = 13;
        stopFinal = 9;
        startMoves = 4;
        toErase = -1;
    }
    const int blockedCells = 5;
    const int dimension = 8;
    const int moves = 15;
    BlackholeSolver solver(dimension, moves, weights.data(), stopFinal,
                           startMoves, step3, step4, toErase);
    
    auto ConvertToIndex = [](const char& line,
                             const char& pos) -> std::tuple<int, int> {
        return std::make_tuple(line - 'A' + pos - '1', pos - '1');
    };
    auto ConvertToCell = [](const int& i,
                            const int& j) -> std::tuple<char, char> {
        return std::make_tuple(i + 'A' - j, j + '1');
    };
    auto OpponentTurn = [ConvertToIndex, &solver](const std::string& command) {
        char line, pos;
        int val;
        sscanf(command.c_str(), "%c%c=%d", &line, &pos, &val);
        
        auto position = ConvertToIndex(line, pos);
#ifdef MY_DEBUG
        std::cerr << CLIENT << "OpponentTurn() " << command << std::endl
        << std::flush;
#endif
        solver.SetOpponetCell(std::get<0>(position), std::get<1>(position), val);
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
    for (int i = 0; i < blockedCells; i++) {
        char line, pos;
        std::cin >> line >> pos;
        auto indx = ConvertToIndex(line, pos);
        solver.MarkBlocked(std::get<0>(indx), std::get<1>(indx));
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
    } else {
        turn = CellState::OPPONENT;
    }
    for (int i = 1; i <= 2 * moves; ++i) {
        if (turn == CellState::MINE) {
            MyTurn();
            turn = CellState::OPPONENT;
        } else {
            if (i > 1) {
                std::cin >> command;
            }
            OpponentTurn(command);
            turn = CellState::MINE;
        }
    }
    return 0;
}
