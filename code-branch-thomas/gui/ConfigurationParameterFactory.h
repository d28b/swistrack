#ifndef HEADER_ConfigurationParameterFactory
#define HEADER_ConfigurationParameterFactory

class ConfigurationParameterFactory;

#include "ConfigurationParameter.h"

class ConfigurationParameterFactory {

public:
	//! Constructor.
	ConfigurationParameterFactory() {}
	//! Destructor.
	~ConfigurationParameterFactory() {}

	//! Creates a new parameter.
	ConfigurationParameter *Create(const wxString &type, wxWindow* parent);
};

#endif
