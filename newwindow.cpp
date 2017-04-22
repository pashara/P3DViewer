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




#include <QtCore/QCoreApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>
#include <QKeyEvent>


CLoad3DS g_Load3ds;             // Наш 3DS класс.
t3DModel g_3DModel;


NewWindow::NewWindow(std::vector<objectfile>* data,QWindow *parent)	: QWindow(parent), m_animating(false), m_context(0), m_device(0) {

	
	perspectiv = new linearPerspective(&g_3DModel, zb, radius);



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
	perspectiv->setWorkMatrix(MAT_1);
	perspectiv->init(_Original1Dot, _Original2Dot, _Original3Dot);

	

	/*ofstream log;
	log.open("log.txt");
	log << "Начало рисования" << endl;
	for (int j = 0; j < zb->sY; j++) {
		for (int i = 0; i < zb->sX; i++) {
			//if (zb->buff[j][i].color != 3)
				log << zb->buff[j][i].color << endl;
		}
		log << endl;
	}
	log << "Конец рисования" << endl;
	
	log.close();
	*/

	
	/*// PUT MATRIX
	pCore::matrix<> *DEBUGMAT_1 = MAT_1;
	ofstream log;
	log.open("log.txt");
	log << setw(5) << DEBUGMAT_1->data[0][0] << " " << setw(5) << DEBUGMAT_1->data[0][1] << " " << setw(5) << DEBUGMAT_1->data[0][2] << " " << setw(5) << DEBUGMAT_1->data[0][3] << endl;
	log << setw(5) << DEBUGMAT_1->data[1][0] << " " << setw(5) << DEBUGMAT_1->data[1][1] << " " << setw(5) << DEBUGMAT_1->data[1][2] << " " << setw(5) << DEBUGMAT_1->data[1][3] << endl;
	log << setw(5) << DEBUGMAT_1->data[2][0] << " " << setw(5) << DEBUGMAT_1->data[2][1] << " " << setw(5) << DEBUGMAT_1->data[2][2] << " " << setw(5) << DEBUGMAT_1->data[2][3] << endl;
	log << setw(5) << DEBUGMAT_1->data[3][0] << " " << setw(5) << DEBUGMAT_1->data[3][1] << " " << setw(5) << DEBUGMAT_1->data[3][2] << " " << setw(5) << DEBUGMAT_1->data[3][3] << endl;
	log.close();
	*/




	QImage imag(width(), height(), QImage::Format_ARGB32_Premultiplied);


	for (int j = 0; j < zb->sY; j++) {
		for (int i = 0; i < zb->sX; i++) {
			if (zb->buff[j][i].color != 3) {
				QPoint dot(i, j);
				imag.setPixel(dot, zb->buff[j][i].color);
			}
		}
	}



	/*
	Output dots
	QImage imag(width(), height(), QImage::Format_ARGB32_Premultiplied);
	
	for (int numObj = 0; numObj < g_3DModel.numOfObjects; numObj++) {
		t3DObject *obj = &(g_3DModel.pObject[numObj]);
		for (int i = 0; i < obj->numOfVerts; i++) {
			QPoint dot(obj->pVerts[i].x + 100, obj->pVerts[i].y + 100);
			imag.setPixel(dot, 5);
		}
	}*/




	
	
	/*
	QImage imag(width(), height(), QImage::Format_ARGB32_Premultiplied);
	for (int x = width()/4; x < width()*0.75; x++) {
		for (int y = height()/4; y < height()*0.75; y++) {
			asd++;
			QPoint dot(x, y);

			imag.setPixel(dot, asd);
		}
	}	*/
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
		renderNow();
		ke = (QKeyEvent *)event;
		switch (ke->key()) {
		case 16777235:		//Стрелка вверх
			Ty--;
		case 16777237:		//Стрелка вниз
			Ty++;
		case 16777236:		//Стрелка вправо
			Tx++;
		case 16777249:		//Стрелка стрелка влево
			Tx--;
		}
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