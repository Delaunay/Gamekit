// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#include "Gamekit/Blueprint/GKMazeGeneration.h"

#include "Gamekit/Container/Matrix.h"

#include "Kismet/KismetMathLibrary.h"
#include "Math/IntVector.h"
#include "Math/RandomStream.h"

int32_t GetY(uint64_t k) {
    uint32_t y = k & 0xffffffffUL;
    return int(y);
}

int32_t GetX(uint64_t k) {
    uint32_t y = k >> 32;
    return int(y);
}

void UGKMazeGeneration::RandomWall(int GridX, int GridY, float Density, TArray<FIntVector> &Walls) {
    int           HalfX = GridX / 2;
    int           HalfY = GridY / 2;
    FRandomStream Stream(0);

    for (int i = -HalfX; i <= HalfX; i++) {
        for (int j = -HalfY; j <= HalfY; j++) {
            int X = i * GridX;
            int Y = j * GridY;

            if (UKismetMathLibrary::RandomFloatFromStream(Stream) < Density) {
                Walls.Emplace(X, Y, 0);
            }
        }
    }
}

#define VISITED 1
#define WALL    0

void UGKMazeGeneration::RandomizedDepthFirstSearch(int GridX, int GridY,
                                                   TArray<FIntVector> &Walls) {
    TArray<FIntVector> Stack;

    Stack.Reserve(GridX * GridY);
    TMatrix3D<int> Grid(GridX, GridY);
    FRandomStream Stream(0);

    // Choose the initial cell,
    int X = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, GridX - 1, Stream);
    int Y = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, GridY - 1, Stream);

    auto Cell  = FIntVector(X, Y, 0);
    Grid(Cell) = VISITED; // mark it as visited
    Stack.Push(Cell);     // push it to the stack

    while (Stack.Num() > 0) {
        Cell = Stack.Pop(false); // Pop a cell from the stack and make it a current cell

        X = Cell.X;
        Y = Cell.Y;

        TArray<FIntVector> Neighbours;
        for (auto &n: Grid.GetNeighbours(Cell)) {
            if (Grid(Cell + n) == VISITED) {
                continue;
            }
            Neighbours.Add(n);
        }

        if (Neighbours.Num() <= 0) {
            continue;
        }

        // Keep current cell so we can backtrack to it
        Stack.Push(Cell);

        // Select a neighboor to continue our exploration
        auto i = UKismetMathLibrary::RandomIntegerInRange(0, Neighbours.Num() - 1);
        auto n = Cell + Neighbours[i];
        auto w = Cell + Neighbours[i] / 2;

        Grid(w) = VISITED;
        Grid(n) = VISITED;
        Stack.Push(n); // Push it to the stack
    }

    int HalfX = GridX / 2;
    int HalfY = GridY / 2;
    for (int i = 0; i < GridX; i++) {
        for (int j = 0; j < GridY; j++) {
            if (Grid(i, j) == WALL) {
                X = (i - HalfX) * GridX;
                Y = (j - HalfY) * GridY;
                Walls.Emplace(X, Y, 0);
            }
        }
    }
}

// The stopping condition is probably not that great
void WilsonWalk(int GridX, int GridY, int Stop, TArray<FIntVector> &Out) {
    TMatrix3D<int> Grid(GridX, GridY);
    FRandomStream Stream(0);
    int           count = 0;

    {
        int  X     = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, GridX - 1, Stream);
        int  Y     = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, GridY - 1, Stream);
        auto Cell  = FIntVector(X, Y, 0);
        Grid(Cell) = VISITED; // mark it as visited
        count += 1;
    }

    int  X             = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, GridX - 1, Stream);
    int  Y             = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, GridY - 1, Stream);
    auto Starting      = FIntVector(X, Y, 0);
    Grid(Starting)     = VISITED; // mark it as visited
    auto Cell          = Starting;
    auto pending_count = count + 1;

    TMatrix3D<int> Pending = Grid;
    do {
        auto Neighbours = Grid.GetNeighbours(Cell);

        auto i = UKismetMathLibrary::RandomIntegerInRange(0, Neighbours.Num() - 1);
        Cell += Neighbours[i];
        pending_count += 1;

        // we found a a visited square
        if (Grid(Cell) == VISITED) {
            // persist the change
            Grid  = Pending;
            count = pending_count;

            // Start a new walk
            X        = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, GridX - 1, Stream);
            Y        = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, GridY - 1, Stream);
            Starting = FIntVector(X, Y, 0);
            Grid(Starting) = VISITED; // mark it as visited
            Cell           = Starting;
        }

        // We made a loop with our own path, restart
        if (Pending(Cell) == VISITED) {
            // Reset our starting point
            Grid(Starting) = WALL;
            Pending        = Grid;
            pending_count  = count;

            // Start a new walk
            X        = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, GridX - 1, Stream);
            Y        = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, GridY - 1, Stream);
            Starting = FIntVector(X, Y, 0);
            Grid(Starting) = VISITED; // mark it as visited
            Cell           = Starting;
            continue;
        }

    } while (int(float(count * 100) / float(GridX * GridY)) < Stop);
}