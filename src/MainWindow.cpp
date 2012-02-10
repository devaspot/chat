//////////////////////////////////////////////////
// Haiku Chat [BlabberMainWindow.cpp]
//////////////////////////////////////////////////

#include <InterfaceKit.h>
#include <cstdio>
#include <app/Application.h>
#include <be_apps/Deskbar/Deskbar.h>
#include <be_apps/NetPositive/NetPositive.h>
#include <interface/MenuBar.h>
#include <interface/MenuItem.h>
#include <interface/ScrollView.h>
#include <String.h>
#include <Roster.h>
#include <Path.h>
#include <FindDirectory.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "MainWindow.h"
#include "BuddyWindow.h"
#include "AppLocation.h"
#include "Messages.h"
#include "ChatWindow.h"
#include "JabberProtocol.h"
#include "Settings.h"
#include "GenericFunctions.h"
#include "MessageRepeater.h"
#include "RosterItem.h"
#include "ModalAlertFactory.h"
#include "TalkManager.h"
#include "JRoster.h"

#define SSL_ENABLED	'ssl3'

BlabberMainWindow *BlabberMainWindow::_instance = NULL;

BlabberMainWindow *BlabberMainWindow::Instance() {
	BlabberSettings *settings = BlabberSettings::Instance();
	
	if (_instance == NULL && !settings->Data("no-window-on-startup")) 
	{
		float main_window_width, main_window_height;

		// determine what the width and height of the window should be
		if (settings->Data("main-window-width") && settings->Data("main-window-height"))
		{
			main_window_width  = atof(settings->Data("main-window-width"));
			main_window_height = atof(settings->Data("main-window-height"));
		} 
		else {
			// default
			main_window_width  = 210;
			main_window_height = 332; 
		}
		
		// create window frame position
		BRect frame(GenericFunctions::CenteredFrame(main_window_width, main_window_height));

		// poition window to last known position
		if (settings->Data("main-window-left") && settings->Data("main-window-top"))
		{
			frame.OffsetTo(BPoint(atof(settings->Data("main-window-left")), atof(settings->Data("main-window-top"))));
		}

		// create window singleton
		_instance = new BlabberMainWindow(frame);
	}
	
	return _instance;
}

BlabberMainWindow::~BlabberMainWindow() {
	// remove self from message family
	MessageRepeater::Instance()->RemoveTarget(this);

	// remove deskbar icon
//	BDeskbar db;
//	db.RemoveItem(_deskbar_id);	

	_instance = NULL;
}

