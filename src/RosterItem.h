/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#ifndef ROSTER_ITEM_H
#define ROSTER_ITEM_H

#include <interface/ListItem.h>
#include <interface/View.h>
#include "UserID.h"

class RosterItem : public BStringItem {
	
public:
			        	RosterItem(UserID *userid);
  			        	~RosterItem();
								 
			bool     	StalePointer() const;
			UserID*		GetUserID();
			void        SetStalePointer(bool is_stale);	
	virtual void     	Update(BView *owner, const BFont *font);
			void     	DrawItem(BView *owner,
								 BRect frame,
								 bool complete = false);
	
private:

			UserID*		_userid;
			bool        _is_stale_pointer;
};

#endif
