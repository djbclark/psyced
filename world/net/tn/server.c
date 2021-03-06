// $Id: server.c,v 1.17 2007/08/27 16:54:14 lynx Exp $ // vim:syntax=lpc

#include <net.h>
#include <person.h>
#include <server.h>

#define emit binary_message

qScheme() { return "tn"; }

human(x) {
	x = lower_case(x);
	if (!strlen(x) || !(abbrev(x, "yes") || abbrev(x, "ja") || abbrev(x, "si") || abbrev(x, "zes"))) {
		emit("\nSorry, no other species welcome currently.\n\n");
	}
		// now check limits
	else if (::logon()) {
		// takes a little tweaking to use T() here
		emit("Name: ");
	}
}

logon() {
	if (nick) return ::logon(); // authlocal
	cat(TELNET_PATH "clear.vt");
	cat("/local/banner.txt");
	input_to(#'human, INPUT_IGNORE_BANG);
	// takes a little tweaking to use T() here
	emit("Are you human? ");
}

password(a) {
	// send IAC + WONT + ECHO to go to echo-mode again
#ifdef OLD_HACKS_WE_PROBABLY_NO_LONGER_NEED
	binary_message(({255, 252, 1}));
#endif
	emit("\n\n");
	return ::password(a);
}

promptForPassword() {
	string me;
	me = user->vQuery("me");

	if (me) emit("\n"+ nick +" "+ me +".\n");
	// takes a little tweaking to use T() here
	emit("Password: ");
#ifdef OLD_HACKS_WE_PROBABLY_NO_LONGER_NEED
	// send IAC + WILL + ECHO to go to noecho-mode
	binary_message(({255, 251, 1}));
#endif
	// this time we do use the telnet echo off feature
	// sowohl in alt und in neu
	input_to(#'password, INPUT_IGNORE_BANG + INPUT_NOECHO);
	return 1;
}

keepUserObject(user) {
	// D(S("tn: %O, %O\n", user, abbrev(TELNET_PATH "user", o2s(user))));
	return abbrev(TELNET_PATH "user", o2s(user));
	// return user->v("scheme") == "tn";
}

createUser(nick) {
	return named_clone(TELNET_PATH "user", nick);
}

hello(ni) {
	unless (ni && ni != "") {
		// MUD tradition expects you to close on empty username
		QUIT
		return 1;
	}
// we'll filter out proxy scanners (usually coming from IRCNETs to which we
// sent our IRCGATE).
//
// only the PROMUVE will need this, as the freemuve isn't accepting "." and
// ":" in nicknames anyways (PROMUVE converts those to "_").
#if 0 //def PRO_PATH
# if 0
	{ int i1, i2, i3, i4, i5;
	    if (strstr(ni, "://") > 0 ||
	       	sscanf(ni, "%d.%d.%d.%d:%d", i1, i2, i3, i4, i5) == 5) {
		    emit("C'mon, you're a proxyscanner, aren't you?\n");
		    QUIT
	    }
	}
	// doesn't catch CONNECTirc·bas·ch·eu or POSThttp___scanner·e TODO
	if (strstr(ni, " /") > 3) {
		emit("Dumbhead.\n<h1>Don't you have a telnet client?</h1>\n");
		QUIT
		return 1;
	}
# else
	// we could simply disallow space in the name here...
	if (abbrev("GET ", ni) || abbrev("POST ", ni) || abbrev("OPTIONS ", ni)
			       || abbrev("CONNECT ", ni)) {
		// hehe.. we could htredirect proxyscanners to our www port   ;)
		emit("Dumbhead.\n<h1>Don't you have a telnet client?</h1>\n");
		QUIT
		return 1;
	}
# endif
#endif
	return ::hello(ni);
}

#if 0 //def BRAIN
morph() {
       if (user->isNewbie())
	   emit("\nSorry, no unregistered users currently.\n\n");
       else return ::morph();
}
#endif