void BlabberMainWindow::MessageReceived(BMessage *msg) {
	static bool reported_info = false;
	
	switch (msg->what) {

		case JAB_CONNECT:
		case JAB_LOGIN: {
			// must pass validation
			if (!ValidateLogin()) {
				break;
			}

			// switch out views
			HideLogin();
			
			UserID username(_login_username->Text());
			
			if (username.JabberServer() == "gmail.com" || 
				username.JabberServer() == "googlemail.com")
			{
				jabber->SetConnection(BString("talk.google.com"), 443, true);
			} else
			{
				jabber->SetConnection(BString(username.JabberServer().c_str()), 5223, true);
			}
			
			/*
			if (_ssl_enabled->Value() == B_CONTROL_ON)
			{
				jabber->SetConnection(BString(_ssl_server->Text()), atoi(_ssl_port->Text()), true);
			}
			else
			{
				if (username.JabberServer() != "gmail.com")
					jabber->SetConnection(BString(username.JabberServer().c_str()), 5222, false);
				else
					jabber->SetConnection(BString("talk.google.com"), 5222, false);
			}
			*/
				
			jabber->SetCredentials(BString(username.JabberUsername().c_str()), 
							BString(username.JabberServer().c_str()), BString(_login_password->Text()));
							
			jabber->LogOn();
			
			break;
		}

		case JAB_LOGGED_IN: 
		{
			SetTitle((string("Chat − ") + UserID(string(jabber->jid.String())).JabberHandle()).c_str());
			
			
			_status_view->SetMessage("gathering agents, roster and presence info");
						
			jabber->RequestRoster();
			jabber->SendStorageRequest("storage", "storage:bookmarks");
			
			utsname uname_info;
			if (uname(&uname_info) == 0) {
				char buffer[1000];
				string os_info = uname_info.sysname;
				long revision = 0;
				if (sscanf(uname_info.version, "r%ld", &revision) == 1) {
					char version[16];
					snprintf(version, sizeof(version), "%ld", revision);
					os_info += " (Revision: ";
					os_info += version;
					os_info += ")";
				}
				jabber->SetStatus("online", os_info.c_str());
			}
				
			//BlabberSettings::Instance()->SetData("last-realname", _login_realname->Text());
			BlabberSettings::Instance()->SetData("last-login", _login_username->Text());
			BlabberSettings::Instance()->SetData("last-password", _login_password->Text());
			//BlabberSettings::Instance()->SetData("last-ssl_server", _ssl_server->Text());
			//BlabberSettings::Instance()->SetData("last-ssl_port", _ssl_port->Text());
			//BlabberSettings::Instance()->SetIntData("last-ssl_enabled", _ssl_enabled->Value());
			BlabberSettings::Instance()->SetTag("auto-login", _login_auto_login->Value());
			BlabberSettings::Instance()->WriteToFile();
			
			

			break;
		}
		
		case JAB_DISCONNECT:
		{
			TalkManager::Instance()->Reset();
			
			SetTitle("Chat");
			jabber->_storage_supported = true;
			Lock();
			_status_view->SetMessage("disconnect");
			Unlock();
			jabber->Disconnect();
			
			break;
		}

		case BLAB_UPDATE_ROSTER:
		{
			// a message that the roster singleton was updated
			Lock();
			_status_view->SetMessage("roster updated.");
			
			_roster->UpdateRoster();
			
			Unlock();
			break;
		}
		
		
		case JAB_OPEN_CHAT_WITH_DOUBLE_CLICK:
		case JAB_OPEN_CHAT:
		{
			// if there's a current selection, begin chat with that user
			Lock();
			RosterItem *item = _roster->CurrentItemSelection();
			
			if (item != NULL) {
				
				TalkManager::Instance()->Lock();
				
				UserID *user = (UserID*)item->GetUserID();
				if (user->UserType() == UserID::CONFERENCE)
				{
					if (!jabber->_storage_supported)
						user->_room_nick = string(BString("__").Append(jabber->user).String());
				
					ChatWindow *window = TalkManager::Instance()->CreateTalkSession(ChatWindow::GROUP, user);
				}
				else
					ChatWindow *window = TalkManager::Instance()->CreateTalkSession(ChatWindow::CHAT, user);
						
				TalkManager::Instance()->Unlock();

			}
			Unlock();
			break;
		}
		
		case JAB_SUBSCRIBE_PRESENCE:
		{
			Lock();
			RosterItem *item = _roster->CurrentItemSelection();
			
			if (item != NULL) {
				UserID *user = (UserID*)item->GetUserID();
				jabber->SendSubscriptionRequest(user->JabberHandle());
			}
			Unlock();
			_roster->UpdateRoster();
			break;
		}
		
		case JAB_UNSUBSCRIBE_PRESENCE:
		{
			Lock();
			fprintf(stderr, "Sending unsubscribe...");
			RosterItem *item = _roster->CurrentItemSelection();
			
			if (item != NULL) {
				UserID *user = (UserID*)item->GetUserID();
				jabber->SendUnsubscriptionRequest(user->JabberHandle());
				fprintf(stderr, " for user %s. Done.\n", user->JabberHandle().c_str());
			
			}
			Unlock();
			_roster->UpdateRoster();
			break;
		}
		
		case JAB_REVOKE_PRESENCE:
		{
			Lock();
			RosterItem *item = _roster->CurrentItemSelection();
			
			if (item != NULL) {
				UserID *user = (UserID*)item->GetUserID();
				jabber->RejectPresence(user->JabberHandle());
			}
			Unlock();
			_roster->UpdateRoster();
			break;
		}
		
		case JAB_RESEND_PRESENCE:
		{
			Lock();
			RosterItem *item = _roster->CurrentItemSelection();
			
			if (item != NULL) {
				UserID *user = (UserID*)item->GetUserID();
				jabber->AcceptPresence(user->JabberHandle());
			}
			Unlock();
			_roster->UpdateRoster();
			break;
		}
		case JAB_OPEN_ADD_BUDDY_WINDOW:
		{
			Lock();
			BuddyWindow::Instance()->SetUser(NULL);
			BuddyWindow::Instance()->Show();
			Unlock();
			break;
		}
		
		case JAB_OPEN_EDIT_BUDDY_WINDOW:
		{
			Lock();
			RosterItem *item = _roster->CurrentItemSelection();
			if (item != NULL) {
				UserID *user = const_cast<UserID *>(item->GetUserID());
				BuddyWindow::Instance()->SetUser(user);
				BuddyWindow::Instance()->Show();
			}
			Unlock();
			break;
		}
		
		case JAB_REMOVE_BUDDY:
		{
			Lock();
			RosterItem *item = _roster->CurrentItemSelection();
			UserID *user = NULL;
			
			if (item != NULL)
			{
				user = (UserID *)item->GetUserID();
				
				if (user->UserType() == UserID::CONFERENCE && jabber->_storage_supported)
				{
					JRoster::Instance()->Lock();
					JRoster::Instance()->RemoveUser(user);
					JRoster::Instance()->Unlock();
					_roster->RemoveItem(item);
					_roster->Invalidate();
					jabber->SaveConference(NULL);
					jabber->SendStorageRequest("storage", "storage:bookmarks");
				}
				else
					jabber->RemoveFromRoster(user);
				
			}
			Unlock();
			break;
		}
		
		case SSL_ENABLED:
		{
			//_ssl_port->SetEnabled(_ssl_enabled->Value());
			//_ssl_server->SetEnabled(_ssl_enabled->Value());
			break;
		}
		
		case B_ABOUT_REQUESTED:
		{
			be_app->PostMessage(B_ABOUT_REQUESTED);
			break;
		}
		
		case JAB_QUIT:
		{
			be_app->PostMessage(B_QUIT_REQUESTED);
			break;
		}
		
		default:
		{
			BWindow::MessageReceived(msg);
		}
		
	}
}

