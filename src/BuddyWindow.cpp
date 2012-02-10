//////////////////////////////////////////////////
// Blabber [BuddyWindow.cpp]
//////////////////////////////////////////////////

#include "BuddyWindow.h"
#include <cstdio>
#include <Application.h>
#include "AppLocation.h"
#include "Settings.h"
#include "GenericFunctions.h"
#include "Messages.h"
#include "ModalAlertFactory.h"
#include "JRoster.h"
#include "TalkManager.h"
#include <strings.h>

BuddyWindow *BuddyWindow::_instance = NULL;

BuddyWindow *BuddyWindow::Instance()
{
	if (_instance == NULL) {
		float main_window_width  = 340;
		float main_window_height = 195;
		
		// create window frame position
		BRect frame(GenericFunctions::CenteredFrame(main_window_width, main_window_height));

		// create window singleton
		_instance = new BuddyWindow(frame, NULL);
	}
	
	return _instance;
}

BuddyWindow::~BuddyWindow() {
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
	//_room_nick->SetEnabled(!userID);
	
	if (userID && (userID->UserType() == UserID::CONFERENCE))
	{
		_room_nick->SetText(userID->_room_nick.c_str());
	 	_chat_services_selection->FindItem("Conference")->SetMarked(true);
	 	_handle->SetLabel("MUC JID:");

	 	//_full_view->AddChild(_room_nick);
	 	
	}
	else
	{
		_chat_services_selection->FindItem("User")->SetMarked(true);
		_full_view->RemoveChild(_room_nick);
	}
	
	if (userID)
		_ok->SetLabel("Save");
	else
		_ok->SetLabel("Create");
}

BuddyWindow::BuddyWindow(BRect frame, UserID *user)
	: BWindow(frame, "Item", B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{

	BRect rect;

	// encompassing view
	rect = Bounds();
	rect.OffsetTo(B_ORIGIN);
	
	_full_view = new BView(rect, NULL, B_FOLLOW_ALL, B_WILL_DRAW);
	_full_view->SetViewColor(216, 216, 216, 255);
	
	rect.OffsetTo(B_ORIGIN);
	rect.InsetBy(10.0, 12.0);
	
	// realname
	rect.bottom = rect.top + 18;
	_realname = new BTextControl(rect, "realname", "Title Name:", NULL, NULL, B_FOLLOW_ALL_SIDES);
	_realname->SetDivider(_realname->Divider() - 75);
	_realname->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	
	rect.OffsetBy(0.0, 23.0);
	_handle = new BTextControl(rect, "handle", "JID:", NULL, NULL, B_FOLLOW_ALL_SIDES);
	_handle->SetDivider(_handle->Divider() - 75);
	_handle->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	
	
	// chat service
	rect.OffsetBy(0.0, 23.0);
	_chat_services_selection = new BPopUpMenu("Simple");
	_chat_services = new BMenuField(rect, "chat_services", "Item Type:", _chat_services_selection);	
	_chat_services->SetDivider(_chat_services->Divider() - 75);
	_chat_services->SetAlignment(B_ALIGN_RIGHT);
	_chat_services_selection->AddItem(new BMenuItem("User", new BMessage(AGENT_MENU_CHANGED_TO_JABBER)));
	_chat_services_selection->AddItem(new BMenuItem("Conference", new BMessage(AGENT_MENU_CHANGED_TO_JABBER_CONFERENCE)));
	_chat_services_selection->FindItem("User")->SetMarked(true);
	
	{
		rect.OffsetBy(0.0, 24.0);
		_room_nick = new BTextControl(rect, "nick_room", "Room Nick:", NULL, NULL, B_FOLLOW_ALL_SIDES);
		_room_nick->SetDivider(_room_nick->Divider() - 75);
		_room_nick->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	}
	
	// ok button
	rect.OffsetBy(220.0, 55.0);
	rect.right = rect.left + 92;

	_ok = new BButton(rect, "ok", "", new BMessage(JAB_OK));
	_ok->MakeDefault(true);
	_ok->SetTarget(this);
	

	// add GUI components to BView
	_full_view->AddChild(_realname);
	_full_view->AddChild(_handle);
	_full_view->AddChild(_chat_services);
	_full_view->AddChild(_room_nick);
	_full_view->AddChild(_ok);
	AddChild(_full_view);

	// focus to start
	_realname->MakeFocus(true);
}

void BuddyWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what) {
		case JAB_OK: {
			AddNewUser();
			break;
		}
		
		case JAB_CANCEL: {
			PostMessage(B_QUIT_REQUESTED);
			break;
		}

		case AGENT_MENU_CHANGED_TO_JABBER: {
			if (!strcasecmp(_handle->Label(), "MUC JID:"))
			{
				_handle->SetLabel("JID:");
				_full_view->RemoveChild(_room_nick);
			}
			break;
		}
		
		case AGENT_MENU_CHANGED_TO_JABBER_CONFERENCE: {
			if (!strcasecmp(_handle->Label(), "JID:"))
			{
				_handle->SetLabel("MUC JID:");
				_full_view->AddChild(_room_nick);
			}
			break;
		}
	}
}

