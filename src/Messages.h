/*
 * Copyright 2010-2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *                Maxim Sokhatsky <maxim@synrc.com>
 *
 */

#ifndef MESSAGES_H
#define MESSAGES_H

// generic
#define JAB_OK                          0x12000000
#define JAB_CANCEL                      0x12000FFF

// flavours
#define JAB_COMMON                      0x12000001
#define JAB_FACEBOOK                    0x12000002
#define JAB_GOOGLE                      0x12000003

// GUI
#define JAB_CLOSE                       0x13000000
#define JAB_ABOUT                       0x13003333
#define JAB_QUIT                        0x13006666
#define JAB_PREFERENCES                 0x1300AAA0
#define JAB_PREFERENCES_DATAFORM		0x1300AAA1
#define JAB_AFFILIATIONS                0x1300AAA2
#define JAB_JABBER_ORG                  0x1300DDDD
#define JAB_JABBER_CENTRAL_ORG          0x1300DDDE
#define JAB_JABBER_VIEW_COM             0x1300DDDF
#define JAB_USER_GUIDE                  0x1300EEEE
#define JAB_RIV                         0x1300EEEF
#define JAB_FAQ                         0x1300EEF0
#define JAB_BE_USERS                    0x1300EEF1

// communication
#define JAB_OPEN_MESSAGE                0x14000000
#define JAB_OPEN_NEW_MESSAGE            0x14000888
#define JAB_CLOSE_MESSAGE               0x14001000
#define JAB_OPEN_CHAT                   0x14001888
#define JAB_OPEN_CHAT_WITH_DOUBLE_CLICK 0x14001889
#define JAB_OPEN_NEW_CHAT               0x14002000
#define JAB_CLOSE_CHAT                  0x14002888
#define JAB_OPEN_NEW_GROUP_CHAT         0x14002900
#define JAB_CLOSE_GROUP_CHAT            0x14002904
#define JAB_MESSAGE_SENT                0x14004000
#define JAB_MESSAGE_RECV                0x14004888
#define JAB_CHAT_SENT                   0x14005000
#define JAB_CHAT_RECV                   0x14005888
#define JAB_GROUP_CHATTER_ONLINE        0x14006000
#define JAB_GROUP_CHATTER_OFFLINE       0x14006888

#define JAB_MESSAGE_1                   0x14007000
#define JAB_MESSAGE_2                   0x14007888
#define JAB_MESSAGE_3                   0x14008000
#define JAB_MESSAGE_4                   0x14008888
#define JAB_MESSAGE_5                   0x14009000
#define JAB_MESSAGE_6                   0x14009888
#define JAB_MESSAGE_7                   0x1400A888
#define JAB_MESSAGE_8                   0x1400B000
#define JAB_MESSAGE_9                   0x1400B888

#define NEWLINE_TOGGLE                  0x1400B999

#define JAB_START_RECORD                0x1400C000
#define JAB_STOP_RECORD                 0x1400C888
#define JAB_SHOW_CHATLOG                0x1400C001
#define JAB_CLOSE_TALKS                 0x1400C999
#define JAB_ROTATE_CHAT_FORWARD         0x1400D000
#define JAB_ROTATE_CHAT_BACKWARD        0x1400D001
#define JAB_FOCUS_BUDDY                 0x1400D002

// roster
#define JAB_OPEN_ADD_BUDDY_WINDOW       0x15000000
#define JAB_ADD_NEW_BUDDY               0x15001000
#define JAB_ADD_BUDDY_FROM_ROSTER       0x15002000
#define JAB_OPEN_EDIT_BUDDY_WINDOW      0x15003000
#define JAB_EDIT_BUDDY                  0x15004000
#define JAB_REMOVE_BUDDY                0x15005000
#define JAB_GOT_ROSTER_FROM_SERVER      0x15006000
#define JAB_USER_INFO                   0x15007000

// jabber
#define JAB_CONNECT                     0x16000000
#define JAB_CONNECTING                  0x16000001
#define JAB_RECONNECTING                0x16000002
#define JAB_GOT_SERVER_INFO             0x16000003
#define JAB_RESET                       0x16001000
#define JAB_DISCONNECT                  0x16002000
#define JAB_DISCONNECTED                0x16003000
#define JAB_ABNORMAL_DISCONNECT         0x16004000
#define JAB_LOGIN                       0x16006000
#define JAB_LOGGED_IN                   0x16007000
#define JAB_LOGOUT                      0x16008000

// presence
#define JAB_SUBSCRIBE_PRESENCE          0x17001001
#define JAB_UNSUBSCRIBE_PRESENCE        0x17001002
#define JAB_REVOKE_PRESENCE        		0x17001003
#define JAB_RESEND_PRESENCE        		0x17001004

// status
#define JAB_AVAILABLE_FOR_CHAT         0x17003000
#define JAB_DO_NOT_DISTURB             0x17003001
#define JAB_AWAY_TEMPORARILY           0x17003002
#define JAB_AWAY_EXTENDED              0x17003003
#define JAB_CUSTOM_STATUS              0x17003004

// transports
#define AGENT_MENU_CHANGED_TO_AIM       0x18000000
#define AGENT_MENU_CHANGED_TO_YAHOO     0x18000001
#define AGENT_MENU_CHANGED_TO_ICQ       0x18000002
#define AGENT_MENU_CHANGED_TO_MSN       0x18000003
#define AGENT_MENU_CHANGED_TO_JABBER    0x18000004
#define REGISTER_TRANSPORT              0x18000005
#define UNREGISTER_TRANSPORT            0x18000006
#define TRANSPORT_UPDATE                0x18000007
#define AGENT_MENU_CHANGED_TO_JABBER_CONFERENCE    0x18000008

#endif
