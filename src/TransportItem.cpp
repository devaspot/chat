//////////////////////////////////////////////////
// Blabber [TransportItem.cpp]
//////////////////////////////////////////////////

#ifndef TRANSPORT_ITEM_H
	#include "TransportItem.h"
#endif

#ifndef AGENT_H
	#include "Agent.h"
#endif

#ifndef AGENT_LIST_H
	#include "AgentList.h"
#endif

#ifndef APP_LOCATION_H
	#include "AppLocation.h"
#endif

#ifndef _TRANSLATION_UTILS_H
	#include <TranslationUtils.h>
#endif

BBitmap *TransportItem::_offline_icon = NULL;
BBitmap *TransportItem::_online_icon  = NULL;
BBitmap *TransportItem::_unknown_icon = NULL;

TransportItem::TransportItem(const UserID *userid)
	: BStringItem("") {
	_userid           = userid;
	_is_stale_pointer = false;

	// intitialize static members
	if (_offline_icon == NULL) {
		std::string graphic;

		// generate path for home icon
		graphic = AppLocation::Instance()->AbsolutePath("resources/icons/online.png");

		// load graphic
		_online_icon = BTranslationUtils::GetBitmap(graphic.c_str());

		// generate path for away icon
		graphic = AppLocation::Instance()->AbsolutePath("resources/icons/offline.png");

		// load graphic
		_offline_icon = BTranslationUtils::GetBitmap(graphic.c_str());

		// generate path for away icon
		graphic = AppLocation::Instance()->AbsolutePath("resources/icons/unknown.png");

		// load graphic
		_unknown_icon = BTranslationUtils::GetBitmap(graphic.c_str());
	}
}

TransportItem::~TransportItem() {
}

void TransportItem::DrawItem(BView *owner, BRect frame, bool complete) {
	// protection
	if (StalePointer()) {
		return;
	}

	// get online status
	UserID::online_status status = _userid->OnlineStatus();

	// text characteristics
	owner->SetFont(be_plain_font);
	owner->SetFontSize(10.0);

	// clear rectangle
	if (IsSelected()) {
		if (status == UserID::TRANSPORT_ONLINE) {
			owner->SetHighColor(200, 255, 200, 255);
		} else if (status == UserID::UNKNOWN) {
			owner->SetHighColor(200, 200, 255, 255);
		} else {
			owner->SetHighColor(255, 200, 200, 255);
		}
	} else {
		owner->SetHighColor(owner->ViewColor());
	}

	owner->FillRect(frame);

	// draw a graphic
	owner->SetDrawingMode(B_OP_ALPHA);
	owner->SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);

	if (status == UserID::TRANSPORT_ONLINE) {
		owner->DrawBitmapAsync(_online_icon, BPoint(frame.left + 1, frame.top + 4));
	} else if (status == UserID::UNKNOWN) {
		owner->DrawBitmapAsync(_unknown_icon, BPoint(frame.left + 1, frame.top + 4));
	} else {
		owner->DrawBitmapAsync(_offline_icon, BPoint(frame.left + 1, frame.top + 4));
	}

	float height;

	// construct name
	std::string name = GetUserID()->FriendlyName();

	if (name.empty()) {
		name = GetUserID()->Handle();

		if (name.empty()) {
			name = "<anonymous>";
		}
	}

	// font color is based on online status
	if (status == UserID::OFFLINE) {
		owner->SetHighColor(255, 0, 0, 255);
	} else if (status == UserID::UNKNOWN) {
		owner->SetHighColor(0, 0, 255, 255);
	} else if (status == UserID::TRANSPORT_ONLINE) {
		owner->SetHighColor(0, 180, 0, 255);
	} else {
		owner->SetHighColor(0, 0, 0, 255);
	}

	// construct text positioning
	font_height fh;
	owner->GetFontHeight(&fh);

	height = fh.ascent + fh.descent;

	// draw name
	Agent *agent = AgentList::Instance()->GetAgentByID(GetUserID()->TransportID());

	if (!agent) {
		// just to be safe
		owner->DrawString(name.c_str(), BPoint(frame.left + 13, frame.bottom - ((frame.Height() - height) / 2) - fh.descent));
	} else {
		owner->DrawString(agent->Name().c_str(), BPoint(frame.left + 13, frame.bottom - ((frame.Height() - height) / 2) - fh.descent));
	}
}

void TransportItem::Update(BView *owner, const BFont *font) {
	BListItem::Update(owner, font);

	// set height to accomodate graphics and text
	SetHeight(16.0);
}

bool TransportItem::StalePointer() {
	return _is_stale_pointer;
}

const UserID *TransportItem::GetUserID() const {
	return _userid;
}

void TransportItem::SetStalePointer(bool is_stale) {
	_is_stale_pointer = is_stale;
}
