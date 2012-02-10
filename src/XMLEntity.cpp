//////////////////////////////////////////////////
// expat [XMLEntity.cpp]
//////////////////////////////////////////////////

#include "XMLEntity.h"

#include <cstring>
#include <stdlib.h>

XMLEntity::XMLEntity(const char *name, const char **atts) {
	_parent         = NULL;
	_children       = new vector<XMLEntity *>;

	_name           = strdup(name);
	_atts           = NULL;
	_character_data = NULL;
	_is_complete    = false;

	SetAttributes(atts);
}

XMLEntity::~XMLEntity() {
	// delete all children (depth first)
	for (vector<XMLEntity *>::iterator i=_children->begin(); i != _children->end(); ++i) {
		delete *i;
	}

	// delete self
	delete   _children;
	free(_name);

	// delete attributes
	if (_atts != NULL) {
		for (int i=0; i<(CountAttributes() + 2); ++i) {
			free(_atts[i]);
		}
	
		free(_atts);
	}

	free(_character_data);
}

XMLEntity *XMLEntity::Parent() {
	return _parent;
}

XMLEntity *XMLEntity::Child(int index) {
	return (*_children)[index];
}

XMLEntity *XMLEntity::Child(const char *name) {
	if (name) {
		for (int i=0; i<CountChildren(); ++i)
			if (!strcasecmp(Child(i)->Name(), name))
				return Child(i);
	}
	
	return NULL;
}

XMLEntity *XMLEntity::Child(const char *name, char *attribute, char *value)
{
	if (name) {
		for (int i=0; i<CountChildren(); ++i)
			if (!strcasecmp(Child(i)->Name(), name) && 
			!strcasecmp(Child(i)->Attribute(attribute), value))
				return Child(i);
	}
	
	return NULL;
}

const char *XMLEntity::Name() {
	return _name;
}

const char *XMLEntity::Attribute(const char *key) {
	if (_atts)
		for(int i=0; _atts[i] && *_atts[i]; i+=2)
			if (!strcasecmp(_atts[i], key))
				return _atts[i+1];
	
	return NULL;
}

const char *XMLEntity::AttributeKey(int index) {
	return _atts[index << 1];
}

const char *XMLEntity::AttributeValue(int index) {
	return _atts[(index << 1) + 1];
}

const char *XMLEntity::CharacterData() {
	if (_character_data && *_character_data)
		return _character_data;
	else
		return "";
}

const char *XMLEntity::ChildData(const char *name) {
	XMLEntity *child = Child(name);
	
	// if the child exists
	if (child)
		return child->Data();

	return NULL;
}

const char *XMLEntity::Data() {
	return CharacterData();
}

char *XMLEntity::ToString(bool encoded, int level) {
	// piece together text-version XML from XMLEntity
	string text = "";

	// assemble indent size
	string indent = "";

	string character_data = EscapeText(CharacterData());

	for(int i=0; i<level; ++i)
		indent += "\t";
	
	// starting tag
	text  = indent;
	text += "<";
	text += Name();

	// attributes
	for(int i=0; i < CountAttributes(); ++i) {
		text += " ";
		text += AttributeKey(i);
		text += "=";
		text += "\"";
		text += EscapeText(AttributeValue(i));
		text += "\"";
	}

	// is this a starting/closing combo tag?
	if (character_data.size() == 0 && CountChildren() == 0)
		text += "/";

	text += ">";

	// character data
	text += character_data;

	// children
	if (CountChildren() > 0) {
		text += "\n";

		for (int i=0; i < CountChildren(); ++i) {
			char *str = Child(i)->ToString(encoded, level + 1);
			if (str) {
				text += str;
				free(str);
			}
		}
		
		text += indent;
	}

	// closing tag
	if (CountChildren() > 0 || character_data.size() > 0) {
		text += "</";
		text += Name();
		text += ">";
	}

	// end
	text += "\n";
	
	// possible point of memory leak - needs resolution
	return strdup(text.c_str());	
}
/*
char *XMLEntity::StartToString(bool encoded) {
	// piece together text-version XML from XMLEntity
	string text = "";

	// starting tag
	text += "<";
	text += Name();

	// attributes
	for(int i=0; i < CountAttributes(); ++i) {
		text += " ";
		text += AttributeKey(i);
		text += "=";
		text += "\"";
		text += EscapeText(AttributeValue(i));
		text += "\"";
	}

	text += ">";

	// possible point of memory leak - needs resolution
	return strdup(text.c_str());	
}

char *XMLEntity::EndToString() {
	// piece together text-version XML from XMLEntity
	string text = "";
	
	// closing tag
	text += "</";
	text += Name();
	text += ">";

	// possible point of memory leak - needs resolution
	return strdup(text.c_str());	
}
*/
int XMLEntity::CountChildren() {
	return _children->size();
}

