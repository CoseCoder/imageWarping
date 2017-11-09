#pragma once
#include <vector>
#include <QPoint>
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;

class RBF
{
public:
	RBF();
	RBF(vector<QPoint> &p_points, vector<QPoint> &q_points, int u);
	~RBF();

public:
	QPoint p2q(QPoint p);						//计算RBF算法将p点映射到的点

private:
	double getDistance(QPoint p, QPoint q);		//计算2点距离的平方

	void calculateDistanceMartix();				//计算任意2个p数据点的距离形成的矩阵

	void calculateRi();							//计算每个p数据点对应的R值

	void calculateAi();							//计算出线性方程组中的ax,ay

private:
	double u_;						//计算公式中的一个参数

	int size_;						//输入数据点对的数量

	vector<QPoint> p_points_;		//输入的数据点对
	vector<QPoint> q_points_;

	double *d_;						//距离矩阵
									//(pi,pj这2点距离的平方)d(i,j)^2=d_[i*size+j]=d_[j*size+i]
									//d(i,i)^2=d_[i*size+i]=0;
		
	double *r_;						//Ri数组(存储每个p数据点对应的R值)

	VectorXd Ax_;					//线性方程组的解
	VectorXd Ay_;
};
