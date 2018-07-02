TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    appimp.cpp \
    cJSON.cpp \
    ckafkaproducer.cpp \
    configpara.cpp \
    recssl.cpp \
    tinyxml2.cpp \
    topkg.cpp
INCLUDEPATH +=/usr/local/include/
INCLUDEPATH +=/opt/instantclient_11_2/sdk/include

LIBS +=-L../lib
LIBS +=-L/opt/instantclient_11_2/  -locci -lclntsh -D_REENTRANT  -fpermissive -lstdc++ -lzlog -lcrypto -lrdkafka
LIBS +=-L/usr/local/lib/ -levpp -levpp_boost

HEADERS += \
    appimp.h \
    cJSON.h \
    ckafkaproducer.h \
    configpara.h \
    recssl.h \
    tinyxml2.h \
    topkg.h
