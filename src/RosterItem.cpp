//////////////////////////////////////////////////
// Haiku Chat [RosterItem.cpp]
//////////////////////////////////////////////////

#include "RosterItem.h"
#include "AppLocation.h"
#include <TranslationUtils.h>

RosterItem::RosterItem(UserID *userid)
	: BStringItem(userid->FriendlyName().c_str()) {
	_userid           = userid;
	_is_stale_pointer = false;
}

RosterItem::~RosterItem() {
}

void RosterItem::DrawItem(BView *owner, BRect frame, bool complete)
{
	// protection
	if (StalePointer()) {
		return;
	}

	// get online status
	UserID::online_status status = _userid->OnlineStatus();
	std::string exact_status = _userid->ExactOnlineStatus();

	// text characteristics
	owner->SetFont(be_plain_font);
	owner->SetFontSize(11);
	
				//owner->GetFont(&statusFont);
				//statusFont.SetFace(B_ITALIC_FACE);
				//owner->SetFont(&statusFont);

	// clear rectangle
	if (IsSelected())
	{
		// font color is based on online status
		if (status == UserID::ONLINE)
		{
			if (exact_status == "xa") 		 owner->SetHighColor(255, 220, 220, 255);
			else if (exact_status == "away") owner->SetHighColor(255, 230, 210, 255); 
			else if (exact_status == "dnd")  owner->SetHighColor(255, 192, 192, 255); 
			else 							 owner->SetHighColor(192, 255, 192, 255); 
		}
		else if (status == UserID::OFFLINE)
		{
			owner->SetHighColor(220, 220, 220, 255);
		}
		else
		{
			owner->SetHighColor(220, 246, 250, 255);
		}
	}
	else
	{
		owner->SetHighColor(owner->ViewColor());
	}

	owner->FillRect(frame);

	float height;

	// construct name
	std::string name = GetUserID()->FriendlyName();

	if (name.empty()) {
		name = GetUserID()->Handle();

		if (name.empty()) {
			name = "<anonymous>";
		}
	}

	BFont statusFont;
			
	// font color is based on online status
	if (status == UserID::ONLINE)
	{
		if (exact_status == "xa") 			owner->SetHighColor(139, 0, 0, 255);
		else if (exact_status == "away")	owner->SetHighColor(255, 140, 0, 255);
		else if (exact_status == "dnd")		owner->SetHighColor(255, 0, 0, 255);
		else								owner->SetHighColor(0, 100, 0, 255);
	}
	else if (status == UserID::OFFLINE)
	{
		owner->SetHighColor(90, 90, 90, 255); // gray
	}
	else
	{
		owner->SetHighColor(0, 0, 255, 255); // blue
	}

	// construct text positioning
	font_height fh;
	owner->GetFontHeight(&fh);

	height = fh.ascent + fh.descent;

	// draw name
	owner->DrawString(name.c_str(),
		BPoint(frame.left/* + 13*/, frame.bottom - ((frame.Height() - height) / 2) - fh.descent));

	// draw show
	if (!GetUserID()->MoreExactOnlineStatus().empty()) {
		owner->SetHighColor(90, 90, 90, 255);

		owner->DrawString(": ");
		owner->DrawString(GetUserID()->MoreExactOnlineStatus().c_str());
		owner->DrawString("");
	}

}

void RosterItem::Update(BView *owner, const BFont *font) {
	BListItem::Update(owner, font);

	// set height to accomodate graphics and text
	SetHeight(16.0);
}

bool RosterItem::StalePointer() const {
	return _is_stale_pointer;
}

UserID *RosterItem::GetUserID() {
	if (StalePointer()) {
		return NULL;
	} else {
		return _userid;
	}
}

void RosterItem::SetStalePointer(bool is_stale) {
	_is_stale_pointer = is_stale;
}
