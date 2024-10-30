// *********************************************************************************
//	太平製RPIファイルクラス
//	[Ver]
//		Ver.01    2010/03/04	初版
//
//	[メモ]
//		・非同期ではありません。でも、書込みは早いです。
// *********************************************************************************

#pragma once
// 共通ヘッダー
#include "..\..\Include\KizuCommon.h"									// システム用インクルード


// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// タイマー

#include "..\..\Library\KizuLibMFC\CLASS\CStringCS.h"

#include "..\..\LibPG\LibRpi\rpi.h"
using namespace RpiLib;

class RpiFileManager : public ThreadManager
{
//// 公開定数
public:
	static const int	MAX_SAVE_FOLDER	= 10;							// 保存最大フォルダ数

//// 公開構造体
public:
	
	//=====================================================================
	// システム内

	//// 設定情報 (共通) (システム起動後変更無いもの)
	struct RPI_SETTING {
		// そもそもファイルを作りこむため用 --------
		int				nFastFrame;										// 最初のコイル単位のフレーム格納数 (0は、1コイル1ファイル。1～は、その枚数になったら次ファイルに)
		int				nCycleFrame;									// ２回目以降のフレーム格納数
		int				nFolderNum;										// 設定フォルダ数
		char			cFolderPath[MAX_SAVE_FOLDER][256];				// フォルダパス (最後の\は無し) 
		int				nWriteMode;										// 書込みモード (0:同期書込みモード 1:セクタ書込みモード)
		int				nSecSize;										// セクタサイズ (分からない人は1で良い)

		// 基本部ヘッダ用 --------
		unsigned long	codec;											// 'JPEG' or 'LAC\0' or 'BMP\0'
        long			width;											// 画像幅 (8の倍数)
        long			height;											// 画像高さ (8の倍数)

		float			reso_x;											// 横分解能
		float			reso_y;											// 縦分解能 


		// 太平ヘッダ用 --------
		char			cSysName[20];									// システム略称
//		char			cKizukenNo[SIZE_KIZUKEN_NO];					// 疵検管理No 
		short			frame_info_type;								// フレーム情報部の格納形式 (0:無し 1:T_RPI_FRAME_INFO_T1)
		short			frame_info_size;								// フレーム情報部のサイズ

		long			nLineCenter;									// ラインセンター画素(連結画像左端からのオフセット値) [pixel]
		long			nStartCamset;									// 左端カメラセット番号 (1オリジン)
		long			nCamsetNum;										// 幅方向フレーム結合数 (1オリジン)
		long			nCamAngle;										// カメラ角度 (1オリジン)
		long			nMen;											// 表裏区分 (0:表 1:裏)

		struct CAM_INFO {
			float		dBaseXoffset;									// 基準位置。ラインセンターからの各カメラ左端の位置(左方向は、マイナス値) [mm] (PARAM_CAMSETのXoffset値そのもの)
			int			nBaseLeft;										// 基準画素。結合後のフレーム左端から自カメラ画像左端までの距離
			int			nOverLap;										// 次カメラとの右側のオーバーラップ量
		}caminfo[NUM_CAM_INFO];
		long			nColorType;										// 画像タイプ (0:輝度疵検  1:色差疵検)
		long			nSysType;										// 疵検タイプ (0:連続ライン  1:バッチライン)

	public:
		RPI_SETTING() {													// デフォルトコンストラクタ
			memset(this, 0x00, sizeof(RPI_SETTING));
		};
	};

	//// 設定情報 (コイル単位で可変)
	struct RPI_COIL_INFO {
		// 基本部ヘッダ用 --------
		float			reso_y;											// 縦分解能 

		// 太平ヘッダ用 --------
		char			cKizukenNo[SIZE_KIZUKEN_NO];					// 疵検管理No
		char			cCoilNo[32];									// コイルNo
		DWORD			nCFNo;											// コイル先頭のフレームNo	framenum

		struct CAM_INFO {
			float		dBaseXoffset;									// 基準位置。ラインセンターからの各カメラ左端の位置(左方向は、マイナス値) [mm] (PARAM_CAMSETのXoffset値そのもの)
			int			nBaseLeft;										// 基準画素。結合後のフレーム左端から自カメラ画像左端までの距離
			int			nOverLap;										// 次カメラとの右側のオーバーラップ量
		}caminfo[NUM_CAM_INFO];

