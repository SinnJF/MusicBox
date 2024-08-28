#ifndef ANDROIDHELPER_H
#define ANDROIDHELPER_H

//#include "QtCore"
#include "qsystemdetection.h"
#include "qoperatingsystemversion.h"

//#include <QtGlobal>

#ifdef Q_OS_ANDROID
#include <QCoreApplication>
#include <QtCore/private/qandroidextras_p.h>
#include <QUrl>
#include <QFileInfo>

inline bool checkPermission() {
    QList<bool> permissions;

    auto r = QtAndroidPrivate::checkPermission("android.permission.MANAGE_EXTERNAL_STORAGE").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.MANAGE_EXTERNAL_STORAGE").result();
        if (r == QtAndroidPrivate::Denied)
            permissions.append(false);
    }
    r = QtAndroidPrivate::checkPermission("android.permission.READ_EXTERNAL_STORAGE").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.READ_EXTERNAL_STORAGE").result();
        if (r == QtAndroidPrivate::Denied)
            permissions.append(false);
    }
    r = QtAndroidPrivate::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.WRITE_EXTERNAL_STORAGE").result();
        if (r == QtAndroidPrivate::Denied)
            permissions.append(false);
    }
    r = QtAndroidPrivate::checkPermission("android.permission.MANAGE_DOCUMENTS").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.MANAGE_DOCUMENTS").result();
        if (r == QtAndroidPrivate::Denied)
            permissions.append(false);
    }
    r = QtAndroidPrivate::checkPermission("android.permission.READ_MEDIA_AUDIO").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.READ_MEDIA_AUDIO").result();
        if (r == QtAndroidPrivate::Denied)
            permissions.append(false);
    }
    r = QtAndroidPrivate::checkPermission("android.permission.WRITE_MEDIA_AUDIO").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.WRITE_MEDIA_AUDIO").result();
        if (r == QtAndroidPrivate::Denied)
            permissions.append(false);
    }
    qInfo() << "permissions: " << permissions;
    return (permissions.count() != 6);
}

inline void accessAllFiles()
{
    if(QOperatingSystemVersion::current() < QOperatingSystemVersion(QOperatingSystemVersion::Android, 11)) {
        qInfo() << "it is less then Android 11 - ALL FILES permission isn't possible!";
        return;
    }
// Here you have to set your PackageName
#define PACKAGE_NAME "package:org.qtproject.musicbox"
    jboolean value = QJniObject::callStaticMethod<jboolean>("android/os/Environment", "isExternalStorageManager");
    if(value == false) {
        qInfo() << "requesting ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION";
        QJniObject ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION = QJniObject::getStaticObjectField( "android/provider/Settings", "ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION","Ljava/lang/String;" );
        QJniObject intent("android/content/Intent", "(Ljava/lang/String;)V", ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION.object());
        QJniObject jniPath = QJniObject::fromString(PACKAGE_NAME);
        QJniObject jniUri = QJniObject::callStaticObjectMethod("android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;", jniPath.object<jstring>());
        QJniObject jniResult = intent.callObjectMethod("setData", "(Landroid/net/Uri;)Landroid/content/Intent;", jniUri.object<jobject>() );
        QtAndroidPrivate::startActivity(intent, 0);
    } else {
        qInfo() << "SUCCESS ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION";
    }
}

inline QString getRealPathFromUri(const QUrl &url)
{
    QString path = "";

    // QFileInfo info = QFileInfo(url.toString());
    // if(info.isFile())
    // {
    //     QString abs = QFileInfo(url.toString()).absoluteFilePath();
    //     if(!abs.isEmpty() && abs != url.toString() && QFileInfo(abs).isFile())
    //     {
    //         return abs;
    //     }
    // }
    // else if(info.isDir())
    // {
    //     QString abs = QFileInfo(url.toString()).absolutePath();
    //     if(!abs.isEmpty() && abs != url.toString() && QFileInfo(abs).isDir())
    //     {
    //         return abs;
    //     }
    // }
    // QString localfile = url.toLocalFile();
    // if((QFileInfo(localfile).isFile() || QFileInfo(localfile).isDir()) && localfile != url.toString())
    // {
    //     return localfile;
    // }
    QJniObject jUrl = QJniObject::fromString(url.toString());
    QJniObject jContext = QtAndroidPrivate::context();
    QJniObject jContentResolver = jContext.callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");
    QJniObject jUri = QJniObject::callStaticObjectMethod("android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;", jUrl.object<jstring>());
    QJniObject jCursor = jContentResolver.callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;", jUri.object<jobject>(), nullptr, nullptr, nullptr, nullptr);
    QJniObject jScheme = jUri.callObjectMethod("getScheme", "()Ljava/lang/String;");
    QJniObject authority;
    if(jScheme.isValid())
    {
        authority = jUri.callObjectMethod("getAuthority", "()Ljava/lang/String;");
    }
    if(authority.isValid() && authority.toString() == "com.android.externalstorage.documents")
    {
        QJniObject jPath = jUri.callObjectMethod("getPath", "()Ljava/lang/String;");
        path = jPath.toString();
    }
    else if(jCursor.isValid() && jCursor.callMethod<jboolean>("moveToFirst"))
    {
        QJniObject jColumnIndex = QJniObject::fromString("_data");
        jint columnIndex = jCursor.callMethod<jint>("getColumnIndexOrThrow", "(Ljava/lang/String;)I", jColumnIndex.object<jstring>());
        QJniObject jRealPath = jCursor.callObjectMethod("getString", "(I)Ljava/lang/String;", columnIndex);
        path = jRealPath.toString();
        if(authority.isValid() && authority.toString().startsWith("com.android.providers") && !url.toString().startsWith("content://media/external/"))
        {
            QStringList list = path.split(":");
            if(list.count() == 2)
            {
                QString type = list.at(0);
                QString id = list.at(1);
                if(type == "image")
                    type = type + "s";
                if(type == "document" || type == "documents")
                    type = "file";
                if(type == "msf")
                    type = "downloads";
                if(QList<QString>({"images","video","audio"}).contains(type))
                    type = type + "/media";
                path = "content://media/external/"+type;
                path = path + "/" + id;
                return getRealPathFromUri(path);
            }
        }
    }
    else
    {
        QJniObject jPath = jUri.callObjectMethod("getPath", "()Ljava/lang/String;");
        path = jPath.toString();
        qInfo() << QFile::exists(path) <<path;
    }

    if(path.startsWith("primary:"))
    {
        path = path.remove(0,QString("primary:").length());
        path = "/sdcard/" + path;
    }
    else if(path.startsWith("/document/primary:"))
    {
        path = path.remove(0,QString("/document/primary:").length());
        path = "/sdcard/" + path;
    }
    else if(path.startsWith("/tree/primary:"))
    {
        path = path.remove(0,QString("/tree/primary:").length());
        path = "/sdcard/" + path;
    }
    else if(path.startsWith("/storage/emulated/0/"))
    {
        path = path.remove(0,QString("/storage/emulated/0/").length());
        path = "/sdcard/" + path;
    }
    else if(path.startsWith("/tree//"))
    {
        path = path.remove(0,QString("/tree//").length());
        path = "/" + path;
    }
    if(!QFileInfo(path).isFile() && !QFileInfo(path).isDir() && !path.startsWith("/data"))
        return url.toString();
    return path;

}
#endif  //Q_OS_ANDROID

#endif // ANDROIDHELPER_H
