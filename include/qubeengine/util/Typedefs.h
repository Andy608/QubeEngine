#ifndef QUBEENGINE_UTIL_TYPEDEFS_H_
#define QUBEENGINE_UTIL_TYPEDEFS_H_

#include <cstdint>

namespace qe::util
{
	//Signed integer type with width of exactly 8, 16, 32, or 64 bits.
	//No padding bits and uses 2's compliment for negative values
	//*Only if the implementation supports the type.
	typedef int8_t  int8;
	typedef int16_t int16;
	typedef int32_t int32;
	typedef int64_t int64;

	//Unsigned integer type with width of exactly 8, 16, 32, or 64 bits.
	//No padding bits and uses 2's compliment for negative values
	//*Only if the implementation supports the type.
	typedef uint8_t  uint8;
	typedef uint16_t uint16;
	typedef uint32_t uint32;
	typedef uint64_t uint64;

	//Signed fast integer type with width of at least 8, 16, 32, or 64 bits.
	typedef int_fast8_t   int8_fast;
	typedef int_fast16_t  int16_fast;
	typedef int_fast32_t  int32_fast;
	typedef int_fast64_t  int64_fast;

	//Unsigned fast integer type with width of at least 8, 16, 32, or 64 bits.
	typedef uint_fast8_t   uint8_fast;
	typedef uint_fast16_t  uint16_fast;
	typedef uint_fast32_t  uint32_fast;
	typedef uint_fast64_t  uint64_fast;
}

using namespace qe::util;

#endif