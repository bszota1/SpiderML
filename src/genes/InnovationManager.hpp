#pragma once

#include <map>
#include <utility>


class InnovationManager{
private:
    int currentInnovationID_;
    int currentNodeID_;
    std::map<std::pair<int, int>, int> history_;

public:
    int getInnovationID(int inNode, int outNode);
    int getNewNodeID();
    void reset();
};