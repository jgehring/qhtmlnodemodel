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


#include <QList>
#include <QHash>

#include <htmlcxx/html/ParserDom.h>

#include "qhtmlnodemodel.h"

using namespace htmlcxx;


// Internal private data
class QHtmlNodeModelPrivate
{
public:
	QHtmlNodeModel *model;
	QUrl uri;
	QHtmlNodeModel::CaseConversion cc;

	HTML::ParserDom parser;
	tree<HTML::Node> dom;

	// Mapping of attribute names to IDs
	mutable QHash<const char *, int> attributeIds;
	mutable QList<const char *> attributeNames;
	mutable int attributeCount;
	const int firstAttribute;

	QHtmlNodeModelPrivate(QHtmlNodeModel *model)
		: model(model), firstAttribute(1)
	{
		cc = QHtmlNodeModel::KeepCase;

		attributeCount = firstAttribute;
		while (attributeNames.count() < firstAttribute) {
			attributeNames += NULL;
		}
	}

	// Parses the given source tree
	void parse(const QByteArray &source)
	{
		dom = parser.parseTree(source.data());
	}

	// Converts a model index to a HTML node
	tree_node_<HTML::Node> *toNode(const QXmlNodeModelIndex &index) const
	{
		return (tree_node_<HTML::Node> *)index.internalPointer();
	}

	// Converts a model index to a tree iterator
	tree<HTML::Node>::iterator toIterator(const QXmlNodeModelIndex &index) const
	{
		return tree<HTML::Node>::iterator((tree_node_<HTML::Node> *)index.internalPointer());
	}

	// Converts a HTML node to a model index
	QXmlNodeModelIndex toNodeIndex(tree_node_<HTML::Node> *node) const
	{
		return model->createIndex((void *)node);
	}

	// Converts an attribute of a HTML node to a model index
	QXmlNodeModelIndex toNodeIndex(tree_node_<HTML::Node> *node, const char *attribute) const
	{
		// Register attribute if necessary
		if (!attributeIds.contains(attribute)) {
			attributeIds[attribute] = attributeCount;
			attributeNames += attribute;
			++attributeCount;
		}
		return model->createIndex((void *)node, attributeIds[attribute]);
	}

	// Converts a whole DOM tree to a model index
	QXmlNodeModelIndex toNodeIndex(tree<HTML::Node> *tree) const
	{
		return toNodeIndex(tree->begin().node);
	}

	// Checks if the given node represents an attribute
	bool isAttribute(const QXmlNodeModelIndex &index)
	{
		return (index.additionalData() >= firstAttribute);
	}

	// Returns the attribute name of the given index
	const char *attributeName(const QXmlNodeModelIndex &index)
	{
		int id = index.additionalData();
		if (id < attributeNames.count()) {
			return attributeNames[id];
		}
		return NULL;
	}
};


/*!
 * Constructor
 */
QHtmlNodeModel::QHtmlNodeModel(const QXmlNamePool &namepool, const QByteArray &source, const QUrl &uri)
	: QSimpleXmlNodeModel(namepool), d(new QHtmlNodeModelPrivate(this))
{
	d->uri = uri;
	d->parse(source);
}

/*!
 * Destructor
 */
QHtmlNodeModel::~QHtmlNodeModel()
{
	delete d;
}

/*!
 * Sets the case conversion mode
 */
void QHtmlNodeModel::setCaseConversion(CaseConversion cc)
{
	d->cc = cc;
}

/*!
 * Returns the current case conversion mode
 */
QHtmlNodeModel::CaseConversion QHtmlNodeModel::caseConversion() const
{
	return d->cc;
}

/*!
 * This function returns the relative document order for the
 * nodes indexed by \a ni1 and \a ni2. It is used for the \c Is
 * operator and for sorting nodes in document order.
 */
QXmlNodeModelIndex::DocumentOrder QHtmlNodeModel::compareOrder(const QXmlNodeModelIndex &n1, const QXmlNodeModelIndex &n2) const
{
	tree<HTML::Node>::iterator it = d->toIterator(n1);
	tree<HTML::Node>::iterator jt = d->toIterator(n2);

	if (it->offset() < jt->offset()) {
		return QXmlNodeModelIndex::Precedes;
	} else if (it->offset() < jt->offset()) {
		return QXmlNodeModelIndex::Follows;
	}
	return QXmlNodeModelIndex::Is;
}

/*!
 * Returns the name of \a ni. The caller guarantees that \a ni is not
 * \c null and that it belongs to this QAbstractXmlNodeModel.
 */
