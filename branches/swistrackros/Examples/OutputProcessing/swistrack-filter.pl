#! /usr/bin/perl
use strict;
$|=1;

# Information for one frame may look like this:
#   $STEP_START
#   $FRAMENUMBER,34
#   $PARTICLE,0,0,0,0
#   $STEP_STOP

# Read from STDIN continously
while (my $line=<>) {
	if ($line =~ /\$STEP_START/) {
		# This is the beginning of a frame
		print 'We\'ve got a new frame ...', "\n";
	} elsif ($line =~ /\$FRAMENUMBER,([\d-]*)/) {
		# This is the frame number (first result of regular expression)
		print '  Framenumber =', $1, "\n";
	} elsif ($line =~ /\$PARTICLE,([\d-]*),([\d\.-]*),([\d\.-]*),([\d\.-]*)/) {
		# This is a particle
		print '  Particle: ID = ', $1, ', X = ', $2, ', Y = ', $3, ', alpha = ', $4, "\n";
	} 
}
