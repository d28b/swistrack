#! /usr/bin/env python

import re

# Information for one frame may look like this:
#   $STEP_START
#   $FRAMENUMBER,34
#   $PARTICLE,0,0,0,0
#   $STEP_STOP

# Prepare regular expressions
re_stepstart = re.compile('\$STEP_START')
re_framenumber = re.compile('\$FRAMENUMBER,([\d\-]*)')
re_particle = re.compile('\$PARTICLE,([\d-]*),([\d\.]*),([\d\.]*),([\d\.]*)')

# Read from STDIN continuously
framenumber = 0
while True:
	line = raw_input()

	result = re_stepstart.search(line)
	if result != None:
		# This is the beginning of a frame
		print 'We\'ve got a new frame ...';
		continue

	result = re_framenumber.search(line)
	if result != None:
		# This is the frame number (first result of regular expression)
		framenumber=result.group(1)
		print '  Framenumber = ', framenumber;
		continue

	result = re_particle.search(line)
	if result != None:
		# This is a particle
		print '  Particle: ID = ', result.group(1), ', X = ', result.group(2), ', Y = ', result.group(3), ', alpha = ', result.group(4);
		continue
