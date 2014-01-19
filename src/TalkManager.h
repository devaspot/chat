/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#ifndef TALK_MANAGER_H
#define TALK_MANAGER_H

#include <map>
#include <string>

#include "GenericFunctions.h"
#include "ChatWindow.h"
#include "XMLEntity.h"

class TalkManager {
	
public:

	typedef map<string, ChatWindow*>                   TalkMap;
	typedef map<string, ChatWindow*>::iterator         TalkIter;
	typedef map<string, ChatWindow*>::const_iterator   ConstTalkIter;

			enum     		rotation {	ROTATE_FORWARD, ROTATE_BACKWARD };
		
public:

	static	TalkManager*	Instance();
      	                	~TalkManager();

			ChatWindow*		CreateTalkSession(ChatWindow::talk_type type, UserID *user);
			ChatWindow*     FindWindow(string username);
			void            ProcessMessageData(XMLEntity *entity);	
			void            UpdateWindowTitles(const UserID *user);
			void            RemoveWindow(string thread_id);
			void            RotateToNextWindow(ChatWindow *current, rotation direction);
	
			void            Reset();
			JabberProtocol*	jabber;
	
			TalkMap         _talk_map;
			void            Lock();
			void            Unlock();
			sem_id			_windows_map_lock;
	
protected:
 	                     	TalkManager();

private:

	static	TalkManager*	_instance;

};

#endif

