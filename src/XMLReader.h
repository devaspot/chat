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

 
#ifndef XML_READER_H
#define XML_READER_H

#include <string>
#include <vector>
#include <expat.h>

#include "XMLEntity.h"

class XMLReader {
	
	public:
		                    XMLReader();
		virtual             ~XMLReader();

		void                Reset();
		void			    FeedData(const char *data, int len); 
		const 	char*		FXMLCheck(const char *xml);
		virtual void		OnStartTag(XMLEntity *entity) {};
		virtual void		OnEndTag(XMLEntity *entity) {};
		virtual void		OnEndEntity(XMLEntity *entity) { delete entity; entity = NULL; }

	protected:
	
				bool        _please_reset;

	private:
	
		static	void		_StaticOnStartTag(void *, const char *, const char **);
		static	void		_StaticOnEndTag(void *, const char *);
		static	void		_StaticOnCharacterData(void *, const XML_Char *, int);

				void		_OnStartTag(const char *, const char **);
				void		_OnEndTag(const char *);
				void		_OnCharacterData(const XML_Char *, int);

	    		XML_Parser	_parser;	
				XMLEntity*	_xml_entity;
				XMLEntity*	_curr_entity;
				string      _curr_character_data;
		
};

#endif
