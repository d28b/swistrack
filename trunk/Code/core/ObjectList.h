#ifndef HEADER_ObjectList
#define HEADER_ObjectList

#include <wx/string.h>
#include <list>
#include "ConfigurationXML.h"

//! Reads and holds a simulation of particles.
class ObjectList: public ConfigurationXML {

public:
	//! A chip sequence type.
	typedef std::vector<float> tChipVector;

	//! Information about one object.
	class Object {
	public:
		int objectid;			//!< The object id.
		tChipVector chips;		//!< The corresponding chips. Note that chips are stored in clockwise order, starting at 12 o'clock (if angle=0).
		float angle;			//!< Correction angle, if the blob is rotated.
	};

	//! A list of objects type.
	typedef std::list<Object> tObjectList;

	tObjectList mObjects;		//!< The objects.
	wxString mError;			//!< The first error that occured.

	//! Constructor.
	ObjectList(const wxFileName &filename);
	//! Destructor.
	~ObjectList();

	//! Returns the file name of the current simulation.
	wxFileName GetFileName() {
		return mFileName;
	}

private:
	wxFileName mFileName;		//!< The name of the currently open file.

	//! Reads one object from the file.
	void ReadObject(wxXmlNode *node);
};

#endif
