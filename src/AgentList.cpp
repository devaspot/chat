//////////////////////////////////////////////////
// Blabber [AgentList.cpp]
//////////////////////////////////////////////////

#ifndef AGENT_LIST_H
	#include "AgentList.h"
#endif

AgentList *AgentList::_instance = NULL;

AgentList *AgentList::Instance() {
	if (_instance == NULL) {
		_instance = new AgentList();
	}

	return _instance;
}

AgentList::~AgentList() {
	_instance = NULL;
}

void AgentList::AddAgent(Agent *new_agent) {
	// only add if it's not yet there
	if (_agent_list.count(new_agent->Service()) == 0) {
		_agent_list[new_agent->Service()] = new_agent;
	}
}

Agent *AgentList::GetAgentByService(std::string service) {
	if (_agent_list.count(service) > 0) {
		return _agent_list[service];
	}

	return NULL;
}

Agent *AgentList::GetAgentByID(std::string id) {
	for (AgentIter i = _agent_list.begin(); i != _agent_list.end(); ++i) {
		if (i->second->JID() == id) {
			return i->second;
		}
	}

	return NULL;
}

AgentList::AgentList() {
}

void AgentList::RemoveAllAgents() {
	for (AgentIter i = _agent_list.begin(); i != _agent_list.end(); ++i) {
		// MEMORY LEAK delete i->second;
	}

	_agent_list.clear();
}
