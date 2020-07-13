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

#ifndef __REPEAT_H__
#define __REPEAT_H__

#include "text.h"
#include "rest.h"

namespace Ms {
class Score;
class Segment;

//---------------------------------------------------------
//   @@ MeasureRepeat
//---------------------------------------------------------

class MeasureRepeat final : public Rest
{
public:
    MeasureRepeat(Score*);
    MeasureRepeat& operator=(const MeasureRepeat&) = delete;

    MeasureRepeat* clone() const override   { return new MeasureRepeat(*this); }
    Element* linkedClone() override         { return Element::linkedClone(); }
    ElementType type() const override       { return ElementType::MEASURE_REPEAT; }

    void setNumMeasures(int n)              { m_numMeasures = n; }
    int numMeasures() const                 { return m_numMeasures; }
    void setSymId(SymId id)                 { m_symId = id; }
    SymId symId() const                     { return m_symId; }

    Measure* measureRepeatFirst() const     { return measure()->measureRepeatFirst(staffIdx()); }

    void draw(QPainter*) const override;
    void layout() override;
    Fraction ticks() const override;
    Fraction actualTicks() const            { return Rest::ticks(); }

    void read(XmlReader&) override;
    void write(XmlWriter& xml) const override;

    QRectF numberRect() const override;
    Shape shape() const override;

    QString accessibleInfo() const override;

    bool placeMultiple() const override     { return numMeasures() == 1; } // prevent overlapping additions with range selection

private:
    int m_numMeasures;
    SymId m_symId;

};
}     // namespace Ms
#endif
