// vim:foldmethod=marker:syntax=lpc:noexpandtab

#include "psyc.h"
#include <net.h>
#include <input_to.h>

private string buffer;
int state;
int may_parse_more;

// being faded out in favor of regular croak()
#define PARSEERROR(reason) { \
   	croak("_error_syntax_broken", \
	      "Failed parsing: " reason); \
	return 0; }

#define DELIM S_GLYPH_PACKET_DELIMITER "\n"
#define C_LINEFEED '\n'
#define MVAR_GLYPH 0
#define MVAR_STATE 1
#define MVAR_VALUE 2

#ifndef PSYC_UDP
# define      QUIT    remove_interactive(ME); return 0;
#endif

step(); // prototype

// overload this as needed
varargs mixed croak(string mc, string data, vamapping vars) { return 0; }

// initialize the parser
void parser_init() {
    buffer = "";
    state = PSYCPARSE_STATE_GREET; // AFTER reset
}

// it is sometimes useful to stop parsing
void interrupt_parse() {
    state = PSYCPARSE_STATE_BLOCKED;
}

// and resume after some blocking operation is done
void resume_parse() {
    state = PSYCPARSE_STATE_HEADER;
}

// called when a complete packet has arrived
void dispatch(mapping rvars, mapping evars, mixed method, mixed body) {
}

void psyc_dispatch(mixed p) {
    if (p[PACKET_METHOD] && p[PACKET_METHOD][0] != '_') {
	    log_file("SPYC", "%O SYNTAX %O\n", query_ip_name(), p);
	    croak("_error_invalid_method_compact",
		    "Compact methods undefined as yet.");
	    QUIT
    }
    dispatch(p[PACKET_ROUTING], p[PACKET_ENTITY], p[PACKET_METHOD], p[PACKET_BODY]);
}

// respond to the first empty packet
void first_response() {
    P0(("parser::first_response called. overload this!"))
}

// input data to the buffer
void feed(string data) {
    P4((">> feed: %O\n", data));
# ifdef _flag_log_sockets_SPYC
    log_file("RAW_SPYC", "» %O\n%s\n", ME, data);
# endif

    buffer += data;

    if (state != PSYCPARSE_STATE_HEADER)
	step();

    if (state == PSYCPARSE_STATE_HEADER) {
	switch (psyc_parse(buffer)) {
	    case 0: // success
		break;
	    case PSYC_PARSE_ERROR_AMOUNT:
		croak("_error_invalid_amount");
		QUIT
	    case PSYC_PARSE_ERROR_DEGREE:
		croak("_error_invalid_degree");
		QUIT
	    case PSYC_PARSE_ERROR_DATE:
		croak("_error_invalid_date");
		QUIT
	    case PSYC_PARSE_ERROR_TIME:
		croak("_error_invalid_time");
		QUIT
	    case PSYC_PARSE_ERROR_FLAG:
		croak("_error_invalid_flag");
		QUIT
	    case PSYC_PARSE_ERROR_LIST:
		croak("_error_invalid_list");
		QUIT
	    case PSYC_PARSE_ERROR_LIST_TOO_LARGE:
		croak("_error_list_too_large");
		QUIT
	    default: // parse error
		croak("_error_invalid_syntax");
	}
	buffer = "";
    }
}

// parser stepping function
void step() {
    P3(("%O step: state %O, buffer %O\n", ME, state, buffer))
    if (!strlen(buffer))
	return;
    switch(state) {
    case PSYCPARSE_STATE_BLOCKED:
	// someone requested to stop parsing - e.g. _request_features circuit
	// message
	break;
    case PSYCPARSE_STATE_GREET: // wait for greeting
	if (strlen(buffer) < 2)
	    return;
	if (buffer[0..1] == DELIM) {
	    state = PSYCPARSE_STATE_HEADER;
	    buffer = buffer[2 ..];
	    first_response();
	} else {
	    croak("_error_syntax_initialization");
		// "The new protocol begins with a pipe and a line feed.");
	}
	break;
    default: // uhm... if we ever get here this is the programmers fault
	break;
    }
}

