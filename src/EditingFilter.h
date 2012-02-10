//////////////////////////////////////////////////
// Blabber [EditingFilter.h]
//////////////////////////////////////////////////

#ifndef EDITING_FILTER_H
#define EDITING_FILTER_H

#include <MessageFilter.h>
#include <interface/TextView.h>

class ChatWindow;

class EditingFilter : public BMessageFilter
{
	public:
	                      		EditingFilter(BTextView *view, ChatWindow *window);

		virtual	filter_result	Filter(BMessage *message, BHandler **target);

	private:
				BTextView		*view;
				ChatWindow		*window;
};

#endif

