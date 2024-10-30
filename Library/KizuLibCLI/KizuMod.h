// *********************************************************************************
//	VB用ライブラリ関数
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//
// *********************************************************************************
#pragma once

#include <windows.h>
#include "..\KizuLib\KizuLib.h"											// 疵検ライブラリヘッダー
#include "..\..\Include\KizuCommon.h"									// 疵検共通ヘッダー
#include "..\..\Include\KizuCommonCLI.h"								// 疵検CLI共通ヘッダー


using namespace System;
using namespace System::Runtime::InteropServices;


namespace KizuLibCLI
{
	// メール受信時のイベント
	public delegate void EvRecvMailSlot(Object^ pData, int nEvtNo);

	/// 疵検ライブラリ (C++とVBの受け渡し)
	public ref class KizuMod
	{
	public:

		KizuMod();																// コンストラクタ
		virtual ~KizuMod();														// デストラクタ


		// メールスロット用
		event		EvRecvMailSlot^	evRecvMail;									// メールスロット受信イベント	 (デリゲート)				
		int			Task_Init(String^ cTaskName);								// タスク生成
		int			Task_Init(String^ cTaskName, int nPCID);					// タスク生成
		void		Task_Exit();												// タスク終了
		static int	Send_Mail(String^ SendSlotName, String^ SendPcName, Object^ pDate);	// メール送信
		//int	Recv_Mail(Object^% pDate, Type^ structureType);					// メール受信 (あまり使わない)
		static int	Send_Mail_Trans(String^ SendSlotName, String^ SendPcName, Object^ pDate);	// メール転送
		static int	Send_Mail_Trans(String^ SendUdpAddr, String^ SendSlotName, String^ SendPcName, Object^ pDate);	// メール転送
		void		typConv(Object^ pTo, Object^ pFrom);						// クラス変換 (newする必要有り)
		ValueType^	typConv(Object^ pFrom, Type^ structureType);				// 構造体変換 (newする必要無し)


		// その他ライブラリアクセス
		static bool GetPcName()				{ return getpcname(); };			// 全PC名称をDBより取得
		static String^ GetMyPcName()		{ return Marshal::PtrToStringAnsi( IntPtr( (char*)getmypcname() )); };	// 自PC名称取得
		static int	SysLog(int nSysNo, String^ cMsg);							// システムログ出力
		static void SetStatus(int id, int state);								// DB_SYSTEM_STATEに機器状態をセットする (0:正常)
		static void SetStatus(int id, bool state);								// DB_SYSTEM_STATEに機器状態をセットする (true:正常)
		static void SetStatus(int id, int state, bool bLogDsp);					// DB_SYSTEM_STATEに機器状態をセットする (0:正常)
		static void SetStatus(int id, bool state, bool bLogDsp);				// DB_SYSTEM_STATEに機器状態をセットする (true:正常)


		static bool			ksMemCreate(String^ cTblName, int size);			// 共有メモリ 生成
		static void			ksMemClose(String^ cTblName);						// 共有メモリ 開放


		static ValueType^	ksMemRead(String^ cTblName, Type^ structureType);								// 共有メモリ データ読込 (これを使うなら ↓を使うこと)
		static ValueType^	ksMemRead(String^ cTblName, int startIndex, Type^ structureType, int^% nRetc);	// 共有メモリ データ読込 (これを使うなら ↓を使うこと)
		static bool			ksMemRead(String^ cTblName, int startIndex, ValueType^% p);						// 共有メモリ データ読込 (構造体読込み) ※ 基本これを使用すること

		static bool			ksMemRead(String^ cTblName, int size, array<unsigned char>^ byteVal);						// 共有メモリ データ読込 (バイト配列読込み)					※ 構造体読込みが可能となったため、使う必要ほぼ無し
		static bool			ksMemRead(String^ cTblName, int startIndex, int size, array<unsigned char>^ byteVal);		// 共有メモリ データ読込 (バイト配列読込み) ※ 構造体読込みが可能となったため、使う必要ほぼ無し

		static bool			ksMemWrite(String^ cTblName, ValueType^ tbl);												// 共有メモリ データ書込み

		// 共有メモリ部分のみマッピング (参照するテーブルが大きい場合は、これを使用すること)
		static ValueType^	ksMemReadMap(String^ cTblName, int startIndex, int size, Type^ structureType, int^% nRetc);	// 共有メモリ データ読込 (これを使うなら ↓を使うこと)
		static bool			ksMemReadMap(String^ cTblName, int startIndex, int size, ValueType^% p);					// 共有メモリ データ読込 (構造体読込み) ※ 基本これを使用すること

		static bool			ksMemWriteMap(String^ cTblName, int startIndex, int size, ValueType^ tbl);					// 共有メモリ上のデータ書込み


		// こまごま
		static int			GetPcID()		{ return getpcid(); }				// 自PCID (1オリジン)
		static void			SetLineID()		{ setlineid(); }					// 自ラインIDセット
		static void			SetLineID(int nLineID) { setlineid(nLineID);}		// 自ラインIDセット
		static int			GetLineID()		{ return getlineid(); }				// 自ラインID取得
		static String^		GetMyTaskName() { return Marshal::PtrToStringAnsi( IntPtr( (char*)getmytaskname() )); }	// 自タスク名称

		// PC名称取得用
		static String^		Pcid_Name(int pcid) {return Marshal::PtrToStringAnsi(IntPtr( PCID_NAME(pcid) )); }	// PC名称 (pcid 1オリジン) Ver.1.1.1.0

		static String^		Hantei(int no)	{ return Marshal::PtrToStringAnsi(IntPtr( HANTEI(no) )); }	// 判定機能を有するPC名称
		static String^		Tokatu()		{ return Marshal::PtrToStringAnsi(IntPtr( TOKATU )); }		// 統括機能を有するPC名称
		static String^		Tikuseki()		{ return Marshal::PtrToStringAnsi(IntPtr( TIKUSEKI )); }	// 蓄積機能を有するPC名称
		static String^		Sousa()			{ return Marshal::PtrToStringAnsi(IntPtr( SOUSA )); }		// 操作機能を有するPC名称
		static String^		Hyouzi(int no)	{ return Marshal::PtrToStringAnsi(IntPtr( HYOUZI(no) )); }	// 表示機能を有するPC名称

		static String^		MasterPcName()	{ return Marshal::PtrToStringAnsi(IntPtr( MASTER_PC_NAME )); }	// 疵検状態管理送り先PC名称
		static String^		SyslogPcName()	{ return Marshal::PtrToStringAnsi(IntPtr( SYSLOG_PC_NAME )); }	// syslog送り先PC名称
		
	private:
		// メールスロット用
		static void MainThread(Object^ arg);									// 受信用スレッド
		Threading::Thread ^ mcls_thread;										// 受信用スレッドインスタンス
		Threading::AutoResetEvent ^	m_evStop;									// 受信用スレッド停止

	};
}