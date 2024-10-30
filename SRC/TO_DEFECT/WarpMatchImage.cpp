#include "StdAfx.h"
#include <windows.h>
#include <crtdbg.h>
#include <math.h>
#include "WarpMatchImage.h"

//
// n点の(x,y)から y = c1x^2 + c2x + c3を求める
//
static bool second_order_fit(const double* x, const double *y, int n, double& c1, double& c2, double& c3)
{
	_ASSERT(0 < n);
	double A = 0;
	double B = 0;
	double C = 0;
	double D = 0;
	double p = 0;
	double q = 0;
	double r = 0;
	double tmp1, tmp2;
	for (int i = 0; i < n; i++) {
		tmp1 = x[i];
		tmp2 = y[i];
		D += tmp1;
		r += tmp2;
		tmp1 *= x[i];
		tmp2 *= x[i];
		C += tmp1;
		q += tmp2;
		tmp1 *= x[i];
		tmp2 *= x[i];
		B += tmp1;
		p += tmp2;
		A += tmp1*x[i];
	}

	double det = (-2*C*B*D+C*C*C+D*D*A+n*B*B-n*A*C);
	if(0.0 == det) return false;
	c1 = (D*D*p-D*B*r-D*q*C+B*n*q-n*p*C+r*C*C)/det;
	c2 = -((D*p*C-D*A*r-B*n*p+B*r*C+n*A*q-q*C*C)/det);
	c3 = -((-D*A*q+A*r*C-B*B*r+C*B*q+p*D*B-p*C*C)/det);
	return true;
}
static bool ransac_second(const double* x, const double* y, int n, double& c1, double& c2, double& c3,
								int ntrial, int nsamp, double error_limit)
{
	int opt_score = 0;

	double* samp_x = new double [nsamp];
	double* samp_y = new double [nsamp];
	int* samp_idx  = new int    [nsamp];

	_ASSERT(n < RAND_MAX);	// データ点数はRAND_MAX以下
	if(n > RAND_MAX) return false;

	do {	// opt_score が 0にならないようにerror_limitをコントロールする
		for (int t = 0; t < ntrial; t++) {
			// nsamp個サンプリング
			for (int i = 0; i < nsamp; ) {
				int s = rand() % n;
				// 同じものをサンプルしていないかチェック
				int j = 0;
				for ( ; j < i; j++) {
					if (s == samp_idx[j]) break;
				}
				if (j == i) {
					samp_x[i] = x[s];
					samp_y[i] = y[s];
					samp_idx[i] = s;
					i++;
				}
			}
			// ２次近似
			double local_c1, local_c2, local_c3;
			if( ! second_order_fit(samp_x, samp_y, nsamp, local_c1, local_c2, local_c3) ) return false;
			// 評価
			int local_score = 0;
			for (int i = 0; i < n; i++) {
				if (fabs(local_c1*x[i]*x[i] + local_c2*x[i] + local_c3 - y[i]) < error_limit) {
					local_score++;
				}
			}
			// 最高値を得る
			if (local_score > opt_score) {
				opt_score = local_score;
				c1 = local_c1;
				c2 = local_c2;
				c3 = local_c3;
			}
		}
		error_limit *= 2;
	} while (0 == opt_score) ;

	delete [] samp_x;
	delete [] samp_y;
	delete [] samp_idx;
	return true;
}

//
// n点の(x,y)から y = c1x + c2を求める
//
static void first_order_fit(const double* x, const double *y, int n, double& c1, double& c2)
{
	_ASSERT(0 < n);
	double A = 0;
	double B = 0;
	double p = 0;
	double q = 0;
	for (int i = 0; i < n; i++) {
		A += x[i]*x[i];
		B += x[i];
		p += y[i]*x[i];
		q += y[i];
	}

	c1 = (B*q - p*n)/(B*B - A*n);
	c2 = (B*p - A*q)/(B*B - A*n);
}

static void ransac_first(const double* x, const double* y, int n, double& c1, double& c2, 
								int ntrial, int nsamp, double error_limit)
{
	int opt_score = 0;

	double* samp_x = new double [nsamp];
	double* samp_y = new double [nsamp];
	int* samp_idx  = new int    [nsamp];

	_ASSERT(n < RAND_MAX);	// データ点数はRAND_MAX以下

	do {	// opt_score が 0にならないようにerror_limitをコントロールする
		for (int t = 0; t < ntrial; t++) {
			// nsamp個サンプリング
			for (int i = 0; i < nsamp; ) {
				int s = rand() % n;
				// 同じものをサンプルしていないかチェック
				int j = 0;
				for ( ; j < i; j++) {
					if (s == samp_idx[j]) break;
				}
				if (j == i) {
					samp_x[i] = x[s];
					samp_y[i] = y[s];
					samp_idx[i] = s;
					i++;
				}
			}
			// 直線近似
			double local_c1, local_c2;
			first_order_fit(samp_x, samp_y, nsamp, local_c1, local_c2);
			// 評価
			int local_score = 0;
			for (int i = 0; i < n; i++) {
				if (fabs(local_c1*x[i] + local_c2 - y[i]) < error_limit) {
					local_score++;
				}
			}
			// 最高値を得る
			if (local_score > opt_score) {
				opt_score = local_score;
				c1 = local_c1;
				c2 = local_c2;
			}
		}
		error_limit *= 2;
	} while (0 == opt_score) ;

	delete [] samp_x;
	delete [] samp_y;
	delete [] samp_idx;
}

