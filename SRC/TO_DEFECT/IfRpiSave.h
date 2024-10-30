// *********************************************************************************
//	太平製全長画像処理とのI/Fクラス
//	[Ver]
//		Ver.01    2011/12/11  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

#include <winsock2.h>													// UDP送信用

// 疵検 インクルード
#include "..\..\include\LineCommon.h"									// 疵検共通ヘッダー
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリヘッダー
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// クリティカルセクション制御クラス

// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー

// 全長画像保存用
#include "..\..\include\OutIF_HT_RAW.h"									// 他システム公開用ヘッダー

using namespace KizuLib;




//#define SEND_RPI_MGR		// TO_RPIMGR送信時 コメントアウト (自動バックアップ切替対応版)



class IfRpiSave
{
	//// 公開関数
public:
	IfRpiSave(void);
	virtual ~IfRpiSave(void);

	// メールスロット関係
	static void Send_TiRpiSave_Start();									// 全長画像処理 へ 検査開始要求
	static void Send_TiRpiSave_Stop();									// 全長画像処理 へ 検査停止要求
	static void Send_TiRpiSave_CoilSend(char const* cKizukenNo, COIL_SETTING const* setting, COIL_BASEDATA const * base, int framenum, int front_pos, int nMen);		// 全長画像処理 へ 次コイル情報 [←CoilManager]


};
