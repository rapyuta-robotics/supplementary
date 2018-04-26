/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include <elastic_nodes/edge.h>
#include <elastic_nodes/node.h>

namespace elastic_nodes
{

QRectF Node::_nodeShape(-LONG_AXIS, -SHORT_AXIS, 2 * LONG_AXIS, 2 * SHORT_AXIS);

Node::Node(const std::string& state, const std::string& task, const std::string& plan)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
    _stateName = QString::fromStdString(state);
    _taskName = QString::fromStdString(task);
    _planName = QString::fromStdString(plan);
    QString tool_tip = QString("Plan: ") + _planName + (", Task: ") + _taskName;
    setToolTip(tool_tip);
}

void Node::addEdge(Edge* edge)
{
    _edgeList << edge;
    edge->adjust();
}

QList<Edge*> Node::edges() const
{
    return _edgeList;
}

QRectF Node::boundingRect() const
{
    qreal buffer = 2;
    return _nodeShape.adjusted(-buffer, -buffer, buffer, buffer);
}

QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addEllipse(_nodeShape);
    return path;
}

void Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*)
{
    // Draw ellipse representing state
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::yellow);
    painter->drawEllipse(_nodeShape);
    // Draw enclosing eclipse
    qreal buffer = 1;
    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(_nodeShape.adjusted(-buffer, -buffer, buffer, buffer));
    // Add text with bounding rectangle
    float factor = _nodeShape.width() / painter->fontMetrics().width(_stateName);
    if ((factor > 0.5 && factor < 1)) {
        QFont f = painter->font();
        f.setPointSizeF(f.pointSizeF() * factor);
        painter->setFont(f);
    }
    painter->drawText(_nodeShape, Qt::AlignCenter | Qt::TextDontClip | Qt::TextWordWrap, _stateName);
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant& value)
{
    switch (change) {
    case ItemPositionHasChanged:
        foreach (Edge* edge, _edgeList)
            edge->adjust();
        // graph->itemMoved();
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

} // namespace elastic_nodes