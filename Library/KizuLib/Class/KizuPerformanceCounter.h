// *********************************************************************************
//	時間測定クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応 (コンストラクタでサイズ指定に変更)
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

#include <windows.h>

namespace KizuLib
{
	class KizuPerformanceCounter  
	{
	// メンバー定数
	private:
		static const int		mcMaxTimestamp = 64;						// 最大タイムスタンプ


	// 公開
	public:
		KizuPerformanceCounter();
		virtual ~KizuPerformanceCounter();
		

		//// 時間差
		// 2点間時間算出
		void	InitAve();													// 2点間時間算出用 初期化
		void	CounterStart();												// 時間算出開始
		void	CounterEnd();												// 時間算出終了
		
		// 周期時間算出
		void	InitCycle();												// 周期算出用 初期化
		void	CounterCycle();												// 間隔算出
		
		// 外部アクセス用
		double	GetAveTime()		const { return m_ave; }					// 平均時間	[ms] 
		double	GetCycleTime()		const { return m_Cycle; }				// 間隔		[ms] 
		double	GetMinTime()		const { return m_min; }					// 最小時間 [ms]
		double	GetMaxTime()		const { return m_max; }					// 最大時間 [ms]


		//// ストップウォッチ機能
		void	PutTimestamp(int no, __int64 time);							// 外部から強制的に時間記憶
		void	PutTimestamp(int no, bool bChangeIndex=true);				// 時間記憶
		void	PutTimestamp();												// 時間記憶
		double	GetTimestamp(int no);										// 時間取得 [ms]
		double	GetTimestamp(int no2, int no1);								// 時間差取得 [ms]
		__int64	GetRawTimestamp(int no) const { return m_timestamp[no];};	// 生時間

	private:
		__int64					m_pf, m_pc;									// PC周波数, 測定点

		// 共通
		double					m_min;										// 最小値	[ms]
		double					m_max;										// 最大値	[ms]
		double					m_ave;										// 平均時間	[ms] 
		bool					m_bFast;									// 一回実行後True
		double					m_Cycle;									// 間隔 [ms]
		

		// ストップウォッチ機能
		__int64					m_timestamp[mcMaxTimestamp];				// タイムスタンプ
		int						m_nIndex;									// タイムスタンプインデックス
	};
};
