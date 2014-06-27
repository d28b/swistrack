#/bin/sh

ST_HOME=/home/disal/swistrack_opencv2/SwisTrackEnvironment
ST_BIN=./SwisTrack

echo "Launching SwisTrack instances"
(cd $ST_HOME && $ST_BIN config/cam1/config.swistrack) &
(cd $ST_HOME && $ST_BIN config/cam2/config.swistrack) &
(cd $ST_HOME && $ST_BIN config/cam3/config.swistrack) &
(cd $ST_HOME && $ST_BIN config/cam4/config.swistrack) &
(cd $ST_HOME && $ST_BIN config/cam5/config.swistrack) &
(cd $ST_HOME && $ST_BIN config/cam6/config.swistrack) &

echo "Waiting"
sleep 1

echo "Launching SwisTrackButton"
./SwisTrackButton &

