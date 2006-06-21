***************************************************************************
* TCP/IP interface for SwisTrack
*
* (c) 2005, 2006 Nikolaus Correll
*			   Swarm-Intelligent Systems Group
*			   Ecole Polytechnique Federale Lausanne
*
***************************************************************************

1. Introduction

On start-up SwisTrack is listening on port 3000 (i.e. localhost:3000), where
it accepts socket connections. Commands can be send as single characters
(case sensitive), and are given below. SwisTrack will respond with the ap-
propriate action or will send back a string containing the response. It is 
important that you make sure to read the exact number of bytes from the socket, 
otherwise messages will get mixed up. The exact number of bytes that you can
expect is given for each command.

2. Available commands

SERVER COMMANDS

'p'  : pauses tracking
'R'  : starts tracker
's'  : performes one step
'S'  : resumes tracking (performs multiple steps)
'!'  : stops tracker

SERVER SETTINGS

'c'  : turns calibration off (default). Every point being sent, be it trajectory
       data or a blob will be sent in image coordinates.
'C'  : turns calibration on. Every point being sent, be it trajectory data or a 
       blob will be sent in world coordinates.
       
COMMANDS FOR RETRIEVING TRAJECTORY DATA

't'  : retrieves data points in an asynchronous fashion. You will always
       get the latest trajectory data. If your application queries data faster
       than it is produced by SwisTrack, you will get the same data more than 
       once. If SwisTrack is running at a faster pace, you will eventually loose
       information. The trajectory data is encoded as tuples of float with fixed
       precision and preceeding zeros in the format "+####.#### +####.####"
       (21) characters. Tuples are separated by spaces.

'T'  : retrieves data points synchronously. Every time, data is requested, SwisTrack
       will process a single frame. If your application queries frames faster than the
       framerate supported by the media in use (camera/video), SwisTrack delay packets
       to be sent out. If your application is running too slow, SwisTrack will skip frames
       provided by a camera, but not from a video source. The data is encoded as for 't'

'N'  : requests the number of tracks being tracked. Data is returned in the format ### 
       (3 characters), with leading zeros if necessary.
       
COMMANDS FOR RETRIEVING BLOB DATA

'b'  : same as 't'. SwisTrack will sent blobs that are preceeded with the number of blobs.
       The number of blobs is encoded as an integer with format '####', which is imediately 
       followed by the blob string (encoded as in 't').
'B'  : sames as 'b', but SwisTrack will perform a single step and go into pause mode after
       (as 'T').
'n'  : requests the number of blobs being detected. Data is returned in the format ### 
       (3 characters), with leading zeros if necessary.
       
COMMANDS FOR GETTING TASK PROGRESS

'i'  : sends back the current frame number in the format ########## (10 characters)
'I'  : sends back the tracking progress in the format ##.## (5 characters), given in percent
'f'  : sends back the current frame rate in the format ##.## (5 characters), given in Hertz
       
DATA FORMAT FOR TRAJECTORY DATA/BLOBS
 
Every point (be it calibrated, or uncalibrated, trajectory, or blob) is sent as fixed precision
floating point number in the format "+####.#### +####.####", and thus having a length of 21 
characters (including the separating spaces). Points are concatunated to a string. There is no
terminating character.
 