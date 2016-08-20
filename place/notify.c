// typical quiet pubsub place for receiving git2psyc notifications

#include <net.h>

#define NAME "notify"
#define SILENT
#define PLACE_HISTORY_EXPORT
#define HISTORY_METHOD	"_notice_update"
#define HISTORY_GLIMPSE 4

// allow notifications from anywhere.. you may want to restrict this.
// see wikinotify.c and http://about.psyc.eu/create_place about that.
#define ALLOW_EXTERNAL

#include <place.gen>

