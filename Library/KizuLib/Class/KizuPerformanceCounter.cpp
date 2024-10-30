// KizuPerformanceCounter.cpp: KizuPerformanceCounter クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KizuPerformanceCounter.h"
#include <crtdbg.h>

using namespace KizuLib;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
//------------------------------------------
KizuPerformanceCounter::KizuPerformanceCounter()
{
	// 周波数取得
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_pf);
	m_pc = 0;

	// 共通用
	m_ave = 0.0;								// 平均時間	[ms] 
	m_min = 9999.0;								// 最小値
	m_max = 0.0;								// 最大値
	m_bFast = false;
	m_Cycle = 0.0;								// 間隔 [ms]

	// ストップウォッチ機能
	m_nIndex = 0;
	memset(m_timestamp, 0x00, sizeof(m_timestamp));
}

//------------------------------------------
// デストラクタ
//------------------------------------------
KizuPerformanceCounter::~KizuPerformanceCounter()
{

}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2点間時間算出用

//------------------------------------------
// 2点間時間算出用 初期化
//------------------------------------------
void KizuPerformanceCounter::InitAve()
{
	m_pc = 0;

	m_ave = 0.0;								// 平均時間	[ms] 
	m_min = 9999.0;								// 最小値
	m_max = 0.0;								// 最大値
	m_bFast = false;
	m_Cycle = 0.0;								// 間隔 [ms]
}
//------------------------------------------
// 時間算出開始
//------------------------------------------
void KizuPerformanceCounter::CounterStart()
{
	QueryPerformanceCounter((LARGE_INTEGER *)&m_pc);
}
//------------------------------------------
// 時間算出終了
//------------------------------------------
void KizuPerformanceCounter::CounterEnd()
{
	// 時間差算出
	__int64 pc2;
	QueryPerformanceCounter((LARGE_INTEGER *)&pc2);
	m_Cycle = (double)(pc2 - m_pc)/m_pf * 1000.0;	// [s] → [ms] 
	
	// 最大最小時間算出
	if ( m_min > m_Cycle ) m_min = m_Cycle;
	if ( m_max < m_Cycle ) m_max = m_Cycle;

	// 平均時間算出 (簡易算出)
	if(m_bFast) {
		m_ave = m_ave*0.8 + m_Cycle*0.2;
	} else{
		m_ave = m_Cycle;
		m_bFast = true;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 周期算出用

//------------------------------------------
// 周期算出用 初期化
//------------------------------------------
void KizuPerformanceCounter::InitCycle()
{
	m_pc = 0;
	m_Cycle = 0.0;								// 間隔 [ms]

	m_ave = 0.0;								// 平均時間	[ms] 
	m_min = 9999.0;								// 最小値
	m_max = 0.0;								// 最大値
	m_bFast = false;
}
//------------------------------------------
// 間隔算出
//------------------------------------------
void KizuPerformanceCounter::CounterCycle()
{
	// 時間差算出
	__int64 pc2;
	QueryPerformanceCounter((LARGE_INTEGER *)&pc2);
	m_Cycle = (double)(pc2 - m_pc) / m_pf * 1000.0;	// [s] → [ms] 
	m_pc = pc2;
	
	// 最大最小時間算出
	if ( m_min > m_Cycle ) m_min = m_Cycle;
	if ( m_max < m_Cycle ) m_max = m_Cycle;

	// 平均時間算出
	if(m_bFast) {
		m_ave = m_ave*0.8 + m_Cycle*0.2;
	} else{
		m_ave = m_Cycle;
		m_bFast = true;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ストップウォッチ用
//------------------------------------------
// 外部から強制的に時間記憶
// int no 位置 (0オリジン)
// __int64 time 記憶時間
//------------------------------------------
void KizuPerformanceCounter::PutTimestamp(int no, __int64 time)
{
	_ASSERT(0 <= no && mcMaxTimestamp > no);

	m_timestamp[no] = time;
}

//------------------------------------------
// 時間記憶
// int no 位置 (0オリジン)
// bool bChangeIndex=true Index変更時true
//------------------------------------------
void KizuPerformanceCounter::PutTimestamp(int no, bool bChangeIndex)
{
	_ASSERT(0 <= no && mcMaxTimestamp > no);

	QueryPerformanceCounter((LARGE_INTEGER *)&m_timestamp[no]);
	
	// インデックスの変更
	if(bChangeIndex) {
		m_nIndex = no+1;
		if(m_nIndex>=mcMaxTimestamp) m_nIndex = 0;
	}
}
//------------------------------------------
// 時間記憶
//------------------------------------------
void KizuPerformanceCounter::PutTimestamp()
{
	PutTimestamp(m_nIndex);
}
//------------------------------------------
// 時間取得
// int no 位置 (0オリジン)
// 戻り値 時間 [ms]
//------------------------------------------
double KizuPerformanceCounter::GetTimestamp(int no)
{
	_ASSERT(0 <= no && mcMaxTimestamp > no);

	return (double)m_timestamp[no] / m_pf * 1000.0;	// [s] → [ms] 
}
//------------------------------------------
// 時間差取得
// int no2 位置 (0オリジン)
// int no1 位置 (0オリジン)
// 戻り値 時間 [ms]
//------------------------------------------
double KizuPerformanceCounter::GetTimestamp(int no2, int no1)
{
	_ASSERT(0 <= no1 && mcMaxTimestamp > no1);
	_ASSERT(0 <= no2 && mcMaxTimestamp > no2);

	return GetTimestamp(no2) - GetTimestamp(no1);
}
