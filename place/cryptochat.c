// protocol or are coming from the localhost (probably SSH users).
//
// both cases are no absolute guarantee for safety.. it is still
// in the hands of each user in the room to safeguard true secrecy
//
// -lynX 2004

#include <net.h>

#define NAME	"CryptoChat"
#define SECURE

// should work like this, but there's a bug to fix here...
// when trying to call https://psyced.org:33333/@cryptochat
//
//efine PLACE_HISTORY
#define PLACE_SCRATCHPAD
//efine	PLACE_OWNED		"ioerror"

#include <place.gen>

