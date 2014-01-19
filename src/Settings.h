/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#ifndef BLABBER_SETTINGS_H
#define BLABBER_SETTINGS_H

#include "FileXMLReader.h"

class BlabberSettings : public FileXMLReader {
	
public:
	
	static	BlabberSettings*	Instance();	
	                        	~BlabberSettings();

protected:

	                         	BlabberSettings(const char *filename);
	
private:

	static	BlabberSettings*	_instance;
			void				SetDefaultTagsValue();
			
};

#endif
