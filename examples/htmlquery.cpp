/*
 * qhtmlnodemodel - A QAbstractXmlNodeModel for HTML documents
 * Copyright (C) 2011 Jonas Gehring
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holders nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#include <QCoreApplication>
#include <QFile>
#include <QXmlQuery>
#include <QXmlSerializer>

#include "qhtmlnodemodel.h"


// Opens a file for reading, using stdout if the path is -
static bool openFile(QFile *f, const QString &path)
{
	if (path == "-") {
		return f->open(stdin, QIODevice::ReadOnly);
	} else {
		f->setFileName(path);
		return f->open(QIODevice::ReadOnly);
	}
}

// Program entry point
int main(int argc, char **argv)
{
	// Validate arguments
	if (argc < 3) {
		qFatal("Usage: %s <html-file> <xquery-file>\nUse - to read from stdin", argv[0]);
	}
	if (!strcmp(argv[1], "-") && !strcmp(argv[2], "-")) {
		qFatal("Err, cannot read both HTML and XQuery file from stdin");
	}

	// We'll need a QCoreApplication instance for this
	QCoreApplication app(argc, argv);

	// Setup query first, so we can use its name pool
	QXmlQuery query;

	// Read HTML data
	QFile htmlFile;
	if (!openFile(&htmlFile, argv[1])) {
		qFatal("Err, can't open HTML file %s", argv[1]);
	}
	QByteArray source = htmlFile.readAll();
	QHtmlNodeModel model(query.namePool(), source, QUrl::fromLocalFile(argv[1]));

	// Bind the "dom" variable to the root element of the document
	query.bindVariable("dom", model.dom());

	// Setup the query
	QFile queryFile;
	if (!openFile(&queryFile, argv[2])) {
		qFatal("Err, can't open XQuery file %s", argv[2]);
	}
	query.setQuery(&queryFile, QUrl::fromLocalFile(argv[2]));

	// Setup a serializer printing to stdout
	QFile out;
	out.open(stdout, QIODevice::WriteOnly);
	QXmlSerializer serializer(query, &out);

	// Evaluate and exit
	if (!query.evaluateTo(&serializer)) {
		return 1;
	}
	return 0;
}
