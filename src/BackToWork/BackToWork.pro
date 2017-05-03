TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lUser32

SOURCES += main.cpp \
    logger.cpp \
    appconfig.cpp \
    backtowork.cpp \
    common.cpp

HEADERS += \
    logger.h \
    appconfig.h \
    winkeys.h \
    backtowork.h \
    common.h

copyconfig.commands = $(COPY_FILE) \"$$shell_path($$PWD/btw.config)\" \"$$shell_path($$OUT_PWD/$$EXE_DIR/)\"
QMAKE_EXTRA_TARGETS += copyconfig
POST_TARGETDEPS += copyconfig
