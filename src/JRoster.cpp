//////////////////////////////////////////////////
// Haiku Chat [JRoster.cpp]
//////////////////////////////////////////////////

#include "MessageRepeater.h"
#include <kernel/OS.h>
#include "Messages.h"
#include "JRoster.h"
#include <string>
#include <string.h>

JRoster *JRoster::_instance = NULL;

JRoster *
JRoster::Instance()
{
	if (_instance == NULL) {
		_instance = new JRoster();
	}
	return _instance;	
}

JRoster::~JRoster()
{
	_instance = NULL;
	delete_sem(_roster_lock);
}
	    
void
JRoster::AddRosterUser(UserID *roster_user)
{
	_roster->push_back(roster_user);
	RefreshRoster();
}

void
JRoster::RemoveUser(const UserID *removed_user)
{
	for (RosterIter i = _roster->begin(); i != _roster->end(); ++i) {
		if (*i == removed_user) {
			_roster->erase(i);
			delete removed_user;
			break;	
		}
	}
}

void
JRoster::RemoveAllUsers()
{
	while(_roster->begin() != _roster->end())
	{
		RosterIter i = _roster->begin();
		UserID *user = *i;
		_roster->erase(i);
		delete user;
	}
}
	
UserID *
JRoster::FindUser(search_method search_type, std::string name)
{
	if (search_type == JRoster::FRIENDLY_NAME) {
		for (RosterIter i = _roster->begin(); i != _roster->end(); ++i) {
			if (!strcasecmp(name.c_str(), (*i)->FriendlyName().c_str())) {
				return (*i);
			}
		}
	}
	
	if (search_type == JRoster::HANDLE) {
		for (RosterIter i = _roster->begin(); i != _roster->end(); ++i) {
			if (!strcasecmp(name.c_str(), (*i)->JabberHandle().c_str())) {
				return (*i);
			}
		}
	}

	if (search_type == JRoster::COMPLETE_HANDLE) {
		for (RosterIter i = _roster->begin(); i != _roster->end(); ++i) {
			if (!strcasecmp(name.c_str(), (*i)->JabberCompleteHandle().c_str())) {
				return (*i);
			}
		}
	}

	return NULL;
}

bool
JRoster::ExistingUserObject(const UserID *comparing_user)
{
	for (RosterIter i = _roster->begin(); i != _roster->end(); ++i) {
		if ((*i) == comparing_user) {
			return true;
		}
	}
	
	return false;
}

UserID *
JRoster::FindUser(UserID *comparing_user)
{
	return FindUser(JRoster::HANDLE, comparing_user->JabberHandle());
}

JRoster::ConstRosterIter
JRoster::BeginIterator()
{
	return _roster->begin();
}

JRoster::ConstRosterIter
JRoster::EndIterator()
{
	return _roster->end();
}

void
JRoster::RefreshRoster()
{
	// update everyone to the change
	MessageRepeater::Instance()->PostMessage(BLAB_UPDATE_ROSTER);
}

void
JRoster::Lock()
{
	acquire_sem(_roster_lock);
}

void
JRoster::Unlock()
{
	release_sem(_roster_lock);
}

JRoster::JRoster()
{
	_roster = new RosterList;
	_roster_lock = create_sem(1, "roster sempahore");
}
