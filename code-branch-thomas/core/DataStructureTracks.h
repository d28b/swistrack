#ifndef HEADER_DataStructureTracks
#define HEADER_DataStructureTracks

#include <vector>
#include <cv.h>
#include "DataStructure.h"
#include "Track.h"

// A DataStructure holding a list of tracks.
class DataStructureTracks: public DataStructure {


public:
	//! Particle vector type.
	typedef std::vector<Track> tTrackVector;

	tTrackVector *mTracks;	//!< Vector of tracks.

	//! Constructor.
	DataStructureTracks(): DataStructure("Tracks"), mTracks(0) {}
	//! Destructor.
	~DataStructureTracks() {}
};

#endif
