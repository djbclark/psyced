#!/usr/bin/perl
#
# generator of psyced place models using combinations of archetype flags.
# lynX 2007

	my @opts, @predef, %map;
	open(I, "archetype.gen") or die <<X;

$0: Cannot open archetype.gen. Need to be in my directory!

X
	while (<I>) {
		if (m!define\s(\w+)\s*//\s\[(.)\]!) {
			if ($2 eq '+') {
				push @predef, $1;
			} else {
				$map{$2} = $1;
				push(@opts, $2);
			}
		} elsif (/^#endif/) {
			last;
		}
	}
	close I;
	print "Generating archetype models...\n";

#	print join("\n", @opts), "\n\n";
#	print "$_\n" foreach ( %map );

	open (O, ">../include/place.i") or die $!;
	print O <<X;
// generated by '$0': place.i for place.gen

X
	my $file = '';
	foreach $o (@opts) {
		print O <<X;
#ifdef $map{$o}
# define O$o "$o"
#else
# define O$o ""
#endif

X
		$file .= " O$o";
	}
	print O <<X;
inherit NET_PATH "place/_"$file;
X
	$predef = '';
	foreach $p (@predef) {
		$predef .= "#define $p\n";
	}

	open(IG, ">.cvsignore") or print <<X;
Warning: cannot create .cvsignore. Well, doesn't matter.
X
	# funny how it likes to see itself in there
	print IG ".cvsignore\n";

	my $bits = 1 + $#opts;
	for ($v = 1 << $bits; $v;) {
		$v--;
		$f = '';
		$model = '';

		for ($i = 0; $i < $bits; $i++) {
			if ($v & 1 << $i) {
				my $o = $opts[$i];
				$f .= $o;
				$model .= "#define $map{$o}\n";
			}
		}

		# special case: skip all exports without history
		next if $f =~ /^e/;
		# same special case is also handled in place.gen

		printf " (%02d _%s)", $v, $f;
		#print " ($v _$f)";

		print IG "_$f.c\n";
		open (O, ">_$f.c") or die $!;
# proud and noisy.. for a week or two at least  ;)
#echo loading model '_$f' generated by '$0'
		print O <<X;
// model '_$f' generated by '$0'

#define ESSENTIALS
$model
#include "archetype.gen"
X
# used to output $predef but it's easier to
# have archetype.gen sort out ESSENTIALS
		close O;
	}
	close IG;

	print "\nModel creation completed successfully.\n";

1; # this just so that psyconf can use it