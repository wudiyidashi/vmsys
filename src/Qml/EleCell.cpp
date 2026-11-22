#include "EleCell.h"
#include <QPainter>
#include <QPen>
#include <QBrush>

void EleCell::paint(QPainter *painter)
{
    QPen pen;
    pen.setColor(Qt::darkRed);
    pen.setWidth(m_iLineWidth);
    painter->setPen(pen);

    //电池头部
    quint32 headWidth = width() * 0.1;
    quint32 headHeight = height() * 0.3;

    auto cellWidth  = width() - pen.width() -headWidth;
    auto cellHeight = height() - pen.width();

    //电池
    painter->drawRect(0, 0, cellWidth, cellHeight);

    auto electricWidth = (cellWidth - pen.width())*m_iElectric / 100;
    auto electricHeight = cellHeight - pen.width();
    painter->fillRect(pen.width(), pen.width(), electricWidth, electricHeight, \
                      m_iElectric > m_iThreshold ? m_qColor : m_qWarningColor);

    //电池头部填充
    auto headX = cellWidth + pen.width();
    auto headY = height() / 2 -  headHeight/ 2;
    painter->fillRect(headX, headY, headWidth, headHeight, Qt::black);

}

void EleCell::setElectric(quint32 value) noexcept
{
    if(value < 0 || value > 100)
    {
        return;
    }

    m_iElectric = value;
    emit electricChanged();
    update();
}

quint32 EleCell::getElectric() const noexcept
{
    return m_iElectric;
}

void EleCell::setColor(QColor value) noexcept
{
    m_qColor = value;
    emit colorChanged();
}

QColor EleCell::getColor() const noexcept
{
    return m_qColor;
}

void EleCell::setThreshold(quint32 value) noexcept
{
    if(value < 0 || value > 100)
    {
        return;
    }

    m_iThreshold = value;
    emit thresholdChanged();
    update();
}

quint32 EleCell::getThreshold() const noexcept
{
    return m_iThreshold;
}
