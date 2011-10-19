#
# qhtmlnodemodel - A QAbstractXmlNodeModel for HTML documents
#

QT -= gui
QT += xmlpatterns

unix: {
	CONFIG += link_pkgconfig
	PKGCONFIG += htmlcxx
}

INCLUDEPATH += ../src
LIBS += -L.. -lqhtmlnodemodel
