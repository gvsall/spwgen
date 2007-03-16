// -*- mode: c++; -*-
/*
  filters.cc - conversion of random numbers to various encodings
  (c) 2007 Zeljko Vrba <zvrba@globalnet.hr>

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <stdexcept>
#include "filters.h"

void RandomItem_Encoder::write(const Botan::byte b[], Botan::u32bit n)
{
	Botan::SecureVector<Botan::byte> out;
	Botan::byte delim[1] = { delim_ };

	/* XXX: 16-bit random number is large enough for all current generators */
	for(unsigned i = 0; i < n; i++) {
		entropy_ += rnd_.get(b[i] * Botan::Global_RNG::random(), out);
		if(delim_) out.append(delim, 1);
		send(out);
	}
}

Koremutake_Encoder::Koremutake_Encoder(
	const std::string &flags, float &entropy) :
	RandomItem_Encoder(rnd_, entropy)
{
	if(flags != "")
		throw std::runtime_error("invalid flags for koremutake method");
	rnd_.add_generator(new simple_item<koremutake_list>());
}

static void check_flags_for_duplicates(const std::string &flags)
{
	for(int i = 0; i < (int)flags.size()-1; i++)
		if(flags.find(flags[i], i+1) != std::string::npos)
			throw std::runtime_error("duplicate flags specified");
}

ASCII_Encoder::ASCII_Encoder(
	const std::string &flags, float &entropy) :
	RandomItem_Encoder(rnd_, entropy)
{
	check_flags_for_duplicates(flags);
	rnd_.add_generator(new simple_item<uppercase_list>());
	rnd_.add_generator(new simple_item<digits_list>());
	for(unsigned i = 0; i < flags.size(); i++)
		switch(flags[i]) {
		case 's':
			rnd_.add_generator(new simple_item<special_list>());
			break;
		case 'y':
			rnd_.add_generator(new syllable_list());
			break;
		case 'c':
			rnd_.add_generator(new simple_item<lowercase_list>());
			break;
		default:
			throw std::runtime_error("invalid flags for method");
		}
}
