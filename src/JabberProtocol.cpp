/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */
 
#include "JabberProtocol.h"
#include "ChatWindow.h"
#include "GenericFunctions.h"
#include "ModalAlertFactory.h"
#include "MainWindow.h"
#include "MessageRepeater.h"
#include "TalkManager.h"
#include "Messages.h"
#include "Base64.h"
#include <OS.h>
#include <sys/utsname.h>
#include <Roster.h>
#include <unistd.h>
#include "version.h"

#define DEBUG 

static int32 SessionDispatcher(void *args);
static int zeroReceived = 0;
static int separatorReceived = 0;

JabberProtocol::JabberProtocol()
{
	_storage_supported = true;
	logged = create_sem(1, "logged");
	read_queue = create_sem(1, "read_queue");
	xml_reader_lock = create_sem(1, "xml_reader");
	reciever_thread = spawn_thread(SessionDispatcher, "xmpp_receiver", B_LOW_PRIORITY, this);
	socketAdapter = new SSLAdapter();
}

JabberProtocol::~JabberProtocol()
{
}

void
JabberProtocol::SetConnection(BString h, int p, bool s)
{
	host = h;
	port = p;
	secure = s;
}

void
JabberProtocol::SetCredentials(BString u, BString d, BString p)
{
	domain = d;
	user = u;
	pass = p;
}

void
JabberProtocol::LockXMLReader() {
	acquire_sem(xml_reader_lock);
}

void
JabberProtocol::UnlockXMLReader() {
	release_sem(xml_reader_lock);
}

void 
JabberProtocol::LogOn() 
{
	if (BeginSession())
	{
		acquire_sem(logged);
		resume_thread(reciever_thread);
	}
	else
	{
		char buffer[50 + host.Length()];
		sprintf(buffer, "Cannot connect to %s:%i.", host.String(), port);
		ModalAlertFactory::Alert(buffer, "Sorry", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT); 
	
		Disconnect();
	}
}

void
JabberProtocol::SetStatus(BString status, BString message)
{
	acquire_sem(logged);
	release_sem(logged);
	
	BString xml;
	xml << "<presence from='";
	xml = xml.Append(jid);
	xml << "'><show>";
	xml = xml.Append(status);
	xml << "</show><status>";
	xml = xml.Append(message);
	xml << "</status></presence>";
	
	socketAdapter->SendData(xml);
	
}

void
JabberProtocol::OnStartTag(XMLEntity *entity)
{
}

void
JabberProtocol::OnEndTag(XMLEntity *entity)
{
	OnTag(entity);
}

void
JabberProtocol::OnEndEntity(XMLEntity *entity)
{
}

