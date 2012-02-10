//////////////////////////////////////////////////
// Haiku Chat [ModalAlertFactory.h]
//////////////////////////////////////////////////

#ifndef MODAL_ALERT_FACTORY_H
#define MODAL_ALERT_FACTORY_H

#ifndef _ALERT_H
	#include <interface/Alert.h>
#endif

class ModalAlertFactory {
public:
	static int32 Alert(const char *text, const char *first, const char *second = NULL, const char *third = NULL, button_width width = B_WIDTH_FROM_LABEL, alert_type type = B_INFO_ALERT, bool leftmost_cancel = true);
	static int32 NonModalAlert(const char *text, const char *first, const char *second = NULL, const char *third = NULL, button_width width = B_WIDTH_FROM_LABEL, alert_type type = B_INFO_ALERT, bool leftmost_cancel = true);
};

#endif
