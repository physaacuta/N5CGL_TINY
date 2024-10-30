'*******************************************************************************
'   φω共通定義
'
'   [Ver]
'       Ver.01  2013/12/11  初版
'
'   [メモ]
'
'*******************************************************************************
Option Strict On

Imports System.Runtime.InteropServices

''' <summary>
''' φω共通定義
''' </summary>
''' <remarks></remarks>
Public Module KizuCommon
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
    'LINEスイッチ(必ず1コのみ有効にする事)VBの場合モジュール毎に必要
#Const cLINE = "LINE_5CGL_TINY"

    '-------------------------------------------------------
    'φω設備関係
    '-------------------------------------------------------
    'カメラ設置位置 幅方向位置
    '#Const cNumCamPos = "NUM_CAM_POS_1"                         '面当りのカメラ位置数(C)
    '#Const cNumCamPos = "NUM_CAM_POS_2"                         '面当りのカメラ位置数(DS,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_3"                         '面当りのカメラ位置数(DS,C,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_4"                         '面当りのカメラ位置数(DS,D1,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_5"                         '面当りのカメラ位置数(DS,D1,C,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_6"                         '面当りのカメラ位置数(DS,D1,D2,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_7"                         '面当りのカメラ位置数(DS,D1,D2,C,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_8"                         '面当りのカメラ位置数(DS,D1,D2,D3,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_9"                         '面当りのカメラ位置数(DS,D1,D2,D3,C,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_10"                        '面当りのカメラ位置数(DS,D1,D2,D3,D4,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_11"                        '面当りのカメラ位置数(DS,D1,D2,D3,D4,C,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_12"                        '面当りのカメラ位置数(DS,D1,D2,D3,D4,D5,W5,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_13"                        '面当りのカメラ位置数(DS,D1,D2,D3,D4,D5,C,W5,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_14"                        '面当りのカメラ位置数(DS,D1,D2,D3,D4,D5,D6,W6,W5,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_15"                        '面当りのカメラ位置数(DS,D1,D2,D3,D4,D5,D6,C,W6,W5,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_16"                        '面当りのカメラ位置数(DS,D1,D2,D3,D4,D5,D6,D7,W7,W6,W5,W4,W3,W2,W1,WS)

#Const cNumCamPos = "NUM_CAM_POS_10"                        '面当りのカメラ位置数(DS,D1,D2,D3,D4,W4,W3,W2,W1,WS)

    '///////////////////////////////////////////////////////////////////////////
    '定数
    '///////////////////////////////////////////////////////////////////////////
    '-------------------------------------------------------
    '設定値[iniファイル]
    '-------------------------------------------------------
    '疵検設定ファイル(機能単位の設定ファイル)
#If cLOCAL = False Then                                     '本番環境
    Public Const TASKINI_NAME As String = ".\KizuTask.ini"                      '疵検タスク設定ファイル
#Else                                                       'テスト環境
	Public Const TASKINI_NAME As String = ".\KizuTask.ini"	                    '疵検タスク設定ファイル
#End If

    '疵検設定ファイル(PC単位の設定ファイル)
#If cLOCAL = False Then                                     '本番環境
    Public Const PCINI_NAME As String = ".\KizuPc.ini"                          'PC設定ファイル
#Else                                                       'テスト環境
	Public Const PCINI_NAME As String = ".\KizuPc.ini"			                'PC設定ファイル
#End If

    '疵検設定ファイル(PCのローカル単位の設定ファイル)
    '特殊なiniファイルなので、以下の事に注意
    '・個々のパソコンの一時的な設定情報を保存しておくファイル
    '・ログオフ等しても上書きされない
    '・当iniファイルが無くても、必ずタスク側は動作する(デフォルト値などで)ように、作っておく事
#If cLOCAL = False Then                                     '本番環境
    Public Const LOCALINI_NAME As String = ".\KizuLocal.ini"                    'Local設定ファイル
#Else                                                       'テスト環境
	Public Const LOCALINI_NAME As String = ".\KizuLocal.ini"	                'Local設定ファイル
#End If

    '共通セッション
    Public Const INI_COMMON As String = "COMMON"                                '共通設定 用セッション
    Public Const INI_DB As String = "DB"                                        'DB接続 用セッション
    Public Const INI_DB_A As String = "DB_A"                                    'DB接続 用セッション(A系)
    Public Const INI_DB_B As String = "DB_B"                                    'DB接続 用セッション(B系)
    Public Const INI_STATUS As String = "STATUS"                                '機器ID 用セッション
    Public Const INI_MEM As String = "MEM"                                      '共有メモリ 用セッション
    Public Const INI_EXE As String = "EXECUTE"                                  'プロセス 用セッション

    '-------------------------------------------------------
    '固定サイズ
    '-------------------------------------------------------
    Public Const SIZE_MSL As Integer = 256                                      'send_mail,recv_mailサイズ
    Public Const SIZE_MSL_DATA As Integer = SIZE_MSL - 8                        'nEventNo以降のデータ長
    Public Const SIZE_NAME_PC As Integer = 16                                   'パソコン名称
    Public Const SIZE_NAME_TASK As Integer = 16                                 'タスク名称
    Public Const SIZE_IMAGE_PATH As Integer = 32                                '画像保存用のパス長
    Public Const SIZE_KIZUKEN_NO As Integer = 20                                '管理No
    Public Const SIZE_SERIAL_NO As Integer = 16                                 '一意な管理No([yyyymmdd hhnnss\0]の形式)
    Public Const SIZE_SERIAL_FF_NO As Integer = 20                              '一意な管理No([yyyymmdd hhnnssff\0]の形式)

    '-------------------------------------------------------
    'EPC関連のマスター
    '-------------------------------------------------------
    Public Const MAX_HT1_CAMSET As Integer = 16                                 '判定PC1台の最大カメラセット(EPC側8セットまでしか対応していない)
    Public Const MAX_HT1_CAMANGLE As Integer = 4                                '判定PC1台の最大カメラ角度 (EPC側4角度までしか対応していない)
    Public Const MAX_ATTR_BASE As Integer = 256                                 '最大特徴量数(基本特徴量)
    'Public Const MAX_ATTR_ANGLE As Integer = 128                                '最大特徴量数(角度別特徴量) ※ 複眼
    Public Const MAX_ATTR_ANGLE As Integer = 256                                '最大特徴量数(角度別特徴量) ※ 色差

    Public Const MAX_TAJUUTHRESHOLD As Integer = 30                             '多重閾値情報数 MAX
    Public Const MAX_REDUCTION As Integer = 30                                  '縮小率情報数 MAX
    Public Const MAX_FILTER As Integer = 30                                     'フィルタ情報数 MAX
    Public Const MAX_CYCLETHRESHOLD As Integer = 30                             '周期用閾値情報数 MAX
    Public Const MAX_CROP_SHRINK As Integer = 4                                 '間引き数 MAX

    '-------------------------------------------------------
    'DBマスター絡み
    '-------------------------------------------------------
    Public Const MAX_STATUS As Integer = 1024                                   '最大 機器状態マスター数
    Public Const MAX_TYPE As Integer = 256                                      '疵種情報MAX
	'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
	'Public Const MAX_GRADE As Integer = 8                                       'グレード情報MAX
	Public Const MAX_GRADE As Integer = 32                                       'グレード情報MAX
	'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
    Public Const MAX_CAMSET As Integer = NUM_CAM_POS * 2                        'カメラセット数MAX
    Public Const MAX_CAMANGLE As Integer = NUM_CAM_ANGLE                        'カメラ角度数MAX(論理的な角度)
    'Public Const MAX_PHYSICAL_CAMANGLE As Integer = NUM_CAM_ANGLE               'カメラ角度数MAX (物理的な角度)		※ 複眼
    Public Const MAX_PHYSICAL_CAMANGLE As Integer = 1                           'カメラ角度数MAX (物理的な角度)		※ 色差
    Public Const MAX_SCODE As Integer = 64                                      '検査表面状態MAX(デフォルト項目1個含む)
    Public Const MAX_PCODE As Integer = 64                                      '検査パターンMAX(デフォルト項目1個含む)
    Public Const MAX_CCODE As Integer = 64                                      'カウントパターンMAX(デフォルト項目1個含む)
    Public Const MAX_CODE_JUDGE As Integer = 32                                 '検査表面状態・検査パターンの条件MAX

    '-------------------------------------------------------
    'NOA定義
    '-------------------------------------------------------
    Public Const NOA_ONLINE As Integer = 0                                      'オンライン
    Public Const NOA_OFFLINE As Integer = 1                                     'オフライン
    Public Const NOA_DEBUG As Integer = 9                                       'デバッグ用 (使いやすい用に全部入り)
    Public Const NOA_KAISEKI As Integer = -1                                    '解析用

    '-------------------------------------------------------
    'PC/タスク設定
    '-------------------------------------------------------
    '20171222 KizuLibCLI.KizuMod.MasterPcNameを使用すること
    '       ※frmMain_Ras、frmMain_Ras2で使用している為、残すが
    '         MASTER_PC_NAME、SYSLOG_PC_NAMEは使用禁止とする。
#If cLOCAL = False Then                                     '本番環境
    Public Const MASTER_PC_NAME As String = "TOKATU"                            '疵検状態管理送り先PC [KS_MASTERの起動PC名称を指定]("."指定はダメ)
    Public Const SYSLOG_PC_NAME As String = "TOKATU"                            'syslog送り先PC名
#Else                                                       'テスト環境
	Public Const MASTER_PC_NAME as String =	"TOKATU"				            '疵検状態管理送り先PC [KS_MASTERの起動PC名称を指定]("."指定はダメ)
	Public Const SYSLOG_PC_NAME as String =	"TOKATU"				            'syslog送り先PC名
#End If

    '-------------------------------------------------------
    'パラメータ変更区分(疵検共通部分)
    '   各ライン固有画面の場合は、LineCommonに記述すること
    '-------------------------------------------------------
    Public Const CH_SCODE_NON As Integer = 0                                    '検査表面状態対象外
    Public Const CH_SCODE_ALL As Integer = 999                                  '検査表面状態全部変更区分

    '今野さんロジックが、全パラメータ一括読込みとなった為、画面単位での通知に変更
    Public Const CH_PARAM_ALL As Integer = 0                                    '全テーブル(デバック用)
    Public Const CH_PARAM_TYPE_GRADE As Integer = 1                             '疵種・Gr

    Public Const CH_PARAM_ATTR_NAME As Integer = 5                              '特徴量名称
    Public Const CH_PARAM_PCINFODESC As Integer = 6                             '上位情報

    Public Const CH_PARAM_COMMON As Integer = 10                                '固定パラメータ
    Public Const CH_PARAM_CAMERA As Integer = 11                                'カメラ設定
    Public Const CH_PARAM_EDGE As Integer = 12                                  'エッジ検出
    Public Const CH_PARAM_SHD As Integer = 13                                   'シェーディング補正
    Public Const CH_PARAM_THRESHOLD As Integer = 14                             '閾値設定
    Public Const CH_PARAM_DETECT As Integer = 15                                '疵検出
    Public Const CH_PARAM_CYCLE As Integer = 16                                 '周期疵判定
    Public Const CH_PARAM_MERGE As Integer = 17                                 'カメラ境界処理
    Public Const CH_PARAM_LIGHT As Integer = 18                                 '照明
    Public Const CH_PARAM_TINY As Integer = 19                                  '微小欠陥設定

    Public Const CH_PARAM_SYNCADD As Integer = 20                               '画像加算
    Public Const CH_PARAM_COLOR_DIV As Integer = 21                             '混色分離
    'Public Const CH_PARAM_YOUSET As Integer = 22                                '溶接点検出

    Public Const CH_PARAM_SETTIG_SCODE As Integer = 110                         '検査表面状態 マスター・条件設定・出力設定
    Public Const CH_PARAM_SETTIG_PCODE As Integer = 111                         '検査パターン マスター・条件設定・出力設定
    Public Const CH_PARAM_SETTIG_COMMON As Integer = 112                        '検査定数設定
    Public Const CH_PARAM_SETTIG_PCOMMON As Integer = 113                       '検査パターン 固有 出力設定
    Public Const CH_PARAM_SETTIG_PALL As Integer = 114                          '検査パターン 共通 出力設定
    Public Const CH_PARAM_SETTIG_LOG As Integer = 115                           'シスログテーブル
    Public Const CH_PARAM_SETTIG_STATUS As Integer = 116                        '機器状態テーブル
    Public Const CH_PARAM_CAMERA_LINKSWITCH As Integer = 117                    'カメラリンクスイッチ設定
    Public Const CH_PARAM_DEMAND As Integer = 118                               '需要家設定

    '-------------------------------------------------------
    'プロセス名称一覧
    '-------------------------------------------------------
    '支援機能
    Public Const KS_MASTER As String = "KS_MASTER"                              'PCマスター管理
    Public Const KS_SLAVE As String = "KS_SLAVE"                                'PCスレーブ管理
    Public Const KS_START As String = "KS_START"                                'スタートアップ管理
    Public Const KS_WAKEUP As String = "KS_WAKEUP"                              'イニシャル中待機画面
    Public Const KS_EVTLOG As String = "KS_EVTLOG"                              'イベントビューアー情報管理
    Public Const KS_SYSLOG As String = "KS_SYSLOG"                              'SYSLOG登録タスク
    Public Const KS_UPSMGR As String = "KS_UPSMGR"                              'UPS異常終了タスク
    Public Const KS_BACKUP As String = "KS_BACKUP"                              'バックアップ切替支援タスク
    Public Const KS_EQALDB As String = "KS_EQALDB"                              'DB同期管理タスク

    Public Const KS_HDDCHK As String = "KS_HDDCHK"                              'HDD状態監視タスク
    Public Const KS_HISTRY As String = "KS_HISTRY"                              '機器状態履歴管理タスク
    Public Const KS_APCHECK As String = "KS_APCHECK"                            'AP状態監視タスク
    Public Const KS_MAILTR As String = "KS_MAILTR"                              'メールスロット転送タスク

    '共通タスク
    'ここに未定義のKTタスクは、LineCommon.hに起動要因等定義しているのがあるので注意
    Public Const KT_CHILER As String = "KT_CHILER"                              '対チラー通信
    Public Const KT_LEDMGR As String = "KT_LEDMGR"                              '対LED照明通信
    Public Const KT_TEMPMT As String = "KT_TEMPMT"                              '対温度パネルメータ
    Public Const KT_RPICHK As String = "KT_RPICHK"                              'RPIファイル管理

    '蓄積機能
    Public Const TI_DELETE As String = "TI_DELETE"                              'DB削除管理タスク	

    '-------------------------------------------------------
    'データベース
    '-------------------------------------------------------
    'マスター
    Public Const DB_STATUS_MASTER As String = "M_STATUS_MASTER"                     '[DB]機器マスターテーブル
    Public Const DB_STATUS_INF As String = "M_STATUS_INF"                           '[DB]機器状態テーブル
    Public Const DB_LOG_MASTER As String = "M_LOG_MASTER"                           '[DB]ログマスターテーブル
    Public Const DB_LOG_INF As String = "T_LOG_INF"                                 '[DB]ログ情報テーブル
    Public Const DB_PC_MASTER As String = "M_PC_MASTER"                             '[DB]PCマスター
    Public Const DB_PC_INF As String = "M_PC_INF"                                   '[DB]PC種別
    Public Const DB_ST_HISTORY_MASTER As String = "M_ST_HISTORY_MASTER"             '[DB]機器履歴マスターテーブル
    Public Const DB_ST_HISTORY_INF As String = "T_ST_HISTORY_INF"                   '[DB]機器履歴テーブル
    Public Const DB_STATUS_LOG As String = "T_STATUS_LOG"                           '[DB]機器状態ログテーブル
    Public Const DB_BASE_ACTION_MASTER As String = "M_BASE_ACTION_MASTER"           '[DB]基本動作一覧設定

    Public Const DB_COMMON_MASTER As String = "M_COMMON_MASTER"                     '[DB]検査定数マスター
    Public Const DB_TYPE_NAME As String = "TYPE_NAME"                               '[DB]疵種マスター
    Public Const DB_GRADE_NAME As String = "GRADE_NAME"                             '[DB]グレードマスター
    Public Const DB_SCODE_MASTER As String = "M_SCODE_MASTER"                       '[DB]検査表面状態マスター
    Public Const DB_SCODE_JUDGE As String = "M_SCODE_JUDGE"                         '[DB]検査表面状態条件設定
    Public Const DB_SCODE_OUTPUT As String = "M_SCODE_OUTPUT"                       '[DB]検査表面状態別出力設定
    Public Const DB_PCODE_MASTER As String = "M_PCODE_MASTER"                       '[DB]検査パターンマスター
    Public Const DB_PCODE_JUDGE As String = "M_PCODE_JUDGE"                         '[DB]検査パターン条件設定
    Public Const DB_PCODE_OUTPUT As String = "M_PCODE_OUTPUT"                       '[DB]検査パターン別出力設定
    Public Const DB_PALL_OUTPUT As String = "M_PALL_OUTPUT"                         '[DB]検査パターン共通出力設定
    Public Const DB_PCOMMON_OUTPUT As String = "M_PCOMMON_OUTPUT"                   '[DB]検査パターン固有出力設定

    Public Const DB_CAMERA_LINKSWITCH As String = "M_CAMERA_LINKSWITCH"             '[DB]カメラリンクスイッチ設定
    Public Const DB_PAR_GRID_SETTING As String = "M_PAR_GRID_SETTING"               '[DB]データグリッド表示設定
    Public Const DB_PAR_LOV_ITEM As String = "M_PAR_LOV_ITEM"                       '[DB]画面選択項目アイテムマスタ


    'パラメータ
    Public Const DB_ATTR_NAME As String = "ATTR_NAME"                               '[DB]特徴量名称マスター
    Public Const DB_PARAM_COMMON As String = "PARAM_COMMON"                         '[DB]共通パラメータ
    Public Const DB_PARAM_SCODE As String = "PARAM_SCODE"                           '[DB]表面状態別パラメータ
    Public Const DB_PARAM_CAMSET As String = "PARAM_CAMSET"                         '[DB]カメラセット別パラメータ
    Public Const DB_PARAM_CAMSET_SCODE As String = "PARAM_CAMSET_SCODE"             '[DB]カメラセット別×表面状態別パラメータ
    Public Const DB_PARAM_CAMSET_ANGLE As String = "PARAM_CAMSET_ANGLE"             '[DB]カメラセット別×カメラ角度別パラメータ
    Public Const DB_PARAM_CAMANGLE_SCODE As String = "PARAM_CAMANGLE_SCODE"         '[DB]カメラ角度別×表面状態別パラメータ
    Public Const DB_PARAM_CAMSET_ANGLE_SCODE As String = "PARAM_CAMSET_ANGLE_SCODE" '[DB]カメラセット別×カメラ角度別×表面状態別パラメータ
    Public Const DB_ROLL_INFO As String = "ROLL_INFO"                               '[DB]ロール情報
    Public Const DB_PARAM_PCINFO_DESC As String = "PCINFO_DESC"                     '[DB]上位情報内容

    Public Const DB_PARAM_REDUCTION As String = "PARAM_REDUCTION"                   '[DB]縮小率
    Public Const DB_PARAM_FILTER As String = "PARAM_FILTER"                         '[DB]フィルタ 
    Public Const DB_PARAM_THRESHOLD As String = "PARAM_THRESHOLD"                   '[DB]多重用閾値
    'Public Const DB_PARAM_CYCLE_THRESHOLD As String = "PD_PARAM_THRESHOLD"          '[DB]周期用閾値
    Public Const DB_PARAM_CROP_SHRINK As String = "PARAM_CROP_SHRINK"               '[DB]間引き

    ' <20221117> <小島> <最終工程高機能化その1> --->>>
    Public Const DB_PARAM_LABELIMAGE As String = "PARAM_LABELIMAGE"                 '[DB]
    Public Const DB_PARAM_SIMPLEBIN As String = "PARAM_SIMPLEBIN"                   '[DB]
    Public Const DB_PARAM_FILTERBIN As String = "PARAM_FILTERBIN"                   '[DB]
    ' <20221117> <小島> <最終工程高機能化その1> <<<---

	'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
	Public Const DB_PARAM_CYCLE As String = "PARAM_CYCLE"                            '[DB] 簡易周期疵判定パラメータテーブル
	'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------

    '-------------------------------------------------------
    '起動要因
    '-------------------------------------------------------
    '+++++++++++++++++++++++++++++++++++
    '支援機能
    '+++++++++++++++++++++++++++++++++++
    'KS_MASTER
    Public Const FACT_KS_MASTER_01 As Integer = 9101        '時刻変更要求                  (←TO_DEFECT,   SO_GAMENN)
    Public Const FACT_KS_MASTER_02 As Integer = 9102        'ハートビート                  (←KS_SLAVE)
    Public Const FACT_KS_MASTER_03 As Integer = 9103        'PCマスター管理状態問い合わせ  (←KS_SLAVE)
    Public Const FACT_KS_MASTER_04 As Integer = 9104        '立ち上げ完了報告              (←KS_SLAVE,    KS_MASTER)
    Public Const FACT_KS_MASTER_05 As Integer = 9105        '立ち下げ完了報告              (←KS_SLAVE,    KS_MASTER)
    Public Const FACT_KS_MASTER_06 As Integer = 9106        'システム終了要求              (←SO_GAMENN,   KS_UPSMGR)
    Public Const FACT_KS_MASTER_07 As Integer = 9107        'PC再起動要求                  (←SO_GAMENN)
    Public Const FACT_KS_MASTER_08 As Integer = 9108        'PCログオフ要求                (←SO_GAMENN)
    Public Const FACT_KS_MASTER_09 As Integer = 9109        'PC常駐タスク終了要求          (←KS_MASTER)

    'KS_SLAVE
    Public Const FACT_KS_SLAVE_01 As Integer = 9121         '時刻変更要求                  (←KS_MASTER)
    Public Const FACT_KS_SLAVE_02 As Integer = 9122         '統括ハートビート              (←KS_MASTER)
    Public Const FACT_KS_SLAVE_03 As Integer = 9123         'PCマスター管理起動完了通知    (←KS_MASTER)
    Public Const FACT_KS_SLAVE_04 As Integer = 9124         'シャットダウン要求            (←KS_MASTER)
    Public Const FACT_KS_SLAVE_05 As Integer = 9125         'PC再起動要求                  (←KS_MASTER)
    Public Const FACT_KS_SLAVE_06 As Integer = 9126         'PCログオフ要求                (←KS_MASTER)
    Public Const FACT_KS_SLAVE_07 As Integer = 9127         'PC常駐タスク終了要求          (←KS_MASTER)

    'KS_START
    '9131

    'KS_SYSLOG
    Public Const FACT_KS_SYSLOG_01 As Integer = 9141        'シスログ出力
    Public Const FACT_KS_SYSLOG_02 As Integer = 9142        '機器状態ログ

    'KS_EVTLOG
    '9151

    'KS_WAKEUP
    Public Const FACT_KS_WAKEUP_01 As Integer = 9161        '統括立ち上げ完了報告          (←KS_MASTER,   KS_SLAVE)
    Public Const FACT_KS_WAKEUP_02 As Integer = 9162        '蓄積立ち上げ完了報告          (←KS_MASTER,   KS_SLAVE)

    'KS_UPSMGR
    '9171

    'KS_BACKUP
    '9181

    'KS_EQALDB
    '9191
    Public Const FACT_KS_EQALDB_01 As Integer = 9191        '即時実行通知                  (←KS_BACKUP)
    'Public Const FACT_KS_EQALDB_02 As Integer = 9192        '実績移動通知                 (←KS_BACKUP)

    'KS_MAILTR
    '9231
    Public Const FACT_KS_MAILTR_01 As Integer = 9231        'UDP送信依頼(mailudp_que)

    '+++++++++++++++++++++++++++++++++++
    '蓄積機能
    '+++++++++++++++++++++++++++++++++++
    'TI_DELETE
    Public Const FACT_TI_DELETE_01 As Integer = 9411        '削除･書込優先モード通知       (←TO_DEFECT)

    '+++++++++++++++++++++++++++++++++++
    'ツール
    '+++++++++++++++++++++++++++++++++++
    '9800～9899 はツール等で使用の為予約

    '+++++++++++++++++++++++++++++++++++
    'ホントに共通で使用
    '+++++++++++++++++++++++++++++++++++
    Public Const FACT_KS_TASK_STATEUS As Integer = 9901     'タスク状態取得要求(～9920まで予約)
    Public Const FACT_KS_DB_STATEUS As Integer = 9997       'DB状態変更通知(fl.data[0]= 0:正常 1:異常)	
    Public Const FACT_KS_PCNAME_CHANGE As Integer = 9998    'PC名称変更通知起動要因
    Public Const FACT_KS_END As Integer = 9999              'タスク終了起動要因

    '-------------------------------------------------------
    '画面表示色
    '-------------------------------------------------------
    '共通
    Public g_ColorControl As Color = Color.FromKnownColor(KnownColor.Control)   'システム背景色

    '稼動状態
    Public g_ColorKadouInit As Color = Color.Red                                '初期化中
    Public g_ColorKadouStop As Color = Color.Red                                '停止中
    Public g_ColorKadouStart As Color = Color.Blue                              '稼動中
    Public g_ColorKadouRetry As Color = Color.DarkOrange                        'ﾘﾄﾗｲ中

    '検査状態
    Public g_ColorKensaNon As Color = Color.FromKnownColor(KnownColor.Control)  '停止
    Public g_ColorKensaOk As Color = Color.Blue                                 '正常
    Public g_ColorKensaRed As Color = Color.Red                                 '縮退(コイル情報欄　赤色)
    Public g_ColorKensaNg As Color = Color.Violet                               '縮退(桃色)
    Public g_ColorKensaEdgeNg As Color = Color.Magenta                          'エッジ不正
    Public g_ColorKensaDbNon As Color = Color.MediumOrchid                      '未保存
    Public g_ColorKensaSkip As Color = Color.MediumPurple                       '未検査
    Public g_ColorKensaStop As Color = Color.Black                              '検査停止
    Public g_ColorMente As Color = Color.DarkOrange                             'ﾒﾝﾃ中

    Public g_ColorOkure As Color = Color.Violet                                 '統括→表示間で受信遅れ(表示PCでしか使わない)

    '操業状態
    Public g_ColorSogyoStop As Color = Color.Red                                '停止中
    Public g_ColorSogyoSlow As Color = Color.Blue                               'ライン一時停止or低速操業
    Public g_ColorSogyoRun As Color = Color.Blue                                '通板中
    Public g_ColorSogyoNon As Color = Color.Red                                 '不明

    '機器状態
    Public g_ColorKikiOk As Color = Color.Blue                                  '正常
    Public g_ColorKikiKei As Color = Color.Orange                               '軽故障
    Public g_ColorKikiJyu As Color = Color.Red                                  '重故障

    '運転モード
    Public g_ColorUntenStop As Color = Color.Red                                '停止
    Public g_ColorUntenNomal As Color = Color.Blue                              '通常運転
    Public g_ColorUntenSamp As Color = Color.Magenta                            'テスト運転
    Public g_ColorUntenCamera As Color = Color.Orange                           '校正モード
    Public g_ColorUntenSmyu As Color = Color.Orange                             'シミュレーション

    'PLG区分
    Public g_ColorPlgLine As Color = Color.Blue                                 'ライン
    Public g_ColorPlgTop As Color = Color.Magenta                               '表校正
    Public g_ColorPlgBot As Color = Color.Magenta                               '裏校正
    Public g_ColorPlgTest As Color = Color.Orange                               'テスト

    '-------------------------------------------------------
    'TEMPファイル関連
    '-------------------------------------------------------
    Public Const IMG_PATH As String = "..\TEMP\JPEG"                            'ハードコピー格納場所
    Public Const CSV_PATH As String = "..\TEMP\CSV"                             'CSV格納場所
    Public Const EXCEL_PATH As String = "..\TEMP\EXCEL"                         'EXCEL格納場所

    '-------------------------------------------------------
    'シスログ№
    '-------------------------------------------------------
    '-----------------------------------------------------------------------------------------------------------------------------
    '全般 0
    '[№]                                                   '[サブメッセージ形式]                   [メッセージ]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_NON As Integer = 0                   '								        SyslogNo未割り当て

    '-----------------------------------------------------------------------------------------------------------------------------
    'メール関連 1～4
    '[№]                                                   '[サブメッセージ形式]                   [メッセージ]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_MAIL_SNED_ERR As Integer = 1         '[通知名 err_code = %d]		            メールスロット送信失敗
    Public Const SYSNO_MAIL_RECV_ERR As Integer = 2         '[通知名 err_code = %d]		            メールスロット受信失敗
    Public Const SYSNO_MAILNO_ERR As Integer = 3            '[event = %d]				            イベントコード不明

    '-----------------------------------------------------------------------------------------------------------------------------
    '手動コメント関連 5～7
    '[№]                                                   '[サブメッセージ形式]                   [メッセージ]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_COMMENT_ALL As Integer = 5           '								        手入力(全般)
    Public Const SYSNO_COMMENT_SOFT As Integer = 6          '								        手入力(ソフト)
    Public Const SYSNO_COMMENT_HARD As Integer = 7          '								        手入力(ハード)

    '-----------------------------------------------------------------------------------------------------------------------------
    'ソケット 10～24
    '[№]                                                   '[サブメッセージ形式]                   [メッセージ]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_SOCK_SEND_TIMEOUT As Integer = 10    '[通信名]					            [SOCKET]送信タイムアウト
    Public Const SYSNO_SOCK_NEW_ERR As Integer = 11         '[err_code = %d]				        [SOCKET]生成失敗
    Public Const SYSNO_SOCK_SEND_ERR As Integer = 12        '[err_code = %d]				        [SOCKET]送信エラー
    Public Const SYSNO_SOCK_CONNECT As Integer = 13         '[通信名]					            [SOCKET]接続完了
    Public Const SYSNO_SOCK_CLOSE As Integer = 14           '[通信名]					            [SOCKET]切断
    Public Const SYSNO_SOCK_RECV_ERR As Integer = 15        '[section=%s err_code=%d]	            [SOCKET]受信エラー
    Public Const SYSNO_SOCK_LISTEN_ERR As Integer = 16      '[section=%s err_code=%d]	            [SOCKET]Listenエラー
    Public Const SYSNO_SOCK_CLIENT_MAX As Integer = 17      '								        [SOCKET]クライアント数オーバー
    Public Const SYSNO_SOCK_INITIALIZ As Integer = 18       '[name=%s]					            [SOCKET]イニシャル伝文受信
    Public Const SYSNO_SOCK_TC_NG As Integer = 19           '[section=%s code=%4.4s]		        [SOCKET]トランザクションコード異常
    Public Const SYSNO_SOCK_RECV_TIMEOUT As Integer = 20    '								        [SOCKET]受信タイムアウト
    Public Const SYSNO_SOCK_CONNECT_ERR As Integer = 21     '[err_code = %d]				        [SOCKET]Connect失敗
    Public Const SYSNO_SOCK_HELCY_TIMEOUT As Integer = 22   '[通信名]                               [SOCKET]ヘルシー監視タイムアウト

    '-----------------------------------------------------------------------------------------------------------------------------
    '共有メモリ 25～29
    '[№]                                                   '[サブメッセージ形式]                   [メッセージ]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_MEM_OPEN As Integer = 25             '[共有メモリ名]				            [共有メモリ]オープン失敗
    Public Const SYSNO_MEM_READ As Integer = 26             '[共有メモリ名]				            [共有メモリ]読み込み失敗
    Public Const SYSNO_MEM_WRITE As Integer = 27            '[共有メモリ名]				            [共有メモリ]書き込み失敗

    '-----------------------------------------------------------------------------------------------------------------------------
    'DB 30～39
    '[№]                                                   '[サブメッセージ形式]                   [メッセージ]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_DB_EXECUTEDIRECT_ERR As Integer = 30 '[テーブル名]				            [DB]SqlExecuteDirectエラー
    Public Const SYSNO_DB_FETCH_ERR As Integer = 31         '[テーブル名]				            [DB]SqlFetchエラー
    Public Const SYSNO_DB_CONNECT_ERR As Integer = 32       '[テーブル名]				            [DB]SqlConnectエラー
    Public Const SYSNO_DB_DATA_ERR As Integer = 33          '[テーブル名]				            [DB]データ不正
    Public Const SYSNO_DB_BINALYWRITE_ERR As Integer = 34   '[テーブル名]				            [DB]バイナリ書込みエラー
    Public Const SYSNO_DB_ODBC_SET_OK As Integer = 35       '[ODBC名,アドレス]		                [DB]ODBC設定書換完了
    Public Const SYSNO_DB_ODBC_SET_ERR As Integer = 36      '[ODBC名,アドレス]		                [DB]ODBC設定書換失敗

    '-----------------------------------------------------------------------------------------------------------------------------
    'RS232C 40～45
    '[№]                                                   '[サブメッセージ形式]                   [メッセージ]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_232C_NEW_ERR As Integer = 40         '[err_code=%d]				            [232C]生成失敗
    Public Const SYSNO_232C_SEND_ERR As Integer = 41        '[err_code=%d]				            [232C]送信エラー
    Public Const SYSNO_232C_RECV_ERR As Integer = 42        '[err_code=%d]				            [232C]受信エラー
    Public Const SYSNO_232C_OPEN As Integer = 43            '[通信名]					            [232C]接続完了
    Public Const SYSNO_232C_CLOSE As Integer = 44           '[通信名]					            [232C]切断

    '-----------------------------------------------------------------------------------------------------------------------------
    'PIPE 46～49
    '[№]                                                   '[サブメッセージ形式]                   [メッセージ]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_PIPE_OPEN As Integer = 45            '							            [PIPE]接続完了
    Public Const SYSNO_PIPE_CLOSE As Integer = 46           '							            [PIPE]切断完了
    Public Const SYSNO_PIPE_CONNECT_ERR As Integer = 47     '[err_code=%d]			                [PIPE]接続失敗
    Public Const SYSNO_PIPE_EXEC As Integer = 48            '							            [PIPE]操作正常完了
    Public Const SYSNO_PIPE_EXEC_ERR As Integer = 49        '[err_code=%d]			                [PIPE]操作異常

    '-----------------------------------------------------------------------------------------------------------------------------
    '50～69 その他グループ
    '[№]                                                   '[サブメッセージ形式]                   [メッセージ]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_CNT_ERR As Integer = 50              '[err_code=%d sub_code=%d]	            [カウンターボード]異常発生
    Public Const SYSNO_PIO_ERR As Integer = 51              '[err_code=%d sub_code=%d]	            [PIOボード]異常発生
    Public Const SYSNO_AIO_ERR As Integer = 52              '[err_code=%d sub_code=%d]	            [アナログボード]異常発生
    Public Const SYSNO_PLG_ERR As Integer = 53              '[検査用=%0.3f 比較用=%0.3f 差=%d]      [カウンターボード]PLG整合性異常

    Public Const SYSNO_STATUS_OK As Integer = 55            '[機器名]					            [機器状態監視]機器障害復旧
    Public Const SYSNO_STATUS_NG As Integer = 56            '[機器名]					            [機器状態監視]機器障害検出
    Public Const SYSNO_EVTLOG_MSG_NG As Integer = 57        '[メッセージ名]				            [イベントログ監視]異常検出
    Public Const SYSNO_EVTLOG_AP_NG As Integer = 58         '							            [イベントログ監視]アプリケーション異常検出
    Public Const SYSNO_STATUS_WAR As Integer = 59           '[機器名]                               [機器状態監視] 機器警告検出 (軽故障)


    '-----------------------------------------------------------------------------------------------------------------------------
    '70～99 一般もろもろ
    '[№]                                                   '[サブメッセージ形式]                   [メッセージ]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_TASK_INIT_ERR As Integer = 70        '[タスク名]					            タスク初期起動処理失敗
    Public Const SYSNO_INI_READ_ERR As Integer = 71         '[セッション,キー]			            [INI]取得エラー
    Public Const SYSNO_PCNAME_GET_ERR As Integer = 72       '								        PC名称取得失敗
    Public Const SYSNO_QUEFULL_ERR As Integer = 73          '[区分名称]				                [LIB]内部バッファキューフル！！
    Public Const SYSNO_QUEFULL_WAR As Integer = 74          '[区分名称]				                [LIB]内部バッファキューフル

    Public Const SYSNO_UPS_NG As Integer = 80               '								        電源異常
    Public Const SYSNO_TIMESET_NG As Integer = 81           '[err_code = %d]				        時刻設定失敗	
    Public Const SYSNO_RESTAET_ERR As Integer = 82          '							            再起動失敗	
    Public Const SYSNO_SHUTDOWN_ERR As Integer = 83         '								        シャットダウン失敗	
    Public Const SYSNO_TIMESET_OK As Integer = 84           '								        時刻設定完了	
    Public Const SYSNO_DELETE_ERR As Integer = 85           '							            DB削除失敗
    Public Const SYSNO_COMPRESS_ERR As Integer = 86         '								        DB圧縮失敗
    Public Const SYSNO_RESTAET_OK As Integer = 87           '[PC名]					                PC再起動要求受付
    Public Const SYSNO_SHUTDOWN_OK As Integer = 88          '                                       システム終了要求受付
    Public Const SYSNO_SYSTEM_START As Integer = 89         '[開始条件]				                検査開始要求
    Public Const SYSNO_SYSTEM_STOP As Integer = 90          '							            検査停止要求
    Public Const SYSNO_LOGOFF_ERR As Integer = 91           '						                ログオフ失敗
    Public Const SYSNO_LOGOFF_OK As Integer = 92            '[PC名]					                PCログオフ要求受付	
    Public Const SYSNO_DB_CHECK_RETRY As Integer = 93       '[リトライ回数]			                データベース接続チェックで接続失敗

    '///////////////////////////////////////////////////////////////////////////
    '列挙体
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 拡張BOOL
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_BOOL
        DIV_BOOL_TRUE = 1               '正常   ○
        DIV_BOOL_FALSE = 0              '異常   ×
        DIV_BOOL_NON = -1               '無し   －
        DIV_BOOL_NULL = -9              '対象外 ＊
    End Enum

    ''' <summary>
    ''' PLG区分
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_PLG
        DIV_PLG_LINE = 1                'ライン
        DIV_PLG_TOP                     '表校正
        DIV_PLG_BOT                     '裏校正
        DIV_PLG_TEST                    'テスト
    End Enum

    ''' <summary>
    ''' 運転モード
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_UNTEN
        DIV_UNTEN_STOP = 0              '停止モード
        DIV_UNTEN_NOMAL                 '通常運転モード
        DIV_UNTEN_SAMP                  'テスト運転モード(サンプル撮影)
        DIV_UNTEN_CAMERA                '校正モード(カメラ調整)
        DIV_UNTEN_SMYU                  'シミュレーションモード
    End Enum

    ''' <summary>
    ''' 操業状態
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_SOGYO
        DIV_SOGYO_NON = -1              '不明
        DIV_SOGYO_STOP = 0              'ライン休止中
        DIV_SOGYO_SLOW                  'ライン一時停止or低速操業
        DIV_SOGYO_RUN                   'ライン操業中
    End Enum

    ''' <summary>
    ''' 稼動状態
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_KADOU
        DIV_KADOU_INIT = 1              '初期化中
        DIV_KADOU_STOP                  '停止中
        DIV_KADOU_START                 '稼動中&テスト中
        DIV_KADOU_RETRY                 'リトライ中
    End Enum

    ''' <summary>
    ''' 検査状態
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_KENSA
        DIV_KENSA_NON = -9              '停止(対象外)

        DIV_KENSA_OK = 0                '正常
        DIV_KENSA_NG = 1                '縮退
        DIV_KENSA_MENTE = 2             'メンテ中(プロビジョン)

        DIV_KENSA_EDGENG = 4            'エッジ不正(エッジ未検出、エッジ整合性異常)
        DIV_KENSA_DBNON = 5             'DB実績未保存(有害Gr実績遅れ、保存件数上限到達)
        DIV_KENSA_SKIP = 6              '未検査区間(画像処理エンジンがスキップ)

        DIV_KENSA_STOP = 9              '停止(片面異常の場合、検査停止中)
    End Enum

    ''' <summary>
    ''' 機器状態
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_KIKI
        DIV_KIKI_OK = 0                 '正常
        DIV_KIKI_KEI = 1                '軽故障
        DIV_KIKI_TYU = 2                '中故障(プロビジョン)
        DIV_KIKI_JYU = 3                '重故障
    End Enum

    ''' <summary>
    ''' カメラポジション
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_POS
#If cNumCamPos = "NUM_CAM_POS_1" Then
        DIV_POS_C = 0                   'C
#End If
#If cNumCamPos = "NUM_CAM_POS_2" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_3" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_C                       'C
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_4" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_5" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_C                       'C
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_6" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_7" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_C                       'C
		DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_8" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_W3                      'W3
        DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_9" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_C                       'C
		DIV_POS_W3                      'W3
		DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_10" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_D4                      'D4
        DIV_POS_W4                      'W4
        DIV_POS_W3                      'W3
        DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_11" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_D4                      'D4
        DIV_POS_C                       'C
		DIV_POS_W4                      'W4
		DIV_POS_W3                      'W3
		DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_12" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_D4                      'D4
        DIV_POS_D5                      'D5
        DIV_POS_W5                      'W5
        DIV_POS_W4                      'W4
        DIV_POS_W3                      'W3
        DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_13" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_D4                      'D4
        DIV_POS_D5                      'D5
        DIV_POS_C                       'C
		DIV_POS_W5                      'W5
		DIV_POS_W4                      'W4
		DIV_POS_W3                      'W3
		DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_14" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_D4                      'D4
        DIV_POS_D5                      'D5
        DIV_POS_D6                      'D6
        DIV_POS_W6                      'W6
        DIV_POS_W5                      'W5
        DIV_POS_W4                      'W4
        DIV_POS_W3                      'W3
        DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
 #If cNumCamPos = "NUM_CAM_POS_15" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_D4                      'D4
        DIV_POS_D5                      'D5
        DIV_POS_D6                      'D6
        DIV_POS_C                       'C
		DIV_POS_W6                      'W6
		DIV_POS_W5                      'W5
		DIV_POS_W4                      'W4
		DIV_POS_W3                      'W3
		DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_16" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_D4                      'D4
        DIV_POS_D5                      'D5
        DIV_POS_D6                      'D6
        DIV_POS_D7                      'D7
        DIV_POS_W7                      'W7
        DIV_POS_W6                      'W6
        DIV_POS_W5                      'W5
        DIV_POS_W4                      'W4
        DIV_POS_W3                      'W3
        DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
   End Enum

    ''' <summary>
    ''' カメラセット
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_CAMSET
        DIV_CAMSET_BACK = 0             '[カメラペアNo]バックアップ
#If cNumCamPos = "NUM_CAM_POS_1" Then
        DIV_CAMSET_T_C                  '表
        
        DIV_CAMSET_B_C                  '裏
#End If
#If cNumCamPos = "NUM_CAM_POS_2" Then
        DIV_CAMSET_T_DS                 '表
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '裏
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_3" Then
        DIV_CAMSET_T_DS                 '表
		DIV_CAMSET_T_C
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '裏
		DIV_CAMSET_B_C
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_4" Then
        DIV_CAMSET_T_DS                 '表
        DIV_CAMSET_T_D1
        DIV_CAMSET_T_W1
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '裏
        DIV_CAMSET_B_D1
        DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_5" Then
        DIV_CAMSET_T_DS                 '表
        DIV_CAMSET_T_D1
        DIV_CAMSET_T_C
        DIV_CAMSET_T_W1
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '裏
        DIV_CAMSET_B_D1
        DIV_CAMSET_B_C
        DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_6" Then
        DIV_CAMSET_T_DS                 '表
        DIV_CAMSET_T_D1
        DIV_CAMSET_T_D2
        DIV_CAMSET_T_W2
        DIV_CAMSET_T_W1
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '裏
        DIV_CAMSET_B_D1
        DIV_CAMSET_B_D2
        DIV_CAMSET_B_W2
        DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_7" Then
	    DIV_CAMSET_T_DS                 '表
	    DIV_CAMSET_T_D1
	    DIV_CAMSET_T_D2
		DIV_CAMSET_T_C
	    DIV_CAMSET_T_W2
	    DIV_CAMSET_T_W1
	    DIV_CAMSET_T_WS
	    
	    DIV_CAMSET_B_DS                 '裏
	    DIV_CAMSET_B_D1
	    DIV_CAMSET_B_D2
		DIV_CAMSET_B_C
	    DIV_CAMSET_B_W2
	    DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_8" Then
        DIV_CAMSET_T_DS                 '表
        DIV_CAMSET_T_D1
        DIV_CAMSET_T_D2
        DIV_CAMSET_T_D3
        DIV_CAMSET_T_W3
        DIV_CAMSET_T_W2
        DIV_CAMSET_T_W1
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '裏
        DIV_CAMSET_B_D1
        DIV_CAMSET_B_D2
        DIV_CAMSET_B_D3
        DIV_CAMSET_B_W3
        DIV_CAMSET_B_W2
        DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_9" Then
	    DIV_CAMSET_T_DS                 '表
	    DIV_CAMSET_T_D1
	    DIV_CAMSET_T_D2
	    DIV_CAMSET_T_D3
		DIV_CAMSET_T_C
	    DIV_CAMSET_T_W3
	    DIV_CAMSET_T_W2
	    DIV_CAMSET_T_W1
	    DIV_CAMSET_T_WS
	    
	    DIV_CAMSET_B_DS                 '裏
	    DIV_CAMSET_B_D1
	    DIV_CAMSET_B_D2
	    DIV_CAMSET_B_D3
		DIV_CAMSET_B_C
	    DIV_CAMSET_B_W3
	    DIV_CAMSET_B_W2
	    DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_10" Then
        DIV_CAMSET_T_DS                 '表
        DIV_CAMSET_T_D1
        DIV_CAMSET_T_D2
        DIV_CAMSET_T_D3
        DIV_CAMSET_T_D4
        DIV_CAMSET_T_W4
        DIV_CAMSET_T_W3
        DIV_CAMSET_T_W2
        DIV_CAMSET_T_W1
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '裏
        DIV_CAMSET_B_D1
        DIV_CAMSET_B_D2
        DIV_CAMSET_B_D3
        DIV_CAMSET_B_D4
        DIV_CAMSET_B_W4
        DIV_CAMSET_B_W3
        DIV_CAMSET_B_W2
        DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_11" Then
	    DIV_CAMSET_T_DS                 '表
	    DIV_CAMSET_T_D1
	    DIV_CAMSET_T_D2
	    DIV_CAMSET_T_D3
	    DIV_CAMSET_T_D4
		DIV_CAMSET_T_C
	    DIV_CAMSET_T_W4
	    DIV_CAMSET_T_W3
	    DIV_CAMSET_T_W2
	    DIV_CAMSET_T_W1
	    DIV_CAMSET_T_WS
	    
	    DIV_CAMSET_B_DS                 '裏
	    DIV_CAMSET_B_D1
	    DIV_CAMSET_B_D2
	    DIV_CAMSET_B_D3
	    DIV_CAMSET_B_D4
		DIV_CAMSET_B_C
	    DIV_CAMSET_B_W4
	    DIV_CAMSET_B_W3
	    DIV_CAMSET_B_W2
	    DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_12" Then
        DIV_CAMSET_T_DS                 '表
        DIV_CAMSET_T_D1
        DIV_CAMSET_T_D2
        DIV_CAMSET_T_D3
        DIV_CAMSET_T_D4
        DIV_CAMSET_T_D5
        DIV_CAMSET_T_W5
        DIV_CAMSET_T_W4
        DIV_CAMSET_T_W3
        DIV_CAMSET_T_W2
        DIV_CAMSET_T_W1
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '裏
        DIV_CAMSET_B_D1
        DIV_CAMSET_B_D2
        DIV_CAMSET_B_D3
        DIV_CAMSET_B_D4
        DIV_CAMSET_B_D5
        DIV_CAMSET_B_W5
        DIV_CAMSET_B_W4
        DIV_CAMSET_B_W3
        DIV_CAMSET_B_W2
        DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_13" Then
	    DIV_CAMSET_T_DS                 '表
	    DIV_CAMSET_T_D1
	    DIV_CAMSET_T_D2
	    DIV_CAMSET_T_D3
	    DIV_CAMSET_T_D4
	    DIV_CAMSET_T_D5
		DIV_CAMSET_T_C
	    DIV_CAMSET_T_W5
	    DIV_CAMSET_T_W4
	    DIV_CAMSET_T_W3
	    DIV_CAMSET_T_W2
	    DIV_CAMSET_T_W1
	    DIV_CAMSET_T_WS
	    
	    DIV_CAMSET_B_DS                 '裏
	    DIV_CAMSET_B_D1
	    DIV_CAMSET_B_D2
	    DIV_CAMSET_B_D3
	    DIV_CAMSET_B_D4
	    DIV_CAMSET_B_D5
		DIV_CAMSET_B_C
	    DIV_CAMSET_B_W5
	    DIV_CAMSET_B_W4
	    DIV_CAMSET_B_W3
	    DIV_CAMSET_B_W2
	    DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_14" Then
        DIV_CAMSET_T_DS                 '表
        DIV_CAMSET_T_D1
        DIV_CAMSET_T_D2
        DIV_CAMSET_T_D3
        DIV_CAMSET_T_D4
        DIV_CAMSET_T_D5
        DIV_CAMSET_T_D6
        DIV_CAMSET_T_W6
        DIV_CAMSET_T_W5
        DIV_CAMSET_T_W4
        DIV_CAMSET_T_W3
        DIV_CAMSET_T_W2
        DIV_CAMSET_T_W1
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '裏
        DIV_CAMSET_B_D1
        DIV_CAMSET_B_D2
        DIV_CAMSET_B_D3
        DIV_CAMSET_B_D4
        DIV_CAMSET_B_D5
        DIV_CAMSET_B_D6
        DIV_CAMSET_B_W6
        DIV_CAMSET_B_W5
        DIV_CAMSET_B_W4
        DIV_CAMSET_B_W3
        DIV_CAMSET_B_W2
        DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_15" Then
	    DIV_CAMSET_T_DS                 '表
	    DIV_CAMSET_T_D1
	    DIV_CAMSET_T_D2
	    DIV_CAMSET_T_D3
	    DIV_CAMSET_T_D4
	    DIV_CAMSET_T_D5
	    DIV_CAMSET_T_D6
		DIV_CAMSET_T_C
	    DIV_CAMSET_T_W6
	    DIV_CAMSET_T_W5
	    DIV_CAMSET_T_W4
	    DIV_CAMSET_T_W3
	    DIV_CAMSET_T_W2
	    DIV_CAMSET_T_W1
	    DIV_CAMSET_T_WS
	    
	    DIV_CAMSET_B_DS                 '裏
	    DIV_CAMSET_B_D1
	    DIV_CAMSET_B_D2
	    DIV_CAMSET_B_D3
	    DIV_CAMSET_B_D4
	    DIV_CAMSET_B_D5
	    DIV_CAMSET_B_D6
		DIV_CAMSET_B_C
	    DIV_CAMSET_B_W6
	    DIV_CAMSET_B_W5
	    DIV_CAMSET_B_W4
	    DIV_CAMSET_B_W3
	    DIV_CAMSET_B_W2
	    DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_16" Then
        DIV_CAMSET_T_DS                 '表
        DIV_CAMSET_T_D1
        DIV_CAMSET_T_D2
        DIV_CAMSET_T_D3
        DIV_CAMSET_T_D4
        DIV_CAMSET_T_D5
        DIV_CAMSET_T_D6
        DIV_CAMSET_T_D7
        DIV_CAMSET_T_W7
        DIV_CAMSET_T_W6
        DIV_CAMSET_T_W5
        DIV_CAMSET_T_W4
        DIV_CAMSET_T_W3
        DIV_CAMSET_T_W2
        DIV_CAMSET_T_W1
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '裏
        DIV_CAMSET_B_D1
        DIV_CAMSET_B_D2
        DIV_CAMSET_B_D3
        DIV_CAMSET_B_D4
        DIV_CAMSET_B_D5
        DIV_CAMSET_B_D6
        DIV_CAMSET_B_D7
        DIV_CAMSET_B_W7
        DIV_CAMSET_B_W6
        DIV_CAMSET_B_W5
        DIV_CAMSET_B_W4
        DIV_CAMSET_B_W3
        DIV_CAMSET_B_W2
        DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
    End Enum

    ''' <summary>
    ''' 欠陥グレード
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DETECT_GR
        DETECT_GR_NON = 0               '無害
        DETECT_GR_KEI                   '軽欠陥
        DETECT_GR_TYU                   '中欠陥
        DETECT_GR_JU                    '重欠陥
    End Enum

    ''' <summary>
    ''' 欠陥グレード
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DETECT_GR2
        DETECT_GR_NON = 0               '無害
        DETECT_GR_KEI                   '軽欠陥
        DETECT_GR_TYU                   '中欠陥
        DETECT_GR_JU                    '重欠陥
    End Enum

    '///////////////////////////////////////////////////////////////////////////
    '構造体
    '///////////////////////////////////////////////////////////////////////////
    '-------------------------------------------------------
    '機器情報[TBL_HY_STATUS]
    '-------------------------------------------------------
    Public Const SIZE_TYP_STATUS_INF As Integer = 288
    ''' <summary>
    ''' 機器情報[TBL_HY_STATUS]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TYP_STATUS_INF, pack:=1)> _
    Public Structure TYP_STATUS_INF
        Dim id As Integer                                   '機器ID
        Dim pcid As Integer                                 'PCID
        Dim area As Integer                                 '異常範囲
        Dim Kensa As EM_DIV_KENSA                           '検査異常度
        Dim Kiki As EM_DIV_KIKI                             '機器異常度
        Dim yusen As Integer                                '優先度
        <VBFixedString(256), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=256)> _
        Dim msg As String                                   '異常時のメッセージ
        Dim pcmode As Integer                               'PC区分
        Dim stat As Integer                                 '現状状態
    End Structure

    '-------------------------------------------------------
    'ステータス[TBL_HY_STATUS]
    '-------------------------------------------------------
    Public Const SIZE_TYP_STATUS_NOWSYSTEM As Integer = SIZE_TYP_STATUS_INF * 5 + 20
    ''' <summary>
    ''' ステータス[TBL_HY_STATUS]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TYP_STATUS_NOWSYSTEM, pack:=1)> _
    Public Structure TYP_STATUS_NOWSYSTEM
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, sizeconst:=2)> _
        Dim st_kensa() As EM_DIV_KENSA                      '検査異常度
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, sizeconst:=2)> _
        Dim st_kiki() As EM_DIV_KIKI                        '機器異常度
        Dim nCnt As Integer                                 '異常セット件数
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, sizeconst:=5)> _
        Dim typErr() As TYP_STATUS_INF                      '機器情報[TBL_HY_STATUS]
        'イニシャライズ
        Public Sub initialize()
            ReDim st_kensa(2 - 1)
            ReDim st_kiki(2 - 1)
            ReDim typErr(5 - 1)
        End Sub
    End Structure
End Module
