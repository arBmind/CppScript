CONFIG += staticlib
TARGET = CppScript

SOURCES += \
    CppScript/Boxed.cpp \
    CppScript/BoxedConversion.cpp

HEADERS += \
    CppScript/Boxed.h \
    CppScript/AnyType.h \
    CppScript/BoxedCast.h \
    CppScript/BoxedCastImpl.h \
    CppScript/BoxedConversion.h

include(../build/qmake/_lib.pri)
