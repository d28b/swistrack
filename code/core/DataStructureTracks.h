#ifndef HEADER_DataStructureTracks
#define HEADER_DataStructureTracks

#include <map>
#include <cv.h>
#include "DataStructure.h"
#include "Track.h"

// A DataStructure holding a list of tracks.
class DataStructureTracks: public DataStructure {


public:
	//! Track vector type.
        typedef std::map<int, Track> tTrackMap;

	tTrackMap *mTracks;	//!< Vector of tracks.

	//! Constructor.
	DataStructureTracks(): DataStructure(wxT("Tracks"), wxT("Tracks")), mTracks(0) {}
	//! Destructor.
	~DataStructureTracks() {}
};

#endif
