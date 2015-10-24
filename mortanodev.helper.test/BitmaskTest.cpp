#include "stdafx.h"
#include "CppUnitTest.h"

#include "structures\Bitmask.h"

#include <vector>

using namespace mdv;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mortanodevhelpertest
{

   TEST_CLASS(BitmaskTest)
   {
   public:
      TEST_METHOD(Test_OneField_DefaultCtor_Get)
      {
         using Mask = Bitmask<1>;
         Mask m;
         Assert::AreEqual(static_cast<uint8_t>(0), m.Get<0>());
      }

      TEST_METHOD(Test_OneField_ArgumentCtor)
      {
         using Mask = Bitmask<1>;
         std::tuple<size_t> args;
         std::get<0>(args) = 1;

         Mask m{ args };
         Assert::AreEqual(static_cast<uint8_t>(1), m.Get<0>());
      }

      TEST_METHOD(Test_OneField_Set)
      {
         using Mask = Bitmask<1>;
         Mask m;
         m.Set<0>(1);
         Assert::AreEqual(static_cast<uint8_t>(1), m.Get<0>());
      }

      TEST_METHOD(Test_OneField_CopyCtor)
      {
         using Mask = Bitmask<1>;
         Mask m;
         m.Set<0>(1);

         Mask copy{ m };
         Assert::AreEqual(m.Get<0>(), copy.Get<0>());
      }

      //Some static asserts
      static_assert(sizeof(Bitmask<>) == 1, "Wrong size!"); //TIL: Empty classes in C++ must have a non-zero size :D 
      static_assert(sizeof(Bitmask<0>) == 1, "Wrong size!");
      static_assert(sizeof(Bitmask<1>) == 1, "Wrong size!");
      static_assert(sizeof(Bitmask<8>) == 1, "Wrong size!");
      static_assert(sizeof(Bitmask<9>) == 2, "Wrong size!");

      static_assert(sizeof(Bitmask<1,1>) == 1, "Wrong size!");
      static_assert(sizeof(Bitmask<7,1>) == 1, "Wrong size!");
      static_assert(sizeof(Bitmask<1,7>) == 1, "Wrong size!");

      static_assert(sizeof(Bitmask<8,1>) == 2, "Wrong size!");
      static_assert(sizeof(Bitmask<1,8>) == 2, "Wrong size!");

   };

}
