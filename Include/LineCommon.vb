'*********************************************************************************
' VB用全φωで共通のインクルード
'	[Ver]
'					2016/06/15	初版作成
'
'	[メモ]
'		基本的に、書き換え可
'*********************************************************************************
Option Strict On

Imports System.Runtime.InteropServices

''' <summary>
''' ライン毎定義
''' </summary>
''' <remarks></remarks>
Public Module LineCommon
    '///////////////////////////////////////////////////////////////////////////
    'スイッチ
    '///////////////////////////////////////////////////////////////////////////
    '-------------------------------------------------------
    '動作環境
    '-------------------------------------------------------
    'False:本番環境、True:テスト環境
#Const cLOCAL = False

    '-------------------------------------------------------
    '横展開用ライン区分
    '-------------------------------------------------------
    'LINEスイッチ(必ず1コのみコントアウトする事)VBの場合モジュール毎に必要
#Const cLINE = "LINE_5CGL_TINY"

    '///////////////////////////////////////////////////////////////////////////
    '定数
    '///////////////////////////////////////////////////////////////////////////
    '-------------------------------------------------------
    'ライン情報
    '-------------------------------------------------------
#If cLOCAL = False Then                                     '本番環境
    Public Const COIL_LEN_MAX As Integer = 10000                                '入側コイル長MAX[m](どの項目でもこの長さ未満となる)
#Else                                                       'テスト環境
	Public Const COIL_LEN_MAX As Integer = 10000								'入側コイル長MAX[m](どの項目でもこの長さ未満となる)
#End If
    Public Const COIL_LEN_MIN As Integer = 200									'入側コイル長MIN[m](よく言う小コイル)
	Public Const COIL_WID_MAX As Integer = 1750									'コイル幅MAX[mm](どの項目でもこの幅未満となる)
	'Public Const COIL_THICK_MAX As Integer = 1600								'材料板厚MAX[μm]

	'-------------------------------------------------------
	'装置
	'-------------------------------------------------------
	Public Const NUM_MEN As Integer = 2											'装置 面単位(この単位で、検査停止が可能)※表/裏じゃない事に注意
	Public Const NUM_MAX_HANTEI As Integer = 7									'片面 判定PCの数(オフライン機も含めた台数)
	Public Const NUM_MAX_CYCLE As Integer = 1									'片面 周期PCの数 (オフライン機も含めた台数) ※ 0だとコンパイルが通らない所があるので、0でも1にしておく
#If cLOCAL = False Then										'本番環境
	Public Const NUM_HANTEI As Integer = 5										'NUM_CAM_POS // 片面 判定PCの数(片面に繋がる判定PCの数)(オンライン機のみ)
	Public Const NUM_HANTEI_CYCLE As Integer = 1								'片面 判定PCの数(片面に繋がる判定PCの数)(オンライン機のみ)
	Public Const NUM_HANTEI_PARA As Integer = 2									'ロジックパラランPCの数
	Public Const NUM_CAMPAIR As Integer = 10									'MAX_CAMSET // カメラペア数(カメラペアとは、複数のカメラセットが１判定PC上で動作する場合のカメラセットの塊の個数)(今回は、1対1の為、8個)
	Public Const NUM_CAMPAIR_CYCLE As Integer = 1								'カメラペア数(判定PC１台で動作するカメラセット)(今回は、1対4)
#Else														'テスト環境
	Public Const NUM_HANTEI As Integer = 5										'片面 判定PCの数(片面に繋がる判定PCの数)(オンライン機のみ)
	Public Const NUM_HANTEI_CYCLE As Integer = 1								'片面 判定PCの数(片面に繋がる判定PCの数)(オンライン機のみ)
	Public Const NUM_HANTEI_PARA As Integer = 2									'ロジックパラランPCの数
	Public Const NUM_CAMPAIR As Integer = 10									'カメラペア数(カメラペアとは、複数のカメラセットが１判定PC上で動作する場合のカメラセットの塊の個数)(今回は、1対1の為、8個)
	Public Const NUM_CAMPAIR_CYCLE As Integer = 1								'カメラペア数(判定PC１台で動作するカメラセット)(今回は、1対4)
