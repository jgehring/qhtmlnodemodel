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


#ifndef QHTMLNODEMODEL_H_
#define QHTMLNODEMODEL_H_


#include <QSimpleXmlNodeModel>
#include <QVector>

class QByteArray;


class QHtmlNodeModelPrivate;

class QHtmlNodeModel : public QSimpleXmlNodeModel
{
	friend class QHtmlNodeModelPrivate;

	public:
		enum CaseConversion {
			KeepCase,       ///< Keep the case of the tags
			ToLowerCase,    ///< Convert all tag names to lowercase
			ToUpperCase     ///< Convert all tag names to uppercase
		};

	public:
		QHtmlNodeModel(const QXmlNamePool &namepool, const QByteArray &source, const QUrl &uri = QUrl());
		~QHtmlNodeModel();

		inline QXmlNodeModelIndex dom() const { return root(QXmlNodeModelIndex()); }

		void setCaseConversion(CaseConversion fc);
		CaseConversion caseConversion() const;

		virtual QXmlNodeModelIndex::DocumentOrder compareOrder(const QXmlNodeModelIndex &ni1, const QXmlNodeModelIndex &ni2) const;
		virtual QXmlName name(const QXmlNodeModelIndex &node) const;
		virtual QUrl documentUri(const QXmlNodeModelIndex &node) const;
		virtual QXmlNodeModelIndex::NodeKind kind(const QXmlNodeModelIndex &node) const;
		virtual QXmlNodeModelIndex root(const QXmlNodeModelIndex &node) const;
		virtual QVariant typedValue(const QXmlNodeModelIndex &node) const;

	protected:
		virtual QVector<QXmlNodeModelIndex> attributes(const QXmlNodeModelIndex &element) const;
		virtual QXmlNodeModelIndex nextFromSimpleAxis(SimpleAxis axis, const QXmlNodeModelIndex &origin) const;

	private:
		QHtmlNodeModelPrivate *d;
};


#endif // QHTMLNODEMODEL_H_