void
JabberProtocol::OnTag(XMLEntity *entity)
{
	char buffer[4096];
	static int seen_streams = 0;

	if (entity->IsCompleted() && !strcasecmp(entity->Name(), "iq"))
	{
		
		if (entity->Child("query") && entity->Child("query")->Attribute("xmlns") &&
			!strcasecmp(entity->Child("query")->Attribute("xmlns"),"jabber:iq:roster"))
		{
			ParseRosterList(entity);
			return;
		}
		
		if (entity->Attribute("id") && entity->Attribute("type") &&
			!strcasecmp(entity->Attribute("type"), "result") &&
			!strcasecmp(entity->Attribute("id"), "sess_1"))
		{
			release_sem(logged);
		
			mainWindow->Lock();
			mainWindow->PostMessage(JAB_LOGGED_IN);
			mainWindow->Unlock();
			
			return;
		}
		
		if (entity->Attribute("id") && entity->Attribute("type") &&
			!strcasecmp(entity->Attribute("type"), "result") &&
			!strcasecmp(entity->Attribute("id"), "bind_0"))
		{
			jid = BString(entity->Child("bind")->Child("jid")->Data());

			fprintf(stderr, "JID: %s.\n", jid.String());

			Session();
			
			return;
		}
		
		if (entity->Attribute("type") && entity->Child("query") &&
			!strcasecmp(entity->Attribute("type"), "result") &&
			entity->Child("query", "xmlns", "jabber:iq:register"))
		{
			Authorize();
			
			return;
		}
		
		if (entity->Attribute("type") && entity->Attribute("id") &&
			!strcasecmp(entity->Attribute("type"), "result") &&
			!strcasecmp(entity->Attribute("id"), "request_room_info"))
		{
			BMessage msg(JAB_PREFERENCES_DATAFORM);
			msg.AddPointer("XMLEntity", entity);
			MessageRepeater::Instance()->PostMessage(&msg);
			
			return;
		}
		
		if (entity->Attribute("type") && entity->Attribute("id") &&
			!strcasecmp(entity->Attribute("type"), "error"))
		{
			if (!strcasecmp(entity->Attribute("id"), "storage_request"))
			{
				_storage_supported = false;
				return;
			}
			else if (!strcasecmp(entity->Attribute("id"), "save_conferences"))
			{
				_storage_supported = false;
				sprintf(buffer, "Storage XEP-0049 is not supported on server. Cannot save conferences.\n\nNext time will try save to roster.");
				ModalAlertFactory::Alert(buffer, "Pity", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT); 
				return;
			}
			else if (!strcasecmp(entity->Attribute("id"), "request_room_info"))
			{
				
				if (entity->Child("error") &&
					entity->Child("error")->Child("text") &&
					entity->Child("error")->Attribute("code"))
					sprintf(buffer, "Error %s:\n\n%s", entity->Child("error")->Attribute("code"),
							entity->Child("error")->Child("text")->Data());
				else
					sprintf(buffer, "Error %s", entity->Child("error")->Attribute("code"));
			
				ModalAlertFactory::Alert(buffer, "OK", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT); 
			
				return;
			}
				
			if (entity->Child("error") &&
				entity->Child("error")->Child("text") &&
				entity->Child("error")->Attribute("code"))
				sprintf(buffer, "Error %s:\n\n%s", entity->Child("error")->Attribute("code"),
					entity->Child("error")->Child("text")->Data());
			else
				sprintf(buffer, "Error %s", entity->Child("error")->Attribute("code"));
			
			ModalAlertFactory::Alert(buffer, "OK", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT); 
			
			Disconnect();
			
			return;
			
		}
		
		if (entity->Attribute("id") && entity->Child("query") && entity->Child("query")->Attribute("xmlns") &&
			entity->Attribute("type") &&
			!strcasecmp(entity->Attribute("id"), "storage_request") &&
			!strcasecmp(entity->Attribute("type"), "result") &&
			!strcasecmp(entity->Child("query")->Attribute("xmlns"), "jabber:iq:private"))
		{
			if (entity->Child("query")->Child("storage"))
				ParseStorage(entity->Child("query")->Child("storage"));
				
			return;
		}
		
		if (entity->Attribute("type") && !strcasecmp(entity->Attribute("type"), "get"))
		{
			BString iq_from;
			BString iq_id;   
			
			if (entity->Attribute("from")) {
				iq_from = BString(entity->Attribute("from"));
			}
			
			if (entity->Attribute("id")) {
				iq_id = BString(entity->Attribute("id"));
			}
			
			XMLEntity *query = entity->Child("query");
			if (query && query->Attribute("xmlns")) {
				if (!strcasecmp(query->Attribute("xmlns"), "jabber:iq:version")) {
					ProcessVersionRequest(iq_id, iq_from);
				}
			}
			
			query = entity->Child("ping");
			if (query && query->Attribute("xmlns")) {
				if (!strcasecmp(query->Attribute("xmlns"), "urn:xmpp:ping"))
				{
					Pong(iq_id, iq_from);
				}
			}
			
			return;
		}
		
		fprintf(stderr, "Unknown IQ message.\n");
		return;
	}
	
	if (entity->IsCompleted() && !strcasecmp(entity->Name(), "success"))
	{
		InitSession();
		
		return;
		
	}
	
	if (entity->IsCompleted() && !strcasecmp(entity->Name(), "presence"))
	{
		ProcessPresence(entity);
		
		return;
	}
	
	if (entity->IsCompleted() && !strcasecmp(entity->Name(), "stream:error")) {
		sprintf(buffer, "An stream error has occurred.");
		ModalAlertFactory::Alert(buffer, "Sorry", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT); 
		
		Disconnect();
		
		return;
	}
	
	if (entity->IsCompleted() && !strcasecmp(entity->Name(), "stream:features"))
	{
		mainWindow->Lock();
		int wantRegister = mainWindow->fNewAccount->Value();
		mainWindow->Unlock();
		
		if (wantRegister == B_CONTROL_ON)
		{
			if (entity->Child("register"))
			{
				mainWindow->Lock();
				mainWindow->fNewAccount->SetValue(B_CONTROL_OFF);
				mainWindow->Unlock();
				
				SendUserRegistration(user, pass, "haiku");
			} else
			{
				sprintf(buffer, "Registration not supported on this server.");
				ModalAlertFactory::Alert(buffer, "Sorry", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT); 
				
				Disconnect();
			}
		}
		else
		{
				 if (entity->Child("starttls"))     StartTLS();
			else if (entity->Child("mechanisms"))	Authorize();	
			else if (entity->Child("bind"))			Bind();
			else if (entity->Child("session"))		Session();
		}
			
		return;
		
	}
	
	if (entity->IsCompleted() && !strcasecmp(entity->Name(), "failure")) {
			 if (entity->Child("not-authorized") != NULL)	 sprintf(buffer, "Not authorized failure.");
		else if (entity->Child("invalid-mechanism") != NULL) sprintf(buffer, "Invalid mechanism failure.");
		else if (entity->Child("invalid-authzid") != NULL)	 sprintf(buffer, "Invalid authorization Id.");
		else 												 sprintf(buffer, "An failure occured.");
			
		ModalAlertFactory::Alert(buffer, "Sorry", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT); 
		
		Disconnect();
		
		return;
	}
	
	if (entity->IsCompleted() && !strcasecmp(entity->Name(), "stream:stream"))
	{
		++seen_streams;
		if (seen_streams % 2 == 1)
			Disconnect();
			
		return;
	}
	
	if (entity->IsCompleted() && !strcasecmp(entity->Name(), "proceed"))
	{
		secure = true;
		socketAdapter->StartTLS();
		InitSession();
		return;
	}
	
	if (entity->IsCompleted() && !strcasecmp(entity->Name(), "message"))
	{
		TalkManager::Instance()->ProcessMessageData(entity);
		return;
	}
	
	//delete entity;
}

