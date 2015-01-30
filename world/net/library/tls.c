#include <net.h> // vim syntax=lpc
#include <proto.h>
#include <sys/tls.h>

mapping tls_certificate(object who, int longnames) {
    mixed *extra, extensions;
    mapping cert;
    int i, j;

    cert = ([ ]);
#if __EFUN_DEFINED__(tls_check_certificate)
# ifdef WANT_S2S_SASL
#  ifdef __psyclpc__
#   if __EFUN_DEFINED__(enable_binary) // happens to be committed at the same time
    extra = tls_check_certificate(who, 2);
#   else
    extra = tls_check_certificate(who, 1);
#   endif
#  else
    extra = tls_check_certificate(who);
#  endif
    unless (extra && sizeof(extra) > 2) return 0;
    cert[0] = extra[0];
    if (sizeof(extra) >= 4)
	cert[1] = extra[3];

    extensions = extra[2];
    extra = extra[1];

    for (i = 0; i < sizeof(extra); i += 3) {
	mixed t;

	t = cert[extra[i]];
	// THIS IS ALWAYS TRUE FOR DRIVER >= 3.3.712 
	// OTHERWISE YOU SHOULD NOT ENABLE S2S SASL. PERIOD.
	if (sizeof(extra) > i+2) {
	    unless (t) {
		cert[extra[i]] = extra[i+2];
	    } else if (stringp(t)) {
		cert[extra[i]] = ({ t, extra[i+2] });
	    } else if (pointerp(t)) {
		cert[extra[i]] += ({ extra[i+2] });
	    } else { 
		PT(("fippo says this should not happen but you know it always does! %O\n", ME))
	    }
	} else {
	    PT(("fippo says this should not happen(2) but you know it always does! %O\n", ME))
	}
    }
    if (longnames) {
	// set up short/long names
	for (i = 0; i < sizeof(extra); i +=3) { 
	    cert[extra[i+1]] = cert[extra[i]];
	}
    }
    for (i = 0; i < sizeof(extensions); i += 3) {
	string key, mkey;
	mixed *val;

	unless(extensions[i]) continue;
	key = extensions[i];
	val = extensions[i+2];
	for (j = 0; j < sizeof(val); j += 3) {
	    mixed t;

	    mkey = key + ":" + val[j];
	    t = cert[mkey];
	    unless (t) {
		cert[mkey] = val[j+2];
	    } else if (stringp(t)) {
		cert[mkey] = ({ t, val[j+2] });
	    } else if (pointerp(t)) {
		cert[mkey] += ({ val[j+2] });
	    } else {
		// should not happen
	    }
	}
    }
# else
#  echo No WANT_S2S_SASL ? You even have the right driver for it!
# endif
#else
# echo No tls_check_certificate available with this driver.
#endif
    P2(("cert is %O\n", cert))
    return cert;
}

