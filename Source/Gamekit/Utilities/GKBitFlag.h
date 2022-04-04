// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "HAL/Platform.h"

//! Check if a flag is set inside Value
//! The flag is the bit shift value
template <typename Enum>
bool IsSet(uint32 Value, Enum Flag)
{
    return (Value & (1 << static_cast<uint32>(Flag))) > 0;
}

//! Set a flag
//! The flag is the bit shift value
template <typename Enum>
uint32 SetFlag(uint32 Value, Enum Flag)
{
    return (Value | (1 << static_cast<uint32>(Flag)));
}

//! Remove a flag
//! The flag is the bit shift value
template <typename Enum>
uint32 RemoveFlag(uint32 Value, Enum Flag)
{
    return (Value & ~(1 << static_cast<uint32>(Flag)));
}