void
JabberProtocol::SendUserRegistration(BString username, BString password, BString resource)
{
	XMLEntity   *entity_iq, *entity_query;
	char **atts_iq    = CreateAttributeMemory(4);
	char **atts_query = CreateAttributeMemory(2);

	// assemble attributes;
	strcpy(atts_iq[0], "id");

	strcpy(atts_iq[1], GenerateUniqueID().c_str());

	strcpy(atts_iq[2], "type");
	strcpy(atts_iq[3], "set");

	strcpy(atts_query[0], "xmlns");
	strcpy(atts_query[1], "jabber:iq:register");

	// construct XML tagset
	entity_iq    = new XMLEntity("iq", (const char **)atts_iq);
	entity_query = new XMLEntity("query", (const char **)atts_query);

	entity_iq->AddChild(entity_query);
	
	entity_query->AddChild("username", NULL, username.String());
	entity_query->AddChild("password", NULL, password.String());
	entity_query->AddChild("resource", NULL, resource.String());

	// send XML command
	char *str = entity_iq->ToString();
	socketAdapter->SendData(BString(str));
	free(str);

	DestroyAttributeMemory(atts_iq, 4);
	DestroyAttributeMemory(atts_query, 2);
	
	delete entity_iq;
}

void
JabberProtocol::Pong(BString id, BString from)
{
	BString xml = "<iq from='";
	xml = xml.Append(jid);
	xml << "' id='";
	xml = xml.Append(id);
	xml << "' to='";
	xml = xml.Append(from);
	xml << "' type='result'/>";

#ifdef DEBUG	
	fprintf(stderr, "Pong reply to %s.\n", from.String());
#endif
	
	socketAdapter->SendData(xml);
}

void
JabberProtocol::SaveConference(UserID *conference)
{
	BString xml ="<iq type='set' id='save_conferences'>"
		"<query xmlns='jabber:iq:private'>"
		"<storage xmlns='storage:bookmarks'>";
			
	int count = mainWindow->fRosterView->GetConferencesCount();
	bool seen = false;
	
	for (int i = 0; i < count; i++)
	{
		const UserID *conf = mainWindow->fRosterView->GetConference(i);
		
		if (conference && (conf->JabberHandle() == conference->JabberHandle()))
		{
			conf = (const UserID *)conference;
			seen = true;
		}
		
		xml << "<conference name='";
		xml = xml.Append(conf->FriendlyName().c_str());
		xml << "' autojoin='";
		xml = xml.Append(conf->Autojoin().c_str());
		xml << "' jid='";
		xml = xml.Append(conf->JabberHandle().c_str());
		xml << "'><nick>";
		xml = xml.Append(conf->_room_nick.c_str());
		xml << "</nick></conference>";
	
	}
	
	if (!seen && conference)
	{
		xml << "<conference name='";
		xml = xml.Append(conference->FriendlyName().c_str());
		xml << "' autojoin='";
		xml = xml.Append(conference->Autojoin().c_str());
		xml << "' jid='";
		xml = xml.Append(conference->JabberHandle().c_str());
		xml << "'><nick>";
		xml = xml.Append(conference->_room_nick.c_str());
		xml << "</nick></conference>";
	}
	
	xml << "</storage></query></iq>";
	
	socketAdapter->SendData(xml);
}

void
JabberProtocol::SendUnavailable(BString to, BString status)
{
	BString xml = "<presence to='";
	xml = xml.Append(to);
	xml << "' type='unavailable'>";
	if (status.Length() > 0)
	{
		xml << "<status>";
		xml = xml.Append(status);
		xml << "</status>";
	}
	xml << "</presence>";
	
	socketAdapter->SendData(xml);
}

void
JabberProtocol::SendAvailable(BString to, BString status)
{
	BString xml = "<presence to='";
	xml = xml.Append(to);
	xml << "' type='available'>";
	if (status.Length() > 0)
	{
		xml << "<status>";
		xml = xml.Append(status);
		xml << "</status>";
	}
	xml << "</presence>";
	
	socketAdapter->SendData(xml);
}
void
JabberProtocol::JoinRoom(BString to, BString password)
{
	BString xml = "<presence to='";
	xml = xml.Append(to);
	xml << "'><priority>5</priority>";
	xml << "<x xmlns='http://jabber.org/protocol/muc'>";
	if (password.Length() > 0)
	{
		xml << "<password>";
		xml = xml.Append(password);
		xml << "</password>";
	}
	xml << "</x></presence>";
	
	socketAdapter->SendData(xml);
}

void
JabberProtocol::ProcessVersionRequest(BString req_id, BString req_from)
{
	XMLEntity   *entity_iq, *entity_query;
	char **atts_iq    = CreateAttributeMemory(6);
	char **atts_query = CreateAttributeMemory(2);

	// assemble attributes;
	strcpy(atts_iq[0], "id");
	strcpy(atts_iq[1], req_id.String());

	strcpy(atts_iq[2], "to");
	strcpy(atts_iq[3], req_from.String());

	strcpy(atts_iq[4], "type");
	strcpy(atts_iq[5], "result");

	strcpy(atts_query[0], "xmlns");
	strcpy(atts_query[1], "jabber:iq:version");

	// construct XML tagset
	entity_iq    = new XMLEntity("iq", (const char **)atts_iq);
	entity_query = new XMLEntity("query", (const char **)atts_query);

	entity_iq->AddChild(entity_query);
	
	entity_query->AddChild("name", NULL, "Dengon");
	entity_query->AddChild("version", NULL, "1.0 (rev: "DENGON_SVNVERSION")");

	BString strVersion("Haiku");
	
	BString os_info;
	utsname uname_info;
	if (uname(&uname_info) == 0) {
		os_info = uname_info.sysname;
		long revision = 0;
		if (sscanf(uname_info.version, "r%ld", &revision) == 1) {
			char version[16];
			snprintf(version, sizeof(version), "%ld", revision);
			os_info += " (rev: ";
			os_info += version;
			os_info += ")";
		}
	}

	entity_query->AddChild("os", NULL, os_info.String());

	// send XML command
	char *str = entity_iq->ToString();
	socketAdapter->SendData(BString(str));
	free(str);

	DestroyAttributeMemory(atts_iq, 6);
	DestroyAttributeMemory(atts_query, 2);
	
	delete entity_iq;
}

