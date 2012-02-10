//////////////////////////////////////////////////
// Interface [StatusView.cpp]
//////////////////////////////////////////////////

#ifndef STATUS_VIEW_H
	#include "StatusView.h"
#endif

#include <ScrollView.h>

StatusView::StatusView(const char *name)
	: BView(BRect(0, 0, 0, 0), name, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM, B_WILL_DRAW) {
	// set font
	SetFont(be_plain_font);
	SetFontSize(9.0);
}

StatusView::~StatusView() {
}

void StatusView::AttachedToWindow() {
	// resize to connect to bottom of parent
	BRect rect = Parent()->Bounds();

	GetFontHeight(&_fh);

	float height = _fh.ascent + _fh.descent + 1.0;
	
	if (height <  B_V_SCROLL_BAR_WIDTH)
		height = B_V_SCROLL_BAR_WIDTH;
		
	
	_height =  height  - _fh.descent - 1.0;	
	
	// set new size
	ResizeTo(rect.Width(), height);
	MoveTo(0.0, rect.bottom - height + 1 );
}

void StatusView::Draw(BRect rect) {
	// draw indent
	SetHighColor(152, 152, 152, 255);
	StrokeLine(BPoint(0.0, 0.0), BPoint(Bounds().Width(), 0.0));

	SetHighColor(255, 255, 255, 255);
	StrokeLine(BPoint(0.0, 1.0), BPoint(Bounds().Width(), 1.0));

	// draw name
	SetHighColor(0, 0, 0, 255);
	DrawString(_current_message.c_str(), BPoint(2, _height));
}

void StatusView::SetMessage(std::string message) {
	_current_message = message;

	// redraw self
	Invalidate();
}

const std::string StatusView::Message() const {
	return _current_message;
}

const float StatusView::GetHeight() const {
	return _height;
}