BOOL SwdetectImm::manual2proj(int neyes, int camwidth, const ManualAdjustCoef* m, ProjCoef* p)
{
	if (neyes < 2) return FALSE;

	for (int i = 0; i < neyes; i++) {
		if (0 == i) {
			p[i].a0 = 0;
			p[i].a1 = 0;
			p[i].a2 = 0;
			p[i].c1 = 0;
			p[i].c2 = 0;
		} else {
			p[i].a0 = 0;
			p[i].a1 = (double)(m[i].Xinsert - m[0].Xinsert) / (camwidth + m[0].Xinsert);
			p[i].a2 = (double)(m[i].YdelayRight - m[i].YdelayLeft + m[0].YdelayLeft - m[0].YdelayRight) / camwidth;
			p[i].c1 = m[i].Xdelay - (double)(camwidth + m[i].Xinsert)/(camwidth+m[0].Xinsert)*m[0].Xdelay;
			p[i].c2 = m[i].YdelayLeft - m[0].YdelayLeft;
		}
	}
	return TRUE;
}

BOOL SwdetectImm::calcDxDy(const ProjCoef& p, Dpoint& d)
{
	double x_dst = ((p.a1+1)*d.x + p.c1) / (p.a0*d.x+1);
	double y_dst = p.a2*d.x + p.c2;

	d.dx = x_dst - d.x;
	d.dy = y_dst;

	return TRUE;
}

BOOL SwdetectImm::calcProj(int n, const Dpoint* d, ProjCoef& p)
{
/*
	// 配列をばらす
	double* x = new double[n];
	double* x_dash = new double[n];
	double* y_dash = new double[n];

	for (int i = 0; i < n; i++) {
		x[i] = d[i].x;
		x_dash[i] = x[i] + d[i].dx;
		y_dash[i] = d[i].dy;
	}

	// x'は二次近似
	// x' = ((a1+1)*x + c1) / (a0*x + 1) ~= ((a1+1)*x+c1)*(1-a0*x) (1 << a0x)
	double c1, c2, c3;
	second_order_fit(x, x_dash, n, c1, c2, c3);
	p.c1 = c3;
	p.a0 = (-c2+sqrt(c2*c2-4*c1*c3)) / (2*c3);
	p.a1 = -c1 / p.a0 - 1;

	// dyは一次近似
	// dy = (a2*x + c2) 
	first_order_fit(x, y_dash, n, c1, c2);
	p.a2 = c1;
	p.c2 = c2;


	delete [] x;
	delete [] x_dash;
	delete [] y_dash;

	return TRUE;
*/

	// 配列をばらす
	double* x = new double[n];
	double* x_dash = new double[n];
	double* y_dash = new double[n];

	for (int i = 0; i < n; i++) {
		x[i] = d[i].x;
		x_dash[i] = d[i].dx;
		y_dash[i] = d[i].dy;
	}

	// x'は二次近似
	// x' = ((a1+1)*x + c1) / (a0*x + 1)
	// x' - x = dx = (-a0*x^2 + a1*x + c1) / (a0*x + 1) ~= (-a0*x^2 + a1*x + c1)
	double c1, c2, c3;
	second_order_fit(x, x_dash, n, c1, c2, c3);
	p.c1 = c3;
	p.a0 = -c1;
	p.a1 = c2;

	// dyは一次近似
	// dy = (a2*x + c2) 
	first_order_fit(x, y_dash, n, c1, c2);
	p.a2 = c1;
	p.c2 = c2;


	delete [] x;
	delete [] x_dash;
	delete [] y_dash;

	return TRUE;

}


BOOL SwdetectImm::calcProj_ransac(int n, const Dpoint* d, ProjCoef& p, const RANSAC_param* rp)
{
	// 配列をばらす
	double* x = new double[n];
	double* x_dash = new double[n];
	double* y_dash = new double[n];

	for (int i = 0; i < n; i++) {
		x[i] = d[i].x;
		x_dash[i] = d[i].dx;
		y_dash[i] = d[i].dy;
	}

	// x'は二次近似
	// x' = ((a1+1)*x + c1) / (a0*x + 1)
	// x' - x = dx = (-a0*x^2 + a1*x + c1) / (a0*x + 1) ~= (-a0*x^2 + a1*x + c1)
	double c1, c2, c3;
	if (NULL == rp) {
		second_order_fit(x, x_dash, n, c1, c2, c3);
	} else {
		_ASSERT(rp->nsamp < n);
		if( ! ransac_second(x, x_dash, n, c1, c2, c3, rp->ntrial, rp->nsamp, rp->error_limit) ) return FALSE;
	}
	p.c1 = c3;
	p.a0 = -c1;
	p.a1 = c2;

	// dyは一次近似
	// dy = (a2*x + c2) 
	if (NULL == rp) {
	first_order_fit(x, y_dash, n, c1, c2);
	} else {
		_ASSERT(rp->nsamp < n);
		ransac_first(x, y_dash, n, c1, c2, rp->ntrial, rp->nsamp, rp->error_limit);
	}
	p.a2 = c1;
	p.c2 = c2;


	delete [] x;
	delete [] x_dash;
	delete [] y_dash;

	return TRUE;
}