void
JabberProtocol::SendGroupPresence(string _group_room, string _group_username)
{
	XMLEntity             *entity_presence;
	char **atts_presence = CreateAttributeMemory(4);

	// assemble group ID
	string group_presence = _group_room + "/" + _group_username;	
	
	// assemble attributes;
	strcpy(atts_presence[0], "to");
	strcpy(atts_presence[1], group_presence.c_str());
	
	strcpy(atts_presence[2], "from");
	strcpy(atts_presence[3], jid.String());

	// construct XML tagset
	entity_presence = new XMLEntity("presence", (const char **)atts_presence);
	
	// send XML command
	char *str = entity_presence->ToString();
	socketAdapter->SendData(BString(str));
	free(str);

	DestroyAttributeMemory(atts_presence, 2);
	
	delete entity_presence;
}

void
JabberProtocol::ProcessPresence(XMLEntity *entity)
{
	int num_matches = 0;
	string room, server, user;

	if (!entity->Attribute("from")) return;
		
	if (entity->Child("x", "xmlns", "http://jabber.org/protocol/muc#user"))
	{
		UserID from = UserID(string(entity->Attribute("from")));
		room = from.JabberUsername();
		server = from.JabberServer();
		user = from.JabberResource();
		fprintf(stderr, "Group Presence in room %s from user %s.\n", 
			from.JabberHandle().c_str(), user.c_str());
						
		BMessage *msg = new BMessage(JAB_GROUP_CHATTER_ONLINE);
		msg->AddString("room", (room + '@' + server).c_str());
		msg->AddString("server", server.c_str());
		msg->AddString("username", user.c_str());
			
		if (!entity->Attribute("type") || !strcasecmp(entity->Attribute("type"), "available"))
		{
			if (entity->Child("show") && entity->Child("show")->Data())
			{
				msg->AddString("show", entity->Child("show")->Data());
			} else
				msg->AddString("show", "online");
				if (entity->Child("status") && entity->Child("status")->Data())
			{
				msg->AddString("status", entity->Child("status")->Data());
			} else
				msg->AddString("status", "");
			
			if (entity->Child("x")->Child("item") &&
				entity->Child("x")->Child("item")->Attribute("role"))
				msg->AddString("role", entity->Child("x")->Child("item")->Attribute("role"));
			else
				msg->AddString("role", "admin");
			
			if (entity->Child("x")->Child("item") &&
				entity->Child("x")->Child("item")->Attribute("affiliation"))
				msg->AddString("affiliation", entity->Child("x")->Child("item")->Attribute("affiliation"));
			else
				msg->AddString("affiliation", "none");
	
			msg->what = JAB_GROUP_CHATTER_ONLINE;
		}
		else if (!strcasecmp(entity->Attribute("type"), "unavailable"))
		{
			msg->what = JAB_GROUP_CHATTER_OFFLINE;
		}
		
		ChatWindow *window = TalkManager::Instance()->FindWindow(from.JabberHandle());
		if (window != NULL) window->PostMessage(msg);
		
		return;
	}		

	UserID from(string(entity->Attribute("from")));
	fprintf(stderr, "Process not in roster presence %s.\n", from.JabberHandle().c_str());
	UserID* userId = mainWindow->fRosterView->fUsers[from.JabberHandle()];
	mainWindow->fRosterView->UnlinkUser(userId);
	ProcessUserPresence(userId, entity);
	mainWindow->fRosterView->LinkUser(userId);
		
}

char **
JabberProtocol::CreateAttributeMemory(int num_items)
{
	char **atts;
	
	atts = (char **)malloc((num_items + 2) * sizeof(char *));
	for (int i=0; i<num_items; ++i)
		atts[i] = (char *)malloc(96 * sizeof(char));
	
	atts[num_items] = NULL;
	atts[num_items+1] = NULL;
	
	return atts;
}

void
JabberProtocol::Disconnect()
{
	Reset();

	mainWindow->Lock();
	mainWindow->fRosterView->MakeEmpty();
	mainWindow->fRosterView->CreateRoots();
	mainWindow->fRosterView->fUsers.clear();
	mainWindow->Unlock();
	
	socketAdapter->Close();
	
	release_sem(logged);
	suspend_thread(reciever_thread);
}

void
JabberProtocol::DestroyAttributeMemory(char **atts, int num_items)
{
	for (int i=0; i<(num_items + 2); ++i) {
		free(atts[i]);
	}
	
	free(atts);
}

void
JabberProtocol::AcceptPresence(string username) {
	XMLEntity *entity;
	
	char **atts = CreateAttributeMemory(4);
	
	// assemble attributes
	strcpy(atts[0], "to");
	strcpy(atts[1], username.c_str());
	strcpy(atts[2], "type");
	strcpy(atts[3], "subscribed");

	entity = new XMLEntity("presence", (const char **)atts);

	// send XML command
	char *str = entity->ToString();
	socketAdapter->SendData(BString(str));
	free(str);
	
	DestroyAttributeMemory(atts, 4);
	delete entity;
}

void
JabberProtocol::RejectPresence(string username)
{
	XMLEntity *entity;
	
	char **atts = CreateAttributeMemory(4);
	
	// assemble attributes
	strcpy(atts[0], "to");
	strcpy(atts[1], username.c_str());
	strcpy(atts[2], "type");
	strcpy(atts[3], "unsubscribed");

	entity = new XMLEntity("presence", (const char **)atts);

	// send XML command
	char *str = entity->ToString();
	socketAdapter->SendData(BString(str));
	free(str);
	
	DestroyAttributeMemory(atts, 4);
	delete entity;
}

