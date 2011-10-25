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
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QXmlFormatter>
#include <QXmlQuery>

#include "qhtmlnodemodel.h"


// This is the XQuery string
static const char xquery[] = \
	"let $query := ($dom//input[@name=\"query\"]/@value/string())[1]\n"
	"return\n"
	"	<search query=\"{$query}\"> {\n"
	"		for $x at $i in $dom//h3\n"
	"		return <item number=\"{$i}\"> {$x//a/@href/string()} </item>\n"
	"		} </search>\n"
;


// Program entry point
int main(int argc, char **argv)
{
	// Validate arguments
	if (argc < 2) {
		qFatal("Usage: %s <search-term> [number-of-results]\n", argv[0]);
	}

	QCoreApplication app(argc, argv);

	// Search via ixquick
	QString term = QString::fromUtf8(QUrl::toPercentEncoding(argv[1]));
	int n = (argc > 2 ? QString(argv[2]).toInt() : 10);
	QUrl url(QString("https://ixquick.com/do/metasearch.pl?prfh=num_of_resultsEEE%1N1N&q=%2").arg(n).arg(term));

	QNetworkAccessManager nma;
	QNetworkRequest request(url);
	request.setRawHeader("User-Agent", "Mozilla/5.0 (x64_64-unknown-linux.gnu)");
	QNetworkReply *reply = nma.get(request);

	qDebug("Performing search on ixquick...");

	// Wait for reply
	QEventLoop loop;
	QObject::connect(reply, SIGNAL(finished()), &app, SLOT(quit()));
	app.exec();

	// Setup query and model
	QXmlQuery query;
	QHtmlNodeModel model(query.namePool(), reply->readAll(), url);
	model.setCaseConversion(QHtmlNodeModel::ToLowerCase);
	query.bindVariable("dom", model.dom());
	query.setQuery(xquery);

	// Setup a formatter printing to stdout
	QFile out;
	out.open(stdout, QIODevice::WriteOnly);
	QXmlFormatter formatter(query, &out);

	// Evaluate and exit
	if (!query.evaluateTo(&formatter)) {
		return 1;
	}
	return 0;
}