// generalized variant of the old certificate_check_jabbername
// RFC 6125 describes the process in more detail
int tls_check_service_identity(string name, mixed cert, string scheme) {
    mixed t;
    string idn;
    // FIXME: should probably be more careful about internationalized
    // domain names - need testcases
#define WILDCARD_MATCH(thing) (strlen(thing) > 2 && thing[0] == '*' && thing[1] == '.' && trail(thing[2..], name))
    /* this does not support wildcards if there is more than one
     * id-on-xmppAddr/CN
     * API Note: name MUST be an utf8 string
     */
    unless(name && cert && mappingp(cert)) return 0;

    name = NAMEPREP(name);

    // subjectAlternativeName - dNSName
    if ((t = cert["2.5.29.17:dNSName"])) { 
	if (stringp(t)) t = ({ t });
	foreach(string t2 : t) {
	    t2 = NAMEPREP(t2);
	    if (name == t2 || WILDCARD_MATCH(t2))
		return 1;
	}
    } 

    // subjectAlternativeName - SRV ID - FIXME
    // unfortunately, the only ones I have encountered so far were ... unusable
    // what they should like is "_psyc.name" - i.e. "_" + scheme + "." + name
    // no wildcards probably
    if ((t = cert["2.5.29.17:1.3.6.1.5.5.7.8.7"])) {
	    P2(("encountered SRVName, please tell fippo: %O\n", t))
    }

    // URI ID  - FIXME
    // not seen yet
    
#if 0
    // id-on-xmppAddr - have not seen them issued by anyone but 
    // startcom and those usually include dnsname, too
    // utf8-encoded
    if ((t = cert["2.5.29.17:1.3.6.1.5.5.7.8.5"])) { 
	if (pointerp(t)) {
	    if (member(t, name) != -1) return 1;
	    foreach(string cn : t) {
		if (NAMEPREP(cn) == name) return 1;
	    }
	} 
	else if (name == NAMEPREP(t))
	    return 1;
    } 
#endif

    // commonName - deprecated to put the host here but...
    // this is only to be checked if no subjectAlternativeName is present
    if (!cert["2.5.29.17"] && (t = cert["2.5.4.3"])) { // common name
	if (pointerp(t)) { // does that happen?! I don't think so...
	    // fast way - works for traditional hostnames
	    if (member(t, name) != -1) return 1;

	    // look for idn encoded stuff
	    foreach(string cn : t) {
#ifdef __IDNA__
		idn = NAMEPREP(idna_to_unicode(cn));
#else
		idn = NAMEPREP(cn);
#endif
		if (idn == name) return 1;
	    }
	    return 0;
	} 
#ifdef __IDNA__
	idn = NAMEPREP(idna_to_unicode(t));
#else
	idn = NAMEPREP(t);
#endif
	if (idn == name || WILDCARD_MATCH(idn))
	    return 1;
    }
    return 0;
}

string tls_bad_cipher(object sock, string scheme) {
    // ignore if pidgin uses a bad cipher over an ssh tunnel
    if (scheme == "jabber" && is_localhost(query_ip_number(sock))) return 0;

    mixed t = tls_query_connection_info(sock);
    unless (t) return "NO-CIPHER";  // shouldnt happen
    t = t[TLS_CIPHER];
    P3(("%O is using the %O cipher.\n", sock, t))

    // shouldn't our negotiation have ensured we have PFS?
    if (stringp(t) &&! (abbrev("DHE", t) || abbrev("ECDHE", t))) {
	monitor_report("_warning_circuit_encryption_cipher_details",
	    object_name(sock) +" · using "+ t +" cipher");
	return t;
    }
    return 0;
}

// Do not use server technologies like psyced for strong
// privacy or anonymity requirements. Get started using
// distributed technologies instead. Check out:
//	http://youbroketheinternet.org
//	http://secushare.org
// But if you're happy with half-baked security, here you
// have it:
//					-lynX 2015
//
int probably_private(object source) {
	// object has no TCP to it. you have to ask its circuit.
	unless (objectp(source) && interactive(source))
	    return PRIVACY_UNKNOWN;
	// should use trustworthy level 9 instead? if so.. how?
	if (query_ip_number(source) == "127.0.0.1"
		// query_ip_number() efun is faster than a local 
		// LPC variable, so it is fine we call it often
             || query_ip_number(source) == __HOST_IP_NUMBER__
#  ifdef SECURE_IP_NUMBER
             || SECURE_IP_NUMBER(query_ip_number(source))
#  endif
	// People coming from localhost have either made it
	// through SSH's reasonable certificate pinning or
	// Tor's public-key based addressing.. both provide
	// reasonable protection from men in the middle, if
	// only the server itself could be considered secure.
	//
	) return PRIVACY_REASONABLE;
#  if __EFUN_DEFINED__(tls_query_connection_state)
	// Alas, this person is using a TLS/SSL-enhanced
	// access protocol which, unless the client implements
	// certificate pinning, is susceptible to man in the
	// middle attacks. Find out more on this topic on
	// http://patrol.psyced.org
	//
        if (tls_query_connection_state(source))
	    return PRIVACY_MITMX509;
#  endif
	return PRIVACY_SURVEILLED;
}

