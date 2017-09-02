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
#include <sstream>
#include <cstring>
#include <utility>
#include <chrono>
#include <iterator>
#include <random>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <set>
#ifdef __APPLE__
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#define LOCAL
#define CLIENT "Client (" << getpid() << ") : "
//#define MY_DEBUG
//#define USE_ASSERT
#endif

enum class CellState
{
    EMPTY,
    BLOCKED,
    MINE,
    OPPONENT
};
class RandomGenerator
{
public:
    static int GetNumber(const int x) {
        static auto d = std::chrono::system_clock::now().time_since_epoch();
        static std::mt19937 gen(std::chrono::duration_cast<std::chrono::milliseconds>(d).count());
        static std::uniform_int_distribution <int> dist(1, 1<<14);
        return dist(gen) % x;
    }
};

class BlackholeSolver
{
public:
    BlackholeSolver(const std::vector<std::vector<int>>&graph, const int& moves,
                    const long double* weights, const unsigned long& stopFinal,
                    const int& startMoves, const int& step3, const int& step4,
                    const int& toErase, std::vector<int> &probabilities,
                    const char* func, const char* func2 = NULL)
    : m_turn(0)
    , m_graph(graph)
    , m_totalMoves(moves)
    , m_weights(weights)
    , m_probabilities(probabilities)
    , m_stopFinal(stopFinal)
    , m_startMoves(startMoves)
    , m_erase(toErase)
    , m_step3(step3)
    , m_step4(step4)
    {
#ifdef USE_ASSERT
        assert(m_graph.size() > 0 && moves > 0);
#endif
        m_cellState = std::vector<CellState>(m_graph.size(), CellState::EMPTY);
        m_cellValues = std::vector<int>(m_graph.size(), 0);
        for (int i = 0; i < m_totalMoves; i++)
            m_availableValues.insert(i + 1);
        m_opponentAvailableValues = m_availableValues;
        for (int i = 0; i < m_graph.size(); i++)
            m_freeCells.insert(i);
        std::map < std::string , std::function<double(int)> > m;
        m["log"] = [](const int x) -> double {
            assert(x > 0);
            return log(x);
        };
        m["sqrt"] = [](const int x) -> double {
            assert(x > 0);
            return sqrt(x);
        };
        m["x"] = [](const int x) -> double {
            assert(x > 0);
            return x;
        };
        m_func = m[func];
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
                             [&](const int& pos1, const int& pos2) -> bool {
                                 return GetCost(pos1) < GetCost(pos2);
                             });
            auto it = m_availableValues.rbegin();
            for (int i = 3; i <= 4; i++) {
                if (1 + RandomGenerator::GetNumber(10) <= i)
                    it++;
            }
            answer = std::tuple<int, int>(*maxPos, *it);
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
                points += m_cellValues[x];
            } else {
                if (m_cellState[x] == CellState::OPPONENT) {
                    points -= m_cellValues[x];
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
                cost -= m_cellValues[x];
            } else {
                if (m_cellState[x] == CellState::OPPONENT) {
                    cost += m_cellValues[x];
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
        if (stopFinal == true) {
#ifdef MY_DEBUG
//                std::cerr << CLIENT << "GetMinMaxTree() turn = " << turn
//                << " size = " << m_freeCells.size()
//                << " IsFinal() = " << IsFinal() << std::endl
//                << std::flush;
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
                int count = 0;
                for (const auto& i : m_freeCells) {
                    if (m_cellState[i] == CellState::EMPTY) {
                        int win = GetWin(i);
                        if (win != 0) {
                            if (win > 0) {
                                cost += m_func(win);
                                count += 1;
                            } else {
                                cost -= m_func(-win);
                                count -= 1;
                            }
                        }
                    }
                }
                std::vector<double> para{cost, 1.0*count};
                std::vector<double> w{0.6, 0.4};
                cost = 0;
                for (int i = 0; i < para.size(); i++) {
                    cost += 1.0*para[i] * w[i];
                }
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
			std::set < double >sons;
            for (const auto& value : availableValues) {
                if (turn == 1) {
                    MarkMine(cell, value);
                } else {
                    SetOpponetCell(cell, value);
                }
                
                auto sonBest = GetMinMaxTree(turn ^ 1, stopSize, stopFinal);
                
                from->insert(value);
                m_cellState[cell] = CellState::EMPTY;
                m_cellValues[cell] = 0;
                m_freeCells.insert(cell);
                
				if (turn == 1) {
					if (std::get<0>(answer) < std::get<0>(sonBest)) {
						answer = std::make_tuple(std::get<0>(sonBest), cell, value);
					}
				}
				else {
					sons.insert(std::get<0>(sonBest));
				}
            }


			if (turn == 0) {
                int i = 0;
                for (auto it = sons.begin(); it != sons.end() && m_probabilities.size(); it++, i += 1) {
                    if (1 + RandomGenerator::GetNumber(100) <= m_probabilities[i]) {
                        answer = std::make_tuple(*it, -1, -1);
                    }
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
        m_cellValues[i] = value;
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
    
    int m_turn;
    std::vector<CellState> m_cellState;
    std::vector<int> m_cellValues;
    std::unordered_set<int> m_freeCells;
    std::set<int> m_availableValues;
    std::set<int> m_opponentAvailableValues;
//    const int dx[6] = { 0, 0, -1, 1, -1, 1 };
//    const int dy[6] = { 1, -1, 0, 0, -1, 1 };

    const std::vector<std::vector<int>> m_graph;
    const int m_totalMoves;
    const long double* const m_weights;
    const std::vector<int> m_probabilities;
    const unsigned long m_stopFinal;
    const int m_startMoves;
    const int m_erase;
    const int m_step3;
    const int m_step4;
    std::function<long double(int)>m_func;
    std::function<long double(int)>m_func2;
};
const char *graph = 
"36\n\
0 1\n\
0 2\n\
1 0\n\
1 2\n\
1 3\n\
1 4\n\
2 0\n\
2 1\n\
2 4\n\
2 5\n\
3 1\n\
3 4\n\
3 6\n\
3 7\n\
4 1\n\
4 2\n\
4 3\n\
4 5\n\
4 7\n\
4 8\n\
5 2\n\
5 4\n\
5 8\n\
5 9\n\
6 3\n\
6 7\n\
6 10\n\
6 11\n\
7 3\n\
7 4\n\
7 6\n\
7 8\n\
7 11\n\
7 12\n\
8 4\n\
8 5\n\
8 7\n\
8 9\n\
8 12\n\
8 13\n\
9 5\n\
9 8\n\
9 13\n\
9 14\n\
10 6\n\
10 11\n\
10 15\n\
10 16\n\
11 6\n\
11 7\n\
11 10\n\
11 12\n\
11 16\n\
11 17\n\
12 7\n\
12 8\n\
12 11\n\
12 13\n\
12 17\n\
12 18\n\
13 8\n\
13 9\n\
13 12\n\
13 14\n\
13 18\n\
13 19\n\
14 9\n\
14 13\n\
14 19\n\
14 20\n\
15 10\n\
15 16\n\
15 21\n\
15 22\n\
16 10\n\
16 11\n\
16 15\n\
16 17\n\
16 22\n\
16 23\n\
17 11\n\
17 12\n\
17 16\n\
17 18\n\
17 23\n\
17 24\n\
18 12\n\
18 13\n\
18 17\n\
18 19\n\
18 24\n\
18 25\n\
19 13\n\
19 14\n\
19 18\n\
19 20\n\
19 25\n\
19 26\n\
20 14\n\
20 19\n\
20 26\n\
20 27\n\
21 15\n\
21 22\n\
21 28\n\
21 29\n\
22 15\n\
22 16\n\
22 21\n\
22 23\n\
22 29\n\
22 30\n\
23 16\n\
23 17\n\
23 22\n\
23 24\n\
23 30\n\
23 31\n\
24 17\n\
24 18\n\
24 23\n\
24 25\n\
24 31\n\
24 32\n\
25 18\n\
25 19\n\
25 24\n\
25 26\n\
25 32\n\
25 33\n\
26 19\n\
26 20\n\
26 25\n\
26 27\n\
26 33\n\
26 34\n\
27 20\n\
27 26\n\
27 34\n\
27 35\n\
28 21\n\
28 29\n\
29 21\n\
29 22\n\
29 28\n\
29 30\n\
30 22\n\
30 23\n\
30 29\n\
30 31\n\
31 23\n\
31 24\n\
31 30\n\
31 32\n\
32 24\n\
32 25\n\
32 31\n\
32 33\n\
33 25\n\
33 26\n\
33 32\n\
33 34\n\
34 26\n\
34 27\n\
34 33\n\
34 35\n\
35 27\n\
35 34\n";
#ifdef LOCAL
std::string GetGraph(const char* graphPath)
{
	FILE *file = fopen(graphPath, "r");
	fseek(file, 0, SEEK_END);
	long lSize = ftell(file);
	rewind(file);

	char *graph;
	graph = (char*)malloc(sizeof(char)*(lSize + 1));
	memset(graph, 0, sizeof(char)*(lSize + 1));
	fread(graph, sizeof(char), lSize, file);
    fclose(file);
    return std::string(graph);
}
#else
std::string GetGraph(const char*)
{
	return std::string(graph);
}
#endif

std::vector<std::vector<int>>ReadGraph(const char* graphContent) {
    int n, x, y, offset = 0;
	sscanf(graphContent, "%d%n", &n, &offset);
    graphContent += offset;
    std::vector<std::vector<int>>graph(n, std::vector<int>());
    while(sscanf(graphContent, "%d %d%n", &x, &y, &offset) == 2) {
        graph[x].push_back(y);
        graph[y].push_back(x);
        graphContent += offset;
    }
    return graph;
}

int main(int argc, const char* argv[])
{
    int blockedCells = -1, moves = -1;
    
    /*for (int i = 0; i < argc ; i++) {
        std::cerr << CLIENT << argv[i]<< " " << std::endl
        << std::flush;
    }*/
    std::vector<long double> weights;
    std::vector<int> probabilities;
	
    int stopFinal = -1, startMoves = -1, toErase = -1, step3 = -1, step4 = -1;
    std::vector<std::vector<int>>graph;
	std::string graphStr, func, func2;
#if defined LOCAL
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-weights") == 0) {
            std::string sentence = argv[i + 1];
            std::replace(sentence.begin(), sentence.end(), ',', ' ');
            std::istringstream iss(sentence);
            weights = std::vector< long double> {std::istream_iterator<long double>{iss},
                std::istream_iterator<long double>{}};
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
        if (strcmp(argv[i], "-blockedCells") == 0) {
            blockedCells = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-moves") == 0) {
            moves = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-graphPath") == 0) {
            graphStr = GetGraph(argv[i + 1]);
        }
        if (strcmp(argv[i], "-probabilities") == 0) {
            std::string sentence = argv[i + 1];
            std::replace(sentence.begin(), sentence.end(), ',', ' ');
            std::istringstream iss(sentence);
            probabilities = std::vector<int> {std::istream_iterator<int>{iss},
                std::istream_iterator<int>{}};
        }
        if (strcmp(argv[i], "-func") == 0) {
            func = argv[i + 1];
        }
        
        if (strcmp(argv[i], "-func2") == 0) {
            func2 = argv[i + 1];
        }
    }
    assert(weights.size() > 0);
    assert(startMoves != -1);
    assert(stopFinal != -1);
    assert(step3 != -1);
    assert(step4 != -1);
    assert(blockedCells != -1);
    assert(moves != -1);
    assert(graphStr.size() > 0);
    assert(probabilities.size() > 0);
    assert(func.size() > 0);
//   assert(func2.size() > 0);
#else
    blockedCells = 5;
    moves = 15;
    step3 = 16;
    step4 = 13;
    stopFinal = 9;
    startMoves = 4;
    toErase = 22;
    weights = std::vector <long double>{0.7, 0.85, 1}; //I win, zero, opponent
    probabilities = std::vector <int>{100, 0, 0};//first, second, third opponent mistake
    graphStr = GetGraph(NULL);
    func = "x";
    func2 = "sqrt";
#endif
#ifdef MY_DEBUG
//    std::cerr << CLIENT << "weights= " << weights[0]<< " " << weights[1] <<" " << weights[2]<< std::endl
//    << std::flush;
//    std::cerr << CLIENT << "probabilities= " << probabilities[0]<< " " << probabilities[1] <<" " << probabilities[2]<< std::endl
//    << std::flush;
#endif
    graph = ReadGraph(graphStr.c_str());
    BlackholeSolver solver(graph, moves, weights.data(), stopFinal,
                           startMoves, step3, step4, toErase, probabilities, func.c_str());
    
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
        return std::make_tuple(i + 'A' - j, j + '1');
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

    }
    std::string command;
    std::cin >> command;
    CellState turn;
    if (command == "Start") {
        turn = CellState::MINE;
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
