//============================================================================
/// \file   RenderWidget.cpp
/// \author Uwe Kindler
/// \date   04.11.2022
/// \brief  Implementation of CRenderWidget
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "RenderWidget.h"

#include <QPainter>
#include <math.h>


//===========================================================================
CRenderWidget::CRenderWidget(QWidget* Parent) :
	QWidget(Parent), m_ScaleFactor(1)
{
	this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	this->setCursor(Qt::OpenHandCursor);
}

//===========================================================================
CRenderWidget::~CRenderWidget()
{

}

//===========================================================================
void CRenderWidget::showImage(const QImage& Image)
{
	m_Image = QPixmap::fromImage(Image);
	this->adjustWidgetSize();
	this->repaint();
}

//===========================================================================
void CRenderWidget::paintEvent(QPaintEvent* Event)
{
	Q_UNUSED(Event);
	QPainter Painter(this);
	Painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	Painter.setRenderHint(QPainter::Antialiasing, true);
	Painter.scale(m_ScaleFactor, m_ScaleFactor);
	Painter.drawPixmap(QPoint(0, 0), m_Image);
}

//============================================================================
void CRenderWidget::zoomIn()
{
	scaleImage(1.25);
}

//============================================================================
void CRenderWidget::zoomOut()
{
	scaleImage(0.8);
}

//============================================================================
void CRenderWidget::zoomByValue(double ZoomValue)
{
	scaleImage(ZoomValue);
}

//============================================================================
void CRenderWidget::normalSize()
{
	m_ScaleFactor = 1;
	this->adjustWidgetSize();
}

//============================================================================
void CRenderWidget::scaleImage(double ScaleFactor)
{
	m_ScaleFactor *= ScaleFactor;
	this->adjustWidgetSize();
}

//============================================================================
void CRenderWidget::adjustWidgetSize()
{
	QSize ScaledImageSize = m_Image.size() * m_ScaleFactor;
	if (ScaledImageSize != this->size())
	{
		this->setFixedSize(ScaledImageSize);
	}
}

//============================================================================
void CRenderWidget::scaleToSize(const QSize& TargetSize)
{
	if (m_Image.isNull())
	{
		return;
	}
	double ScaleFactorH = (double) TargetSize.width() / m_Image.size().width();
	double ScaleFactorV = (double) TargetSize.height()
	    / m_Image.size().height();
	m_ScaleFactor = (ScaleFactorH < ScaleFactorV) ? ScaleFactorH : ScaleFactorV;
	this->adjustWidgetSize();
}

//---------------------------------------------------------------------------
// EOF RenderWidget.cpp
