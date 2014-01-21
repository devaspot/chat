/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#include "BuddyWindow.h"
#include <cstdio>
#include <Application.h>
#include <LayoutBuilder.h>
#include "Settings.h"
#include "GenericFunctions.h"
#include "Messages.h"
#include "ModalAlertFactory.h"
#include "TalkManager.h"
#include <strings.h>

#define BOX_WIDTH BSize(_realname->StringWidth("w")*20, B_SIZE_UNSET)

BuddyWindow *BuddyWindow::_instance = NULL;

BuddyWindow *BuddyWindow::Instance()
{
	if (_instance == NULL)
	{
		float main_window_width  = 340;
		float main_window_height = 195;
		BRect frame(GenericFunctions::CenteredFrame(main_window_width, main_window_height));
		_instance = new BuddyWindow(frame, NULL);
	}
	
	return _instance;
}

BuddyWindow::~BuddyWindow()
{
	_instance = NULL;
}

void
BuddyWindow::SetUser(UserID *user)
{
	userID = user;
	
	if (userID)
	{
		_realname->SetText(userID->FriendlyName().c_str());
		_handle->SetText(userID->JabberHandle().c_str());
	}
	
	if (!TalkManager::Instance()->jabber->_storage_supported)
		_room_nick->SetEnabled(false);
			
	_chat_services->SetEnabled(userID == NULL);
	_handle->SetEnabled(userID == NULL);
	
	if (userID && (userID->UserType() == UserID::CONFERENCE))
	{
		_room_nick->SetText(userID->_room_nick.c_str());
	 	_chat_services_selection->FindItem("Conference")->SetMarked(true);
	 	_handle->SetLabel("MUC JID:");
	}
	else
	{
		_chat_services_selection->FindItem("User")->SetMarked(true);
		_full_view->RemoveChild(_room_nick);
	}
	
	if (userID) _ok->SetLabel("Save");
	else _ok->SetLabel("Create");
}

BuddyWindow::BuddyWindow(BRect frame, UserID *user)
	:
	BWindow(frame, "Item", B_TITLED_WINDOW, B_NOT_RESIZABLE)
{

	_full_view = new BView(Bounds(), NULL, B_FOLLOW_ALL, B_WILL_DRAW);
	_full_view->SetViewColor(216, 216, 216, 255);
	
	_realname = new BTextControl("Title Name:", "", NULL);
	_realname->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	
	_handle = new BTextControl("JID:", "", NULL);
	_handle->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	
	_chat_services_selection = new BPopUpMenu("Simple");
	_chat_services = new BMenuField("Item Type:", _chat_services_selection);	
	_chat_services_selection->AddItem(new BMenuItem("User", new BMessage(AGENT_MENU_CHANGED_TO_JABBER)));
	_chat_services_selection->AddItem(new BMenuItem("Conference", new BMessage(AGENT_MENU_CHANGED_TO_JABBER_CONFERENCE)));
	_chat_services_selection->FindItem("User")->SetMarked(true);
	
	_room_nick = new BTextControl("Room Nick:", "", NULL);
	_room_nick->SetDivider(_room_nick->Divider() - 75);
	_room_nick->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	
	_ok = new BButton(Bounds(), "ok", "", new BMessage(JAB_OK));
	_ok->MakeDefault(true);
	_ok->SetTarget(this);
	
	BLayoutItem* fUsernameBox = _realname->CreateTextViewLayoutItem();
	fUsernameBox->SetExplicitMinSize(BOX_WIDTH);
	
	BLayoutBuilder::Group<>(_full_view, B_VERTICAL, 0)
		.SetInsets(B_USE_DEFAULT_SPACING)
		.AddGlue()
		.AddGrid(B_USE_DEFAULT_SPACING, B_USE_SMALL_SPACING)
			.Add(_realname->CreateLabelLayoutItem(), 0, 0)
			.Add(_realname->CreateTextViewLayoutItem(), 1, 0)
			.Add(_handle->CreateLabelLayoutItem(), 0, 1)
			.Add(_handle->CreateTextViewLayoutItem(), 1, 1)
			.Add(_chat_services, 1, 2)
			.Add(_room_nick, 1, 3)
		.End()
		.AddGlue()
		.AddGroup(B_HORIZONTAL)
			.AddGlue()
			.Add(_ok)
		.End()
		.AddGlue()
		.View();
		
	AddChild(_full_view);

	_realname->MakeFocus(true);
	
	int width = ceilf(BOX_WIDTH.width*1.7);
	SetSizeLimits(width, width, 250, 250);
}

