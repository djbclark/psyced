// This is a place that keeps a log of what's been said.
// it's too simplistic tho - it doesnt handle /me correctly

#include <net.h>
#include <text.h>

#define CREATE localize();
#define ON_CONVERSE log_file(MYNICK, "%s\t%s", isotime(ctime(), 1), \
			psyctext(T(mc, "[_data]"), vars, data, source));

#include <place.gen>
