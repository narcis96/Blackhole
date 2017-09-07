//
//  ParamParser.h
//  ParamParser
//
//  Created by Gemene Narcis on 07/09/2017.
//  Copyright © 2017 Gemene Narcis. All rights reserved.
//
#ifndef PARAM_PARSER_H
#define PARAM_PARSER_H
#include <map>
#include <string>
#include <vector>
#include <cassert>
class ParamParser
{
public:
    ParamParser(int argc, const char* argv[]){
        for(int i = 1; i < argc - 1; i += 2) {
            m_mapParams[argv[i]].push_back(argv[i+1]);
        }
    }
    std::string GetParam(const char* option) {
        assert(m_mapParams[option].size() == 1);
        return m_mapParams[option].back();
    }
    
    std::vector<std::string> GetParam(const char* option, bool) {
        return m_mapParams[option];
    }
    
private:
    std::map<std::string, std::vector<std::string>> m_mapParams;
};

#endif


