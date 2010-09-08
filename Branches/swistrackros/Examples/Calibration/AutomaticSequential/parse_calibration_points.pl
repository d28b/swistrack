#! /usr/bin/perl
use strict;

# Configuration
my $camera_id_wish=(shift || 0);
my $skip_images_at_beginning=4;
#my $camera_robotid_wish=(shift || -1);	# Activate this line if you are using markers with IDs

# Parse the file
my $target_id = 0;
my $camera_id = 0;
my $camera_x = 0;
my $camera_y = 0;
my $robot_x = 0;
my $robot_y = 0;
my $step_number = 0;
while (my $line=<>) {
	while (chomp $line) {}
	if ($line =~ /^\$POSITION,([\d\.-]+),([\d\.-]+),([\d\.-]+)/) {
		$target_id++;
		$robot_x = $1;
		$robot_y = $2;
	} elsif ($line =~ /^\$PARTICLE,([\d\.-]+),([\d\.-]+),([\d\.-]+)/) {
		my $camera_robotid = $1;

		#if ($camera_robotid_wish == $camera_robotid) {  # Activate this line if you are using markers with IDs
			$camera_x = $2;
			$camera_y = $3;
		#}
	} elsif ($line =~ /^\$CAMERA,(\d+)/) {
		$camera_id = $1;
		$step_number = 0;
	} elsif ($line =~ /^\$STEP_STOP/) {
		$step_number++;
		if ($step_number > $skip_images_at_beginning) {
			if ($camera_id_wish == $camera_id) {
				print join("\t", $target_id, $robot_x, $robot_y, $camera_x, $camera_y), "\n";
			}
		}

		$camera_robotid = 0;
		$camera_x = 0;
		$camera_y = 0;
	}
}
