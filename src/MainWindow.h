/*
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                John Blanco
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Button.h>
#include <CheckBox.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <PopUpMenu.h>
#include <GridView.h>
#include <TextControl.h>
#include <Window.h>

#include "JabberProtocol.h"
#include "RosterView.h"
#include "StatusView.h"
#include "Messages.h"

class BlabberMainWindow : public BWindow {

public:

	static	BlabberMainWindow  	*Instance();
	                          	~BlabberMainWindow();

	virtual void               	MessageReceived(BMessage *msg);
	virtual void               	MenusBeginning();
	virtual	void		       	FrameResized(float width, float height);
	virtual bool               	QuitRequested();
			void				SaveConfig();

			bool               	ValidateLogin();
			void               	Login();
			void               	ShowLogin();
			void               	HideLogin();
			JabberProtocol*    	jabber;
			BCheckBox*         	fNewAccount;
			RosterView*        	fRosterView;
			StatusView*        	fStatusView;
			int					jabber_code;
			std::map<std::string,int> codes;
			BString				flavour;
		
protected:
	                           	BlabberMainWindow(BRect frame);
		                           
private:

	static	BlabberMainWindow*  fInstance;
	
			BView*              fMainView;
			BCheckBox*          fAutoLogin;
			BButton*            fLogin;
			BTextControl*       fUsername;
			BTextControl*       fPassword;
			BView*              fLoginView;
			BScrollView*        fRosterScroller;
	
			BMenuBar*           fMenuBar;
			BMenu*              fFile;
			BMenu*              fEdit;
			BMenuItem*          fDisconnect;
			BMenuItem*          fAbout;
			BMenuItem*          fQuit;
			BMenuItem*          fAddBuddy;
			BMenuItem*          fPreferences;
	
			BMenuField*			jabber_type_menu;
			BPopUpMenu*			jabber_type;
};

#endif
