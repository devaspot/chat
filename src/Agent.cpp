#include "Agent.h"

Agent::Agent() {
	SetJID("");
	SetName("");
	SetDescription("");
	SetService("");
	SetRegisterableFlag(false);
	SetRegisteredFlag(false);
	SetTransport("");
	SetSearchableFlag(false);
}

Agent::~Agent() {
}

std::string Agent::JID() const {
	return _jid;
}

std::string Agent::Name() {
	return _name;
}

std::string Agent::Description() {
	return _description;
}

std::string Agent::Service() const {
	return _service;
}

bool Agent::IsRegisterable() const {
	return _is_registerable;
}

bool Agent::IsRegistered() const {
	return _is_registered;
}

std::string Agent::Transport() {
	return _transport;
}

bool Agent::IsSearchable() {
	return _is_searchable;
}

std::string Agent::Username() const {
	return _username;
}

std::string Agent::Password() const {
	return _password;
}

void Agent::SetJID(std::string jid) {
	_jid = jid;
}

void Agent::SetName(std::string name) {
	_name = name;
}

void Agent::SetDescription(std::string description) {
	_description = description;
}

void Agent::SetService(std::string service) {
	_service = service;
}

void Agent::SetRegisterableFlag(bool is_registerable) {
	_is_registerable = is_registerable;
}

void Agent::SetRegisteredFlag(bool is_registered) {
	if (_is_registered != is_registered) {
		_is_registered = is_registered;


		if (_is_registered == false) {
			SetUsername("");
			SetPassword("");
		}
	}
}

void Agent::SetTransport(std::string transport) {
	_transport = transport;
}

void Agent::SetSearchableFlag(bool is_searchable) {
	_is_searchable = is_searchable;
}

void Agent::SetUsername(std::string username) {
	_username = username;
}

void Agent::SetPassword(std::string password) {
	_password = password;
}

void Agent::Register() {
	_is_registered = true;
}

void Agent::UnRegister() {
	_is_registered = false;
}
