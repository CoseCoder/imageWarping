#pragma once
#include <QWidget>
#include <vector>
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#include "IDW.h"
#include "RBF.h"

using namespace std;

QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget(void);
	~ImageWidget(void);

protected:
	void paintEvent(QPaintEvent *paintevent);
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);

	public slots:
	// File IO
	void Open();												// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();												// Save image to another file

	// Image processing
	void Invert();												// Invert pixel value in image
	void Mirror(bool horizontal = false, bool vertical = true);		// Mirror image vertically or horizontally
	void TurnGray();											// Turn image to gray-scale map
	void Restore();												// Restore image to origin

	void Input();												//输入数据点对
	void WarpingIDW();											//通过IDW算法将图像扭曲
	void WarpingRBF();											//通过RBF算法将图像扭曲

private:
	void FillHole();											//根据周围点的情况填充白缝

private:
	QImage		*ptr_image_;				// image 
	QImage		*ptr_image_backup_;
	QImage		*ptr_image_temp_;				 

	bool input_;							//表示当前是否处于输入数据点对的状态

	vector<QPoint> p_points_;				//输入的数据点对
	vector<QPoint> q_points_;

	int current_;							//表示当前输入的是哪一对数据点
	
	bool **change_;							//表示扭曲后的图像的每个像素点是否有被改变过
};

