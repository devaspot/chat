//////////////////////////////////////////////////
// Interface [StatusView.h]
//////////////////////////////////////////////////

#ifndef STATUS_VIEW_H
#define STATUS_VIEW_H

#include <string>

#include <interface/View.h>

class StatusView : public BView {
public:
						StatusView(const char *name = NULL);
						~StatusView();

	void				AttachedToWindow();
	void				Draw(BRect rect);

	void				SetMessage(std::string message);
	const std::string	Message() const;

	const float			GetHeight() const;

private:
	std::string		_current_message;
	float			_height;
	font_height		_fh;
};

#endif
