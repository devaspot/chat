/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#include <cstdio>
#include <storage/Directory.h>
#include <storage/File.h>
#include <storage/FindDirectory.h>
#include <storage/StorageDefs.h>
#include <stdlib.h>
#include <strings.h>

#include "FileXMLReader.h"
#include "XMLEntity.h"

FileXMLReader::FileXMLReader(const char *filename, bool in_settings)
{
	_entity_tree = NULL;
	_path_name = (char *)malloc(B_PATH_NAME_LENGTH * sizeof(char));
	
	if (in_settings)
	{
		find_directory(
			B_USER_SETTINGS_DIRECTORY,
			0,
			false,
			_path_name,
			B_PATH_NAME_LENGTH - strlen(filename) + 1);

		strcat(_path_name, "/");
		strcat(_path_name, filename);
	} else {
		strcpy(_path_name, filename);
	}

	CreatePath();

	BFile *_fh = new BFile();

	if (_fh->SetTo(_path_name, B_READ_ONLY) == B_NO_ERROR)
	{
		int bytes_read;
		char buffer[1001];

		while((bytes_read = _fh->Read(buffer, 1000))) {
			buffer[bytes_read] = '\0';
			FeedData(buffer, bytes_read);
		}
		
		if (_please_reset || !_entity_tree)
		{
			_entity_tree = new XMLEntity("container", NULL);
			_file_status = FILE_CORRUPTED;
			WriteToFile();
		} else
		{
			_file_status = FILE_OK;
		}
	} else
	{
		_entity_tree = new XMLEntity("container", NULL);
		_file_status = FILE_NOT_FOUND;
		WriteToFile();
	}

	delete _fh;
}

FileXMLReader::~FileXMLReader()
{
	WriteToFile();
	free(_path_name);
	delete _entity_tree;
}

void
FileXMLReader::CreatePath()
{
	string directory = _path_name;
	string::size_type last_slash_pos = directory.rfind("/");
	if (last_slash_pos != string::npos)
		create_directory(directory.substr(0, last_slash_pos).c_str(), 0744);
}	

void
FileXMLReader::OnEndEntity(XMLEntity *entity)
{
	if (!strcasecmp(entity->Name(), "container"))
		_entity_tree = entity;
}

void
FileXMLReader::SetEntityTree(XMLEntity *entity_tree)
{
	delete _entity_tree; _entity_tree = NULL;
	_entity_tree = entity_tree;
}

const char*
FileXMLReader::Data(const char *key)
{
	if (!_entity_tree) return NULL;

	if (_entity_tree->Child(key))
	{
		const char *data = _entity_tree->ChildData(key);
		
		if (data && *data) return _entity_tree->ChildData(key);
		else return NULL;
	}
	else
	{
		return NULL;
	}
}

void
FileXMLReader::SetData(const char *key, const char *value)
{
	if (!_entity_tree) return;
	if (_entity_tree->Child(key)) _entity_tree->Child(key)->SetData(value);
	else _entity_tree->AddChild(key, NULL, value);
}

void
FileXMLReader::SetIntData(const char *key, int value)
{
	char buffer[64];
	sprintf(buffer, "%d", value);
	SetData(key, buffer);
}

void
FileXMLReader::SetFloatData(const char *key, float value)
{
	char buffer[64];
	sprintf(buffer, "%.0f", value);
	SetData(key, buffer);
}

bool
FileXMLReader::Tag(const char *key)
{
	if (!_entity_tree) return false;
	return _entity_tree->Child(key);
}

void
FileXMLReader::SetTag(const char *key, bool enabled)
{
	if (!_entity_tree) return;
	if (enabled)
	{
		if (_entity_tree->Child(key) == NULL)
			_entity_tree->AddChild(key, NULL, NULL);
	}
	else
	{
		if (_entity_tree->Child(key) != NULL)
			_entity_tree->RemoveChild(key);
	}
}

FileXMLReader::file_status
FileXMLReader::FileStatus()
{
	return _file_status;
}

void
FileXMLReader::WriteToFile()
{
	if (!_entity_tree) return;

	BFile *_fh = new BFile();

	if (_fh->SetTo(_path_name, B_CREATE_FILE | B_WRITE_ONLY | B_ERASE_FILE) == B_NO_ERROR)
	{
		char *buffer = _entity_tree->ToString();
		_fh->Write(buffer, strlen(buffer));
		free(buffer);
	}
	
	delete _fh;
}

XMLEntity*
FileXMLReader::EntityTree()
{
	return _entity_tree;
}