string
JabberProtocol::GenerateUniqueID()
{
	static long counter = 0;
	pid_t pid = getpid();
	time_t secs = time(NULL);
	++counter;
	char buffer[100];
	sprintf(buffer, "%lu:%lu:%lu", pid, secs, counter);
	return string(buffer);
}


void
JabberProtocol::SendSubscriptionRequest(string username)
{
	XMLEntity *entity;
	
	char **atts = CreateAttributeMemory(6);
	
	// assemble attributes
	strcpy(atts[0], "to");
	strcpy(atts[1], username.c_str());
	strcpy(atts[2], "type");
	strcpy(atts[3], "subscribe");
	strcpy(atts[4], "id");
	strcpy(atts[5], GenerateUniqueID().c_str());

	entity = new XMLEntity("presence", (const char **)atts);

	// log command
	//_iq_map[atts[5]] = LOGIN;
	
	// send XML command
	char *str = entity->ToString();
	socketAdapter->SendData(BString(str));
	free(str);
	
	DestroyAttributeMemory(atts, 6);
	delete entity;
}

void
JabberProtocol::SendUnsubscriptionRequest(string username)
{
	XMLEntity *entity;
	
	char **atts = CreateAttributeMemory(6);
	
	// assemble attributes
	strcpy(atts[0], "to");
	strcpy(atts[1], username.c_str());
	strcpy(atts[2], "type");
	strcpy(atts[3], "unsubscribe");
	strcpy(atts[4], "id");
	strcpy(atts[5], GenerateUniqueID().c_str());

	entity = new XMLEntity("presence", (const char **)atts);

	// send XML command
	char *str = entity->ToString();
	socketAdapter->SendData(BString(str));
	free(str);
	
	DestroyAttributeMemory(atts, 6);
	delete entity;
}

void
JabberProtocol::AddToRoster(UserID *new_user)
{
	BString xml = "<iq type='set'>"
		"<query xmlns='jabber:iq:roster'>"
		"<item jid='";
	xml.Append(new_user->Handle().c_str());
	xml << "' name='";
	xml.Append(new_user->FriendlyName().c_str());
	xml << "' subscription='to'>";
	
	if (new_user->UserType() == UserID::CONFERENCE)
	{
		xml << "<group>#Conference</group>";
	}
	
	xml << "</item></query></iq>";
	
	if (new_user->UserType() == UserID::CONFERENCE && _storage_supported)
		SaveConference(new_user);
	else
		socketAdapter->SendData(xml);
   
}

void
JabberProtocol::RemoveFromRoster(UserID *removed_user)
{
	XMLEntity *entity, *entity_query, *entity_item;

	char **atts       = CreateAttributeMemory(4);
	char **atts_query = CreateAttributeMemory(2);
	char **atts_item  = CreateAttributeMemory(6);
	
	strcpy(atts[0], "type");
	strcpy(atts[1], "set");
	strcpy(atts[2], "id");
	strcpy(atts[3], GenerateUniqueID().c_str());

	strcpy(atts_query[0], "xmlns");
	strcpy(atts_query[1], "jabber:iq:roster");

	strcpy(atts_item[0], "jid");
	strcpy(atts_item[1], removed_user->Handle().c_str());
	strcpy(atts_item[2], "name");
	strcpy(atts_item[3], removed_user->FriendlyName().c_str());
	strcpy(atts_item[4], "subscription");
	strcpy(atts_item[5], "remove");

	entity = new XMLEntity("iq", (const char **)atts);
	entity_query = new XMLEntity("query", (const char **)atts_query);
	entity_item = new XMLEntity("item", (const char **)atts_item);

	entity_query->AddChild(entity_item);
	entity->AddChild(entity_query);

	char *str = entity->ToString();
	socketAdapter->SendData(BString(str));
	free(str);
	
	DestroyAttributeMemory(atts, 4);
	DestroyAttributeMemory(atts_query, 2);
	DestroyAttributeMemory(atts_item, 6);
	
	delete entity;
}

