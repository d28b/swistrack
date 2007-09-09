#!/usr/bin/perl
use strict;
use IO::Socket;

# Connect to SwisTrack: replace "localhost" by the name or IP address of the computer SwisTrack is running on
my $swistrack = IO::Socket::INET->new(
	Proto    => 'tcp',
	PeerAddr => 'localhost',
	PeerPort => 3000,
	) or die 'Cannot connect to SwisTrack!';

# Set running = true
print 'Starting SwisTrack ...', "\n";
print $swistrack '$RUN,true', "\n";

# Read the output of SwisTrack
print 'Waiting for the first blob ...', "\n";
my $framenumber=0;
while (my $line=<$swistrack>) {
	if ($line =~ /\$BEGINFRAME,([\d-]*)\*/) {
		# This is the beginning of a frame - just store the frame number (first result of regular expression)
		$framenumber=$1;
	} elsif ($line =~ /\$PARTICLE,([\d-]*),([\d\.-]*),([\d\.-]*),([\d\.-]*)\*/) {
		# This is a particle
		print 'Particle found in frame ', $framenumber, ': ', $1, ' ', $2, ' ', $3, ' ', $4, "\n";
		print 'Stopping SwisTrack now ...', "\n";
		print $swistrack '$RUN,false', "\n";
	} 
}
