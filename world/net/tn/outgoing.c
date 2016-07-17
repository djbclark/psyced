// vim:syntax=lpc

#include <net.h>
//#include <status.h>
#include <services.h>
#include <text.h>
//#include <uniform.h>

// message queueing and automatic reconnect mgmt
inherit NET_PATH "circuit";

//inherit NET_PATH "place/public";

#ifdef _flag_log_sockets_telnet_outgoing
emit(all, source) {
	log_file("RAW_TN_OUT", "%d %O\t-> %s", time(), ME, all);
	return ::emit(all);
}
#endif

parse(all) {
	next_input_to(#'parse);
#ifdef _flag_log_sockets_telnet_outgoing
	log_file("RAW_TN_OUT", "%d %O\t<- %s", time(), ME, all);
#endif
	// assumes multicaster.. fix later
	castmsg(ME, "_notice_telnet_verbatim", "[_text_verbatim]",
	    ([ "_text_verbatim": all ]));
}

link(ho, po) {
	sTextPath(0, 0, "tn");
	return ::circuit(ho, po || TELNET_SERVICE);
}

logon(failure) {
	int rc = ::logon(failure);
	unless (rc) return 0;
	next_input_to(#'parse);
	parse("[connected]");	// hack to maintain logon ascii graphics intact
	call_out(#'runQ, 3);	// deliver the queue of messages in circuit.c
	return rc;
}

disconnect(remainder) {
	// we typically just want to give it one more try
	reconnect();
	// the reconnect in circuit.c will do certain amount of retries only
        return 0;   // unexpected
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
	if (abbrev("_message_public", mc))
	    return emit(data + "\n");
#endif
}

#if 0

render(string mc, string data, mapping vars, mixed source) {
        string template, output;
        mixed t;

        P3(("tn/out:render %O %O\n", ME, data));
        template = T(mc, 0); // enable textdb inheritance
	output = psyctext( template, vars, data, source);
        if (!output || output=="") return D2(D("tn/out: empty output\n"));
	if (template == "") output += "\n";
	emit(output);
}

cmd(a, args, b, source, vars) {
	string t;

	// extra room commands for operators of the gateway
	if (b > 0) switch(a) {
	case "disc":
		return disc();
	}
	return ::cmd(a, args, b, source, vars);
}

showStatus(verbosity, al, person, mc, data, vars) {
	if (names && namesfrom) sendmsg(person, "_status_place_members_IRC",
	     "irc://[_server_IRC]/[_nick_place_IRC] contains: [_members_IRC]",
		    ([  "_members_IRC": names,
		       //"_source_IRC": "irc://"+namesfrom+"/"+namesto
		         "_server_IRC": namesfrom,
		     "_nick_place_IRC": namesto ]));
	return ::showStatus(verbosity, al, person, mc, data, vars);
}

reboot(reason, restart, pass) {
	emit("QUIT :Service "+ (restart ? "restart" : "shutdown")
	     +": "+ reason +"\n");
	return ::reboot(reason, restart, pass);
}

static disc() {
	emit("QUIT :Service temporarily disabled\n");
	return 1;
}

onEnter(source, mc, data, vars) {
	if (interactive(ME)) tellChannel(source, mc, vars, data);
	return ::onEnter(source, mc, data, vars);
}
leave(source, mc, data, vars) {
	if (interactive(ME)) tellChannel(source, mc, vars, data);
	return ::leave(source, mc, data, vars);
}

#endif
