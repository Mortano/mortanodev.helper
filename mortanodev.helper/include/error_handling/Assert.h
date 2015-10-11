#pragma once
#include <iostream>

namespace mdv
{

#ifdef _DEBUG
#define DEBUG_LEVEL_HIGH
#endif

#ifdef DEBUG_LEVEL_HIGH
   void _AssertFail(const char* conditionText, const char* file, int line)
   {
      std::cerr << "Condition " << conditionText << " failed at [" << file << "; line " << line << "]" << std::endl;
      __debugbreak();
   }
#endif

#ifdef DEBUG_LEVEL_HIGH
#define MDV_ASSERT(cond) if(!(cond)) { _AssertFail(#cond, __FILE__, __LINE__); }
#else
#define MDV_ASSERT(cond)
#endif

}
