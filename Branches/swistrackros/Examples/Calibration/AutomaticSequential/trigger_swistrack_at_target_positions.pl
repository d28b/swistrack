#! /usr/bin/perl
use strict;

# This script receives the output of the "odometry_goto" program running on the robot. Each time the robot reaches a target position, it tells SwisTrack to take a series of pictures.

# Configuration
my @swistrack=(['localhost', 3000]);
my $number_of_shots=10;

# Configuration example for a multi-camera system with 4 cameras and two PCs processing their images
#my @swistrack=(
#	['192.168.0.10', 3000],
#	['192.168.0.10', 3001],
#	['192.168.0.11', 3000],
#	['192.168.0.11', 3001],
#	);

# Prepare
my $step_message='$STEP\n' x $number_of_shots;

# Process each line
while (my $line=<>) {
	# Cut away all line breaks
	while (chomp $line) {}

	# Did we get a position message?
	if ($line =~ /^\$POSITION,([\d\.-]+),([\d\.-]+),([\d\.-]+)/) {
		# Print this position message
		print $line, "\n";

		# Take pictures with each camera
		my $camera_id=0;
		foreach my $connection (@swistrack) {
			print '$CAMERA,', $camera_id, "\n";
			system('echo -e \''.$step_message.'\' | nc -q 1 '.$connection->[0].' '.$connection->[1]);
			$camera_id++;
		}
	}
}
