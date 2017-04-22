//#include "3dloader.h"
//#include "PCMatrix.h"
//#include "AffineTransformations.h"
//#include "zbuffer.h"
#include "newwindow.h"
#include "perspective.h"
#include "linearPerspective.h"
#include "AffineTransformations.h"




#include <QtCore/QCoreApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>

NewWindow::NewWindow(std::vector<objectfile>* data,QWindow *parent)
	: QWindow(parent)
	, m_animating(false)
	, m_context(0)
	, m_device(0)
{
	zb = new ZBuffer(width(), height());
	this->data = data;
	std::string windowTitle = "Loaded: "+data->at(0).getTitle();

	//g_Load3ds.Import3DS(&(g_3DModel), data->at(0).getPath());


	setTitle(QString::fromStdString(windowTitle));
	setSurfaceType(QWindow::OpenGLSurface);
}

NewWindow::~NewWindow()
{
	delete m_device;
}
void NewWindow::render(QPainter *painter)
{
	/*
	BAD DRAW
	for (int x = 0; x < width(); x++) {
		for (int y = 0; y < height(); y++) {
			QPoint a(x, y);

			QPen linepen(Qt::red);
			//linepen.setCapStyle(Qt::RoundCap);
			linepen.setWidth(1);
			painter->setRenderHint(QPainter::Antialiasing, true);
			painter->setPen(linepen);


			painter->drawPoint(a);
		}
	}
	*/

	//perspectiv = new linearPerspective(&g_3DModel, zb, radius);
//	pCore::AffineTransformations *a = new pCore::AffineTransformations();
//	pCore::matrix<>* MAT_1;
//	MAT_1 = new pCore::matrix<>(4, 4);
//	a->setT(this->Tx, this->Ty, this->Tz);
//	a->setS(1.0, 1.0, 1.0);
//	a->setRy(this->ugolY);
//	a->setRz(this->ugolZ);
//	a->setRx(this->ugolX);
//	a->getResult(*MAT_1);


//	zb->Clear();


//	perspectiv->setWorkMatrix(MAT_1);
//	perspectiv->init(_Original1Dot, _Original2Dot, _Original3Dot);

	
	QImage imag(width(), height(), QImage::Format_ARGB32_Premultiplied);
	
	
	for (int x = width()/4; x < width()*0.75; x++) {
		for (int y = height()/4; y < height()*0.75; y++) {
			asd++;
			QPoint dot(x, y);

			imag.setPixel(dot, asd);
		}
	}	
	painter->drawImage(QRect(0, 0, width(), height()), imag);
	



	Q_UNUSED(painter);
}

void NewWindow::initialize()
{
}

void NewWindow::render()
{
	if (!m_device)
		m_device = new QOpenGLPaintDevice;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	setPosition(100,100);
	setWidth(800);
	setHeight(600);
	m_device->setSize(size());





	QPainter painter(m_device);
	render(&painter);
}

void NewWindow::renderLater()
{
	requestUpdate();
}

bool NewWindow::event(QEvent *event)
{

	switch (event->type()) {
	case QEvent::UpdateRequest:
		renderNow();
		return true;
	case QEvent::KeyPress:
		renderNow();
		return true;
	default:
		return QWindow::event(event);
	}


}

void NewWindow::exposeEvent(QExposeEvent *event)
{
	Q_UNUSED(event);

	if (isExposed())
		renderNow();
}

void NewWindow::renderNow()
{
	if (!isExposed())
		return;

	bool needsInitialize = false;

	if (!m_context) {
		m_context = new QOpenGLContext(this);
		m_context->setFormat(requestedFormat());
		m_context->create();

		needsInitialize = true;
	}

	m_context->makeCurrent(this);

	if (needsInitialize) {
		initializeOpenGLFunctions();
		initialize();
	}

	render();

	m_context->swapBuffers(this);

	if (m_animating)
		renderLater();
}

void NewWindow::setAnimating(bool animating)
{
	m_animating = animating;

	if (animating)
		renderLater();
}


void NewWindow::init() {
	/*pCore::AffineTransformations *a = new pCore::AffineTransformations();
	pCore::matrix<>* MAT_1;

	MAT_1 = new pCore::matrix<>(4, 4);
	a->setT(this->Tx, this->Ty, this->Tz);
	a->setS(1.0, 1.0, 1.0);
	a->setRy(this->ugolY);
	a->setRz(this->ugolZ);
	a->setRx(this->ugolX);
	a->getResult(*MAT_1);


	zb->Clear();*/
}