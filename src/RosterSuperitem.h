/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#ifndef ROSTER_SUPERITEM_H
#define ROSTER_SUPERITEM_H

#include <interface/ListItem.h>
#include <interface/View.h>

class RosterSuperitem : public BStringItem {
	
public:
			       	RosterSuperitem(const char *text);
  			      	~RosterSuperitem();

			void	DrawItem(BView *owner,
							 BRect frame,
							 bool complete = false);

};

#endif
