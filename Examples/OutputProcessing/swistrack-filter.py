#! /usr/bin/env python

import re

# Prepare regular expressions
re_beginframe = re.compile('\$BEGINFRAME,([\d\-]*)\*')
re_particle = re.compile('\$PARTICLE,([\d-]*),([\d\.]*),([\d\.]*),([\d\.]*)\*')

# Read from STDIN continously
framenumber = 0
while True:
	line = raw_input()

	result = re_beginframe.search(line)
	if result != None:
		# This is the beginning of a frame - just store the frame number (first result of regular expression)
		framenumber=result.group(1)
		continue

	result = re_particle.search(line)
	if result != None:
		# This is a particle
		print framenumber, ' ', result.group(1), ' ', result.group(2), ' ', result.group(3), ' ', result.group(4), "\n";
		continue
