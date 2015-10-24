#include "stdafx.h"
#include "CppUnitTest.h"

#include "structures\Bitmask.h"

#include <vector>

using namespace mdv;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft {
   namespace VisualStudio {
      namespace CppUnitTestFramework
      {
         template <> static std::wstring ToString<uint16_t>(const uint16_t& q) {
            RETURN_WIDE_STRING(q);
         }
      }
   }
}

namespace mortanodevhelpertest
{

   TEST_CLASS(BitmaskTest)
   {
   public:
      TEST_METHOD(Test_OneField_DefaultCtor)
      {
         using Mask = Bitmask<1>;
         Mask m;
         Assert::AreEqual(static_cast<uint8_t>(0), m.Get<0>());
      }

      TEST_METHOD(Test_OneField_TupleCtor)
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

      TEST_METHOD(Test_TwoFields_DefaultCtor)
      {
         using Mask = Bitmask<3, 3>;
         Mask m;

         Assert::AreEqual(static_cast<uint8_t>(0), m.Get<0>());
         Assert::AreEqual(static_cast<uint8_t>(0), m.Get<1>());
      }

      TEST_METHOD(Test_TwoFields_TupleCtor)
      {
         using Mask = Bitmask<3, 3>;
         
         std::tuple<size_t, size_t> args;
         std::get<0>(args) = 3;
         std::get<1>(args) = 3;
         
         Mask m{ args };
         Assert::AreEqual(static_cast<uint8_t>(std::get<0>(args)), m.Get<0>());
         Assert::AreEqual(static_cast<uint8_t>(std::get<1>(args)), m.Get<1>());
      }

      TEST_METHOD(Test_TwoFields_CopyCtor)
      {
         using Mask = Bitmask<3, 3>;

         std::tuple<size_t, size_t> args;
         std::get<0>(args) = 3;
         std::get<1>(args) = 3;

         Mask m{ args };
         Mask copy{ m };

         Assert::AreEqual(m.Get<0>(), copy.Get<0>());
         Assert::AreEqual(m.Get<1>(), copy.Get<1>());
      }

      TEST_METHOD(Test_TwoFields_Set)
      {
         using Mask = Bitmask<3, 3>;

         Mask m;

         m.Set<0>(7);
         m.Set<1>(5);
         //First set both, then check for equality to rule out problems with overwriting section data!
         Assert::AreEqual(static_cast<uint8_t>(7), m.Get<0>());
         Assert::AreEqual(static_cast<uint8_t>(5), m.Get<1>());
      }

      TEST_METHOD(Test_OneBigField_DefaultCtor)
      {
         using Mask = Bitmask<32>;
         Mask m;
         Assert::AreEqual(0U, m.Get<0>());
      }

      TEST_METHOD(Test_OneBigField_TupleCtor)
      {
         using Mask = Bitmask<32>;
         std::tuple<size_t> args;
         std::get<0>(args) = 0x8f8f8f8f;

         Mask m{ args };
         Assert::AreEqual(static_cast<uint32_t>(std::get<0>(args)), m.Get<0>());
      }

      TEST_METHOD(Test_OneBigField_CopyCtor)
      {
         using Mask = Bitmask<32>;
         std::tuple<size_t> args;
         std::get<0>(args) = 0x8f8f8f8f;

         Mask m{ args };
         Mask copy{ m };

         Assert::AreEqual(m.Get<0>(), copy.Get<0>());
      }

      TEST_METHOD(Test_OneBigField_Set)
      {
         using Mask = Bitmask<32>;

         Mask m;

         m.Set<0>(0x8f8f8f8f);

         Assert::AreEqual(0x8f8f8f8fU, m.Get<0>());
      }

      TEST_METHOD(Test_TwoOddFields_Set)
      {
         using Mask = Bitmask<9, 11>;
         using namespace std::literals;

         const auto l1 = 0b111111111;
         const auto l2 = 0b01010101010;

         Mask m;
         m.Set<0>( l1 ); 
         m.Set<1>( l2 ); 

         Assert::AreEqual(static_cast<uint16_t>(l1), m.Get<0>());
         Assert::AreEqual(static_cast<uint16_t>(l2), m.Get<1>());

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
