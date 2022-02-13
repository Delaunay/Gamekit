#pragma once

#include "HAL/UnrealMemory.h"
#include "Containers/Array.h"

extern FIntVector NOFFSET[6];

template <typename T>
struct TMatrix3D {
    TMatrix3D(int row, int col, int depth = 1): Row(row), Col(col), Depth(depth) {
        Data.Init(T(), Row * Col * Depth);
    }

    T &operator()(int r, int c, int d = 0) { return Data[c + r * Col + d * (Col * Row)]; }

    T &operator()(FIntVector v) { return Data[v.Y + v.X * Col + v.Z * (Col * Row)]; }

    TArray<FIntVector> GetNeighbours(FIntVector v) {
        TArray<FIntVector> Result;
        Result.Reserve(4);

        for (int i = 0; i < 4; i++) {
            auto n = v + NOFFSET[i];
            if (n.X < 0 || n.X >= Row) {
                continue;
            }
            if (n.Y < 0 || n.Y >= Col) {
                continue;
            }
            if (n.Z < 0 || n.Z >= Depth) {
                continue;
            }
            Result.Add(NOFFSET[i]);
        }
        return Result;
    }

    TArray<T> &GetData() { return Data; }
    
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm#Variant_with_integer-based_arithmetic
    void DrawCircle() {
    
    }

    void FillLine3D(int32 x0, int32 y0, int32 x1, T Value) {
        if (y0 < 0 || y0 >= Height() || x0 >= Width() || x1 < 0)
		    return;

    	x0 = FMath::Max(x0, 0);
        x1 = FMath::Clamp(x1, x0, Width() - 1);

        FMemory::Memset(&(*this)(x1, y0), Value, x1 - x0 + 1);
    }

    FORCEINLINE int Width() const { return Col; }
    FORCEINLINE int Height() const { return Row; }

    private:
    int       Row;
    int       Col;
    int       Depth;
    TArray<T> Data;
};

