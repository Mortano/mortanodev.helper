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
      struct SizeToType
      {
         using type = std::conditional_t<
             Size <= sizeof(uint8_t) * 8,
             uint8_t,
             std::conditional_t<Size <= sizeof(uint16_t) * 8,
                                uint16_t,
                                std::conditional_t<Size <= sizeof(uint32_t) * 8,
                                                   uint32_t,
                                                   uint64_t  // Bitmask asserts that the sum is not
                                                             // greater than 64 bits, so this is ok!
                                                   > > >;
      };

      template<size_t Size>
      using SizeToType_t = typename SizeToType<Size>::type;

      template<typename T>
      using SizeTypeToType_t = typename SizeToType<T::value>::type;

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

      template<
         typename SectionList,      //A list with all sections of the Bitmask
         size_t CumulativeOffset   //The cumulative offset of all previous sections
      >
      struct MaskAndOffset
      {
      };

      //Once this works, I will never be able to understand it :(

      template<
         size_t FirstSection, 
         size_t... OtherSections, 
         size_t CumulativeOffset>
      struct MaskAndOffset<
         meta::Numberlist<FirstSection, OtherSections...>, 
         CumulativeOffset
      >
      {
         template<typename First, typename... Other>
         static uint64_t Get(First first, Other... other)
         {
            static_assert(sizeof...(OtherSections) == sizeof...(Other), "Section parameter pack and function arguments must have the same size!");
      
            constexpr static size_t Mask = detail::Mask<FirstSection>::value;
            //Mask and shift the current value and then OR with the remaining values
            return ((first & Mask) << CumulativeOffset) |
               MaskAndOffset<
                  meta::Numberlist<OtherSections...>, //Skip the current section
                  CumulativeOffset + FirstSection    //Add the size of this section to the offset
               >::Get(other...);
         }
      };
      
      template<size_t FirstSection, size_t CumulativeOffset>
      struct MaskAndOffset<
         meta::Numberlist<FirstSection>,
         CumulativeOffset
      >
      {
         template<typename First>
         static uint64_t Get(First first)
         {
            constexpr static size_t Mask = detail::Mask<FirstSection>::value;
            return ((first & Mask) << CumulativeOffset);
         }
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

      //! \brief Default ctor, sets all bits to zero
      constexpr Bitmask() : 
         _data(0)
      {
      }

      //! \brief Initializes this bitmask with the given values
      //! \param args One value for each section of this Bitmask. The type of that value matches the size of 
      //!             the section (uint8_t for 8 bit or less, uint16_t for 16 bits or less etc.)
      constexpr explicit Bitmask(detail::SizeToType_t<Bits>... args) :
         _data(0) //TODO Assemble all args into one bitmask at once and assign this to _data!
      {
         using Indices_t = std::make_index_sequence<Sections>;
         SetAllFromArgs(args..., Indices_t());
      }

      //! \brief Copy ctor
      Bitmask(const Bitmask& other)         
      {
         _data = other._data;
      }

      //! \brief Copy assignment
      Bitmask& operator=(const Bitmask& other)
      {
         _data = other._data;
         return *this;
      }

      //! \brief Sets the bits of the section with the given index
      //! \param value Value for the bits in the section
      //! \tparam Index The index of the section for which to set the bits
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
      inline void SetAllFromArgs(detail::SizeToType_t<Bits>... args, std::index_sequence<Is...>)
      {
         using swallow = int[];
         (void)swallow {
            0, ((void)Set<Is>( args ),0)...
         };
      }

      using Data_t = detail::SizeToType_t<RequiredSize>;
      Data_t _data;
   };

   template<>
   class Bitmask<> {};

   //! \brief Bitmask class that uses named sections
   template<typename... NamedBits>
   class NamedBitmask
   {
   public:
      using NamedSections = meta::Typelist<NamedBits...>;
      using Numbers = meta::NumberlistFromTypelist_t<NamedSections>;
      constexpr static size_t Sections = meta::Size<Numbers>::value;
      constexpr static size_t RequiredSize = meta::Sum<Numbers>::value;

      constexpr NamedBitmask() :
         _data(0)
      {
      }

      //! \brief Initializes this bitmask with the given values
      //! \param args One value for each section of this Bitmask. The type of that value matches the size of 
      //!             the section (uint8_t for 8 bit or less, uint16_t for 16 bits or less etc.)
      constexpr explicit NamedBitmask(detail::SizeTypeToType_t<NamedBits>... args) :
         _data(static_cast<decltype(_data)>(detail::MaskAndOffset<Numbers, 0>::Get(args...)))
      {
      }

      NamedBitmask(const NamedBitmask& other)
      {
         _data = other._data;
      }

      NamedBitmask& operator=(const NamedBitmask& other)
      {
         _data = other._data;
         return *this;
      }

      template<
         typename Section,
         typename ValueSize_t = detail::SizeToType_t<
            meta::At_t<
               meta::IndexOf<Section, NamedSections>::value, Numbers
            >::value
         >
      >
      void Set(ValueSize_t value)
      {
         static_assert(meta::Contains<Section, NamedSections>::value, "Section not found in this Bitmask!");

         //First get the index of the section from the NamedSections typelist
         constexpr static size_t SectionIndex = meta::IndexOf<Section, NamedSections>::value;
         //Get the previous sections of this bitmask, these are the bits that we have to skip
         using Previous_t = meta::Take_t<SectionIndex, Numbers>;
         //By summing the elements of the previous section, we get the total bit count to skip
         constexpr static size_t OffsetBits = meta::Sum<Previous_t>::value;
         //We also need the size of the current section
         constexpr static size_t SectionBits = meta::At_t<SectionIndex, Numbers>::value;
         //Now we can create a mask of the size of the current section
         constexpr static size_t Mask = detail::Mask<SectionBits>::value;

         //First clear the data because we don't want any bits to remain in the section
         _data &= ~(Mask << OffsetBits);
         //Then set the new bits
         _data |= (value & Mask) << OffsetBits;
      }

      //! \brief Get the value inside this bitmask at the given section
      //! \returns Value of the section at Index
      //! \tparam Section The section to get the value from 
      template<typename Section>
      decltype(auto) Get() const
      {
         static_assert(meta::Contains<Section, NamedSections>::value, "Section not found in this Bitmask!");

         //First get the index of the section from the NamedSections typelist
         constexpr static size_t SectionIndex = meta::IndexOf<Section, NamedSections>::value;
         //Get the previous sections of this bitmask, these are the bits that we have to skip
         using Previous_t = meta::Take_t<SectionIndex, Numbers>;
         //By summing the elements of the previous section, we get the total bit count to skip
         constexpr static size_t OffsetBits = meta::Sum<Previous_t>::value;
         //We also need the size of the current section
         constexpr static size_t SectionBits = meta::At_t<SectionIndex, Numbers>::value;
         //Now we can create a mask of the size of the current section
         constexpr static size_t Mask = detail::Mask<SectionBits>::value;
         //And, neat sideeffect, we can determine the correct return type from the size of the current section
         using Return_t = detail::SizeToType_t<SectionBits>;

         return static_cast<Return_t>((_data >> OffsetBits) & Mask);
      }

   private:
      constexpr static size_t RequiredBytes = (RequiredSize + 7) / 8;
      static_assert(RequiredBytes <= sizeof(uint64_t), "Maximum bitmask size exceeded! Largest supported type is uint64_t!");

      using Data_t = detail::SizeToType_t<RequiredSize>;
      Data_t _data;
   };

   template<>
   class NamedBitmask<> {};

}