/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */
 
#ifndef APP_H
#define APP_H

#include <Application.h>

class App : public BApplication
{

public:

					App(void);
			void 	AboutRequested();

};

#endif
