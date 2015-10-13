#pragma once
#include <type_traits>

namespace mdv
{

namespace meta
{

	template<typename T> struct AlwaysFalse : std::false_type {};
	template<size_t Idx> struct AlwaysFalseNumeric : std::false_type {};

   template<typename...> using void_t = void;

   template<typename First, typename...>
   struct Redirect
   {
      using type = First;
   };

//---------- Typelist ----------
	template<typename... Args> struct Typelist {};

//---------- Typelist operations ----------
	
#pragma region At
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
#pragma endregion

#pragma region Size
	template<typename> struct Size;

	//! \brief Size of a typelist
	template<typename... Args>
	struct Size<Typelist<Args...>> :
		std::integral_constant<size_t, sizeof...(Args)>
	{
	};
#pragma endregion

#pragma region IndexOf
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
#pragma endregion

#pragma region Contains
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
#pragma endregion

#pragma region PushBack

   template<typename, typename> struct PushBack;

   template<typename Value, typename... Args>
   struct PushBack<Value, Typelist<Args...>>
   {
      using type = Typelist<Args..., Value>;
   };

   //! \brief PushBack metafunction for typelists
   template<typename Value, typename TList>
   using PushBack_t = typename PushBack<Value, TList>::type;

#pragma endregion

#pragma region PushFront

   template<typename, typename> struct PushFront;

   template<typename Value, typename... Args>
   struct PushFront<Value, Typelist<Args...>>
   {
      using type = Typelist<Value, Args...>;
   };

   //! \brief PushFront metafunction for typelists
   template<typename Value, typename TList>
   using PushFront_t = typename PushFront<Value, TList>::type;

#pragma endregion

#pragma region Reverse

   template<typename> struct Reverse;

   template<>
   struct Reverse<Typelist<>>
   {
      using type = Typelist<>;
   };

   template<typename First, typename... Rest>
   struct Reverse<Typelist<First, Rest...>>
   {
      using type = PushBack_t < First, Reverse<Typelist<Rest...>> >;
   };

   //! \brief Reverse the given typelist
   template<typename TList>
   using Reverse_t = typename Reverse<TList>::type;

#pragma endregion

#pragma region MaxOf
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
#pragma endregion

#pragma region Foldl

#pragma region Fold-Helpers

   template<typename, typename> struct And;

   //! \brief And template metafunction
   template<bool L, bool R>
   struct And<std::bool_constant<L>, std::bool_constant<R>>
   {
      using type = std::bool_constant<L && R>;
   };

   template<typename, typename> struct Or;

   //! \brief Or template metafunction
   template<bool L, bool R>
   struct Or<std::bool_constant<L>, std::bool_constant<R>>
   {
      using type = std::bool_constant<L || R>;
   };

#pragma endregion

   template<template<typename, typename> class, typename, typename> struct Foldl;

   template<
      template<typename, typename> class Func,
      typename Initial
   >
   struct Foldl<Func, Initial, Typelist<>>
   {
      using type = Initial;
   };

   template<
      template<typename, typename> class Func,
      typename Initial,
      typename First,
      typename... Rest
   >
   struct Foldl<Func, Initial, Typelist<First, Rest...>>
   {
      using type = typename Foldl<
                                    Func,
                                    typename Func<Initial, First>::type, //Fold from the left, the new initial value is the fold of the initial and the first value
                                    Typelist<Rest...>                    //Then we simply fold with the rest, if there is no rest the result is the initial value
                                 >::type;
   };

   //! \brief Left fold metafunction. Applies a metafunction to all elements of a typelist and folds 
   //!        them from the left, using a given initial value
   template <
      template <typename, typename> class Func, 
      typename Initial, 
      typename TList
   >
   using Foldl_t = typename Foldl<Func, Initial, TList>::type;

#pragma endregion

#pragma region Transform

   template<template<typename> class, typename> struct Transform;

   template<
      template<typename> class Func
   >
   struct Transform<Func, Typelist<>>
   {
      using type = Typelist<>;
   };

   template<
      template<typename> class Func,
      typename First,
      typename... Rest
   >
   struct Transform<Func, Typelist<First, Rest...>>
   {
      using type = PushFront_t<
         typename Func<First>::type, 
         typename Transform<Func, Typelist<Rest...>>::type
      >;
   };

   //! \brief Transform a typelist with the given metafunction
   template<
      template<typename> class Func,
      typename TList
   >
   using Transform_t = typename Transform<Func, TList>::type;

#pragma endregion

   //! \brief sizeof as a meta function
   template<typename T>
   struct Sizeof :
      std::integral_constant<size_t, sizeof(T)>
   {
   };


}

}