void BlabberMainWindow::MenusBeginning() {
	char buffer[1024];

	// FILE menu
	if (!_full_view->IsHidden()) {
		_connect_item->SetEnabled(false);
		_disconnect_item->SetEnabled(true);
	} else {
		_connect_item->SetEnabled(true);
		_disconnect_item->SetEnabled(false);
	}
/*
	// EDIT menu
	if (RosterItem *item = _roster->CurrentItemSelection()) {
		// if a  item is selected
		sprintf(buffer, "Edit %s", item->GetUserID()->FriendlyName().c_str());
		_change_buddy_item->SetLabel(buffer);
		_change_buddy_item->SetEnabled(true);

		sprintf(buffer, "Remove %s", item->GetUserID()->FriendlyName().c_str());
		_remove_buddy_item->SetLabel(buffer);
		_remove_buddy_item->SetEnabled(true);

		_user_info_item->SetEnabled(true);
		_user_chatlog_item->SetEnabled(BlabberSettings::Instance()->Tag("autoopen-chatlog"));
	} else {		
		sprintf(buffer, "Edit Buddy");
		_change_buddy_item->SetLabel(buffer);
		_change_buddy_item->SetEnabled(false);

		sprintf(buffer, "Remove Buddy");
		_remove_buddy_item->SetLabel(buffer);
		_remove_buddy_item->SetEnabled(false);

		_user_info_item->SetEnabled(false);
		_user_chatlog_item->SetEnabled(false);
	}
*/
}

