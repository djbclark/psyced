#include <net.h>

//#ifdef BRAIN
# define	CONNECT_TELNET	"nemesis.de", 2000
//#else
//# define	CONNECT_TELNET	"nemesis.de", 2001  // NemTest
//#endif

#define	ON_CONNECT	emit("guest\n");
#define	ON_ANY		NET_PATH "tn/outgoing"::msg(source, mc, data, vars);

#include <place.gen>

// to do this properly, we need a systematic way to redo prompts FIXME
parse(all) {
	// remove Nemesis' guest prompt.. why was i so funny
	// back then to implement several prompt chars?
	if (strlen(all) > 2 && all[1] == ' ' &&
	    (all[0] == '#' || all[0] == '>')) all = all[2 ..];
	return ::parse(all);
}

#echo Nemesis gateway loaded.

