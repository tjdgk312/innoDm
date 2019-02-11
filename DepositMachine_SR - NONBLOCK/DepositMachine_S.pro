#-------------------------------------------------
#
# Project created by QtCreator 2017-11-10T10:05:50
#
#-------------------------------------------------

QT       += core gui    \
            sql         \
            serialport  \
            network     \
            multimedia  \

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

sources.files = SetFiles
sources.path = /home/pi/INNOSTEC

audio.files = audio
audio.path = /home/pi/INNOSTEC

TARGET = DepositMachine_S
TEMPLATE = app

target.path = /home/pi/INNOSTEC
INSTALLS += target sources audio

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

#INCLUDEPATH += C:\Users\kimsungha\Desktop\inno_develop_test\innoSDK
#LIBS += C:\Users\kimsungha\Desktop\inno_develop_test\build-innoSDK-Desktop_Qt_5_7_1_MinGW_32bit-Debug\debug\libinnoSDK.a

#INCLUDEPATH += /mnt/hgfs/shared/innoSDK
#LIBS += /mnt/hgfs/shared_1/build-innoSDK-Rpi_Kits-Debug/libinnoSDK.a

#INCLUDEPATH += /mnt/hgfs/shared/innoSDK
#LIBS += /mnt/hgfs/shared/build-innoSDK-Rpi_Kits-Debug/libinnoSDK.a

#export QT_QPA_GENERIC_PLUGINS=evdevmouse
#export QT_QPA_EVDEV_MOUSE_PARAMETERS=/dev/input/event0
#export QT_QPA_FB_HIDECURSOR=0

INCLUDEPATH += $$PWD/printer
#INCLUDEPATH += $$PWD/plugins
#LIBS        = -L../plugins -lpnp_basictools
LIBS +=  $$PWD/libWoosimPrinter.a

INCLUDEPATH += $$PWD/PCSC
LIBS += -L$$PWD/PCSC/ -lpcsclite

#if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
#   mac:LIBS = $$member(LIBS, 0) $$member(LIBS, 1)_debug
#   win32:LIBS = $$member(LIBS, 0) $$member(LIBS, 1)d
#}

TRANSLATIONS    = SetFiles/tt2_en_device.ts \
                  SetFiles/tt2_kr_device.ts \
                  SetFiles/tt2_es_device.ts \

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    page_login.cpp \
    page_menu.cpp \
    page_loading.cpp \
    page_result.cpp \
    page_setting.cpp \
    numpad/numpad.cpp \
    keypad/keypad.cpp \
    automata/automata.cpp \
    sdkheader/innosdk.cpp \
    sdkheader/printer.cpp \
    sdkheader/user.cpp \
    sdkheader/vault.cpp \
    sdkheader/device/class_gate.cpp \
    sdkheader/device/currencycls.cpp \
    sdkheader/device/logger.cpp \
    sdkheader/device/machine.cpp \
    sdkheader/device/statethread.cpp \
    sdkheader/database/databasemanager.cpp \
    page_transcash.cpp \
    page_managermenu.cpp \
    sdkheader/device/barcode.cpp \
    page_user.cpp \
    page_engineer.cpp \
    page_search.cpp \
    sdkheader/device/finger.cpp \
    messagedialog.cpp \
    page_eventview.cpp \
    page_repair.cpp \
    page_signimage.cpp \
    class/paintarea.cpp \
    class/listitemclass.cpp \
    page_visitorlogin.cpp \
    tcpserver/tcpserver.cpp \
    tcpserver/tcpthread.cpp \
    page_devicesetup.cpp \
    page_etc.cpp \
    musicthread.cpp \
    sdkheader/device/comm.cpp \
    sdkheader/usbfinger/finger_class.cpp \
    sdkheader/usbfinger/include/SFM_Verify.c \
    sdkheader/usbfinger/include/SFM_Utils.c \
    sdkheader/usbfinger/include/SFM_Template.c \
    sdkheader/usbfinger/include/SFM_Sysparameter.c \
    sdkheader/usbfinger/include/SFM_SysConfig.c \
    sdkheader/usbfinger/include/SFM_Serial.c \
    sdkheader/usbfinger/include/SFM_Security.c \
    sdkheader/usbfinger/include/SFM_RS232.c \
    sdkheader/usbfinger/include/SFM_Packet.c \
    sdkheader/usbfinger/include/SFM_Image.c \
    sdkheader/usbfinger/include/SFM_Identify.c \
    sdkheader/usbfinger/include/SFM_GPIO.c \
    sdkheader/usbfinger/include/SFM_Factory.c \
    sdkheader/usbfinger/include/SFM_Error.c \
    sdkheader/usbfinger/include/SFM_Enroll.c \
    sdkheader/usbfinger/include/SFM_Delete.c \
    sdkheader/usbfinger/include/SFM_DB.c \
    sdkheader/usbfinger/include/SFM_Command.c

