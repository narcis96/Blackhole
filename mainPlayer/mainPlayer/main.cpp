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
#define LOCAL
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
    BlackholeSolver(const std::vector<std::vector<int>>&graph, const int& moves,
                    const long double* weights, const unsigned long& stopFinal,
                    const int& startMoves, const int& step3, const int& step4,
                    const int& toErase)
    : m_graph(graph)
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
        m_cellState = std::vector<CellState>(m_graph.size(), CellState::EMPTY);
        m_cellValue = std::vector<int>(m_graph.size(), 0);
        for (int i = 0; i < m_totalMoves; i++)
            m_availableValues.insert(i + 1);
        m_opponentAvailableValues = m_availableValues;
        for (int i = 0; i < m_graph.size(); i++)
            m_freeCells.insert(i);
    }
    void MarkBlocked(const int& i)
    {
#ifdef USE_ASSERT
        CheckCell(i);
#endif
        SetCell(i, CellState::BLOCKED, 0);
    }
    void SetOpponetCell(const int& i, const int& val)
    {
#ifdef USE_ASSERT
        CheckCell(i);
#endif
        SetCell(i, CellState::OPPONENT, val);
        EraseVal(m_opponentAvailableValues, val);
    }
    
    std::tuple<int, int> GetTurn()
    {
#ifdef USE_ASSERT
        assert(m_turn < m_totalMoves);
#endif
        m_turn += 1;
        
        std::tuple<int, int> answer(-1, -1);
        if (m_turn <= m_startMoves) {
            auto maxPos = std::max_element(m_freeCells.begin(), m_freeCells.end(),
                             [&](const int& pos1,
                                 const int& pos2) -> bool {
                                 return GetCost(pos1) < GetCost(pos2);
                             });
            answer = std::tuple<int, int>(*maxPos, *m_availableValues.rbegin());
        } else {
            if (IsFinal() == true) {
#ifdef MY_DEBUG
                std::cerr << CLIENT << "reach final" << std::endl << std::flush;
#endif
                auto positions = GetLastPositions();
                auto minPos = std::min_element(positions.begin(), positions.end(),
                                 [&](const int& pos1,
                                     const int& pos2) -> bool {
                                     return GetWin(pos1) < GetWin(pos2);
                                 });
                answer = std::make_tuple(*minPos, *m_availableValues.begin());
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
                auto ret = GetMinMaxTree(1, m_freeCells.size() - steps, stopFinal);
                answer = std::tuple<int,int>(std::get<1>(ret),std::get<2>(ret));
#ifdef MY_DEBUG
                std::cerr << CLIENT << "finished computing mix-max tree" << std::endl
                << std::flush;
#endif
            }
        }
        
        int i, val;
        std::tie(i, val) = answer;
#ifdef MY_DEBUG
        std::cerr << CLIENT << "found " << i << " " << val << std::endl
        << std::flush;
#endif
        MarkMine(i, val);
        return answer;
    }
    
