//////////////////////////////////////////////////
// Haiku Chat [BlabberSettings.h]
//     "Singleton" object that reads application settings for Blabber.
//////////////////////////////////////////////////

#ifndef BLABBER_SETTINGS_H
#define BLABBER_SETTINGS_H

#ifndef FILE_XML_READER_H
	#include "FileXMLReader.h"
#endif

class BlabberSettings : public FileXMLReader {
public:
	static BlabberSettings  *Instance();	
	                        ~BlabberSettings();

protected:
	                         BlabberSettings(const char *filename);
	
private:
	// "Singleton" pattern's only instance of class
	static BlabberSettings *_instance;
	
	void SetDefaultTagsValue();
};

#endif
