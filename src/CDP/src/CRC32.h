/*
 * MIT License
 * 
 * Copyright (c) 2016 Takuya Sawada
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef __CRC32_H
#define __CRC32_H
#include <numeric>
#include <array>
#include <stdint.h>

template<uint32_t poly>
struct crc32_table_generator {
private:
	template<typename T, bool flag>
	struct denominator;
	
	template<typename T>
	struct denominator<T, true>
	{
		static constexpr T value = poly;
	};
	
	template<typename T>
	struct denominator<T, false>
	{
		static constexpr T value = 0;
	};

	template<uint8_t index, uint8_t N=8>
	struct crc32_table_elem {
		static constexpr bool carry =
			static_cast<bool>(crc32_table_elem<index, N-1>::value & 0x80000000);
		static constexpr uint32_t value =
			(crc32_table_elem<index, N-1>::value << 1) ^ denominator<uint32_t, carry>::value;
	};

	template<uint8_t index>
	struct crc32_table_elem<index, 0> {
		static constexpr uint32_t value = (index << 24);
	};

	template<size_t N=255, uint32_t ...Indices>
	struct array_impl {
		static constexpr auto value = array_impl<N-1, crc32_table_elem<N>::value, Indices...>::value;
	};
	
	template<uint32_t ...Indices>
	struct array_impl<0, Indices...> {
		static constexpr std::array<uint32_t, sizeof...(Indices) +1> value
			= {{crc32_table_elem<0>::value, Indices...}};
	};

public:
	static constexpr std::array<uint32_t, 256> value = array_impl<>::value;
};


static const uint32_t IEEE8023_CRC32_POLYNOMIAL = 0x04C11DB7UL;


template<uint32_t poly, typename iterator_t>
static inline uint32_t crc32(uint32_t crc, const iterator_t head, const iterator_t tail)
{
	// instantiate crc32 table (compilie-time)
	static const auto crc32_table = crc32_table_generator<poly>::value;

	// calculate crc32 checksum for each byte
	return std::accumulate(head, tail, crc, [](const uint32_t &crc, const uint8_t &x) -> uint32_t {
		return (crc << 8) ^ crc32_table[((crc >> 24) ^ x) & 0xFF];
	});
}

#endif /* __CRC32_H */