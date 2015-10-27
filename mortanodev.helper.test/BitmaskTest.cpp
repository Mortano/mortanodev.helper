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

   constexpr static struct FirstSection : std::integral_constant<size_t, 2> {} Section1;
   constexpr static struct SecondSection : std::integral_constant<size_t, 4> {} Section2;
   constexpr static struct ThirdSection : std::integral_constant<size_t, 7> {} Section3;
   constexpr static struct FourthSection : std::integral_constant<size_t, 3> {} Section4;

   TEST_CLASS(BitmaskTest)
   {
   public:
      TEST_METHOD(Test_OneField_DefaultCtor)
      {
         using Mask = Bitmask<1>;
         Mask m;
         Assert::AreEqual(static_cast<uint8_t>(0), m.Get<0>());
      }

      TEST_METHOD(Test_OneField_ArgsCtor)
      {
         using Mask = Bitmask<1>;

         Mask m{ 1 };
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

      TEST_METHOD(Test_TwoFields_ArgsCtor)
      {
         using Mask = Bitmask<3, 3>;
         
         Mask m{ 3, 3 };
         Assert::AreEqual(static_cast<uint8_t>(3), m.Get<0>());
         Assert::AreEqual(static_cast<uint8_t>(3), m.Get<1>());
      }

      TEST_METHOD(Test_TwoFields_CopyCtor)
      {
         using Mask = Bitmask<3, 3>;

         Mask m{ 3, 3 };
         Mask copy{ m };

         Assert::AreEqual(static_cast<uint8_t>(3), copy.Get<0>());
         Assert::AreEqual(static_cast<uint8_t>(3), copy.Get<1>());
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
         const auto val = 0x8f8f8f8f;

         Mask m{ val };
         Assert::AreEqual(static_cast<uint32_t>(val), m.Get<0>());
      }

      TEST_METHOD(Test_OneBigField_CopyCtor)
      {
         using Mask = Bitmask<32>;
         const auto val = 0x8f8f8f8f;

         Mask m{ val };
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

      TEST_METHOD(Test_ManyFields_Set)
      {
         using Mask = Bitmask<2, 3, 12, 4, 7, 2>;
         Mask m;

         const auto l0 = 0b11;
         const auto l1 = 0b011;
         const auto l2 = 0b010101010101;
         const auto l3 = 0b0111;
         const auto l4 = 0b01010101;
         const auto l5 = 0b01;

         m.Set<0>(l0);
         m.Set<1>(l1);
         m.Set<2>(l2);
         m.Set<3>(l3);
         m.Set<4>(l4);
         m.Set<5>(l5);

         Assert::AreEqual(static_cast<uint8_t>(l0), m.Get<0>());
         Assert::AreEqual(static_cast<uint8_t>(l1), m.Get<1>());
         Assert::AreEqual(static_cast<uint16_t>(l2), m.Get<2>());
         Assert::AreEqual(static_cast<uint8_t>(l3), m.Get<3>());
         Assert::AreEqual(static_cast<uint8_t>(l4), m.Get<4>());
         Assert::AreEqual(static_cast<uint8_t>(l5), m.Get<5>());
      }

      TEST_METHOD(Test_UseTypesForSections)
      {
         //using Mask = Bitmask<Section1(), Section2(), Section3(), Section4()>;
         //static_assert(sizeof(Mask) == sizeof(uint16_t), "Wrong size!");
         //
         //Mask m;
         //
         //const auto l0 = 0b11;
         //const auto l1 = 0b0101;
         //const auto l2 = 0b0101011;
         //const auto l3 = 0b011;
         //
         //m.Set<Section1()>(l0);
         //m.Set<Section2()>(l1);
         //m.Set<Section3()>(l2);
         //m.Set<Section4()>(l3);
         //
         //Assert::AreEqual(static_cast<uint8_t>(l0), m.Get<Section1()>());
         //Assert::AreEqual(static_cast<uint8_t>(l1), m.Get<Section2()>());
         //Assert::AreEqual(static_cast<uint8_t>(l2), m.Get<Section3()>());
         //Assert::AreEqual(static_cast<uint8_t>(l3), m.Get<Section4()>());
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
