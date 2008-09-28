#! /usr/bin/perl
use strict;

# This script executes the "odometry_goto" program on the robot and feeds its output to the "trigger_swistrack_at_target.pl" script.  The latter will instruct SwisTrack to take a series of pictures.

# In the following line, modify the IP address of the Khepera III robot as well as the path to your files on the robot
#                      vvv                   vvv                                    vvv
system('ssh root@192.168.141.200 "/root/your_username/odometry_goto file /root/your_username/positions" | ./trigger_swistrack_at_target_positions.pl');
