/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */


#include "MessageRepeater.h"

MessageRepeater* MessageRepeater::fInstance = NULL;

MessageRepeater*
MessageRepeater::Instance()
{
	if (fInstance == NULL) fInstance = new MessageRepeater();
	return fInstance;
}

MessageRepeater::~MessageRepeater()
{
	delete_sem(fLooperLock);
	fInstance = NULL;
}

void
MessageRepeater::MessageReceived(BMessage *msg)
{
	_Broadcast(msg);
}

void
MessageRepeater::AddTarget(BLooper *added_looper)
{
	acquire_sem(fLooperLock);
	fLooperList.push_back(added_looper);
	release_sem(fLooperLock);
}

void
MessageRepeater::RemoveTarget(BLooper *removed_looper)
{
	acquire_sem(fLooperLock);

	for (LooperIter i = fLooperList.begin(); i != fLooperList.end(); ++i) {
		if ((*i) == removed_looper) {
			fLooperList.erase(i);
			break;
		}
	}

	release_sem(fLooperLock);
}

MessageRepeater::MessageRepeater()
	:
	BLooper()
{
	fLooperLock = create_sem(1, "looper sempahore");
}

void
MessageRepeater::_Broadcast(BMessage *msg)
{
	acquire_sem(fLooperLock);
	
	for (ConstLooperIter i = fLooperList.begin(); i != fLooperList.end(); ++i) {
		(*i)->PostMessage(msg);
	}

	release_sem(fLooperLock);
}
