//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2002-2011 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#include "measurerepeat.h"
#include "barline.h"
#include "sym.h"
#include "score.h"
#include "system.h"
#include "measure.h"
#include "mscore.h"
#include "staff.h"
#include "utils.h"

namespace Ms {
//---------------------------------------------------------
//   MeasureRepeat
//---------------------------------------------------------

MeasureRepeat::MeasureRepeat(Score* score)
    : Rest(score), m_numMeasures(0), m_symId(SymId::noSym)
{
}

//---------------------------------------------------------
//   draw
//---------------------------------------------------------

void MeasureRepeat::draw(QPainter* painter) const
{
    painter->setPen(curColor());
    drawSymbol(m_symId, painter);

    if (m_numMeasures > 1 && track() != -1) { // in score rather than palette
        std::vector<SymId>&& numberSym = toTimeSigString(QString("%1").arg(m_numMeasures));
        qreal x = (symBbox(m_symId).width() - symBbox(numberSym).width()) * .5;
        qreal y = -1.5 * spatium() - staff()->height() * .5;
        drawSymbols(numberSym, painter, QPointF(x, y));
        if (score()->styleB(Sid::fourMeasureRepeatShowExtenders) && m_numMeasures == 4) {
            // TODO: add style settings specific to measure repeats
            // for now, using thickness and margin same as mmrests
            qreal hBarThickness = score()->styleP(Sid::mmRestHBarThickness);
            if (hBarThickness) { // don't draw at all if 0, QPainter interprets 0 pen width differently
                QPen pen(painter->pen());
                pen.setCapStyle(Qt::FlatCap);
                pen.setWidthF(hBarThickness);
                painter->setPen(pen);

                qreal twoMeasuresWidth = 2 * measure()->width();
                qreal margin = score()->styleP(Sid::multiMeasureRestMargin);
                qreal xOffset = symBbox(m_symId).width() * .5;
                qreal gapDistance = (symBbox(m_symId).width() + spatium()) * .5;
                painter->drawLine(QLineF(-twoMeasuresWidth + xOffset + margin, 0.0, xOffset - gapDistance, 0.0));
                painter->drawLine(QLineF(xOffset + gapDistance, 0.0, twoMeasuresWidth + xOffset - margin, 0.0));
            }
        }
    }
}

//---------------------------------------------------------
//   layout
//---------------------------------------------------------

void MeasureRepeat::layout()
{
    for (Element* e : el()) {
        e->layout();
    }

    switch (m_numMeasures) {
        case 1:
            setSymId(SymId::repeat1Bar);
            break;
        case 2:
            setSymId(SymId::repeat2Bars);
            break;
        case 4:
            setSymId(SymId::repeat4Bars);
            break;
        default:
            m_symId = SymId::noSym; // should never happen
            break;
    }

    if (track() != -1) {    // in score rather than palette
        setPos(0, 2.0 * spatium() + 0.5 * styleP(Sid::staffLineWidth)); // xpos handled by Measure::stretchMeasure()
    }
    setbbox(symBbox(m_symId));
    addbbox(numberRect());
    setAutoplace(false); // TODO: re-enable without distorting measure width
}

//---------------------------------------------------------
//   numberRect
///   returns the measure repeat number's bounding rectangle
//---------------------------------------------------------

QRectF MeasureRepeat::numberRect() const
{
    if (m_numMeasures < 2 || track() == -1) { // don't display in palette
        return QRectF();
    }
    std::vector<SymId>&& numberSym = toTimeSigString(QString("%1").arg(m_numMeasures));
    QRectF r = symBbox(numberSym);
    qreal x = (symBbox(m_symId).width() - symBbox(numberSym).width()) * .5;
    qreal y = -1.5 * spatium() - staff()->height() * .5;
    r.translate(QPointF(x, y));
    return r;
}

//---------------------------------------------------------
//   shape
//---------------------------------------------------------

Shape MeasureRepeat::shape() const
{
    Shape shape;
    shape.add(numberRect());
    shape.add(symBbox(m_symId));
    return shape;
}


//---------------------------------------------------------
//   write
//---------------------------------------------------------

void MeasureRepeat::write(XmlWriter& xml) const
{
    xml.stag(this);
    xml.tag("subtype", numMeasures());
    Rest::writeProperties(xml);
    el().write(xml);
    xml.etag();
}

//---------------------------------------------------------
//   read
//---------------------------------------------------------

void MeasureRepeat::read(XmlReader& e)
{
    while (e.readNextStartElement()) {
        const QStringRef& tag(e.name());
        if (tag == "subtype") {
            setNumMeasures(e.readInt());
        } else if (!Rest::readProperties(e)) {
            e.unknown();
        }
    }
}

//---------------------------------------------------------
//   ticks
//---------------------------------------------------------

Fraction MeasureRepeat::ticks() const
{
    if (measure()) {
        return measure()->stretchedLen(staff());
    }
    return Fraction(0, 1);
}

//---------------------------------------------------------
//   accessibleInfo
//---------------------------------------------------------

QString MeasureRepeat::accessibleInfo() const
{
    return Element::accessibleInfo();
}
}
