#pragma once

#include "Defines.hpp"
#include "RTTI/RTTI.hpp"

// Math
#include "Math/BasicMath.hpp"
#include "Math/Angle.hpp"
#include "Math/Vector.hpp"
#include "Math/Vector2f.hpp"
#include "Math/Vector2i.hpp"
#include "Math/Vector3f.hpp"
#include "Math/Matrix.hpp"
#include "Math/Quaternion.hpp"
#include "Math/SimdMath.hpp"

// Geometry
#include "Math/AABox.hpp"
#include "Math/AARect.hpp"

// Memory
#include "BaseObject.hpp"
#include "Memory/Allocator.hpp"
#include "Memory/PoolAllocator.hpp"
#include "Memory/IterablePoolAllocator.hpp"
#include "Memory/RefCountedBase.hpp"

// Containers
#include "Collections/String.hpp"
#include "Collections/StringBuilder.hpp"
#include "Collections/Dynarray.hpp"
#include "Collections/Queue.hpp"
#include "Collections/PriorityQueue.hpp"

// Other
#include "Math/Color.hpp"
#include "FileIO.hpp"
#include "Utils/Logger.hpp"
#include "UniqueID.hpp"
#include "Utils/EnumUtils.hpp"
#include "Utils/OutputStream.hpp"