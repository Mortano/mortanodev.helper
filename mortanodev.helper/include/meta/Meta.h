#pragma once
#include <type_traits>

namespace mdv
{

namespace meta
{

	template<typename T> struct AlwaysFalse : std::false_type {};
	template<size_t Idx> struct AlwaysFalseNumeric : std::false_type {};

//---------- Typelist ----------
	template<typename... Args> struct Typelist {};

//---------- Typelist operations ----------
	template<size_t, typename> struct At;

	template<size_t Idx>
	struct At<Idx, Typelist<>>
	{
		static_assert(AlwaysFalseNumeric<Idx>::value, "At<> is not valid for empty typelist!");
	};

	template<size_t Idx, typename First, typename... Last>
	struct At<Idx, Typelist<First, Last...>>
	{
		static_assert(Idx < (1 + sizeof...(Last)), "Index out of bounds!");
		using type = std::conditional_t<Idx == 0, First, At<Idx - 1, Typelist<Last...>>::type>;
	};

	template<size_t Idx, typename Typelist>
	using At_t = At<Idx, Typelist>::type;


	template<typename> struct Size;

	template<typename... Args>
	struct Size<Typelist<Args...>> :
		std::integral_constant<size_t, sizeof...(Args)>
	{
	};
}

}