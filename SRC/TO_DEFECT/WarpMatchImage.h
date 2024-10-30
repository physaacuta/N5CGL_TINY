//
// WarpMatchImage.h
// 複眼ずれ計算用ライブラリ
// すべて入出力は画素単位

#pragma once

namespace SwdetectImm {

	struct ManualAdjustCoef {
		int Xdelay;			// X遅延
		int Xinsert;		// X挿入
		int YdelayLeft;		// Y遅延左	
		int YdelayRight;	// Y遅延右
	};

	struct ProjCoef {
		//	x' = ((a1+1)*x + c1) / (a0*x + 1)
		//	y' = (a2*x + y + c2) 
		double a0;
		double a1;
		double a2;
		double c1;
		double c2;
	};

	struct Dpoint {	// 全部単位は画素、mmで与えてはいけない
		int	x;
		double dx;
		double dy;
	};

	struct RANSAC_param {
		int nsamp;			// 全部のデータ点数より小さくないといけない
		int ntrial;			// 試行回数
		double error_limit;	// 許容誤差 画素単位なので、0.3ぐらいが適当
	};


	// X遅延,X挿入,Y遅延左右から射影変換パラメータを求める
	// neyesは眼数、camwidthはカメラの横ビット数、m[neyes],p[neyes]である。
	// 複眼の場合neyes=2, m[2]であり、m[0]は正、m[1]は乱の値を入れて呼ぶと
	// 正画像座標(x,y)→乱画像座標(x',y')に対しての射影変換パラメータがp[1]に入る
	BOOL manual2proj(int neyes, int camwidth, const ManualAdjustCoef* m, ProjCoef* p);

	// カメラ幅方向位置xについて
	// 射影変換パラメータがpであったときの、
	// 正カメラに対するズレ量dx,dyを求める
	BOOL calcDxDy(const ProjCoef& p, Dpoint& d); 

	// 特徴量のズレ量データ配列d[n]から
	// 射影変換係数pを求める
	BOOL calcProj(int n, const Dpoint* d, ProjCoef& p);
	
	BOOL calcProj_ransac(int n, const Dpoint* d, ProjCoef& p, const RANSAC_param* rp=NULL);

};


