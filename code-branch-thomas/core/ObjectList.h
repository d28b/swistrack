#ifndef HEADER_ObjectList
#define HEADER_ObjectList

#include <string>
#include <list>
#include "ConfigurationXML.h"

//! Reads and holds a simulation of particles.
class ObjectList: public ConfigurationXML {

public:
	//! Information about one object.
	class Object {
	public:
		int objectid;			//!< The object id.
		tChipVector chips;		//!< The corresponding chips. Note that chips are stored in clockwise order, starting at 12 o'clock (if angle=0).
		float angle;			//!< Correction angle, if the blob is rotated.
	};

	//! A chip sequence type.
	typedef std::vector<float> tChipVector;
	//! A list of objects type.
	typedef std::list<Object> tObjectList;

	tObjectList mObjects;		//!< The objects.
	std::string mError;			//!< The first error that occured.

	//! Constructor.
	ObjectList(const std::string &filename);
	//! Destructor.
	~ObjectList();

	//! Returns the file name of the current simulation.
	std::string GetFileName() {return mFileName;}

private:
	std::string mFileName;		//!< The name of the currently open file.

	//! Reads one object from the file.
	void ReadObject(wxXmlNode *node);
};

#endif