//////////////////////////////////////////////////
// Haiku Chat [ModalAlertFactory.cpp]
//////////////////////////////////////////////////

#ifndef MODAL_ALERT_FACTORY_H
	#include "ModalAlertFactory.h"
#endif

#ifndef MACROS_H
	#include "macros.h"
#endif

int32 ModalAlertFactory::Alert(const char *text, const char *first, const char *second, const char *third, button_width width, alert_type type, bool leftmost_cancel) {
	// create alert
	BAlert *alert = new BAlert("", text, first, second, third, width, type);

	// the leftmost button may be a cancel
	if (leftmost_cancel == true)
		alert->SetShortcut(0, B_ESCAPE);

	// display alert and return the button ID pressed
	int32 button_pressed = alert->Go();

	// return the button press
	return button_pressed;
}

int32 ModalAlertFactory::NonModalAlert(const char *text, const char *first, const char *second, const char *third, button_width width, alert_type type, bool leftmost_cancel) {
	// create alert
	BAlert *alert = new BAlert("", text, first, second, third, width, type);

	// the leftmost button may be a cancel
	if (leftmost_cancel == true)
		alert->SetShortcut(0, B_ESCAPE);

	// display alert and return the button ID pressed
	alert->Go(NULL);

	// return the button press
	return 0;
}
