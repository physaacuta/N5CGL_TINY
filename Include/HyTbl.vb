'*******************************************************************************
'   共有メモリ定義
'
'  [Ver]
'       Ver.01  2013/12/12  初版
'
'  [メモ]
'
'*******************************************************************************
Option Strict On

Imports System.Runtime.InteropServices

Public Module HyTbl
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
    '共有メモリ名称
    '-------------------------------------------------------
    Public Const TBL_HY_STATUS_NAME As String = "TBL_HY_STATUS"					'ステータス情報テーブル
	Public Const TBL_HY_POINT_NAME As String = "TBL_HY_POINT"					'ポインタ情報テーブル
	Public Const TBL_HY_COILINF_NAME As String = "TBL_HY_COILINF"				'コイル情報テーブル
	Public Const TBL_HY_COILRESULT_NAME As String = "TBL_HY_COILRESULT"			'コイル実績テーブル
	'Public Const TBL_HY_MAP_COLROW_NAME As String = "TBL_HY_MAP_COLROW"			'マップ情報テーブル
	Public Const TBL_HY_MAP_ROW_NAME As String = "TBL_HY_MAP_ROW"				'マップ位置テーブル
	Public Const TBL_HY_DEFECTINF_NAME As String = "TBL_HY_DEFECTINF"           '疵情報テーブル

    Public Const TBL_HY_DEFLIST_NAME As String = "TBL_HY_DEFLIST"               '重欠陥一覧表示ポインタ情報
    'Public Const TBL_HY_DSPLENGTH_NAME As String = "TBL_HY_DSPLENGTH"			'表示位置情報テーブル
    Public Const TBL_HY_DEFMAP_NAME As String = "TBL_HY_DEFMAP"                 '疵マップ表示情報テーブル

    'Public Const TBL_HY_MAE_MAP_NAME As String = "TBL_HY_MAE_MAP"				'前工程マップ情報
    Public Const TBL_HY_MAE_INF_NAME As String = "TBL_HY_MAE_INF"               '前工程疵情報

    'Public Const TBL_HY_CYCLE_MAP_NAME As String = "TBL_HY_CYCLE_MAP"			'周期マップ情報
    'Public Const TBL_HY_CYCLE_INF_NAME As String = "TBL_HY_CYCLE_INF"           '周期疵情報

    Public Const TBL_HY_ALARM_NAME As String = "TBL_HY_ALARM"                   '警報情報

	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	Public Const TBL_HY_LONGPD_NAME As String = "TBL_HY_LONGPD"                 '長周期情報テーブル
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	'-------------------------------------------------------
	'固定サイズ
	'-------------------------------------------------------
#If cLOCAL = False Then                                        '本番環境
	Public Const TBL_HY_REC_MAX As Integer = 6                                  '表示PC共有メモリレコード数
	Public Const TBL_HY_MAE_INF_MAX As Integer = 2000                           '前工程疵情報
    Public Const HY_DEFLIST_REC_MAX As Integer = 25                             '重欠陥一覧表示ポインタ情報 最大レコード数
    Public Const HY_DEFMAP_REC_MAX As Integer = 6                               '疵マップ表示情報　最大件数
    Public Const DSP_IDX_OLD_MAX As Integer = 1                                 '参照レコード数 (自コイル含めず)
#Else                                                     'テスト環境
    Public Const TBL_HY_REC_MAX As Integer = 6                                  '表示PC共有メモリレコード数
    Public Const TBL_HY_MAE_INF_MAX As Integer = 2000                           '前工程疵情報
    Public Const HY_DEFLIST_REC_MAX As Integer = 25                             '重欠陥一覧表示ポインタ情報 最大レコード数
    Public Const HY_DEFMAP_REC_MAX As Integer = 6                               '疵マップ表示情報　最大件数
    Public Const DSP_IDX_OLD_MAX As Integer = 1                                 '参照レコード数 (自コイル含めず)
