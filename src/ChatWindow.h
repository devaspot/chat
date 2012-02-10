#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <Window.h>
#include <MenuBar.h>
#include "StatusView.h"
#include "SplitView.h"
#include "MainWindow.h"
#include "JabberProtocol.h"
#include "UserID.h"
#include <cstdio>
#include <deque>
#include <string>
#include <storage/FilePanel.h>

class ChatWindow : public BWindow
{
public:
enum        talk_type {MESSAGE, CHAT, GROUP};
enum        user_type {MAIN_RECIPIENT, LOCAL, OTHER, TOPIC};
			
						ChatWindow(talk_type type, UserID *user);
						~ChatWindow();
							
			void		MessageReceived(BMessage *msg);
			bool		QuitRequested(void);
			void		FrameResized(float width, float height);
			void		AddToTalk(string username, string message, user_type type);
			void		NewMessage(string new_message);
			void		NewMessage(string username, string new_message);
			string		GetGroupUsername();
			string		GetGroupRoom();
			void 		AddGroupChatter(string user, string show, string status, string role, string affiliation);
			void		RemoveGroupChatter(string username);
			int 		CountHyperlinks(string message);
			void 		GenerateHyperlinkText(string message, text_run standard, text_run_array **tra);
			
			
	const	UserID		*GetUserID();
			void		SetThreadID(string id);

			JabberProtocol *jabber;
						
private:
	//std::deque<std::string>  _chat_history;
	std::string              _chat_buffer;
	int                 _chat_index;
	
	BFilePanel         *_fp;
	bool                _am_logging;
	FILE               *_log;
	BString				originalWindowTitle;
			UserID      			*_user;
			std::string 			_group_room;
			std::string            _group_username;
			std::string 			last_username;
			UserID::online_status  _current_status;
			talk_type              _type;
			std::string            _thread;
			BListView          *_people;
	
			BMenuBar	*menu;
			BMenu              *_file_menu;
	BMenu              *_edit_menu;
	BMenuItem          *_preferences_item;
	BMenuItem          *_affiliations;
	BMenuItem          *_close_item;
	
			StatusView	*statusView;
			SplitPane	*chatView;
			SplitPane          *_split_group_people;
			BView		*messageView;
			BView		*historyView;
			BView		*mainView;
			BScrollView	*messageScroller;
			BScrollView	*historyScroller;
			BScrollView        *_scrolled_people_pane;
			BTextView	*historyTextView;
			BTextView	*messageTextView;

};

#endif
