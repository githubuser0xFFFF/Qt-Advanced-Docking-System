//============================================================================
/// \file   ImageViewer.cpp
/// \author Uwe Kindler
/// \date   04.11.2022
/// \brief  Implementation of CImageViewer
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "ImageViewer.h"

#include <math.h>

#include <QLabel>
#include <QImageReader>
#include <QImageWriter>
#include <QMessageBox>
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>
#include <QAction>
#include <QScrollBar>
#include <QPixmap>
#include <QPainter>
#include <QImage>
#include <QMouseEvent>
#include <QWheelEvent>

#include "RenderWidget.h"

/**
 * Private image viewer data
 */
struct ImageViewerPrivate
{
	CImageViewer* _this;
	CRenderWidget* RenderWidget;///< renders the image to screen
	bool AutoFit;///< automatically fit image to window size on resize events
	QSize ImageSize;///< stores the image size to detect image size changes
	QPoint MouseMoveStartPos;///< for calculation of mouse move vector
	QLabel* ScalingLabel;///< label displays scaling factor
	QList<QWidget*> OverlayTools;///< list of tool widget to overlay

	ImageViewerPrivate(CImageViewer* _public) : _this(_public) {}
};



//============================================================================
CImageViewer::CImageViewer(QWidget *parent)
	: Super(parent),
	  d(new ImageViewerPrivate(this))
{
	d->AutoFit = true;
	d->RenderWidget = new CRenderWidget(this);

	this->setBackgroundRole(QPalette::Light);
	this->setAlignment(Qt::AlignCenter);
	this->setWidget(d->RenderWidget);
	this->createActions();
	this->setMouseTracking(false); // only produce mouse move events if mouse button pressed
}


//============================================================================
CImageViewer::~CImageViewer()
{
	delete d;
}


//============================================================================
bool CImageViewer::loadFile(const QString& fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull())
    {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }

    setImage(newImage);
    setWindowFilePath(fileName);
    return true;
}


//===========================================================================
void CImageViewer::setImage(const QImage &newImage)
{
    d->RenderWidget->showImage(newImage);
    this->adjustDisplaySize(newImage);
}


//============================================================================
void CImageViewer::adjustDisplaySize(const QImage& Image)
{
	if (d->ImageSize == Image.size())
	{
		return;
	}
	d->ImageSize = Image.size();
	if (d->AutoFit)
	{
		this->fitToWindow();
	}
}


//===========================================================================
static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    for (const QByteArray &mimeTypeName : supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}


//===========================================================================
void CImageViewer::open()
{
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}


//===========================================================================
void CImageViewer::createActions()
{
	QAction* a;
    a = new QAction(tr("&Open..."));
    a->setIcon(QIcon(":/adsdemo/images/perm_media.svg"));
    connect(a, &QAction::triggered, this, &CImageViewer::open);
    a->setShortcut(QKeySequence::Open);;
    this->addAction(a);

	a = new QAction(tr("Fit on Screen"));
	a->setIcon(QIcon(":/adsdemo/images/zoom_out_map.svg"));
	connect(a, &QAction::triggered, this, &CImageViewer::fitToWindow);
	this->addAction(a);

	a = new QAction(tr("Actual Pixels"));
	a->setIcon(QIcon(":/adsdemo/images/find_in_page.svg"));
	connect(a, &QAction::triggered, this, &CImageViewer::normalSize);
	this->addAction(a);

	a = new QAction(this);
	a->setSeparator(true);
	this->addAction(a);

	a = new QAction(tr("Zoom In (25%)"));
	a->setIcon(QIcon(":/adsdemo/images/zoom_in.svg"));
	connect(a, &QAction::triggered, this, &CImageViewer::zoomIn);
	this->addAction(a);

	a = new QAction(tr("Zoom Out (25%)"));
	a->setIcon(QIcon(":/adsdemo/images/zoom_out.svg"));
	connect(a, &QAction::triggered, this, &CImageViewer::zoomOut);
	this->addAction(a);

	this->setContextMenuPolicy(Qt::ActionsContextMenu);
}


//===========================================================================
void CImageViewer::zoomIn()
{
	d->AutoFit = false;
	d->RenderWidget->zoomIn();
}


//===========================================================================
void CImageViewer::zoomOut()
{
	d->AutoFit = false;
	d->RenderWidget->zoomOut();
}


//===========================================================================
void CImageViewer::normalSize()
{
	d->AutoFit = false;
	d->RenderWidget->normalSize();
}


//===========================================================================
void CImageViewer::fitToWindow()
{
	d->AutoFit = true;
	d->RenderWidget->scaleToSize(this->maximumViewportSize());
}


//============================================================================
void CImageViewer::resizeEvent(QResizeEvent* ResizeEvent)
{
	Super::resizeEvent(ResizeEvent);
	if (d->AutoFit)
	{
		this->fitToWindow();
	}
}


//============================================================================
void CImageViewer::mousePressEvent(QMouseEvent* Event)
{
	d->RenderWidget->setCursor(Qt::ClosedHandCursor);
	d->MouseMoveStartPos = Event->pos();
	Super::mousePressEvent(Event);
}


//============================================================================
void CImageViewer::mouseReleaseEvent(QMouseEvent* Event)
{
	d->RenderWidget->setCursor(Qt::OpenHandCursor);
	Super::mouseReleaseEvent(Event);
}


//============================================================================
void CImageViewer::mouseMoveEvent(QMouseEvent* Event)
{
	QPoint MoveVector = Event->pos() - d->MouseMoveStartPos;
	d->MouseMoveStartPos = Event->pos();
	horizontalScrollBar()->setValue(horizontalScrollBar()->value()
	    - MoveVector.x());
	verticalScrollBar()->setValue(verticalScrollBar()->value() - MoveVector.y());
}


//============================================================================
void CImageViewer::wheelEvent(QWheelEvent* Event)
{
	double numDegrees = Event->angleDelta().y() / 8;
	double numSteps = numDegrees / 15;
	d->AutoFit = false;
	double Zoom;
	if (numSteps < 0)
	{
		Zoom = pow(0.9, 0 - numSteps);
	}
	else
	{
		Zoom = pow(1.10, numSteps);
	}
	d->RenderWidget->zoomByValue(Zoom);
}

#include "moc_ImageViewer.cpp"
//---------------------------------------------------------------------------
// EOF ImageViewer.cpp
