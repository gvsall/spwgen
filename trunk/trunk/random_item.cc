// -*- mode: c++; -*-
/*
  random_item.cc - generating individual password elements
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
#include <string.h>
#include <math.h>
#include <stdexcept>
#include "random_item.h"

/*****************************************************************************/
/* SMALL TABLES                                                              */
/*****************************************************************************/

const float diceware_list::entropy_per_item  = 13;	 // log2(8192)
const float skey_list::entropy_per_item      = 11;	 // log2(2048)
const float uppercase_list::entropy_per_item = log2f(26);
const float lowercase_list::entropy_per_item = log2f(26);
const float digits_list::entropy_per_item    = log2f(10);
const float special_list::entropy_per_item   = 5; // log2(32)
const float syllable_list::entropy_per_item = log2f(216);
const float koremutake_list::entropy_per_item = 7; // log2(128)

const char *uppercase_list::list[26] = {
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
	"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"
};

const char *lowercase_list::list[26] = {
	"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
	"n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"
};

const char *digits_list::list[10] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
};

const char *special_list::list[32] = {
	"!", "@", "#", "$", "%", "^", "&", "*", "(", ")",  "-", "_", 
	"+", "=", "[", "]", "{", "}", ";", ":", "'", "\"", ",", ".",
	"<", ">", "/", "?", "`", "~", "|", "\\"
};

const char *syllable_list::list_12[36] = {
	"B", "C", "D", "F", "G", "H",
	"J", "K", "L", "M", "N", "P",
	"QU", "R", "S", "T", "V", "W",
	"X", "Z", "CH", "CR", "FR", "ND",
	"NG", "NK", "NT", "PH", "PR", "RD",
	"SH", "SL", "SP", "ST", "TH", "TR"
};

const char *syllable_list::list_3[6] = {
	"A", "E", "I", "O", "U", "Y"
};

const char *koremutake_list::list[128] = {
	"BA", "BE", "BI", "BO", "BU", "BY", "DA", "DE",
	"DI", "DO", "DU", "DY", "FA", "FE", "FI", "FO",
	"FU", "FY", "GA", "GE", "GI", "GO", "GU", "GY",
	"HA", "HE", "HI", "HO", "HU", "HY", "JA", "JE",
	"JI", "JO", "JU", "JY", "KA", "KE", "KI", "KO",
	"KU", "KY", "LA", "LE", "LI", "LO", "LU", "LY",
	"MA", "ME", "MI", "MO", "MU", "MY", "NA", "NE",
	"NI", "NO", "NU", "NY", "PA", "PE", "PI", "PO",
	"PU", "PY", "RA", "RE", "RI", "RO", "RU", "RY",
	"SA", "SE", "SI", "SO", "SU", "SY", "TA", "TE",
	"TI", "TO", "TU", "TY", "VA", "VE", "VI", "VO",
   	"VU", "VY", "BRA", "BRE", "BRI", "BRO", "BRU", "BRY",
	"DRA", "DRE", "DRI", "DRO", "DRU", "DRY", "FRA", "FRE",
	"FRI", "FRO", "FRU", "FRY", "GRA", "GRE", "GRI", "GRO",
	"GRU", "GRY", "PRA", "PRE", "PRI", "PRO", "PRU", "PRY",
	"STA", "STE", "STI", "STO", "STU", "STY", "TRA", "TRE"
};

random_item::~random_item()
{
	for(unsigned i = 0; i < items_.size(); i++)
		delete items_[i];
}

void random_item::add_generator(simple_item_base *g) {
	if(items_.size() >= 256)
		throw std::logic_error("too many individual generators in random_item");
	items_.push_back(g);
}

float random_item::get(size_t rnd, Botan::SecureVector<Botan::byte> &out) {
	if(!items_.size())
		throw std::logic_error("no simple generators in random_item");
	size_t g = Botan::Global_RNG::random() % items_.size();
	return items_[g]->get(rnd, out);
}