QXmlName QHtmlNodeModel::name(const QXmlNodeModelIndex &node) const
{
	if (d->isAttribute(node)) {
		return QXmlName(namePool(), d->attributeName(node));
	}

	tree<HTML::Node>::iterator it = d->toIterator(node);
	if (!it->isTag() || it == d->dom.begin()) {
		return QXmlName();
	}

	QString tag(it->tagName().c_str());
	switch (d->cc) {
		case ToLowerCase:
			tag = tag.toLower();
			break;
		case ToUpperCase:
			tag = tag.toUpper();
			break;
		default:
			break;
	}
	return QXmlName(namePool(), tag);
}

/*!
 * Returns the document URI of \a n. The document URI identifies the
 * resource which is the document. For example, the document could be a
 * regular file, e.g., \c{file:/}, or it could be the \c{http://} URL of
 * the location of a file. The document URI is used for resolving URIs
 * and to simply know where the document is.
 */
QUrl QHtmlNodeModel::documentUri(const QXmlNodeModelIndex &node) const
{
	Q_UNUSED(node);
	return d->uri;
}

/*!
 * Returns a value indicating the kind of node identified by \a ni.
 * The caller guarantees that \a ni is not null and that it identifies
 * a node in this node model. This function maps to the \c
 * dm:node-kind() accessor.
 */
QXmlNodeModelIndex::NodeKind QHtmlNodeModel::kind(const QXmlNodeModelIndex &node) const
{
	if (d->isAttribute(node)) {
		return QXmlNodeModelIndex::Attribute;
	}

	tree<HTML::Node>::iterator it = d->toIterator(node);
	if (!it->isTag() || it == d->dom.begin()) {
		return QXmlNodeModelIndex::Document;
	} else if (it->isComment()) {
		return QXmlNodeModelIndex::Comment;
	} else if (it->isTag()) {
		return QXmlNodeModelIndex::Element;
	}

	return QXmlNodeModelIndex::Text;
}

/*!
 * Returns the root node of the tree that contains the node whose index
 * is \a n. The caller guarantees that \a n is not \c null and that it
 * identifies a node in this node model.
 */
QXmlNodeModelIndex QHtmlNodeModel::root(const QXmlNodeModelIndex &node) const
{
	Q_UNUSED(node);
	return d->toNodeIndex(&d->dom);
}

/*!
 * Returns the typed value for node \a node.
 */
QVariant QHtmlNodeModel::typedValue(const QXmlNodeModelIndex &node) const
{
	tree<HTML::Node>::iterator it = d->toIterator(node);
	if (d->isAttribute(node)) {
		const char *attrName = d->attributeName(node);
		return QString(it->attribute(attrName).second.c_str());
	}

	if (!it->isTag()) {
		return QLatin1String(it->text().c_str());
	}

	QString tag(it->tagName().c_str());
	switch (d->cc) {
		case ToLowerCase:
			tag = tag.toLower();
			break;
		case ToUpperCase:
			tag = tag.toUpper();
			break;
		default:
			break;
	}
	return tag;
}

/*!
 * Returns the attributes of \a element. The caller guarantees
 * that \a element is an element in this node model.
 */
QVector<QXmlNodeModelIndex> QHtmlNodeModel::attributes(const QXmlNodeModelIndex &element) const
{
	QVector<QXmlNodeModelIndex> result;

	tree_node_<HTML::Node> *t = d->toNode(element);
	t->data.parseAttributes();

	std::map<std::string, std::string>::const_iterator it;
	std::map<std::string, std::string>::const_iterator end = t->data.attributes().end();
	for (it = t->data.attributes().begin(); it != end; ++it) {
		if (!it->first.empty()) {
			result += d->toNodeIndex(t, it->first.c_str());
		}
	}

	return result;
}

/*!
 * When QtXmlPatterns evaluate path expressions, it emulate them through a
 * combination of calls with QSimpleXmlNodeModel::SimpleAxis values. Therefore,
 * the implementation of this function must return the node, if any, that
 * appears on the \a axis emanating from the \a origin.
 */
QXmlNodeModelIndex QHtmlNodeModel::nextFromSimpleAxis(SimpleAxis axis, const QXmlNodeModelIndex &origin) const
{
	tree<HTML::Node>::iterator it = d->toIterator(origin), jt;
	switch (axis) {
		case Parent:
			return d->toNodeIndex(d->dom.parent(it).node);

		case FirstChild:
			jt = d->dom.begin(it);
			if (jt == d->dom.end(it)) {
				return QXmlNodeModelIndex();
			}
			return d->toNodeIndex(jt.node);

		case PreviousSibling:
			jt = d->dom.previous_sibling(it);
			if (!d->dom.is_valid(jt)) {
				return QXmlNodeModelIndex();
			}
			return d->toNodeIndex(jt.node);

		case NextSibling:
			jt = d->dom.next_sibling(it);
			if (!d->dom.is_valid(jt)) {
				return QXmlNodeModelIndex();
			}
			return d->toNodeIndex(jt.node);
	}

	Q_ASSERT_X(false, Q_FUNC_INFO, "Invalid axis!");
	return QXmlNodeModelIndex();
}
