#ifndef FILE_XML_READER_H
#define FILE_XML_READER_H

#include "XMLReader.h"

class FileXMLReader : public XMLReader {
public:
	enum file_status {FILE_OK, FILE_NOT_FOUND, FILE_CORRUPTED};
	
public:
	                  FileXMLReader(const char *filename, bool in_settings = false);
	                 ~FileXMLReader();

	void			  CreatePath();
	
	void              OnEndEntity(XMLEntity *entity);	
	void              SetEntityTree(XMLEntity *entity_tree);

	const char       *Data(const char *key);
	void              SetData(const char *key, const char *value);
	void              SetIntData(const char *key, int value);
	void              SetFloatData(const char *key, float value);

	bool              Tag(const char *key);
	void              SetTag(const char *key, bool enabled);

	file_status       FileStatus();
		
	void              WriteToFile();
	
	XMLEntity        *EntityTree();
	
private:
	XMLEntity       *_entity_tree;
	char            *_path_name;
	file_status      _file_status;
};

#endif