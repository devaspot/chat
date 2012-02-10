//////////////////////////////////////////////////
// expat [XMLReader.h]
//     C++ wrapper that encapsulates the reading
//     of data (in memory) to produce a tree of
//     XMLEntity classes.
//////////////////////////////////////////////////

#ifndef XML_READER_H
#define XML_READER_H

#include <string>
#include <vector>

#include "XMLEntity.h"
#include <expat.h>

class XMLReader {
	public:
		                      XMLReader();
		virtual              ~XMLReader();

		void                  Reset();
		void			      FeedData(const char *data, int len); 
		const char *		  FXMLCheck(const char *xml);
		//const char *		  FXMLSkipHead(const char *xml);

		virtual void		  OnStartTag(XMLEntity *entity) {};
		virtual void		  OnEndTag(XMLEntity *entity) {};
		virtual void		  OnEndEntity(XMLEntity *entity) { delete entity; }

	protected:
		bool                 _please_reset;

	private:
		static void		     _StaticOnStartTag(void *, const char *, const char **);
		static void		     _StaticOnEndTag(void *, const char *);
		static void		     _StaticOnCharacterData(void *, const XML_Char *, int);

		void			 	 _OnStartTag(const char *, const char **);
		void				 _OnEndTag(const char *);
		void				 _OnCharacterData(const XML_Char *, int);

	    XML_Parser          _parser;	
		XMLEntity           *_xml_entity;
		XMLEntity           *_curr_entity;
		string               _curr_character_data;
		
};

#endif
