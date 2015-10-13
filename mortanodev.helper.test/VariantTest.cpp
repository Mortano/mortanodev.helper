#include "stdafx.h"
#include "CppUnitTest.h"

#include "structures\Variant.h"

#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mortanodevhelpertest
{		

   size_t operator"" _sz_t(unsigned long long val)
   {
      return static_cast<size_t>(val);
   }


   struct Counter
   {
      static size_t DefaultCtorCalls;
      static size_t CopyCtorCalls;
      static size_t MoveCtorCalls;
      static size_t DtorCalls;

      Counter() { DefaultCtorCalls++; }
      Counter(const Counter&) { CopyCtorCalls++; }
      Counter(Counter&&) { MoveCtorCalls++; }
      ~Counter() { DtorCalls++; }

      static void Reset()
      {
         DefaultCtorCalls = 0;
         CopyCtorCalls = 0;
         MoveCtorCalls = 0;
         DtorCalls = 0;
      }
   };

   size_t Counter::DefaultCtorCalls = 0;
   size_t Counter::CopyCtorCalls = 0;
   size_t Counter::MoveCtorCalls = 0;
   size_t Counter::DtorCalls = 0;

	TEST_CLASS(VariantTest)
	{
	public:
		
		TEST_METHOD(Test_Empty_DefaultCtor)
		{
			mdv::Variant<> v1;

			Assert::IsFalse(v1.HasValue());
		}

		TEST_METHOD(Test_Empty_CopyCtor)
		{
			mdv::Variant<> v1;
			mdv::Variant<> v2(v1);

			Assert::AreEqual(v1.HasValue(), v2.HasValue());
		}

		TEST_METHOD(Test_Int_DefaultCtor)
		{
			mdv::Variant<int> v1;                  

			Assert::IsFalse(v1.HasValue());
			Assert::ExpectException<std::exception>([&v1]() { v1.Get<int>(); });
		}

		TEST_METHOD(Test_Int_ArgCtor)
		{
			mdv::Variant<int> v1(42);

			Assert::IsTrue(v1.HasValue());
			Assert::IsTrue(v1.Is<int>());
			Assert::AreEqual(42, v1.Get<int>());
		}

		TEST_METHOD(Test_Int_CopyCtor)
		{
			mdv::Variant<int> v1(42);
			mdv::Variant<int> v2(v1);

			Assert::AreEqual(v1.HasValue(), v2.HasValue());
			Assert::AreEqual(v1.Get<int>(), v2.Get<int>());
		}

		TEST_METHOD(Test_Int_MoveCtor)
		{
			mdv::Variant<int> src(42);
			mdv::Variant<int> dst(std::move(src));

			Assert::IsTrue(src.HasValue());
			Assert::IsTrue(dst.HasValue());
			Assert::AreEqual(42, dst.Get<int>());
		}

		TEST_METHOD(Test_Int_CopyAssign)
		{
			mdv::Variant<int> v1;
			mdv::Variant<int> v2(23);

			v1 = v2;

			Assert::AreEqual(v2.HasValue(), v1.HasValue());
			Assert::AreEqual(v2.Get<int>(), v1.Get<int>());
		}

		TEST_METHOD(Test_Int_MoveAssign)
		{
			mdv::Variant<int> src(42);
			mdv::Variant<int> dst;

			dst = std::move(src);

			Assert::IsTrue(src.HasValue());
			Assert::IsTrue(dst.HasValue());
			Assert::AreEqual(42, dst.Get<int>());
		}

		TEST_METHOD(Test_Int_ValueAssign)
		{
			mdv::Variant<int> v;

			v = 42;

			Assert::IsTrue(v.HasValue());
			Assert::AreEqual(42, v.Get<int>());
		}

		TEST_METHOD(Test_Int_Clear)
		{
			mdv::Variant<int> v(42);

			v.Clear();

			Assert::IsFalse(v.HasValue());
			Assert::IsFalse(v.Is<int>());
			Assert::ExpectException<std::exception>([&v]() {v.Get<int>();});
		}

		TEST_METHOD(Test_String_DefaultCtor)
		{
			mdv::Variant<std::string> v;

			Assert::IsFalse(v.HasValue());
			Assert::ExpectException<std::exception>([&v]() { v.Get<std::string>(); });
		}

		TEST_METHOD(Test_String_ArgCtor)
		{
			using namespace std::string_literals;
			mdv::Variant<std::string> v( "Hello"s );

			Assert::IsTrue(v.HasValue());
			Assert::IsTrue(v.Is<std::string>());
         auto& got = v.Get<std::string>();
			Assert::AreEqual("Hello"s, v.Get<std::string>());
		}

		TEST_METHOD(Test_String_CopyCtor)
		{
			using namespace std::string_literals;
			mdv::Variant<std::string> v("Hello"s);
			mdv::Variant<std::string> copy(v);

			Assert::AreEqual(v.HasValue(), copy.HasValue());
			Assert::AreEqual(v.Get<std::string>(), copy.Get<std::string>());
		}

		TEST_METHOD(Test_String_MoveCtor)
		{
			using namespace std::string_literals;
			mdv::Variant<std::string> src("Hello"s);
			mdv::Variant<std::string> dst(std::move(src));

			Assert::IsTrue(src.HasValue());
			Assert::IsTrue(dst.HasValue());
			Assert::AreEqual("Hello"s, dst.Get<std::string>());
		}

		TEST_METHOD(Test_String_CopyAssign)
		{
			using namespace std::string_literals;
			mdv::Variant<std::string> v1("Hello"s);
			mdv::Variant<std::string> v2;

			v2 = v1;

			Assert::AreEqual(v1.HasValue(), v2.HasValue());
			Assert::AreEqual(v1.Get<std::string>(), v2.Get<std::string>());
		}

		TEST_METHOD(Test_String_MoveAssign)
		{
			using namespace std::string_literals;
			mdv::Variant<std::string> src("Hello"s);
			mdv::Variant<std::string> dst;

			dst = std::move(src);

			Assert::IsTrue(src.HasValue());
			Assert::IsTrue(dst.HasValue());
			Assert::AreEqual("Hello"s, dst.Get<std::string>());
		}

		TEST_METHOD(Test_String_ValueAssign)
		{
			using namespace std::string_literals;

         mdv::Variant<std::string> v("HansWurst"s);         
         v.Get<std::string>();

			v = "Hello"s;

			Assert::IsTrue(v.HasValue());
			Assert::AreEqual("Hello"s, v.Get<std::string>());
		}

		TEST_METHOD(Test_String_Clear)
		{
			using namespace std::string_literals;
			mdv::Variant<std::string> v("Hello"s);

			v.Clear();

			Assert::IsFalse(v.HasValue());
			Assert::IsFalse(v.Is<std::string>());
			Assert::ExpectException<std::exception>([&v]() { v.Get<std::string>(); });
		}

		TEST_METHOD(Test_TwoPrimitives_DefaultCtor)
		{
			using Var_t = mdv::Variant<int, float>;
			Var_t v;

			Assert::IsFalse(v.HasValue());
			Assert::IsFalse(v.Is<int>());
			Assert::IsFalse(v.Is<float>());
			Assert::ExpectException<std::exception>([&v]() { v.Get<int>(); });
			Assert::ExpectException<std::exception>([&v]() { v.Get<float>(); });
		}

		TEST_METHOD(Test_TwoPrimitives_ArgCtor)
		{
			using Var_t = mdv::Variant<int, float>;
			Var_t v1(42); //Will be int

			Assert::IsTrue(v1.HasValue());
			Assert::IsTrue(v1.Is<int>());
			Assert::AreEqual(42, v1.Get<int>());

			Assert::IsFalse(v1.Is<float>());
			Assert::ExpectException<std::exception>([&v1]() { v1.Get<float>(); });

			Var_t v2(42.f);
			Assert::IsTrue(v2.HasValue());
			Assert::IsTrue(v2.Is<float>());
			Assert::AreEqual(42.f, v2.Get<float>());

			Assert::IsFalse(v2.Is<int>());
			Assert::ExpectException<std::exception>([&v2]() { v2.Get<int>(); });
		}

      TEST_METHOD(Test_TwoPrimitives_CopyCtor)
      {
         using Var_t = mdv::Variant<int, float>;
         Var_t v1(42);
         Var_t v2(v1);

         Assert::AreEqual(v1.HasValue(), v2.HasValue());
         Assert::AreEqual(v1.Is<int>(), v2.Is<int>());
         Assert::AreEqual(v1.Get<int>(), v2.Get<int>());

         Var_t v3(42.f);
         Var_t v4(v3);

         Assert::AreEqual(v3.HasValue(), v4.HasValue());
         Assert::AreEqual(v3.Is<float>(), v4.Is<float>());
         Assert::AreEqual(v3.Get<float>(), v4.Get<float>());
      }

      TEST_METHOD(Test_TwoPrimitives_CopyFromEmpty)
      {
         using Var_t = mdv::Variant<int, float>;
         Var_t v1;
         Var_t v2(v1);

         Assert::AreEqual(v1.HasValue(), v2.HasValue());
         Assert::AreEqual(v1.Is<int>(), v2.Is<int>());
         Assert::AreEqual(v1.Is<float>(), v2.Is<float>());
      }

      TEST_METHOD(Test_TwoPrimitives_MoveCtor)
      {
         using Var_t = mdv::Variant<int, float>;
         Var_t v1(42);
         Var_t v2(std::move(v1));

         Assert::IsTrue(v2.HasValue());
         Assert::IsTrue(v2.Is<int>());
         Assert::AreEqual(42, v2.Get<int>());

         Assert::IsTrue(v1.HasValue());
         Assert::IsTrue(v1.Is<int>());

         Var_t v3(42.f);
         Var_t v4(std::move(v3));

         Assert::IsTrue(v4.HasValue());
         Assert::IsTrue(v4.Is<float>());
         Assert::AreEqual(42.f, v4.Get<float>());

         Assert::IsTrue(v3.HasValue());
         Assert::IsTrue(v3.Is<float>());
      }

      TEST_METHOD(Test_TwoPrimitives_CopyAssign)
      {
         using Var_t = mdv::Variant<int, float>;

         Var_t v1(42);
         Var_t v2(23);

         v2 = v1;

         Assert::AreEqual(v1.HasValue(), v2.HasValue());
         Assert::AreEqual(v1.Is<int>(), v2.Is<int>());
         Assert::AreEqual(v1.Get<int>(), v2.Get<int>());

         Var_t v3(42.f);
         Var_t v4(23.f);

         v4 = v3;

         Assert::AreEqual(v3.HasValue(), v4.HasValue());
         Assert::AreEqual(v3.Is<float>(), v4.Is<float>());
         Assert::AreEqual(v3.Get<float>(), v4.Get<float>());

         //Cross asign to different type!
         v4 = v1;

         Assert::AreEqual(v1.HasValue(), v4.HasValue());
         Assert::AreEqual(v1.Is<int>(), v4.Is<int>());
         Assert::AreEqual(v1.Get<int>(), v4.Get<int>());
      }

      TEST_METHOD(Test_TwoPrimitives_MoveAssign)
      {

         using Var_t = mdv::Variant<int, float>;

         Var_t v1(42);
         Var_t v2(23);

         v2 = std::move(v1);

         Assert::IsTrue(v2.HasValue());
         Assert::IsTrue(v2.Is<int>());
         Assert::AreEqual(42, v2.Get<int>());

         Assert::IsTrue(v1.HasValue());
         Assert::IsTrue(v1.Is<int>());

         Var_t v3(42.f);
         Var_t v4(23.f);

         v4 = std::move(v3);

         Assert::IsTrue(v4.HasValue());
         Assert::IsTrue(v4.Is<float>());
         Assert::AreEqual(42.f, v4.Get<float>());

         Assert::IsTrue(v3.HasValue());
         Assert::IsTrue(v3.Is<float>());
         
         //Cross asign to different type!
         v4 = std::move(v2);

         Assert::IsTrue(v4.HasValue());
         Assert::IsTrue(v4.Is<int>());
         Assert::AreEqual(42, v4.Get<int>());

         Assert::IsTrue(v2.HasValue());
         Assert::IsTrue(v2.Is<int>());
      }

      TEST_METHOD(Test_TwoPrimitives_ValueAssign)
      {
         using Var_t = mdv::Variant<int, float>;

         Var_t v(42);

         v = 42.f;

         Assert::IsTrue(v.HasValue());
         Assert::IsTrue(v.Is<float>());
         Assert::AreEqual(42.f, v.Get<float>());

         v = 23;

         Assert::IsTrue(v.HasValue());
         Assert::IsTrue(v.Is<int>());
         Assert::AreEqual(23, v.Get<int>());
      }

      TEST_METHOD(Test_TwoPrimitives_Clear)
      {
         using Var_t = mdv::Variant<int, float>;

         Var_t v1(23);

         v1.Clear();

         Assert::IsFalse(v1.HasValue());
         Assert::IsFalse(v1.Is<int>());
         Assert::IsFalse(v1.Is<float>());

         Var_t v2(42.f);

         v2.Clear();

         Assert::IsFalse(v2.HasValue());
         Assert::IsFalse(v2.Is<int>());
         Assert::IsFalse(v2.Is<float>());
      }

      TEST_METHOD(Test_TwoMixed_DefaultCtor)
      {
         using Var_t = mdv::Variant<int, std::string>;

         Var_t v;

         Assert::IsFalse(v.HasValue());
         Assert::IsFalse(v.Is<int>());
         Assert::IsFalse(v.Is<std::string>());
         Assert::ExpectException<std::exception>([&v]() { v.Get<int>(); });
         Assert::ExpectException<std::exception>([&v]() { v.Get<std::string>(); });
      }

      TEST_METHOD(Test_TwoMixed_ArgCtor)
      {
         using namespace std::string_literals;
         using Var_t = mdv::Variant<int, std::string>;

         Var_t v1(42);

         Assert::IsTrue(v1.HasValue());
         Assert::IsTrue(v1.Is<int>());
         Assert::AreEqual(42, v1.Get<int>());

         Var_t v2("Hello"s);

         Assert::IsTrue(v2.HasValue());
         Assert::IsTrue(v2.Is<std::string>());
         Assert::AreEqual("Hello"s, v2.Get<std::string>());
      }

      TEST_METHOD(Test_TwoMixed_CopyCtor)
      {
         using namespace std::string_literals;
         using Var_t = mdv::Variant<int, std::string>;

         Var_t v1(42);
         Var_t v2(v1);

         Assert::AreEqual(v1.HasValue(), v2.HasValue());
         Assert::AreEqual(v1.Is<int>(), v2.Is<int>());
         Assert::AreEqual(v1.Get<int>(), v2.Get<int>());

         Var_t v3("Hello"s);
         Var_t v4(v3);

         Assert::AreEqual(v3.HasValue(), v4.HasValue());
         Assert::AreEqual(v3.Is<std::string>(), v4.Is<std::string>());
         Assert::AreEqual(v3.Get<std::string>(), v4.Get<std::string>());
      }

      TEST_METHOD(Test_TwoMixed_CopyFromEmpty)
      {
         using Var_t = mdv::Variant<int, std::string>;

         Var_t v1;
         Var_t v2(v1);

         Assert::AreEqual(v1.HasValue(), v2.HasValue());
         Assert::AreEqual(v1.Is<int>(), v2.Is<int>());
      }

      TEST_METHOD(Test_TwoMixed_MoveCtor)
      {
         using namespace std::string_literals;
         using Var_t = mdv::Variant<int, std::string>;

         Var_t v1(42);
         Var_t v2(std::move(v1));

         Assert::IsTrue(v2.HasValue());
         Assert::IsTrue(v2.Is<int>());
         Assert::AreEqual(42, v2.Get<int>());

         Assert::IsTrue(v1.HasValue());
         Assert::IsTrue(v1.Is<int>());

         Var_t v3("Hello"s);
         Var_t v4(std::move(v3));

         Assert::IsTrue(v4.HasValue());
         Assert::IsTrue(v4.Is<std::string>());
         Assert::AreEqual("Hello"s, v4.Get<std::string>());

         Assert::IsTrue(v3.HasValue());
         Assert::IsTrue(v3.Is<std::string>());
      }

      TEST_METHOD(Test_TwoMixed_CopyAssign)
      {
         using namespace std::string_literals;
         using Var_t = mdv::Variant<int, std::string>;

         Var_t v1(42);
         Var_t v2(23);

         v2 = v1;

         Assert::AreEqual(v1.HasValue(), v2.HasValue());
         Assert::AreEqual(v1.Is<int>(), v2.Is<int>());
         Assert::AreEqual(v1.Get<int>(), v2.Get<int>());

         Var_t v3("Hello"s);
         Var_t v4("HansWurst"s);

         v4 = v3;

         Assert::AreEqual(v3.HasValue(), v4.HasValue());
         Assert::AreEqual(v3.Is<std::string>(), v4.Is<std::string>());
         Assert::AreEqual(v3.Get<std::string>(), v4.Get<std::string>());

         //Cross type assign
         Var_t v5("Hello"s);
         Var_t v6(42);

         v6 = v5;

         Assert::AreEqual(v5.HasValue(), v6.HasValue());
         Assert::AreEqual(v5.Is<std::string>(), v6.Is<std::string>());
         Assert::AreEqual(v5.Get<std::string>(), v6.Get<std::string>());
      }

      TEST_METHOD(Test_TwoMixed_MoveAssign)
      {
         using namespace std::string_literals;
         using Var_t = mdv::Variant<int, std::string>;

         Var_t v1(42);
         Var_t v2(23);

         v2 = std::move(v1);

         Assert::IsTrue(v2.HasValue());
         Assert::IsTrue(v2.Is<int>());
         Assert::AreEqual(42, v2.Get<int>());

         Assert::IsTrue(v1.HasValue());
         Assert::IsTrue(v1.Is<int>());

         Var_t v3("Hello"s);
         Var_t v4("HansWurst"s);

         v4 = std::move(v3);

         Assert::IsTrue(v4.HasValue());
         Assert::IsTrue(v4.Is<std::string>());
         Assert::AreEqual("Hello"s, v4.Get<std::string>());

         Assert::IsTrue(v3.HasValue());
         Assert::IsTrue(v3.Is<std::string>());

         //Cross type assign
         Var_t v5("Hello"s);
         Var_t v6(42);

         v6 = std::move(v5);

         Assert::IsTrue(v6.HasValue());
         Assert::IsTrue(v6.Is<std::string>());
         Assert::AreEqual("Hello"s, v6.Get<std::string>());

         Assert::IsTrue(v5.HasValue());
         Assert::IsTrue(v5.Is<std::string>());
      }

      TEST_METHOD(Test_TwoMixed_ValueAssign)
      {
         using namespace std::string_literals;
         using Var_t = mdv::Variant<int, std::string>;

         Var_t v1(23);

         v1 = "Hello"s;

         Assert::IsTrue(v1.HasValue());
         Assert::IsTrue(v1.Is<std::string>());
         Assert::AreEqual("Hello"s, v1.Get<std::string>());

         v1 = 42;

         Assert::IsTrue(v1.HasValue());
         Assert::IsTrue(v1.Is<int>());
         Assert::AreEqual(42, v1.Get<int>());
      }

      TEST_METHOD(Test_TwoMixed_Clear)
      {
         using namespace std::string_literals;
         using Var_t = mdv::Variant<int, std::string>;

         Var_t v1(42);

         v1.Clear();

         Assert::IsFalse(v1.HasValue());
         Assert::IsFalse(v1.Is<int>());

         v1 = "Hello"s;

         v1.Clear();

         Assert::IsFalse(v1.HasValue());
         Assert::IsFalse(v1.Is<std::string>());
      }

      TEST_METHOD(Test_ConstructorsCalled)
      {
         using Var_t = mdv::Variant<Counter>;

         {
            Var_t v1;

            Assert::AreEqual(0_sz_t, Counter::DefaultCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::CopyCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::MoveCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::DtorCalls);
         }

         //No destructors must be called when empty Variant gets destroyed!
         Assert::AreEqual(0_sz_t, Counter::DtorCalls);

         {
            Var_t v2{Counter()};

            Assert::AreEqual(1_sz_t, Counter::DefaultCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::CopyCtorCalls);
            Assert::AreEqual(1_sz_t, Counter::MoveCtorCalls);
            Assert::AreEqual(1_sz_t, Counter::DtorCalls);
         }

         Assert::AreEqual(2_sz_t, Counter::DtorCalls);
         Counter::Reset();

         {
            Counter counter;
            Var_t v2{ counter };

            Assert::AreEqual(1_sz_t, Counter::DefaultCtorCalls);
            Assert::AreEqual(1_sz_t, Counter::CopyCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::MoveCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::DtorCalls);
         }

         Assert::AreEqual(2_sz_t, Counter::DtorCalls); //'counter' object and copy inside the variant!
         Counter::Reset();

         {
            Counter counter;
            Var_t v1{ counter };

            Counter::Reset();

            Var_t v2(v1);

            Assert::AreEqual(0_sz_t, Counter::DefaultCtorCalls);
            Assert::AreEqual(1_sz_t, Counter::CopyCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::MoveCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::DtorCalls);
         }

         Assert::AreEqual(3_sz_t, Counter::DtorCalls);
         Counter::Reset();

         {
            Counter counter;
            Var_t v1{ counter };

            Counter::Reset();

            Var_t v2(std::move(v1));

            Assert::AreEqual(0_sz_t, Counter::DefaultCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::CopyCtorCalls);
            Assert::AreEqual(1_sz_t, Counter::MoveCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::DtorCalls);
         }

         Assert::AreEqual(3_sz_t, Counter::DtorCalls);
         Counter::Reset();

         {
            Counter counter;
            Var_t v1{ counter };

            Var_t v2;

            Counter::Reset();

            v2 = v1;

            Assert::AreEqual(0_sz_t, Counter::DefaultCtorCalls);
            Assert::AreEqual(1_sz_t, Counter::CopyCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::MoveCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::DtorCalls);
         }

         Assert::AreEqual(3_sz_t, Counter::DtorCalls);
         Counter::Reset();

         {
            Counter counter;
            Var_t v1{ counter };

            Var_t v2;

            Counter::Reset();

            v2 = std::move(v1);

            Assert::AreEqual(0_sz_t, Counter::DefaultCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::CopyCtorCalls);
            Assert::AreEqual(1_sz_t, Counter::MoveCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::DtorCalls);
         }

         Assert::AreEqual(3_sz_t, Counter::DtorCalls);
         Counter::Reset();

         {
            Var_t v1{ Counter() };

            Counter::Reset();

            v1 = Counter();

            Assert::AreEqual(1_sz_t, Counter::DefaultCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::CopyCtorCalls);
            Assert::AreEqual(1_sz_t, Counter::MoveCtorCalls);
            Assert::AreEqual(2_sz_t, Counter::DtorCalls);

            Counter tmp;

            Counter::Reset();

            v1 = tmp;

            Assert::AreEqual(0_sz_t, Counter::DefaultCtorCalls);
            Assert::AreEqual(1_sz_t, Counter::CopyCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::MoveCtorCalls);
            Assert::AreEqual(1_sz_t, Counter::DtorCalls);
         }

         Assert::AreEqual(3_sz_t, Counter::DtorCalls);
         Counter::Reset();

         {
            Var_t v1{ Counter() };

            Counter::Reset();

            v1.Clear();

            Assert::AreEqual(0_sz_t, Counter::DefaultCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::CopyCtorCalls);
            Assert::AreEqual(0_sz_t, Counter::MoveCtorCalls);
            Assert::AreEqual(1_sz_t, Counter::DtorCalls);
         }

         Counter::Reset();         
      }


//TODO We need some tool that checks the compiler output 
#ifdef CHECK_IF_COMPILES

      TEST_METHOD(Test_NonCopyable)
      {
         struct NonCopyable
         {
            NonCopyable() = default;
            NonCopyable(const NonCopyable&) = delete;
         };

         mdv::Variant<NonCopyable> v;
         mdv::Variant<NonCopyable> v2(v); 

         mdv::Variant<int, NonCopyable> v3;
         mdv::Variant<int, NonCopyable> v4(v3);
      }

      TEST_METHOD(Test_NonMoveable) 
      {
         struct NonMoveable
         {
            NonMoveable() = default;
            NonMoveable(const NonMoveable&) = default;
            NonMoveable(NonMoveable&&) = delete;
         };

         mdv::Variant<NonMoveable> v;
         mdv::Variant<NonMoveable> v2(std::move(v)); //This should compile because we fall back to copy here!
      }

#endif

	};

}
