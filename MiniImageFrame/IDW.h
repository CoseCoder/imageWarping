#pragma once
#include <vector>
#include <QPoint>
using namespace std;
class IDW
{
public:
	IDW();
	IDW(vector<QPoint> p_points, vector<QPoint> q_points, double u);
	~IDW();

public:
	QPoint p2q(QPoint p, bool simple);					//计算IDW算法将p点映射到的点
														//simple为true表示直接将每个局部近似函数fi(x)的变换矩阵T当成2阶单位矩阵
														//simple为false表示计算每个局部近似函数fi(x)的变换矩阵T

private:
	double getDistance(QPoint p, QPoint q);				//计算2点距离的平方

	void calculateSum(QPoint p);						//计算σi(x)=1/(di(x))^u的和

	void calculateT(QPoint p, int i);					//计算局部近似函数fi(x)的变换矩阵T

	double getWi(QPoint p, int i);						//计算每个数据点对的权重
		
	QPoint getFiPoint(QPoint p, int i, bool simple);	//计算p点在局部近似函数fi(x)映射后的点
														//simple为true表示直接将局部近似函数fi(x)的变换矩阵T当成2阶单位矩阵
														//simple为false表示计算局部近似函数fi(x)的变换矩阵T


private:
	double u_;						//计算公式中的一个参数

	double sum_;					//σi(x)=1/(di(x))^u的和

	double **t_;					//存储每个数据点对对应的变换矩阵T
									//Ti=(t_[0][2*i],t_[0][2*i+1])
									//    (t_[1][2*i],t_[1][2*i+1])

	int size_;						//输入数据点对的数量
								
	vector<QPoint> p_points_;		//输入的数据点对
	vector<QPoint> q_points_;

};