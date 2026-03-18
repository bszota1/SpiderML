cmake_minimum_required(VERSION 3.15)
project(SpiderML)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES 
        src/main.cpp

        src/genes/Genome.cpp
        src/genes/Population.cpp
        src/genes/InnovationManager.cpp

        src/brain/NeuralNetwork.cpp

        src/agent/Spider.cpp
        src/agent/PopulationManager.cpp
    )
set(HEADERS
        src/genes/NodeGene.hpp
        src/genes/Population.hpp
        src/genes/InnovationManager.hpp
        src/genes/Genome.hpp
        src/genes/ConnectionGene.hpp

        src/brain/Neuron.hpp
        src/brain/Synapse.hpp
        src/brain/NeuralNetwork.hpp

        src/agent/Spider.hpp
        src/agent/PopulationManager.hpp

        src/evolution/NeatConfig.hpp
    )

add_executable(SpiderML ${SOURCES} ${HEADERS})

find_package(raylib CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
target_link_libraries(SpiderML PRIVATE raylib box2d::box2d)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/genes
    ${CMAKE_SOURCE_DIR}/src/evolution
    ${CMAKE_SOURCE_DIR}/src/brain
    ${CMAKE_SOURCE_DIR}/src/agent
)