		long			nStartLen;										// コイル先頭の長手位置 [mm]
		long			nFTposCfNo[2];									// 先後端位置フレームNo (このファイル先頭からのフレームNo (0オリジン))			※ バッチのみ
		long			nFTposPixel[2];									// 先後端位置フレーム内pixel (そのフレーム上端からのピクセル位置 (0オリジン))
		long			nCompQuality;

	public:
		RPI_COIL_INFO() {												// デフォルトコンストラクタ
			memset(this, 0x00, sizeof(RPI_COIL_INFO));
		};

	};
	//// 設定情報 (フレーム単位で可変)
	struct RPI_FRAME_INFO {
		DWORD			nFNo;											// 今のフレームNo
	};


//// 公開関数
public:
	RpiFileManager(char const* cThreadName);
	virtual ~RpiFileManager(void);


	bool				RpiOpen();										// RPIファイルオープン
	bool				RpiClose();										// RPIファイルクローズ
	bool				WriteFrame(int nImgSize, int nFrameSize, BYTE* pFrame);	// フレーム情報専用
	
//	void				UpdateHeader();									// ヘッダー情報更新


	//=========================================
	// 外部アクセス
	RPI_SETTING&		RpiSetting()	{ return my_Setting; }			// RPIファイル設定情報プロパティ
	RPI_COIL_INFO&		RpiCoilInfo()	{ return my_CoilInfo; }			// RPIファイル設定情報プロパティ
	RPI_FRAME_INFO&		RpiFrameInfo()	{ return my_FrameInfo; }		// RPIファイル設定情報プロパティ

	RpiLib::RpiHeader const*	GetRpiHeader() const ;

	bool				IsOpen() const;
	CString				GetRpiFullPath() { return CStringCS::LockCpy( my_sFullPath); }		// CStringで落ちた対策
	CString				GetRpiFileName() { return my_sFileName; }

	double				GetWriteSpeedAve() const { return my_TimeWrite[0]; }	// 書込み速度
	double				GetWriteSpeedMin() const { return my_TimeWrite[1]; }	// 書込み速度
	double				GetWriteSpeedMax() const { return my_TimeWrite[2]; }	// 書込み速度


	// モジュール
	static int			GetSecSize(char const* cDrivePath="c:\\");				// HDDのセクタサイズを取得する
	static int			GetSizeJust(int x, int l) { return ((((x-1)/l)+1)*l); }	// xサイズをlの倍数に揃える(必ず、xのままか、ちょっと大きくなる方向)
					


//// 基本 (公開しない)
private:
	virtual int ThreadFirst() { return 0; };							// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	virtual int ThreadLast() { return 0; };								// スレッド終了前処理 (終了シグナル後にコールバック)
    virtual void ThreadEvent(int nEventNo) { return ; };				// スレッドイベント発生


//// メンバー関数
protected :
	//=========================================
	// 仮想関数
	// 今回追加
	virtual void		OnSetFileName(RpiHeader const& h, RPI_SETTING const& s, RPI_COIL_INFO const& c, CString* strName)=0;	// ファイル名決定
	virtual void		OnRpiFileSave(char const* strPath, CString* strName)=0;	// RPIファイル生成完了

	// 内部のみ
	void				CreateHeader(RpiHeader& h);					// ヘッダー情報生成
	void				CreateHeader_Ending(RpiHeader& h);			// ヘッダー情報更新(コイル確定時)
	bool				Open(CString sName, RpiHeader* h);			// ファイルの生成
	bool				Next();											// 1コイル内でのファイル切り替え


//// メンバー変数
protected :
	RPI_SETTING			my_Setting;										// 設定情報 (共通設定)
	RPI_COIL_INFO		my_CoilInfo;									// 設定情報 (コイル単位設定)
	RPI_FRAME_INFO		my_FrameInfo;									// 設定情報 (コイル単位設定)



	CString				my_sFullPath;									// 書込み中のRPIファイル名
	CString				my_sFileName;									// 書込み中のRPIファイル名
	int					my_nWriteFrameCnt;								// コイル単位でフレーム書込み数
	int					my_nSaveFolderIndex;							// 保存フォルダIndex (0オリジン)

	RpiLib::TRpiWriter *my_pWriter;
	//// 書込み速度検証
	KizuPerformanceCounter	mycls_Time;									// 書込み時間
	double				my_TimeWrite[3];								// 書込み速度[Mbyte/sec] (平均、最低、最高)
};