#End If

    Public Const TBL_HY_COIL_MAX As Integer = TBL_HY_REC_MAX '(TBL_HY_REC_MAX * MAX_V_COIL_NUM)	'表示PC共有メモリ コイル数



	'///////////////////////////////////////////////////////////////////////////
	'列挙体
	'///////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 画面表示状態
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_DIV_DSP_STATUS
		DIV_DSP_NG = -1					'画面表示不可 (表示通知無しで、次のコイルにいっちゃった)
		DIV_DSP_INITIALIZE				'データなし   (初期値)
		DIV_DSP_STANDBY					'画面表示用意 (検査中だが、対象材がまだ、表示基点を通過していない)
		DIV_DSP_OK						'画面表示可
	End Enum

    '///////////////////////////////////////////////////////////////////////////
    '構造体
    '///////////////////////////////////////////////////////////////////////////
    '-------------------------------------------------------
    'ステータス情報テーブル[TBL_HY_STATUS]
    '-------------------------------------------------------
    Public Const SIZE_TBL_HY_STATUS As Integer = 4 + 4 + SIZE_TYP_STATUS_NOWSYSTEM
    ''' <summary>
    ''' ステータス情報テーブル[TBL_HY_STATUS]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_STATUS, Pack:=1)>
    Public Structure TBL_HY_STATUS
        Dim emUnten As EM_DIV_UNTEN                             '運転状態
        Dim emKadou As EM_DIV_KADOU                             '稼動状態
        Dim status As TYP_STATUS_NOWSYSTEM                      'ステータス
    End Structure

    '↓↓↓↓ポインタ情報テーブル[TBL_HY_POINT]↓↓↓↓
    ''-------------------------------------------------------
    ''全体付加情報[TBL_HY_POINT]
    ''-------------------------------------------------------
    'Public Const SIZE_POINT_INF_NOW As Integer = 4
    '''' <summary>
    '''' 全体情報
    '''' </summary>
    '''' <remarks></remarks>
    '<StructLayout(LayoutKind.Sequential, Size:=SIZE_POINT_INF_NOW, pack:=1)> _
    'Public Structure POINT_INF_NOW
    '	Dim nLenZan As Integer									'溶接点残長
    'End Structure

    '-------------------------------------------------------
    '位置情報 通板距離[TBL_HY_POINT]
    '-------------------------------------------------------
    'Public Const SIZE_POINT_LENGTH As Integer = 4 * NUM_MEN
    Public Const SIZE_POINT_LENGTH As Integer = 4
    ''' <summary>
    ''' 通板距離
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_POINT_LENGTH, Pack:=1)>
    Public Structure POINT_LENGTH
        Dim nLen As Integer                                   '通板距離
        'イニシャライズ
        Public Sub initialize()
        End Sub
    End Structure

    '-------------------------------------------------------
    '位置情報 マップ切替[TBL_HY_POINT]
    '-------------------------------------------------------
    Public Const SIZE_POINT_STAGE As Integer = 4 * MAP_STAGE_NUM
    ''' <summary>
    ''' マップ切替
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_POINT_STAGE, Pack:=1)>
    Public Structure POINT_STAGE
        <VBFixedArray(MAP_STAGE_NUM - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAP_STAGE_NUM)>
        Dim nStage() As Integer                                 'マップ切替
        'イニシャライズ
        Public Sub initialize()
            ReDim nStage(MAP_STAGE_NUM - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '各検査台位置情報[TBL_HY_POINT]
    '-------------------------------------------------------
    Public Const SIZE_ROW_INFO As Integer = SIZE_POINT_LENGTH * MAX_DISP_POS + SIZE_POINT_STAGE * MAX_DISP_POS _
    + SIZE_POINT_STAGE * NUM_MEN + SIZE_POINT_STAGE + 8
    ''' <summary>
    ''' 各検査台位置情報
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_ROW_INFO, Pack:=1)>
    Public Structure POINT_ROW_INFO
        <VBFixedArray(MAX_DISP_POS - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_DISP_POS)>
        Dim nLenDisp() As POINT_LENGTH                          '通板距離[m]
        <VBFixedArray(MAX_DISP_POS - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_DISP_POS)>
        Dim nRowDisp() As POINT_STAGE                           '通板行
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)>
        Dim nRowWrite() As POINT_STAGE                          '現在書き込み行
        <VBFixedArray(MAP_STAGE_NUM - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAP_STAGE_NUM)>
        Dim nRowLast() As Integer                               '検査完了行
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim yobi() As Integer                                   '予備
        'イニシャライズ
        Public Sub initialize()
            ReDim nLenDisp(MAX_DISP_POS - 1)
            For ii As Integer = 0 To nLenDisp.Length - 1
                nLenDisp(ii).initialize()
            Next
            ReDim nRowDisp(MAX_DISP_POS - 1)
            For ii As Integer = 0 To nRowDisp.Length - 1
                nRowDisp(ii).initialize()
            Next
            ReDim nRowWrite(NUM_MEN - 1)
            For ii As Integer = 0 To nRowWrite.Length - 1
                nRowWrite(ii).initialize()
            Next
            ReDim nRowLast(MAP_STAGE_NUM - 1)
            ReDim yobi(2 - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '各レコード単位のポインタ情報[TBL_HY_POINT]
    '-------------------------------------------------------
    Public Const SIZE_POINT_TBL As Integer = SIZE_KIZUKEN_NO + 4 + SIZE_ROW_INFO
    ''' <summary>
    ''' 各レコード単位のポインタ情報
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_POINT_TBL, Pack:=1)>
    Public Structure POINT_TBL
        <VBFixedArray(SIZE_KIZUKEN_NO - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_KIZUKEN_NO)>
        Dim cKizukenNo() As Byte                                '管理No
        Dim emDispStatus As EM_DIV_DSP_STATUS                   '画面表示状態
        Dim typ As POINT_ROW_INFO                               '位置情報
        'イニシャライズ
        Public Sub initialize()
            ReDim cKizukenNo(SIZE_KIZUKEN_NO - 1)

            typ.initialize()
        End Sub
    End Structure

    '-------------------------------------------------------
    'ポインタ情報テーブル[TBL_HY_POINT]
    '-------------------------------------------------------
    Public Const SIZE_TBL_HY_POINT As Integer = 4 + 4 + 4 * NUM_MEN + 4 + 4 * MAX_DISP_POS _
    + 2 * 4 + SIZE_POINT_TBL * TBL_HY_REC_MAX
    ''' <summary>
    ''' ポインタ情報テーブル[TBL_HY_POINT]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_POINT, Pack:=1)>
    Public Structure TBL_HY_POINT
        Dim nRefresh As Integer                                 '更新中フラグ  ※TRUEの時は、[nRecDisp]参照禁止
        Dim nRecWriteBase As Integer                            '書込みレコード
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)>
        Dim nRecWrite() As Integer                              '各検査装置単位での現在レコード
        Dim nRecDispBase As Integer                             '表示基点位置での現在表示レコード
        <VBFixedArray(MAX_DISP_POS - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_DISP_POS)>
        Dim nRecDisp() As Integer                               '各通板ポイントでの 現在レコード
        '<VBFixedArray(MAX_DISP_POS - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_DISP_POS)> _
        'Dim inf() As POINT_INF_NOW								'全体付加情報
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim yobi() As Integer                                   '予備
        <VBFixedArray(TBL_HY_REC_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_REC_MAX)>
        Dim pnt() As POINT_TBL                                  '各レコード単位のポインタ情報
        'イニシャライズ
        Public Sub initialize()
            ReDim nRecWrite(NUM_MEN - 1)
            ReDim nRecDisp(MAX_DISP_POS - 1)
            'ReDim inf(MAX_DISP_POS - 1)
            ReDim yobi(2 - 1)
            ReDim pnt(TBL_HY_REC_MAX - 1)
            For ii As Integer = 0 To pnt.Length - 1
                pnt(ii).initialize()
            Next
        End Sub
    End Structure

    '↓↓↓↓コイル情報テーブル[TBL_HY_COILINF]↓↓↓↓
    '-------------------------------------------------------
    'レコード[TBL_HY_COILINF]
    '-------------------------------------------------------
    Public Const SIZE_COILINF_REC As Integer = SIZE_COINFO_BASE
    ''' <summary>
    ''' レコード[TBL_HY_COILINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_COILINF_REC, Pack:=1)>
    Public Structure COILINF_REC
        Dim coilinf As COINFO_BASE                              'コイル情報共通
    End Structure

    '-------------------------------------------------------
    'コイル情報テーブル[TBL_HY_COILINF]
    '-------------------------------------------------------
    Public Const SIZE_TBL_HY_COILINF As Integer = SIZE_COILINF_REC * TBL_HY_COIL_MAX
    ''' <summary>
    ''' コイル情報テーブル[TBL_HY_COILINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_COILINF, Pack:=1)>
    Public Structure TBL_HY_COILINF
        <VBFixedArray(TBL_HY_COIL_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_COIL_MAX)>
        Dim rec() As COILINF_REC                                'レコード
        'イニシャライズ
        Public Sub initialize()
            ReDim rec(TBL_HY_COIL_MAX - 1)
            For ii As Integer = 0 To rec.Length - 1
                rec(ii).coilinf.initialize()
            Next
        End Sub
    End Structure

    '↓↓↓↓コイル実績テーブル[TBL_HY_COILRESULT]↓↓↓↓
    '-------------------------------------------------------
    'カット情報[TBL_HY_COILRESULT]
    '-------------------------------------------------------
    Public Const SIZE_TO_SO_DATA_CUT As Integer = 4 + 4
    ''' <summary>
    ''' カット情報[TBL_HY_COILRESULT]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TO_SO_DATA_CUT, Pack:=1)>
    Public Structure TO_SO_DATA_CUT
        Dim nMode As Integer                                    '種類(1:ファーストカット 2:ラストカット 3:分割 4:異種点)
        Dim nPos As Integer                                     '位置[mm]
        'イニシャライズ
    End Structure

    '-------------------------------------------------------
    'レコード[TBL_HY_COILRESULT]
    '-------------------------------------------------------
    Public Const SIZE_COILRESULT_REC As Integer = 4 + 4 + SIZE_TO_SO_DATA_CUT * MAX_SHACUT_COUNT
    ''' <summary>
    ''' レコード[TBL_HY_COILRESULT]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_COILRESULT_REC, Pack:=1)>
    Public Structure COILRESULT_REC
        Dim nLen As Integer                                     '検査実測長[mm]
        Dim nCutCnt As Integer                                  'カット情報セット数
        <VBFixedArray(MAX_SHACUT_COUNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_SHACUT_COUNT)>
        Dim cut() As TO_SO_DATA_CUT                             'カット情報
        'イニシャライズ
        Public Sub initialize()
            ReDim cut(MAX_SHACUT_COUNT - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    'コイル実績テーブル[TBL_HY_COILRESULT]
    '-------------------------------------------------------
    Public Const SIZE_TBL_HY_COILRESULT As Integer = SIZE_COILRESULT_REC * TBL_HY_COIL_MAX
    ''' <summary>
    ''' コイル実績テーブル[TBL_HY_COILRESULT]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_COILRESULT, Pack:=1)>
    Public Structure TBL_HY_COILRESULT
        <VBFixedArray(TBL_HY_COIL_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_COIL_MAX)>
        Dim rec() As COILRESULT_REC                             'レコード
        'イニシャライズ
        Public Sub initialize()
            ReDim rec(TBL_HY_COIL_MAX - 1)
            For ii As Integer = 0 To rec.Length - 1
                rec(ii).initialize()
            Next
        End Sub
    End Structure

    '↓↓↓↓マップ情報テーブル[TBL_HY_MAP_COLROW]↓↓↓↓
    '-------------------------------------------------------
    'マップ情報テーブル[TBL_HY_MAP_COLROW]
    '-------------------------------------------------------
    'Public Const SIZE_TBL_HY_MAP_COLROW As Integer = SIZE_DSP_MAP_COLROW * TBL_HY_REC_MAX
    '''' <summary>
    '''' マップ情報テーブル[TBL_HY_MAP_COLROW]
    '''' </summary>
    '''' <remarks></remarks>
    '<StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_MAP_COLROW, pack:=1)> _
    'Public Structure TBL_HY_MAP_COLROW
    '	<VBFixedArray(TBL_HY_REC_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_REC_MAX)> _
    '	Dim rec() As DSP_MAP_COLROW								'レコード
    '	'イニシャライズ
    '	Public Sub initialize()
    '		ReDim rec(TBL_HY_REC_MAX - 1)
    '		For ii As Integer = 0 To rec.Length - 1
    '			rec(ii).initialize()
    '		Next
    '	End Sub
    'End Structure

    '↓↓↓↓マップ位置テーブル[TBL_HY_MAP_ROW]↓↓↓↓
    '-------------------------------------------------------
    'マップ位置テーブル[TBL_HY_MAP_ROW]
    '-------------------------------------------------------
    Public Const SIZE_TBL_HY_MAP_ROW As Integer = SIZE_DSP_MAP_ROW * TBL_HY_REC_MAX
    ''' <summary>
    ''' マップ位置テーブル[TBL_HY_MAP_ROW]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_MAP_ROW, Pack:=1)>
    Public Structure TBL_HY_MAP_ROW
        <VBFixedArray(TBL_HY_REC_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_REC_MAX)>
        Dim rec() As DSP_MAP_ROW                                'レコード
        'イニシャライズ
        Public Sub initialize()
            ReDim rec(TBL_HY_REC_MAX - 1)
            For ii As Integer = 0 To rec.Length - 1
                rec(ii).initialize()
            Next
        End Sub
    End Structure

    '↓↓↓↓疵情報テーブル[TBL_HY_DEFECTINF]↓↓↓↓
    '-------------------------------------------------------
    '疵情報テーブル[TBL_HY_DEFECTINF]
    '-------------------------------------------------------
    Public Const SIZE_TBL_HY_DEFECTINF As Integer = SIZE_DSP_MAP_DEFECTINF * TBL_HY_REC_MAX
    ''' <summary>
    ''' 疵情報テーブル[TBL_HY_DEFECTINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_DEFECTINF, Pack:=1)>
    Public Structure TBL_HY_DEFECTINF
        <VBFixedArray(TBL_HY_REC_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_REC_MAX)>
        Dim rec() As DSP_MAP_DEFECTINF                          'レコード
        'イニシャライズ
        Public Sub initialize()
            ReDim rec(TBL_HY_REC_MAX - 1)
            For ii As Integer = 0 To rec.Length - 1
                rec(ii).initialize()
            Next
        End Sub
    End Structure

    '↓↓↓↓重欠陥一覧表示ポインタ情報テーブル[TBL_HY_DEFECTINF]↓↓↓↓
    '-------------------------------------------------------
    '疵情報[TBL_HY_DEFLIST]
    '-------------------------------------------------------
    Public Const SIZE_DSP_LIST_DEFECT As Integer = SIZE_KIZUKEN_NO + SIZE_COIL_NO + 1 + SIZE_MEI_NO + 1 + 2 + SIZE_DSP_TYP_DEFECTINF
    ''' <summary>
    ''' 疵情報
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_LIST_DEFECT, Pack:=1)>
    Public Structure DSP_LIST_DEFECT 'DSP_DATA_DEF_LIST
        <VBFixedString(SIZE_KIZUKEN_NO), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=SIZE_KIZUKEN_NO)>
        Dim cKanriNo As String                                  ' 管理NO
        <VBFixedArray(SIZE_COIL_NO - 1 + 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_COIL_NO + 1)>
        Dim cCoilNo() As Byte                                   ' 生産番号
        <VBFixedArray(SIZE_MEI_NO - 1 + 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_MEI_NO + 1)>
        Dim cMeiNo() As Byte                                    ' 命令No
        <VBFixedString(2 - 1), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=2)>
        Dim cYobi1 As String                                    ' 予備
        Dim typ As DSP_TYP_DEFECTINF                            ' 表示疵情報[TBL_HY_DEFECTINF]

        'イニシャライズ
        Public Sub initialize()
            ReDim cCoilNo(SIZE_COIL_NO - 1 + 1)
            ReDim cMeiNo(SIZE_COIL_NO - 1 + 1)
            typ.initialize()
        End Sub

    End Structure

    '-------------------------------------------------------
    'データ部[TBL_HY_DEFLIST]
    '-------------------------------------------------------
    Public Const SIZE_DSP_LIST_DATA As Integer = SIZE_DSP_LIST_DEFECT * HY_DEFLIST_REC_MAX
    ''' <summary>
    ''' データ部
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_LIST_DATA, Pack:=1)>
    Public Structure DSP_LIST_DATA

        <VBFixedArray(HY_DEFLIST_REC_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=HY_DEFLIST_REC_MAX)>
        Dim DfData() As DSP_LIST_DEFECT                         '重欠陥一覧表示疵情報

        Public Sub initialize()
            ReDim DfData(HY_DEFLIST_REC_MAX - 1)
        End Sub
    End Structure


    '-------------------------------------------------------
    'ヘッダ部[TBL_HY_DEFLIST]
    '-------------------------------------------------------
    Public Const SIZE_DSP_LIST_HEADER As Integer = 4 * 2 + 24
    ''' <summary>
    ''' ヘッダ部
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure DSP_LIST_HEADER
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)>
        Dim nPos() As Integer                                   '最新レコード位置
        <VBFixedArray(24 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=24)>
        Dim cYobi() As Byte                                     '予備

        Public Sub initialize()
            ReDim nPos(NUM_MEN - 1)
            ReDim cYobi(24 - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    'レコード[TBL_HY_DEFLIST]
    '-------------------------------------------------------
    Public Const SIZE_DSP_LIST As Integer = SIZE_DSP_LIST_HEADER + SIZE_DSP_LIST_DATA * NUM_MEN
    ''' <summary>
    ''' レコード
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_LIST, Pack:=1)>
    Public Structure DSP_LIST
        Dim head As DSP_LIST_HEADER                             'ヘッダ部
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)>
        Dim data() As DSP_LIST_DATA                             'データ部
        Public Sub initialize()
            ReDim data(NUM_MEN - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '重欠陥一覧表示ポインタ情報テーブル[TBL_HY_DEFLIST]
    '-------------------------------------------------------
    Public Const SIZE_TBL_HY_DEFLIST As Integer = SIZE_DSP_LIST
    ''' <summary>
    ''' 重欠陥一覧表示ポインタ情報テーブル
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_DEFLIST, Pack:=1)>
    Public Structure TBL_HY_DEFLIST
        Dim rec As DSP_LIST                                     'レコード
    End Structure


    '↓↓↓↓前工程疵情報テーブル[TBL_HY_MAE_INF]↓↓↓↓
    '-------------------------------------------------------
    '前工程検査装置情報[TBL_HY_MAE_INF]
    '-------------------------------------------------------
    Public Const SIZE_DSP_TYP_MAE_DEFECTINF As Integer = 52
    ''' <summary>
    ''' 前工程検査装置情報
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_TYP_MAE_DEFECTINF, Pack:=1)>
    Public Structure DSP_TYP_MAE_DEFECTINF

        Dim nImageFlg As Short                                  ' 画像形式 (0:Jpeg 1:BMP)
        Dim nDspMode As Short                                   ' 事前編集フラグ (-1:非表示  0:通常表示  1:強調表示) ※プロビジョン

        Dim men As UShort                                       ' 表裏区分  (0:表 1:裏)
        Dim cycle_div As UShort                                 ' 周期疵区分(0:一般疵, 1:簡易周期疵,連続疵 2:周期欠陥)
        Dim dsws As UShort                                      ' DSWS区分  (0:DS 1:WS)
        Dim yobi As UShort

        '// 長手、幅位置は、FF情報を使用
        Dim speed As Integer                                    ' 速度[mpm]
        Dim pitch As Integer                                    ' ピッチ[mm]
        Dim roolkei As Single                                   ' ロール径[mm]
        Dim box_wid As Short                                    ' 疵幅 [mm]
        Dim box_len As Short                                    ' 疵長さ [mm]

        '// 画像表示に必要
        Dim h_res As Single                                     ' 縦分解能[mm/pixel]
        Dim w_res As Single                                     ' 横分解能[mm/pixel]
        Dim box_x_min As Short                                  ' 外接Xmin[pixel]		(切り出し画像の左上からの位置) (マイナス値ありえる)
        Dim box_x_max As Short                                  ' 外接Xmax[pixel]
        Dim box_y_min As Short                                  ' 外接Ymin[pixel]
        Dim box_y_max As Short                                  ' 外接Ymax[pixel]
        Dim center_x As Short                                   ' 疵中心位置X [pixel]  (切り出し画像の左上からの位置)
        Dim center_y As Short                                   ' 疵中心位置Y [pixel]
        Dim h_size As UShort                                    ' 縦画素数[pixel]
        Dim w_size As UShort                                    ' 横画素数[pixel]

    End Structure


    '-------------------------------------------------------
    '前工程疵画像[TBL_HY_MAE_INF]
    '-------------------------------------------------------
    Public Const SIZE_DSP_TYP_MAE_KIZUINF As Integer = 256
    ''' <summary>
    ''' 前工程疵画像
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_TYP_MAE_KIZUINF, Pack:=1)>
    Public Structure DSP_TYP_MAE_KIZUINF
        Dim data As DSP_TYP_MAE_DEFECTINF                       ' 前工程疵情報

        <VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)>
        Dim image_path_1 As String                              ' 画像のパス1 (画像無し時はALL NULL)
        <VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)>
        Dim image_path_2 As String                              ' 画像のパス2 (画像無し時はALL NULL)
        <VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)>
        Dim image_path_3 As String                              ' 画像のパス3 (画像無し時はALL NULL)
        <VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)>
        Dim image_path_4 As String                              ' 画像のパス4 (画像無し時はALL NULL)

        <VBFixedArray(76 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=76)>
        Dim reserve() As Byte

        Public Sub initialize()
            ReDim reserve(76 - 1)
        End Sub
    End Structure

	'-------------------------------------------------------
	'FF情報[TBL_HY_MAE_INF]
	'-------------------------------------------------------
	'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
	'Public Const SIZE_DSP_TYP_MAE_FFINF As Integer = SIZE_KIZUKEN_NO + 4 + 2 + 2 + 2 + 2 + 4 + 4 + 4 + 4 + 4 + 2 + 2
	'Public Const SIZE_DSP_TYP_MAE_FFINF As Integer = SIZE_KIZUKEN_NO + 4 + 2 + 2 + 2 + 2 + 4 + 4 + 4 + 4 + 4 + 2 + 2 + 4
	'' 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

	Public Const SIZE_DSP_TYP_MAE_FFINF As Integer = SIZE_KIZUKEN_NO + 4 + 2 + 2 + 2 + 2 + 4 + 4 + 4 + 4 + 4 + 4 + 2 + 2 + 4
	'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
    ''' <summary>
    ''' FF情報
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_TYP_MAE_FFINF, Pack:=1)>
    Public Structure DSP_TYP_MAE_FFINF

        <VBFixedArray(SIZE_KIZUKEN_NO - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_KIZUKEN_NO)>
        Dim cMaeKizukenNo() As Byte                             ' FF管理No (前工程φωでの管理No)

        Dim nMaeKizuNo As Integer                               ' FF疵No	   (前工程疵検での疵No)

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim cT() As Byte                                        ' FF疵種

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim cG() As Byte                                        ' FFグレード

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim cProc() As Byte                                     ' FF疵検出工程

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim yobi1() As Byte

        Dim nDiv As Integer                                     ' FF疵区分
        Dim nPos As Integer                                     ' FF疵長さ方向位置 [mm]	※これは使わない
        Dim nWidth As Integer                                   ' FF疵巾方向位置 [mm]		(WSからの位置)
        Dim nLen As Integer                                     ' FF疵長さ [mm]

        '// 拡張
        Dim nY As Integer                                       ' 最終長手位置 [mm]
		'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
		Dim nY_Out As Integer                                   ' 最終出側長手位置 [mm]
		'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
		
        Dim nTid As Short                                       ' MAE_TYPE_NAME  の ID  (0:不明   1～：前工程の対象疵) ※不明の場合は、FF情報で表示。マスター未定義や
        Dim nGid As Short                                       ' MAE_GRADE_NAME の ID  (0:不明)

        ' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
        Dim nKouteiID As Integer                                ' 工程種別
        ' 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

        Public Sub initialize()
            ReDim cMaeKizukenNo(SIZE_KIZUKEN_NO - 1)
            ReDim cT(2 - 1)
            ReDim cG(2 - 1)
            ReDim cProc(2 - 1)
            ReDim yobi1(2 - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '前工程疵情報[TBL_HY_MAE_INF]
    '-------------------------------------------------------
    Public Const SIZE_MAE_INF As Integer = 4 + SIZE_DSP_TYP_MAE_FFINF + SIZE_DSP_TYP_MAE_KIZUINF + 4
    ''' <summary>
    ''' 前工程疵情報
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_MAE_INF, Pack:=1)>
    Public Structure MAE_INF
        Dim nMaeDsp As Integer                                  '前工程疵の表示種類
        Dim ff As DSP_TYP_MAE_FFINF                             'FF情報
        Dim inf As DSP_TYP_MAE_KIZUINF                          '前工程疵情報
        Dim nKizuNo As Integer                                  '自工程DB 疵No
        Public Sub initialize()
            ff.initialize()
            inf.initialize()
       End Sub
    End Structure

    '-------------------------------------------------------
    '面情報[TBL_HY_MAE_INF]
    '-------------------------------------------------------
    Public Const SIZE_MEN As Integer = 4 + SIZE_MAE_INF * TBL_HY_MAE_INF_MAX
    ''' <summary>
    ''' 面情報
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_MEN, Pack:=1)>
    Public Structure MEN
        Dim num As Integer                                      '現在の格納件数
        <VBFixedArray(TBL_HY_MAE_INF_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_MAE_INF_MAX)>
        Dim maeinf() As MAE_INF                                 '前工程疵情報
        Public Sub initialize()
            ReDim maeinf(TBL_HY_MAE_INF_MAX - 1)
            For ii As Integer = 0 To maeinf.Length - 1
                maeinf(ii).initialize()
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    'レコード[TBL_HY_MAE_INF]
    '-------------------------------------------------------
    Public Const SIZE_REC As Integer = SIZE_MEN * NUM_MEN
    ''' <summary>
    ''' レコード
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_REC, Pack:=1)>
    Public Structure REC
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)>
        Dim men() As MEN                                    '面情報
        'イニシャライズ
        Public Sub initialize()
            ReDim men(NUM_MEN - 1)
            For ii As Integer = 0 To men.Length - 1
                men(ii).initialize()
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    '前工程疵情報[TBL_HY_MAE_INF]
    '-------------------------------------------------------
	'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>	
	'Public Const SIZE_TBL_HY_MAE_INF As Integer = SIZE_MEN * TBL_HY_REC_MAX
	Public Const SIZE_TBL_HY_MAE_INF As Integer = SIZE_REC * TBL_HY_REC_MAX
	'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
    ''' <summary>
    ''' 前工程疵情報
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_MAE_INF, Pack:=1)>
    Public Structure TBL_HY_MAE_INF
        <VBFixedArray(TBL_HY_REC_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_REC_MAX)>
        Dim rec() As REC                                          'レコード
        'イニシャライズ
        Public Sub initialize()
            ReDim rec(TBL_HY_REC_MAX - 1)
            For ii As Integer = 0 To rec.Length - 1
                rec(ii).initialize()
            Next
        End Sub
    End Structure


    '↓↓↓↓警報情報テーブル[TBL_HY_ALARM]↓↓↓↓
    '-------------------------------------------------------
    '列単位[TBL_HY_ALARM]
    '-------------------------------------------------------
    Public Const SIZE_DSP_ALARM_DATA As Integer = 4 * MAP_COL_NUM
    ''' <summary>
    ''' 列単位
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_ALARM_DATA, Pack:=1)>
    Public Structure DSP_ALARM_DATA
        <VBFixedArray(MAP_COL_NUM - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAP_COL_NUM)>
        Dim row() As Integer                '列

        Public Sub initialize()
            ReDim row(MAP_COL_NUM - 1)
        End Sub

    End Structure

    '-------------------------------------------------------
    '表裏単位[TBL_HY_ALARM]
    '-------------------------------------------------------
    Public Const SIZE_DSP_TYP_ALARM_TORB As Integer = SIZE_DSP_ALARM_DATA * NUM_MEN
    ''' <summary>
    ''' 表裏単位
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_TYP_ALARM_TORB, Pack:=1)>
    Public Structure DSP_TYP_ALARM_TORB
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)>
        Dim torb() As DSP_ALARM_DATA        '表裏
        Public Sub initialize()
            ReDim torb(NUM_MEN - 1)
            For ii As Integer = 0 To torb.Length - 1
                torb(ii).initialize()
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    '検査台単位[TBL_HY_ALARM]
    '-------------------------------------------------------
    Public Const SIZE_DSP_TYP_ALARM_KENSADAI As Integer = SIZE_DSP_TYP_ALARM_TORB * 2
    ''' <summary>
    ''' 検査台単位
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_ALARM, Pack:=1)>
    Public Structure DSP_TYP_ALARM_KENSADAI
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim kendai() As DSP_TYP_ALARM_TORB       '検査台別
        Public Sub initialize()
            ReDim kendai(2 - 1)
            For ii As Integer = 0 To kendai.Length - 1
                kendai(ii).initialize()
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    '警報情報テーブル[TBL_HY_ALARM]
    '-------------------------------------------------------
    Public Const SIZE_TBL_HY_ALARM As Integer = SIZE_DSP_TYP_ALARM_KENSADAI
	''' <summary>
	''' 警報情報テーブル
	''' </summary>
	''' <remarks></remarks>
	<StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_ALARM, Pack:=1)>
	Public Structure TBL_HY_ALARM
		Dim a_mode As DSP_TYP_ALARM_KENSADAI       '警報フラグ
		Public Sub initialize()
			a_mode.initialize()
		End Sub
	End Structure

	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	'-------------------------------------------------------
	'長周期情報テーブル[TBL_HY_LONGPD]
	'-------------------------------------------------------
	Public Const SIZE_TBL_HY_LONGPD_REC As Integer = 4
	Public Const SIZE_TBL_HY_LONGPD As Integer = 4 + SIZE_TBL_HY_LONGPD_REC * TBL_HY_COIL_MAX

	<StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_LONGPD_REC, Pack:=1)>
	Public Structure TBL_HY_LONGPD_REC
		Dim nLongPdOccurrenceFlag As Integer
	End Structure

	<StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_LONGPD, Pack:=1)>
	Public Structure TBL_HY_LONGPD

		'// 全体ステータス
		Dim nGuidanceFlag As Integer                '// ガイダンス表示フラグ(0:表示なし 1:表示あり) ※現コイル、前コイルで判定

		'// コイル単位
		<VBFixedArray(TBL_HY_COIL_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_COIL_MAX)>
		Dim rec() As TBL_HY_LONGPD_REC

		Public Sub initialize()
			ReDim rec(TBL_HY_COIL_MAX - 1)
		End Sub
	End Structure
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

End Module