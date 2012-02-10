NAME= Chat
TYPE= APP
SRCS= App.cpp JRoster.cpp \
	  RosterItem.cpp UserID.cpp BuddyWindow.cpp \
	  PeopleListItem.cpp TalkManager.cpp StatusView.cpp \
	  RosterView.cpp AppLocation.cpp \
	  RosterSuperitem.cpp ModalAlertFactory.cpp \
	  XMLEntity.cpp Settings.cpp MessageRepeater.cpp \
	  XMLReader.cpp FileXMLReader.cpp MainWindow.cpp \
	  ChatWindow.cpp EditingFilter.cpp GenericFunctions.cpp \
	  Socket.cpp SecureSocket.cpp JabberProtocol.cpp \
	  SplitView.cpp Base64.cpp ChatTextView.cpp DataForm.cpp

RSRCS= Resources.rsrc
LIBS= stdc++ be root network ssl crypto expat 
      
LIBPATHS= /boot/develop/lib/x86/ /boot/common/lib/ 
SYSTEM_INCLUDE_PATHS= /boot/develop/headers/be
LOCAL_INCLUDE_PATHS=
OPTIMIZE=-O3
#	specify any preprocessor symbols to be defined.  The symbols will not
#	have their values set automatically; you must supply the value (if any)
#	to use.  For example, setting DEFINES to "DEBUG=1" will cause the
#	compiler option "-DDEBUG=1" to be used.  Setting DEFINES to "DEBUG"
#	would pass "-DDEBUG" on the compiler's command line.
DEFINES=
#	specify special warning levels
#	if unspecified default warnings will be used
#	NONE = supress all warnings
#	ALL = enable all warnings
WARNINGS =ALL
# Build with debugging symbols if set to TRUE
SYMBOLS=
COMPILER_FLAGS=
LINKER_FLAGS=

# Generate version info
#DUMMY := $(shell version.sh)

## include the makefile-engine
include $(BUILDHOME)/etc/makefile-engine