bool BlabberMainWindow::QuitRequested() {
	
	// remember last coordinates
	BlabberSettings::Instance()->SetFloatData("main-window-left", Frame().left);
	BlabberSettings::Instance()->SetFloatData("main-window-top", Frame().top);
	BlabberSettings::Instance()->SetFloatData("main-window-width", Bounds().Width());
	BlabberSettings::Instance()->SetFloatData("main-window-height", Bounds().Height());
	
	// save login settings
	//BlabberSettings::Instance()->SetData("last-realname", _login_realname->Text());
	BlabberSettings::Instance()->SetData("last-login", _login_username->Text());
	BlabberSettings::Instance()->SetData("last-password", _login_password->Text());
	//BlabberSettings::Instance()->SetData("last-ssl_server", _ssl_server->Text());
	//BlabberSettings::Instance()->SetData("last-ssl_port", _ssl_port->Text());
	//BlabberSettings::Instance()->SetIntData("last-ssl_enabled", _ssl_enabled->Value());
	BlabberSettings::Instance()->SetTag("auto-login", _login_auto_login->Value());
			
	BlabberSettings::Instance()->WriteToFile();

	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

BlabberMainWindow::BlabberMainWindow(BRect frame)
	: BWindow(frame, "Chat", B_DOCUMENT_WINDOW, B_ASYNCHRONOUS_CONTROLS) {

	// editing filter for taksing
//	AddCommonFilter(new RotateChatFilter(NULL));
	
	// add deskbar icon
//	BDeskbar     db;
//	DeskbarIcon *new_entry = new DeskbarIcon();

//	db.AddItem(new_entry, &_deskbar_id);
//	new_entry->SetMyID(_deskbar_id);
	
	// add self to message family
	MessageRepeater::Instance()->AddTarget(this);

	// set size constraints
	SetSizeLimits(300, 3000, 300, 3000);

	BRect rect;

	// encompassing view
	rect = Bounds();
	rect.OffsetTo(B_ORIGIN);
	
	_full_view = new BView(rect, "main-full", B_FOLLOW_ALL, B_WILL_DRAW);
	_full_view->SetViewColor(216, 216, 216, 255);

	// status bar
	_status_view = new StatusView();
	_status_view->SetViewColor(216, 216, 216, 255);
	_status_view->SetLowColor(216, 216, 216, 255);
	
	// menubar
	rect = Bounds();
	rect.bottom = rect.top + 18;

	_menubar = new BMenuBar(rect, "menubar");

	// FILE MENU
	_file_menu = new BMenu("File");

		_connect_item    = new BMenuItem("Log On", new BMessage(JAB_CONNECT));
		_connect_item->SetShortcut('N', 0);

		_disconnect_item = new BMenuItem("Sign-out", new BMessage(JAB_DISCONNECT));
		_disconnect_item->SetShortcut('B', 0);

		_about_item = new BMenuItem("About...", new BMessage(B_ABOUT_REQUESTED));
		_about_item->SetTarget(be_app);

		_quit_item = new BMenuItem("Quit", new BMessage(JAB_QUIT));
		_quit_item->SetShortcut('Q', 0);

//	_file_menu->AddItem(_connect_item);
	_file_menu->AddItem(_disconnect_item);
	_file_menu->AddSeparatorItem();
	_file_menu->AddItem(_about_item);
	_file_menu->AddSeparatorItem();
	_file_menu->AddItem(_quit_item);
	_file_menu->SetTargetForItems(MessageRepeater::Instance());

	// EDIT MENU
	_edit_menu = new BMenu("Roster");

		_add_buddy_item = new BMenuItem("Add Item...", new BMessage(JAB_OPEN_ADD_BUDDY_WINDOW));
		_add_buddy_item->SetShortcut('N', 0);

		_preferences_item = new BMenuItem("Preferences...", new BMessage(JAB_PREFERENCES));
		_preferences_item->SetEnabled(false);

	_edit_menu->AddItem(_add_buddy_item);
	_edit_menu->AddSeparatorItem();
	_edit_menu->AddItem(_preferences_item);
	_edit_menu->SetTargetForItems(this);

	_menubar->AddItem(_file_menu);
	_menubar->AddItem(_edit_menu);

	// tabbed view
	rect = Bounds();
	rect.top = _menubar->Bounds().bottom + 1 ;
	rect.bottom -= B_H_SCROLL_BAR_HEIGHT;

	// roster view
	rect.right  -= B_V_SCROLL_BAR_WIDTH;	

	_roster          = new RosterView(rect);
	_roster_scroller = new BScrollView(NULL, _roster, B_FOLLOW_ALL_SIDES, 0, false, true);
	_roster->TargetedByScrollView(_roster_scroller);

	// chat service
	rect.OffsetBy(7.0, _roster_scroller->Bounds().Height());
	rect.bottom = rect.top + 18;

	_full_view->AddChild(_status_view);
	_full_view->AddChild(_menubar);
	_full_view->AddChild(_roster_scroller);

	AddChild(_full_view);
	
	///// NOW DO LOGIN STUFF
	// encompassing view
	rect = Bounds();
	rect.OffsetTo(B_ORIGIN);

	_login_full_view = new BView(rect, "login-full", B_FOLLOW_ALL, B_WILL_DRAW);
	_login_full_view->SetViewColor(216, 216, 216, 255);

	// graphics
	//_login_bulb = new PictureView(AppLocation::Instance()->AbsolutePath("resources/graphics/jabber-title.png").c_str(), BPoint((Bounds().Width() - 189.0) / 2.0, 5.0), B_FOLLOW_H_CENTER);

	// username/password controls
	rect.InsetBy(5.0, 5.0);
	rect.top = 77.0;
	rect.right -= 3.0;
	
	
	_login_realname = new BTextControl(rect, NULL, "Title Name: ", NULL, NULL, B_FOLLOW_LEFT_RIGHT);
	
	rect.OffsetBy(0.0, 21.0); //fix this is too static!
	
	_login_username = new BTextControl(rect, NULL, "JID: ", NULL, NULL, B_FOLLOW_LEFT_RIGHT);
	_login_username->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
		
	rect.OffsetBy(0.0, 21.0); //fix this is too static!
	
	_login_password = new BTextControl(rect, NULL, "Password: ", NULL, NULL, B_FOLLOW_LEFT_RIGHT);
	_login_password->TextView()->HideTyping(true);
	_login_password->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	
	_login_realname->SetDivider(80);
	_login_username->SetDivider(80);
	_login_password->SetDivider(80);
	
	/*
	// SSL Box
	rect.OffsetBy(0.0, 21.0); //fix this is too static!
	BRect crect(rect);
	crect.bottom = crect.top + 100; //fix this is too static!
	
	_ssl_enabled = new BCheckBox(BRect(0,0,20,20), NULL, "SSL", new BMessage(SSL_ENABLED), B_FOLLOW_LEFT);
	_ssl_enabled->ResizeToPreferred();
	_ssl_enabled->SetValue(B_CONTROL_ON);
	
	BBox* _ssl_box=new BBox(crect,"box",B_FOLLOW_LEFT_RIGHT);
	_ssl_box->SetLabel(_ssl_enabled);
	
	BRect insideRect(_ssl_box->Bounds());
	
	insideRect.OffsetTo(2,_ssl_enabled->Frame().bottom + 2);
	insideRect.InsetBy(4, 2);
	BRect servRect(insideRect);	
	
	
	_ssl_server = new BTextControl(servRect, NULL, "Server: ", NULL, NULL, B_FOLLOW_LEFT_RIGHT);
	_ssl_server->ResizeToPreferred();
	_ssl_server->SetEnabled(true);
	
	servRect.OffsetBy(0,_ssl_server->Bounds().Height() + 1);
	_ssl_port = new BTextControl(servRect, NULL, "Port: ", NULL, NULL, B_FOLLOW_LEFT_RIGHT);
	_ssl_port->ResizeToPreferred();
	_ssl_port->SetEnabled(true);
	

	_ssl_server->SetDivider(74);
	_ssl_port->SetDivider(74);
	
	
	_ssl_box->ResizeTo(_ssl_box->Bounds().Width(), _ssl_port->Frame().bottom + 10.0);
	
	_ssl_box->AddChild(_ssl_server);
	_ssl_box->AddChild(_ssl_port);

	
	//end SSL Box
*/	

	//rect.top = _ssl_box->Frame().bottom + 10.0; //crect.bottom;
	rect.OffsetBy(80.0, 25.0);
	
	rect.right = rect.left + 175.0;
	rect.bottom = rect.top + 19.0;
	_login_new_account = new BCheckBox(rect, NULL, "Register Account", NULL, B_FOLLOW_LEFT);
	_login_new_account->SetEnabled(true);

	rect.OffsetBy(0.0, 19.0);
	_login_auto_login = new BCheckBox(rect, NULL, "Auto-login", NULL, B_FOLLOW_LEFT);
	_login_auto_login->SetEnabled(false);

	// login button
	//rect.OffsetTo((Bounds().Width() - 120.0) / 2.0, rect.top + 35.0);
	//rect.right = rect.left + 120.0;
	rect.OffsetTo(Bounds().Width() - 130.0, Bounds().Height() - 60.0);
	rect.right = rect.left + 100.0;

	_login_login = new BButton(rect, "login", "Sign-in", new BMessage(JAB_LOGIN), B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	_login_login->MakeDefault(false);
	_login_login->SetTarget(this);

	// new user notes
	rect.Set(0, rect.top + 32.0, Bounds().Width(), rect.top + 102.0); 
	rect.InsetBy(5.0, 0.0);

	//rgb_color note = {0, 0, 0, 255};
	BFont black_9(be_plain_font);
	black_9.SetSize(9.0);

	BRect text_rect(rect);
	text_rect.OffsetTo(B_ORIGIN);

	// login always hidden at start
	_login_full_view->Hide();

	// attach all-encompassing main view to window
	

	//_login_full_view->AddChild(_login_realname);
	_login_full_view->AddChild(_login_username);
	_login_full_view->AddChild(_login_password);
//	_login_full_view->AddChild(_ssl_box);
	_login_full_view->AddChild(_login_new_account);
	_login_full_view->AddChild(_login_auto_login);
	_login_full_view->AddChild(_login_login);
	
	AddChild(_login_full_view);
	
	/*
	// default
	if(BlabberSettings::Instance()->Data("last-realname")) {
		_login_realname->SetText(BlabberSettings::Instance()->Data("last-realname"));
	} else {
		_login_realname->SetText("Maxim Sokhatsky");
	}
	*/
	
	if (BlabberSettings::Instance()->Data("last-login")) {
		_login_username->SetText(BlabberSettings::Instance()->Data("last-login"));
	} else {
		_login_username->SetText("masique@gmail.com");
	}
	
	_login_password->SetText(BlabberSettings::Instance()->Data("last-password"));

	_login_auto_login->SetValue(BlabberSettings::Instance()->Tag("auto-login"));

	_login_password->MakeFocus(true);
	
	/*

	_ssl_server->SetText(BlabberSettings::Instance()->Data("last-ssl_server"));

	if (BlabberSettings::Instance()->Data("last-ssl_port"))
		_ssl_port->SetText(BlabberSettings::Instance()->Data("last-ssl_port"));
	else
		_ssl_port->SetText("5223");
		
	if (BlabberSettings::Instance()->Data("last-ssl_server"))
		_ssl_server->SetText(BlabberSettings::Instance()->Data("last-ssl_server"));
	else
		_ssl_server->SetText("talk.google.com");

	if (BlabberSettings::Instance()->Data("last-ssl_enabled"))
	{
		int enabled = atoi(BlabberSettings::Instance()->Data("last-ssl_enabled"));
		_ssl_enabled->SetValue(enabled);
		
		_ssl_port->SetEnabled(_ssl_enabled->Value());
		_ssl_server->SetEnabled(_ssl_enabled->Value());
	}
	*/

	ShowLogin();
	
}

bool BlabberMainWindow::ValidateLogin() {
	// existance of username
	if (!strcmp(_login_username->Text(), "")) {
		ModalAlertFactory::Alert("Wait, you haven't specified your Jabber ID yet.\n(e.g. me@jabber.org)", "Doh!", NULL, NULL, B_WIDTH_FROM_LABEL, B_STOP_ALERT);
		_login_username->MakeFocus(true);

		return false;
	}

	// validity of username
	UserID username(_login_username->Text());

	if (username.WhyNotValidJabberHandle().size()) {
		char buffer[1024];
		
		if (_login_new_account->Value())
			sprintf(buffer, "The Jabber ID you specified is not legal for the following reason:\n\n%s\n\nYou must specify a legal Jabber ID before you may create a new account.", username.WhyNotValidJabberHandle().c_str());
		else
			sprintf(buffer, "The Jabber ID you specified must not be yours because it's invalid for the following reason:\n\n%s\n\nIf you can't remember it, it's OK to create a new one by checking the \"Create a new Jabber Account!\" box.", username.WhyNotValidJabberHandle().c_str());

		ModalAlertFactory::Alert(buffer, "OK", NULL, NULL, B_WIDTH_FROM_LABEL, B_STOP_ALERT);
		_login_username->MakeFocus(true);

		return false;
	}	

	// 	existance of password
	if (!strcmp(_login_password->Text(), "")) {
		char buffer[1024];

		if (_login_new_account->Value())
			sprintf(buffer, "To create a new account, you must specify a password to protect it, %s.", username.Handle().c_str());
		else
			sprintf(buffer, "You must specify a password so I can make sure it's you, %s.", username.Handle().c_str());

		ModalAlertFactory::Alert(buffer, "Sorry!", NULL, NULL, B_WIDTH_FROM_LABEL, B_STOP_ALERT);
		_login_password->MakeFocus(true);

		return false;
	}

	/*
	int port = 0;
	if (_ssl_port->Text())
		port = atoi(_ssl_port->Text());
	
	if (_ssl_enabled->Value() && (!strcmp(_ssl_server->Text(), "") || port <=0 ) )
	{
		ModalAlertFactory::Alert("You enabled SSL. Please specify a valid server name and port.", "Sorry!", NULL, NULL, B_WIDTH_FROM_LABEL, B_STOP_ALERT);
		return false;		
	}
	*/

	return true;
}

void BlabberMainWindow::ShowLogin() {
	SetSizeLimits(300, 3000, 300, 3000);
	_login_login->MakeDefault(true);
	_full_view->Hide();
	_full_view->Hide();
	_login_full_view->Show();
	_login_full_view->Show();
	_login_new_account->SetValue(B_CONTROL_OFF);
}

void BlabberMainWindow::HideLogin()
{
	SetSizeLimits(100, 3000, 200, 3000);
	_login_login->MakeDefault(false);
	_full_view->Show();
	_full_view->Show();
	_login_full_view->Hide();
	_login_full_view->Hide();
}

void BlabberMainWindow::SetCustomStatus(string status) {
	char buffer[2048];
	
	// create menued status message
	sprintf(buffer, "%s", status.c_str()); 

	_custom_item->SetMarked(true);
	_custom_item->SetLabel(buffer);
}
