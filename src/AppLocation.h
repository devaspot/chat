//////////////////////////////////////////////////
// Application Directory [AppLocation.h]
//     Provides directory information to the
//     executable path.
//////////////////////////////////////////////////

#ifndef APP_LOCATION_H
#define APP_LOCATION_H

#include <string>

class AppLocation {
public:
	static AppLocation  *Instance();
	                    ~AppLocation();

	void                 SetExecutableCall(std::string executable_call);
	std::string          Path();
	std::string          AbsolutePath(std::string relative_path);

protected:
	                     AppLocation();

private:
	static AppLocation  *_instance;
	std::string         _executable_path;
};

#endif
