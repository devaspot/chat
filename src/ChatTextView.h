//////////////////////////////////////////////////
// Blabber [ChatTextView.h]
//     Handles MouseDown.
//////////////////////////////////////////////////

#ifndef CHAT_TEXT_VIEW_H
#define CHAT_TEXT_VIEW_H

#include <interface/TextView.h>
#include <String.h>

class ChatTextView : public BTextView {
public:
	ChatTextView(BRect frame, const char *name, BRect text_rect, uint32 resizing_mode, uint32 flags);
	ChatTextView(BRect frame, const char *name, BRect text_rect, const BFont *font, const rgb_color *color, uint32 resizing_mode, uint32 flags);
	~ChatTextView();
	void MouseDown(BPoint pt);
};

#endif
