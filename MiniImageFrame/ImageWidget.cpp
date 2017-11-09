#include "ImageWidget.h"
#include <QTime>

ImageWidget::ImageWidget(void)
{
	ptr_image_ = new QImage();
	ptr_image_backup_ = new QImage();
	ptr_image_temp_ = new QImage();
	input_ = false;
	current_ = -1;
}


ImageWidget::~ImageWidget(void)
{
	for (int i = 0; i < ptr_image_->height(); i++)
		delete change_[i];
	delete change_;
	delete ptr_image_;
	delete ptr_image_backup_;
	delete ptr_image_temp_;
}

void ImageWidget::paintEvent(QPaintEvent *paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	/*QRect rect = QRect((width() - ptr_image_->width()) / 2, (height() - ptr_image_->height()) / 2, ptr_image_->width(), ptr_image_->height());*/
	QRect rect = QRect(0, 0, ptr_image_->width(), ptr_image_->height());
	painter.drawImage(rect, *ptr_image_);

	//若处于输入数据点对的状态,将所画过的和正在画的直线显示出来
	if (input_)
	{
		QPen pen;
		pen.setColor("red");
		pen.setWidth(3);
		painter.setPen(pen);

		for (size_t i = 0; i < p_points_.size(); i++)
		{
			painter.drawLine(p_points_[i], q_points_[i]);
		}
	}

	painter.end();
	update();
}


//监听鼠标事件，提供输入数据点对(即直线)功能
//直线的起始点->p,直线的终止点->q
void ImageWidget::mousePressEvent(QMouseEvent * event)
{
	if (input_&&Qt::LeftButton == event->button())
	{
		current_++;

		//设置控制点	
		p_points_.push_back(event->pos());
		q_points_.push_back(event->pos());
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent * event)
{
	if (input_)
	{
		q_points_[current_] = event->pos();
	}
}

void ImageWidget::mouseReleaseEvent(QMouseEvent * event)
{
	if (!input_)
		return;
	if (p_points_[current_].x() < 0 || p_points_[current_].y() < 0 ||
		p_points_[current_].x() > ptr_image_->width() || p_points_[current_].y() > ptr_image_->height() ||
		q_points_[current_].x() < 0 || q_points_[current_].y() < 0 ||
		q_points_[current_].x() > ptr_image_->width() || q_points_[current_].y() > ptr_image_->height())
	{
		p_points_.pop_back();
		q_points_.pop_back();
		current_--;
	}

}

void ImageWidget::Open()
{
	// Open file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

	// Load file
	if (!fileName.isEmpty())
	{
		ptr_image_->load(fileName);
		*(ptr_image_backup_) = *(ptr_image_);
	}

	change_ = new bool*[ptr_image_->height()];
	for (int i = 0; i < ptr_image_->height(); i++)
		change_[i] = new bool[ptr_image_->width()];

	//ptr_image_->invertPixels(QImage::InvertRgb);
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	//*(ptr_image_) = ptr_image_->rgbSwapped();
	cout << "image size: " << ptr_image_->width() << ' ' << ptr_image_->height() << endl;
	update();
}

void ImageWidget::Save()
{
	SaveAs();
}

void ImageWidget::SaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}

	ptr_image_->save(filename);
}

void ImageWidget::Invert()
{
	for (int i = 0; i < ptr_image_->width(); i++)
	{
		for (int j = 0; j < ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			ptr_image_->setPixel(i, j, qRgb(255 - qRed(color), 255 - qGreen(color), 255 - qBlue(color)));
		}
	}

	// equivalent member function of class QImage
	// ptr_image_->invertPixels(QImage::InvertRgb);
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	QImage image_tmp(*(ptr_image_));
	int width = ptr_image_->width();
	int height = ptr_image_->height();

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, height - 1 - j));
				}
			}
		}
		else
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(i, height - 1 - j));
				}
			}
		}

	}
	else
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, j));
				}
			}
		}
	}

	// equivalent member function of class QImage
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	update();
}

void ImageWidget::TurnGray()
{
	for (int i = 0; i < ptr_image_->width(); i++)
	{
		for (int j = 0; j < ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			int gray_value = (qRed(color) + qGreen(color) + qBlue(color)) / 3;
			ptr_image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value));
		}
	}

	update();
}

