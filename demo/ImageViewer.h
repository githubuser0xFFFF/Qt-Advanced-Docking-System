#ifndef ImageViewerH
#define ImageViewerH
//============================================================================
/// \file   ImageViewer.h
/// \author Uwe Kindler
/// \date   04.11.2022
/// \brief  Declaration of CImageViewer
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QScrollArea>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE


struct ImageViewerPrivate;

/**
 * Tiny simple image viewer for showing images in demo
 */
class CImageViewer : public QScrollArea
{
	Q_OBJECT
public:
	using Super = QScrollArea;

	explicit CImageViewer(QWidget *parent = nullptr);
    virtual ~CImageViewer();

	bool loadFile(const QString& Filename);
	void setImage(const QImage &newImage);

public Q_SLOTS:
	void open();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();

protected:
	/**
	 * @brief Reimplemented from QScrollArea to adjust image scaling if m_AutoFit is
	 * true.
	 */
	virtual void resizeEvent(QResizeEvent* ResizeEvent);

	/**
	 * @brief Handle mouse press events.
	 */
	virtual void mousePressEvent(QMouseEvent* Event);

	/**
	 * @brief Handles mouse release events.
	 */
	virtual void mouseReleaseEvent(QMouseEvent* Event);

	/**
	 * @brief Handle mouse move events.
	 */
	virtual void mouseMoveEvent(QMouseEvent* Event);

	/**
	 * @brief Use mouse wheel to change scaling of the image.
	 */
	virtual void wheelEvent(QWheelEvent* Event);

private:
    /**
	 * @brief Create the wiget actions.
	 */
    void createActions();

	/**
	 * @brief Adjust size of render widget in case of image size change.
	 * @param[in] Image The new image that may have a different image size.
	 */
	void adjustDisplaySize(const QImage& Image);

    ImageViewerPrivate* d;
    friend ImageViewerPrivate;
};

//---------------------------------------------------------------------------
#endif // ImageViewerH
