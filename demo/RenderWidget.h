#ifndef RenderWidgetH
#define RenderWidgetH
//============================================================================
/// \file   RenderWidget.h
/// \author Uwe Kindler
/// \date   04.11.2022
/// \brief  Declaration of CRenderWidget
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QWidget>
#include <QPixmap>

//============================================================================
//                           FORWARD DECLARATIONS
//============================================================================
class QImage;


/**
 * @brief Widget for fast display of images (i.e. for video capture devices)
 */
class CRenderWidget : public QWidget
{
	Q_OBJECT
private:
	QPixmap m_Image;
	double m_ScaleFactor;

protected:
	/**
	 * @brief Reimplemented paint event method showing actual image.
	 */
	void paintEvent(QPaintEvent* PaintEvent);

	/**
	 * @brief Change scale factor
	 */
	void scaleImage(double ScaleFactor);

	/**
	 * @brief Adjust widget size to size of image.
	 */
	void adjustWidgetSize();

public:
	/**
	 * Constructor
	 * @param[in] Parent Parent widget.
	 */
	CRenderWidget(QWidget* Parent);

	/**
	 * Destructor
	 */
	virtual ~CRenderWidget();

signals:
	/**
	 * @brief Signalize change of captured image size.
	 * @param ImageSize New image size.
	 */
	void imageSizeChanged(const QSize& ImageSize);

public slots:
	/**
	 * @brief Show new image in render widget.
	 */
	void showImage(const QImage& Image);

	/**
	 * @brief Zoom into the scene.
	 * This function decreases the scaling factor by setting it to the previous
	 * value in internal scaling list.
	 * @brief Steps The number of steps to zoom in. One step is 25%.
	 */
	void zoomIn();

	/**
	 * @brief Zoom out of the scene.
	 * This function decreases the scaling factor by setting it to the next
	 * value in internal scaling list.
	 * @brief Steps The number of steps to zoom out. One step is 25%.
	 */
	void zoomOut();

	/**
	 * @brief Change zoom by zoom value.
	 * @param[in] ZoomValue This is the zoom value to apply. A value of 1
	 * means no change a value > 1 increases the image (i.e. 1.25 would increase
	 * the image by 25%) and a value of < 1 decreases the image size (i.e.
	 * a value of 0.8 would decrease the image size by 25%).
	 */
	void zoomByValue(double ZoomValue);

	/**
	 * @brief Resets the actual scaling to 1 and display the image with its
	 *        actual pixel size.
	 */
	void normalSize();

	/**
	 * @brief Scales the wiget and its content image to the given TargetSize
	 */
	void scaleToSize(const QSize& TargetSize);
}; // class CRenderWidget

//---------------------------------------------------------------------------
#endif // RenderWidgetH
