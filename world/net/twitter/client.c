/* twitter client
 * http://apiwiki.twitter.com/Twitter-API-Documentation
 *
 * - register @ http://twitter.com/apps
 *   - settings:
 *     - app name: e.g. psyc://your.host/
 *     - app type: browser
 *     - callback url: http://your.host/oauth
 *                     (actually the url psyced sends will be used but you have to type in something)
 *     - access type: read/write
 * - then in local.h #define TWITTER_KEY & TWITTER_SECRET
 */
#include <net.h>
#include <ht/http.h>

inherit NET_PATH "http/oauth";

object load(object usr, string key, string secret, string request, string access, string authorize) {
    consumer_key = TWITTER_KEY;
    consumer_secret = TWITTER_SECRET;
    request_token_url = "http://twitter.com/oauth/request_token";
    access_token_url = "http://twitter.com/oauth/access_token";
    authorize_url = "http://twitter.com/oauth/authorize";

    return ::load(usr, key, secret, request, access, authorize);
}

void parse_status_update(string body, string headers, int http_status) {
    P3(("twitter/client:parse_status_update(%O, %O, %O)\n", body, headers, http_status))
    if (http_status != R_OK)
	sendmsg(user, "_error_twitter_status_update", "Error: failed to post status update on twitter.");
}

void status_update(string text) {
    P3(("twitter/client:status_update()\n"))
    if (strlen(text) > 140) text = text[0..136] + "...";

    object ua = clone_object(NET_PATH "http/fetch");
    ua->content(#'parse_status_update, 1, 1); //');
    fetch(ua, "http://api.twitter.com/1/statuses/update.json", "POST", (["status": text]));
}

void parse_home_timeline(string body, string headers, int http_status) {
    P3(("twitter/client:parse_home_timeline(%O, %O, %O)\n", body, headers, http_status))
}

void home_timeline() {
    P3(("twitter/client:home_timeline()\n"))
    object ua = clone_object(NET_PATH "http/fetch");
    ua->content(#'parse_home_timeline, 1, 1); //');
    fetch(ua, "http://api.twitter.com/1/statuses/home_timeline.json");
}