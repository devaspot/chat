//////////////////////////////////////////////////
//
// Haiku Chat [DataFormView.h]
//
//     XEP-0004 Data Forms implementation.
//
// Copyright (c) 2010 Maxim Sokhatsky (maxim.sokhatsky@gmail.com)
//
//////////////////////////////////////////////////

#ifndef DATAFORM_H
#define DATAFORM_H

#include <interface/View.h>
#include <interface/Window.h>
#include <interface/CheckBox.h>
#include <interface/TextControl.h>
#include <interface/ScrollView.h>
#include <String.h>
#include "XMLEntity.h"

class DataForm : BWindow
{
	public:
							DataForm(BRect rect);
							~DataForm();

				XMLEntity	*entity;
				BView *xform;
				int leftSize;
		void		LoadDataForm(XMLEntity *entity);
		virtual void		FrameResized(float width, float height);

};

#endif
