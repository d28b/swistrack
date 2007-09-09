#! /usr/bin/perl
use strict;
$|=1;

# Read from STDIN continously
my $framenumber=0;
while (my $line=<>) {
	if ($line =~ /\$BEGINFRAME,([\d-]*)\*/) {
		# This is the beginning of a frame - just store the frame number (first result of regular expression)
		$framenumber=$1;
	} elsif ($line =~ /\$PARTICLE,([\d-]*),([\d\.-]*),([\d\.-]*),([\d\.-]*)\*/) {
		# This is a particle
		print $framenumber, ' ', $1, ' ', $2, ' ', $3, ' ', $4, "\n";
	} 
}
