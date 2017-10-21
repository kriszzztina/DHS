#-------------------------------------------------
#
# Project created by QtCreator 2016-07-09T22:11:00
#
#-------------------------------------------------

QT       += core gui sql

QTPLUGIN += qsqlmysql


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES -= UNICODE

DEFINES += _MBCS

TARGET = Medical_card
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    authorization_wnd.cpp \
    write_data.cpp \
    read_full_data.cpp \
    read_half_data.cpp \
    cardwatcher.cpp \
    change_data.cpp \
    create_patient_db.cpp

HEADERS  += mainwindow.h \
    ../rtSDK/Samples/rtPKCS11/Include/Common.h \
    ../rtSDK/Samples/rtPKCS11/Include/cryptoki.h \
    ../rtSDK/Samples/rtPKCS11/Include/pkcs11.h \
    ../rtSDK/Samples/rtPKCS11/Include/pkcs11f.h \
    ../rtSDK/Samples/rtPKCS11/Include/pkcs11t.h \
    ../rtSDK/Samples/rtPKCS11/Include/rtpkcs11.h \
    ../rtSDK/Samples/rtPKCS11/Include/rtpkcs11f.h \
    ../rtSDK/Samples/rtPKCS11/Include/rtpkcs11t.h \
    ../rtSDK/Samples/rtPKCS11/Include/rtRusPKCS11.h \
    ../rtSDK/Samples/rtPKCS11/Include/win2nix.h \
    ../rtSDK/Samples/rtPKCS11/Include/wintypes.h \
    authorization_wnd.h \
    write_data.h \
    read_full_data.h \
    read_half_data.h \
    cardwatcher.h \
    change_data.h \
    create_patient_db.h

INCLUDEPATH += ../rtSDK/Samples/rtPKCS11/Include/

FORMS    += mainwindow.ui \
    authorization_wnd.ui \
    write_data.ui \
    read_full_data.ui \
    read_half_data.ui \
    change_data.ui \
    create_patient_db.ui

RESOURCES += \
    resourses.qrc
