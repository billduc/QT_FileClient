# allows to add DEPLOYMENTFOLDERS and links to the V-Play library and QtCreator auto-completion
CONFIG += v-play

qmlFolder.source = qml
DEPLOYMENTFOLDERS += qmlFolder # comment for publishing

assetsFolder.source = assets
DEPLOYMENTFOLDERS += assetsFolder

# Add more folders to ship with the application here

RESOURCES += #    resources.qrc # uncomment for publishing

LIBS += -L/usr/lib -lssl -lcrypto

# NOTE: for PUBLISHING, perform the following steps:
# 1. comment the DEPLOYMENTFOLDERS += qmlFolder line above, to avoid shipping your qml files with the application (instead they get compiled to the app binary)
# 2. uncomment the resources.qrc file inclusion and add any qml subfolders to the .qrc file; this compiles your qml files and js files to the app binary and protects your source code
# 3. change the setMainQmlFile() call in main.cpp to the one starting with "qrc:/" - this loads the qml files from the resources
# for more details see the "Deployment Guides" in the V-Play Documentation

# during development, use the qmlFolder deployment because you then get shorter compilation times (the qml files do not need to be compiled to the binary but are just copied)
# also, for quickest deployment on Desktop disable the "Shadow Build" option in Projects/Builds - you can then select "Run Without Deployment" from the Build menu in Qt Creator if you only changed QML files; this speeds up application start, because your app is not copied & re-compiled but just re-interpreted


# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    resources/connection.cpp \
    resources/filehandle.cpp \
    resources/manageconnection.cpp \
    resources/md5.cpp \
    resources/packet.cpp \
    resources/uploadfile.cpp


android {
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    OTHER_FILES += android/AndroidManifest.xml       android/build.gradle
}

ios {
    QMAKE_INFO_PLIST = ios/Project-Info.plist
    OTHER_FILES += $$QMAKE_INFO_PLIST
    
}

# set application icons for win and macx
win32 {
    RC_FILE += win/app_icon.rc
}
macx {
    ICON = macx/app_icon.icns
}

HEADERS += \
    header/connection.h \
    header/filehandle.h \
    header/filesys.h \
    header/manageconnection.h \
    header/md5.h \
    header/packet.h \
    header/uploadfile.h

DISTFILES += \
    qml/login.qml \
    qml/userPage.qml \
    qml/filePage.qml \
    image/file.jpg \
    image/add-page.png \
    image/cancel.png \
    image/checked.png \
    image/doc.png \
    image/down.png \
    image/download-file.png \
    image/download.png \
    image/file.png \
    image/folder.png \
    image/icon.png \
    image/jpg.png \
    image/json-file.png \
    image/mp3.png \
    image/newfile.png \
    image/newfile1.png \
    image/pdf.png \
    image/png.png \
    image/readjson.png \
    image/txt.png \
    image/download.svg \
    image/file.jpg \
    image/txt.png \
    image/readjson.png \
    image/png.png \
    image/pdf.png \
    image/newfile1.png \
    image/newfile.png \
    image/mp3.png \
    image/json-file.png \
    image/jpg.png \
    image/icon.png \
    image/folder.png \
    image/file.png \
    image/download-file.png \
    image/download.png \
    image/down.png \
    image/doc.png \
    image/checked.png \
    image/cancel.png \
    image/add-page.png \
    image/download.svg
