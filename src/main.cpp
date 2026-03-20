#include <iostream>
#include <raylib.h>
#include <box2d/box2d.h>

#include "Spider.hpp"
#include "Genome.hpp"
#include "PopulationManager.hpp"

int main() {
    InitWindow(1200, 800, "Spider Evolution NEAT");
    SetTargetFPS(60);

    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {0.0f, 98.1f};
    b2WorldId world = b2CreateWorld(&worldDef);

    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = { 1000.0f, 550.0f }; 
    b2BodyId groundId = b2CreateBody(world, &groundBodyDef);

    b2Polygon groundBox = b2MakeBox(2000.0f, 20.0f); 
    
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    groundShapeDef.material.friction = 0.8f;
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    PopulationManager popManager(300, world);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        b2World_Step(world, dt, 4);
        popManager.update(dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangle(-100, 530, 4000, 40, DARKGRAY);
        
        popManager.draw();

        DrawText(TextFormat("Pokolenie: %i", popManager.getGeneration()), 20, 20, 20, BLACK);
        DrawText(TextFormat("Czas tury: %.1f / %.1f", popManager.getTimer(), popManager.getGenerationTime()), 20, 50, 20, DARKGRAY);
        DrawText(TextFormat("Best fitness: %.2f", popManager.getBestFitness()), 20, 80, 20, BLUE);
        DrawText(TextFormat("Avg fitness: %.2f", popManager.getAvgFitness()), 20, 110, 20, DARKBLUE);
        DrawText(TextFormat("Median fitness: %.2f", popManager.getMedianFitness()), 20, 140, 20, DARKGREEN);
        DrawText(TextFormat("Stddev fitness: %.2f", popManager.getStdDevFitness()), 20, 170, 20, DARKPURPLE);
        DrawText(TextFormat("Species: %i", popManager.getSpeciesCount()), 20, 200, 20, MAROON);
        DrawText(TextFormat("Rolling best (10): %.2f", popManager.getRollingBestFitness()), 20, 230, 20, PURPLE);
        DrawText(TextFormat("Rolling avg (10): %.2f", popManager.getRollingAvgFitness()), 20, 260, 20, VIOLET);

        EndDrawing();
    }

    b2DestroyWorld(world);
    CloseWindow();
    return 0;
}