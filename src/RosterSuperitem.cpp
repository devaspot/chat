/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#include "RosterSuperitem.h"
#include <iostream>

RosterSuperitem::RosterSuperitem(const char *text)
	:
	BStringItem(text)
{
}

RosterSuperitem::~RosterSuperitem()
{
}

void
RosterSuperitem::DrawItem(BView *owner, BRect frame, bool complete)
{
	owner->SetFontSize(11.0);
	owner->SetHighColor(0, 0, 0, 255);

	BStringItem::DrawItem(owner, frame, complete);
}
