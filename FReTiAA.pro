#-------------------------------------------------
#
# Project created by QtCreator 2017-10-26T18:18:11
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = FReTiAA
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


#Application version
VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_BUILD = 0

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"\
       "VERSION_BUILD=$$VERSION_BUILD"

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot/qcustomplot.cpp \
    levelmeter.cpp \
    multimeter.cpp \
    dbmeter.cpp \
    audioengine.cpp \
    oscillogram.cpp \
    audioinputconfigdial.cpp \
    gainconfigdial.cpp \
    clickablelabel.cpp \
    aboutdialog.cpp \
    spectrum.cpp \
    octavespectrum.cpp \
    onenoctavefilters.cpp \
    biquad.cpp \
    spectrogram.cpp \
    xyrangedialog.cpp \
    datasharer.cpp \
    qfgraph.cpp \
    qcpitemtracerbars.cpp \
    qcpitemtracercolormap.cpp

HEADERS  += mainwindow.h \
    qcustomplot/qcustomplot.h \
    levelmeter.h \
    multimeter.h \
    dbmeter.h \
    audioengine.h \
    oscillogram.h \
    audioinputconfigdial.h \
    gainconfigdial.h \
    clickablelabel.h \
    aboutdialog.h \
    datasharer.h \
    spectrum.h \
    ffft/Array.h \
    ffft/Array.hpp \
    ffft/def.h \
    ffft/DynArray.h \
    ffft/DynArray.hpp \
    ffft/FFTReal.h \
    ffft/FFTReal.hpp \
    ffft/FFTRealFixLen.h \
    ffft/FFTRealFixLen.hpp \
    ffft/FFTRealFixLenParam.h \
    ffft/FFTRealPassDirect.h \
    ffft/FFTRealPassDirect.hpp \
    ffft/FFTRealPassInverse.h \
    ffft/FFTRealPassInverse.hpp \
    ffft/FFTRealSelect.h \
    ffft/FFTRealSelect.hpp \
    ffft/FFTRealUseTrigo.h \
    ffft/FFTRealUseTrigo.hpp \
    ffft/OscSinCos.h \
    ffft/OscSinCos.hpp \
    octavespectrum.h \
    onenoctavefilters.h \
    biquad.h \
    spectrogram.h \
    xyrangedialog.h \
    qfgraph.h \
    qcpitemtracerbars.h \
    qcpitemtracercolormap.h

RESOURCES += \
    ressources.qrc

DISTFILES += \
    NOTES \
    AUTHORS \
    LICENSE
