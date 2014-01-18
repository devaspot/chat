/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#ifndef AGENT_LIST_H
#define AGENT_LIST_H

#include <map>
#include <string>
#include "Agent.h"

class AgentList {
	
public:

	typedef std::map<std::string, Agent *>                   AgentMap;
	typedef std::map<std::string, Agent *>::iterator         AgentIter;
	typedef std::map<std::string, Agent *>::const_iterator   ConstAgentIter;

public:

	static	AgentList*	Instance();
					  	~AgentList();

			void        AddAgent(Agent *new_agent);
			Agent*		GetAgentByService(std::string service);
			Agent*		GetAgentByID(std::string id);
			void        RemoveAllAgents();

protected:
	                   	AgentList();

private:

	static 	AgentList*	_instance;
			AgentMap    _agent_list;
};

#endif