bool BuddyWindow::QuitRequested() {
	_instance = NULL;
	return true;
}

void BuddyWindow::AddNewUser()
{
	char buffer[4096];

	if (!strcmp(_realname->Text(), "")) {
		ModalAlertFactory::Alert("Please specify your buddy's real name.", "Oops!");
		_realname->MakeFocus(true);

		return;
	}
	
	if (!strcmp(_handle->Text(), "")) {
		sprintf(buffer, "Please specify %s's %s handle (or screenname).", _realname->Text(), (_chat_services_selection->FindMarked())->Label()); 
		ModalAlertFactory::Alert(buffer, "Oops!");
		_handle->MakeFocus(true);

		return;
	}
	
	// internally replace the username with a proper one if necessary (AOL, Yahoo!, etc...)
	//Agent *agent;
	string username = _handle->Text();

	// if not Jabber
	if (strcasecmp(_handle->Label(), "JID:")) {
		username = GenericFunctions::CrushOutWhitespace(username);
	}

	// make a user to validate against	
	UserID validating_user(username);
	
	if (!strcasecmp(_handle->Label(), "JID:") && validating_user.WhyNotValidJabberHandle().size()) {
		sprintf(buffer, "%s is not a valid Jabber ID for the following reason:\n\n%s\n\nPlease correct it.", _handle->Text(), validating_user.WhyNotValidJabberHandle().c_str()); 
		ModalAlertFactory::Alert(buffer, "Hmm, better check that...");
		_handle->MakeFocus(true);
		
		return;
	}
	
	if (userID == NULL)
	{
		// make sure it's not a duplicate of one already existing (unless itself)
		JRoster::Instance()->Lock();
		if (JRoster::Instance()->FindUser(JRoster::HANDLE, validating_user.JabberHandle())) {
			sprintf(buffer, "%s already exists in your buddy list.  Please choose another so you won't get confused.", _handle->Text()); 
			ModalAlertFactory::Alert(buffer, "Good Idea!");
			_handle->MakeFocus(true);
			
			JRoster::Instance()->Unlock();
			return;
		}
		JRoster::Instance()->Unlock();
	}

	
	UserID *new_user;
	
	if (userID) // edit existing
	{
		new_user = userID;
		new_user->SetRoomNick(_room_nick->Text());
		userID = NULL;
	} else // create a new user
	{
		new_user = new UserID(UserID(username));

		if (!strcasecmp(_handle->Label(), "JID:"))
		{
			new_user->SetUsertype(UserID::JABBER);
		} else {
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
	
	// close window explicitly
	PostMessage(B_QUIT_REQUESTED);
}
