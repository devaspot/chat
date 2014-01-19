/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#ifndef BUDDY_WINDOW_H
#define BUDDY_WINDOW_H

#include <Box.h>
#include <Button.h>
#include <MenuField.h>
#include <PopUpMenu.h>
#include <TextControl.h>
#include <View.h>
#include <Window.h>
#include "UserID.h"

class BuddyWindow : public BWindow
{
	public:
		static	BuddyWindow		*Instance();
								~BuddyWindow();

				void			MessageReceived(BMessage *msg);
				bool            QuitRequested();
				void            AddNewUser();
				UserID			*userID;
				void			SetUser(UserID *user);
				void			ApplyChangesToUser();
	
	protected:
	                     		BuddyWindow(BRect frame, UserID *user);
				
	private:
		static	BuddyWindow		*_instance;
				BBox            *_surrounding;
				BView           *_full_view;
				BTextControl    *_realname;
				BTextControl    *_room_nick;
				BMenuField      *_chat_services;
				BPopUpMenu      *_chat_services_selection;
				BTextControl    *_handle;
				BTextView       *_enter_note;
				BButton         *_cancel;
				BButton         *_ok;
	
};

#endif
