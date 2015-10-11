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

   template<size_t Idx, typename Only>
   struct At<Idx, Typelist<Only>>
   {
      static_assert(Idx == 0, "Index out of bounds!");
      using type = Only;
   };

	//! \brief Access an element of a typelist
	template<size_t Idx, typename First, typename... Last>
	struct At<Idx, Typelist<First, Last...>>
	{
		static_assert(Idx < (1 + sizeof...(Last)), "Index out of bounds!");
		using type = std::conditional_t<Idx == 0, First, typename At<Idx - 1, Typelist<Last...>>::type>;
	};

	template<size_t Idx, typename Typelist>
	using At_t = typename At<Idx, Typelist>::type;


	template<typename> struct Size;

	//! \brief Size of a typelist
	template<typename... Args>
	struct Size<Typelist<Args...>> :
		std::integral_constant<size_t, sizeof...(Args)>
	{
	};


   template<typename, typename> struct IndexOf;

   //! \brief Index of a type in a typelist
   template<typename What, typename First, typename... Rest>
   struct IndexOf<What, Typelist<First, Rest...>>       
   {
   private:
      constexpr static size_t tmp = IndexOf<What, Typelist<Rest...>>::value;
   public:
#pragma warning(push)
#pragma warning(disable: 4307) //Compiler complains about integral overflow because we *might* add 1 to size_t max value. However, this will never happen because of the if check!
      constexpr static size_t value = std::is_same<What, First>::value ? 0 :
         (tmp == static_cast<size_t>(-1) ? -1 : 1 + tmp);
#pragma warning(pop)
   };

   template<typename What>
   struct IndexOf<What, Typelist<>>
   {
      constexpr static size_t value = static_cast<size_t>(-1);
   };


   template<typename, typename> struct Contains;

   //! \brief Is a type contained within a typelist?
   template<typename What, typename First, typename... Rest>
   struct Contains<What, Typelist<First, Rest...>> :
      std::bool_constant<
         std::is_same<What, First>::value | Contains<What, Typelist<Rest...>>::value
      >
   {
   };

   //! \brief Recursion stop case for empty type list
   template<typename What>
   struct Contains<What, Typelist<>> :
      std::bool_constant<false>
   {
   };


   template<template <typename, typename> class, typename> struct MaxOf;

   //! \brief Maximum element of a typelist using a custom comparator
   template<template <typename, typename> class Comp, typename First, typename... Rest>
   struct MaxOf<Comp, Typelist<First, Rest...>>
   {
      using type = typename Comp<First, typename MaxOf<Comp, Typelist<Rest...>>::type>::type;
   };

   template<template <typename, typename> class Comp, typename Only>
   struct MaxOf<Comp, Typelist<Only>>
   {
      using type = Only;
   };

   //! \brief Type alias to get the maximum element of a typelist using a compare metafunction
   template<template <typename, typename> class Comp, typename TList>
   using MaxOf_t = typename MaxOf<Comp, TList>::type;


   //! \brief sizeof as a meta function
   template<typename T>
   struct Sizeof :
      std::integral_constant<size_t, sizeof(T)>
   {
   };


}

}