int XMLEntity::CountAttributes() {
	if (!_atts)
		return 0;
		
	int i = 0;
	
	for(i=0; _atts[i] != NULL; i+=2);

	return (i >> 1);
}

bool XMLEntity::IsCompleted() {
	return _is_complete;
}
void XMLEntity::SetName(const char *new_name) {
	if (_name) {
		free(_name);
		_name = NULL;
	}
	
	_name = strdup(new_name);
}

void XMLEntity::SetParent(XMLEntity *entity) {
	_parent = entity;
}

void XMLEntity::AddChild(XMLEntity *entity) {
	// automatically set parent of target child
	entity->SetParent(this);

	// add child
	_children->push_back(entity);
}

void XMLEntity::AddChild(const char *name, const char **atts, const char *data) {
	XMLEntity *entity = new XMLEntity(name, atts);
	entity->SetCharacterData(data);
	AddChild(entity);
}

void XMLEntity::RemoveChild(const char *name) {
	for (vector<XMLEntity *>::iterator i=_children->begin(); i != _children->end(); ++i) {
		if (!strcasecmp((*i)->Name(), name)) {
			delete *i;
			_children->erase(i);

			return;
		}
	}
}

void XMLEntity::RemoveChild(XMLEntity *entity) {
	for (vector<XMLEntity *>::iterator i=_children->begin(); i != _children->end(); ++i)
		if ((*i) == entity) {
			delete *i;
			_children->erase(i);

			return;
		}
}

void XMLEntity::RemoveSelf() {
	if(Parent()) {
		Parent()->RemoveChild(this);
	} else {
		delete this;
	}
}

void XMLEntity::SetAttributes(const char **atts) {
	// delete attributes
	if (_atts != NULL) {
		for (int i=0; i<(CountAttributes() + 2); ++i) {
			delete[] _atts[i];
		}
	
		delete[] _atts;
	
		_atts = NULL;
	}

	if (atts == NULL)
		return;

	// initialize attributes
	int i;

	for (i=0; atts[i] && *atts[i] && atts[i+1]; i+=2);

	if (i > 0) {
		_atts = (char **)malloc((i+4) * sizeof(char *));
		//fprintf(stderr, "Entity %s has %i attributes.\n", Name(), i);
	
		for(int j=0; j<i; j+=2) {
			_atts[j]   = strdup(atts[j]);
			_atts[j+1] = strdup(atts[j+1]);
			//fprintf(stderr, "%s='%s'\n", _atts[j],_atts[j+1]);
		}

		_atts[i] = NULL;
		_atts[i+1] = NULL;
	}
}

void XMLEntity::SetCharacterData(const char *data) {
	// clear data?
	if (data && *data) {
		if (_character_data) {
			free(_character_data);
			_character_data = NULL;
		}
		
		_character_data = strdup(data);
	} else {
		_character_data = NULL;
	}
}

void XMLEntity::SetData(const char *data) {
	SetCharacterData(data);
}

void XMLEntity::SetCompleted(bool is_complete) {
	_is_complete = is_complete;
}

string XMLEntity::EscapeText(string text) {
	unsigned int replacement;

	replacement = 0;
	while ((replacement = text.find("&", replacement)) != string::npos) {
		text.replace(replacement++, 1, "&amp;");
	}	

	replacement = 0;
	while ((replacement = text.find("<", replacement)) != string::npos) {
		text.replace(replacement++, 1, "&lt;");
	}	

	replacement = 0;
	while ((replacement = text.find(">", replacement)) != string::npos) {
		text.replace(replacement++, 1, "&gt;");
	}		

	return text;
}
/*
string XMLEntity::DeEscapeText(string text) {
	unsigned int replacement;

	replacement = 0;
	while ((replacement = text.find("&amp;", replacement)) != string::npos) {
		text.replace(replacement++, 5, "&");
	}	

	replacement = 0;
	while ((replacement = text.find("&lt;", replacement)) != string::npos) {
		text.replace(replacement++, 4, "<");
	}	

	replacement = 0;
	while ((replacement = text.find("&gt;", replacement)) != string::npos) {
		text.replace(replacement++, 4, ">");
	}		

	return text;

}
*/
