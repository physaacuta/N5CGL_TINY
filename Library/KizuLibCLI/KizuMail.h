// *********************************************************************************
//	MaliSlotアクセス用ライブラリ関数
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//
// *********************************************************************************
#pragma once

#include "..\KizuLib\KizuLib.h"
#include "..\..\Include\KizuCommonCLI.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace KizuLibCLI
{
	// メール受信時のイベント
	public delegate void EvRecvMailSlot(Object^ pData, int nEvtNo);

	/// 疵検ライブラリ (C++とVBの受け渡し)
	public ref class KizuMail
	{

	public :

		event EvRecvMailSlot^	evRecvMail;										// メールスロット受信イベント	 (デリゲート)				

		KizuMail();																// コンストラクタ
		virtual ~KizuMail();													// デストラクタ

		// メール
		int Task_Init(String^ cTaskName);	
		int Task_Init(String^ cTaskName, String^ cPcName);						// タスク生成
		void Task_Exit();														// タスク終了
		static int Send_Mail(String^ SendSlotName, String^ SendPcName, Object^ pDate);	// メール送信
		//int	Recv_Mail(Object^% pDate, Type^ structureType);						// メール受信 (あまり使わない)
	
		void typConv(Object^ pTo, Object^ pFrom);								// クラス変換 (newする必要有り)
		Object^ typConv(Object^ pFrom, Type^ structureType);					// 構造体変換 (newする必要無し)

	private:
		static void MainThread(Object^ arg);									// 受信用スレッド

		Threading::Thread ^ mcls_thread;										// 受信用スレッドインスタンス
		Threading::AutoResetEvent ^	m_evStop;									// 受信用スレッド停止
	};
}
