#
# qhtmlnodemodel - A QAbstractXmlNodeModel for HTML documents
#

TEMPLATE = lib
DESTDIR = ..
TARGET = qhtmlnodemodel

CONFIG += staticlib create_prl

QT -= gui
QT += xmlpatterns

# Try to find htmlcxx via pkg-config
unix {
	CONFIG += link_pkgconfig
	PKGCONFIG += htmlcxx
}

HEADERS += \
	qhtmlnodemodel.h

SOURCES += \
	qhtmlnodemodel.cpp
