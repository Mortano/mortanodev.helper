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

   struct _SectionOneBit : std::integral_constant<size_t, 1> {};
   struct _SectionEightBit : std::integral_constant<size_t, 8> {};

   struct _Section1 : std::integral_constant<size_t, 2> {};
   struct _Section2 : std::integral_constant<size_t, 4> {};
   struct _Section3 : std::integral_constant<size_t, 7> {};
   struct _Section4 : std::integral_constant<size_t, 3> {};

   struct _LargeSection : std::integral_constant<size_t, 32> {};

   TEST_CLASS(NamedBitmaskTest)
   {
   public:
      TEST_METHOD(Test_OneField_DefaultCtor)
      {
         using Mask = NamedBitmask<_SectionOneBit>;
         Mask m;
         Assert::AreEqual(static_cast<uint8_t>(0), m.Get<_SectionOneBit>());
      }

      TEST_METHOD(Test_OneField_TupleCtor)
      {
         //TODO Implement
         Assert::Fail(L"Not implemented!");
         //using Mask = Bitmask<1>;
         //std::tuple<size_t> args;
         //std::get<0>(args) = 1;
         //
         //Mask m{ args };
         //Assert::AreEqual(static_cast<uint8_t>(1), m.Get<0>());
      }

      TEST_METHOD(Test_OneField_Set)
      {
         using Mask = NamedBitmask<_SectionOneBit>;
         Mask m;
         m.Set<_SectionOneBit>(1);
         Assert::AreEqual(static_cast<uint8_t>(1), m.Get<_SectionOneBit>());
      }

      TEST_METHOD(Test_OneField_CopyCtor)
      {
         using Mask = NamedBitmask<_SectionOneBit>;
         Mask m;
         m.Set<_SectionOneBit>(1);

         Mask copy{ m };
         Assert::AreEqual(m.Get<_SectionOneBit>(), copy.Get<_SectionOneBit>());
      }

      TEST_METHOD(Test_TwoFields_DefaultCtor)
      {
         using Mask = NamedBitmask<_Section1, _Section2>;
         Mask m;

         Assert::AreEqual(static_cast<uint8_t>(0), m.Get<_Section1>());
         Assert::AreEqual(static_cast<uint8_t>(0), m.Get<_Section2>());
      }

      TEST_METHOD(Test_TwoFields_TupleCtor)
      {
         Assert::Fail(L"Not implemented!");
         //using Mask = Bitmask<3, 3>;
         //
         //std::tuple<size_t, size_t> args;
         //std::get<0>(args) = 3;
         //std::get<1>(args) = 3;
         //
         //Mask m{ args };
         //Assert::AreEqual(static_cast<uint8_t>(std::get<0>(args)), m.Get<0>());
         //Assert::AreEqual(static_cast<uint8_t>(std::get<1>(args)), m.Get<1>());
      }

      TEST_METHOD(Test_TwoFields_CopyCtor)
      {
         using Mask = NamedBitmask<_Section1, _Section2>;

         Mask m;

         const auto l0 = 0b11;
         const auto l1 = 0b0111;

         m.Set<_Section1>(l0);
         m.Set<_Section2>(l1);

         Mask copy{ m };

         Assert::AreEqual(m.Get<_Section1>(), copy.Get<_Section1>());
         Assert::AreEqual(m.Get<_Section2>(), copy.Get<_Section2>());
      }

      TEST_METHOD(Test_TwoFields_Set)
      {
         using Mask = NamedBitmask<_Section1, _Section2>;

         Mask m;

         const auto l0 = 0b11;
         const auto l1 = 0b0111;

         m.Set<_Section1>(l0);
         m.Set<_Section2>(l1);

         Assert::AreEqual(static_cast<uint8_t>(l0), m.Get<_Section1>());
         Assert::AreEqual(static_cast<uint8_t>(l1), m.Get<_Section2>());
      }

      TEST_METHOD(Test_OneBigField_DefaultCtor)
      {
         using Mask = NamedBitmask<_LargeSection>;
         Mask m;
         Assert::AreEqual(0U, m.Get<_LargeSection>());
      }

      TEST_METHOD(Test_OneBigField_TupleCtor)
      {
         Assert::Fail(L"Not implemented!");
         //using Mask = Bitmask<32>;
         //std::tuple<size_t> args;
         //std::get<0>(args) = 0x8f8f8f8f;
         //
         //Mask m{ args };
         //Assert::AreEqual(static_cast<uint32_t>(std::get<0>(args)), m.Get<0>());
      }

      TEST_METHOD(Test_OneBigField_CopyCtor)
      {
         using Mask = NamedBitmask<_LargeSection>;

         Mask m;

         const auto l0 = 0x8f8f8f8fu;

         m.Set<_LargeSection>(l0);

         Mask copy{ m };

         Assert::AreEqual(m.Get<_LargeSection>(), copy.Get<_LargeSection>());
      }

      TEST_METHOD(Test_OneBigField_Set)
      {
         using Mask = NamedBitmask<_LargeSection>;

         Mask m;

         const auto l0 = 0x8f8f8f8fu;

         m.Set<_LargeSection>(l0);

         Assert::AreEqual(l0, m.Get<_LargeSection>());
      }

      TEST_METHOD(Test_ManyFields_Set)
      {
         using Mask = NamedBitmask<_Section1, _Section2, _Section3, _Section4>;
         Mask m;

         const auto l0 = 0b11;
         const auto l1 = 0b0111;
         const auto l2 = 0b0101011;
         const auto l3 = 0b011;

         m.Set<_Section1>(l0);
         m.Set<_Section2>(l1);
         m.Set<_Section3>(l2);
         m.Set<_Section4>(l3);

         Assert::AreEqual(static_cast<uint8_t>(l0), m.Get<_Section1>());
         Assert::AreEqual(static_cast<uint8_t>(l1), m.Get<_Section2>());
         Assert::AreEqual(static_cast<uint8_t>(l2), m.Get<_Section3>());
         Assert::AreEqual(static_cast<uint8_t>(l3), m.Get<_Section4>());
      }

      //Some static asserts
      static_assert(sizeof(NamedBitmask<>) == 1, "Wrong size!"); 
      static_assert(sizeof(NamedBitmask<_SectionOneBit>) == 1, "Wrong size!");
      static_assert(sizeof(NamedBitmask<_SectionEightBit>) == 1, "Wrong size!");
      static_assert(sizeof(NamedBitmask<_LargeSection>) == 4, "Wrong size!");

      static_assert(sizeof(NamedBitmask<_Section1, _Section2>) == 1, "Wrong size!");
      static_assert(sizeof(NamedBitmask<_Section3, _SectionOneBit>) == 1, "Wrong size!");
      static_assert(sizeof(NamedBitmask<_SectionOneBit, _Section3>) == 1, "Wrong size!");
      static_assert(sizeof(NamedBitmask<_Section3, _Section4>) == 2, "Wrong size!");

   };

}
