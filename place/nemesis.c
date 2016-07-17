#include <net.h>

#define	CONNECT_TELNET	"nemesis.de", 2000
#define	ON_CONNECT	emit("guest\n");
#define	ON_ANY		NET_PATH "tn/outgoing"::msg(source, mc, data, vars);

#include <place.gen>

parse(all) {
	// remove Nemesis' guest prompt.. why was i so funny
	// back then to implement several prompt chars?
	if (strlen(all) > 2 && all[1] == ' ' &&
	    (all[0] == '#' || all[0] == '>')) all = all[2 ..];
	return ::parse(all);
}

#echo Nemesis gateway loaded.

