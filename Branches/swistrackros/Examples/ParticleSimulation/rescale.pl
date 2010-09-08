#! /usr/bin/perl
use strict;

my $scaling_factor=shift;
if (! $scaling_factor) {
	print 'Usage: ./rescale.pl SCALING_FACTOR < input_file > output_file', "\n";
	exit(1);
}

while (my $line=<>) {
	if ($line =~ /\$PARTICLE,(\d*),([\d\.-]*),([\d\.-]*),([\d\.-]*),([\d\.-]*),([\d\.-]*)/) {
		print '$PARTICLE,', $1, ',', $2, ',', $3, ',', $4, ',', $5*$scaling_factor, ',', $6*$scaling_factor, "\n";
	} else {
		print $line;
	}
}
