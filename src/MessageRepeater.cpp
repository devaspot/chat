//////////////////////////////////////////////////
// Message Repeater [MessageRepeater.cpp]
//////////////////////////////////////////////////

#ifndef MESSAGE_REPEATER_H
	#include "MessageRepeater.h"
#endif

MessageRepeater *MessageRepeater::_instance = NULL;

MessageRepeater *MessageRepeater::Instance() {
	if (_instance == NULL) {
		_instance = new MessageRepeater();
	}
	
	return _instance;
}

MessageRepeater::~MessageRepeater() {
	// destroy semaphore
	delete_sem(_looper_lock);

	_instance = NULL;
}

void MessageRepeater::MessageReceived(BMessage *msg) {
	_Broadcast(msg);
}

void MessageRepeater::AddTarget(BLooper *added_looper) {
	acquire_sem(_looper_lock);
	_looper_list.push_back(added_looper);
	release_sem(_looper_lock);
}

void MessageRepeater::RemoveTarget(BLooper *removed_looper) {
	acquire_sem(_looper_lock);

	for (LooperIter i = _looper_list.begin(); i != _looper_list.end(); ++i) {
		if ((*i) == removed_looper) {
			_looper_list.erase(i);
			break;
		}
	}

	release_sem(_looper_lock);
}

MessageRepeater::MessageRepeater()
	: BLooper() {
	// create semaphore
	_looper_lock = create_sem(1, "looper sempahore");
}

void MessageRepeater::_Broadcast(BMessage *msg) {
	acquire_sem(_looper_lock);
	
	for (ConstLooperIter i = _looper_list.begin(); i != _looper_list.end(); ++i) {
		(*i)->PostMessage(msg);
	}

	release_sem(_looper_lock);
}