void ImageWidget::Restore()
{
	*(ptr_image_) = *(ptr_image_backup_);
	update();
}

void ImageWidget::Input()
{
	input_ = true;
	p_points_.clear();
	q_points_.clear();
	current_ = -1;

}

//对当前图片以及用户提供的控制点对实现IDW算法,扫描所有的坐标并根据算法进行变换
void ImageWidget::WarpingIDW()
{
	QTime start_time = QTime::currentTime();
	input_ = false;
	if (p_points_.empty())
		return;
	*(ptr_image_temp_) = *(ptr_image_);
	int width = ptr_image_->width();
	int height = ptr_image_->height();
	for (int i = 1; i < height - 1; i++)
		for (int j = 1; j < width - 1; j++)
			change_[i][j] = false;
	IDW idw = IDW(p_points_, q_points_, 2);
	QPoint t, t1;
	ptr_image_->fill(Qt::white);
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{
			t = QPoint(i, j);
			//bool参数设置求变换矩阵的方式
			t1 = idw.p2q(t, false);
			if (t1.x() > 0 && t1.y() > 0 && t1.x() < width&&t1.y() < height)
				change_[t1.y()][t1.x()] = true;
			ptr_image_->setPixel(t1, ptr_image_temp_->pixel(t));
		}
	QTime stop_time = QTime::currentTime();
	cout << "IDW: "<<start_time.msecsTo(stop_time) << endl;
	FillHole();
	update();
}

//对当前图片以及用户提供的控制点对实现RBF算法,扫描所有的坐标并根据算法进行变换
void ImageWidget::WarpingRBF()
{
	QTime start_time = QTime::currentTime();
	input_ = false;
	if (p_points_.empty())
		return;
	*ptr_image_temp_ = *ptr_image_;
	int width = ptr_image_->width();
	int height = ptr_image_->height();

	for (int i = 1; i < height - 1; i++)
		for (int j = 1; j < width - 1; j++)
			change_[i][j] = false;

	RBF rbf = RBF(p_points_, q_points_, -1);
	QPoint t, t1;

	ptr_image_->fill(Qt::white);

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{
			t = QPoint(i, j);
			t1 = rbf.p2q(t);
			if (t1.x() > 0 && t1.y() > 0 && t1.x() < width&&t1.y() < height)
				change_[t1.y()][t1.x()] = true;
			ptr_image_->setPixel(t1, ptr_image_temp_->pixel(t));
		}
	QTime stop_time = QTime::currentTime();
	cout << "RBF: " << start_time.msecsTo(stop_time) << endl;
	FillHole();
	update();
}

void ImageWidget::FillHole()
{
	int x[8] = { 0,1,1,1,0,-1,-1,-1 };
	int y[8] = { -1,-1,0,1,1,1,0,-1 };
	int width = ptr_image_->width();
	int height = ptr_image_->height();

	QColor color;

	//对每个像素点进行扫描,若是未被映射到的点,则根据周围8个点的信息来确定颜色
	for (int i = 1; i < height - 1; i++)
		for (int j = 1; j < width - 1; j++)
		{
			if (!change_[i][j])
			{
				color = Qt::white;

				int r = 0, g = 0, b = 0;		//周围已被映射到的点的颜色信息(用于求red,green,blue的和)
				int tr = 0, tg = 0, tb = 0;		//周围8个点的颜色信息(用于求red,green,blue的和)
				int t = 0;						//周围已被映射到的点的数量

				//分别计算周围8个点的红,绿,蓝值之和与周围已被映射到的点的红,绿,蓝值之和
				for (int p = 0; p < 8; p++)
				{
					color = ptr_image_->pixelColor(j + y[p], i + x[p]);
					if (change_[i + x[p]][j + y[p]])
					{
						r += color.red();
						g += color.green();
						b += color.blue();
						t++;
					}
					tr += color.red();
					tg += color.green();
					tb += color.blue();
				}
				//若周围有已被映射到的点,则确定该点的颜色为周围已被映射到的点的颜色平均值,否则为周围8个点的颜色平均值
				if (t)
					ptr_image_->setPixelColor(j, i, QColor(r / t, g / t, b / t));
				else
					ptr_image_->setPixelColor(j, i, QColor(tr / 8, tg / 8, tb / 8));
			}
		}
}
