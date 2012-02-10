//////////////////////////////////////////////////
// expat [XMLEntity.h]
//     C++ wrapper class that encapsulates an
//     XML tagset.
//////////////////////////////////////////////////

#ifndef XML_ENTITY_H
#define XML_ENTITY_H

#include <string>
#include <vector>

using namespace std;

class XMLEntity {
	public:
		                      XMLEntity(const char *name, const char **atts);
		                     ~XMLEntity();

		// Accessors
		XMLEntity            *Parent();
		XMLEntity            *Child(int index);
		XMLEntity            *Child(const char *name);
		XMLEntity			 *Child(const char *name, char *attribute, char *value);

		const char           *Name();
		const char           *Attribute(const char *key);
		const char           *AttributeKey(int index);
		const char           *AttributeValue(int index);
		const char           *CharacterData();
		const char           *ChildData(const char *name);
		const char           *Data();

		char                 *ToString(bool encoded=true, int level=0);
		char                 *StartToString(bool encoded=true);
		char                 *EndToString();
		
		int                   CountChildren();
		int                   CountAttributes();
		bool                  IsCompleted();

		// Mutators		
		void                  SetName(const char *new_name);
		void                  SetParent(XMLEntity *parent);
		void                  AddChild(XMLEntity *);
		void                  AddChild(const char *name, const char **atts, const char *data);
		void                  RemoveChild(const char *name);
		void                  RemoveChild(XMLEntity *entity);
		void                  RemoveSelf();

		void                  SetAttributes(const char **atts);
		void                  SetCharacterData(const char *data);
		void                  SetData(const char *data);
 
		void                  SetCompleted(bool is_complete);

		string                EscapeText(string text);
		string                DeEscapeText(string text);

	private:
		XMLEntity           *_parent;
		vector<XMLEntity *> *_children;

		char                *_name;
		char               **_atts;
		char                *_character_data;

		bool                 _is_complete;
};

#endif
