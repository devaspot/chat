//////////////////////////////////////////////////
// Blabber [TransportItem.h]
//     Transport entries of the RosterView widget.
//////////////////////////////////////////////////

#ifndef TRANSPORT_ITEM_H
#define TRANSPORT_ITEM_H

#ifndef _BITMAP_H
	#include <interface/Bitmap.h>
#endif

#ifndef _LIST_ITEM_H
	#include <interface/ListItem.h>
#endif

#ifndef _VIEW_H
	#include <interface/View.h>
#endif

#ifndef USER_ID_H
	#include "UserID.h"
#endif

class TransportItem : public BStringItem {
public:
			         TransportItem(const UserID *userid);
  			        ~TransportItem();

	void             DrawItem(BView *owner, BRect frame, bool complete = false);
	virtual void     Update(BView *owner, const BFont *font);
	
	bool             StalePointer();

	const UserID    *GetUserID() const;
	void             SetStalePointer(bool is_stale);	
	
private:
	const UserID   *_userid;
	bool            _is_stale_pointer;
		
	static BBitmap *_online_icon;
	static BBitmap *_offline_icon;
	static BBitmap *_unknown_icon;
};

#endif