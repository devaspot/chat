//////////////////////////////////////////////////
// Blabber [GenericFunctions.cpp]
//////////////////////////////////////////////////

#ifndef GENERIC_FUNCTIONS_H
	#include "GenericFunctions.h"
#endif

#ifndef __CSTDIO__
	#include <cstdio>
#endif

#ifndef __CSTDLIB__
	#include <cstdlib>
#endif

#ifndef _UNI_STD_H
	#include <unistd.h>
#endif

#ifndef __STRING__
	#include <string>
#endif

#ifndef _RECT_H
	#include <interface/Rect.h>
#endif

#ifndef _SCREEN_H
	#include <interface/Screen.h>
#endif

BRect GenericFunctions::CenteredFrame(float window_width, float window_height) {
	// position return rectangle based on screen coordinates
	BScreen screen;

	float screen_width  = screen.Frame().Width();
	float screen_height = screen.Frame().Height();

	float frame_width   = screen_width  - window_width;
	float frame_height  = screen_height - window_height;

	float frame_start_x = frame_width   / 2;
	float frame_start_y = frame_height  / 2;
	float frame_end_x   = frame_start_x + window_width;
	float frame_end_y   = frame_start_y + window_height;

	// assemble window frame size
	return BRect(frame_start_x, frame_start_y, frame_end_x, frame_end_y);
}

std::string GenericFunctions::GenerateUniqueID() {
	static long counter = 0;

	// element #1: PID of process (same through application run-time)
	pid_t pid = getpid();

	// element #2: seconds since Jan. 1, 1970 (new value every second)
	time_t secs = time(NULL);

	// element #3: private counter (new value every call)
	++counter;

	// glue number together
	char buffer[100];

	sprintf(buffer, "%lu:%lu:%lu", pid, secs, counter);

	// return value
	return std::string(buffer);
}

std::string GenericFunctions::TimeStamp() {
	// get current time in seconds
	time_t t = time(NULL);

	// format time
	std::string time_string = ctime(&t);

	// chop off trailing newline
	if (time_string[time_string.size() - 1] == '\n') {
		time_string.erase(time_string.size() - 1);
	}

	return time_string;
}

std::string GenericFunctions::GenerateNick(std::string username) {
	std::string nick = username;

	// strip out everything after @
	std::string::size_type at_pos = nick.find("@");
	if (at_pos != std::string::npos) {
		nick = nick.substr(0, at_pos);
	}

	return nick;
}

std::string GenericFunctions::CrushOutWhitespace(std::string text) {
	std::string::size_type pos;

	while(text.find(" ") != std::string::npos) {
		pos = text.find(" ");
		text.erase(pos, 1);
	}

	while(text.find("\t") != std::string::npos) {
		pos = text.find("\t");
		text.erase(pos, 1);
	}

	while(text.find("\r") != std::string::npos) {
		pos = text.find("\r");
		text.erase(pos, 1);
	}

	while(text.find("\n") != std::string::npos) {
		pos = text.find("\n");
		text.erase(pos, 1);
	}

	return text;
}

int GenericFunctions::SeparateGroupSpecifiers(std::string text, std::string &room, std::string &server, std::string &user) {
	// separate room and server
	std::string::size_type at_pos = text.find("@");

	if (at_pos != std::string::npos) {
		// get group room
		room = text.substr(0, at_pos);

		// clear out text
		text = text.substr(at_pos + 1);

		// now pare out server and username
		std::string::size_type slash_pos = text.find("/");

		if (slash_pos != std::string::npos) {
			// get server
			server = text.substr(0, slash_pos);

			// clear out text
			user = text.substr(slash_pos + 1);

			return 3;
		} else {
			// get server
			server = text.substr(0, slash_pos);

			return 2;
		}
	} else {
		return 0;
	}
}
