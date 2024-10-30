#pragma once

#include <string>
#include <windows.h>
#include <WtsApi32.h>

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\FuncBase.h"			// ワーカースレッドマネージャー

#pragma comment(lib, "Wtsapi32.lib")

class AllUserLogOff : public FuncBase
{

// 公開関数
public:
	AllUserLogOff(void);
	~AllUserLogOff(void);

	bool Exec();				// リモート接続しているユーザーにログオフを実行
	bool Change();				// コンソール側が 表でない時には、コンソールを表に変更する

};

