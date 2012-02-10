//////////////////////////////////////////////////
// Blabber [Agent.h]
//     Agent information.
//////////////////////////////////////////////////

#ifndef AGENT_H
#define AGENT_H

#include <string>

class Agent {
	public:
		        Agent();
		       ~Agent();

		std::string  JID() const;
		std::string  Name();
		std::string  Description();
		std::string  Service() const;
		bool    IsRegisterable() const;
		bool    IsRegistered() const;
		std::string  Transport();
		bool    IsSearchable();

		std::string  Username() const;
		std::string  Password() const;

		void    SetJID(std::string jid);
		void    SetName(std::string name);
		void    SetDescription(std::string description);
		void    SetService(std::string service);
		void    SetRegisterableFlag(bool is_registerable);
		void    SetRegisteredFlag(bool is_registered);
		void    SetTransport(std::string transport);
		void    SetSearchableFlag(bool is_searchable);

		void    SetUsername(std::string username);
		void    SetPassword(std::string password);

		void    Register();
		void    UnRegister();

	private:
		std::string _jid;
		std::string _name;
		std::string _description;
		std::string _service;
		bool   _is_registerable;
		std::string _transport;
		bool   _is_searchable;

		std::string _username;
		std::string _password;

		bool   _is_registered;
};

#endif
