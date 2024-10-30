#pragma once
#include "..\..\library\kizulibmfc\class\threadmanager.h"
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL文生成補助クラス
#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB操作クラス

#include "MainConst.h"



class TestMaeManager :
	public ThreadManager
{
public:
////　公開関数
public:
	//=============================
	// 本クラス受け渡し用クラス
	//=============================
	//// メール情報受け渡しキュー
	struct DELIVERY_KEY {
		char	cKizukenNo[SIZE_KIZUKEN_NO + 1];		// 疵検管理No
		char	cKeyInf[SIZE_COIL_KEY + 1];				// キー情報
		char	cMeiNo[SIZE_MEI_NO + 1];				// 命令No
		char	cCoilNo[SIZE_COIL_NO + 1];				// HOTコイルNo
		int		nCoilLen;
		double	dNobiSPM;								// SPM伸び率[10^2%]

	};
	ThreadQueue<DELIVERY_KEY>	gque_Deli;

	// 前工程疵情報構造体
	struct MAE_DEFECT_IMAGE_INF
	{
		char cKizuKenNo[SIZE_KIZUKEN_NO];		// 疵検管理No
		char cPreKizuKenNo[SIZE_KIZUKEN_NO];	// 前工程疵検管理No
		int  nKouteiSyubetu;					// 工程種別「疵検出工程判」部分の何番目か
		int  nKizuNo;							// 疵No
		int  nMaeKizuNo;						// 前工程疵No
		int  nOuDiv;							// 表裏区分
		int  nCnt;								// 画像データ数
		DETECT_BASE_DATA typDefectData;			// 疵情報
		int  nGazouNo;							// 画像No
		DWORD nSize[MAX_MAE_IMAGE];				// 画像サイズ
		BYTE* pImg[MAX_MAE_IMAGE];				// 画像データ
	};


	TestMaeManager(void);
	virtual ~TestMaeManager(void);

//// メンバ関数
private:



	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);				

	void Exec( DELIVERY_KEY* pDeli);
	bool SetMaeCoilInf(char const* cKizukenNo, char const* cKeyInf, char const* cMeiNo, char const* cCoilNo);
	bool SetMaeFFInf(char const* cKizukenNo , int nKizuNo, int ou, double	dNobiSPM, int nKouteiID);
	bool SetMaeCoilLine(char const* cKizukenNo,int ou,int nKouteiID);
	bool SetMaeDefectInf(MAE_DEFECT_IMAGE_INF* typData);
	bool CreateDefectInf(char const* cKizukenNo , int nKizuNo, int ou, int nKouteiID);

	bool InsertDefectImg(MAE_DEFECT_IMAGE_INF* typData);

	void Send_Hy_MaeRead(char const* cKizukenNo);



};
