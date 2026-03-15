#include "InnovationManager.hpp"

int InnovationManager::getInnovationID(int inNodeID, int outNodeID){
    auto key = std::make_pair(inNodeID, outNodeID);
    auto it = history_.find(key);

    if (it != history_.end()){
        return it -> second;
    } else {
        int newID = currentInnovationID_++;
        history_.insert({key,newID});
        return newID;
    }
}

int InnovationManager::getNewNodeID() {
    return currentNodeID_++;
}

void InnovationManager::reset(){
    history_.clear();
}