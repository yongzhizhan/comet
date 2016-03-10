TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
DESTDIR = build
INCLUDEPATH += ../cpprest/build/inc ../kaiwu/build/inc

LIBS += -L../cpprest/build -lcpprestd-1.0
LIBS += -L../kaiwu/build -lkaiwud-1.0
LIBS += -L"/usr/local/lib/" -lgtest -lpthread -levent

SOURCES += \
    main.cc \
    server.cc \
    subscriber.cc

HEADERS += \
    server.h \
    channel.h \
    subscriber.h

