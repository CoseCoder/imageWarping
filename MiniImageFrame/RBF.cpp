#include "RBF.h"
#include<iostream>
using namespace std;

RBF::RBF()
{
}

RBF::RBF(vector<QPoint>& p_points, vector<QPoint>& q_points, int u = -1)
{
	p_points_ = p_points;
	q_points_ = q_points;
	u_ = u;
	size_ = p_points_.size();
	d_ = new double[size_*size_];
	r_ = new double[size_];
	calculateDistanceMartix();
	calculateRi();
	calculateAi();
}

RBF::~RBF()
{
	delete d_;
	delete r_;
}

double RBF::getDistance(QPoint p, QPoint q)
{
	return (p.x() - q.x())*(p.x() - q.x()) + (p.y() - q.y())*(p.y() - q.y());
}

void RBF::calculateDistanceMartix()
{
	for (size_t i = 0; i < size_; i++)
	{
		d_[i*size_ + i] = 0;
		for (size_t j = i + 1; j < size_; j++)
		{
			d_[i*size_ + j] = d_[j*size_ + i] = getDistance(p_points_[i], p_points_[j]);
		}
	}
}

void RBF::calculateRi()
{
	double min;

	//计算每个p数据点与其他p数据点之中最小的距离,该值即为Ri
	//Ri=距离矩阵中每行(或每列)最小的值(除去与自身的距离->d_[i*size+i])
	for (size_t i = 0; i < size_; i++)
	{
		min = INT_MAX;
		for (size_t j = 0; j < size_; j++)
		{
			if (min > d_[i*size_ + j] && i != j)
				min = d_[i*size_ + j];
		}
		r_[i] = min;
	}
}

void RBF::calculateAi()
{
	Matrix<double, Dynamic, Dynamic> K(size_, size_);
	VectorXd Bx(size_);
	VectorXd By(size_);
	for (size_t i = 0; i < size_; i++)
	{
		Bx(i) = q_points_[i].x() - p_points_[i].x();
		By(i) = q_points_[i].y() - p_points_[i].y();
		for (size_t j = 0; j < size_; j++)
		{
			K(i, j) = pow(d_[j*size_ + i] + r_[j], u_);
		}
	}
	Ax_ = K.colPivHouseholderQr().solve(Bx);
	Ay_ = K.colPivHouseholderQr().solve(By);
}

QPoint RBF::p2q(QPoint p)
{
	double x = p.x(), y = p.y(), t;
	for (int i = 0; i < size_; i++)
	{
		t = pow(getDistance(p, p_points_[i]) + r_[i], u_);
		x += Ax_(i)*t;
		y += Ay_(i)*t;
	}
	return QPoint(x, y);
}