void
JabberProtocol::ProcessUserPresence(UserID *user, XMLEntity *entity)
{
	char buffer[4096];
	
	// get best asker name
	const char *asker;
					
	if (user && user->FriendlyName().size() > 0) {
		// they have a friendly name
		asker = user->FriendlyName().c_str();
	} else if (entity->Attribute("from")) {
		// they have a JID
		asker = entity->Attribute("from");
	} else {
		// they have no identity (illegal case)
		asker = "<unknown>";
	}
	
	// get presence
	const char *availability = NULL;

	if (entity->Attribute("type")) {
		availability = entity->Attribute("type");
	} else {
		availability = "available";
	}
	
	
	
	// reflect presence
	if (user && !strcasecmp(availability, "unavailable"))
	{
		if (entity->Child("delay"))
			return;
		
		if (user->SubscriptionStatus() == "none")
		{
			user->SetOnlineStatus(UserID::UNKNOWN);
		}
		else
		{	
			if (user->OnlineStatus() == UserID::ONLINE)
			{
				user->SetOnlineStatus(UserID::OFFLINE);
			}
		}
		
		fprintf(stderr, "User %s is unavailable.\n", user->JabberHandle().c_str());
	}
	else if (user && !strcasecmp(availability, "available"))
	{
		user->SetOnlineStatus(UserID::ONLINE);
		
		fprintf(stderr, "User %s is available '%s'.\n", user->JabberHandle().c_str(),
																user->SubscriptionStatus().c_str());
	}
	else if (user && !strcasecmp(availability, "unsubscribe"))
	{
		if (user->SubscriptionStatus() == "none")
		{
			user->SetOnlineStatus(UserID::UNKNOWN);
		}
		
		sprintf(buffer, "User %s is unsubscribed from you.\n", user->JabberHandle().c_str());
		fprintf(stderr, buffer);
		ModalAlertFactory::NonModalAlert(buffer, "I feel so unloved.");
	}
	else if (user && !strcasecmp(availability, "unsubscribed"))
	{
		if (entity->Attribute("subscription"))
		{
			if (!strcasecmp(entity->Attribute("subscription"), "from"))
				user->SetOnlineStatus(UserID::OFFLINE);
			else if (!strcasecmp(entity->Attribute("subscription"), "none"))
				user->SetOnlineStatus(UserID::UNKNOWN);
		} else {
			if (user->SubscriptionStatus() == "none")
				user->SetOnlineStatus(UserID::UNKNOWN);
			else 
				user->SetOnlineStatus(UserID::OFFLINE);
		}
			
		sprintf(buffer, "User %s deny request or cancel subscription", user->JabberHandle().c_str());
		fprintf(stderr, buffer);
	}
	else if (user && !strcasecmp(availability, "subscribed"))
	{
		// http://tools.ietf.org/html/rfc3921
		// 8.2.  User Subscribes to Contact, paragraph 8
		
		if (user->SubscriptionStatus()=="none" || user->SubscriptionStatus()=="from")
			SendSubscriptionRequest(user->JabberHandle());
			
		user->SetOnlineStatus(UserID::ONLINE);
		
		sprintf(buffer, "Your subscription request was accepted by %s!", user->JabberHandle().c_str());
		ModalAlertFactory::Alert(buffer, "Hooray!");
	}
	else if (user && !strcasecmp(availability, "subscribe"))
	{
		
		sprintf(buffer, "User %s wants to subscribe to you.\n", user->JabberHandle().c_str());
		fprintf(stderr, buffer);

		// query for presence authorization (for users)
		int32 answer = 0;
				
		answer = ModalAlertFactory::Alert(buffer, "Deny", "Grant!");

		// send back the response
		if (answer == 1) {
			// presence is granted
			AcceptPresence(string(entity->Attribute("from")));
			SendSubscriptionRequest(user->JabberHandle());
		} else if (answer == 0) {
			// presence is denied
			RejectPresence(string(entity->Attribute("from")));
		}
	}
	else if (!strcasecmp(availability, "error"))
	{
		if (entity->Child("error") && entity->Child("error")->Child("text"))
		{
			string username(entity->Attribute("from"));
			sprintf(buffer, "Presence error from %s:\n\n%s.\n", username.c_str(),
				entity->Child("error")->Child("text")->Data());
			fprintf(stderr, buffer);
			
			ModalAlertFactory::NonModalAlert(buffer, "Sad");
		}
	}

	if (user && !strcasecmp(availability, "available"))
	{
		if (entity->Child("show") && entity->Child("show")->Data()) {
			user->SetExactOnlineStatus(string(entity->Child("show")->Data()));
		} else {
			user->SetExactOnlineStatus("chat");
			
		}	

		if (entity->Child("status") && entity->Child("status")->Data()) {
			user->SetMoreExactOnlineStatus(string(entity->Child("status")->Data()));
		} else
			user->SetMoreExactOnlineStatus("");
			
	}
	

}

void
JabberProtocol::ParseStorage(XMLEntity *storage)
{

	for (int i=0; i<storage->CountChildren(); ++i)
	{
		UserID user(string(storage->Child(i)->Attribute("jid")));
		UserID *roster_user = mainWindow->fRosterView->fUsers[user.JabberHandle()];
		
		if (roster_user)
		{
			fprintf(stderr, "Cnference %s already in roster. Remove it first.\n", user.JabberHandle().c_str());
		}
		else
		{
			fprintf(stderr, "Added conference %s to roster.\n", user.JabberHandle().c_str());
			roster_user = new UserID(user.JabberHandle());
		}
		
		roster_user->SetFriendlyName(string(storage->Child(i)->Attribute("name")));
		roster_user->SetOnlineStatus(UserID::CONF_STATUS);
		roster_user->SetUsertype(UserID::CONFERENCE);
		if (storage->Child(i)->Child("nick"))
			roster_user->SetRoomNick(storage->Child(i)->Child("nick")->Data());
			
		mainWindow->fRosterView->LinkUser(roster_user, false);
		mainWindow->fRosterView->fUsers[roster_user->JabberHandle()] = roster_user;

			
	}
	
}

