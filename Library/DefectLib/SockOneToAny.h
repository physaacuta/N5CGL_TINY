#pragma once
// 半固定
#include "..\..\Include\KizuCommon.h"									// 疵検インクルード


// 20150316 追加 DefectLib/SockOneToAnySend
struct SOCK_F_HEAD2 {
	SOCK_BASE_HEAD	base_head;								// 共通ヘッダ部

	// ------>> DefectLib/SockOneToAnySend にて使用
	// ※ SOCK_BASE_HEADでは、SOCK_F_HEADから連続データ＋addrの両方に対応出来ない対策

	struct SOCK_BASE_HEAD_ADDR {
		// SOCK_BASE_HEAD/len は、全てのデータサイズ。len = len_head+len_add[0]+len_add[1]
		long			len_head;				// base_headからの連続データ
		long			len_addr[2];			// addr部分のデータ長
	} base_head_addr;

	// <<------

	char			reserve[SIZE_SOCK_HEAD-sizeof(SOCK_BASE_HEAD)-sizeof(SOCK_BASE_HEAD_ADDR)];	// 各PC間通信で可変ヘッダー部
};
