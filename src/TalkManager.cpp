//////////////////////////////////////////////////
// Blabber [TalkManager.cpp]
//////////////////////////////////////////////////

#include <interface/Window.h>
#include "Settings.h"
#include "ChatWindow.h"
#include "JRoster.h"
#include "MessageRepeater.h"
#include "Messages.h"
#include "ModalAlertFactory.h"
#include "TalkManager.h"

TalkManager *TalkManager::_instance = NULL;

TalkManager *TalkManager::Instance() {
	if (_instance == NULL) {
		_instance = new TalkManager();
	}

	return _instance;
}

TalkManager::TalkManager()
{
	_windows_map_lock = create_sem(1, "TalkManager lock"); 
}

TalkManager::~TalkManager()
{
	for (TalkIter i = _talk_map.begin(); i != _talk_map.end(); ++i) {
		i->second->PostMessage(B_QUIT_REQUESTED);
	}

	_instance = NULL;
	delete_sem(_windows_map_lock);
}

ChatWindow *TalkManager::CreateTalkSession(ChatWindow::talk_type type, UserID *user)
{
	ChatWindow *window = FindWindow(user->JabberHandle());
	
	if (window)
	{
		window->Lock();
		window->Activate();
		window->Unlock();
	} 
	else
	{
		window = new ChatWindow(type, user);
		window->jabber = jabber;
		_talk_map[user->JabberHandle()] = window;
		
		if (type == ChatWindow::GROUP)
		{
			jabber->JoinRoom(BString((user->JabberHandle() + "/" +  user->_room_nick).c_str()), "");
		}
	}
	
	return window;
}

void TalkManager::ProcessMessageData(XMLEntity *entity)
{
	ChatWindow::talk_type type;
	string                thread_id;
	string                sender;
	string				  receiver;

	string                group_room;
	string                group_server;
	string                group_identity;
	string                group_username;
	
	// must be sender to continue
	if (!entity->Attribute("from"))
	{
		fprintf(stderr, "From is unspecified. Return.\n");
		return;
	}

	// determine message type
	if (!entity->Attribute("type"))
	{
		if (entity->Child("x", "xmlns", "jabber:x:oob"))
			type = ChatWindow::GROUP;
		else
		{
			fprintf(stderr, "Type is unspecified. Return.\n");
			return;
		}
	}
	else if (!strcasecmp(entity->Attribute("type"), "normal"))
	{
		type = ChatWindow::CHAT;
	}
	else if (!strcasecmp(entity->Attribute("type"), "chat"))
	{
		type = ChatWindow::CHAT;
	}
	else if (!strcasecmp(entity->Attribute("type"), "groupchat"))
	{
		type = ChatWindow::GROUP;
	}
	else if (!strcasecmp(entity->Attribute("type"), "error"))
	{
		char buffer[2048];
		
		if (entity->Child("error")) {
			const char *text = entity->Child("error")->Child("text") ? entity->Child("error")->Child("text")->Data() : "Unknown";
			sprintf(buffer, "An error occurred when you tried sending a message to %s.  The reason is as follows:\n\n%s", entity->Attribute("from"), text);
			ModalAlertFactory::Alert(buffer, "Oh, well.", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT);
		}
		
		//delete entity;
		
		return;
	}
	else if (!strcasecmp(entity->Attribute("type"), "headline"))
	{
		fprintf(stderr, "Tune message.\n");
		return;
	}
	else
	{
		type = ChatWindow::CHAT;
	}
	
	// configure routing information
	sender = string(entity->Attribute("from"));
	receiver = string(entity->Attribute("to"));
	group_username = UserID(sender).JabberResource();
	group_room = UserID(sender).JabberUsername();
	UserID sender_user(sender);

	ChatWindow *window = FindWindow(sender_user.JabberHandle());
		
	if (!window)
	{
		JRoster::Instance()->Lock();
			
			  
		UserID *user = JRoster::Instance()->FindUser(&sender_user);
		
		if (!user)
		{
			user = new UserID(sender_user);
			
			fprintf(stderr, "Username: %s.\n", user->JabberUsername().c_str());
			fprintf(stderr, "Server: %s.\n", user->JabberServer().c_str());
			fprintf(stderr, "Resource: %s.\n", user->JabberResource().c_str());
			fprintf(stderr, "Valid?: %s.\n", user->WhyNotValidJabberHandle().c_str());
			if (user->JabberServer().empty())
			{
				user->SetFriendlyName(string("Server Chat"));
				user->SetJabberServer(sender);
				fprintf(stderr, "Username: %s.\n", user->JabberUsername().c_str());
				fprintf(stderr, "Server: %s.\n", user->JabberServer().c_str());
				fprintf(stderr, "Resource: %s.\n", user->JabberResource().c_str());
			}
			fprintf(stderr, "Not found incoming message user in roster.\n");
		}

		JRoster::Instance()->Unlock();
			
		if (type==ChatWindow::CHAT /*|| type==ChatWindow::GROUP*/ )
		{
			window = CreateTalkSession(type, user);
			fprintf(stderr, "Create Window for incoming message.\n");
			window->jabber = jabber;
		} else {
			fprintf(stderr, "Unexisted Groupchat Window. No route\n");
			return;
		}
	}
	else
	{
		fprintf(stderr, "Redirected to Existed Window: %s.\n", sender_user.JabberHandle().c_str());
	}
				
	fprintf(stderr, "Lock. ");
	window->Lock();
	
	//fprintf(stderr, "Activate. ");
	//window->Activate();
	
		
	string body;
	string subject;
		
	if (entity->Child("body") && entity->Child("body")->Data())
		body = string(entity->Child("body")->Data());
		
	if (entity->Child("subject") && entity->Child("subject")->Data())
		subject = string(entity->Child("subject")->Data());
				
	if (type == ChatWindow::CHAT && !body.empty())
	{
		window->NewMessage(body);
	}
	else if (type == ChatWindow::GROUP && (!body.empty() || !subject.empty()))
	{
		//Accept exectly our JID in destinations 
		if (UserID(receiver).JabberHandle() != UserID(jabber->jid.String()).JabberHandle())
		{
			window->Unlock();
			fprintf(stderr, "Unlock.\n");
			delete entity;
			return;
		}
		
		if (group_username == "")
			window->AddToTalk(group_room, body, ChatWindow::MAIN_RECIPIENT); // channel messages
		else
		{
			if (!subject.empty())
			{
				window->AddToTalk(group_username, subject, ChatWindow::TOPIC); // topic messages
				fprintf(stderr, "Topic message.\n");
			}
			else			
				window->AddToTalk(group_username, body, ChatWindow::MAIN_RECIPIENT); // user messages
		}
	}
		
	window->Unlock();
	fprintf(stderr, "Unlock.\n");
	
}

