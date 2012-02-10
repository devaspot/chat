//////////////////////////////////////////////////
// Message Repeater [MessageRepeater.h]
//     Echoes BMessages sent to it to all
//     registered message handlers. Acts as a
//     singleton.
//////////////////////////////////////////////////

#ifndef MESSAGE_REPEATER_H
#define MESSAGE_REPEATER_H

#include <list>
#include <Looper.h>

typedef std::list<BLooper *>                  LooperList;
typedef std::list<BLooper *>::iterator        LooperIter;
typedef std::list<BLooper *>::const_iterator  ConstLooperIter;

class MessageRepeater : public BLooper {
public:
	static MessageRepeater  *Instance();
	                        ~MessageRepeater();

	void                     MessageReceived(BMessage *msg);

	void                     AddTarget(BLooper *added_looper);
	void                     RemoveTarget(BLooper *removed_looper);

protected:
	                         MessageRepeater();

private:
	void                     _Broadcast(BMessage *msg);

	static MessageRepeater *_instance;
	LooperList              _looper_list;
	
	sem_id                  _looper_lock;
	
};

#endif
