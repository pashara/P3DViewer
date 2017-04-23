//#include "3dloader.h"
//#include "PCMatrix.h"
//#include "AffineTransformations.h"
//#include "zbuffer.h"
#include "newwindow.h"
#include "perspective.h"
#include "linearPerspective.h"
#include "AffineTransformations.h"
#include "3dloader.h"
#include <fstream>
#include <iomanip> 
#include <thread>
#include <windows.h>




#include <QtCore/QCoreApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>
#include <QKeyEvent>


CLoad3DS g_Load3ds;             // Наш 3DS класс.
t3DModel g_3DModel;


NewWindow::NewWindow(std::vector<objectfile>* data,QWindow *parent)	: QWindow(parent), m_animating(false), m_context(0), m_device(0) {

	


	this->data = data;
	std::string windowTitle = "Loaded: "+data->at(0).getTitle();

	g_Load3ds.Import3DS(&(g_3DModel), data->at(0).getPath());


	setTitle(QString::fromStdString(windowTitle));
	setSurfaceType(QWindow::OpenGLSurface);
}

NewWindow::~NewWindow()
{
	delete m_device;
}
void NewWindow::render(QPainter *painter)
{

	pCore::AffineTransformations *a = new pCore::AffineTransformations();
	pCore::matrix<>* MAT_1;
	zb = new ZBuffer(width(), height());

	MAT_1 = new pCore::matrix<>(4, 4);
	a->setT(this->Tx, this->Ty, this->Tz);
	a->setS(1.0, 1.0, 1.0);
	a->setRy(this->ugolY);
	a->setRz(this->ugolZ);
	a->setRx(this->ugolX);
	a->getResult(*MAT_1);

	zb->Clear();
	perspectiv = new linearPerspective(&g_3DModel, zb, radius);
	isInitPerspective = 1;
	perspectiv->setWorkMatrix(MAT_1);
	perspectiv->init(_Original1Dot, _Original2Dot, _Original3Dot);

	

	QImage *imag = new QImage (width(), height(), QImage::Format_ARGB32_Premultiplied);

	int ii = 1;
	if (ii == 1) {
		std::thread thr0(drawPart, 0, std::ref(zb), std::ref(imag));
		thr0.join();
		std::thread thr1(drawPart, 1, std::ref(zb), std::ref(imag));
		thr1.join();
		std::thread thr2(drawPart, 2, std::ref(zb), std::ref(imag));
		thr2.join();
		std::thread thr3(drawPart, 3, std::ref(zb), std::ref(imag));
		thr3.join();
	} else if(ii == 2) {
		drawPart(0, zb, imag);
		drawPart(1, zb, imag);
		drawPart(2, zb, imag);
		drawPart(3, zb, imag);
	}else{
		for (int j = 0; j < zb->sY; j++) {
			for (int i = 0; i < zb->sX; i++) {
				//if (zb->buff[j][i].color != 3) {
					QPoint dot(i, j);
					imag->setPixel(dot, zb->buff[j][i].color);
				//}
			}
		}
	}



	painter->drawImage(QRect(0, 0, width(), height()), *imag);
	
	delete perspectiv;
	delete a;
	delete MAT_1;
	delete zb;
	delete imag;

	Q_UNUSED(painter);
}


void NewWindow::drawPart(int part, ZBuffer *zb, QImage* imag) {
	int fromX, toX, fromY, toY;
	switch(part) {
	case 0: {
		fromX = 0;
		toX = zb->sX / 2;
		fromY = 0;
		toY = zb->sY / 2;
		break;
	}
	case 1: {
		fromX = zb->sX / 2 + 1;
		toX = zb->sX;
		fromY = 0;
		toY = zb->sY / 2;
		break;
	}
	case 2: {
		fromX = 0;
		toX = zb->sX / 2;
		fromY = zb->sY / 2 + 1;
		toY = zb->sY;
		break;
	}
	case 3: {
		fromX = zb->sX / 2+1;
		toX = zb->sX;
		fromY = zb->sY / 2 + 1;
		toY = zb->sY;
		break;
	}
	}
	for (int j = fromY; j < toY; j++) {
		for (int i = fromX; i < toX; i++) {
			if (zb->buff[j][i].color != 3) {
				QPoint dot(i, j);
				imag->setPixel(dot, zb->buff[j][i].color);
			}else{
				QPoint dot(i, j);
				imag->setPixel(dot, 55);			
			}
		}
	}
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
	/*
	16777235 - arrow down
	16777237 - arrow down
	16777249 - arrow left
	16777236 - arrow right
	*/ 
	int keyValue;
	QKeyEvent *ke;
	ofstream log;
	switch (event->type()) {
	case QEvent::UpdateRequest:
		renderNow();
		return true;
	case QEvent::KeyPress:
		ugolY+=1;
		renderNow();
		/*ke = (QKeyEvent *)event;
		switch (ke->key()) {
		case 16777235:		//Стрелка вверх
			Ty--;
		case 16777237:		//Стрелка вниз
			Ty++;
		case 16777236:		//Стрелка вправо
			Tx++;
		case 16777249:		//Стрелка стрелка влево
			Tx--;
		}*/
		/*keyValue = ke->key();
		
		log.open("log.txt");
		log << "Начало рисования" << endl;
		
		log << keyValue << endl;

		log << "Конец рисования" << endl;

		log.close();*/

		//setTitle(keyValue + ":");
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