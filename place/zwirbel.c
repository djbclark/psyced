// vim:syntax=lpc
//
// still fails in some unusual cases, like
// "Comments? Suggestions? Submissions? Fire away!"

#define ON_CONVERSE \
    if (stringp(data)) data = zwirbel(0, data); \
    walk_mapping(vars, #'zwirbel);

#include <place.gen>

mixed* randomize(mixed* abs) {
	int size = sizeof(abs);
	int c, i;
	mixed temp;

	if (size > 3) {
		for (i = 0; i != size - 1; i++) {
		// das hier ist definitiv noch nicht perfekt.. vielleicht kann man sich
		// die for auch sparen, man changed ja eh.. also.. hmm
			while (c == i)
				c = random(size-1);

			// die if da tut mir wirklich leid... hehe :)
			if (((abs[c] >= 65 && abs[c] <= 90) || (abs[c] >= 97 && abs[c] <= 122)) && ((abs[i] >= 65 && abs[i] <= 90) || (abs[i] >= 97 && abs[i] <= 122))) {
				temp = abs[c];
				abs[c] = abs[i];
				abs[i] = temp;
			}
		}
	} else {
		while (c == i) {
			i = random(size-1);
			c = random(size-1);
		}
		temp = abs[c];
		abs[c] = abs[i];
		abs[i] = temp;
	}
	return abs;
}

mixed zwirbel(mixed name, mixed data) {
	string* words;
	int c = 0;

	if (!stringp(data) || strlen(data) < 5
	    || name == "_nick" || name == "_color") return data;
	words = explode(data, " ");
	foreach (string w : words) {
		// hier nochmal nach irgendwelchen satzzeichen am ende gucken
		// hmmm
		if (((w[0] >= 65 && w[0] <= 90) || (w[0] >= 97 && w[0] <= 122)) && strlen(w) >= 4) {
			words[c] = w[0..0] + to_string(randomize(to_array(w[1..<2]))) + w[<1..<1];
			
		}	
		c++;
	}
	data = implode(words, " ");
	//debug_message(sprintf("%O - %O\n", ME, data));
	return data;
}

htget(prot, query, headers, qs) {
    htok3(prot, "text/html", "");
    unless(query["s"]) {
	write("<form method='get'>\n"
	      "<input name='s' type='text' maxlength='512'>\n"
	      "<input type='submit'>\n"
	      "</form>\n");
    	P2(("done1\n"))
    } else {
	write(zwirbel(0, query["s"]));
    }
}
