#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <botan/botan.h>
#include <botan/rng.h>
#include "filters.h"

using std::cout;
using std::cerr;
using std::endl;

/** Reasonable limit. */
#define MAXN 1024

/**
   Generate an array of 8 random numbers and encode it with filter selected
   according to command-line switches.  This means that the encoder may use
   only 1 item of input per one item of output.  If additional randomness is
   desired, it must generate it on its own.
*/
template<class Filter>
static void make_password(const std::string &flags, size_t n)
{
	using Botan::SecureVector;
	using Botan::byte;
	
	SecureVector<byte> randomness(n);
	float entropy;

	Botan::Global_RNG::randomize(randomness, randomness.size());
	Botan::Pipe random_encoder(new Filter(flags, entropy));
	random_encoder.process_msg(randomness);
	SecureVector<byte> password = random_encoder.read_all();
	cout << "PASSPHRASE FOLLOWS; ENTROPY=" << entropy << endl;
	cout.write((char*)(byte*)password, password.size());
	cout << endl;
}

static void usage(const char *argv0)
{
	cerr << "USAGE: " << argv0 << " METHOD[:FLAGS] N\n"
		 << "Generate a password according to given parameters\n"
		 << endl
		 << "METHOD is one of 'diceware', 'skey', 'ascii', 'koremutake' or 'base64'\n"
		 << "FLAGS is an optional string of characters to modify the basic method\n"
		 << "N is password length (exact meaning depends on method)\n"
		 << endl
		 << "METHOD: diceware: N words from the diceware dictionary\n"
		 << "METHOD: skey    : N words from the s/key [RFC2289] dictionary\n"
		 << "FLAGS : e       : enhanced passphrase\n"
		 << endl
		 << "METHOD: ascii   : N elements from an alphabet determined by FLAGS\n"
		 << "FLAGS : (none)  : only alphanumeric characters\n"
		 << "        s       : includes special characters\n"
		 << "        y       : includes syllables\n"
		 << "        c       : includes variation in case\n"
		 << endl
		 << "METHOD: koremutake: N random koremutake syllables\n"
		 << "METHOD: base64    : N random bytes, base64 encoded\n"
		 << endl;
	exit(1);
}

int main(int argc, char **argv) try {
	Botan::LibraryInitializer _botan("secure_memory");

	if(argc != 3)
		usage(argv[0]);

	/* Extract and check N. */
	size_t n = atoi(argv[2]);
	if(n > MAXN) {
		cerr << "LIMIT ERROR: N>" << MAXN << "; recompile the program.\n";
		exit(1);
	}
	if(!n) {
		cerr << "LIMIT ERROR: N==0\n";
		exit(1);
	}

	/* Extract method and flags. */
	std::string method(argv[1]);
	std::string flags(argv[1]);
	size_t flags_start = method.find(':');
	if(flags_start < flags.size()) {
		method.erase(flags_start);
		flags.erase(0, flags_start+1);
	} else {
		flags.clear();
	}

	/* Generate the password. */
	if(method == "diceware")
		make_password<Dictionary_Encoder<diceware_list> >(flags, n);
	else if(method == "skey")
		make_password<Dictionary_Encoder<skey_list> >(flags, n);
	else if(method == "ascii")
		make_password<ASCII_Encoder>(flags, n);
	else if(method == "koremutake")
		make_password<Koremutake_Encoder>(flags, n);
	else if(method == "base64")
		make_password<B64_Encoder>(flags, n);
	else
		usage(argv[0]);
	
	return 0;
} catch(std::exception &e) {
	cerr << "ERROR: " << e.what() << endl;
	exit(2);
}
