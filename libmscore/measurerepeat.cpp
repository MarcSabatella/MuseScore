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
#include "sym.h"
#include "score.h"
#include "system.h"
#include "measure.h"
#include "mscore.h"
#include "staff.h"

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
