#pragma once

#include "..\..\Include\LibSelect.h"										// ライブラリ切替ヘッダー

class AtlRegManager
{
public:
	AtlRegManager(void);
	virtual ~AtlRegManager(void);

	static bool IsRegularA(char const* val, char const* match);				// 正規表現 をチェック [1byte ascii] (日本語未対応) ← 一応対応は出来そう
	static bool IsRegularA(char const* val, int valSize, char const* match);// 正規表現 をチェック [1byte ascii] 

	static bool IsRegularMB(unsigned char const* val, unsigned char const* match);
};
