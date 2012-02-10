//////////////////////////////////////////////////
// Haiku Chat [PeopleListItem.cpp]
//////////////////////////////////////////////////

#include "PeopleListItem.h"
#include <interface/Font.h>
#include <interface/View.h>
#include "JabberProtocol.h"

PeopleListItem::PeopleListItem(std::string whoami, std::string user,
	std::string show, std::string status, std::string role, std::string affiliation)
	: BListItem()
{
	_user   = user;
	_whoami = whoami;
	_show = show;
	_status = status;
	_role = role;
	_affiliation = affiliation;
}

PeopleListItem::~PeopleListItem() {
}

void PeopleListItem::DrawItem(BView *owner, BRect frame, bool complete)
{
	// text characteristics
	owner->SetFont(be_plain_font);
	owner->SetFontSize(11.0);
	
	// construct text positioning
	font_height fh;
	owner->GetFontHeight(&fh);
	
	// clear rectangle
	if (IsSelected())
	{
		// font color is based on online status
		if (_show == "xa") 		  owner->SetHighColor(255, 220, 220, 255);
		else if (_show == "away") owner->SetHighColor(255, 230, 210, 255); 
		else if (_show == "dnd")  owner->SetHighColor(255, 192, 192, 255); 
		else 					  owner->SetHighColor(192, 255, 192, 255); 
		owner->SetLowColor(owner->HighColor());

	} else {
		owner->SetHighColor(owner->ViewColor());
		owner->SetLowColor(owner->HighColor());
	}
	
	owner->FillRect(frame);

	float height = fh.ascent + fh.descent;
	
	owner->SetHighColor(90, 90, 90, 255);
	
	std::string sign = "";
	int sign_offset = 0;
	
	if (_affiliation == "owner")
		sign = "!!!";
	else if (_affiliation == "admin")
		sign = "@";
	else if (_affiliation == "member")
		sign = "+";
	else if (_affiliation == "none")
	{
		if (_role == "admin")
			sign = "@";
		else if (_role == "moderator")
			sign = "@";
		else if (_role == "participant")
			sign = "+";
		else 
			sign = "";
	}
	owner->DrawString(sign.c_str(), BPoint(frame.left, frame.bottom - ((frame.Height() - height) / 2) - fh.descent));
	
	// standard text color
	if (_show == "xa") 		   owner->SetHighColor(139, 0, 0, 255);
	else if (_show == "away")  owner->SetHighColor(255, 140, 0, 255);
	else if (_show == "dnd")   owner->SetHighColor(255, 0, 0, 255);
	else					   owner->SetHighColor(0, 100, 0, 255);

	// draw information
	
	owner->DrawString(User().c_str(),
		BPoint(frame.left + 12.0, frame.bottom - ((frame.Height() - height) / 2) - fh.descent));
	
	if (!_status.empty())
	{
		owner->SetHighColor(90, 90, 90, 255);

		owner->DrawString(": ");
		owner->DrawString(_status.c_str());
		owner->DrawString("");
	}
}

void PeopleListItem::Update(BView *owner, const BFont *font) {
	BListItem::Update(owner, font);

	// set height to accomodate graphics and text
	SetHeight(13.0);
}

std::string PeopleListItem::User() const {
	return _user;
}
