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
using namespace std;
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
    BlackholeSolver(int n, const int moves):m_dimension(n), m_turn(1), m_moves(moves)
    {
        assert(n > 0);
        m_cellState = std::vector <std::vector<CellState>>(m_dimension,std::vector<CellState>(m_dimension, CellState::FREE));
        m_cellValue = std::vector <std::vector<int>> (m_dimension,std::vector<int>(m_dimension, 0));
        m_availableValues = std::vector <int>(m_moves);
        for (int i = 0; i < m_moves; i++)
            m_availableValues[i] = i + 1;
        std::random_shuffle(m_availableValues.begin(), m_availableValues.end());
        for (int i = 0;i < m_dimension; i++)
        {
            for(int j = 0; j <= i; j++)
                m_freeCells.emplace_back(i,j);
        }
    }
    void MarkBlocked(const int &i,const int &j)
    {
        CheckCell(i, j);
        assert(m_cellState[i][j] == CellState::FREE);
        m_cellState[i][j] = CellState::BLOCKED;
    }
    void SetOpponetCell(const int &i,const int &j, const int &val)
    {
        CheckCell(i, j);
        assert(m_cellState[i][j] == CellState::FREE);
        m_cellState[i][j] = CellState::OPPONENT;
        m_cellValue[i][j] = val;
        EraseCell(i, j);

    }
    
    std::tuple<int,int,int> GetTurn()
    {
        std::tuple<int,int,int> turn;
        
        int index = rand()%m_availableValues.size();
        MarkMine(m_freeCells[index].first, m_freeCells[index].second, m_availableValues[index]);
        m_turn += 1;
        return turn;
    }
private:
    
    void MarkMine(const int &i,const int &j,const int &val)
    {
        CheckCell(i,j);
        m_cellState[i][j] = CellState::MINE;
        m_cellValue[i][j] = val;
        
        EraseVal(val);
        EraseCell(i,j);

    }
    
    void CheckCell(const int &i, const int &j)
    {
        assert(0<= i && i < m_dimension);
        assert(0<= j && j <= i);
    }
    
    void EraseCell(const int &i,const int &j)
    {
        auto it = std::find(m_freeCells.begin(),m_freeCells.end(), make_pair(i, j));
        assert(it != m_freeCells.end());
        m_freeCells.erase(it);
    }
    
    void EraseVal(const int &val)
    {
        auto it = std::find(m_availableValues.begin(),m_availableValues.end(), val);
        assert(it != m_availableValues.end());
        m_availableValues.erase(it);
    }
    const int m_dimension;
    const int m_moves;
    int m_turn;
    std::vector <std::vector<CellState>> m_cellState;
    std::vector <std::vector<int>> m_cellValue;
    std::vector <std::pair<int,int>> m_freeCells;
    std::vector <int> m_availableValues;
    
};
int main(int argc, const char * argv[])
{
    srand(time(0));
    const int countBlocked = 5;
    BlackholeSolver solver(8, 15);
    auto ConvertToIndex = [](const char &line,const char &pos) -> std::pair < int ,int >
    {
        return {line-'A' + pos - '1',pos - '1'};
    };
    auto ConvertToCell = [](const int &i,const int &j) -> std::pair < char ,char >
    {
        return {i + 'A' - j, j + '1'};
    };
    
    for (int i = 0;i < countBlocked; i++)
    {
        char line, pos;
        std::cin >> line >> pos;
        auto indx = ConvertToIndex(line, pos);
        solver.MarkBlocked(indx.first, indx.second);
    }
    std::string command;
    do
    {
        std::cin >> command;
        if(command == "Quit")
        {
            continue;
        }
        std::tuple<int,int,int> answer;
        if(command == "Start")
        {
            answer = solver.GetTurn();
        }
        else
        {
            char line, pos;
            int val;
            sscanf(command.c_str(),"%c%c=%d",&line,&pos,&val);
            
            int i, j;
            std::tie(i,j) = ConvertToIndex(line, pos);
            
            solver.SetOpponetCell(i, j,val);
            answer = solver.GetTurn();
            
        }
        int i, j, val;
        std::tie(i,j,val) = answer;
        char line, pos;
        std::tie(line,pos) = ConvertToCell(i,j);
        std::cout << line << pos << "="<<val << std::flush;
    }
    while(command != "Quit");
    return 0;
}