void
BuddyWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case JAB_OK: 
			AddNewUser();
			break;
		
		case JAB_CANCEL: 
			PostMessage(B_QUIT_REQUESTED);
			break;

		case AGENT_MENU_CHANGED_TO_JABBER: 
			if (!strcasecmp(_handle->Label(), "MUC JID:"))
			{
				_handle->SetLabel("JID:");
				_full_view->RemoveChild(_room_nick);
			}
			break;
	
		case AGENT_MENU_CHANGED_TO_JABBER_CONFERENCE:
			if (!strcasecmp(_handle->Label(), "JID:"))
			{
				_handle->SetLabel("MUC JID:");
				_full_view->AddChild(_room_nick);
			}
			break;
	}
}

bool
BuddyWindow::QuitRequested()
{
	_instance = NULL;
	return true;
}

void
BuddyWindow::AddNewUser()
{
	char buffer[4096];

	if (!strcmp(_realname->Text(), ""))
	{
		ModalAlertFactory::Alert("Please specify your buddy's real name.", "Oops!");
		_realname->MakeFocus(true);

		return;
	}
	
	if (!strcmp(_handle->Text(), ""))
	{
		sprintf(buffer,
			"Please specify %s's %s handle.",
			_realname->Text(),
			(_chat_services_selection->FindMarked())->Label()); 
			
		ModalAlertFactory::Alert(buffer, "Oops!");
		_handle->MakeFocus(true);

		return;
	}
	
	string username = _handle->Text();

	if (strcasecmp(_handle->Label(), "JID:")) {
		username = GenericFunctions::CrushOutWhitespace(username);
	}

	UserID validating_user(username);
	
	if (!strcasecmp(_handle->Label(), "JID:") && 
		validating_user.WhyNotValidJabberHandle().size())
	{

		sprintf(buffer,
			"%s is not a valid Jabber ID for the following reason:\n\n"
			"%s\n\n"
			"Please correct it.",
			_handle->Text(),
			validating_user.WhyNotValidJabberHandle().c_str()); 
			
		ModalAlertFactory::Alert(buffer, "Hmm, better check that...");
		_handle->MakeFocus(true);
		
		return;
	}
	
	if (userID == NULL)
	{
		UserID* validated = BlabberMainWindow::Instance()->
								fRosterView->fUsers[validating_user.JabberHandle()];
								
		if (validated)
		{

			sprintf(buffer,
				"%s already exists in your buddy list. "
				"Please choose another so you won't get confused.",
				_handle->Text()); 
				
			ModalAlertFactory::Alert(buffer, "Good Idea!");
			
			_handle->MakeFocus(true);
			
			return;
		}
	}

	
	UserID *new_user;
	
	if (userID)
	{
		new_user = userID;
		new_user->SetRoomNick(_room_nick->Text());
		userID = NULL;
	}
	else 
	{
		new_user = new UserID(UserID(username));

		if (!strcasecmp(_handle->Label(), "JID:"))
		{
			new_user->SetUsertype(UserID::JABBER);
		}
		else
		{
			new_user->SetUsertype(UserID::CONFERENCE);
			new_user->SetOnlineStatus(UserID::CONF_STATUS);
			new_user->SetRoomNick(_room_nick->Text());
		}
	}
	
	new_user->SetFriendlyName(_realname->Text());
	
	if (new_user->UserType() == UserID::CONFERENCE && TalkManager::Instance()->jabber->_storage_supported)
	{
		TalkManager::Instance()->jabber->SaveConference(new_user);
		TalkManager::Instance()->jabber->SendStorageRequest("storage", "storage:bookmarks");
	}
	else
	{
		TalkManager::Instance()->jabber->AddToRoster(new_user);
		TalkManager::Instance()->jabber->SendSubscriptionRequest(new_user->JabberHandle());
	}
	
	PostMessage(B_QUIT_REQUESTED);
}
