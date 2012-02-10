//////////////////////////////////////////////////
// Haiku Chat [ChatTextView.cpp]
//////////////////////////////////////////////////

#include "ChatTextView.h"

#include <string>
#include <iostream>

#include <be_apps/NetPositive/NetPositive.h>
#include <Roster.h>

ChatTextView::ChatTextView(BRect frame, const char *name, BRect text_rect,
	uint32 resizing_mode, uint32 flags)
	: BTextView(frame, name, text_rect, resizing_mode, flags)
{
}

ChatTextView::ChatTextView(BRect frame, const char *name, BRect text_rect,
	const BFont *font, const rgb_color *color, uint32 resizing_mode, uint32 flags)
	: BTextView(frame, name, text_rect, font, color, resizing_mode, flags)
{
}

ChatTextView::~ChatTextView()
{
	fprintf(stderr, "ChatTextView desctructor called.\n");
}

void ChatTextView::MouseDown(BPoint pt)
{
	fprintf(stderr, "ChatTextView::MouseDown.\n");
	
	const char *text = BString(Text()).String();
	std::string url;

	// base
	BTextView::MouseDown(pt);

	// Ugly link search
	int32 curr_offset = OffsetAt(pt);

	// no more looking at spaces
	while (curr_offset >= 0 && !isspace(text[curr_offset])) {
		if (curr_offset + 8 <= TextLength() && text[curr_offset] == 'h'
			&& text[curr_offset + 1] == 't' && text[curr_offset + 2] == 't'
			&& text[curr_offset + 3] == 'p' && text[curr_offset + 4] == ':'
			&& text[curr_offset + 5] == '/' && text[curr_offset + 6] == '/') {

			url = text[curr_offset++];
			while(curr_offset < TextLength() && !isspace(text[curr_offset])) {
				url += text[curr_offset++];
			}

			break;
		}

		if (curr_offset + 7 <= TextLength() && text[curr_offset] == 'f'
			&& text[curr_offset + 1] == 't' && text[curr_offset + 2] == 'p'
			&& text[curr_offset + 3] == ':' && text[curr_offset + 4] == '/'
			&& text[curr_offset + 5] == '/') {

			url = text[curr_offset++];
			while(curr_offset < TextLength() && !isspace(text[curr_offset])) {
				url += text[curr_offset++];
			}

			break;
		}

		--curr_offset;
	}

	// default to www.
	if (url.empty()) {
		// Ugly link search
		curr_offset = OffsetAt(pt);

		while (curr_offset >= 0 && !isspace(text[curr_offset])) {
			if (curr_offset + 5 <= TextLength() && text[curr_offset] == 'w'
				&& text[curr_offset + 1] == 'w' && text[curr_offset + 2] == 'w'
				&& text[curr_offset + 3] == '.') {
				// ignore if it's not at the beginning or has no whitespace
				if ((curr_offset - 1) >= 0 && isalnum(text[curr_offset - 1])) {
					--curr_offset;
					continue;
				}

				// is it part of a sentence
				if (isspace(text[curr_offset + 4]) || text[curr_offset + 4] == '.') {
					--curr_offset;
					continue;
				}

				url = text[curr_offset++];

				while(curr_offset < TextLength() && !isspace(text[curr_offset])) {
					url += text[curr_offset++];
				}

				// prepend http
				url = "http://" + url;

				break;
			}

			if (curr_offset + 5 <= TextLength() && text[curr_offset] == 'f'
				&& text[curr_offset + 1] == 't' && text[curr_offset + 2] == 'p'
				&& text[curr_offset + 3] == '.') {
				url = text[curr_offset++];

				while(curr_offset < TextLength() && !isspace(text[curr_offset])) {
					url += text[curr_offset++];
				}

				// prepend http
				url = "ftp://" + url;

				break;
			}

			--curr_offset;
		}
	}

	// prune punctuation
	if (!url.empty()) {
		while (url.size() > 0) {
			if (url[url.size() - 1] == ',' || url[url.size() - 1] == '!'
				|| url[url.size() - 1] == '.' || url[url.size() - 1] == ')'
				|| url[url.size() - 1] == ';' || url[url.size() - 1] == ']'
				|| url[url.size() - 1] == '>' || url[url.size() - 1] == '?'
				|| url[url.size() - 1] == '\'' || url[url.size() - 1] == '"') {
				url.erase(url.size() - 1);
			} else {
				break;
			}
		}
	}

	// load up browser!!
	if (!url.empty()) {
		char *argv[] = {const_cast<char *>(url.c_str()), NULL};
		if (!be_roster->IsRunning("text/html"))
			be_roster->Launch("text/html", 1, argv);
		else {
			BMessenger messenger("text/html");
			BMessage msg(B_NETPOSITIVE_OPEN_URL);
			msg.AddString("be:url", url.c_str());
			messenger.SendMessage(&msg);
		}
	}
}
