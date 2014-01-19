/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#include "Settings.h"
#include <storage/Directory.h>
#include "ModalAlertFactory.h"

BlabberSettings* BlabberSettings::_instance = NULL;

BlabberSettings*
BlabberSettings::Instance()
{
	if (_instance == NULL) 
		_instance = new BlabberSettings("Chat/app-settings");
	
	return _instance;
}

BlabberSettings::~BlabberSettings()
{
	_instance = NULL;
}

BlabberSettings::BlabberSettings(const char *filename)
	:
	FileXMLReader(filename, true)
{
	FileXMLReader::file_status status = FileStatus();
	
	if (status == FileXMLReader::FILE_NOT_FOUND)
	{
       SetDefaultTagsValue();
	}
	else if (status == FileXMLReader::FILE_CORRUPTED)
	{
		ModalAlertFactory::Alert(
			"We regret to inform you that your settings "
			"file has been corrupted. "
			"It has been replaced with a fresh copy.",
			"Oh, darn!");
			
		SetDefaultTagsValue();
	}
}

void
BlabberSettings::SetDefaultTagsValue()
{
	SetTag("enable-double-click", true);
}
