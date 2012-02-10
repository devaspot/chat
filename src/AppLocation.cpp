//////////////////////////////////////////////////
// Application Directory [AppLocation.cpp]
//////////////////////////////////////////////////

#ifndef APP_LOCATION_H
	#include "AppLocation.h"
#endif

#ifndef _PATH_H
	#include <storage/Path.h>
#endif

AppLocation *AppLocation::_instance = NULL;

AppLocation *AppLocation::Instance() {
	if (_instance == NULL) {
		_instance = new AppLocation();
	}

	return _instance;
}

AppLocation::~AppLocation() {
}

void AppLocation::SetExecutableCall(std::string executable_call) {
	BPath path(executable_call.c_str());
	BPath directory;

	path.GetParent(&directory);

	_executable_path = directory.Path();
}

std::string AppLocation::Path() {
	return _executable_path;
}

std::string AppLocation::AbsolutePath(std::string relative_path) {
	return _executable_path + '/' + relative_path;
}

AppLocation::AppLocation() {
}
