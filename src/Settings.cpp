//////////////////////////////////////////////////
// Haiku Chat [BlabberSettings.cpp]
//////////////////////////////////////////////////

#ifndef BLABBER_SETTINGS_H
	#include "Settings.h"
#endif

#ifndef _DIRECTORY_H
	#include <storage/Directory.h>
#endif

#ifndef MODAL_ALERT_FACTORY_H
	#include "ModalAlertFactory.h"
#endif

BlabberSettings *BlabberSettings::_instance = NULL;

BlabberSettings *BlabberSettings::Instance() {
	if (_instance == NULL) {
		_instance = new BlabberSettings("jabber-for-beos/app-settings");
	}
	
	return _instance;
}

BlabberSettings::~BlabberSettings() {
	_instance = NULL;
}

BlabberSettings::BlabberSettings(const char *filename)
	: FileXMLReader(filename, true) {
	// check file opening status and give a friendly message
	FileXMLReader::file_status status = FileStatus();
	
	if (status == FileXMLReader::FILE_NOT_FOUND) {
		// first time user perhaps?
//		ModalAlertFactory::NonModalAlert("It appears that this is your first time using Jabber for Haiku.", "Your welcome!");
       SetDefaultTagsValue();
	} else if (status == FileXMLReader::FILE_CORRUPTED) {
		// back up their settings
		ModalAlertFactory::Alert("We regret to inform you that your settings file has been corrupted.  It has been replaced with a fresh copy.", "Oh, darn!");
		SetDefaultTagsValue();
	}
}

void
BlabberSettings::SetDefaultTagsValue()
{
	SetTag("enable-double-click", true);
}
