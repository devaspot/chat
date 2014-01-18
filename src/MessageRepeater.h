/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#ifndef MESSAGE_REPEATER_H
#define MESSAGE_REPEATER_H

#include <list>
#include <Looper.h>

typedef std::list<BLooper *>                  LooperList;
typedef std::list<BLooper *>::iterator        LooperIter;
typedef std::list<BLooper *>::const_iterator  ConstLooperIter;

class MessageRepeater : public BLooper {

public:

	static	MessageRepeater*	Instance();
	                        	~MessageRepeater();
			void             	MessageReceived(BMessage *msg);
			void                AddTarget(BLooper *added_looper);
			void                RemoveTarget(BLooper *removed_looper);

protected:
	                         	MessageRepeater();

private:

			void                _Broadcast(BMessage *msg);
	static 	MessageRepeater*	fInstance;
			LooperList          fLooperList;
			sem_id              fLooperLock;
	
};

#endif
