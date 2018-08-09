#-------------------------------------------------
#
# Project created by QtCreator 2018-08-02T10:52:26
#
#-------------------------------------------------
#---
#QT       += core gui
#QT       += phonon

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#TARGET = ddmusic
#TEMPLATE = app


#SOURCES += main.cpp\
#        mainwindow.cpp

#HEADERS  += mainwindow.h

#wince*
#{
#    DEPLOYMENT_PLUGIN += phonon_ds9 phonon_waveout
#}
#endif
 QT        += phonon

 HEADERS   += mainwindow.h
 SOURCES   += main.cpp \
              mainwindow.cpp

 # install
 target.path = $$[QT_INSTALL_EXAMPLES]/phonon/qmusicplayer
 sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES *.pro *.png images
 sources.path = $$[QT_INSTALL_EXAMPLES]/phonon/qmusicplayer
 INSTALLS += target sources

 wince*{
     DEPLOYMENT_PLUGIN += phonon_ds9 phonon_waveout
 }

 symbian {
     TARGET.UID3 = 0xA000CF6A
     include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
 }

 maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)
 symbian: warning(This example might not fully work on Symbian platform)
 simulator: warning(This example might not fully work on Simulator platform)
