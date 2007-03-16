// -*- mode: c++; -*-
/*
  random_item.h - generating individual password elements
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
#ifndef RNDITEM_H__
#define RNDITEM_H__

#include <string.h>
#include <math.h>
#include <vector>
#include <stdexcept>
#include <botan/botan.h>

/** Base class for random item generators. */
struct simple_item_base {
	/** Returns the entropy of the item. */
	template<size_t size>
	static float get_item(
		const char *(&list)[size], size_t rnd,
		Botan::SecureVector<Botan::byte> &out,
		bool append = false);
	
	/**
	   Given random number (rnd), set the out to the random item and return
	   the amount of added entropy for that item.
	*/
	virtual float get(size_t rnd, Botan::SecureVector<Botan::byte> &out) = 0;
	virtual ~simple_item_base() { }
};

template<size_t size>
float simple_item_base::get_item(
	const char *(&list)[size], size_t rnd,
	Botan::SecureVector<Botan::byte> &out,
	bool append)
{
	const char *word = list[rnd % size];
	size_t len = strlen(word);
	
	if(!append) out.set((Botan::byte*)word, len);
	else        out.append((Botan::byte*)word, len);
	
	return log2f(size);
}

/**
   Simple random item generator just chooses an item from a fixed set.  The
   class is templatized by item list.
*/
template<typename ItemList>
struct simple_item : public simple_item_base {
	float get(size_t rnd, Botan::SecureVector<Botan::byte> &out) {
		return get_item(ItemList::list, rnd, out);
	}
};

/* Templates arguments for simple_random_item generator. */

struct diceware_list {			//!< Diceware word list
	static const char *list[8192];
	static const float entropy_per_item;
};

struct skey_list {				//!< SKey word list
	static const char *list[2048];
	static const float entropy_per_item;
};

struct uppercase_list {			//!< Uppercase letters
	static const char *list[26];
	static const float entropy_per_item;
};

struct lowercase_list {			//!< Lowercae letters
	static const char *list[26];
	static const float entropy_per_item;
};

struct digits_list {			//!< Digits 0-9
	static const char *list[10];
	static const float entropy_per_item;
};

struct special_list {			//!< Special chars
	static const char *list[32];
	static const float entropy_per_item;
};

struct koremutake_list {		//!< Koremutake syllables
	static const char *list[128];
	static const float entropy_per_item;
};

/** Composes a syllable from two sets. */
struct syllable_list : simple_item_base {
	static const char *list_12[36], *list_3[6];
	static const float entropy_per_item;

	float get(size_t rnd, Botan::SecureVector<Botan::byte> &out) {
		return get_item(list_12, rnd % 36, out) +
			get_item(list_3, rnd / 36, out, true);
	}
};

/**
   Return a mangled word from the dictionary.  Mangling is done by replacing
   a random letter in the word with a random symbol from the mangle set.
*/
template<typename WordList, typename MangleSet>
struct mangled_word : simple_item<WordList> {
	virtual float get(size_t rnd, Botan::SecureVector<Botan::byte> &out);
};

template<typename WordList, typename MangleSet>
float mangled_word<WordList, MangleSet>::get(
	size_t rnd, Botan::SecureVector<Botan::byte> &out)
{
	simple_item<WordList> gen_w;
	simple_item<MangleSet> gen_m;
	Botan::SecureVector<Botan::byte> m;
	
	float entropy = gen_w.get(rnd, out);
	if(out.size() > 256)
		throw std::logic_error("too long word for mangled_word");
	rnd = Botan::Global_RNG::random() % out.size();
	entropy += log2f(out.size()); // both the position and the actual value
	entropy += gen_m.get(rnd, m); // of symbol add to total entropy
	if(m.size() != 1)
		throw std::logic_error("wrong mangle set used in mangled_word");
	out[rnd] = m[0];
	return entropy;
}

/**
   Combination of different simple item generators.  XXX: Entropy calculation
   when there is more than one generator is wrong for the moment! [This
   currently applies only to the ASCII method.]
*/
class random_item {
private:
	std::vector<simple_item_base*> items_;

public:
	random_item() { }
	random_item(simple_item_base *g) : items_(1, g) { }
	~random_item();
	
	void add_generator(simple_item_base *g);
	float get(size_t rnd, Botan::SecureVector<Botan::byte> &out);
};

#endif	// RNDITEM_H__