void
JabberProtocol::ParseRosterList(XMLEntity *iq_roster_entity)
{
	XMLEntity *entity = iq_roster_entity;
	
	// go one level deep to query
	if (entity->Child("query")) {
		entity = entity->Child("query");
	} else {
		return;
	}
	
	for (int i=0; i<entity->CountChildren(); ++i)
	{

		// handle the item child
		if (!strcasecmp(entity->Child(i)->Name(), "item"))
		{
			if (!entity->Child(i)->Attribute("jid"))
			{
				continue;
			}
			
			

			// make a user
			UserID user(string(entity->Child(i)->Attribute("jid")));

			// set subscription status
			if (entity->Child(i)->Attribute("subscription"))
			{
				user.SetSubscriptionStatus(string(entity->Child(i)->Attribute("subscription")));
			}
			
			
			// set user type
			if (entity->Child(i)->Child("group") &&
				!strcasecmp(entity->Child(i)->Child("group")->Data(), "#Conference"))
			{
				user.SetUsertype(UserID::CONFERENCE);
				user.SetOnlineStatus(UserID::CONF_STATUS);
				fprintf(stderr, "Roster item %s (conference).\n", user.JabberHandle().c_str());
			}
			else
			{
			
				user.SetUsertype(UserID::JABBER);
				fprintf(stderr, "Roster item %s.\n", user.JabberHandle().c_str());
			}

			// set friendly name
			if (entity->Child(i)->Attribute("name")) {
				user.SetFriendlyName(string(entity->Child(i)->Attribute("name")));
			}
			
			

			UserID *roster_user = BlabberMainWindow::Instance()->fRosterView->
							fUsers[user.JabberHandle()];
			
			if (roster_user) 
			{
				// Roster item updating never changes Online Status
				// Online Status can only be changed by <presence> messages
			
				if (!user.SubscriptionStatus().empty())
				{
					if (user.SubscriptionStatus() == "remove")
					{
						//JRoster::Instance()->RemoveUser(roster_user);
						mainWindow->fRosterView->fUsers.erase(roster_user->JabberHandle());
												
						fprintf(stderr, "User %s was removed from roster.\n",
							roster_user->JabberHandle().c_str());
							
						continue;
					}
					else
					{
			
						if (user.SubscriptionStatus() == "from" && 
							(roster_user->OnlineStatus() == UserID::UNKNOWN))
						{
							roster_user->SetOnlineStatus(UserID::OFFLINE);
						}
						else if (user.SubscriptionStatus() == "none" &&
							roster_user->OnlineStatus() == UserID::OFFLINE)
						{
							roster_user->SetOnlineStatus(UserID::UNKNOWN);
						}
						
						fprintf(stderr, "User %s in roster just updated subscription from '%s' to '%s'.\n",
							roster_user->JabberHandle().c_str(),
							user.SubscriptionStatus().c_str(),
							roster_user->SubscriptionStatus().c_str());
					
						roster_user->SetSubscriptionStatus(user.SubscriptionStatus());
					}
				}
				
				if (!user.FriendlyName().empty())
					roster_user->SetFriendlyName(user.FriendlyName());

			}
			else
			{
				// Handle Roster Item from Server that is not in our Roster List
				// Determine OFFLINE or UNKNOWN nodes to initial placing
				
				if (user.UserType() == UserID::JABBER)
				{
					if (user.SubscriptionStatus() == "none")
						user.SetOnlineStatus(UserID::UNKNOWN);
					else 
						user.SetOnlineStatus(UserID::OFFLINE);
				}
					
				roster_user = new UserID(string(entity->Child(i)->Attribute("jid")));
				
				roster_user->SetSubscriptionStatus(user.SubscriptionStatus());
				roster_user->SetFriendlyName(user.FriendlyName());
				roster_user->SetOnlineStatus(user.OnlineStatus());
				roster_user->SetUsertype(user.UserType());
								
				mainWindow->fRosterView->LinkUser(roster_user, false);
				mainWindow->fRosterView->fUsers[roster_user->JabberHandle()] = roster_user;
				
				fprintf(stderr, "User %s was added to roster subscription='%s'.\n",
					roster_user->JabberHandle().c_str(), roster_user->SubscriptionStatus().c_str());
			}
			
			
			
		}
	}
	
	mainWindow->Lock();
	mainWindow->fStatusView->SetMessage("roster updated.");
	mainWindow->Unlock();
	
}

static int32 SessionDispatcher(void *args) 
{
	JabberProtocol *jabber = (JabberProtocol*)args;
	BMessage *msg = new BMessage(PORT_TALKER_DATA);
	
	while (true)
	{
		acquire_sem(jabber->read_queue);
			
		jabber->ReceiveData(msg);
		jabber->ReceivedMessageHandler(msg);
				
		release_sem(jabber->read_queue);
		
		snooze(1000);
	}
	
	return 0;
}

void
JabberProtocol::ReceiveData(BMessage *msg)
{
	BMessage packet;
	BString msgData;
	
	msg->MakeEmpty();
	
	bool found_stream_start = false;
	bool found_stream_end = false;
	
	do 
	{
		BString data;
		int32 length;
		
		packet.MakeEmpty();
		socketAdapter->ReceiveData(&packet);
	
		packet.FindString("data", &data);
		packet.FindInt32("length", &length);

		if (data.FindFirst("<stream:stream") >= 0)
			found_stream_start = true;
			
		if (data.FindFirst("</stream:stream") >= 0)
			found_stream_end = true;
			
		msgData.Append(data);
		
	} while (FXMLCheck(msgData.String()) == NULL && 
				!found_stream_start && !found_stream_end);

	// TODO: handle XML head more accurately
	//		 should be fixed in XMLReader base class
	
	msgData.RemoveFirst("<?xml version='1.0'?>")
		   .RemoveFirst("<?xml version='1.0' ?>").Append("</dengon>").Prepend("<dengon>");
					
	msg->AddInt32("length", msgData.Length());
	msg->AddString("data", msgData);
	
}

void 
JabberProtocol::RequestRoomOptions(BString room)
{
	BString xml = "<iq from='";
	xml = xml.Append(jid);
	xml << "' id='request_room_info' to='";
	xml = xml.Append(room);
	xml << "' type='get'><query xmlns='http://jabber.org/protocol/muc#owner'/></iq>";
	
	socketAdapter->SendData(xml);
}

