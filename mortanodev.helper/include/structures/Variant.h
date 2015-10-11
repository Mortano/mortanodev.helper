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
  static void CopyConstruct(const void* src, void* dst, size_t typeIndex) 
  {
     if (typeIndex == Idx)
     {
        auto& srcObj = *reinterpret_cast<const CurrentType_t*>(src);
        new (dst) CurrentType_t(srcObj);
     }
     else
     {
        ConstructHelper<Idx - 1, Args...>::CopyConstruct(src, dst, typeIndex);
     }
  }

  //! \brief Move construct from src into dst using the type at the given index
  static void MoveConstruct(void* src, void* dst, size_t typeIndex) 
  {
     if (typeIndex == Idx)
     {
        auto& srcObj = *reinterpret_cast<CurrentType_t*>(src);
        new (dst) CurrentType_t(std::move(srcObj));
     }
     else
     {
        ConstructHelper<Idx - 1, Args...>::MoveConstruct(src, dst, typeIndex);
     }
  }

  //! \brief Destruct the object in mem that has the type with the given index
  static void Destruct(void* src, size_t typeIndex) 
  {
     if (typeIndex == Idx)
     {
        auto srcObj = *reinterpret_cast<CurrentType_t*>(src);
        srcObj.~CurrentType_t();
     }
     else
     {
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

  static void Destruct(void* src, size_t typeIndex) 
  {
     MDV_ASSERT(typeIndex == 0);
     auto srcObj = *reinterpret_cast<First*>(src);
     srcObj.~First();
  }
};

//! \brief Metafunction to compare two types by their size (sizeof)
template<typename L, typename R>
struct BiggerType
{
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

  Variant(const ThisType& other)
  {
     ConstructHelper_t::CopyConstruct(other._data, _data, other._index);
     _index = other._index;
  }

  Variant(ThisType&& other) 
  {
     ConstructHelper_t::MoveConstruct(other._data, _data, other._index);
     _index = other._index;
     //We have to set the index of the other object to invalid when moving from it. However this could prevent 
     //the destructor from executing, so we execute it explicitly after moving!
     ConstructHelper_t::Destruct(other._data, other._index); 
     other._index = InvalidIdx;
  }

  template <typename T, typename Decayed_t = std::decay_t<T>>
  Variant(T&& val,
          std::enable_if_t<!std::is_same<ThisType, std::decay_t<T>>::value>* =
              nullptr) 
  {
     new (_data) Decayed_t(std::forward<T>(val));
     _index = meta::IndexOf<T, Types>::value;
  }

  ~Variant()
  {
     if (_index != InvalidIdx)
     {
        ConstructHelper_t::Destruct(_data, _index);
     }
  }

  Variant& operator=(const ThisType& other) 
  {
     auto thisValid = _index != InvalidIdx;
     auto otherValid = other._index != InvalidIdx;
     if (thisValid)
     {
        ConstructHelper_t::Destruct(_data, _index);        
     }
     if(otherValid)
     {
        ConstructHelper_t::CopyConstruct(other._data, _data, other._index);
        _index = other._index;
     }
     else
     {
        _index = InvalidIdx;
     }
     return *this; 
  }

  Variant& operator=(ThisType&& other) 
  {
     auto thisValid = _index != InvalidIdx;
     auto otherValid = other._index != InvalidIdx;
     if (thisValid)
     {
        ConstructHelper_t::Destruct(_data, _index);
     }
     if (otherValid)
     {
        ConstructHelper_t::MoveConstruct(other._data, _data, other._index);
        _index = other._index;        
        //See comment in move constructor!
        ConstructHelper_t::Destruct(other._data, other._index);
        other._index = InvalidIdx;
     }
     else
     {
        _index = InvalidIdx;
     }
     return *this;
  }

  template <typename T, std::enable_if_t<!std::is_same<ThisType, T>::value>>
  Variant& operator=(T&& val) { return *this; }

  void Clear() 
  {
     if (_index == InvalidIdx) return;
     detail::ConstructHelper<ArgCount - 1, Args...>::Destruct(_data, _index);
     _index = InvalidIdx;
  }

  bool HasValue() const 
  {
     return _index != InvalidIdx; 
  }

  template <typename T>
  bool Is() const {
    return meta::IndexOf<T, Types>::value == _index;
  }

  template <typename T>
  T& Get() 
  {
     static_assert(meta::Contains<T, Types>::value, "This is no valid type for this variant!");
     if (!Is<T>()) throw std::exception("Trying to get data of a type from a variant that does not store this type currently!");
     return *reinterpret_cast<T*>(_data);
  }

  template <typename T>
  const T& Get() const 
  {
     static_assert(meta::Contains<T, Types>::value, "This is no valid type for this variant!");
     if (!Is<T>()) throw std::exception("Trying to get data of a type from a variant that does not store this type currently!");
     return *reinterpret_cast<const T*>(_data);
  }

 private:
  constexpr static size_t InvalidIdx = static_cast<size_t>(-1);
  constexpr static size_t MaxSize = meta::Sizeof<meta::MaxOf_t<detail::BiggerType, Types>>::value;
  char _data[MaxSize];
  size_t _index;
};

template<>
class Variant<>
{
public:
   using ThisType = Variant<>;
   using Types = meta::Typelist<>;
   constexpr static size_t ArgCount = 0;

   constexpr Variant() {}

   constexpr Variant(const ThisType& other)
   {
   }
   
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