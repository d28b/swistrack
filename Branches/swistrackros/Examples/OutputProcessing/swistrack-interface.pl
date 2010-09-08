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
	if ($line =~ /\$STEP_START/) {
	} elsif ($line =~ /\$PARTICLE,([\d-]*),([\d\.-]*),([\d\.-]*),([\d\.-]*)/) {
		# This is a particle
		print 'Particle found: ', $1, ' ', $2, ' ', $3, ' ', $4, "\n";

		# We stop SwisTrack here (note that you may receive a few more particles until SwisTrack really stops)
		print 'Stopping SwisTrack now ...', "\n";
		print $swistrack '$RUN,false', "\n";
		last;
	} 
}
