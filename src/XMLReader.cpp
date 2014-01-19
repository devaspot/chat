/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 *
 *
 * XML Kit: XMLEntry and XMLReader
 *
 */

#include "XMLReader.h"

#include <cstdio>
#include <string>
#include <strings.h>
#include <stdlib.h>

XMLReader::XMLReader()
{
	_parser = NULL;
	_xml_entity = NULL;
	_curr_entity = NULL;
	_curr_character_data = "";
	_please_reset = false;
    _parser = XML_ParserCreate(NULL);	

	if (_parser)
	{
		XML_SetUserData(_parser, this);
	    XML_SetElementHandler(_parser, _StaticOnStartTag, _StaticOnEndTag);
    	XML_SetCharacterDataHandler(_parser, _StaticOnCharacterData);
	}
}

XMLReader::~XMLReader()
{
	if (_parser) XML_ParserFree(_parser);
	delete _xml_entity; _xml_entity = NULL;
}

void
XMLReader::Reset()
{
	_please_reset = true;
}

void
XMLReader::FeedData(const char *data, int len)
{
    if (_please_reset)
    {
		XML_ParserFree(_parser);
		_parser = NULL;
		
		delete _xml_entity;
		_xml_entity = NULL;
		_curr_entity = NULL;
		_curr_character_data = "";
		_please_reset = false;
	    _parser = XML_ParserCreate(NULL);	

		if (_parser)
		{
			XML_SetUserData(_parser, this);
		    XML_SetElementHandler(_parser, _StaticOnStartTag, _StaticOnEndTag);
	    	XML_SetCharacterDataHandler(_parser, _StaticOnCharacterData);
		}
    }

    if (_parser && !XML_Parse(_parser, data, len, 0))
    {
		_please_reset = true;
    }    
}

void
XMLReader::_StaticOnStartTag(
	void *userData,
	const char *name,
	const char **atts)
{
	((XMLReader *)userData)->_OnStartTag(name, atts);
}

void
XMLReader::_StaticOnEndTag(
	void *userData,
	const char *name)
{
	((XMLReader *)userData)->_OnEndTag(name);	
}

void
XMLReader::_StaticOnCharacterData(
	void *userData,
	const XML_Char *data,
	int len)
{
	((XMLReader *)userData)->_OnCharacterData(data, len);
}

void
XMLReader::_OnStartTag(
	const char *name,
	const char **atts)
{
	if (!strcasecmp(name, "?xml")) return;

	_curr_character_data = "";
	XMLEntity *new_entity = new XMLEntity(name, atts);

	if (_xml_entity == NULL) _curr_entity = _xml_entity = new_entity;
	else
	{
		_curr_entity->AddChild(new_entity);
		_curr_entity = new_entity;
	}

	OnStartTag(_curr_entity);
}

const char*
FXMLSkipHead(const char *xml)
{
	const char *ptr = xml;
	const char *tmp;
	int nopen = 0;
	if (xml == NULL) return NULL;
	if(ptr == NULL) return NULL;
	ptr = strchr(ptr , '<');
	if(ptr == NULL) return NULL;
	if(*(ptr+1) == '?') {
		ptr = strchr(ptr , '>');
		if(ptr == NULL) return NULL;
		if(*(ptr-1) != '?')	return NULL;
		ptr = strchr(ptr , '<');
		if(ptr == NULL) return NULL;
	}
	tmp = ptr;
	while(*tmp)
	{
		if(*tmp == '<')
		{
			nopen++;
			if(nopen > 1) return NULL;
		}
		else if(*tmp == '>')
		{
			nopen--;
			if(nopen < 0) return NULL;
		}
		tmp++;
	}
	return (nopen == 0) ? ptr : NULL;
}


const char*
XMLReader::FXMLCheck(const char *xml)
{
	const char *ptr;
	const char *tmp;
	int nopen = 0;
	if (xml == NULL) return NULL;
	ptr = FXMLSkipHead(xml);
	tmp = ptr;
	while(tmp)
	{
		if(*(tmp+1) == '/')
		{
			nopen--;
			if(nopen < 0) return NULL;
			tmp = strchr(tmp, '>');
			if(tmp == NULL) return NULL;
		}
		else
		{
			nopen++;
			tmp = strchr(tmp, '>');
			if(tmp == NULL) return NULL;
			if(*(tmp-1) == '/') nopen--;
		}
		tmp = strchr(tmp, '<');
	}
	return (nopen == 0) ? ptr : NULL;
}

void
XMLReader::_OnEndTag(const char *name)
{
	if (_curr_character_data.size() > 0)
		_curr_entity->SetCharacterData(_curr_character_data.c_str());

	_curr_entity->SetCompleted(true);
	_curr_character_data = string("");

	OnEndTag(_curr_entity);

	if (_xml_entity == NULL) return;

	if (_curr_entity && (_curr_entity == _xml_entity))
	{
		XMLEntity *completed_entity = _xml_entity;
		_curr_entity = _xml_entity = NULL;
		OnEndEntity(completed_entity);
	}
	else
	{
		_curr_entity = _curr_entity->Parent();
	}
}

void
XMLReader::_OnCharacterData(const XML_Char *data, int len)
{
	char *new_data = (char *)malloc((len*2 + 2) * sizeof(char));
	strncpy(new_data, data, len);
	new_data[len] = '\0';

	if (!new_data || *new_data == 0) {
		return;
	}

	string added_data = string(new_data);
	
	if (_curr_character_data.size() == 0)
		while (added_data.size() > 0 && 
				(added_data[0] == ' ' || added_data[0] == '\n'))
			added_data.erase(0, 1);

	uint replacement;

	while ((replacement = _curr_character_data.find("&apos;")) != string::npos) 	{
		_curr_character_data.replace(replacement, 6, "&");
	}	

	while ((replacement = _curr_character_data.find("&lt;")) != string::npos)
	{
		_curr_character_data.replace(replacement, 4, "<");
	}	

	while ((replacement = _curr_character_data.find("&gt;")) != string::npos)
	{
		_curr_character_data.replace(replacement, 4, ">");
	}	
	
	_curr_character_data += added_data;

	free(new_data);
}
