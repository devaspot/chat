/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#include "GenericFunctions.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <Rect.h>
#include <Screen.h>

BRect
GenericFunctions::CenteredFrame(float window_width, float window_height)
{
	BScreen screen;

	float screen_width  = screen.Frame().Width();
	float screen_height = screen.Frame().Height();
	float frame_width   = screen_width  - window_width;
	float frame_height  = screen_height - window_height;
	float frame_start_x = frame_width   / 2;
	float frame_start_y = frame_height  / 2;
	float frame_end_x   = frame_start_x + window_width;
	float frame_end_y   = frame_start_y + window_height;
	return BRect(frame_start_x, frame_start_y, frame_end_x, frame_end_y);
}

std::string
GenericFunctions::GenerateUniqueID()
{
	static long counter = 0;
	pid_t pid = getpid();
	time_t secs = time(NULL);
	++counter;
	char buffer[100];
	sprintf(buffer, "%lu:%lu:%lu", pid, secs, counter);
	return std::string(buffer);
}

std::string
GenericFunctions::TimeStamp()
{
	time_t t = time(NULL);
	std::string time_string = ctime(&t);
	if (time_string[time_string.size() - 1] == '\n')
		time_string.erase(time_string.size() - 1);
	return time_string;
}

std::string
GenericFunctions::GenerateNick(std::string username)
{
	std::string nick = username;
	std::string::size_type at_pos = nick.find("@");
	if (at_pos != std::string::npos)
		nick = nick.substr(0, at_pos);
	return nick;
}

std::string
GenericFunctions::CrushOutWhitespace(std::string text)
{
	std::string::size_type pos;
	while(text.find(" ") != std::string::npos)
	{
		pos = text.find(" ");
		text.erase(pos, 1);
	}

	while(text.find("\t") != std::string::npos)
	{
		pos = text.find("\t");
		text.erase(pos, 1);
	}

	while(text.find("\r") != std::string::npos)
	{
		pos = text.find("\r");
		text.erase(pos, 1);
	}

	while(text.find("\n") != std::string::npos)
	{
		pos = text.find("\n");
		text.erase(pos, 1);
	}

	return text;
}

int
GenericFunctions::SeparateGroupSpecifiers(
	std::string text,
	std::string &room,
	std::string &server,
	std::string &user)
{
	std::string::size_type at_pos = text.find("@");

	if (at_pos != std::string::npos)
	{
		room = text.substr(0, at_pos);
		text = text.substr(at_pos + 1);
		std::string::size_type slash_pos = text.find("/");

		if (slash_pos != std::string::npos)
		{
			server = text.substr(0, slash_pos);
			user = text.substr(slash_pos + 1);
			return 3;
		}
		else
		{
			server = text.substr(0, slash_pos);
			return 2;
		}
	}
	else
	{
		return 0;
	}
}
