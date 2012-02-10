//////////////////////////////////////////////////
// Haiku Chat [BlabberMainWindow.h]
//     Central information display.
//////////////////////////////////////////////////

#ifndef BLABBER_MAIN_WINDOW_H
#define BLABBER_MAIN_WINDOW_H

#include <InterfaceKit.h>
#include <interface/Button.h>
#include <interface/CheckBox.h>
#include <interface/MenuItem.h>
#include <interface/PopUpMenu.h>
#include <interface/TabView.h>
#include <interface/TextControl.h>
#include <interface/Window.h>

#include "JabberProtocol.h"
#include "RosterView.h"
#include "StatusView.h"

class BlabberMainWindow : public BWindow {
public:
	static BlabberMainWindow  *Instance();
	                          ~BlabberMainWindow();

	virtual void               MessageReceived(BMessage *msg);
	virtual void               MenusBeginning();
	virtual bool               QuitRequested();

	bool                       ValidateLogin();
	void                       ShowLogin();
	void                       HideLogin();
	void                       SetCustomStatus(std::string status);
	JabberProtocol				*jabber;
	BCheckBox                *_login_new_account;
	RosterView               *_roster;
		
protected:
	                           BlabberMainWindow(BRect frame);
		                           
private:
	// singleton
	static BlabberMainWindow *_instance;
	
	BView                    *_login_full_view;

	BTextControl             *_login_realname;
	BTextControl             *_login_username;
	BTextControl             *_login_password;

	BTextControl			 *_ssl_server;
	BTextControl			 *_ssl_port;


	BCheckBox                *_login_auto_login;
	BCheckBox				 *_ssl_enabled;
	
	BButton                  *_login_login;

	BMenuBar                 *_menubar;
	BMenu                    *_file_menu;
	BMenu                    *_edit_menu;
	BMenu                    *_status_menu;
	BMenu                    *_channel_menu;
	BMenu                    *_talk_menu;

	BMenu                    *_common_status_menu;
	BMenu                    *_help_menu;
	BMenuItem                *_connect_item;
	BMenuItem                *_disconnect_item;
	BMenuItem                *_about_item;
	BMenuItem                *_quit_item;
	BMenuItem                *_add_buddy_item;
	BMenuItem                *_change_buddy_item;
	BMenuItem                *_remove_buddy_item;
	BMenuItem                *_user_info_item;
	BMenuItem                *_user_chatlog_item;
	BMenuItem                *_chat_item;
	BMenuItem                *_away_item;
	BMenuItem                *_dnd_item;
	BMenuItem                *_xa_item;
	BMenuItem                *_school_item;
	BMenuItem                *_work_item;
	BMenuItem                *_lunch_item;
	BMenuItem                *_dinner_item;
	BMenuItem                *_sleep_item;
	BMenuItem                *_custom_item;

	BMenuItem                *_rotate_chat_forward_item;
	BMenuItem                *_rotate_chat_backward_item;
	BMenuItem                *_send_message_item;
	BMenuItem                *_send_chat_item;
	BMenuItem                *_send_groupchat_item;

	BMenuItem                *_preferences_item;
	BMenuItem                *_beos_user_item;
	BMenuItem                *_riv_item;
	BMenuItem                *_jabber_org_item;
	BMenuItem                *_jabber_central_org_item;
	BMenuItem                *_jabber_view_com_item;
	BMenuItem                *_user_guide_item;
	BMenuItem                *_faq_item;

	int32                     _deskbar_id;
	
	BView                    *_full_view;

	StatusView               *_status_view;
	
	BPopUpMenu               *_online_status_selection;
	BMenuField               *_online_status;

	
	BScrollView              *_roster_scroller;
};

#endif
