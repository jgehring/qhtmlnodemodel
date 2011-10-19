#
# qhtmlnodemodel - A QAbstractXmlNodeModel for HTML documents
#

TEMPLATE = lib
DESTDIR = ..
TARGET = qhtmlnodemodel

CONFIG += staticlib create_prl

QT -= gui
QT += xmlpatterns

HEADERS = qhtmlnodemodel.h
SOURCES = qhtmlnodemodel.cpp

# Try to find htmlcxx
!isEmpty(htmlcxx_cflags) | !isEmpty(htmlcxx_libs) {
	QMAKE_CXXFLAGS += $$htmlcxx_flags
	QMAKE_LIBS += $$htmlcxx_libs
} else:contains(QT_VERSION, ^4\\.[0-7]\\..*) {
	unix {
		CONFIG += link_pkgconfig
		PKGCONFIG += htmlcxx
	} else {
		error("Please run qmake with htmlcxx_cflags and htmlcxx_libs")
	}
} else {
	packagesExist(htmlcxxp) {
		CONFIG += link_pkgconfig
		PKGCONFIG += htmlcxx
	} else {
		error("Please run qmake with htmlcxx_cflags and htmlcxx_libs")
	}
}
