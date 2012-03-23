#-------------------------------------------------
#
# Project created by QtCreator 2012-03-22T15:21:03
#
#-------------------------------------------------

cmake_minimum_required(VERSION 2.6)
PROJECT(trayfreq-qt)

find_package(Qt4 REQUIRED)
find_library(CPUFREQ cpufreq)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

SET(trayfreq_SOURCES main.cpp freqtray.cpp)
SET(trayfreq_HEADERS freqtray.h)
SET(trayfreq_RESOURCES cpufreq.qrc)

SET(CMAKE_CXX_FLAGS "-O2 -march=native -ftree-vectorize -fomit-frame-pointer")

QT4_WRAP_CPP(trayfreq_HEADERS_MOC ${trayfreq_HEADERS})
QT4_ADD_RESOURCES(trayfreq_RESOURCES_RCC ${trayfreq_RESOURCES})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_EXECUTABLE(trayfreq-qt ${trayfreq_SOURCES}
                        ${trayfreq_HEADERS_MOC}
                        ${trayfreq_RESOURCES_RCC}
                )
TARGET_LINK_LIBRARIES(trayfreq-qt ${QT_LIBRARIES} ${CPUFREQ})

INSTALL(TARGETS trayfreq-qt
    RUNTIME DESTINATION "$ENV{DESTDIR}/usr/bin"
    )
