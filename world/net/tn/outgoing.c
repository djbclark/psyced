// vim:syntax=lpc

// DEBUG: FIXME
#define _flag_log_sockets_telnet_outgoing

#include <net.h>
#include <services.h>
//#include <text.h>

// this outgoing "telnet" socket thing implements both '/set mudlink'
// for users as MUD chatrooms such as @nemesis. would be cool if it
// actually was capable of handling telnet protocol, but the mud in
// question isn't, either.
//
// for a tighter integration between psyced and MUD it would also be
// cool to either have a native PSYC side channel, or to embed crypto-
// graphically signed PSYC packets (using a shared secret hash) into
// this stream.

// message queueing and automatic reconnect mgmt
inherit NET_PATH "circuit";

virtual inherit NET_PATH "output";

static string connectstring;
static object owner;

send(all) {
	// if this doesn't work, then maybe you just forgot
	// to /set charset in your test user
	iconv(all, SYSTEM_CHARSET, "ASCII//TRANSLIT");	// should be configurable
#ifdef _flag_log_sockets_telnet_outgoing
	log_file("RAW_TN_OUT", "%O > %O\n", ME, all);
#endif
	//if (function_exists("emit")) return emit(all);
	return emit(all);
}

parse(all) {
	next_input_to(#'parse);
#ifdef _flag_log_sockets_telnet_outgoing
	log_file("RAW_TN_OUT", "%O < %O\n", ME, all);
#endif
	if (all && all != "") iconv(all, "ASCII//TRANSLIT", SYSTEM_CHARSET);
	if (owner) sendmsg(owner,
	     "_notice_telnet_verbatim", "[_text_verbatim]",
	    ([ "_text_verbatim": all ]));
	else if (function_exists("castmsg")) castmsg(ME,
	     "_notice_telnet_verbatim", "[_text_verbatim]",
	    ([ "_text_verbatim": all ]));
}

link(ho, po) {
	//sTextPath(0, 0, "tn");
	return ::circuit(ho, po || TELNET_SERVICE);
}

config(host, port, justdoit) {
	connectstring = replace(justdoit, " ", "\n") + "\n";
	owner = previous_object();
	link(host, port);
}

logon(failure) {
	int rc = ::logon(failure);
	unless (rc) return 0;
	next_input_to(#'parse);
	parse("[connected]");	// hack to maintain logon ascii graphics intact
				// could have queued this instead:
	if (connectstring) call_out(#'send, 1, connectstring);
	call_out(#'runQ, 3);	// deliver the queue of messages in circuit.c
	return rc;
}

msg(source, mc, data, mapping vars, showingLog, target) {
	string t, ni, mca;

	if (showingLog) return 1;
	PT(("TNOUT(%O,%O,%O,%O,%O,%O)\n", source,mc,data,vars,
					    showingLog,target))
	unless(interactive()) {
		connect();
		// special requirement for enqueue
		unless (vars["_source"])
		    vars["_source"] = UNIFORM(source);
		return enqueue(source, mc, data, vars, showingLog, target);
	}
	if (abbrev("_status_place",mc) || abbrev("_message_announcement",mc)) {
		if (source && source != ME) {
			monitor_report("_error_rejected_message_telnet",
			    S("Caught a %O from %O (%O)", mc, source, data));
			return;
		}
#if 0
		tellChannel(source, mc, vars, data);
#endif
		return 1;
	}
#if 0
	if (abbrev("_message_echo", mc)) return;
	if (!data && stringp(vars["_action"]) && abbrev("_message", mc))
	    mca = mc +"_action";
	else
	    mca = mc;
	render(mca, data, vars, source);
	return 1;
#else
	if (abbrev("_message_public", mc)) {
		// FIXME: should we handle the disconnect instead?
		if (data == "quit") return 1;
		return send(data + "\n");
	}
#endif
}

#if 0

disconnected(remaining) {
	// circuit code would destruct this object
	return NET_PATH "connect"::disconnected(remaining);
}

render(string mc, string data, mapping vars, mixed source) {
        string template, output;
        mixed t;

        P3(("tn/out:render %O %O\n", ME, data));
        template = T(mc, 0); // enable textdb inheritance
	output = psyctext( template, vars, data, source);
        if (!output || output=="") return D2(D("tn/out: empty output\n"));
	if (template == "") output += "\n";
	send(output);
}

#endif
