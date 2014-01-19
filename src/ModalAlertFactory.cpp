/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#include "ModalAlertFactory.h"

int32
ModalAlertFactory::Alert(
	const char *text,
	const char *first,
	const char *second,
	const char *third,
	button_width width,
	alert_type type,
	bool leftmost_cancel)
{
	BAlert *alert = new BAlert("", text, first, second, third, width, type);
	if (leftmost_cancel == true) alert->SetShortcut(0, B_ESCAPE);
	int32 button_pressed = alert->Go();
	return button_pressed;
}

int32
ModalAlertFactory::NonModalAlert(
	const char *text,
	const char *first,
	const char *second,
	const char *third,
	button_width width,
	alert_type type,
	bool leftmost_cancel)
{
	BAlert *alert = new BAlert("", text, first, second, third, width, type);
	if (leftmost_cancel == true) alert->SetShortcut(0, B_ESCAPE);
	alert->Go(NULL);
	return 0;
}
