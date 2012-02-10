//////////////////////////////////////////////////
// Haiku Chat [RosterSuperitem.h]
//     Entries of the RosterView widget that
//     maintain the RosterItems
//////////////////////////////////////////////////

#ifndef ROSTER_SUPERITEM_H
#define ROSTER_SUPERITEM_H

#include <interface/ListItem.h>
#include <interface/View.h>

class RosterSuperitem : public BStringItem {
public:
			       RosterSuperitem(const char *text);
  			      ~RosterSuperitem();

	void           DrawItem(BView *owner, BRect frame, bool complete = false);
};

#endif
