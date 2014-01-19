/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#ifndef GENERIC_FUNCTIONS_H
#define GENERIC_FUNCTIONS_H

#include <string>

class BRect;

class GenericFunctions {
	
public:

	static	BRect		CenteredFrame(float window_width, float window_height);
	static	std::string	GenerateUniqueID();
	static	std::string	TimeStamp();
	static	std::string	GenerateNick(std::string username);
	static	std::string	CrushOutWhitespace(std::string text);
	static	int    		SeparateGroupSpecifiers(
							std::string text,
							std::string &room,
							std::string &server,
							std::string &user);
							
};

#endif
