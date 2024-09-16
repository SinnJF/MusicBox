QT += quick concurrent svg core-private multimedia
#CONFIG += c++17

win32 {
    DEFINES += TAGLIB_STATIC #炸裂，debug模式下taglib会报错

    CONFIG(debug, debug | release) {
        QMAKE_CXXFLAGS_DEBUG += /MTd /NODEFAULTLIB:msvcrtd.lib
    }
    CONFIG(release, debug | release){
        QMAKE_CXXFLAGS_RELEASE += /MT /NODEFAULTLIB:msvcrt.lib
    }
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        controller/AppService.cpp \
        controller/InfoEditManager.cpp \
        controller/TranscodeManager.cpp \
        main.cpp \
        model/DataModel/AudioInfo.cpp \
        model/FunModel/convert/ConverterFactory.cpp \
        model/FunModel/convert/KGMusicConverter.cpp \
        model/FunModel/convert/MusicFactory.cpp \
        model/FunModel/convert/NEMusicConverter.cpp \
        model/FunModel/convert/QQMusicConverter.cpp

RESOURCES += qml.qrc \
    res.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    common/AndroidHelper.h \
    common/Cde.h \
    common/Common.h \
    common/Logger.h \
    controller/AppService.h \
    controller/InfoEditManager.h \
    controller/TranscodeManager.h \
    model/DataModel/AudioInfo.h \
    model/DataModel/KGMusicData.h \
    model/DataModel/NCMusicData.h \
    model/DataModel/QQMusicData.h \
    model/FunModel/convert/Converter.h \
    model/FunModel/convert/ConverterFactory.h \
    model/FunModel/convert/KGMusicConverter.h \
    model/FunModel/convert/MusicFactory.h \
    model/FunModel/convert/NEMusicConverter.h \
    model/FunModel/convert/QQMusicConverter.h

INCLUDEPATH += \
    E:/Projects/taglib-1.13.1 \
    E:/Projects/taglib-1.13.1/taglib \
    E:/Projects/taglib-1.13.1/taglib/toolkit \
    E:/Projects/taglib-1.13.1/taglib/flac \
    E:/Projects/taglib-1.13.1/taglib/mpeg/id3v2 \

win32 {
    INCLUDEPATH += \
        E:/Projects/taglib-1.13.1/build \
        "D:/Program Files (x86)/OpenSSL/include" \
}

#INCLUDEPATH += $$PWD/libs/release/openssl
#DEPENDPATH += $$PWD/libs/release/openssl

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/release/openssl/ -llibcrypto
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/debug/openssl/ -llibcrypto

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/release/openssl/ -llibssl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/debug/openssl/ -llibssl


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/release/taglib/ -ltag
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/debug/taglib/ -ltag

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/release/taglib/ -ltag_c
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/debug/taglib/ -ltag_cd

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/release/zlib/ -lzlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/debug/zlib/ -lzlibd

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/release/zlib/ -lzlibstatic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/release/zlib/ -lzlibstaticd

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml \
    android/res/xml/qtprovider_paths.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
android: include(C:/android-sdk-windows/android_openssl/openssl.pri)
android: {
    INCLUDEPATH += C:/android-sdk-windows/android_openssl/ssl_1.1/include \
                   "E:\Projects\taglib-1.13.1\build\Qt_6_7_2_Clang_armeabi_v7a-MinSizeRel" \


    LIBS += "C:\android-sdk-windows\android_openssl\ssl_1.1\armeabi-v7a\libcrypto.a" \
            "C:\android-sdk-windows\android_openssl\ssl_1.1\armeabi-v7a\libssl.a" \
            "E:\Projects\taglib-1.13.1\build\Qt_6_7_2_Clang_armeabi_v7a-MinSizeRel\taglib\libtag.a" \
            "E:\Projects\taglib-1.13.1\build\Qt_6_7_2_Clang_armeabi_v7a-MinSizeRel\bindings\c\libtag_c.a"
}
