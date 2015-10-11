#pragma once
#include "..\meta\Meta.h"
#include "..\error_handling\Assert.h"

#include <type_traits>

namespace mdv {

namespace detail {

//! \brief Helper structure that provides methods to construct an object of a
//! type only known at
//! runtime into a memory block. This selects the right type out of a typelist
//! using a runtime index
template <size_t Idx, typename... Args>
struct ConstructHelper {
   using TList = meta::Typelist<Args...>;
   using CurrentType_t = meta::At_t<Idx, TList>;

   //! \brief Copy construct from src into dst using the type at the given index
   static void CopyConstruct(const void* src, void* dst, size_t typeIndex) {
      if (typeIndex == Idx) {
         auto& srcObj = *reinterpret_cast<const CurrentType_t*>(src);
         new (dst) CurrentType_t(srcObj);
      } else {
         ConstructHelper<Idx - 1, Args...>::CopyConstruct(src, dst, typeIndex);
      }
   }

   //! \brief Move construct from src into dst using the type at the given index
   static void MoveConstruct(void* src, void* dst, size_t typeIndex) {
      if (typeIndex == Idx) {
         auto& srcObj = *reinterpret_cast<CurrentType_t*>(src);
         new (dst) CurrentType_t(std::move(srcObj));
      } else {
         ConstructHelper<Idx - 1, Args...>::MoveConstruct(src, dst, typeIndex);
      }
   }

   //! \brief Destruct the object in mem that has the type with the given index
   static void Destruct(void* src, size_t typeIndex) {
      if (typeIndex == Idx) {
         auto srcObj = *reinterpret_cast<CurrentType_t*>(src);
         srcObj.~CurrentType_t();
      } else {
         ConstructHelper<Idx - 1, Args...>::Destruct(src, typeIndex);
      }
   }
};

template <typename First, typename... Rest>
struct ConstructHelper<0, First, Rest...> {
   static void CopyConstruct(const void* src, void* dst, size_t typeIndex) {
      MDV_ASSERT(typeIndex == 0);
      auto& srcObj = *reinterpret_cast<const First*>(src);
      new (dst) First(srcObj);
   }

   static void MoveConstruct(void* src, void* dst, size_t typeIndex) {
      MDV_ASSERT(typeIndex == 0);
      auto& srcObj = *reinterpret_cast<First*>(src);
      new (dst) First(std::move(srcObj));
   }

   static void Destruct(void* src, size_t typeIndex) {
      MDV_ASSERT(typeIndex == 0);
      auto& srcObj = *reinterpret_cast<First*>(src);
      srcObj.~First();
   }
};

//! \brief Metafunction to compare two types by their size (sizeof)
template <typename L, typename R>
struct BiggerType {
   using type = std::conditional_t<sizeof(L) >= sizeof(R), L, R>;
};
}

template <typename... Args>
class Variant {
public:
   constexpr static size_t ArgCount = sizeof...(Args);

   using ThisType = Variant<Args...>;
   using Types = meta::Typelist<Args...>;
   using ConstructHelper_t = detail::ConstructHelper<ArgCount - 1, Args...>;

   Variant() : _index(InvalidIdx) {}

   Variant(const ThisType& other) {
      if (other._index != InvalidIdx) {
         ConstructHelper_t::CopyConstruct(other._data, _data, other._index);
      }
      _index = other._index;
   }

   Variant(ThisType&& other) {
      if (other._index == InvalidIdx) {
         _index = InvalidIdx;
         return;
      }
      ConstructHelper_t::MoveConstruct(other._data, _data, other._index);
      _index = other._index;
      // We are NOT deleting the other objects value (if it has one). Variant should behave as if it
      // is the
      // contained value, so even if we move from it, it still stores a valid instance (if it did so
      // before).
      // The state of that instance is the usual state of objects after being moved from
   }

   template <typename T, typename Decayed_t = std::decay_t<T>>
   explicit Variant(T&& val,
                    std::enable_if_t<!std::is_same<ThisType, std::decay_t<T>>::value>* = nullptr) {
      static_assert(meta::Contains<Decayed_t, Types>::value,
                    "This is no valid type for this variant!");
      new (_data) Decayed_t(std::forward<T>(val));
      _index = meta::IndexOf<Decayed_t, Types>::value;
   }

   ~Variant() {
      if (_index != InvalidIdx) {
         ConstructHelper_t::Destruct(_data, _index);
      }
   }

   Variant& operator=(const ThisType& other) {
      auto thisValid = _index != InvalidIdx;
      auto otherValid = other._index != InvalidIdx;
      if (thisValid) {
         ConstructHelper_t::Destruct(_data, _index);
      }
      if (otherValid) {
         ConstructHelper_t::CopyConstruct(other._data, _data, other._index);
         _index = other._index;
      } else {
         _index = InvalidIdx;
      }
      return *this;
   }

   Variant& operator=(ThisType&& other) {
      auto thisValid = _index != InvalidIdx;
      auto otherValid = other._index != InvalidIdx;
      if (thisValid) {
         ConstructHelper_t::Destruct(_data, _index);
      }
      if (otherValid) {
         ConstructHelper_t::MoveConstruct(other._data, _data, other._index);
         _index = other._index;
         // See comment in move constructor!
      } else {
         _index = InvalidIdx;
      }
      return *this;
   }

   template <typename T, typename Decayed_t = std::decay_t<T>>
   std::enable_if_t<!std::is_same<ThisType, Decayed_t>::value, Variant&> operator=(T&& val) {
      static_assert(meta::Contains<Decayed_t, Types>::value, "This is no valid type for this variant!");
      if (_index != InvalidIdx) {
         ConstructHelper_t::Destruct(_data, _index);
      }
      new (_data) Decayed_t(std::forward<T>(val));
      _index = meta::IndexOf<Decayed_t, Types>::value;
      return *this;
   }

   void Clear() {
      if (_index == InvalidIdx)
         return;
      detail::ConstructHelper<ArgCount - 1, Args...>::Destruct(_data, _index);
      _index = InvalidIdx;
   }

   bool HasValue() const { return _index != InvalidIdx; }

   template <typename T>
   bool Is() const {
      static_assert(meta::Contains<T, Types>::value, "This is no valid type for this variant!");
      return meta::IndexOf<T, Types>::value == _index;
   }

   template <typename T>
   T& Get() {
      static_assert(meta::Contains<T, Types>::value, "This is no valid type for this variant!");
      if (!Is<T>())
         throw std::exception(
             "Trying to get data of a type from a variant that does not store "
             "this type currently!");
      return *reinterpret_cast<T*>(_data);
   }

   template <typename T>
   const T& Get() const {
      static_assert(meta::Contains<T, Types>::value, "This is no valid type for this variant!");
      if (!Is<T>())
         throw std::exception(
             "Trying to get data of a type from a variant that does not store "
             "this type currently!");
      return *reinterpret_cast<const T*>(_data);
   }

private:
   constexpr static size_t InvalidIdx = static_cast<size_t>(-1);
   constexpr static size_t MaxSize = meta::Sizeof<meta::MaxOf_t<detail::BiggerType, Types>>::value;
   char _data[MaxSize];
   size_t _index;
};

template <>
class Variant<> {
public:
   using ThisType = Variant<>;
   using Types = meta::Typelist<>;
   constexpr static size_t ArgCount = 0;

   constexpr Variant() {}

   constexpr Variant(const ThisType& other) {}

   ~Variant() = default;

   Variant& operator=(const ThisType& other) { return *this; }

   void Clear() {}

   bool HasValue() const { return false; }

   template <typename T>
   bool Is() const {
      return false;
   }
};
}