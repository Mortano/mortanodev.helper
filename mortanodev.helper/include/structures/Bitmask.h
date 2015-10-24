#pragma once
#include <type_traits>
#include <stdint.h>
#include <cstring>

#include "..\meta\Meta.h"

namespace mdv
{

   namespace detail
   {

      template<size_t Size>
      using SizeToType_t =
         std::conditional_t<
            Size <= sizeof(uint8_t),
            uint8_t,
            std::conditional_t<
               Size <= sizeof(uint16_t),
               uint16_t,
               std::conditional_t<
                  Size <= sizeof(uint32_t),
                  uint32_t,
                  uint64_t                   //Bitmask asserts that the sum is not greater than 64 bits, so this is ok!
               >
            >
         >;

      //! \brief Creates a mask where the first Size bits are set
      //!
      //! Example: Mask<1>::value => 0b00000001
      //!          Mask<2>::value => 0b00000011
      //!          Mask<4>::value => 0b00001111
      //!          etc.
      template<size_t Size>
      struct Mask :
         std::integral_constant<size_t,
            Size == 0 ? 
            0 :
            1 | (Mask<Size-1>::value << 1)
         >
      {
      };

      template<>
      struct Mask<0> :
         std::integral_constant<size_t, 0>
      {
      };

   }

   //! \brief Super-awesome Bitmask of variable size
   //!        Allows to specify a couple of integers that define the size of each section within the bitmask and access
   //!        the data in each of these sections separately.
   //! ONLY SUPPORTS BITMASK OF UP TO 64 BITS!
   template<size_t... Bits>
   class Bitmask
   {
   public:
      using Numbers = meta::Numberlist<Bits...>;
      using TupleOfBits_t = meta::AsTuple_t<Bits...>;
      constexpr static size_t Sections = meta::Size<Numbers>::value;
      constexpr static size_t RequiredSize = meta::Sum<Numbers>::value;     

      constexpr Bitmask() : 
         _data(0)
      {
      }

      //! \brief Constructor that takes a tuple that contains one size_t for each of the sections of this Bitmask
      //! \param args Tuple containing one size_t for each section of this Bitmask
      explicit Bitmask(const TupleOfBits_t& args)
      {
         using Sequence_t = std::make_integer_sequence<size_t, Sections>;
         SetAllFromTuple(args, Sequence_t());
      }

      Bitmask(const Bitmask& other)         
      {
         _data = other._data;
      }

      Bitmask& operator=(const Bitmask& other)
      {
         _data = other._data;
         return *this;
      }

      template<
         size_t Index,
         typename ValueSize_t = detail::SizeToType_t<
            meta::At_t<Index, Numbers>::value
         >
      >
      void Set(ValueSize_t value)
      {
         static_assert(Index < Sections, "Index out of bounds!");         

         //Get the previous sections of this bitmask, these are the bits that we have to skip
         using Previous_t = meta::Take_t<Index, Numbers>;
         //By summing the elements of the previous section, we get the total bit count to skip
         constexpr static size_t OffsetBits = meta::Sum<Previous_t>::value;
         //We also need the size of the current section
         constexpr static size_t SectionBits = meta::At_t<Index, Numbers>::value;
         //Now we can create a mask of the size of the current section
         constexpr static size_t Mask = detail::Mask<SectionBits>::value;

         //First clear the data because we don't want any bits to remain in the section
         _data &= ~(Mask << OffsetBits);
         //Then set the new bits
         _data |= (value & Mask) << OffsetBits;
      }

      //! \brief Get the value inside this bitmask at the given section index
      //! \returns Value of the section at Index
      //! \tparam Index Index of the section to get the value from     
      template<size_t Index>
      decltype(auto) Get() const
      {
         static_assert(Index < Sections, "Index out of bounds!");

         //Get the previous sections of this bitmask, these are the bits that we have to skip
         using Previous_t = meta::Take_t<Index, Numbers>;
         //By summing the elements of the previous section, we get the total bit count to skip
         constexpr static size_t OffsetBits = meta::Sum<Previous_t>::value;
         //We also need the size of the current section
         constexpr static size_t SectionBits = meta::At_t<Index, Numbers>::value;
         //Now we can create a mask of the size of the current section
         constexpr static size_t Mask = detail::Mask<SectionBits>::value;
         //And, neat sideeffect, we can determine the correct return type from the size of the current section
         using Return_t = detail::SizeToType_t<SectionBits>;

         return static_cast<Return_t>( (_data >> OffsetBits) & Mask ); 
      }

   private:
      constexpr static size_t RequiredBytes = (RequiredSize + 7) / 8;
      static_assert(RequiredBytes <= sizeof(uint64_t), "Maximum bitmask size exceeded! Largest supported type is uint64_t!");

      //! \brief Sets each section value by extracting the corresponding value from the tuple
      //! This uses an index_sequence to set along the elements of the tuple, get each element and call Set<> for each element
      template<size_t... Is>
      void SetAllFromTuple(const TupleOfBits_t& tuple, std::index_sequence<Is...>)
      {
         using swallow = int[];
         (void)swallow {
            0, ((void)Set<Is>( std::get<Is>(tuple) ),0)...
         };
      }

      using Data_t = detail::SizeToType_t<RequiredBytes>;
      Data_t _data;
   };

   template<>
   class Bitmask<> {};

}