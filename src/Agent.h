/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */
 
#ifndef AGENT_H
#define AGENT_H

#include <string>

class Agent {
	public:
		        				Agent();
		       					~Agent();

				std::string  	JID() const;
				std::string  	Name();
				std::string  	Description();
				std::string  	Service() const;
				std::string  	Username() const;
				std::string  	Password() const;
				std::string  	Transport();

				bool    		IsRegisterable() const;
				bool    		IsRegistered() const;
				bool    		IsSearchable();

				void    		SetJID(std::string jid);
				void   			SetName(std::string name);
				void    		SetDescription(std::string description);
				void    		SetService(std::string service);
				void    		SetRegisterableFlag(bool is_registerable);
				void    		SetRegisteredFlag(bool is_registered);
				void    		SetTransport(std::string transport);
				void    		SetSearchableFlag(bool is_searchable);
				void    		SetUsername(std::string username);
				void    		SetPassword(std::string password);

				void    		Register();
				void    		UnRegister();

	private:
	
				std::string 	_jid;
				std::string 	_name;
				std::string 	_description;
				std::string 	_service;
				std::string 	_transport;
				std::string 	_username;
				std::string 	_password;

				bool   			_is_searchable;
				bool   			_is_registerable;
				bool   			_is_registered;
};

#endif
