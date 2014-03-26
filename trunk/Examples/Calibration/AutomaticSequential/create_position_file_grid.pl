#! /usr/bin/perl
use strict;

# This script creates a position file with positions on a regular grid with rectangular grid cells.
# To write a position file, adjust the values below, execute the script and pipe the output into a file:
#	./create_position_file_grid.pl > positions

# Configuration
my $x_count=5;		# Number of points in X direction
my $y_count=5;		# Number of points in Y direction
my $x_arena_size=2;	# Size of the arena in X direction [meters]
my $y_arena_size=2;	# Size of the arena in Y direction [meters]

# Coordinates in X and Y direction
my @xlist=map {($_-1) / ($x_count-1) * $x_arena_size} (1..$x_count);
my @ylist=map {($_-1) / ($y_count-1) * $y_arena_size} (1..$y_count);

# Write one entry for each grid point
foreach my $x (@xlist) {
	foreach my $y (@ylist) {
		print '$GOTO_POSITION,', $x, ',', $y, "\n";
		print '$GOTO_WAIT,3000000', "\n";
	}
	@ylist=reverse @ylist;
}

# At the end, go back to (0, 0)
print '$GOTO_POSITION,0,0', "\n";
