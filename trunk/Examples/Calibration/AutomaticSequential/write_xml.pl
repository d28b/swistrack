#! /usr/bin/perl
use strict;

my @positions;

while (my $line=<>) {
	while (chomp $line) {}
	my @args=split(/\s+/, $line);

	my $position_id=$args[0];
	if (! $positions[$position_id]) {$positions[$position_id]=[];}
	push @{$positions[$position_id]}, [@args];
}

print '<?xml version="1.0"?>', "\n";
print '	<pointlist>', "\n";
print '		<points>', "\n";

foreach my $position (@positions) {
	next if (! $position);

	my $camera_x_sum=0;
	my $camera_y_sum=0;
	my $camera_count=0;
	foreach my $measurement (@$position) {
		next if (! $measurement);

		$camera_x_sum+=$measurement->[4];
		$camera_y_sum+=$measurement->[5];
		$camera_count++;
	}

	print '			<point>', "\n";
	print '				<xworld>', $position->[0]->[1]+0.4, '</xworld>', "\n";
	print '				<yworld>', $position->[0]->[2]+0.4, '</yworld>', "\n";
	print '				<ximage>', $camera_x_sum/$camera_count, '</ximage>', "\n";
	print '				<yimage>', $camera_y_sum/$camera_count, '</yimage>', "\n";
	print '			</point>', "\n";
}

print '		</points>', "\n";
print '	</pointlist>', "\n";
