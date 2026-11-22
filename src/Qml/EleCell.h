#ifndef ELECELL_H
#define ELECELL_H

#include <QQuickPaintedItem>
class EleCell : public QQuickPaintedItem
{
    Q_OBJECT
      Q_PROPERTY(quint32 electric READ getElectric  WRITE  setElectric NOTIFY electricChanged)
      Q_PROPERTY(QColor  color READ getColor  WRITE  setColor NOTIFY colorChanged)
      Q_PROPERTY(quint32 threshold READ  getThreshold WRITE setThreshold NOTIFY thresholdChanged)
  public:
      using QQuickPaintedItem::QQuickPaintedItem;

      //重写绘制
      void paint(QPainter*) override;

      //设置电量
      void setElectric( quint32 value) noexcept;
      quint32  getElectric()const noexcept;

      //设置颜色
      void setColor(QColor value)noexcept;
      QColor  getColor()const noexcept;

      //设置阈值
      void setThreshold(quint32 value) noexcept;
      quint32  getThreshold()const noexcept;

  signals:
      void electricChanged();
      void colorChanged();
      void thresholdChanged();

  private:
      quint32 m_iLineWidth = 1;
      quint32 m_iElectric = 70;
      QColor  m_qColor = Qt::green;
      QColor  m_qWarningColor = Qt::red;
      quint32 m_iThreshold = 20;


};

#endif // ELECELL_H
