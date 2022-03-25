#pragma once

#include "HAL/UnrealMemory.h"
#include "Containers/Array.h"

extern FIntVector NOFFSET[6];

template <typename T>
struct TMatrix3D {
    TMatrix3D(): Row(0), Col(0), Depth(0) {
    }

    TMatrix3D(int row, int col, int depth = 1): Row(row), Col(col), Depth(depth) {
        Data.Init(T(), Row * Col * Depth);
    }

    void Init(T Value, int row, int col, int depth = 1) {
        if (row * col * depth > 0 && row * col * depth != Row * Col * Depth)
        {
            Row   = row;
            Col   = col;
            Depth = depth;
            Data.Init(Value, Row * Col * Depth);
        }
    }

    FORCEINLINE int Num() const { return Row * Col * Depth;  }

    FORCEINLINE T &operator()(int r, int c, int d = 0) { return Data[c + r * Col + d * (Col * Row)]; }

    FORCEINLINE T &operator()(FIntVector v) { return Data[v.Y + v.X * Col + v.Z * (Col * Row)]; }

    FORCEINLINE T operator()(int r, int c, int d = 0) const { return Data[c + r * Col + d * (Col * Row)]; }

    FORCEINLINE T operator()(FIntVector v) const { return Data[v.Y + v.X * Col + v.Z * (Col * Row)]; }

    FORCEINLINE T GetWithDefault(FIntVector v, T Default) const { 
        if (!Valid(v))
        {
            return Default;
        }
        return Data[v.Y + v.X * Col + v.Z * (Col * Row)]; 
    }

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

    FORCEINLINE TArray<T> &GetData() { return Data; }

    FORCEINLINE T *GetLayer(int d) { return &Data[d * (Col * Row)]; }

    FORCEINLINE int GetLayerSize() { return Col * Row;  }

    FORCEINLINE int GetLayerSizeBytes() { return Col * Row * sizeof(T);  }

    FORCEINLINE void ResetLayer(int d, T Value) { 
        FMemory::Memset(GetLayer(d), Value, GetLayerSizeBytes());
    }
    
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm#Variant_with_integer-based_arithmetic
    void DrawCircle() {
        // TODO:
    }

    void FillLine3D(int32 x0, int32 y0, int32 x1, T Value) {
        if (y0 < 0 || y0 >= Height() || x0 >= Width() || x1 < 0)
            return;

        x0 = FMath::Max(x0, 0);
        x1 = FMath::Clamp(x1, x0, Width() - 1);

        FMemory::Memset(&(*this)(x1, y0), Value, (x1 - x0 + 1) * sizeof(T));
    }

    FIntVector Clamp(FIntVector P) { 
        return FIntVector(
            FMath::Clamp(P.X, 0, Width() - 1),
            FMath::Clamp(P.Y, 0, Height() - 1),
            P.Z
        );
    }

    void FillRectangle2D(FIntVector Point1, FIntVector Point2, T Value) {
        Point1 = Clamp(Point1);
        Point2 = Clamp(Point2);

        FIntVector p1(
            FMath::Min(Point1.X, Point2.X),
            FMath::Min(Point1.Y, Point2.Y),
            0
        );

        FIntVector p2(
            FMath::Max(Point1.X, Point2.X),
            FMath::Max(Point1.Y, Point2.Y),
            0
        );

        int Length = p2.X - p1.X + 1;
        for (int i = p1.Y; i < p2.Y; i++)
        {
            FMemory::Memset(&(*this)(p1.X, i), Value, Length * sizeof(T));
        }
    }

    FORCEINLINE bool Valid(FIntVector V) const {
        return V.X >= 0 && V.X < Width() && 
               V.Y >= 0 && V.Y < Height();
    }

    FORCEINLINE int Width() const { return Col; }
    FORCEINLINE int Height() const { return Row; }

    private:
    int       Row;
    int       Col;
    int       Depth;
    TArray<T> Data;
};

