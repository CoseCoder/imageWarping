#include "IDW.h"
#include<iostream>
using namespace std;

IDW::IDW()
{
}

IDW::IDW(vector<QPoint> p_points, vector<QPoint> q_points, double u = 2)
{
	u_ = u;
	p_points_ = p_points;
	q_points_ = q_points;
	size_ = p_points_.size();

	t_ = new double*[2];
	for (int i = 0; i < 2; i++)
		t_[i] = new double[size_ * 2];

	//计算每个数据点对对应的变换矩阵Ti
	for (int i = 0; i < size_; i++)
		calculateT(p_points_[i], i);
}

IDW::~IDW()
{
	delete t_[0];
	delete t_[1];
	delete[] t_;
}

double IDW::getDistance(QPoint p, QPoint q)
{
	return (p.x() - q.x())*(p.x() - q.x()) + (p.y() - q.y())*(p.y() - q.y());
}

void IDW::calculateSum(QPoint p)
{
	sum_ = 0;
	for (size_t i = 0; i < size_; i++)
	{
		if (getDistance(p, p_points_[i]) == 0)
			continue;
		sum_ += 1.0 / pow(getDistance(p, p_points_[i]), u_ / 2.0);
	}
}

double IDW::getWi(QPoint p, int i)
{
	calculateSum(p);
	if (p == p_points_[i])
		return 1;
	return 1.0 / pow(getDistance(p, p_points_[i]), u_ / 2.0) / sum_;
}

void IDW::calculateT(QPoint p, int i)
{

	//代入公式计算变换矩阵的值
	double det = 0, wij;
	double t1 = 0, t2 = 0, t3 = 0, t4, px, py, qx, qy;
	t_[0][2 * i] = t_[0][2 * i + 1] = t_[1][2 * i] = t_[1][2 * i + 1] = 0;

	for (int j = 0; j < size_; j++)
	{
		if (j == i)
			continue;
		wij = getWi(p, j);
		px = p_points_[j].x() - p_points_[i].x();
		py = p_points_[j].y() - p_points_[i].y();
		t1 += wij*px*px;
		t2 += wij*py*py;
		t3 += wij*px*py;
	}

	det = t1*t2 - t3*t3;

	if (det == 0)
	{
		for (int j = 0; j < size_; j++)
		{
			if (j == i)
				continue;
			wij = getWi(p, j);
			px = p_points_[j].x() - p_points_[i].x();
			py = p_points_[j].y() - p_points_[i].y();
			det += wij*(px*px + py*py);
		}
		for (int j = 0; j < size_; j++)
		{
			if (j == i)
				continue;
			wij = getWi(p, j);
			px = p_points_[j].x() - p_points_[i].x();
			py = p_points_[j].y() - p_points_[i].y();
			qx = q_points_[j].x() - q_points_[i].x();
			qy = q_points_[j].y() - q_points_[i].y();
			t_[0][2 * i] += wij*(px*qx + py*qy);
			t_[0][2 * i + 1] += wij*(py*qx - px*qy);
		}
		t_[0][2 * i] /= det;
		t_[1][2 * i + 1] = t_[0][2 * i];
		t_[0][2 * i + 1] /= det;
		t_[1][2 * i] = -t_[0][2 * i + 1];
	}
	else
	{
		t1 = t2 = t3 = t4 = 0;
		for (int j = 0; j < size_; j++)
		{
			if (j == i)
				continue;
			wij = getWi(p, j);
			px = p_points_[j].x() - p_points_[i].x();
			py = p_points_[j].y() - p_points_[i].y();
			qx = q_points_[j].x() - q_points_[i].x();
			qy = q_points_[j].y() - q_points_[i].y();
			t1 += wij*qx*px;
			t2 += wij*py*py;
			t3 += wij*qx*py;
			t4 += wij*px*py;
		}
		t_[0][2 * i] = (t1*t2 - t3*t4) / det;

		t1 = t2 = t3 = t4 = 0;
		for (int j = 0; j < size_; j++)
		{
			if (j == i)
				continue;
			wij = getWi(p, j);
			px = p_points_[j].x() - p_points_[i].x();
			py = p_points_[j].y() - p_points_[i].y();
			qx = q_points_[j].x() - q_points_[i].x();
			qy = q_points_[j].y() - q_points_[i].y();
			t1 += wij*qx*py;
			t2 += wij*px*px;
			t3 += wij*qx*px;
			t4 += wij*px*py;
		}
		t_[0][2 * i + 1] = (t1*t2 - t3*t4) / det;

		t1 = t2 = t3 = t4 = 0;
		for (int j = 0; j < size_; j++)
		{
			if (j == i)
				continue;
			wij = getWi(p, j);
			px = p_points_[j].x() - p_points_[i].x();
			py = p_points_[j].y() - p_points_[i].y();
			qx = q_points_[j].x() - q_points_[i].x();
			qy = q_points_[j].y() - q_points_[i].y();
			t1 += wij*qy*px;
			t2 += wij*py*py;
			t3 += wij*qy*py;
			t4 += wij*px*py;
		}
		t_[1][2 * i] = (t1*t2 - t3*t4) / det;

		t1 = t2 = t3 = t4 = 0;
		for (int j = 0; j < size_; j++)
		{
			if (j == i)
				continue;
			wij = getWi(p, j);
			px = p_points_[j].x() - p_points_[i].x();
			py = p_points_[j].y() - p_points_[i].y();
			qx = q_points_[j].x() - q_points_[i].x();
			qy = q_points_[j].y() - q_points_[i].y();
			t1 += wij*qy*py;
			t2 += wij*px*px;
			t3 += wij*qy*px;
			t4 += wij*px*py;
		}
		t_[1][2 * i + 1] = (t1*t2 - t3*t4) / det;
	}

}


QPoint IDW::getFiPoint(QPoint p, int i, bool simple)
{
	QPoint result;

	double x = p.x() - p_points_[i].x();
	double y = p.y() - p_points_[i].y();

	if (simple)
	{
		result.rx() = q_points_[i].x() + x;
		result.ry() = q_points_[i].y() + y;
	}
	else
	{
		result.rx() = q_points_[i].x() + x*t_[0][2 * i] + y*t_[0][2 * i + 1];
		result.ry() = q_points_[i].y() + x*t_[1][2 * i] + y*t_[1][2 * i + 1];
	}

	return result;
}

QPoint IDW::p2q(QPoint p, bool simple = true)
{
	double x = 0, y = 0, wi;
	QPoint fip;

	for (size_t i = 0; i < size_; i++)
	{
		fip = getFiPoint(p, i, simple);
		wi = getWi(p, i);
		x += fip.x() * wi;
		y += fip.y() * wi;
	}

	return QPoint(x, y);
}
