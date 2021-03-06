#include <net.h>
#include <person.h>
#include <status.h>

#define BLAME "!configuration"
#define DONT_REWRITE_NICKS
#define HISTORY_GLIMPSE 12

#include <uniform.h>

inherit NET_PATH "place/threads";

volatile mixed lastTry;

volatile string owner;
volatile string channel;

#ifdef TWITTER
volatile object twitter;
#endif
#ifdef IDENTICA
volatile object identica;
#endif

load(name, keep) {
    P3((">> userthreads:load(%O, %O)\n", name, keep))
    int ret = ::load(name, keep);

    sscanf(name, "~%s#%s", owner, channel);

    unless (v("owners")) vSet("owners", ([ lower_case(owner) ]));
    unless (v("privacy")) vSet("privacy", "private");
    unless (vExist("twitter")) vSet("twitter", 0);
    unless (vExist("identica")) vSet("identica", 0);

    vSet("_restrict_invitation", BLAME);
    vSet("_filter_conversation", BLAME);

    return ret;
}

enter(source, mc, data, vars) {
    P3((">> userthreads:enter(%O, %O, %O, %O)\n", source, mc, data, vars))
    object p = summon_person(owner, NET_PATH "user");
    string src = objectp(source) ? psyc_name(source) : source;

    unless (v("privacy") == "public" ||
	    (p && (p == source || qAide(src) || (objectp(source) && qAide(source->qNameLower()))
		   || p->qFriend(source) || p->qFollower(source)))) {
	sendmsg(source, "_error_place_enter_necessary_invitation",
		"[_nick_place] can only be entered upon invitation.",
		([ "_nick_place" : qName() ]) );
	if (source != lastTry) {
	    castmsg(ME, "_failure_place_enter_necessary_invitation",
		    "Admission into [_nick_place] denied for uninvited user [_nick].",
		    vars);
	    lastTry = source;
	}
	return 0;
    }

    if (p == source) {
	p->sChannel(MYNICK);
#ifdef TWITTER
	if (v("twitter") && !twitter) twitter = clone_object(NET_PATH "twitter/client")->load(source);
#endif
#ifdef IDENTICA
	if (v("identica") && !identica) identica = clone_object(NET_PATH "identica/client")->load(source);
#endif
    }

    return ::enter(source, mc, data, vars);
}

_request_add(source, mc, data, vars, b) {
    P3((">> userthreads:_request_add(%O, %O, %O, %O, %O)\n", source, mc, data, vars, b))
    unless (vars["_person"]) {
	sendmsg(source, "_warning_usage_add", "Usage: /add <person>", ([ ]));
	return 1;
    }

    object p = summon_person(owner, NET_PATH "user");
    object target = summon_person(vars["_person"], NET_PATH "user");
    string ni;
    if (objectp(target)) {
	ni = target->qName();
    } else {
	target = vars["_person"];
	mixed *u = parse_uniform(target);
	if (u) ni = u[UNick];
    }

    unless (ni && p && (p->qFriend(target) || p->qFollower(target))) {
	sendmsg(source, "_error_request_add", // FIXMC
		"Error: [_person] is not a friend or follower.", ([ "_person": vars["_person"]]));
	return 1;
    }

    string _mc = "_notice_place_enter_automatic_subscription_follow";
    sendmsg(target, _mc, "You're now following [_nick_place].", (["_nick_place" : qName()]), source);
    //insert_member(target, mc, 0, (["_nick": ni]), ni);

    return 1;
}

_request_remove(source, mc, data, vars, b) {
    P3((">> userthreads:_request_remove(%O, %O, %O, %O, %O)\n", source, mc, data, vars, b))
    unless (vars["_person"]) {
	sendmsg(source, "_warning_usage_remove", "Usage: /remove <person>", ([ ]));
	return 1;
    }

    object p = summon_person(owner, NET_PATH "user");
    object target = summon_person(vars["_person"], NET_PATH "user") || vars["_person"];

    unless (p && (qMember(target) || p->qFriend(target) || p->qFollower(target))) {
	sendmsg(source, "_error_request_remove", // FIXMC
		"Can't remove: [_person] is not a friend or follower.", ([ "_person": vars["_person"]]));
	return 1;
    }

    string _mc = "_notice_place_leave_automatic_subscription_follow";
    sendmsg(target, _mc, "You're no longer following [_nick_place].", (["_nick_place" : qName()]), source);
    remove_member(target);

    return 1;
}

// set privacy: private or public
//  - private: only friends & invited people can enter (default)
//  - public: anyone can enter
_request_set_privacy(source, mc, data, vars, b) {
    return _request_set_default_list(source, mc, data, vars, b, ({"public", "private"}));
}

#ifdef TWITTER
_request_set_twitter(source, mc, data, vars, b) {
    unless (qOwner(SNICKER)) return 0;
    string value = vars["_value"];
    if (is_true(value)) {
	unless (twitter) twitter = clone_object(NET_PATH "twitter/client")->load(source);
	vSet("twitter", 1);
	save();
    } else if (is_false(value)) {
	if (twitter) twitter = 0;
	vSet("twitter", 0);
	save();
    }

    sendmsg(source, "_info_set_place_twitter", "Twitter submission is [_status].", (["_status": v("twitter") ? "enabled" : "disabled"]));
    return 1;
}
#endif

#ifdef IDENTICA
_request_set_identica(source, mc, data, vars, b) {
    unless (qOwner(SNICKER)) return 0;
    string value = vars["_value"];
    if (is_true(value)) {
	unless (identica) identica = clone_object(NET_PATH "identica/client")->load(source);
	vSet("identica", 1);
	save();
    } else if (is_false(value)) {
	if (identica) identica = 0;
	vSet("identica", 0);
	save();
    }

    sendmsg(source, "_info_set_place_identica", "Identi.ca submission is [_status].", (["_status": v("identica") ? "enabled" : "disabled"]));
    return 1;
}
#endif

varargs int addEntry(mixed source, mapping vars, string _data, string _mc) {
    int ret;
    if (ret = ::addEntry(source, vars, _data, _mc)) {
#ifdef TWITTER
	if (v("twitter") && twitter) twitter->status_update(vars["_text"]);
#endif
#ifdef IDENTICA
	if (v("identica") && identica) identica->status_update(vars["_text"]);
#endif
    }
    return ret;
}

htMain(int limit, int offset, int submit) {
    return ::htMain(limit, offset, submit, channel);
}

canPost(snicker) {
    return qOwner(snicker);
}

isPublic() {
    return vQuery("privacy") == "public";
}

showWebLog() {
    return 0;
}

qChannel() {
    return channel;
}

int qHistoryGlimpse() {
    return HISTORY_GLIMPSE;
}

psycName() {
    return MYLOWERNICK;
}

pathName() {
    return regreplace(MYNICK, "#", "/", 1);
}

#ifdef _flag_save_userthreads_immediately
qSaveImmediately() {
    return 1;
}
#endif
