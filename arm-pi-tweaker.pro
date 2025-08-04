QT += core widgets network

CONFIG += c++17

TARGET = arm-pi-tweaker
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Include paths for llama.cpp
INCLUDEPATH += include/3rdparty/llamacpp/include \
               include/3rdparty/llamacpp/ggml-include \
               include/3rdparty/llamacpp/common

# Source files
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    systemmanager.cpp \
    gpumanager.cpp \
    kernelmanager.cpp \
    storagemanager.cpp \
    imagebuilder.cpp \
    widgets/upgradewidget.cpp \
    widgets/customimagewizard.cpp \
    widgets/uefitab.cpp \
    widgets/rockytab.cpp \
    widgets/welcometab.cpp \
    widgets/backupwizard.cpp

# Header files
HEADERS += \
    mainwindow.h \
    systemmanager.h \
    gpumanager.h \
    kernelmanager.h \
    storagemanager.h \
    imagebuilder.h \
    widgets/upgradewidget.h \
    widgets/customimagewizard.h \
    widgets/uefitab.h \
    widgets/rockytab.h \
    widgets/welcometab.h \
    widgets/backupwizard.h

# Resource files
RESOURCES += resources.qrc

# Link llama.cpp static libraries
LIBS += -L$$PWD/include/3rdparty/lib \
        -lllama \
        -lggml \
        -lggml-base \
        -lggml-cpu \
        -lm \
        -lpthread

# OpenMP support
CONFIG += openmp
QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target