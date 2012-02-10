//////////////////////////////////////////////////
// Blabber [AgentList.h]
//     Stores the list of agents supported by
//     the server.
//////////////////////////////////////////////////

#ifndef AGENT_LIST_H
#define AGENT_LIST_H

#ifndef __MAP__
	#include <map>
#endif

#ifndef __STRING__
	#include <string>
#endif

#ifndef AGENT_H
	#include "Agent.h"
#endif

class AgentList {
public:
	typedef std::map<std::string, Agent *>                   AgentMap;
	typedef std::map<std::string, Agent *>::iterator         AgentIter;
	typedef std::map<std::string, Agent *>::const_iterator   ConstAgentIter;

public:
	static AgentList  *Instance();
					  ~AgentList();

	void               AddAgent(Agent *new_agent);
	Agent             *GetAgentByService(std::string service);
	Agent             *GetAgentByID(std::string id);

	void               RemoveAllAgents();

protected:
	                   AgentList();

private:
	static AgentList *_instance;

	AgentMap          _agent_list;
};

#endif