HEADERS  += mainwindow.h \
    page_login.h \
    page_menu.h \
    page_loading.h \
    page_result.h \
    page_setting.h \
    global.h \
    numpad/numpad.h \
    keypad/keypad.h \
    automata/automata.h \
    sdkheader/currency.h \
    sdkheader/sdk_global.h \
    sdkheader/innosdk.h \
    sdkheader/printer.h \
    sdkheader/user.h \
    sdkheader/vault.h \
    sdkheader/device/class_gate.h \
    sdkheader/device/currencycls.h \
    sdkheader/device/define_currency.h \
    sdkheader/device/definition.h \
    sdkheader/device/logger.h \
    sdkheader/device/machine.h \
    sdkheader/device/statethread.h \
    sdkheader/database/databasemanager.h \
    page_transcash.h \
    page_managermenu.h \
    sdkheader/device/barcode.h \
    page_user.h \
    page_engineer.h \
    page_search.h \
    sdkheader/device/finger.h \
    messagedialog.h \
    page_eventview.h \
    page_repair.h \
    page_signimage.h \
    class/paintarea.h \
    class/listitemclass.h \
    page_visitorlogin.h \
    tcpserver/tcpserver.h \
    tcpserver/tcpthread.h \
    page_devicesetup.h \
    page_etc.h \
    musicthread.h \
    sdkheader/device/comm.h \
    sdkheader/usbfinger/include/SFM_BMP.h \
    sdkheader/usbfinger/include/SFM_Command.h \
    sdkheader/usbfinger/include/SFM_Connect.h \
    sdkheader/usbfinger/include/SFM_Delete.h \
    sdkheader/usbfinger/include/SFM_Enroll.h \
    sdkheader/usbfinger/include/SFM_Error.h \
    sdkheader/usbfinger/include/SFM_Flag.h \
    sdkheader/usbfinger/include/SFM_GPIO.h \
    sdkheader/usbfinger/include/SFM_Identify.h \
    sdkheader/usbfinger/include/SFM_Image.h \
    sdkheader/usbfinger/include/SFM_Module.h \
    sdkheader/usbfinger/include/SFM_Packet.h \
    sdkheader/usbfinger/include/SFM_RS232.h \
    sdkheader/usbfinger/include/SFM_Security.h \
    sdkheader/usbfinger/include/SFM_Serial.h \
    sdkheader/usbfinger/include/SFM_SysParameter.h \
    sdkheader/usbfinger/include/SFM_Template.h \
    sdkheader/usbfinger/include/SFM_Test.h \
    sdkheader/usbfinger/include/SFM_Type.h \
    sdkheader/usbfinger/include/SFM_Utils.h \
    sdkheader/usbfinger/include/SFM_Verify.h \
    sdkheader/usbfinger/include/UF_API.h \
    sdkheader/usbfinger/finger_class.h

FORMS    += mainwindow.ui \
    page_login.ui \
    page_menu.ui \
    page_loading.ui \
    page_result.ui \
    page_setting.ui \
    numpad/numpad.ui \
    keypad/keypad.ui \
    page_transcash.ui \
    page_managermenu.ui \
    page_user.ui \
    page_engineer.ui \
    page_search.ui \
    messagedialog.ui \
    page_eventview.ui \
    page_repair.ui \
    page_signimage.ui \
    page_visitorlogin.ui \
    page_devicesetup.ui \
    page_etc.ui

RESOURCES += \
    image.qrc

DISTFILES += \
    printer/libWoosimPrinter.a
