'*******************************************************************************
'   表示共通定義
'
'   [Ver]
'       Ver.01  2015/06/04  初版作成
'
'   [メモ]
'
'*******************************************************************************
Option Strict On

''' <summary>
''' 表示共通定義
''' </summary>
''' <remarks></remarks>
Public Module DspCommon
	'///////////////////////////////////////////////////////////////////////////
	'スイッチ
	'///////////////////////////////////////////////////////////////////////////
	'-------------------------------------------------------
	'横展開用ライン区分
	'-------------------------------------------------------
	'LINEスイッチ(必ず1コのみ有効にする事)VBの場合モジュール毎に必要
#Const cLINE = "LINE_5CGL_TINY"

	'///////////////////////////////////////////////////////////////////////////
	'定数
	'///////////////////////////////////////////////////////////////////////////
	'-------------------------------------------------------
	'画面共通タイトル
	'-------------------------------------------------------
	Public Const SYSTEM_TITLE As String = "5CGL極微小φω"

	'-------------------------------------------------------
	'疵マップ表示色
	'-------------------------------------------------------
	Public g_ColorTanshoNon As Color = Color.DarkGray							'未探傷
	Public g_ColorTanshoOk As Color = Color.White								'正常探傷
	Public g_ColorTanshoGai As Color = Color.FromKnownColor(KnownColor.Control)	'探傷範囲外
	Public g_ColorJyusinOkure As Color = Color.Pink								'受信遅れ
	Public g_ColorOmoKizu As Color = Color.Cyan									'表面の疵
	Public g_ColorUraKizu As Color = Color.Lime									'裏面の疵
	Public g_ColorNewKizu As Color = Color.Cyan									'最新の疵
	Public g_ColorOldKizu As Color = Color.Lime									'一つ前の疵
	Public g_ColorOverlay As Color = Color.Cyan									'オーバーレイ色
	Public g_ColorMemori As Color = Color.Lime									'メモリ色
	'一時停止用
	Public g_ColorStop1st As Color = Color.Cyan									'1個目の疵
	Public g_ColorStop2nd As Color = Color.Lime									'2個目の疵

	Public g_ColorYousetu As Color = Color.Blue									'溶接線
	Public g_ColorSharStart As Color = Color.Red								'ファーストカット線
	Public g_ColorSharEnd As Color = Color.Red									'ラストカット線
	Public g_ColorShar As Color = Color.Red										'分割カット線
    Public g_ColorKensa As Color = Color.HotPink                                '検査台
    Public g_ColorIsyuten As Color = Color.Blue                                 '異種点
    'Public g_ColorIrigawa1 As Color = Color.FromArgb(255, 255, 255)			'入側コイル１
    'Public g_ColorIrigawa2 As Color = Color.FromArgb(255, 255, 255)			'入側コイル２
    'Public g_ColorIrigawa3 As Color = Color.FromArgb(255, 255, 255)			'入側コイル３

    '-------------------------------------------------------
    '重欠陥一覧表示色
    '-------------------------------------------------------
    Public g_ColLstOverlay As Color = Color.DarkGreen                           'オーバーレイ、目盛り表示色
    Public g_ColLstKensadaiMae As Color = Color.Cyan                            '検査台前
    Public g_ColLstNewKizu As Color = Color.Cyan                                '最新画像フレーム色
    Public g_ColLstNewSp As Color = Color.BlueViolet                            '最新+強調画像フレーム色
    Public g_ColLstFix As Color = Color.Orange                                  '固定画像フレーム色(ロック)
    Public g_ColLstSp As Color = Color.Red                                      '強調枠表示フレーム色

    '-------------------------------------------------------
    '欠陥グレード表示色
    '-------------------------------------------------------
    Public g_ColorGrNon As Color = Color.White                                  '無害
    Public g_ColorGrKei As Color = Color.Orange									'軽欠陥
	Public g_ColorGrTyu As Color = Color.Magenta								'中欠陥
	Public g_ColorGrJyu As Color = Color.Red									'重欠陥

	'// 2023.09.14 画像処理装置 H・Sソフト改造 -------------------->>>>>
	'-------------------------------------------------------
	'疵情報関連
	'-------------------------------------------------------
	Public g_ColorTypeFMerge As Color = Color.Red                           ' フレーム間連結疵
	'// 2023.09.14 画像処理装置 H・Sソフト改造 <<<<<--------------------

	'-------------------------------------------------------
	'RAS画面パスワード
	'-------------------------------------------------------
	Public Const PASSWORD As String = "KIZU"

    Public g_ColorSimpleCycle As Color = Color.Yellow                           ' 簡易周期疵
    Public g_ColorKizuCycle As Color = Color.Red                                ' 周期疵

    '-------------------------------------------------------
    '探傷画面用
    '-------------------------------------------------------
    Public Const DSP_COIL_MAX As Integer = 5                                    '探傷画面表示コイルMAX(現在コイル＋過去4コイル表示可能)
    Public Const DSP_IMAGE_MAX As Integer = 2									'疵画像表示個数(1疵あたり画像が2つ)
	Public Const DSP_DEFECT_MAX As Integer = 2                                  '疵情報表示個数[次、次々]
    Public Const DSP_KIRI_IMAGE_MAX As Integer = 2                              '拡大画像表示可能疵個数
    Public Const DSP_TRACKING_POS As Integer = 2                                'トラッキング表示開始検査位置(今回は全てリアルタイム表示)

    Public Const TANSHO_MAP_DSP_ROW_MAX As Integer = 39                         '探傷画面グリッド最大行数
    '   Public Const TANSHO_SCALE_INTVAL_X As Integer = 30							'疵画像のスケールのX軸メモリ間隔[mm]
    'Public Const TANSHO_SCALE_INTVAL_Y As Integer = 30                          '疵画像のスケールのY軸メモリ間隔[mm]
    Public Const TANSHO_SCALE_INTVAL_X As Integer = 10                          '疵画像のスケールのX軸メモリ間隔[mm]
    Public Const TANSHO_SCALE_INTVAL_Y As Integer = 10                          '疵画像のスケールのY軸メモリ間隔[mm]
    Public Const TANSHO_SCALE_NUM_X As Integer = 2                              '疵画像のスケールのX軸メモリ数
    Public Const TANSHO_SCALE_NUM_Y As Integer = 2                              '疵画像のスケールのY軸メモリ数

    '-------------------------------------------------------
    '実績画面用
    '-------------------------------------------------------
    Public Const JISSEKI_MAP_DSP_ROW_MAX As Integer = 41                        '実績画面グリッド最大行数
    '   Public Const JISSEKI_SCALE_INTVAL_X As Integer = 30							'疵画像のスケールのX軸メモリ間隔[mm]
    'Public Const JISSEKI_SCALE_INTVAL_Y As Integer = 30                         '疵画像のスケールのY軸メモリ間隔[mm]
    'Public Const JISSEKI_SCALE_NUM_X As Integer = 2                             '疵画像のスケールのX軸メモリ数
    'Public Const JISSEKI_SCALE_NUM_Y As Integer = 2                             '疵画像のスケールのY軸メモリ数
    Public Const JISSEKI_SCALE_INTVAL_X As Integer = 10                         '疵画像のスケールのX軸メモリ間隔[mm]
    Public Const JISSEKI_SCALE_INTVAL_Y As Integer = 10                         '疵画像のスケールのY軸メモリ間隔[mm]
    Public Const JISSEKI_SCALE_NUM_X As Integer = 3                             '疵画像のスケールのX軸メモリ数
    Public Const JISSEKI_SCALE_NUM_Y As Integer = 3                             '疵画像のスケールのY軸メモリ数
    Public Const JISSEKI_MAP_START_POS As Integer = 1                           'マップ入側、出側開始位置0:0～[0,5,10･･･]、1:最小～[5,10,15･･･]
    Public Const JISSEKI_DSP_KIRI_IMAGE_MAX As Integer = 4                      '実績疵拡大画像表示個数

    '-------------------------------------------------------
    '探傷・実績画面共通
    '-------------------------------------------------------
    Public Const KZCNT_CNT_MAX As Integer = 99                                  '疵数表示 最大値

    '-------------------------------------------------------
    '探傷・実績画面共通(リサイズ)
    '-------------------------------------------------------
    Public Const WIN_WIDTH_MAX As Integer = 1920                                '画面最大幅
    Public Const WIN_HEIGHT_MAX As Integer = 1080                               '画面最大高さ
    Public Const WIN_WIDTH_MIN As Integer = 800                                 '画面最小幅
    Public Const WIN_HEIGHT_MIN As Integer = 600                                '画面最小高さ

    '//=======================================
    '// 探傷画面・実績画面 画像名称　
    '//=======================================
    '全角1文字 or 半角２文字
    Public Const DIRECTION_NAME_DS As String = "DS"
    Public Const DIRECTION_NAME_WS As String = "WS"

    '// 画像名称
    Public DEFECT_IMG_NAME() As String = {"正反射(R)", "乱反射(G)", "乱反射(B)", "乱反射(G-B)"}

    '-------------------------------------------------------
    'パラメータ設定画面、疵一覧表示画面共通(システム毎に変更)
    '-------------------------------------------------------
    Public Const MAP_ROW_HIGHT As Integer = 19
    Public Const HED_ROW_HIGHT As Integer = 16

    '設定切替定数
    Public Const MC_SHOUSAI As Integer = 0										'詳細設定
	Public Const MC_KANI As Integer = 1                                         '簡易設定

    '簡易設定時の項目数
    Public Const MC_KIZUSHU_NUM As Integer = 8                                  '疵種は5個まで
    Public Const MC_GRADE_NUM As Integer = 1									'グレードは1個まで

	'///////////////////////////////////////////////////////////////////////////
	'構造体
	'///////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' 幅方向分布
	''' </summary>
	Public Enum EM_DIST_WIDTH
		WS = 0								' WS位置に欠陥有り
		C									' C位置に欠陥有り
		DS									' DS位置に欠陥有り
	End Enum

	''' <summary>
	''' 欠陥フラグ
	''' </summary>
	Public Enum EM_DEFECT_FLG
		KEI = 0								' 軽欠陥
		JYU									' 重欠陥
	End Enum

	'///////////////////////////////////////////////////////////////////////////
	'列挙体
	'///////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 疵マップ列
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_MAP_COL_INF
		DEGAWA = 0							'出側距離
		COL_1								'列1
		COL_2								'列2
		COL_3								'列3
		COL_4								'列4
		COL_5								'列5
		COL_6								'列6
		COL_7								'列7
		COL_8								'列8
		IRIGAWA                             '入側距離
        KIZUNUM                             '疵個数
        OTH1                                'その他１
        OTH2                                'その他２
    End Enum

    ''' <summary>
    ''' 探傷画面同期モード
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_SYNC_MODE
		ALL = 0								'全画面同期
		BY_DSP								'検査台位置毎に同期
		NONE								'同期しない
	End Enum

	''' <summary>
	''' 探傷画面疵表示更新モード
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_DEFECT_MODE
		TRACKING = 0						'トラッキング表示
		REALTIME							'リアルタイム表示
	End Enum

	''' <summary>
	''' 探傷画面更新モード
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_REFRESH_MODE
		AUTO = 0							'自働更新モード
		PAUSE								'一時停止モード
	End Enum

	'''<summary>
	'''長手間隔
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_LONG_POS
		MOSTSMALL = 0						'最小
		SMALL								'長手間隔小
		BIG									'長手間隔大
	End Enum

	'''<summary>
	'''疵マップ表示設定
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_MAP_SIDE
		NORMAL = 0							'左がDS
		REVERSE								'左がWS
	End Enum

	'// 2023.11.17 最終工程検査装置高機能化その２ -------------------->>>>>
	'''<summary>
	'''疵マップ表示設定
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_MAP_WIDTH_NO
		NORMAL = 0                          '左から1,2,・・・
		REVERSE                             '右から1,2,・・・
	End Enum
	'// 2023.11.17 最終工程検査装置高機能化その２ <<<<<--------------------

	'''<summary>
	'''疵マップセル設定
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_MAP_CELL
		NORMAL = 0							'左右反転無し
		REVERSE								'左右反転有り
	End Enum

	'''<summary>
	'''疵マップ進行方向設定
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_MAP_SCROOL
		TAIL_TO_HEAD = 0					'下から上へ
		HEAD_TO_TAIL						'上から下へ
	End Enum

	'''<summary>
	'''画像上下反転設定
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_IMAGE_TOP
		NORMAL = 0							'上下反転有り
		REVERSE								'上下反転無し
	End Enum

	'''<summary>
	'''画像左右反転設定
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_IMAGE_SIDE
		NORMAL = 0							'左右反転有り
		REVERSE								'左右反転無し
	End Enum
	'''<summary>
	'''画像90度回転
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_IMAGE_90
		NORMAL = 0							'回転なし
		LOTATE								'90度回転
	End Enum

	'''<summary>
	'''画像位置
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_IMAGE_POS
		IMG1 = 0							'上
		IMG2 = 1							'下
	End Enum

	'''<summary>
	'''画像番号
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_IMAGE_NO
		S = 1								'正反射
		R1									'乱反射１
		R2									'乱反射２
		R3									'乱反射３(色差)
	End Enum

	''' <summary>
	''' 起動引数
	''' </summary>
	''' <remarks></remarks>
	Public Structure START_ARGS
		Dim nFormPos As Integer         ' 画面位置番号 (FORM_POS:=)

		Dim sArgs() As String           ' 引数無加工
	End Structure

	Public Const START_FORM_POS As String = "FORM_POS:="

	'''<summary>
	'''前工程疵自動表示あり
	'''</summary>
	'''<remarks>新規立上げ時は未使用</remarks>
	Public Enum EM_MAEIMAGE_AUTOPOP
		DISABLE = 0							'自動表示なし
		ENABLE								'自動表示あり
	End Enum

    ''-------------------------------------------------------
    ''機器情報(RAS画面で使用)
    ''システムやラインによって変更すること 
    ''-------------------------------------------------------
    '操業状態表示欄(STATUS_MASTER 対象IDNo)
    Public Const DIV_KADOU_ID As Integer = 663              '稼動状態
    Public Const DIV_UNTEN_ID As Integer = 664              '運転モード
    Public Const DIV_SOGYO_ID As Integer = 665              '操業状態
    Public Const DIV_PLGMODE_ID As Integer = 666            'PLG区分

    '立入制限(STATUS_MASTER 対象IDNo)
    Public Const DIV_SECURITY_DOOR_ID As Integer = 705      '扉状態
    Public Const DIV_SECURITY_KEY_ID As Integer = 706       '鍵状態

End Module