ChatWindow* TalkManager::FindWindow(string username) {
	// check handles (with resource)
	int j = 0;
	for (TalkIter i = _talk_map.begin(); i != _talk_map.end(); i++) {
		//fprintf(stderr, "iter %i: %s\n", j++, (*i).second->GetUserID()->JabberHandle().c_str());
		if ((*i).second->GetUserID()->JabberHandle() == UserID(username).JabberHandle()) {
			return (*i).second;
		}
	}

	// no matches
	return NULL;
}

void TalkManager::UpdateWindowTitles(const UserID *user) {
	// check handles (without resource)
	for (TalkIter i = _talk_map.begin(); i != _talk_map.end(); ++i) {
		if ((*i).second->GetUserID()->JabberHandle() == user->JabberHandle()) {
			(*i).second->SetTitle(user->FriendlyName().c_str());
		}
	}
}

void TalkManager::RemoveWindow(string username) {
	if (_talk_map.count(username) > 0) {
		_talk_map.erase(username);
	}
}

void TalkManager::RotateToNextWindow(ChatWindow *current, rotation direction) {
	// no chat windows
	if (_talk_map.size() == 0) {
		return;
	}

	// from chat windows
	if (_talk_map.size() == 1 && current != NULL) {
		return;
	}

	// remember first and last, we may need them later
	ChatWindow *first = (*_talk_map.begin()).second;
	ChatWindow *last  = (*_talk_map.rbegin()).second;
		
	// from non-chat windows
	if (current == NULL) {
		if (direction == ROTATE_FORWARD) {
			first->Activate();
		} else {
			last->Activate();
		}
		
		return;
	}
	
	// iterate and find the current window
	ChatWindow *previous = NULL;
	for (TalkIter i = _talk_map.begin(); i != _talk_map.end(); ++i) {
		if ((*i).second == current) {
			// we found our window, now check bordering windows
			if (direction == ROTATE_FORWARD) {
				if (++i != _talk_map.end()) {
					(*i).second->Activate();
				} else {
					first->Activate();
				}
			} else {
				if (previous) {
					previous->Activate();
				} else {
					last->Activate();
				}
			}

			break;
		} else {
			previous = (*i).second;
		}
	}
}

void
TalkManager::Lock()
{
	acquire_sem(_windows_map_lock);
}

void
TalkManager::Unlock()
{
	release_sem(_windows_map_lock);
}

void TalkManager::Reset() {
	MessageRepeater::Instance()->PostMessage(JAB_CLOSE_TALKS);
	_talk_map.clear();
}
