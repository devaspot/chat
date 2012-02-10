/*
 * Copyright 2010 Maxim Sokhatsky <maxim.sokhatsky@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
 
#ifndef JABBER_PROTOCOL_H
#define JABBER_PROTOCOL_H

#include "SecureSocket.h"
#include "XMLReader.h"
#include "UserID.h"

#include <OS.h>
#include <String.h>
#include <string.h>
#include <Locker.h>
#include <Message.h>

enum JabberMessages { PORT_TALKER_DATA = 0x68374920 };

class ChatWindow;
class BlabberMainWindow;

class JabberProtocol : public XMLReader
{
	public:
	
								enum iq_intent {
									LOGIN,
									ROSTER,
									AGENTS,
									REGISTER,
									SEND_REGISTER,
									UNREGISTER,
									SEND_UNREGISTER,
									NEW_USER,
									MESSAGE,
									CHAT
								};

								JabberProtocol();
								~JabberProtocol();
								
				void			SetConnection(BString host, int port, bool secure);
				void			SetCredentials(BString user, BString domain, BString pass);
				void			LogOn();
				void			RequestRoster();
				void			RequestInfo();
				void			Session();
				void			SendMessage(BString to, BString text);
				void			SendGroupchatMessage(BString to, BString text);
				void			SetStatus(BString status, BString message);
				void			ReceiveData(BMessage *msg);
				void			ReceivedMessageHandler(BMessage *jabberMessage);
				void			ProcessVersionRequest(BString req_id, BString req_from);
				void			Disconnect();
				void			Pong(BString id, BString from);
				void			LockXMLReader();
				void			UnlockXMLReader();
				void			OnStartTag(XMLEntity *entity);
				void			OnEndTag(XMLEntity *entity);
				void			OnEndEntity(XMLEntity *entity);
				void			OnTag(XMLEntity *entity);
				void			ParseRosterList(XMLEntity *entity);
				void			ProcessPresence(XMLEntity *entity);
				void			ParseStorage(XMLEntity *storage);
				void			SaveConference(UserID *conference);
				void			ProcessUserPresence(UserID *user, XMLEntity *entity);
				void			AcceptPresence(string username);
				void			SendStorageRequest(BString tag, BString ns); // XEP-0049
				void			RejectPresence(string username);
				char**			CreateAttributeMemory(int num_items);
				void			DestroyAttributeMemory(char **atts, int num_items);
				string			GenerateUniqueID();
				void			SendSubscriptionRequest(string username);
				void			SendUnsubscriptionRequest(string username);
				void			AddToRoster(UserID *new_user);
				void			RemoveFromRoster(UserID *removed_user);
				void			SendMUCConferenceRequest(BString conference);
				void			SendMUCRoomRequest(BString room);
				void			SendUnavailable(BString to, BString status);
				void			SendAvailable(BString to, BString status);
				void			SendUserRegistration(BString username, BString password, BString resource);
				void			JoinRoom(BString to, BString pass);
				void			SendGroupPresence(string _group_room, string _group_username);
				void			RequestRoomOptions(BString room);
				
				SSLAdapter		*socketAdapter;
				sem_id			logged;
				sem_id			read_queue;
				BString			jid;
				BString			user;
				BString			domain;		
		BlabberMainWindow		*mainWindow;
				sem_id          xml_reader_lock;
				bool			_storage_supported;
				
	private:
	
				bool			BeginSession();
				bool			Authorize();
				void			InitSession();
				void			Bind();
				void			StartDispatcher();
				
				int32			reciever_thread;
				BString			host;
				int				port;
				BString			pass;
				bool			secure;
				

};

#endif // _H
