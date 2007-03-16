// -*- mode: c++; -*-
/*
  filters.h - conversion of random numbers to various encodings
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
#ifndef FILTERS_H__
#define FILTERS_H__

#include <string>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include <botan/botan.h>
#include "random_item.h"

/** Encode as a Base64 string. */
class B64_Encoder : public Botan::Base64_Encoder {
private:
	float &entropy_;
	
public:
	B64_Encoder(const std::string &flags, float &entropy)
	: Botan::Base64_Encoder(), entropy_(entropy)
	{
		if(flags != "")
			throw std::invalid_argument("Invalid flags given to raw random.");
		entropy_ = 0;
	}
	void write(const Botan::byte b[], Botan::u32bit n) {
		Base64_Encoder::write(b, n);
		entropy_ += 8.0*n;
	}
};

class RandomItem_Encoder : public Botan::Filter {
private:
	random_item &rnd_;
	float &entropy_;
	char delim_;
	
protected:
	RandomItem_Encoder(random_item &rnd, float &entropy, char delim = 0) :
		rnd_(rnd), entropy_(entropy), delim_(delim) { }

public:
	void write(const Botan::byte b[], Botan::u32bit n);
};

class Koremutake_Encoder : public RandomItem_Encoder {
private:
	random_item rnd_;
	
public:
	Koremutake_Encoder(const std::string &flags, float &entropy);
};

class ASCII_Encoder : public RandomItem_Encoder {
private:
	random_item rnd_;

public:
	ASCII_Encoder(const std::string &flags, float &entropy);
};

template<typename WordList>
class Dictionary_Encoder : public RandomItem_Encoder {
private:
	random_item rnd_;

public:
	Dictionary_Encoder(const std::string &flags, float &entropy);
};

template<typename WordList>
Dictionary_Encoder<WordList>::Dictionary_Encoder(
	const std::string &flags, float &entropy) :
	RandomItem_Encoder(rnd_, entropy, ' ')
{
	if(flags == "")
		rnd_.add_generator(new simple_item<WordList>());
	else if(flags == "e")
		rnd_.add_generator(new mangled_word<WordList, special_list>());
	else
		throw std::runtime_error("invalid flags for method");
}

#endif	// FILTERS_H__
