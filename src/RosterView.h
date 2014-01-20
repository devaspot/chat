/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#ifndef ROSTER_VIEW_H
#define ROSTER_VIEW_H

#include <map>
#include <ListItem.h>
#include <OutlineListView.h>
#include <PopUpMenu.h>
#include "RosterItem.h"
#include "RosterSuperitem.h"
#include "UserID.h"
#include "XMLEntity.h"

class RosterView : public BOutlineListView {
public:
	typedef std::map<BListItem *, UserID::online_status>  ItemToStatusMap;
	typedef std::map<UserID::online_status, BListItem *>  StatusToItemMap;
	typedef std::map<string, UserID*>  UsersMap;

public:
	     	            		RosterView(BRect frame);
		               			~RosterView();

	static 	int          		ListComparison(const void *a, const void *b);

			void                AttachedToWindow();
			RosterItem         *CurrentItemSelection();
			void                KeyDown(const char *bytes, int32 len);
			void                MouseDown(BPoint point);
			void                RemoveSelected();
			void                SelectionChanged();
			void				CreateRoots();
			void                LinkUser(UserID *added_user, bool sort = true);
			void                UnlinkUser(UserID *removed_user);
			int32               FindUser(UserID *compare_user);
			void                UpdatePopUpMenu();
			void                UpdateRoster();
			int					GetConferencesCount();
	const 	UserID*				GetConference(int i);
			UsersMap			fUsers;

private:

			BPopUpMenu*        	_popup;
			BMenuItem*         	_message_item;
			BMenuItem*          _chat_item;
			BMenuItem*          _change_user_item;
			BMenuItem*          _remove_user_item;
			BMenuItem*          _user_info_item;
			BMenuItem*          _user_chatlog_item;

			BMenu*              _presence;
			BMenuItem*          _subscribe_presence;
			BMenuItem*          _unsubscribe_presence;
			BMenuItem*          _revoke_presence;
			BMenuItem*			_resend_presence;

			StatusToItemMap    	_status_to_item_map;
			ItemToStatusMap    	_item_to_status_map;

			RosterSuperitem*	_offline;
			RosterSuperitem*	_online;
			RosterSuperitem*	_unknown;
			RosterSuperitem*	_unaccepted;
			RosterSuperitem*	_transports;
			RosterSuperitem*	_conferences;
			
};

#endif