private:
    int GetWin(const int& i)
    {
#ifdef USE_ASSERT
        assert(m_cellState[i] == CellState::EMPTY);
#endif
        int points = 0;
        for (const auto &x: m_graph[i]) {
            if (m_cellState[x] == CellState::MINE) {
                points += m_cellValue[x];
            } else {
                if (m_cellState[x] == CellState::OPPONENT) {
                    points -= m_cellValue[x];
                }
            }
        }
        return points;
    }
    
    int GetNeighbors(const int& i)
    {
#ifdef USE_ASSERT
        assert(m_cellState[i] == CellState::EMPTY);
#endif
        int neighbors = 0;
        for (const auto &x: m_graph[i])
            if(m_cellState[x] == CellState::EMPTY)
                neighbors += 1;
        
        return neighbors;
    }
    
    long double GetCost(const int& i)
    {
#ifdef USE_ASSERT
        assert(m_cellState[i] == CellState::EMPTY);
#endif
        long double cost = 0;
        int opponentMaxValue = *std::max_element(m_opponentAvailableValues.begin(),
                                                 m_opponentAvailableValues.end());
        
        for (const auto &x: m_graph[i]) {
            if (m_cellState[x] == CellState::MINE) {
                cost -= m_cellValue[x];
            } else {
                if (m_cellState[x] == CellState::OPPONENT) {
                    cost += m_cellValue[x];
                } else if (m_cellState[x] == CellState::EMPTY) {
                    long double weight = m_weights[1];
                    int points = GetWin(x);
                    if (points > 0) {
                        weight = m_weights[0];
                    } else if (points < 0) {
                        weight = m_weights[2];
                    }
                    cost += opponentMaxValue * weight;
                }
            }
        }
        return cost;
    }
    
    std::tuple<double, int, int> GetMinMaxTree(const bool& turn, const unsigned long& stopSize, const bool& stopFinal)
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
                          [&](const int& pos1,
                              const int& pos2) -> bool {
                              return GetWin(pos1) < GetWin(pos2);
                          });
                while (positions.size() > 1) {
                    if (currentTurn == 1)
                        positions.pop_front();
                    else
                        positions.pop_back();
                    currentTurn = currentTurn ^ 1;
                }
                return std::make_tuple(GetWin(positions.back()), -1, -1);
            }
        } else {
            if (m_freeCells.size() == stopSize) {
                double cost = 0;
                int count = 1;
                auto magic = [](const int x) -> double { return log(x); };
                for (const auto& i : m_freeCells) {
                    if (m_cellState[i] == CellState::EMPTY) {
                        int win = GetWin(i);
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
                return std::make_tuple(cost, -1, -1);
            }
        }
        std::deque<int> cells;
        for (auto cell : m_freeCells)
            cells.push_back(cell);
        
        std::tuple<double, int ,int> answer(0, -1, -1);
        std::set<int> availableValues;
        
        std::set<int>* from;
        if (turn == 1) // my turn
        {
            answer = std::tuple<double, int ,int> (-1000.0, -1, -1);
            from = &m_availableValues;
        } else // opponent turn
        {
            answer = std::tuple<double, int ,int> (1000.0, -1, -1);
            from = &m_opponentAvailableValues;
        }
        if (m_erase != -1 && stopFinal == false) {
            std::sort(cells.begin(), cells.end(),
                      [&](const int& pos1,
                          const int& pos2) -> bool {
                          return GetWin(pos1) < GetWin(pos2);
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
            if (GetNeighbors(cell) == 0) {
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
                    MarkMine(cell, value);
                } else {
                    SetOpponetCell(cell, value);
                }
                
                auto sonBest = GetMinMaxTree(turn ^ 1, stopSize, stopFinal);
                
                from->insert(value);
                m_cellState[cell] = CellState::EMPTY;
                m_cellValue[cell] = 0;
                m_freeCells.insert(cell);
                
                if (turn == 1) {
                    if (std::get<0>(answer) < std::get<0>(sonBest)) {
                        answer = std::make_tuple(std::get<0>(sonBest), cell, value);
                    }
                } else if (std::get<0>(answer) > std::get<0>(sonBest)) {
                    answer = std::make_tuple(std::get<0>(sonBest), -1, -1);
                }
            }
        }
        return answer;
    }
    
    std::deque<int> GetLastPositions()
    {
        std::deque<int> positions;
        for (const auto& position : m_freeCells)
            if (GetNeighbors(position) == 0) {
                positions.emplace_back(position);
            }
        return positions;
    }
    
    bool IsFinal()
    {
        for (const auto& position : m_freeCells)
            if (GetNeighbors(position) > 0) {
                return false;
            }
        return true;
    }
    
    void MarkMine(const int& i, const int& val)
    {
#ifdef USE_ASSERT
        CheckCell(i);
        assert(1 <= val && val <= m_totalMoves);
#endif
        SetCell(i, CellState::MINE, val);
        EraseVal(m_availableValues, val);
    }
    
    void SetCell(const int& i, const CellState& state,
                 const int& value)
    {
        m_cellState[i] = state;
        m_cellValue[i] = value;
        EraseCell(i);
    }
    
    void EraseCell(const int& i)
    {
        auto it = m_freeCells.find(i);
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
    
    void CheckCell(const int& i)
    {
#ifdef MY_DEBUG
        if (IsInterior(i) == false) {
            std::cerr << CLIENT << i << "is out of boundaries"
            << std::endl
            << std::flush;
        }
        if (m_cellState[i] != CellState::EMPTY) {
            std::cerr << CLIENT << i  << " is not empty" << std::endl
            << std::flush;
        }
#endif
        assert(IsInterior(i));
        assert(m_cellState[i] == CellState::EMPTY);
    }
    
    bool IsInterior(const int& indx)
    {
        return (0 <= indx && indx < m_graph.size());
    }
    
    const std::vector<std::vector<int>> m_graph;
    const int m_totalMoves;
    int m_turn;
    std::vector<CellState> m_cellState;
    std::vector<int> m_cellValue;
    std::unordered_set<int> m_freeCells;
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
    int blockedCells = -1, moves = -1;
    
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
            if (strcmp(argv[i], "-blockedCells") == 0) {
                blockedCells = atoi(argv[i + 1]);
            }
            if (strcmp(argv[i], "-moves") == 0) {
                moves = atoi(argv[i + 1]);
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
        blockedCells = 5;
        moves = 15;
        weights[0] = 0.7;  // I winn
        weights[1] = 0.85; // zero
        weights[2] = 1;    // opponent
        step3 = 14;
        step4 = 13;
        stopFinal = 9;
        startMoves = 4;
        toErase = -1;
    }
    std::vector<std::vector<int>>graph;
    BlackholeSolver solver(graph, moves, weights.data(), stopFinal,
                           startMoves, step3, step4, toErase);
    
    auto ConvertToIndex = [](const char& line,
                             const char& pos) -> int {
        int i = line - 'A' + pos - '1';
        int j = pos - '1';
        return i*(i+1)/2 + j;
    };
    
    auto ConvertToCell = [](int indx) -> std::tuple<char, char> {
        indx = indx + 1;
        int line = 0;
        int count = 0;
        int i = 0, j = 0;
        while (true) {
            if (count + line + 1 >= indx) {
                i = line;
                j = indx - count - 1;
                break;
            }
            line += 1;
            count += line;
        }
        return {i + 'A' - j, j + '1'};
    };
    auto OpponentTurn = [ConvertToIndex, &solver](const std::string& command) {
        int indx, val;
#ifdef LOCAL
        sscanf(command.c_str(), "%d=%d", &indx, &val);
#else
        char line, pos;
        sscanf(command.c_str(), "%c%c=%d", &line, &pos, &val);
        indx = ConvertToIndex(line, pos);
#endif
        
#ifdef MY_DEBUG
        std::cerr << CLIENT << "OpponentTurn() " << command << std::endl
        << std::flush;
#endif
        
        solver.SetOpponetCell(indx, val);
    };
    
    auto MyTurn = [ConvertToCell, &solver]() {
        int i, val;
        std::tie(i, val) = solver.GetTurn();
#ifdef LOCAL
        std::cout << i << "=" << val << std::endl << std::flush;
#else
        char line, pos;
        std::tie(line, pos) = ConvertToCell(i);
        std::cout << line << pos << "=" << val << std::endl << std::flush;
#endif
        
    };
    for (int i = 0; i < blockedCells; i++) {
#ifdef LOCAL
        int indx;
        std::cin >> indx;
#else
        char line, pos;
        std::cin >> line >> pos;
        int indx = ConvertToIndex(line, pos);
#endif
        solver.MarkBlocked(indx);

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
