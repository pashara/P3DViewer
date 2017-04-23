#pragma once
#include "libs.h"
#include "3dloader.h"
#include "objectfile.h"
#include "zbuffer.h"
#include "perspective.h"
#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>


class QPainter;
class QOpenGLContext;
class QOpenGLPaintDevice;

class NewWindow : public QWindow, protected QOpenGLFunctions
{
	Q_OBJECT
public:
	static void drawPart(int part, ZBuffer *zb, QImage* imag);
	explicit NewWindow(std::vector<objectfile>* data, QWindow *parent = 0);
	~NewWindow();

	virtual void render(QPainter *painter);
	virtual void render();

	virtual void initialize();

	void setAnimating(bool animating);

	public slots:
	void renderLater();
	void renderNow();

protected:
	int asd = 0;
	std::vector<objectfile>* data;

	int isInitPerspective = 0;

	ZBuffer * zb;					// Z-ασττεπ
	int Tx = 0;
	int Ty = 0;
	int Tz = 600;
	double ugolY = -45;
	double ugolZ = 0;
	double ugolX = 0;
	int perspective1 = 1;
	double radius = 1200;
	perspective *perspectiv = NULL;
	PointTo3D* _Original1Dot = NULL;
	PointTo3D* _Original2Dot = NULL;
	PointTo3D* _Original3Dot = NULL;



	bool event(QEvent *event) override;
	void init();

	void exposeEvent(QExposeEvent *event) override;

private:
	bool m_animating;

	QOpenGLContext *m_context;
	QOpenGLPaintDevice *m_device;
};