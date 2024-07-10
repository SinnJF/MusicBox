QT += quick
CONFIG += c++17

CONFIG(debug, debug | release) {
    QMAKE_CXXFLAGS_DEBUG += /MTd
}
CONFIG(release, debug | release){
    QMAKE_CXXFLAGS_RELEASE += /MT
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        controller/TranscodeManager.cpp \
        main.cpp \
        model/FunModel/convert/ConverterFactory.cpp \
        model/FunModel/convert/KGMusicConverter.cpp \
        model/FunModel/convert/MusicFactory.cpp \
        model/FunModel/convert/NEMusicConverter.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    common/Cde.h \
    common/Common.h \
    controller/TranscodeManager.h \
    model/DataModel/KGMusicData.h \
    model/DataModel/NCMusicData.h \
    model/FunModel/convert/Converter.h \
    model/FunModel/convert/ConverterFactory.h \
    model/FunModel/convert/KGMusicConverter.h \
    model/FunModel/convert/MusicFactory.h \
    model/FunModel/convert/NEMusicConverter.h

INCLUDEPATH += \
    E:\Projects\taglib-1.13.1 \
    E:\Projects\taglib-1.13.1\build \
    E:\Projects\taglib-1.13.1\taglib \
    E:\Projects\taglib-1.13.1\taglib\toolkit \
    E:\Projects\taglib-1.13.1\taglib\flac \
    E:\Projects\taglib-1.13.1\taglib\mpeg\id3v2 \
    "D:\Program Files (x86)\OpenSSL\include" \
    libs\release\zlib \
    libs\release\taglib \
    libs\release\openssl \
    libs\debug\zlib \
    libs\debug\taglib \
    libs\debug\openssl

#LIBPATH += libs
#CONFIG(debug, debug | release)
#{
#}
#CONFIG(release, debug | release)
#{
#    LIBS += /release/libcrypto.lib
#    LIBS += /release/libssl.lib
#    LIBS += /release/crypt32.lib
#    LIBS += /release/tag_c.lib
#    LIBS += /release/tag.lib
#    LIBS += /release/zlib.lib
#}

INCLUDEPATH += $$PWD/libs/release/openssl
DEPENDPATH += $$PWD/libs/release/openssl

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/release/openssl/ -llibcrypto
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/debug/openssl/ -llibcrypto

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/release/openssl/ -llibssl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/debug/openssl/ -llibssl

INCLUDEPATH += $$PWD/libs/release/taglib
DEPENDPATH += $$PWD/libs/release/taglib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/release/taglib/ -ltag
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/debug/taglib/ -ltagd

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libs/release/taglib/libtag.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libs/debug/taglib/libtagd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libs/release/taglib/tag.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libs/debug/taglib/tagd.lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/release/taglib/ -ltag_c
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/debug/taglib/ -ltag_cd

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libs/release/taglib/libtag_c.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libs/debug/taglib/libtag_cd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libs/release/taglib/tag_c.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libs/debug/taglib/tag_cd.lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/release/zlib/ -lzlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/debug/zlib/ -lzlibd

INCLUDEPATH += $$PWD/libs/release/zlib
DEPENDPATH += $$PWD/libs/release/zlib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libs/release/zlib/libzlib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libs/debug/zlib/libzlibd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libs/release/zlib/zlib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libs/debug/zlib/zlibd.lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/release/zlib/ -lzlibstatic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/release/zlib/ -lzlibstaticd

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libs/release/zlib/libzlibstatic.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libs/debug/zlib/libzlibstaticd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libs/release/zlib/zlibstatic.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libs/debug/zlib/zlibstaticd.lib