void
JabberProtocol::ReceivedMessageHandler(BMessage *msg)
{
	BString data;
	msg->FindString("data", &data);
	int length = msg->FindInt32("length");
	
#ifdef DEBUG

		fprintf(stderr, "\nDATA received %i: %s\n", (int)data.Length(), data.String());

#endif

	Reset();
	LockXMLReader();
	FeedData(data.String(), data.Length());
	UnlockXMLReader();

}



void
JabberProtocol::SendMessage(BString to, BString text)
{
	XMLEntity   *entity;
	char **atts = CreateAttributeMemory(4);

	strcpy(atts[0], "to");
	strcpy(atts[1], to.String());
	strcpy(atts[2], "type");
	strcpy(atts[3], "chat");
	
	entity = new XMLEntity("message", (const char **)atts);
	entity->AddChild("body", NULL, text.String());
	char *str = entity->ToString();
	socketAdapter->SendData(str);
	free(str);

	DestroyAttributeMemory(atts, 4);
	
	delete entity;
}

void
JabberProtocol::SendGroupchatMessage(BString to, BString text)
{
	XMLEntity   *entity;
	char **atts = CreateAttributeMemory(4);

	strcpy(atts[0], "to");
	strcpy(atts[1], to.String());
	strcpy(atts[2], "type");
	strcpy(atts[3], "groupchat");
	
	entity = new XMLEntity("message", (const char **)atts);
	entity->AddChild("body", NULL, text.String());

	char *str = entity->ToString();
	socketAdapter->SendData(str);
	free(str);

	DestroyAttributeMemory(atts, 4);
	
	delete entity;
}

void 
JabberProtocol::RequestInfo()
{
	BString xml;
	xml << "<iq type='get' to='gmail.com'><query xmlns='http://jabber.org/protocol/disco#info'/></iq>";
	
	socketAdapter->SendData(xml);
}

void
JabberProtocol::RequestRoster() 
{
	acquire_sem(logged);
	release_sem(logged);
		
	BString xml;
	
	xml << "<iq id='roster_2' from='";
	xml = xml.Append(jid);
	xml << "' type='get'><query xmlns='jabber:iq:roster'/></iq>";
		
	socketAdapter->SendData(xml);
	
}

void
JabberProtocol::SendStorageRequest(BString tag, BString ns)
{
	BString xml = "<iq type='get' id='storage_request'>"
		"<query xmlns='jabber:iq:private'><";
	xml = xml.Append(tag);
	xml << " xmlns='";
	xml = xml.Append(ns);
	xml << "'/></query></iq>";
	
	socketAdapter->SendData(xml);
}

void
JabberProtocol::SendMUCConferenceRequest(BString conference)
{
	BString xml = "<iq from='";
	xml = xml.Append(jid);
	xml << "' id='disco1' to='";
	xml = xml.Append(conference);
	xml << "' type='get'><query xmlns='http://jabber.org/protocol/disco#info'/></iq>";
	
	socketAdapter->SendData(xml);
}

void
JabberProtocol::SendMUCRoomRequest(BString room)
{
	BString xml = "<iq from='";
	xml = xml.Append(jid);
	xml << "' id='disco3' to='";
	xml = xml.Append(room);
	xml << "' type='get'><query xmlns='http://jabber.org/protocol/disco#info'/></iq>";
	
	socketAdapter->SendData(xml);
}

void
JabberProtocol::InitSession() 
{
	BString xml = "<stream:stream to='";
	xml = xml.Append(domain);
	xml << "' "
		"xmlns='jabber:client' "
		"xmlns:stream='http://etherx.jabber.org/streams' "
		"version='1.0'>";
	
	socketAdapter->SendData(xml);
}

void
JabberProtocol::Session()
{
	BString xml = "<iq to='";
	xml = xml.Append(domain);
	xml << "' type='set' id='sess_1'>"
				"<session xmlns='urn:ietf:params:xml:ns:xmpp-session'/></iq>";
	
	socketAdapter->SendData(xml);
}

void
JabberProtocol::Bind() 
{
	BString xml = "<iq type='set' id='bind_0'>"
  		"<bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'>"
    	"<resource>haiku</resource>"
    	"</bind>"
		"</iq>";

	socketAdapter->SendData(xml);
}

bool
JabberProtocol::BeginSession() 
{
	if (host == "" || domain == "" || user == "" || pass == "" || port <= 0)
		return false;

	socketAdapter->Create();
	socketAdapter->Open(host, port);
	
	if (secure)
		socketAdapter->StartTLS();
		
	if (socketAdapter->IsConnected())
	{
		InitSession();
		return true;
	}

	return false;
}

void
JabberProtocol::StartTLS()
{
	printf("Start TLS recieved\n");
	BString xml = "<starttls xmlns='urn:ietf:params:xml:ns:xmpp-tls'/>";
	socketAdapter->SendData(xml);
}

// SASL PLAIN

bool
JabberProtocol::Authorize()
{
	int length = (strlen(user.String())*2)+strlen(domain.String())+strlen(pass.String())+3;
	char credentials[length];
	
	printf("Host: %s\n",host.String());
	printf("User: %s\n",user.String());
	printf("Domain: %s\n",domain.String());
	
	     if (domain.String()=="facebook.com")
		 sprintf(credentials, "%c%s%c%s", '\0',user.String(),  '\0', pass.String());
	else sprintf(credentials, "%s@%s%c%s%c%s", user.String(), domain.String(), '\0',
											   user.String(), '\0', pass.String());
	
	BString xml = "<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='PLAIN'>";
	
	char creds64[length*4];
	
	Base64Encode(credentials, length, creds64, length*4);
	xml = xml.Append(creds64);
	xml << "</auth>";
					
	socketAdapter->SendData(xml);
	
	return false;
}
