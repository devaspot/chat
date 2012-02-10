//////////////////////////////////////////////////
// Blabber [RosterView.h]
//     A graphical view of the buddy list.
//////////////////////////////////////////////////

#ifndef AFFLIST_VIEW_H
#define AFFLIST_VIEW_H

#include <map>
#include <interface/ListItem.h>
#include <interface/OutlineListView.h>
#include <interface/PopUpMenu.h>
#include "JRoster.h"
#include "RosterItem.h"
#include "RosterSuperitem.h"
#include "TransportItem.h"
#include "UserID.h"
#include "XMLEntity.h"

class AffiliationsListView : public BOutlineListView {
public:
	enum 

	typedef std::map<BListItem *, UserID::online_status>  ItemToStatusMap;
	typedef std::map<UserID::online_status, BListItem *>  StatusToItemMap;

public:
	     	            AffiliationsListView(BRect frame);
		               ~AffiliationsListView();

	// sort algorithm used before outline list view used
	static int          ListComparison(const void *a, const void *b);

	void                AttachedToWindow();
	RosterItem         *CurrentItemSelection();
	void                KeyDown(const char *bytes, int32 len);
	void                MouseDown(BPoint point);
	void                RemoveSelected();
	void                SelectionChanged();

	void                LinkUser(const UserID *added_user);
	void                UnlinkUser(const UserID *removed_user);
	int32               FindUser(const UserID *compare_user);

	void                UpdatePopUpMenu();
	void                UpdateRoster();

private:
	BPopUpMenu        *_popup;

	BMenuItem         *_message_item;
	BMenuItem         *_chat_item;
	BMenuItem         *_change_user_item;
	BMenuItem         *_remove_user_item;
	BMenuItem         *_user_info_item;
	BMenuItem         *_user_chatlog_item;

	BMenu             *_presence;
	BMenuItem         *_subscribe_presence;
	BMenuItem         *_unsubscribe_presence;

	StatusToItemMap    _status_to_item_map;
	ItemToStatusMap    _item_to_status_map;

	RosterSuperitem   *_owners;
	RosterSuperitem   *_online;
	RosterSuperitem   *_unknown;
	RosterSuperitem   *_unaccepted;
	RosterSuperitem   *_transports;
	RosterSuperitem   *_conferences;
};

#endif
