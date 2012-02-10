#include "DataForm.h"


DataForm::DataForm(BRect rect)
: BWindow(rect, "DataForm", B_DOCUMENT_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	BRect b = Bounds();
	

	
	xform = new BView(b, "form1", B_FOLLOW_ALL, B_WILL_DRAW | B_FRAME_EVENTS);
	xform->SetViewColor(216, 216, 216, 255);
	BScrollView *scrolled_form = new BScrollView("form2", xform, B_FOLLOW_ALL, false, true);
	xform->TargetedByScrollView(scrolled_form);

	scrolled_form->MoveTo(-2,-2);
	scrolled_form->ResizeBy(rect.Width()+4,rect.Height()-10);	
	
	BView *enveloped_view = new BView(b, NULL, B_FOLLOW_ALL, 0);
	enveloped_view->AddChild(scrolled_form);
	
	AddChild(enveloped_view);
	
	leftSize = rect.Width() * 0.34;
	
	Show();
					
}

DataForm::~DataForm()
{
}

void
DataForm::FrameResized(float width, float height)
{
	BWindow::FrameResized(width, height);
	
	
	//fprintf(stderr, "FrameResized");
	for (int i =0; i<xform->CountChildren();i++)
		xform->ChildAt(i)->Invalidate();
}

void
DataForm::LoadDataForm(XMLEntity *_entity)
{
	entity = _entity;
	
	if (entity->Child("query")) {
		entity = entity->Child("query");
	} else {
		return;
	}
	
	if (entity->Child("x")) {
		entity = entity->Child("x");
	} else {
		return;
	}
	
	
	BRect rect = Bounds();
	
	rect.top += 50;

	for (int i=0; i<entity->CountChildren(); ++i)
	{
		if (!strcasecmp(entity->Child(i)->Name(), "instructions"))
		{
			continue;
		} 
		else if (!strcasecmp(entity->Child(i)->Name(), "title"))
		{
			SetTitle(entity->Child(i)->Data());
		} 
		else if (!strcasecmp(entity->Child(i)->Name(), "field"))
		{
			if (!strcasecmp(entity->Child(i)->Attribute("type"), "boolean"))
			{
				BRect box_rect(rect);
				
				box_rect.OffsetBy(leftSize, 0);
				box_rect.right = Bounds().right - 20;
				box_rect.bottom = box_rect.top + 19.0;
				BCheckBox *checkBox = new BCheckBox(box_rect,
					entity->Child(i)->Attribute("var"),
					entity->Child(i)->Attribute("label"), NULL, B_FOLLOW_LEFT_RIGHT);
				xform->AddChild(checkBox);
				rect.OffsetBy(0.0, 23.0);
				
			}
			else if (!strcasecmp(entity->Child(i)->Attribute("type"), "hidden"))
			{
				
			}
			else if (!strcasecmp(entity->Child(i)->Attribute("type"), "fixed"))
			{
				
			}
			else if (!strcasecmp(entity->Child(i)->Attribute("type"), "jid-multi"))
			{
				
			}
			else if (!strcasecmp(entity->Child(i)->Attribute("type"), "jid-single"))
			{
				
			}
			else if (!strcasecmp(entity->Child(i)->Attribute("type"), "list-multi"))
			{
				
			}
			else if (!strcasecmp(entity->Child(i)->Attribute("type"), "text-multi"))
			{
				
			}
			else if (!strcasecmp(entity->Child(i)->Attribute("type"), "text-single"))
			{
				BString s;
				s.Append(entity->Child(i)->Attribute("label"));
				
				int lines = s.Length() / 22;
				
				BRect label_rect;
				label_rect.top = rect.top + 2;
				label_rect.bottom = label_rect.top + lines * 13;
				label_rect.left = Bounds().left;
				label_rect.right = label_rect.left + leftSize - 10;
				
				BTextView *label = new BTextView(BRect(0,0,leftSize-4,Bounds().bottom),
					entity->Child(i)->Attribute("var"), label_rect,	B_FOLLOW_LEFT);
				label->SetAlignment(B_ALIGN_RIGHT); 
				label->SetWordWrap(true);
				label->SetText(entity->Child(i)->Attribute("label"));
				label->MakeEditable(false);
				label->SetViewColor(216, 216, 216, 255);
		
				BRect text_rect(rect);
				text_rect.top += 2;
				text_rect.right -= 20;
				text_rect.bottom = text_rect.top + 23;
	
				BTextControl *textBox = new BTextControl(text_rect,
					entity->Child(i)->Attribute("var"),
					"", entity->Child(i)->Child("value")->Data(), NULL, B_FOLLOW_LEFT_RIGHT);
				textBox->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
				textBox->SetDivider(leftSize);
				
				xform->AddChild(textBox);			
				xform->AddChild(label);

				rect.OffsetBy(0.0, lines == 1 ? 25 : lines * 13 + 2);
							
			}
			else if (!strcasecmp(entity->Child(i)->Attribute("type"), "text-private"))
			{
			}
			else if (!strcasecmp(entity->Child(i)->Attribute("type"), "list-singlee"))
			{
			}
		} 
		
		
	}
}

