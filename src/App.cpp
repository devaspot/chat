/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */
 
#include "App.h"
#include "MainWindow.h"
#include "TalkManager.h"
#include "MessageRepeater.h"
#include "ChatWindow.h"
#include "SecureSocket.h"
#include "JabberProtocol.h"
#include "version.h"

App::App(void)
	:	
	BApplication("application/x-vnd.Chat")
{
	
	MessageRepeater::Instance()->Run();
	JabberProtocol *jabber = new JabberProtocol();
	jabber->mainWindow = BlabberMainWindow::Instance();
	TalkManager::Instance()->jabber = jabber;
	BlabberMainWindow::Instance()->jabber = jabber;
	BlabberMainWindow::Instance()->Show();
}

void
App::AboutRequested()
{
	BAlert *alert = new BAlert("about", "Chat\n\n"
		"By Namdak Tonpa.\n\n"
		"Thanks to John Blanco, Mathieu Brault and "
		"Oliver Ruiz Dorantes.\n\n", "Chat!");
	BTextView *view = alert->TextView();
	BFont font;
	view->SetStylable(true);
	view->GetFont(&font);
	font.SetSize(font.Size()+7.0f);
	font.SetFace(B_BOLD_FACE);
	view->SetFontAndColor(0,4,&font);
	alert->Go();
}

int
main(void)
{
	App *app = new App();
	app->Run();
	delete app;
	return 0;
}
