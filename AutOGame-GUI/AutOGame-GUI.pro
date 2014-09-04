#-------------------------------------------------
#
# Project created by QtCreator 2014-09-03T22:46:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AutOGame-GUI
TEMPLATE = app


SOURCES += main.cpp\
        autogame.cpp \
    loading.cpp \
    worker.cpp

HEADERS  += autogame.h \
    loading.h \
    worker.h

FORMS    += autogame.ui \
    loading.ui

CONFIG += mobility
MOBILITY = 

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    android/AndroidManifest.xml

DEFINES += CURL_STATICLIB
QMAKE_CXXFLAGS += -DCURL_STATICLIB

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Autogame/Build/src/release/ -lAutOgame
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Autogame/Build/src/debug/ -lAutOgame
else:unix: LIBS += -L$$PWD/../Autogame/Build/src/ -lAutOgame

INCLUDEPATH += $$PWD/../Autogame/src/Init
INCLUDEPATH += $$PWD/../Autogame/src/Common
DEPENDPATH += $$PWD/../Autogame/src/Init
DEPENDPATH += $$PWD/../Autogame/src/Common

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/src/release/libAutOgame.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/src/debug/libAutOgame.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/src/release/AutOgame.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/src/debug/AutOgame.lib
else:unix: PRE_TARGETDEPS += $$PWD/../Autogame/Build/src/libAutOgame.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Autogame/Build/dep/tinyxml2/release/ -ltinyxml2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Autogame/Build/dep/tinyxml2/debug/ -ltinyxml2
else:unix: LIBS += -L$$PWD/../Autogame/Build/dep/tinyxml2/ -ltinyxml2

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/tinyxml2/release/libtinyxml2.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/tinyxml2/debug/libtinyxml2.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/tinyxml2/release/tinyxml2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/tinyxml2/debug/tinyxml2.lib
else:unix: PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/tinyxml2/libtinyxml2.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Autogame/Build/dep/libtidy/release/ -llibtidy
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Autogame/Build/dep/libtidy/debug/ -llibtidy
else:unix: LIBS += -L$$PWD/../Autogame/Build/dep/libtidy/ -llibtidy

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/libtidy/release/liblibtidy.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/libtidy/debug/liblibtidy.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/libtidy/release/libtidy.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/libtidy/debug/libtidy.lib
else:unix: PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/libtidy/liblibtidy.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Autogame/Build/dep/curl/lib/release/ -llibcurl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Autogame/Build/dep/curl/lib/debug/ -llibcurl
else:unix: LIBS += -L$$PWD/../Autogame/Build/dep/curl/lib/ -llibcurl

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/curl/lib/release/liblibcurl.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/curl/lib/debug/liblibcurl.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/curl/lib/release/libcurl.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/curl/lib/debug/libcurl.lib
else:unix: PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/curl/lib/liblibcurl.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Autogame/Build/dep/restclient-cpp/release/ -lrestclient-cpp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Autogame/Build/dep/restclient-cpp/debug/ -lrestclient-cpp
else:unix: LIBS += -L$$PWD/../Autogame/Build/dep/restclient-cpp/ -lrestclient-cpp

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/restclient-cpp/release/librestclient-cpp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/restclient-cpp/debug/librestclient-cpp.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/restclient-cpp/release/restclient-cpp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/restclient-cpp/debug/restclient-cpp.lib
else:unix: PRE_TARGETDEPS += $$PWD/../Autogame/Build/dep/restclient-cpp/librestclient-cpp.a

LIBS += -lws2_32 -lAdvapi32
