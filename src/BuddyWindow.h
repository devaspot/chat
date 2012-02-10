//////////////////////////////////////////////////
// Blabber [BuddyWindow.h]
//     Lets a user add or edit a roster buddy.
//////////////////////////////////////////////////

#ifndef BUDDY_WINDOW_H
#define BUDDY_WINDOW_H

#include <interface/Box.h>
#include <interface/Button.h>
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/PopUpMenu.h>
#include <interface/TextControl.h>
#include <interface/View.h>
#include <interface/Window.h>
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