#End If
	Public Const NUM_ALL_HTBACK As Integer = 4									'全面の判定バックアップPCの台数
	Public Const NUM_ALL_HTPARA As Integer = 2									'全面の判定ロジックパラランPCの台数
	Public Const NUM_ALL_HANTEI As Integer = (NUM_HANTEI*NUM_MEN) + NUM_ALL_HTBACK + NUM_ALL_HTPARA         '全面の判定PCの台数
    Public Const NUM_CAMERA As Integer = (MAX_CAMSET * MAX_PHYSICAL_CAMANGLE)   '全面のカメラ台数(カメラセット × カメラ角度)
    Public Const MAX_IMAGE_CNT As Integer = 4									'画像枚数(式差まではMAX_CAMANGLEとMAX_IMAGE_CNTは同義だったが、式差では、カメラ１台で複数画像が取れる対策)

	Public Const NUM_LIGHT As Integer = 2										'片面 照明装置の数
	Public Const NUM_LED_COLOR As Integer = 3                                   '照明装置の色数（RGB）
    'Public Const NUM_LED_BLOCK As Integer = 36                                  '画面設定可能な配光ブロック数(iniファイルは実際のブロック数)(MAX256)
    Public Const NUM_LED_BLOCK As Integer = 48                                  '画面設定可能な配光ブロック数(iniファイルは実際のブロック数)(MAX256)

    Public Const NUM_CAMLINKSW As Integer = 4									'カメラリンクスイッチ台数
	Public Const NUM_CAMLINK_BOARD As Integer = 2                               'PC1台当たりの最大入力数 (BOARDになっているが、実際は取り込み口数。)

    '-------------------------------------------------------
    'システム情報
    '-------------------------------------------------------
    '// 20200520 疵保存個数上限拡張 --->>>
    Public Const VRCL_DEFECT_NO_START As Integer = 64001                        'VRCL新規疵の入力範囲の疵No(開始)
    Public Const VRCL_DEFECT_NO_END As Integer = 65000                          'VRCL新規疵の入力範囲の疵No(終了)
    Public Const VRCL_DEFECT_NUM As Integer = (VRCL_DEFECT_NO_END - VRCL_DEFECT_NO_START + 1)   'VRCL新規疵の入力疵個数
    'Public Const MAX_DETECT_SAVE As Integer = 32000								'1コイルあたりの保持する疵個数(表裏別)
    Public Const MAX_DETECT_SAVE As Integer = 524000 - CInt(VRCL_DEFECT_NUM / NUM_MEN) '1コイルあたりの保持する疵個数(表裏別)
    '// 20200520 疵保存個数上限拡張 ---<<<
    Public Const MAX_DETECT_LOWGR_SAVE As Integer = 25000                       '1コイルあたりの保持する無害Gr疵個数(表裏別)
    'Public Const MAX_DETECT_IMAGE_SAVE As Integer = 32000                       '1コイルあたりの保持する疵画像(表裏別)
    'Public Const MAX_DEFECT_LOWER_IMG_SAVE As Integer = 7000					'1コイルあたりの保持する無害疵画像（表裏別）
    Public Const MAX_MAP_DEFECT_NUM As Integer = 200000                         '1コイルあたりの表示疵個数(昔は、列数×行数×面でよかったが、深さが加わったため、最大件数を決めるようにする)


    Public Const MAP_REPRES_MIN As Integer = 5									'最小代表疵間隔[m]
	Public Const MAP_COL_INTERVAL As Integer = 200								'マップ列1列あたりの幅[mm] ※CTL未使用
	Public Const MAP_COL_NUM As Integer = 8										'マップ最大列数
	Public Const MAP_ROW_NUM As Integer = CInt(COIL_LEN_MAX / MAP_REPRES_MIN)	'マップ最大行数(コイル最大長[m]÷マップ最小間隔[m])
	Public Const MAP_STAGE_NUM As Integer = 3									'マップの切り替え段数(0:最小間隔 1:実際の表示エリア小 2:実際の表示エリア大)
	Public Const MAP_NEST_NUM As Integer = 5									'マップのセル内のリスト件数
	Public Const MAP_CYCLE_COL_NUM As Integer = 1								'周期疵マップ列数
	Public Const ALARM_COL_NUM As Integer = 8									'疵接近ランプ最大列数(過去の互換性用 将来削除予定)

	Public Const SIZE_FRAME_IMG_X As Integer = 4096								'横フレーム画像サイズ[pixel]
	Public Const SIZE_FRAME_IMG_Y As Integer = 4096								'縦フレーム画像サイズ[pixel]

	'-------------------------------------------------------
	'周期条件
	'-------------------------------------------------------
	'Public Const MAP_CYCLE_COLNUM As Integer = 1								'周期疵マップ列数
    'Public Const STARTINDEX_CYCLE_PCID As Integer = (NUM_ALL_HANTEI + 1)		'周期PCのPCIDの先頭位置
	'Public Const GET_CYCLE_PCID(pcid) As Integer = (pcid - STARTINDEX_CYCLE_PCID + 1)	'周期PCのPCIDから、1始まりの連番を算出する。
	'Public Const GET_CYCLE_PCKIND(pckind) As Integer = (pckind - PCNO_CYCLE)			'周期PCのPCKINDから、1始まりの連番を算出する。


	'-------------------------------------------------------
	'カメラ左側がDS/WS対応
	'-------------------------------------------------------
	Public Const CAMLEFT_IS_DS_T As Integer = 0									'0:カメラ左側がDS  1:カメラ右側がWS(表面)
	Public Const CAMLEFT_IS_DS_B As Integer = 0									'0:カメラ左側がDS  1:カメラ右側がWS(裏面)

	'-------------------------------------------------------
	'全長保存
	'-------------------------------------------------------
	Public Const NUM_JOIN_FRAME As Integer = 1500								'現フレームから途中参加させるフレーム数(現フレーム＋α)

	'-------------------------------------------------------
	'その他(各システム固有)
	'-------------------------------------------------------
    Public Const SIZE_COIL_KEY As Integer = 20                                  'コイル特定するためのキー情報(今回は、命令No＋コイルNo)
    Public Const SIZE_COIL_NO As Integer = PRO_SIZE_COIL_NO                     'コイルNoサイズ
    Public Const SIZE_MEI_NO As Integer = 4                                     '命令Noサイズ(コンパイルが通るように最少１とする)
    'Public Const MAX_CAMERA_GAP_WIDDIV As Integer = 10                          '複眼ズレ補正 幅方向分割数(=最大グループ数)

    Public Const MAX_SHACUT_COUNT As Integer = 10                               '最大カット管理回数
    Public Const MAX_PROGRP_TYPE As Integer = 10                                'プロコンへ送信する疵種グループ（0：無し  1～：各対応するグループ）
    Public Const MAX_PROGRP_GRADE As Integer = 6                                'プロコンへ送信するグレードグループ（0：無し  1～：各対応するグループ）

    Public Const NUM_DSP_WPD As Integer = 1										'表示基点数
	Public Const BASE_DSP_MEN As Integer = 1									'表示基点の基準面（裏面）(表示機能への通板距離送信で使用)

	Public Const MAX_SHALINK_COUNT As Integer = 3								'最大出側合成管理回数

	'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
	'// 周期疵関係
	Public Const MAX_CYCLE As Integer = 16                                      ' 周期疵の検出設定数
	'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------

	'-------------------------------------------------------
	'前工程関係
	'-------------------------------------------------------
	Public Const MAX_MAE_LINE As Integer = 16                                   '前工程登録数
	Public Const MAX_MAE_TYPE As Integer = 64                                   '前工程疵種最大登録数
	Public Const MAX_MAE_GRADE As Integer = 10                                  '前工程グレード最大登録数
	Public Const MAX_MAE_IMAGE As Integer = 4                                   '前工程画像最大数
	Public Const MAE_MAP_COL_NUM As Integer = 1                                 '前工程MAP最大列数

	'-------------------------------------------------------
	'重欠陥一覧画面
	'-------------------------------------------------------
	Public Const MAX_DEFLIST_BUF As Integer = 30                                '重欠陥一覧画面用 バッファ件数

	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	'-------------------------------------------------------
	'長周期欠陥
	'-------------------------------------------------------
	Public Const MAX_LONGPD_FILTER As Integer = 16                              '長周期の最大フィルタ数
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	'-------------------------------------------------------
	'表示ソケット
	'-------------------------------------------------------
	Public Const HY_SOCK_NUM As Integer = 2                                     '表示ソケット数

	'-------------------------------------------------------
	'表示位置
	'-------------------------------------------------------
	Public Const MAX_DISP_POS As Integer = EM_DIV_DISP.DIV_DISP_POS_END         '表示基準位置 個数 ※共有メモリサイズに係るため、大きい方に合わせておく

	Public Const MAX_ALARME_POS As Integer = 2                                  '警報出力位置 個数 (0:表面検査台 1:裏面検査台)

	'-------------------------------------------------------
	'疵マップ表示画面(散布図)用の疵シンボル定義
	'0：・,1:●,2:■,3:▲,4:▼,5:◆,6:★,7:＊,8:×,9:＋
	'-------------------------------------------------------
	'疵マップ表示画面(散布図)用の疵シンボル定義
	Public PLOT_SYMBOL_TYPE() As String = {
		"・", "●", "■", "▲", "▼", "◆", "★", "＊", "×", "＋", "◎", "○", "□", "△", "▽", "◇", "☆", "〓", "≡"
	}

	Public CUT_RESULT_ID_NO() As String = {
		"①", "②", "③", "④", "⑤", "⑥", "⑦", "⑧", "⑨", "⑩", "⑪", "⑫", "⑬", "⑭", "⑮", "⑯"
	}

	'//=======================================
	'// データベース
	'//=======================================
	'コイル
	Public Const DB_COIL_INF As String = "T_COIL_INF"                           '[DB]コイル情報
	Public Const DB_COIL_RESULT As String = "T_COIL_RESULT"                     '[DB]コイル実績
	'Public Const DB_COIL_OUTINF_R As String = "T_COIL_OUTINF_R"                 '[DB]出側物理コイル実績
	Public Const DB_COIL_POSITION As String = "T_COIL_POSITION"                 '[DB]コイル位置情報
	Public Const DB_COIL_POSITION_OUT As String = "T_COIL_POSITION_OUT"         '[DB]出側コイル位置情報
	Public Const DB_COIL_POSITION_ANA As String = "T_COIL_POSITION_ANA"         '[DB]穴検コイル位置情報
	Public Const DB_COIL_TRAND As String = "T_COIL_TRAND"                       '[DB]コイルトレンド情報
	Public Const DB_COIL_CUT_POS As String = "T_COIL_CUT_POS"                   '[DB]カット位置テーブル
	Public Const DB_COIL_SAISUN_RESULT As String = "T_COIL_SAISUN_RESULT"       '[DB]採寸実績テーブル
	Public Const DB_COIL_ORDER As String = "T_COIL_ORDER"                       '[DB]コイル通板順位情報

	'表疵
	Public Const DB_DEFECT_INF_T As String = "T_DEFECT_INF_T"                   '[DB]表疵情報
	Public Const DB_DEFECT_IMG_T As String = "T_DEFECT_IMG_T"                   '[DB]表疵画像
	Public Const DB_DEFECT_ATTR0_T As String = "T_DEFECT_ATTR0_T"               '[DB]表基本特徴量
	Public Const DB_DEFECT_ATTR1_T As String = "T_DEFECT_ATTR1_T"               '[DB]表カメラ角度1特徴量
	Public Const DB_DEFECT_ATTR2_T As String = "T_DEFECT_ATTR2_T"               '[DB]表カメラ角度2特徴量
	Public Const DB_DEFECT_ATTR3_T As String = "T_DEFECT_ATTR3_T"               '[DB]表カメラ角度3特徴量
	Public Const DB_DEFECT_ATTR4_T As String = "T_DEFECT_ATTR4_T"               '[DB]表カメラ角度3特徴量

	'裏疵
	Public Const DB_DEFECT_INF_B As String = "T_DEFECT_INF_B"                   '[DB]裏疵情報
	Public Const DB_DEFECT_IMG_B As String = "T_DEFECT_IMG_B"                   '[DB]裏疵画像
	Public Const DB_DEFECT_ATTR0_B As String = "T_DEFECT_ATTR0_B"               '[DB]裏基本特徴量
	Public Const DB_DEFECT_ATTR1_B As String = "T_DEFECT_ATTR1_B"               '[DB]裏カメラ角度1特徴量
	Public Const DB_DEFECT_ATTR2_B As String = "T_DEFECT_ATTR2_B"               '[DB]裏カメラ角度2特徴量
	Public Const DB_DEFECT_ATTR3_B As String = "T_DEFECT_ATTR3_B"               '[DB]裏カメラ角度3特徴量
	Public Const DB_DEFECT_ATTR4_B As String = "T_DEFECT_ATTR4_B"               '[DB]裏カメラ角度3特徴量

	'エッジ検出強化
	Public Const DB_EDGE_COIL_INF As String = "T_EDGE_COIL_INF"                 '[DB]エッジ検出実績
	Public Const DB_EDGE_CHANGE_INF As String = "T_EDGE_CHANGE_INF"             '[DB]エッジ検出モード変化管理

	'次工程
	Public Const DB_ACCESS_NEXT_COIL_INF As String = "T_ACCESS_NEXT_COIL_INF"   '[DB] 次工程コイル情報
	Public Const DB_ACCESS_NEXT_DEFECT_INF As String = "T_ACCESS_NEXT_DEFECT_INF"   '[DB] 次工程疵情報

	'送受信伝文バッファ
	Public Const DB_BUF_PRO_R_COILINF As String = "BUF_PRO_R_COILINF"           '[DB]プロコンコイル情報受信バッファ
	Public Const DB_BUF_PRO_R_SAISUN As String = "BUF_PRO_R_SAISUN"             '[DB]プロコン採寸実績受信バッファ
	Public Const DB_BUF_PRO_R_CUT As String = "BUF_PRO_R_CUT"                   '[DB]プロコンカット情報受信バッファ
	Public Const DB_BUF_PRO_S_ZISEKI As String = "BUF_PRO_S_ZISEKI"             '[DB]プロコン検査実績送信伝文バッファ
	Public Const DB_BUF_FF_S_ZISEKI As String = "BUF_FF_S_ZISEKI"               '[DB]FF検査実績送信バッファ
	Public Const DB_BUF_FF_R_MAEINF As String = "BUF_FF_R_MAEINF"               '[DB]FF前工程疵情報受信バッファ

	'// 前工程用
	Public Const DB_MAE_MASTER As String = "M_MAE_MASTER"                       '[DB]前工程マスター
	Public Const DB_MAE_TYPE_NAME As String = "M_MAE_TYPE_NAME"                 '[DB]前工程疵種マスター
	Public Const DB_MAE_GRADE_NAME As String = "M_MAE_GRADE_NAME"               '[DB]前工程グレードマスター

	Public Const DB_MAE_COIL_INF As String = "T_MAE_COIL_INF"                   '[DB]前工程コイル情報
	Public Const DB_MAE_COIL_LINE As String = "T_MAE_COIL_LINE"                 '[DB]前工程ライン別コイル情報
	Public Const DB_MAE_FF_INF As String = "T_MAE_FF_INF"                       '[DB]前工程ＦＦ疵情報
	Public Const DB_MAE_DEFECT_INF As String = "T_MAE_DEFECT_INF"               '[DB]前工程疵情報
	Public Const DB_MAE_DEFECT_IMG As String = "T_MAE_DEFECT_IMG"               '[DB]前工程疵画像

	Public Const DB_DUMMY_DEFECT_INF As String = "T_DUMMY_DEFECT_INF"           '[DB]擬似疵情報 (ストップマーク (ダミー疵専用))

	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	'// 長周期用
	Public Const DB_LONGPD_COMMON As String = "M_LONGPD_COMMON"                             '[DB] 長周期欠陥検出_マスター設定テーブル
	Public Const DB_LONGPD_PCODE As String = "M_LONGPD_PCODE"                               '[DB] 長周期欠陥検出_検査パターンコード別設定テーブル
	Public Const DB_LONGPD_PCODE_FILTER As String = "M_LONGPD_PCODE_FILTER"                 '[DB] 長周期欠陥検出_検査パターン×フィルター設定テーブル
	Public Const DB_LONGPD_PCODE_FILTER_OUTPUT As String = "M_LONGPD_PCODE_FILTER_OUTPUT"   '[DB] 長周期欠陥_疵種設定テーブル

	Public Const DB_LONGPD_COIL_RESULT As String = "T_LONGPD_COIL_RESULT"                   '[DB] 長周期欠陥検出_実績テーブル
	Public Const DB_LONGPD_EXEC_RESULT_WIDTH As String = "T_LONGPD_EXEC_RESULT_WIDTH"       '[DB] 長周期欠陥検出_フィルター実績テーブル
	Public Const DB_LONGPD_EXEC_RESULT_DEFECT_T As String = "T_LONGPD_EXEC_RESULT_DEFECT_T" '[DB] 長周期欠陥_表疵情報テーブル
	Public Const DB_LONGPD_EXEC_RESULT_DEFECT_B As String = "T_LONGPD_EXEC_RESULT_DEFECT_B" '[DB] 長周期欠陥_裏疵情報テーブル
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	'-------------------------------------------------------
	'パラメータ変更区分(ライン固有部分)
	'   疵検共通画面の場合は、KizuCommonに記述すること
	'-------------------------------------------------------
	Public Const CH_PARAM_DEFLIST As Integer = 200                              '疵一覧
	Public Const CH_PARAM_SETTIG_ALARM As Integer = 201                         '疵検警報出力設定
	Public Const CH_PARAM_MAE_MAST As Integer = 209                             '前工程設定
	Public Const CH_PARAM_TAJUUTHRESHOLD As Integer = 210                       '多重分解能閾値設定
	Public Const CH_PARAM_REDUCTION As Integer = 211                            '縮小率設定
	Public Const CH_PARAM_FILTER As Integer = 212                               'フィルタ設定
	'Public Const CH_PARAM_CYCLETHRESHOLD As Integer = 213						'周期用分解能閾値設定
	Public Const CH_PARAM_PARA As Integer = 214                                 'ロジックパララン設定
	Public Const CH_PARAM_CROP_SHRINK As Integer = 215                          '間引き設定
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	Public Const CH_PARAM_LONGPD As Integer = 301                                '長周期欠陥検出
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	'-------------------------------------------------------
	'タスク名称
	'-------------------------------------------------------
	'共通タスク(若干ラインよりのやつ。LineCommonを使わないでよいのであれば、KizuCommonに定義)

	'統括機能
	Public Const TO_DEFECT As String = "TO_DEFECT"                              '統括管理
	Public Const TO_CAMERA As String = "TO_CAMERA"                              '対カメラコントローラ通信
	Public Const TO_LINKSW As String = "TO_LINKSW"                              '対カメラリンクスイッチ通信
	Public Const TO_SEQDM As String = "TO_SEQDM"                                '対シーケンサ データメモリ通信
	Public Const TO_GETDAT As String = "TO_GETDAT"                              '入力データ編集管理
	Public Const TO_PUTDAT As String = "TO_PUTDAT"                              '実績データ編集管理
	Public Const TO_MAEDAT As String = "TO_MAEDAT"                              '前工程実績編集処理
	Public Const TO_PROCON As String = "TO_PROCON"                              '対プロコン通信
	Public Const TO_FFSERV As String = "TO_FFSERV"                              '対FFサーバー通信
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	Public Const TO_CHKLONG As String = "TO_CHKLONG"                            '長周期欠陥監視
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	'判定機能
	Public Const HT_DEFECT As String = "HT_DEFECT"                              '判定管理

	'表示機能
	Public Const HY_DEFECT As String = "HY_DEFECT"                              '表示管理
	Public Const HY_TANSHO As String = "HY_TANSHO"                              '探傷画面
	Public Const HY_DEFLIST As String = "HY_DEFLIST"                            '疵一覧表示画面
	'Public Const HY_DEFMAP As String = "HY_DEFMAP"                              '疵マップ表示画面

	'操作機能
	Public Const SO_GAMENN As String = "SO_GAMENN"                              '操作画面
	Public Const SO_TRAND As String = "SO_TRAND"                                'トレンド画面
	Public Const SO_DEFMAP As String = "SO_DEFMAP"                              '入側単位 疵点マップ画面


	'解析機能
	Public Const KA_PARSET As String = "KA_PARSET"                              'パラメータ設定画面[システム用]
	Public Const KA_PARCOLOR As String = "KA_PARCOLOR"                          'パラメータ設定画面[エンジン用]


	'//***************************************************************
	'// 起動要因
	'//***************************************************************
	'+++++++++++++++++++++++++++++++++++
	'統括機能
	'+++++++++++++++++++++++++++++++++++
	'TO_DEFECT
	Public Const FACT_TO_DEFECT_01 As Integer = 2101        'パラメータ変更要求					(←KA_PARSET)
	Public Const FACT_TO_DEFECT_02 As Integer = 2102        '検査開始要求						(←SO_GAMENN)
	Public Const FACT_TO_DEFECT_03 As Integer = 2103        '検査停止要求						(←SO_GAMENN)
	Public Const FACT_TO_DEFECT_04 As Integer = 2104        '稼動状態取得要求					(←SO_GAMENN)
	'Public Const FACT_TO_DEFECT_05 As Integer = 2105		'※今回未使用
	Public Const FACT_TO_DEFECT_06 As Integer = 2106        '判定開始アンサー					(←HT_DEFECT)
	Public Const FACT_TO_DEFECT_07 As Integer = 2107        '判定停止アンサー					(←HT_DEFECT)
	'Public Const FACT_TO_DEFECT_08 As Integer = 2108		'判定パラメータ変更アンサー			(←HT_DEFECT)
	Public Const FACT_TO_DEFECT_09 As Integer = 2109        '判定状態問合せ報告					(←HT_DEFECT)
	'Public Const FACT_TO_DEFECT_10 As Integer = 2110		'フレーム画像取得失敗通知			(←HT_DEFECT)
	Public Const FACT_TO_DEFECT_11 As Integer = 2111        'コイル情報受信通知					(←TO_GETDAT)
	Public Const FACT_TO_DEFECT_12 As Integer = 2112        '前工程疵情報書込み完了				(←TO_MAEDAT)
	Public Const FACT_TO_DEFECT_13 As Integer = 2113        '実績書き込み状況問合せ				(←TO_PUTDAT)
	Public Const FACT_TO_DEFECT_14 As Integer = 2114        '現露光時間報告                     (←TO_CAMERA)
	Public Const FACT_TO_DEFECT_15 As Integer = 2115        'イニシャル伝文受信通知				(←TO_PROCON)
	'Public Const FACT_TO_DEFECT_16 As Integer = 2116       'カット範囲情報通知			    	(←TO_PUTDAT)
	'Public Const FACT_TO_DEFECT_17 As Integer = 2117		'※今回未使用
	'Public Const FACT_TO_DEFECT_18 As Integer = 2118		'※今回未使用
	'Public Const FACT_TO_DEFECT_19 As Integer = 2119		'※今回未使用
	Public Const FACT_TO_DEFECT_20 As Integer = 2120        'ボイス警報出力解除要求				(←HY_TANSHO)
	Public Const FACT_TO_DEFECT_21 As Integer = 2121        '途中開始要求					    (←SO_GAMENN)
	Public Const FACT_TO_DEFECT_22 As Integer = 2122        '途中停止要求					    (←SO_GAMENN)
	Public Const FACT_TO_DEFECT_23 As Integer = 2123        '途中開始アンサー                   (←HT_DEFECT)
	Public Const FACT_TO_DEFECT_24 As Integer = 2124        '途中停止アンサー                   (←HT_DEFECT)
	'FACT_TO_DEFECT_30以降はTO_LINKSWからの応答で予約
	Public Const FACT_TO_DEFECT_30 As Integer = 2130        'カメラリンクスイッチ設定完了通知	(←TO_LINKSW)
	Public Const FACT_TO_DEFECT_31 As Integer = 2131        '途中参加設定完了通知				(←TO_LINKSW)
	Public Const FACT_TO_DEFECT_32 As Integer = 2132        'カメラ選択設定完了通知             (←TO_LINKSW)

	'TO_CAMERA
	Public Const FACT_TO_CAMERA_01 As Integer = 2201        'パラメータ変更要求					(←TO_DEFECT)
	Public Const FACT_TO_CAMERA_02 As Integer = 2202        '検査用設定コマンド					(←TO_DEFECT)
	Public Const FACT_TO_CAMERA_03 As Integer = 2203        'パルス出力設定						(←TO_DEFECT)
	Public Const FACT_TO_CAMERA_04 As Integer = 2204        'AGC設定							(←TO_DEFECT)
	Public Const FACT_TO_CAMERA_05 As Integer = 2205        'コイル切替							(←TO_DEFECT)
	Public Const FACT_TO_CAMERA_06 As Integer = 2206        '現露光時間問い合わせ				(←TO_DEFECT)
	Public Const FACT_TO_CAMERA_07 As Integer = 2207        '校正用設定コマンド					(←TO_DEFECT)

	'TO_LINKSW
	Public Const FACT_TO_LINKSW_01 As Integer = 2301        'パラメータ変更通知					(←TO_DEFECT)
	Public Const FACT_TO_LINKSW_02 As Integer = 2302        '検査用設定コマンド					(←TO_DEFECT)
	Public Const FACT_TO_LINKSW_03 As Integer = 2303        'カメラ選択設定コマンド				(←TO_DEFECT)
	Public Const FACT_TO_LINKSW_04 As Integer = 2304        '途中参加設定コマンド				(←TO_DEFECT)
	Public Const FACT_TO_LINKSW_05 As Integer = 2305        '高速リセット通知					(←TO_DEFECT)
	Public Const FACT_TO_LINKSW_06 As Integer = 2306        'AOC速度変更指示					(←TO_LINKSW)

	'KT_LEDMGR
	Public Const FACT_KT_LEDMGR_01 As Integer = 2401        'パラメータ変更通知					(←TO_DEFECT)
	Public Const FACT_KT_LEDMGR_02 As Integer = 2402        '設定要求 （検査表面状態指定）		(←TO_DEFECT)
	Public Const FACT_KT_LEDMGR_03 As Integer = 2403        '設定要求 （光量指定）				(←TO_DEFECT)
	Public Const FACT_KT_LEDMGR_04 As Integer = 2404        '点灯・消灯指示
	Public Const FACT_KT_LEDMGR_05 As Integer = 2405        '累積点灯時間リセット				(←SO_GAMENN)

	'TO_SEQDM
	'Public Const FACT_TO_SEQDM_01 As Integer  = 2401		'※今回未使用

	'TO_GETDAT
	'Public Const FACT_TO_GETDAT_01 As Integer = 2501		'パラメータ変更要求					(←TO_DEFECT)
	Public Const FACT_TO_GETDAT_02 As Integer = 2502        'コイル情報受信通知					(←TO_SEQDM)

	'TO_PUTDAT
	'Public Const FACT_TO_PUTDAT_01 As Integer = 2601		'パラメータ変更要求					(←TO_DEFECT)
	Public Const FACT_TO_PUTDAT_02 As Integer = 2602        '採寸実績伝文受信通知				(←TO_PROCON)
	Public Const FACT_TO_PUTDAT_03 As Integer = 2603        'カット実績伝文受信通知				(←TO_PROCON)
	Public Const FACT_TO_PUTDAT_04 As Integer = 2604        '実績書き込み完了通知				(←TO_DEFECT)

	'TO_MAEDAT
	'Public Const FACT_TO_MAEDAT_01 As Integer = 2701		'パラメータ変更要求					(←TO_DEFECT)
	Public Const FACT_TO_MAEDAT_02 As Integer = 2702        '検査用コイル情報受信完了通知		(←TO_DEFECT)
	Public Const FACT_TO_MAEDAT_03 As Integer = 2703        '前工程疵情報受信完了通知			(←TO_FFSERV)

	'TO_PROCON
	Public Const FACT_TO_PROCON_01 As Integer = 2801        'プロコン検査実績送信要求			(←TO_PUTDAT)

	'TO_FFSERV
	Public Const FACT_TO_FFSERV_01 As Integer = 2901        '検査実績伝文送信要求				(←TO_PUTDAT)
	Public Const FACT_TO_FFSERV_02 As Integer = 2902        '前工程疵情報要求通知				(←TO_MAEDAT)



	'+++++++++++++++++++++++++++++++++++
	'判定機能
	'+++++++++++++++++++++++++++++++++++
	'HT_DEFECT
	Public Const FACT_HT_DEFECT_01 As Integer = 3101        'パラメータ変更通知					(←TO_DEFECT)
	Public Const FACT_HT_DEFECT_02 As Integer = 3102        '検査開始要求						(←TO_DEFECT)
	Public Const FACT_HT_DEFECT_03 As Integer = 3103        '検査停止要求						(←TO_DEFECT)
	Public Const FACT_HT_DEFECT_04 As Integer = 3104        'Jpeg圧縮率設定						(←TO_DEFECT)
	Public Const FACT_HT_DEFECT_05 As Integer = 3105        '上位情報設定要求					(←TO_DEFECT)
	'Public Const FACT_HT_DEFECT_06 As Integer = 3106		'遅延実行キャンセル					(←TO_DEFECT)
	Public Const FACT_HT_DEFECT_07 As Integer = 3107        '続行不可能 解除					(←TO_DEFECT)
	Public Const FACT_HT_DEFECT_08 As Integer = 3108        '状態問い合わせ						(←TO_DEFECT)
	'Public Const FACT_HT_DEFECT_09 As Integer = 3109		'フレーム画像取得要求				(←TO_DEFECT)
	'Public Const FACT_HT_DEFECT_10 As Integer = 3110		'エッジ検出サーチ範囲変更依頼		(←TO_DEFECT)
	'Public Const FACT_HT_DEFECT_11 As Integer = 3111		'健全性評価データ収集要求			(←TO_DEFECT)
	'Public Const FACT_HT_DEFECT_12 As Integer = 3112		'画像レコーディング要求				(←TO_DEFECT)
	'Public Const FACT_HT_DEFECT_13 As Integer = 3113		'判定AGC設定						(←TO_DEFECT)
	'Public Const FACT_HT_DEFECT_14 As Integer = 3114		'上位情報設定変更要求(分割版)		(←TO_DEFECT)
	'Public Const FACT_HT_DEFECT_15 As Integer = 3115		'カメラゲイン変更要求				(←TO_DEFECT)
	'Public Const FACT_HT_DEFECT_16 As Integer = 3116		'露光時間変更通知					(←TO_DEFECT)
	Public Const FACT_HT_DEFECT_19 As Integer = 3119        '未実行設定のキャンセル要求			(←TO_DEFECT)
	Public Const FACT_HT_DEFECT_20 As Integer = 3120        '内部距離カウンタ設定要求			(←TO_DEFECT)
	Public Const FACT_HT_DEFECT_21 As Integer = 3121        'パラメータ事前読込要求				(←TO_DEFECT)

	'+++++++++++++++++++++++++++++++++++
	'表示機能
	'+++++++++++++++++++++++++++++++++++
	'HY_DEFECT
	Public Const FACT_HY_DEFECT_01 As Integer = 5101        'パラメータ変更通知					(←TO_DEFECT)
	Public Const FACT_HY_DEFECT_02 As Integer = 5102        '表示情報初期化通知					(←TO_DEFECT)
	Public Const FACT_HY_DEFECT_03 As Integer = 5103        '前工程疵情報読込通知				(←TO_DEFECT)
	'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	Public Const FACT_HY_DEFECT_04 As Integer = 5104        '前工程疵情報読込完了通知			(←HY_DEFECT)
	'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	Public Const FACT_HY_DEFECT_05 As Integer = 5105        '長周期欠陥発生通知					(←TO_LONGCHK)
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	'HY_TANSHO
	Public Const FACT_HY_TANSHO_01 As Integer = 5201		'パラメータ変更通知					(←HY_DEFECT)
	Public Const FACT_HY_TANSHO_02 As Integer = 5202		'イニシャル通知						(←HY_DEFECT)

	'HY_DEFLIST
	Public Const FACT_HY_DEFLIST_01 As Integer = 5301		'パラメータ変更通知					(←HY_DEFECT)
	'Public Const FACT_HY_DEFLIST_02 As Integer = 5302		'イニシャル通知						(←HY_DEFECT)
	
	''HY_DEFMAP
	'Public Const FACT_HY_DEFMAP_01 As Integer = 5401		'パラメータ変更通知					(←HY_DEFECT)
	'Public Const FACT_HY_DEFMAP_02 As Integer = 5402		'イニシャル通知						(←HY_DEFECT)
	'Public Const FACT_HY_DEFMAP_03 As Integer = 5403		'表示更新要求						(←HY_DEFECT)

	'+++++++++++++++++++++++++++++++++++
	'操作機能
	'+++++++++++++++++++++++++++++++++++
	'SO_GAMENN
	Public Const FACT_SO_GAMENN_01 As Integer = 6101		'稼動状態通知						(←TO_DEFECT)
	'SO_TRAND
	Public Const FACT_SO_TRAND_01 As Integer = 6201			'画面更新 							(←TO_DEFECT)
    'SO_DEFMAP
    Public Const FACT_SO_DEFMAP_01 As Integer = 6301        'パラメータ変更通知 				(←TO_DEFECT)
    Public Const FACT_SO_DEFMAP_02 As Integer = 6302        '画面更新 							(←SO_TRAND)
 
	'+++++++++++++++++++++++++++++++++++
	'全長画像保存機能
	'+++++++++++++++++++++++++++++++++++
	'TI_RPISAVE
	Public Const FACT_TI_RPISAVE_01 As Integer = 7101		'検査開始要求						(←TO_DEFECT)
	Public Const FACT_TI_RPISAVE_02 As Integer = 7102		'検査停止要求						(←TO_DEFECT)
	Public Const FACT_TI_RPISAVE_03 As Integer = 7103		'撮像開始/停止要求					(←TO_DEFECT)
	Public Const FACT_TI_RPISAVE_04 As Integer = 7104		'次コイル情報事前通知				(←TO_DEFECT)
	'Public Const FACT_TI_RPISAVE_05 As Integer = 7105		'外部コイル切替通知					(←TO_DEFECT)
	'Public Const FACT_TI_RPISAVE_06 As Integer = 7106		'問い合わせ要求						(←TO_DEFECT)
	Public Const FACT_TI_RPISAVE_07 As Integer = 7107		'再同期要求							(←TO_DEFECT)
	Public Const FACT_TI_RPISAVE_08 As Integer = 7108		'途中参加完了通知					(←TO_DEFECT)


	'///////////////////////////////////////////////////////////////////////////
	'列挙体
	'///////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 表示位置
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_DIV_DISP
        ''共通
        DIV_LEN_POS_KEN_TOP = 0                             '[通板距離] 検査基点 表　(カメラ位置)
        DIV_LEN_POS_KEN_BOT                                 '[通板距離] 検査基点 裏　(カメラ位置)
        DIV_LEN_POS_DSP_TOP                                 '[通板距離] 表示基点 表　(検査台位置1)
        DIV_LEN_POS_DSP_BOT                                 '[通板距離] 表示基点 裏　(検査台位置2)
        DIV_LEN_POS_BASE_END                                '共通項目の最後

        ''個別
        DIV_DISP_POS_DSP_P1                                 '[通板距離] 個別位置1
        DIV_DISP_POS_DSP_P2                                 '[通板距離] 個別位置2
        'DIV_DISP_POS_DSP_P3                                 '[通板距離] 個別位置3
        'DIV_DISP_POS_DSP_P4                                 '[通板距離] 個別位置4
        'DIV_DISP_POS_DSP_P5                                 '[通板距離] 個別位置5

        DIV_DISP_POS_END = DIV_LEN_POS_BASE_END             '最後に使用

	End Enum

	''' <summary>
	''' システム状態
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_DIV_SYS
		DIV_SYS_INIT = 1									'[システム状態]停止モード
		DIV_SYS_IDLE										'[システム状態]準備完了
		DIV_SYS_INITSTART									'[システム状態]検査開始初期化中
		DIV_SYS_WAITSTART									'[システム状態]検査開始待ち

		DIV_SYS_RUN											'[システム状態]検査中

		DIV_SYS_WAITSTOP									'[システム状態]検査停止待ち
		DIV_SYS_STOP										'[システム状態]検査停止
		DIV_SYS_RETRY										'[システム状態]強制検査再起動中
	End Enum

    ''' <summary>
    ''' 周期機能関連
    ''' </summary>
    ''' <remarks></remarks>
	Public Enum EM_DIV_CYCLESTAT
		DIV_CYCLESTAT_NON = 1								'[周期機能状態] 対象外 (SPM開放時等)
		DIV_CYCLESTAT_OK = 0								'[周期機能状態] 正常
		DIV_CYCLESTAT_STOP = -9								'[周期機能状態] 停止  (片面異常の場合、検査停止中)
	End Enum

    ''' <summary>
    ''' ロジックパララン 稼動状態
    ''' </summary>
    ''' <remarks></remarks>
	Public Enum EM_DIV_PARA_KADOU
		DIV_PARA_KADOU_STOP = 0								'[稼動状態] 停止
		DIV_PARA_KADOU_WAITSTOP								'[稼動状態] 停止中
		DIV_PARA_KADOU_WAITSTART							'[稼動状態] 開始中
		DIV_PARA_KADOU_START								'[稼動状態] 検査中
	End Enum

    ''' <summary>
    ''' 担当判定
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_HANTEI
		DIV_HANTEI_NON = -1									'[担当判定]関係無し
		
		DIV_HANTEI_BK = 0									'[担当判定]バックアップ
		DIV_HANTEI_HT										'[担当判定]単発判定
		DIV_HANTEI_CY										'[担当判定]周期判定
		DIV_HANTEI_PR										'[担当判定]パララン判定
	End Enum

	''' <summary>
	''' 表示管理の受信状態
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_DIV_HY_RECVST
		DIV_HY_RECVST_SKIP = -1								'[受信状態] スキップ (処理遅れにより受信データ破棄区間)
		DIV_HY_RECVST_NON = 0								'[受信状態] 未受信
		DIV_HY_RECVST_OK									'[受信状態] 正常受信
	End Enum

	'//=======================================
	'// LED照明装置 カラー
	'//=======================================
	Public Enum EM_DIV_LED_COLOR
		DIV_LED_COLOR_R = 0									'[LED照明装置] R
		DIV_LED_COLOR_G1									'[LED照明装置] G1
		DIV_LED_COLOR_G2									'[LED照明装置] G2
		DIV_LED_COLOR_B1									'[LED照明装置] B1
		DIV_LED_COLOR_B2									'[LED照明装置] B2

		DIV_LED_COLOR_END									'終端で使用するため注意
	End Enum

	'//=======================================
	'// LED照明装置 ユニット
	'//=======================================
	Public Enum EM_DIV_LED_UNIT
		DIV_LED_UNIT_R = 0									'[LED照明装置] R
		DIV_LED_UNIT_G = DIV_LED_UNIT_R						'[LED照明装置] G
		DIV_LED_UNIT_B										'[LED照明装置] B

		DIV_LED_UNIT_END									'終端で使用するため注意
	End Enum
	Public Const MAX_LED_CH As Integer = 3					'1ユニット辺りのチャンネル数
	Public Const MAX_LED_TEMP_UNIT As Integer = 3			'温度ユニット数

	'//=======================================
	'// 画像情報 (エンジンから出力される画像セット順)
	'//=======================================
    Public Enum EM_DIV_IMAGE_KIND
        DIV_IMAGE_KIND_R = 0                                ' 赤
        DIV_IMAGE_KIND_G                                    ' 緑
        DIV_IMAGE_KIND_B                                    ' 青
        DIV_IMAGE_KIND_GB                                   ' 色差(G-B)
    End Enum

    '//=======================================
    '// カット区分等
    '//=======================================
    Public Enum EM_DIV_CUT
        DIV_CUT_FCUT = 1                                    ' Fカット
        DIV_CUT_LCUT										' Lカット
		DIV_CUT_DIV											' 分割
		DIV_CUT_WPD_OTHER									' 異種点

		'DIV_CUT_CUT = 1			0						' 成品 先端-尾端位置
    End Enum

    Public Const ROW_KIND_UPPER As Integer = &H1            ' 成品先端
    Public Const ROW_KIND_BOTTOM As Integer = &H2           ' 成品尾端
    Public Const ROW_KIND_OTHER As Integer = &H4            ' 異種点
    Public Const ROW_KIND_YOSETU As Integer = &H8           ' 溶接点

	'//=======================================
	'// SPM圧下区分
	'//=======================================
	Public Enum EM_DIV_SPM_STAT
		DIV_SPM_INI = -1									' SPM圧下初期値
		DIV_SPM_FALSE = 0									' SPM圧下無し
		DIV_SPM_TRUE										' SPM圧下有り
    End Enum

    '//=======================================
    '// 実績格納区分
    '//=======================================
    Public Enum ENUM_END_MODE
        DIV_END_NON = 0                                     ' 不明
        DIV_END_NORMAL = 1                                  ' 通常切り替え
        DIV_END_BACKUP                                      ' バックアップ切替(検査用コイル長が管理上限に到達時)
        DIV_END_STOP_MANUAL                                 ' 手動停止
        DIV_END_STOP_AUTO                                   ' 強制停止
    End Enum

    '//=======================================
    '// プロコン採寸区分
    '//=======================================
    Public Enum EM_DIV_PRO_CUT
		DIV_PRO_CUT_NORMAL = 0								' [プロコン採寸区分] 通常カット
		DIV_PRO_CUT_FRONT_UNION								' [プロコン採寸区分] 前コイルと結合
		DIV_PRO_CUT_TAIL_UNION								' [プロコン採寸区分] 後コイルと結合
		DIV_PRO_CUT_UNION									' [プロコン採寸区分] 両方(前コイル、後ろコイル)結合
    End Enum


	'//=======================================
	'// 本システム固有の構造体
	'//=======================================
	''' <summary>
	''' 画像取込開始～統括処理完了までの所要時間
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_REQUIRED_TIME
		EM_TIME_HT_IMGGETSTART_TO_RECVEND = 1				'画像処理エンジンが画像取込開始～統括が受信完了
		EM_TIME_HT_IMGGETSTART_TO_EXECEND					'画像処理エンジンが画像取込開始～統括が処理完了
		EM_TIME_HT_IMGEXECEND_TO_RECVSTART					'画像処理エンジンが画像処理完了～統括が受信開始
		EM_TIME_TO_RECVSTART_TO_RECVEND_CAM					'統括が受信開始～統括が1カメラセット分の受信を完了
		EM_TIME_TO_RECVSTART_TO_RECVEND_ALL					'統括が受信開始～統括が全カメラセット分の受信を完了
		EM_TIME_HT_IMGGETSTART_HT_IMGEXECEND				'画像処理エンジンが画像取込開始～画像処理エンジンが画像処理完了
	End Enum
	Public Const MAX_REQUIRED_TIME As Integer = 10


    '***************************************************************
    ' システム共通構造体
    '***************************************************************
    '///////////////////////////////////////////////////////////////////////////
    '構造体
    '///////////////////////////////////////////////////////////////////////////
    '↓↓↓↓疵情報テーブル[TBL_HY_DEFECTINF]↓↓↓↓
    '-------------------------------------------------------
    '疵情報[TBL_HY_DEFECTINF]
    '-------------------------------------------------------
    Public Const SIZE_DETECT_BASE_DATA As Integer = 192
    ''' <summary>
    ''' 疵情報[TBL_HY_DEFECTINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DETECT_BASE_DATA, pack:=1)> _
    Public Structure DETECT_BASE_DATA
        Dim nKizuNo As Integer                              '疵No(1～の連番)
        Dim TorB As UShort                                  '表裏区分(0:表 1:裏)
        Dim nTid As UShort                                  '疵種EdasysID
        Dim nGid As UShort                                  'グレードEdasysID

        Dim cycle_div As UShort                             '周期疵区分(0:一般疵, 1:簡易周期疵,連続疵 )
        Dim dsws As UShort                                  'DSWS区分(0:DS 1:WS)
        Dim camset As UShort                                'カメラセットID (1～)	表DS, 表WS, 裏DS, 裏WS

        Dim y As Single                                     'Fからの位置[mm]
        Dim y_out As Single                                 '出側物理コイル先端からの位置[mm]※統括ではyと同じ距離。 表示管理で、出側距離に振り直し
        Dim ds As Single                                    'DS側エッジからの位置[mm]
        Dim ws As Single                                    'WS側エッジからの位置[mm]
        Dim dsc As Single                                   '板センターからの距離[mm]
        Dim wsc As Single                                   '板センターからの距離[mm]

        Dim speed As Integer                                '速度[mpm]
        Dim pitch As Integer                                'ピッチ[mm]
        Dim roolkei As Single                               'ロール径[mm]

        Dim aria As Single                                  '面積[mm^2]
        Dim kizu_wid As Integer                             '疵幅[mm]
        Dim kizu_len As Integer                             '疵長さ[mm]

        Dim h_res As Single                                 '縦分解能[mm/pixel]
        Dim w_res As Single                                 '横分解能[mm/pixel]
        Dim nFrameNo As Integer                             'フレームNo

        ' フレーム画像の左上からの位置
        Dim frm_x_min As Short                              'フレーム外接Xmin[pixel]
        Dim frm_x_max As Short                              'フレーム外接Xmax[pixel]
        Dim frm_y_min As Short                              'フレーム外接Ymin[pixel]
        Dim frm_y_max As Short                              'フレーム外接Ymax[pixel]

        ' 間引きフレーム画像の左上からの位置
        Dim cull_frm_x_min As Short                         '間引きフレーム外接Xmin[pixel]
        Dim cull_frm_x_max As Short                         '間引きフレーム外接Xmax[pixel]
        Dim cull_frm_y_min As Short                         '間引きフレーム外接Ymin[pixel]
        Dim cull_frm_y_max As Short                         '間引きフレーム外接Ymax[pixel]

        ' 切り出し画像関係 (原点は、切り出し画像の左上からの位置)
        Dim box_x_min As Short                              '外接Xmin[pixel](切り出し画像の左上からの位置)(マイナス値ありえる)
        Dim box_x_max As Short                              '外接Xmax[pixel]
        Dim box_y_min As Short                              '外接Ymin[pixel]
        Dim box_y_max As Short                              '外接Ymax[pixel]
        Dim center_x As Short                               '疵中心位置X[pixel](切り出し画像の左上からの位置)
        Dim center_y As Short                               '疵中心位置Y[pixel]
        Dim h_size As UShort                                '縦画素数[pixel]
        Dim w_size As UShort                                '横画素数[pixel]

        'ここまでで 108byte ----------------------------------------------------
        Dim nTyuu As UShort                                 '疵種優先度
        Dim nGyuu As UShort                                 'グレード優先度
        Dim nWidAbsPos As Integer                           '幅方向絶対位置
        Dim nWidDiv As Short                                '幅分割区分 (0:DS ～ 12:WS)
        Dim nObjTansho As Short                             '探傷画面対象疵(1:対象)
        Dim nObjDeflist As Short                            '重欠陥一覧画面対象疵
        Dim nModeDeflist As Short                           '重欠陥一覧強調表示対象疵  (0:強調枠無し(自動疵削除)  1:強調枠表示(手動疵削除)) 
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim time() As Short                                 '疵発生時刻年月日時分秒
        ' 元画像の切り出し画像関係 (原点は、切り出し画像の左上からの位置)
        Dim base_box_x_min As Short                         '元画像_外接Xmin[pixel](切り出し画像の左上からの位置)(マイナス値ありえる)
        Dim base_box_x_max As Short                         '元画像_外接Xmax[pixel]
        Dim base_box_y_min As Short                         '元画像_外接Ymin[pixel]
        Dim base_box_y_max As Short                         '元画像_外接Ymax[pixel]
        Dim base_center_x As Short                          '元画像_疵中心位置X[pixel](切り出し画像の左上からの位置)
        Dim base_center_y As Short                          '元画像_疵中心位置Y[pixel]
        Dim base_h_size As UShort                           '元画像_縦画素数[pixel]
        Dim base_w_size As UShort                           '元画像_横画素数[pixel]
        Dim base_h_res As Single                            '元画像_縦分解能[mm/pixel]
        Dim base_w_res As Single                            '元画像_横分解能[mm/pixel]
        Dim nShrinkRatio As Integer                         '縮小率 (1, 2, 4)
		'// 2023.09.14 画像処理装置 H・Sソフト改造 -------------------->>>>>
		'<VBFixedArray(28 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=28)>
		'Dim nYobi() As Byte                                 '予備

		Dim nIFMergeFlg As Integer                           ' フレーム間連結フラグ(0:単独疵 1:連結疵先頭 2:連結疵先頭以外)
		<VBFixedArray(24 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=24)>
		Dim nYobi() As Byte                                 '予備
		'// 2023.09.14 画像処理装置 H・Sソフト改造 -------------------->>>>>

		Public Sub initialize()
			ReDim time(6 - 1)
			'// 2023.09.14 画像処理装置 H・Sソフト改造 -------------------->>>>>
			'ReDim nYobi(28 - 1)
			ReDim nYobi(24 - 1)
			'// 2023.09.14 画像処理装置 H・Sソフト改造 <<<<<--------------------
		End Sub
    End Structure

    '↓↓↓↓コイル情報テーブル[TBL_HY_COILINF]↓↓↓↓
    '-------------------------------------------------------
    'システム共通のコイル情報[TBL_HY_COILINF]
    '-------------------------------------------------------
    Public Const SIZE_COIL_BASEDATA As Integer = 96
    ''' <summary>
    ''' システム共通のコイル情報[TBL_HY_COILINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_COIL_BASEDATA, pack:=1)> _
    Public Structure COIL_BASEDATA
        ' 基本部 (TO_GETDATでセット)
        <VBFixedArray(SIZE_MEI_NO - 1 + 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_MEI_NO + 1)> _
        Dim cMeiNo() As Byte                                '命令No+NULL
        <VBFixedArray(SIZE_COIL_NO - 1 + 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_COIL_NO + 1)> _
        Dim cCoilNoIn() As Byte                             '入側コイルNo+NULL
        <VBFixedArray(SIZE_COIL_KEY - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_COIL_KEY)>
        Dim cCoilKey() As Byte                              'コイルを一意に特定するためのキー+NULL
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cYobi1() As Byte                                '予備[バウンダリあわせ]
        Dim nLenIn As Integer                               '入側コイル長[mm]
        Dim nAtu As Integer                                 '板厚[μm]
        Dim nWid As Integer                                 '板幅[mm](=通常上位の板幅だが、上位情報が無い場合等は、最大板幅をセットしておく)

        ''↑↑ここまでは固定------------------------------
        Dim nCutLenF As Integer                             '入側フロント端板長さ
        Dim nCutLenT As Integer                             '入側テール端板長さ
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim cYobi2() As Byte                                '予備[バウンダリあわせ]
        Dim dNobiSPM As Double                              'SPM伸び率
        Dim nLenOut As Integer                              '出側コイル長
        <VBFixedArray(20 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=20)>
        Dim cYobi3() As Byte                                '予備
        'イニシャライズ
        Public Sub initialize()
            ReDim cCoilNoIn(SIZE_COIL_NO - 1 + 1)
            ReDim cMeiNo(SIZE_MEI_NO - 1 + 1)
            ReDim cCoilKey(SIZE_COIL_KEY - 1)
            ReDim cYobi1(2 - 1)
            ReDim cYobi2(4 - 1)
            ReDim cYobi3(20 - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    'コイル基本設定[TBL_TO_COIL]
    '-------------------------------------------------------
    '表示機能では未使用だが「LineCommon.h」と同じにするため
    Public Const SIZE_COIL_SETTING As Integer = 64
    ''' <summary>
    ''' コイル基本設定[TBL_TO_COIL]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_COIL_SETTING, pack:=1)> _
    Public Structure COIL_SETTING
        Dim bWalm As Byte                                   'ウォーマ材区分(ture:ウォーマ材)
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim bDefScode() As Byte                             '検査表面状態 デフォルト値使用(true:何かしらの異常でデフォルト値使用時)
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim bDefPcode() As Byte                             '検査パターン デフォルト値使用(true:何かしらの異常でデフォルト値使用時)
        <VBFixedArray(3 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=3)>
        Dim cYobi1() As Byte                                '予備[バウンダリあわせ]
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim scode() As Integer                              '検査表面状態(表/裏)(1オリジン)
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim pcode() As Integer                              '検査パターン(表/裏)(1オリジン)
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim scode_spmoff() As Integer                       'SPM開放時の検査表面状態(表/裏)(1オリジン)
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim pcode_spmoff() As Integer                       'SPM開放時の検査パターン(表/裏)(1オリジン)
        <VBFixedArray(24 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=24)>
        Dim cYobi2() As Byte                                '予備

        'イニシャライズ
        Public Sub initialize()
            ReDim bDefScode(NUM_MEN - 1)
            ReDim bDefPcode(NUM_MEN - 1)
            ReDim cYobi1(3 - 1)
            ReDim scode(NUM_MEN - 1)
            ReDim pcode(NUM_MEN - 1)
            ReDim scode_spmoff(NUM_MEN - 1)
            ReDim pcode_spmoff(NUM_MEN - 1)
            ReDim cYobi2(24 - 1)
        End Sub
    End Structure


    '=======================================
    ' 画面表示用 構造体
    '=======================================
    '-------------------------------------------------------
    'コイル情報共通部[TBL_HY_COILINF]
    '-------------------------------------------------------
    Public Const SIZE_COINFO_BASE As Integer = 128 + 1024
    Public Const SIZE_COINFO_RESERVE As Integer = 1024 - SIZE_COIL_BASEDATA - SIZE_PRO_TYPE_DATA_COILINF
    ''' <summary>
    ''' コイル情報共通部[TBL_HY_COILINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_COINFO_BASE, pack:=1)> _
    Public Structure COINFO_BASE
        <VBFixedArray(SIZE_KIZUKEN_NO - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_KIZUKEN_NO)> _
        Dim cKizukenNo() As Byte                            '管理No
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim scode() As Integer                              '検査表面状態
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim pcode() As Integer                              '検査パターン
        Dim nMapColNum As Integer                           'マップ列数

        Dim nCoilKensa As Integer                           'そのコイルで一区間でも縮退時:1 正常:0
        <VBFixedArray(84 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=84)> _
        Dim yobi() As Byte                                  '予備

        Dim base As COIL_BASEDATA                           'システム共通のコイル情報
        Dim data As PRO_TYPE_DATA_COILINF                   'コイル情報

        <VBFixedArray(SIZE_COINFO_RESERVE - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_COINFO_RESERVE)> _
        Dim reserve() As Byte                               '予備
        'イニシャライズ
        Public Sub initialize()
            ReDim cKizukenNo(SIZE_KIZUKEN_NO - 1)
            ReDim scode(NUM_MEN - 1)
            ReDim pcode(NUM_MEN - 1)
            ReDim yobi(84 - 1)
            ReDim reserve(SIZE_COINFO_RESERVE - 1)
            base.initialize()
            data.initialize()

        End Sub
    End Structure

    '↓↓↓↓マップ位置テーブル[TBL_HY_MAP_ROW]↓↓↓↓
    '-------------------------------------------------------
    '詳細情報[TBL_HY_MAP_ROW]
    '-------------------------------------------------------
    Public Const SIZE_DSP_TYP_ROW As Integer = 64
    ''' <summary>
    ''' 詳細情報[TBL_HY_MAP_ROW]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_TYP_ROW, pack:=1)> _
    Public Structure DSP_TYP_ROW
        Dim nEnable As Integer                              '情報受信状態(昔は、行列情報の-1で判断していた奴) (0:未受信 1:受信)
        Dim emKadou As EM_DIV_KADOU                         '稼動状態 (DIV_KADOU_INIT･･･)
        Dim emDspKensa As EM_DIV_KENSA                      '表示用検査状態 (DIV_KENSA_NON･･･)
        Dim emDspKiki As EM_DIV_KIKI                        '表示用機器状態 (DIV_KIKI_OK･･･)
        Dim nKizuCnt As Integer                             '行単位表示対象疵個数

        Dim nLen_i As Integer                               '入側距離(要は溶接点からの距離) [m]
        Dim nLen_o As Integer                               '出側距離(要はカットからの距離) [m]
        Dim nCut As Integer                                 'シャーカット区分(0:シャーカット無し 1:シャーカットされた)
        Dim nKind As Integer                                '付加情報 (0:付加情報無し 0x01:成品先端  0x02：成品尾端)

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim pnt() As Integer                                '疵情報へのポインタ[0:最小/1:最大]
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim pntMae() As Integer                             '前工程疵情報へのポインタ[0:最小/1:最大]
        <VBFixedArray(3 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=3)> _
        Dim yobi() As Integer                               '予備
        'イニシャライズ
        Public Sub initialize()
            ReDim pnt(2 - 1)
            ReDim pntMae(2 - 1)
            ReDim yobi(3 - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '行情報[TBL_HY_MAP_ROW]
    '-------------------------------------------------------
    Public Const SIZE_MAP_ROW_ROW As Integer = SIZE_DSP_TYP_ROW
    ''' <summary>
    ''' 行情報[TBL_HY_MAP_ROW]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_MAP_ROW_ROW, pack:=1)> _
    Public Structure MAP_ROW_ROW
        Dim typ As DSP_TYP_ROW                              '詳細情報[TBL_HY_MAP_ROW]
        'イニシャライズ
        Public Sub initialize()
            typ.initialize()
        End Sub
    End Structure

    '-------------------------------------------------------
    '面情報[TBL_HY_MAP_ROW]
    '-------------------------------------------------------
    Public Const SIZE_MAP_ROW_MEN As Integer = SIZE_MAP_ROW_ROW * MAP_ROW_NUM
    ''' <summary>
    ''' 面情報[TBL_HY_MAP_ROW]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_MAP_ROW_MEN, pack:=1)> _
    Public Structure MAP_ROW_MEN
        <VBFixedArray(MAP_ROW_NUM - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAP_ROW_NUM)> _
        Dim row() As MAP_ROW_ROW                            '行情報[TBL_HY_MAP_ROW]
        'イニシャライズ
        Public Sub initialize(Optional ByVal nRow As Integer = 0)
            If nRow = 0 Then
                ReDim row(MAP_ROW_NUM - 1)
            Else
                ' 行数指定の初期化が行われた場合の対応
                ReDim row(nRow - 1)
            End If
            For ii As Integer = 0 To row.Length - 1
                row(ii).initialize()
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    'マップ切替[TBL_HY_MAP_ROW]
    '-------------------------------------------------------
    Public Const SIZE_MAP_ROW_STAGE As Integer = SIZE_MAP_ROW_MEN * NUM_MEN
    ''' <summary>
    ''' マップ切替[TBL_HY_MAP_ROW]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_MAP_ROW_STAGE, pack:=1)> _
    Public Structure MAP_ROW_STAGE
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim men() As MAP_ROW_MEN                            '面情報[TBL_HY_MAP_ROW]
        'イニシャライズ
        Public Sub initialize(Optional ByVal nRow As Integer = 0)
            ReDim men(NUM_MEN - 1)
            For ii As Integer = 0 To men.Length - 1
                men(ii).initialize(nRow)
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    'レコード[TBL_HY_MAP_ROW]
    '-------------------------------------------------------
    Public Const SIZE_DSP_MAP_ROW As Integer = SIZE_MAP_ROW_STAGE * MAP_STAGE_NUM
    ''' <summary>
    ''' レコード[TBL_HY_MAP_ROW]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_MAP_ROW, pack:=1)> _
    Public Structure DSP_MAP_ROW
        <VBFixedArray(MAP_STAGE_NUM - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAP_STAGE_NUM)> _
        Dim stage() As MAP_ROW_STAGE                        'マップ切替[TBL_HY_MAP_ROW]
        'イニシャライズ
        Public Sub initialize(Optional ByVal nRow As Integer = 0)
            ReDim stage(MAP_STAGE_NUM - 1)
            For ii As Integer = 0 To stage.Length - 1
                stage(ii).initialize(nRow)
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    '表示疵情報[TBL_HY_DEFECTINF]
    '-------------------------------------------------------
    Public Const SIZE_DSP_TYP_DEFECTINF As Integer = 440
    Public Const SIZE_DSP_TYP_DEFECTINF_RESERVE As Integer = SIZE_DSP_TYP_DEFECTINF - SIZE_DETECT_BASE_DATA - 32 * 4
    ''' <summary>
    ''' 表示疵情報[TBL_HY_DEFECTINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_TYP_DEFECTINF, pack:=1)> _
    Public Structure DSP_TYP_DEFECTINF
        Dim data As DETECT_BASE_DATA                        '疵情報[TBL_HY_DEFECTINF]
        <VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim image_path_s As String                          '画像のパス[正](画像無し時はALL NULL)(絶対パスでもOK。極力相対パスにしてね)
        <VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim image_path_r As String                          '画像のパス[乱](画像無し時はALL NULL)(絶対パスでもOK。極力相対パスにしてね)
        <VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim image_path_r2 As String                         '画像のパス[乱2](画像無し時はALL NULL)(絶対パスでもOK。極力相対パスにしてね)
        <VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim image_path_r3 As String                         '画像のパス[乱3](画像無し時はALL NULL)(絶対パスでもOK。極力相対パスにしてね)
        <VBFixedArray(SIZE_DSP_TYP_DEFECTINF_RESERVE - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_DSP_TYP_DEFECTINF_RESERVE)> _
        Dim reserve() As Byte                               '予備
        'イニシャライズ
        Public Sub initialize()
            data.initialize()
            ReDim reserve(SIZE_DSP_TYP_DEFECTINF_RESERVE - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    'ポインタ[TBL_HY_DEFECTINF]
    '-------------------------------------------------------
    Public Const SIZE_DEFECTINF_PNT As Integer = SIZE_DSP_TYP_DEFECTINF
    ''' <summary>
    ''' ポインタ[TBL_HY_DEFECTINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DEFECTINF_PNT, pack:=1)> _
    Public Structure DEFECTINF_PNT
        Dim typ As DSP_TYP_DEFECTINF                        '表示疵情報[TBL_HY_DEFECTINF]
        'イニシャライズ
        Public Sub initialize()
            typ.initialize()
        End Sub
    End Structure

    '-------------------------------------------------------
    'ヘッダー情報[TBL_HY_DEFECTINF]
    '-------------------------------------------------------
    Public Const SIZE_DEFECTINF_HEADER As Integer = SIZE_KIZUKEN_NO + 4 + 4 + 8
    ''' <summary>
    ''' ヘッダー情報[TBL_HY_DEFECTINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DEFECTINF_HEADER, pack:=1)> _
    Public Structure DEFECTINF_HEADER
        <VBFixedArray(SIZE_KIZUKEN_NO - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_KIZUKEN_NO)> _
        Dim KizukenNo() As Byte                             '疵検管理No
        <VBFixedArray(SIZE_MEI_NO - 1 + 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_MEI_NO + 1)> _
        Dim cMeiNo() As Byte                                '命令No
        <VBFixedArray(3 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=3)> _
        Dim cyobi() As Byte                                 '予備
        Dim num As UInteger                                 '現在の格納件数
        Dim yobi As UInteger
        'イニシャライズ
        Public Sub initialize()
            ReDim KizukenNo(SIZE_KIZUKEN_NO - 1)
            ReDim cMeiNo(SIZE_MEI_NO - 1)
            ReDim cyobi(3 - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    'レコード[TBL_HY_DEFECTINF]
    '-------------------------------------------------------
    Public Const SIZE_DSP_MAP_DEFECTINF As Integer = SIZE_DEFECTINF_HEADER + SIZE_DEFECTINF_PNT * MAX_MAP_DEFECT_NUM
    ''' <summary>
    ''' レコード[TBL_HY_DEFECTINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_MAP_DEFECTINF, pack:=1)> _
    Public Structure DSP_MAP_DEFECTINF
        Dim header As DEFECTINF_HEADER                      'ヘッダー情報[TBL_HY_DEFECTINF]
        <VBFixedArray(MAX_MAP_DEFECT_NUM - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_MAP_DEFECT_NUM)> _
        Dim pnt() As DEFECTINF_PNT                          'ポインタ[TBL_HY_DEFECTINF]
        'イニシャライズ
        Public Sub initialize()
            header.initialize()
            ReDim pnt(MAX_MAP_DEFECT_NUM - 1)
            For ii As Integer = 0 To MAX_MAP_DEFECT_NUM - 1
                pnt(ii).initialize()
            Next
        End Sub
    End Structure

End Module
