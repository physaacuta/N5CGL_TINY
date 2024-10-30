'*******************************************************************************
'   疵マップ表示
'
'   [Ver]
'       Ver.01  2013/12/25  初版
'
'   [メモ]
'       tMod_MemSet.MemSet関数を使用しているため
'*******************************************************************************
Option Strict On
Imports System.Threading

Imports KizuLibCLI.KizuMod
Imports KizuForm

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports tClassLibrary.tClass.tClass_LogManager
Imports System.Reflection

''' <summary>
''' 疵マップ表示クラス
''' </summary>
''' <remarks></remarks>
Public Class clsDispMap
    Implements IDisposable
    '///////////////////////////////////////////////////////////////////////////
    '定数定義
    '///////////////////////////////////////////////////////////////////////////
    Private Const MAX_MAPDEFLST_ROW As Integer = 3         ' 疵リスト最大表示数


    '///////////////////////////////////////////////////////////////////////////
    '列挙体
    '///////////////////////////////////////////////////////////////////////////
    Public Enum EM_DSP_KIZU_INF
        DSP_JI1 = 0
        DSP_JI2
    End Enum

    ''' <summary>
    ''' 疵マップ疵リスト表示列
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_DEEP_LIST_COL
        emNo = 0                    ' No
        emGr                        ' Gr
        emType                      ' 疵名
        emEnd
    End Enum

    ''' <summary>
    ''' 周期疵マップ疵リスト表示列
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_CYC_LIST_COL
        emNo = 0                    ' No
        emGr                        ' Gr
        emType                      ' 疵名
        emEnd
    End Enum

    ''' <summary>
    ''' TCM疵リスト表示列
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_TCM_LIST_COL
        emNo = 0                    ' No
        emLine                      ' ライン名
        emGr                        ' Gr
        emType                      ' 疵名
        emEnd
    End Enum

    ''' <summary>
    ''' 疵の選択位置
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_SELECT_POS
        emKensaAto = 0              ' 検査台後
        emKensaMae                  ' 検査台前
        emEnd
    End Enum

    '///////////////////////////////////////////////////////////////////////////
    '構造体
    '///////////////////////////////////////////////////////////////////////////
    'マップ情報
    Public Structure TYP_MAP_INF
        Dim dgvMap As tCnt_DataGridView_turn                '疵マップ
        Dim dgvDeep As tCnt_DataGridView_turn               '疵深さリスト
        Dim dgvTcm As tCnt_DataGridView_turn                'TCMリスト
        Dim vsbMap As tCnt_VScrollBar                       'スクロールバー
        Dim lblKensaPoint As Label                          '検査台
    End Structure


    '///////////////////////////////////////////////////////////////////////////
    'メンバ変数
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                        'リソース開放フラグ[True:開放済,False:未開放]
    Private m_emStage As EM_LONG_POS                                '長手間隔(1:小、2:大)
    Private m_emMapDir As EM_MAP_SCROOL                             'マップの進行方向
    Private m_emSide As EM_MAP_SIDE                                 '疵ラベルDS位置
    Private m_DSPos As EM_MAP_CELL                                  '疵マップDS位置[NORMAL:左がDS、REVERSE:左がWS]
    Private m_nDspMen As Integer                                    '表示面
	'// 2023.11.17 最終工程検査装置高機能化その２ -------------------->>>>>	
	Private m_emWidthNo As EM_MAP_WIDTH_NO                          ' マップ幅位置番号[NORMAL:左から1,2,・・・、REVERSE:右から1,2,・・・]
	'// 2023.11.17 最終工程検査装置高機能化その２ <<<<<--------------------

    Private m_emViewMode As EM_REFRESH_MODE                         '疵マップ更新モード
    Private m_nKensaNo As Integer                                   '検査台番号
    Private m_bKensaDsp As Boolean                                  '検査台表示有無
    Private m_emMapMode As EM_DEFECT_MODE                           '疵マップ表示モード
    ' 20201010 検査台位置表示変更 --->>>
    Private m_nKensaPos As Integer                                  '検査台表示位置
    ' 20201010 検査台位置表示変更 <<<---

    Private m_typMapInf As TYP_MAP_INF                              '疵マップ
    Private mcls_Log As tClass_LogManager                           'ログ管理
    Private mcls_Param As clsParamManager                           'パラメータ管理
    Private mcls_PreParam As clsPreParamManager                     '前工程パラメータ管理
    Private mcls_PointInf As clsPointInf                            'ポインタ情報
    Private mcls_CoilInf As clsDispCoil                             'コイル情報

	Private m_nDispRec(TBL_HY_REC_MAX - 1) As Integer               '表示対象レコード

	Private m_typSelectValue(DSP_DEFECT_MAX - 1) As TYP_MAP_LIST_VALUE   '選択疵
    Private m_typTcmSelectValue As TYP_MAP_LIST_VALUE               'TCM選択疵

    Private m_nMaxRowCnt As Integer                                 '最大行数

    Private m_posSelectValue(DSP_DEFECT_MAX - 1) As EM_SELECT_POS   '選択疵の検査台位置

    Private m_bcheck As Boolean = False
    '///////////////////////////////////////////////////////////////////////////
    'プロパティ
    '///////////////////////////////////////////////////////////////////////////
#Region "プロパティ"

    ''' <summary>
    ''' 表示面
    ''' </summary>
    ''' <value>0:表面、1:裏面</value>
    ''' <returns>0:表面、1:裏面</returns>
    ''' <remarks></remarks>
    Public Property Men() As Integer
        Get
            Return m_nDspMen
        End Get
        Set(ByVal value As Integer)
            m_nDspMen = value
        End Set
    End Property

    ''' <summary>
    ''' 表示間隔
    ''' </summary>
    ''' <value>SMALL(1):長手間隔小 BIG(2):長手間隔大</value>
    ''' <returns>SMALL(1):長手間隔小 BIG(2):長手間隔大</returns>
    ''' <remarks></remarks>
    Public Property Stage() As EM_LONG_POS
        Get
            Return m_emStage
        End Get
        Set(ByVal value As EM_LONG_POS)
            m_emStage = value
        End Set
    End Property

    ''' <summary>
    ''' マップ進行方向
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property MapDir() As EM_MAP_SCROOL
        Get
            Return m_emMapDir
        End Get
        Set(ByVal value As EM_MAP_SCROOL)
            m_emMapDir = value
        End Set
    End Property

    ''' <summary>
    ''' 疵マップDS位置
    ''' </summary>
    ''' <value>NORMAL:左がDS、REVERSE:左がWS</value>
    ''' <returns>NORMAL:左がDS、REVERSE:左がWS</returns>
    ''' <remarks></remarks>
    Public Property DSPos() As EM_MAP_CELL
        Get
            Return m_DSPos
        End Get
        Set(ByVal value As EM_MAP_CELL)
            m_DSPos = value
			'// 2023.11.17 最終工程検査装置高機能化その２ -------------------->>>>>			
			'If EM_MAP_CELL.NORMAL = m_DSPos Then
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_2).HeaderText = "2"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_3).HeaderText = "3"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_4).HeaderText = "4"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_5).HeaderText = "5"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_6).HeaderText = "6"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_7).HeaderText = "7"
			'Else
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_2).HeaderText = "7"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_3).HeaderText = "6"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_4).HeaderText = "5"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_5).HeaderText = "4"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_6).HeaderText = "3"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_7).HeaderText = "2"
			'End If
			'// 2023.11.17 最終工程検査装置高機能化その２ <<<<<--------------------
        End Set
    End Property

    ''' <summary>
    ''' ラベルDS位置
    ''' </summary>
    ''' <value>NORMAL:左がDS、REVERSE:左がWS</value>
    ''' <returns>NORMAL:左がDS、REVERSE:左がWS</returns>
    ''' <remarks></remarks>
    Public Property Side() As EM_MAP_SIDE
        Get
            Return m_emSide
        End Get
        Set(ByVal value As EM_MAP_SIDE)
            m_emSide = value
            If m_emSide = EM_MAP_SIDE.NORMAL Then
                m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_1).HeaderText = DIRECTION_NAME_DS
                m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_8).HeaderText = DIRECTION_NAME_WS
            Else
                m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_1).HeaderText = DIRECTION_NAME_WS
                m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_8).HeaderText = DIRECTION_NAME_DS
            End If
        End Set
    End Property

	'// 2023.11.17 最終工程検査装置高機能化その２ -------------------->>>>>	
	''' <summary>
	''' マップ番号位置
	''' </summary>
	''' <value>[NORMAL:左から1,2,・・・、REVERSE:右から1,2,・・・]</value>
	''' <returns>[NORMAL:左から1,2,・・・、REVERSE:右から1,2,・・・]</returns>
	''' <remarks></remarks>
	Public Property WidthNo() As EM_MAP_WIDTH_NO
		Get
			Return m_emWidthNo
		End Get
		Set(ByVal value As EM_MAP_WIDTH_NO)
			m_emWidthNo = value
			If m_emWidthNo = EM_MAP_SIDE.NORMAL Then
				Dim nAddr As Integer = EM_MAP_COL_INF.COL_2
				For ii As Integer = EM_MAP_COL_INF.COL_2 To EM_MAP_COL_INF.COL_7
					m_typMapInf.dgvMap.Columns(ii).HeaderText = nAddr.ToString
					nAddr = nAddr + 1
				Next
			Else
				Dim nAddr As Integer = EM_MAP_COL_INF.COL_7
				For ii As Integer = EM_MAP_COL_INF.COL_2 To EM_MAP_COL_INF.COL_7
					m_typMapInf.dgvMap.Columns(ii).HeaderText = nAddr.ToString
					nAddr = nAddr - 1
				Next
			End If
		End Set
	End Property
	'// 2023.11.17 最終工程検査装置高機能化その２ <<<<<--------------------

    ''' <summary>
    ''' 通板ポイント位置
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property KensaNo() As Integer
        Get
            Return m_nKensaNo
        End Get
        Set(ByVal value As Integer)
            m_nKensaNo = value
        End Set
    End Property

    ''' <summary>
    ''' 検査台表示有無
    ''' </summary>
    ''' <value>検査台位置</value>
    ''' <returns>検査台位置</returns>
    ''' <remarks></remarks>
    Public Property KensaDsp() As Boolean
        Get
            Return m_bKensaDsp
        End Get
        Set(ByVal value As Boolean)
            m_bKensaDsp = value
        End Set
    End Property

    ' 20201010 検査台位置表示変更 --->>>
    ''' <summary>
    ''' 検査台表示行
    ''' </summary>
    ''' <value>1～50:検査台位置</value>
    ''' <returns>1～50:検査台位置</returns>
    ''' <remarks></remarks>
    Public Property KensaPos() As Integer
        Get
            Return m_nKensaPos
        End Get
        Set(ByVal value As Integer)
            m_nKensaPos = value
        End Set
    End Property
    ' 20201010 検査台位置表示変更 <<<---

    ''' <summary>
    ''' 画面更新モード
    ''' </summary>
    ''' <value>AUTO(0):自働更新モード PAUSE(1):一時停止モード</value>
    ''' <returns>AUTO(0):自働更新モード PAUSE(1):一時停止モード</returns>
    ''' <remarks></remarks>
    Public Property RefreshMode() As EM_REFRESH_MODE
        Get
            Return m_emViewMode
        End Get
        Set(ByVal value As EM_REFRESH_MODE)
            m_emViewMode = value
            m_typMapInf.dgvMap.Refresh()
        End Set
    End Property

    ''' <summary>
    ''' 調査用F01ボタン押されたフラグ
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property checkcheck() As Boolean
        Get
            Return m_bcheck
        End Get
        Set(ByVal value As Boolean)
            m_bcheck = value
        End Set
    End Property

    ''' <summary>
    ''' 疵マップ更新モード
    ''' </summary>
    ''' <value>TRACKING(0):トラッキング疵画像表示 REALTIME(1):リアルタイム疵画像表示</value>
    ''' <returns>TRACKING(0):トラッキング疵画像表示 REALTIME(1):リアルタイム疵画像表示</returns>
    ''' <remarks></remarks>
    Public Property MapUpdateMode() As EM_DEFECT_MODE
        Get
            Return m_emMapMode
        End Get
        Set(ByVal value As EM_DEFECT_MODE)
            m_emMapMode = value
        End Set
    End Property

	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	''' <summary>
	''' マップ表示対象レコード
	''' </summary>
	''' <returns></returns>
	Public ReadOnly Property MapDispRec() As Integer()
		Get
			Return m_nDispRec
		End Get
	End Property
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

#End Region

	'///////////////////////////////////////////////////////////////////////////
	'イベント
	'///////////////////////////////////////////////////////////////////////////
#Region "イベント"
	''' <summary>
	''' 
	''' </summary>
	''' <param name="nMen"></param>
	''' <param name="pntKizu"></param>
	''' <param name="emInf"></param>
	''' <remarks></remarks>
	Public Event SelectMapCell_Auto(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE, ByVal emInf As EM_DSP_KIZU_INF)

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="emInf"></param>
    ''' <remarks></remarks>
    Public Event SelectMapCell_AutoDspInit(ByVal emInf As EM_DSP_KIZU_INF)

    '''' <summary>
    '''' 
    '''' </summary>
    '''' <param name="nMen"></param>
    '''' <param name="pntKizu"></param>
    '''' <remarks></remarks>
    'Public Event SelectMapCell_Click(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)
    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="nMen"></param>
    ''' <param name="pntKizu"></param>
    ''' <param name="bSelect"></param>
    ''' <remarks></remarks>
    Public Event SelectMapCell_Click(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE, ByVal bSelect As Boolean)

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="nMen"></param>
    ''' <param name="pntKizu"></param>
    ''' <remarks></remarks>
    Public Event SelectTcmMapCell_Auto(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <remarks></remarks>
    Public Event SelectTcmMapCell_AutoDspInit()
    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="nMen"></param>
    ''' <param name="pntKizu"></param>
    Public Event SelectMapTcmCell_Click(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)

#End Region


    '///////////////////////////////////////////////////////////////////////////
    'メンバ関数
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="typMapInf"></param>
    ''' <param name="pointer"></param>
    ''' <param name="coil"></param>
    ''' <param name="param"></param>
    ''' <param name="preparam"></param>
    ''' <param name="log"></param>
    ''' <param name="nFormID"></param>
    Public Sub New(ByVal typMapInf As TYP_MAP_INF, ByRef param As clsParamManager, ByRef preparam As clsPreParamManager, ByRef log As tClass_LogManager, ByRef pointer As clsPointInf, ByRef coil As clsDispCoil, ByVal nFormID As Integer)
        Try
            m_typMapInf = typMapInf
            mcls_Param = param
            mcls_PreParam = preparam
            mcls_PointInf = pointer
            mcls_CoilInf = coil
            mcls_Log = log


            For ii As Integer = 0 To m_typSelectValue.Length - 1
                m_typSelectValue(ii).Initialize()
            Next
            m_typTcmSelectValue.Initialize()

            '疵マップグリッド
            dgvInitialize(m_typMapInf.dgvMap)
            m_typMapInf.dgvMap.bTanspose = False
            InitdgvMap(m_typMapInf.dgvMap)
            Side = EM_MAP_SIDE.NORMAL
            DSPos = EM_MAP_CELL.NORMAL
            AddHandler m_typMapInf.dgvMap.Paint, AddressOf dgvMap_Paint
            AddHandler m_typMapInf.dgvMap.CellMouseDown, AddressOf dgvMap_CellMouseDown

            '疵リストグリッド
            InitdgvDeep(m_typMapInf.dgvDeep)
            m_typMapInf.dgvDeep.bTanspose = False
            AddHandler m_typMapInf.dgvDeep.Paint, AddressOf dgvDeep_Paint
            AddHandler m_typMapInf.dgvDeep.CellMouseDown, AddressOf dgvDeep_CellMouseDown

            'TCMリストグリッド
            InitdgvTcm(m_typMapInf.dgvTcm)
            m_typMapInf.dgvTcm.bTanspose = False
            'AddHandler m_typMapInf.dgvTcm.Paint, AddressOf dgvTcm_Paint
            AddHandler m_typMapInf.dgvTcm.CellMouseDown, AddressOf dgvTcm_CellMouseDown

            m_typMapInf.vsbMap.SmallChange = 1
            m_typMapInf.vsbMap.LargeChange = m_typMapInf.dgvMap.RowCount

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("疵マップ表示生成異常[{0}]", ex.Message))
        End Try
    End Sub

#Region " IDisposable Support "
    ''' <summary>
    ''' デストラクタ
    ''' </summary>
    ''' <param name="disposing">True:明示的な呼び出し False:暗黙の呼び出し</param>
    ''' <remarks></remarks>
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        Try
            If Not m_bIsDesposed Then
                'オブジェクト破棄
            End If
            m_bIsDesposed = True                                                'Dispose済みとする
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("オブジェクト破棄異常[{0}]", ex.Message))
        End Try
    End Sub

    'このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
    Public Sub Dispose() Implements IDisposable.Dispose
        'このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

    ''' <summary>
    ''' 疵マップデータグリッド初期化
    ''' </summary>
    ''' <param name="dgvCtrl">疵マップデータグリッド</param>
    ''' <remarks></remarks>
    Private Sub InitdgvMap(ByVal dgvCtrl As tCnt_DataGridView_turn)
        '基本設定
        dgvCtrl.dgvInitialize(dgvCtrl)
        dgvCtrl.DataGridViewSelectionCancel(dgvCtrl)

        With dgvCtrl
            '行定義
            .RowCount = TANSHO_MAP_DSP_ROW_MAX
            .ColumnHeadersHeight = 20

            '動作定義
            .ScrollBars = ScrollBars.None
            .ReadOnly = True

            'セル定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '列定義
            .ColumnCount = MAP_COL_NUM + 3 + 1             '入側距離、出側距離、個数、TCM

            .Columns(EM_MAP_COL_INF.DEGAWA).HeaderText = "出側"
            .Columns(EM_MAP_COL_INF.DEGAWA).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_MAP_COL_INF.DEGAWA).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_MAP_COL_INF.DEGAWA).Width = 43
            .Columns(EM_MAP_COL_INF.DEGAWA).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MAP_COL_INF.DEGAWA).Frozen = False
            .Columns(EM_MAP_COL_INF.DEGAWA).HeaderCell.Style.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            For ii As Integer = EM_MAP_COL_INF.COL_1 To EM_MAP_COL_INF.COL_8
                .Columns(ii).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
                .Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                .Columns(ii).Width = 52
                .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(ii).Frozen = True
                .Columns(ii).HeaderCell.Style.Font = New System.Drawing.Font("ＭＳ ゴシック", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            Next ii

            .Columns(EM_MAP_COL_INF.IRIGAWA).HeaderText = "入側"
            .Columns(EM_MAP_COL_INF.IRIGAWA).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_MAP_COL_INF.IRIGAWA).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_MAP_COL_INF.IRIGAWA).Width = 43
            .Columns(EM_MAP_COL_INF.IRIGAWA).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MAP_COL_INF.IRIGAWA).Frozen = True
            .Columns(EM_MAP_COL_INF.IRIGAWA).HeaderCell.Style.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            .Columns(EM_MAP_COL_INF.KIZUNUM).HeaderText = "数"
            .Columns(EM_MAP_COL_INF.KIZUNUM).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_MAP_COL_INF.KIZUNUM).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_MAP_COL_INF.KIZUNUM).Width = 30
            .Columns(EM_MAP_COL_INF.KIZUNUM).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MAP_COL_INF.KIZUNUM).Frozen = False
            .Columns(EM_MAP_COL_INF.KIZUNUM).HeaderCell.Style.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            .Columns(EM_MAP_COL_INF.OTH1).HeaderText = "TCM"
            .Columns(EM_MAP_COL_INF.OTH1).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_MAP_COL_INF.OTH1).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_MAP_COL_INF.OTH1).Width = 32
            .Columns(EM_MAP_COL_INF.OTH1).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MAP_COL_INF.OTH1).Frozen = True
            .Columns(EM_MAP_COL_INF.OTH1).HeaderCell.Style.Font = New System.Drawing.Font("ＭＳ ゴシック", 8.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

        End With
    End Sub

    ''' <summary>
    ''' 表示設定
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize(ByVal dgv As tCnt_DataGridView_turn)

        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                         ' 選択色無し
        Call dgv.DataGridViewDataExec(dgv)

        With dgv
            '行定義
            .RowTemplate.Height = MAP_ROW_HIGHT
            .ColumnHeadersHeight = HED_ROW_HIGHT

            ' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 8.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black
            .ColumnHeadersHeight = 20
            .ScrollBars = ScrollBars.None                                 ' スクロールバーの表示

            '背景色
            .BackgroundColor = Color.White

            ' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 7.5, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True

            .ColumnHeadersVisible = False                   ' 列ヘッダ表示
            .RowHeadersVisible = False                      ' 行ヘッダ表示

        End With
    End Sub

    ''' <summary>
    ''' 深さリスト初期化処理
    ''' </summary>
    ''' <param name="dgvCtrl">マップデータグリッド</param>
    ''' <remarks></remarks>
    Private Sub InitdgvDeep(ByVal dgvCtrl As tCnt_DataGridView_turn)
        '基本設定
        dgvCtrl.dgvInitialize(dgvCtrl)
        dgvCtrl.DataGridViewSelectionCancel(dgvCtrl)

        With dgvCtrl

            '動作定義
            .ScrollBars = ScrollBars.Vertical
            .ReadOnly = True

            'セル定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '列定義
            .ColumnCount = EM_DEEP_LIST_COL.emEnd
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20
            For nCnt As Integer = 0 To .ColumnCount - 1
                .Columns(nCnt).ReadOnly = True
                If nCnt = EM_DEEP_LIST_COL.emNo Then
                    .Columns(nCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                    .Columns(nCnt).Width = 30
                    .Columns(nCnt).HeaderText = "No"
                ElseIf nCnt = EM_DEEP_LIST_COL.emGr Then
                    .Columns(nCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    .Columns(nCnt).Width = 48
                    .Columns(nCnt).HeaderText = "Gr"
                ElseIf nCnt = EM_DEEP_LIST_COL.emType Then
                    .Columns(nCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    .Columns(nCnt).Width = .ClientSize.Width - .Columns(0).Width - .Columns(1).Width - 3
                    .Columns(nCnt).HeaderText = "疵リスト(0)"
                End If
                .Columns(nCnt).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(nCnt).Frozen = False
                .Columns(nCnt).HeaderCell.Style.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                '.Width = .Width + .Columns(nCnt).Width + 1
            Next

            .RowCount = MAX_MAPDEFLST_ROW

        End With
    End Sub

    ''' <summary>
    ''' TCM疵リスト初期化処理
    ''' </summary>
    ''' <param name="dgvCtrl">マップデータグリッド</param>
    ''' <remarks></remarks>
    Private Sub InitdgvTcm(ByVal dgvCtrl As tCnt_DataGridView_turn)
        '基本設定
        dgvCtrl.dgvInitialize(dgvCtrl)
        dgvCtrl.DataGridViewSelectionCancel(dgvCtrl)

        With dgvCtrl

            '動作定義
            .ScrollBars = ScrollBars.Vertical
            .ReadOnly = True

            'セル定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '列定義
            .ColumnCount = EM_TCM_LIST_COL.emEnd
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20
            For nCnt As Integer = 0 To .ColumnCount - 1
                .Columns(nCnt).ReadOnly = True
                If nCnt = EM_TCM_LIST_COL.emNo Then
                    .Columns(nCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                    .Columns(nCnt).Width = 30
                    .Columns(nCnt).HeaderText = "No"
                ElseIf nCnt = EM_TCM_LIST_COL.emLine Then
                    .Columns(nCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    .Columns(nCnt).Width = 42
                    .Columns(nCnt).HeaderText = "ﾗｲﾝ"
                ElseIf nCnt = EM_TCM_LIST_COL.emGr Then
                    .Columns(nCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    .Columns(nCnt).Width = 56
                    .Columns(nCnt).HeaderText = "Gr"
                ElseIf nCnt = EM_TCM_LIST_COL.emType Then
                    .Columns(nCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    .Columns(nCnt).Width = .ClientSize.Width - .Columns(0).Width - .Columns(1).Width - .Columns(2).Width - 3
                    .Columns(nCnt).HeaderText = "TCM(0)"
                End If
                .Columns(nCnt).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(nCnt).Frozen = False
                .Columns(nCnt).HeaderCell.Style.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                '.Width = .Width + .Columns(nCnt).Width + 1
            Next

            .RowCount = MAX_MAPDEFLST_ROW

        End With
    End Sub

    ''' <summary>
    ''' マップ初期化
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ClearMap()
        Try

            '行データ
            For nRow As Integer = 0 To m_typMapInf.dgvMap.RowCount - 1

                m_typMapInf.dgvMap.Rows(nRow).Height = 20

                '列データ
                For nCol As Integer = 0 To m_typMapInf.dgvMap.ColumnCount - 1
                    With m_typMapInf.dgvMap
                        .Item(nCol, nRow).Tag = Nothing
                        .Item(nCol, nRow).Value = ""
                        .Item(nCol, nRow).Style.ForeColor = Color.Black
                        .Item(nCol, nRow).Style.BackColor = Color.White
                    End With
                Next nCol
            Next nRow


        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("マップ初期化異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵リスト初期化
    ''' </summary>
    Public Sub ClearDeep()

        With m_typMapInf.dgvDeep
            .Rows.Clear()
            .RowCount = 0
            .Columns(EM_DEEP_LIST_COL.emType).HeaderText = "疵リスト(0)"
        End With

    End Sub
    ''' <summary>
    ''' TCMリスト初期化
    ''' </summary>
    Public Sub ClearTcm()

        With m_typMapInf.dgvTcm
            .Rows.Clear()
            .RowCount = 0
            .Columns(EM_TCM_LIST_COL.emType).HeaderText = "TCM(0)"
        End With
        m_typTcmSelectValue.Initialize()

    End Sub

    ''' <summary>
    ''' 最大行数初期化
    ''' </summary>
    Public Sub ClearMaxRowCnt()
        m_nMaxRowCnt = 0
    End Sub

    ''' <summary>
    ''' 疵マップ初期表示状態
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MapInit()
        Try
            ClearMap()
            m_typMapInf.dgvMap.Refresh()
            ClearDeep()
            m_typMapInf.dgvDeep.Refresh()
            ClearTcm()
            m_typMapInf.dgvTcm.Refresh()
        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("疵マップ表示処理異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵マップ表示処理
    ''' </summary>
    ''' <param name="nMen">0:表面 1:裏面</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function DspData(ByVal nMen As Integer) As Boolean
        Dim bReturn As Boolean = False

        Try
            m_nMaxRowCnt = 0

            '比較用に変更前の情報をコピー
            Dim nSelectValue(m_typSelectValue.Length - 1) As TYP_MAP_LIST_VALUE
            For ii As Integer = 0 To m_typSelectValue.Length - 1
                nSelectValue(ii) = m_typSelectValue(ii)
            Next
            Dim nTcmSelectValue As TYP_MAP_LIST_VALUE = m_typTcmSelectValue

            '自動更新時
            If m_emViewMode = EM_REFRESH_MODE.AUTO Then
                For ii As Integer = 0 To m_typSelectValue.Length - 1
                    m_typSelectValue(ii).Initialize()
                Next
                m_typTcmSelectValue.Initialize()
            End If

            '表示対象レコード取得
            If Not GetDispRecord(nMen) Then
                MapInit()
                Exit Try
            End If

			'表示対象レコード無し
			If m_nDispRec(0) = 0 Then
                MapInit()
                Exit Try
            End If

			'疵マップ設定
			If Not SetMapData(nMen, 0, m_nMaxRowCnt) Then
                Exit Try
            End If

            Dim nRecDisp As Integer = mcls_PointInf.RecDisp(m_nKensaNo)
            If Not mcls_PointInf.KizukenNo(nRecDisp) Is Nothing And m_emViewMode = EM_REFRESH_MODE.AUTO Then
                '最新疵を見つける
                GetSearchKizuFirst(nMen)
                GetSearchTcmKizuFirst(nMen)
            End If

            ''疵自動選択イベント発生
            'If 0 < m_typSelectValue(0).nPnt And nSelectValue(0) <> m_typSelectValue(0) Then
            '    ' 最新疵選択が変化した場合はマップ位置の変化を確認
            '    Dim nRow As Integer
            '    Dim nCol As Integer
            '    Dim bDeepDisp As Boolean = GetMapPos(m_typSelectValue(0), nRow, nCol)

            '    If bDeepDisp Then
            '        Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(m_typMapInf.dgvMap.Item(nCol, nRow).Tag, List(Of TYP_MAP_LIST_VALUE))
            '        ' 深さリストの表示
            '        SetDeepList(m_typMapInf.dgvDeep, lstVal)
            '    End If
            'End If

            Dim nDeepRow As Integer = 0
            Dim nDeepCol As Integer = 0
            Dim bDeepFlg As Boolean = False
            For ii As Integer = m_typSelectValue.Length - 1 To 0 Step -1
                '' 疵選択が変化した場合はマップ位置の変化を確認
                If (0 < m_typSelectValue(ii).nPnt And nSelectValue(ii) <> m_typSelectValue(ii)) Or bDeepFlg Then
                    Dim nRowWk As Integer
                    Dim nColWk As Integer
                    If GetMapPos(m_typSelectValue(ii), nRowWk, nColWk) Then
                        nDeepRow = nRowWk
                        nDeepCol = nColWk
                        bDeepFlg = True
                    End If
                End If
            Next
            If bDeepFlg Then
                Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(m_typMapInf.dgvMap.Item(nDeepCol, nDeepRow).Tag, List(Of TYP_MAP_LIST_VALUE))
                ' 深さリストの表示
                SetDeepList(m_typMapInf.dgvDeep, lstVal)
            End If

            '選択外となったときリストを初期化
            LstSelectClear()

            Dim nTcmRow As Integer = 0
            Dim nTcmCol As Integer = 0
            Dim bTcmFlg As Boolean = False
            '' 疵選択が変化した場合はマップ位置の変化を確認
            If 0 < m_typTcmSelectValue.nPnt And nTcmSelectValue <> m_typTcmSelectValue Then
                Dim nRowWk As Integer
                Dim nColWk As Integer
                If GetMapTcmPos(m_typTcmSelectValue, nRowWk, nColWk) Then
                    nTcmRow = nRowWk
                    nTcmCol = nColWk
                    bTcmFlg = True
                End If
            End If
            If bTcmFlg Then
                Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(m_typMapInf.dgvMap.Item(nTcmCol, nTcmRow).Tag, List(Of TYP_MAP_LIST_VALUE))
                ' TCMリストの表示
                SetTcmList(m_typMapInf.dgvTcm, lstVal)
            End If

            For ii As Integer = 0 To m_typSelectValue.Length - 1
                If 0 < m_typSelectValue(ii).nPnt Then
                    If nSelectValue(ii) <> m_typSelectValue(ii) Then
                        RaiseEvent SelectMapCell_Auto(nMen, m_typSelectValue(ii), CType(ii, EM_DSP_KIZU_INF))
                    End If
                Else
                    If nSelectValue(ii) <> m_typSelectValue(ii) Then
                        RaiseEvent SelectMapCell_AutoDspInit(CType(ii, EM_DSP_KIZU_INF))
                    End If
                End If
            Next
            If 0 < m_typTcmSelectValue.nPnt Then
                If nTcmSelectValue <> m_typTcmSelectValue Then
                    RaiseEvent SelectTcmMapCell_Auto(nMen, m_typTcmSelectValue)
                End If
            Else
                If nTcmSelectValue <> m_typTcmSelectValue Then
                    RaiseEvent SelectTcmMapCell_AutoDspInit()
                End If
            End If

            bReturn = True

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("疵マップ表示処理異常[{0}]", ex.Message))
        End Try

        Return bReturn
    End Function


    ''' <summary>
    ''' 選択外となったときリストを初期化
    ''' </summary>
    Private Sub LstSelectClear()
        Dim bDeepDelFlg As Boolean = False
        For ii As Integer = m_typSelectValue.Length - 1 To 0 Step -1
            Dim nRowWk As Integer
            Dim nColWk As Integer
            If GetMapPos(m_typSelectValue(ii), nRowWk, nColWk) Then
                bDeepDelFlg = True
            End If
        Next ii
        If Not bDeepDelFlg Then
            ClearDeep()
        End If
        m_typMapInf.dgvDeep.Refresh()

    End Sub

    ''' <summary>
    ''' 表示対象レコード取得
    ''' </summary>
    ''' <param name="nMen">表裏面</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetDispRecord(ByVal nMen As Integer) As Boolean
        Dim bRet As Boolean = False
        Dim nDispRec As Integer                             '表示レコード
        Dim nCoilCnt As Integer = 0

        Try
            '初期化
            For ii As Integer = 0 To TBL_HY_REC_MAX - 1
                m_nDispRec(ii) = 0
            Next

            '検査台の現在表示レコード取得
            nDispRec = mcls_PointInf.RecDisp(m_nKensaNo)
            If nDispRec = 0 Then
                Exit Try
            End If

            ' 20201010 検査台位置表示変更 --->>>
            'トラッキング表示の場合は、表示基点～検査台位置 と 検査台表示行の差分ずらす。
            If EM_DEFECT_MODE.TRACKING = m_emMapMode Then                       ' トラッキング表示 
                Dim nRec As Integer = 0         ' 表示開始レコード取得用
                Dim nRow As Integer = 0         ' 表示開始行取得用
                ' トラッキング用表示開始位置取得
                If GetDispStartRecAndRow(nMen, nRec, nRow) Then
                    nDispRec = nRec
                End If
            End If
            ' 20201010 検査台位置表示変更 <<<---

            '表示対象レコード収集
            For ii As Integer = 0 To TBL_HY_REC_MAX - 1     'レコード数分
                If ii > DSP_COIL_MAX Then                   '表示数分
                    Exit For
                End If
                '表示可能または表示予定
                If mcls_PointInf.DispStatus(nDispRec) = EM_DIV_DSP_STATUS.DIV_DSP_OK _
                Or mcls_PointInf.DispStatus(nDispRec) = EM_DIV_DSP_STATUS.DIV_DSP_STANDBY Then
                    m_nDispRec(nCoilCnt) = nDispRec         'レコード格納
                    nCoilCnt = nCoilCnt + 1
                End If
                '前回表示レコード取得
                nDispRec = clsPointInf.GetPrevRecNo(nDispRec)
            Next
            '処理正常
            bRet = True
        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("疵マップ表示処理異常[{0}]", ex.Message))
        End Try

        Return bRet
    End Function


    ''' <summary>
    ''' 疵マップ情報表示
    ''' </summary>
    ''' <param name="nMen">表裏面</param>
    ''' <param name="nMove">移動量</param>
    ''' <param name="nMaxRowNum">最大列数</param>
    ''' <remarks></remarks>
    Public Function SetMapData(ByVal nMen As Integer, ByVal nMove As Integer, ByRef nMaxRowNum As Integer) As Boolean
        Dim nRowDisp As Integer                             '現在表示行
        Dim nRowPos As Integer                              'データ行位置
        Dim nDgvPos As Integer = 0                          'DGV行位置
        Dim nRecDisp As Integer                             '表示レコード
        Dim nStartID As Integer = 0                         '開始レコード位置
        Dim nRecID As Integer = 0                           '表示レコード位置
        Dim nCoilCnt As Integer = 0                         '表示コイル数
        Dim bStart As Boolean = False                       '処理開始フラグ
        Dim nKensaPos As Integer = 0                        '検査台位置
        Dim nRowCheck As Integer = 0
        Dim nRowCount As Integer = 0
        Dim nStage As Integer = CInt(Stage)
        Dim nRowCnt As Integer = 0
        Dim bRet As Boolean = False


        Try
            If True = mcls_PointInf.Refresh Then
                Exit Try
            End If

            '初期化
            ClearMap()

            '表示レコード取得
            nRecDisp = mcls_PointInf.RecDisp(m_nKensaNo)

            '現在表示行取得
            nRowDisp = mcls_PointInf.RowDisp(nRecDisp, m_nKensaNo, nStage)

            ' 20201010 検査台位置表示変更 --->>>
            'トラッキング表示の場合は、表示基点～検査台位置 と 検査台表示行の差分ずらす。
            If EM_DEFECT_MODE.TRACKING = m_emMapMode Then                       ' トラッキング表示 
                Dim nRec As Integer = 0         ' 表示開始レコード取得用
                Dim nRow As Integer = 0         ' 表示開始行取得用
                ' トラッキング用表示開始位置取得
                If GetDispStartRecAndRow(nMen, nRec, nRow) Then
                    nRowDisp = nRow
                End If
            End If
            ' 20201010 検査台位置表示変更 <<<---

            'マップ表示ループ
            For ii As Integer = 0 To TBL_HY_REC_MAX - 1
                Dim nRec As Integer = m_nDispRec(ii)
                If nRec <= 0 Then                           '無効レコード
                    Exit For
                End If
                If nCoilCnt >= DSP_COIL_MAX Then            '表示コイル分完了
                    Exit For
                End If
                If nRec = m_nDispRec(nStartID) Then         '開始レコード
                    bStart = True                           '処理開始
                End If
                If Not bStart Then                           '処理開始
                    Continue For
                End If
                If nCoilCnt = 0 Then
                    nRowCnt = nRowCnt + nRowDisp
                    nRowPos = nRowDisp + nMove
                Else
                    Dim LPos As Integer = mcls_PointInf.RowLast(nRec, nStage)                 ' 検査完了行
                    Dim WPos As Integer = mcls_PointInf.RowWrite(nRec, nMen, nStage) + 1      ' 現在書き込み行
                    Dim DPos As Integer = mcls_PointInf.RowDisp(nRec, m_nKensaNo, nStage)     ' 現在表示行
                    Dim nPos As Integer = 0
                    If LPos > 0 Then
                        '検査完了行セット
                        WPos = LPos
                    End If

                    If LPos <> 0 Then
                        nPos = LPos
                    Else
                        nPos = DPos
                    End If

                    nRowCnt = nRowCnt + nPos

                    If nRowPos > -1 Then
                        nRowPos = nPos
                    Else
                        nRowPos = nPos + nRowPos
                    End If
                End If
                If m_typMapInf.dgvMap.RowCount > nDgvPos Then
                    DspMap(nMen, nStage, nRowPos, nDgvPos, nRec)
                End If
                If checkcheck Then
                    mcls_Log.Write(LNO.MSG, String.Format("調査 {0}本目 行{1}({2}) rec={3} 管理No=[{4}] men={5} nmove={6}" _
                                                          , nCoilCnt + 1, nRowPos, nRowCnt, nRec, mcls_PointInf.KizukenNo(nRec), nMen, nMove))
                End If
                nCoilCnt = nCoilCnt + 1                                     '表示コイル数
            Next

            '移動量が0の場合に、最大行数を更新する
            nMaxRowNum = nRowCnt

            bRet = True

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("疵マップ情報セット異常[{0}]", ex.Message))
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 最新疵検索
    ''' </summary>
    ''' <param name="nMen">表裏面番号</param>
    ''' <returns>True：存在する、False：存在しない</returns>
    ''' <remarks></remarks>
    Private Function GetSearchKizuFirst(ByVal nMen As Integer) As Boolean
        Dim nColMost As Integer
        Dim nRowStart As Integer
        Dim nRowEnd As Integer
        Dim nRowStep As Integer
        Dim nChkCnt As Integer = 0

        '検査台の行数取得(１オリジン)
        Dim nKensaRow As Integer = GetKensaRow() + 1

        Dim bDispRec As Boolean
        Dim nRowDisp As Integer                             '検査台位置の現在表示行
        Dim nRowLast As Integer                             '検査完了行
        Dim nRecDisp As Integer                             '検査台位置の現在表示レコード
        Dim ii As Integer = 0                               'レコードカウント

        ' 20201010 検査台位置表示変更 --->>>
        If EM_DEFECT_MODE.TRACKING = m_emMapMode Then                       ' トラッキング表示 
            nRecDisp = m_nDispRec(0)
            nRowDisp = mcls_PointInf.RowDisp(m_nDispRec(0), m_nKensaNo, m_emStage)
            nRowLast = mcls_PointInf.RowLast(m_nDispRec(0), m_emStage)
        Else
            nRecDisp = mcls_PointInf.RecDisp(m_nKensaNo)
            nRowDisp = mcls_PointInf.RowDisp(nRecDisp, m_nKensaNo, m_emStage)
            nRowLast = mcls_PointInf.RowLast(nRecDisp, m_emStage)
        End If
        ' 20201010 検査台位置表示変更 <<<---

        Dim nRec As Integer = 0
        For ii = 0 To DSP_COIL_MAX - 1                      '表示コイル分ループ
            nRec = m_nDispRec(ii)                           '対象レコード

            If nRec <= 0 Then
                Exit For
            End If


            If nRowLast > 0 Then
                If nRowDisp - 1 > nRowLast Then
                    If nRec = m_nDispRec(0) Then
                        bDispRec = True
                    Else
                        bDispRec = False
                    End If
                Else
                    If nRec = nRecDisp Then
                        bDispRec = True
                    Else
                        bDispRec = False
                    End If
                End If
            Else
                If nRec = m_nDispRec(0) Then
                    bDispRec = True
                Else
                    bDispRec = False
                End If
            End If

            If bDispRec Then
                If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                    nRowStart = CType(m_typMapInf.dgvMap(EM_MAP_COL_INF.IRIGAWA, 0).Tag, G_MAP_ROW).nMemRowPos
                Else
                    nRowStart = CType(m_typMapInf.dgvMap(EM_MAP_COL_INF.IRIGAWA, TANSHO_MAP_DSP_ROW_MAX - 1).Tag, G_MAP_ROW).nMemRowPos
                End If
            Else
                If mcls_PointInf.RowDisp(nRec, m_nKensaNo, m_emStage) > mcls_PointInf.RowLast(nRec, m_emStage) Then
                    nRowStart = mcls_PointInf.RowDisp(nRec, m_nKensaNo, m_emStage) - 1
                Else
                    nRowStart = mcls_PointInf.RowLast(nRec, m_emStage) - 1
                    ' 20201010 検査台位置表示変更(不要ロジックのため削除) --->>>
                    'If m_emStage = EM_LONG_POS.BIG Then
                    '    nRowStart = nRowStart + 10
                    'End If
                    ' 20201010 検査台位置表示変更 <<<---
                End If
            End If

            nRowEnd = 0
            nRowStep = -1

            'Dim nMapColNum As Integer = GetMapColNum(nRec)

            For nCnt As Integer = nRowStart To nRowEnd Step nRowStep

                'マップ情報取得
                Dim typRow As G_MAP_ROW = Nothing
                Dim lstCell() As List(Of TYP_MAP_LIST_VALUE)
                Dim lstTcmCell() As List(Of TYP_MAP_LIST_VALUE)

                typRow.initialize()
                ReDim lstCell(MAP_COL_NUM - 1)
                For jj As Integer = 0 To lstCell.Length - 1
                    lstCell(jj) = Nothing
                Next
                ReDim lstTcmCell(1 - 1)
                For jj As Integer = 0 To lstTcmCell.Length - 1
                    lstTcmCell(jj) = Nothing
                Next

                GetMapRowData(nRec, nMen, m_emStage, nCnt, typRow, lstCell, lstTcmCell)

                nColMost = -1

                If nChkCnt < nKensaRow Then             ' 検査台以前の疵？

                    ' 最新疵取得
                    nColMost = GetMostDeffectColumn(nMen, lstCell, typRow, nRec)
                    If 0 <= nColMost And MAP_COL_NUM > nColMost Then
                        If 0 < lstCell(nColMost)(0).nPnt Then
                            m_typSelectValue(1) = m_typSelectValue(0)
                            m_typSelectValue(0) = lstCell(nColMost)(0)
                            m_posSelectValue(1) = m_posSelectValue(0)
                            m_posSelectValue(0) = EM_SELECT_POS.emKensaMae
                        End If
                    End If

                    ' 次疵取得
                    nColMost = GetMostDeffectColumn(nMen, lstCell, typRow, nRec, nColMost)
                    If 0 <= nColMost And MAP_COL_NUM > nColMost Then
                        If m_typSelectValue(0) <> lstCell(nColMost)(0) Then
                            If 0 < lstCell(nColMost)(0).nPnt Then
                                m_typSelectValue(1) = lstCell(nColMost)(0)
                                m_posSelectValue(1) = EM_SELECT_POS.emKensaMae
                            End If
                        End If
                    End If
                Else
                    If 0 >= m_typSelectValue(0).nPnt Then

                        ' 最新疵が決定されていない時
                        nColMost = GetMostDeffectColumn(nMen, lstCell, typRow, nRec)
                        If 0 <= nColMost And MAP_COL_NUM > nColMost Then
                            If 0 < lstCell(nColMost)(0).nPnt Then
                                m_typSelectValue(0) = lstCell(nColMost)(0)
                                m_posSelectValue(0) = EM_SELECT_POS.emKensaAto
                            End If
                        End If
                    End If
                    If 0 < m_typSelectValue(0).nPnt And 0 >= m_typSelectValue(1).nPnt Then

                        ' 最新疵が決定していて、1つ前の疵が決定していない場合
                        nColMost = GetMostDeffectColumn(nMen, lstCell, typRow, nRec, nColMost)
                        If 0 <= nColMost And MAP_COL_NUM > nColMost Then
                            If m_typSelectValue(0) <> lstCell(nColMost)(0) Then
                                If 0 < lstCell(nColMost)(0).nPnt Then
                                    m_typSelectValue(1) = lstCell(nColMost)(0)
                                    m_posSelectValue(1) = EM_SELECT_POS.emKensaAto
                                    Return True ' 対象の疵が２つ見つかったので検索終了
                                End If
                            End If
                        End If
                    End If
                End If

                nChkCnt = nChkCnt + 1
            Next
        Next
        Return True
    End Function

    ''' <summary>
    ''' 最新疵検索
    ''' </summary>
    ''' <param name="nMen">表裏面番号</param>
    ''' <returns>True：存在する、False：存在しない</returns>
    ''' <remarks></remarks>
    Private Function GetSearchTcmKizuFirst(ByVal nMen As Integer) As Boolean
        Dim nColMost As Integer
        Dim nRowStart As Integer
        Dim nRowEnd As Integer
        Dim nRowStep As Integer
        Dim nChkCnt As Integer = 0

        '検査台の行数取得(１オリジン)
        Dim nKensaRow As Integer = GetKensaRow() + 1

        Dim bDispRec As Boolean
        Dim nRowDisp As Integer                             '検査台位置の現在表示行
        Dim nRowLast As Integer                             '検査完了行
        Dim nRecDisp As Integer                             '検査台位置の現在表示レコード
        Dim ii As Integer = 0                               'レコードカウント

        ' 20201010 検査台位置表示変更 --->>>
        If EM_DEFECT_MODE.TRACKING = m_emMapMode Then                       ' トラッキング表示 
            nRecDisp = m_nDispRec(0)
            nRowDisp = mcls_PointInf.RowDisp(m_nDispRec(0), m_nKensaNo, m_emStage)
            nRowLast = mcls_PointInf.RowLast(m_nDispRec(0), m_emStage)
        Else
            nRecDisp = mcls_PointInf.RecDisp(m_nKensaNo)
            nRowDisp = mcls_PointInf.RowDisp(nRecDisp, m_nKensaNo, m_emStage)
            nRowLast = mcls_PointInf.RowLast(nRecDisp, m_emStage)
        End If
        ' 20201010 検査台位置表示変更 <<<---

        Dim nRec As Integer = 0
        For ii = 0 To DSP_COIL_MAX - 1                      '表示コイル分ループ
            nRec = m_nDispRec(ii)                           '対象レコード

            If nRec <= 0 Then
                Exit For
            End If

            If nRowLast > 0 Then
                If nRowDisp - 1 > nRowLast Then
                    If nRec = m_nDispRec(0) Then
                        bDispRec = True
                    Else
                        bDispRec = False
                    End If
                Else
                    If nRec = nRecDisp Then
                        bDispRec = True
                    Else
                        bDispRec = False
                    End If
                End If
            Else
                If nRec = m_nDispRec(0) Then
                    bDispRec = True
                Else
                    bDispRec = False
                End If
            End If

            If bDispRec Then
                If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                    nRowStart = CType(m_typMapInf.dgvMap(EM_MAP_COL_INF.IRIGAWA, 0).Tag, G_MAP_ROW).nMemRowPos
                Else
                    nRowStart = CType(m_typMapInf.dgvMap(EM_MAP_COL_INF.IRIGAWA, TANSHO_MAP_DSP_ROW_MAX - 1).Tag, G_MAP_ROW).nMemRowPos
                End If
            Else
                If mcls_PointInf.RowDisp(nRec, m_nKensaNo, m_emStage) > mcls_PointInf.RowLast(nRec, m_emStage) Then
                    nRowStart = mcls_PointInf.RowDisp(nRec, m_nKensaNo, m_emStage) - 1
                Else
                    nRowStart = mcls_PointInf.RowLast(nRec, m_emStage) - 1
                    ' 20201010 検査台位置表示変更(不要ロジックのため削除) --->>>
                    'If m_emStage = EM_LONG_POS.BIG Then
                    '    nRowStart = nRowStart + 10
                    'End If
                    ' 20201010 検査台位置表示変更 <<<---
                End If
            End If

            nRowEnd = 0
            nRowStep = -1

            For nCnt As Integer = nRowStart To nRowEnd Step nRowStep

                'マップ情報取得
                Dim typRow As G_MAP_ROW = Nothing
                Dim lstCell() As List(Of TYP_MAP_LIST_VALUE)
                Dim lstTcmCell() As List(Of TYP_MAP_LIST_VALUE)

                typRow.initialize()
                ReDim lstCell(MAP_COL_NUM - 1)
                For jj As Integer = 0 To lstCell.Length - 1
                    lstCell(jj) = Nothing
                Next
                ReDim lstTcmCell(1 - 1)
                For jj As Integer = 0 To lstTcmCell.Length - 1
                    lstTcmCell(jj) = Nothing
                Next

                GetMapRowData(nRec, nMen, m_emStage, nCnt, typRow, lstCell, lstTcmCell)

                nColMost = -1
                If nChkCnt < nKensaRow Then            ' 検査台以前の疵？
                    ' 最新TCM疵取得
                    If lstTcmCell(0) IsNot Nothing AndAlso 0 < lstTcmCell(0)(0).nPnt Then
                        m_typTcmSelectValue = lstTcmCell(0)(0)
                    End If
                Else                                    ' 検査台より後の疵
                    If 0 >= m_typTcmSelectValue.nPnt Then
                        ' 最新TCM疵が決定されていない時
                        If lstTcmCell(0) IsNot Nothing AndAlso 0 < lstTcmCell(0)(0).nPnt Then
                            m_typTcmSelectValue = lstTcmCell(0)(0)
                            Return True ' 対象TCM疵が見つかったので検索終了
                        End If
                    Else
                        Return True ' 検査台前に対象TCM疵が見つかっているので検索終了
                    End If
                End If

                nChkCnt = nChkCnt + 1
                If nChkCnt > m_typMapInf.dgvMap.DisplayedRowCount(True) Then
                    '表示範囲を超えたので検索終了
                    Return True
                End If
            Next
        Next
        Return True
    End Function

    ''' <summary>
    ''' マップ情報セット
    ''' </summary>
    ''' <param name="nMen">表裏面</param>
    ''' <param name="nStage">ステージ</param>
    ''' <param name="nMapPos">現在表示行</param>
    ''' <param name="nDgvPos">DGV表示位置</param>
    ''' <param name="nRec">レコード番号</param>
    ''' <remarks></remarks>
    Private Sub DspMap(ByVal nMen As Integer, ByVal nStage As Integer, ByVal nMapPos As Integer, ByRef nDgvPos As Integer, ByVal nRec As Integer)
        Dim nColStart As Integer                            '開始列インデックス
        Dim nColEnd As Integer                              '終了列インデックス
        Dim nStep As Integer                                'ステップ間隔
        Dim nColCnt As Integer                              '表示COLカウンタ
        Dim nRowStart As Integer
        Dim bCoilMax As Boolean = False


        '疵マップDS位置
        If m_DSPos = EM_MAP_CELL.NORMAL Then                '左がDS
            nColStart = 0
            nColEnd = MAP_COL_NUM - 1
            nStep = 1
        Else                                                '左がWS
            nColStart = MAP_COL_NUM - 1
            nColEnd = 0
            nStep = -1
        End If


        ' 20201010 検査台位置表示変更(不要ロジックのため削除) --->>>
        'Dim nRecDisp As Integer = mcls_PointInf.RecDisp(m_nKensaNo)
        'Dim nRowDisp As Integer = mcls_PointInf.RowDisp(nRecDisp, m_nKensaNo, nStage)
        ' 20201010 検査台位置表示変更 <<<---

        Dim nMapColNum As Integer = GetMapColNum(nRec)

        Try
            nRowStart = nMapPos - 1     '0オリジン

            For nMapRow As Integer = nRowStart To 0 Step -1

                bCoilMax = False

                If nDgvPos >= m_typMapInf.dgvMap.RowCount Then
                    Exit For
                End If

                Dim nDgvRow As Integer = nDgvPos
                If EM_MAP_SCROOL.TAIL_TO_HEAD = MapDir Then         '下から上
                    nDgvRow = m_typMapInf.dgvMap.RowCount - 1 - nDgvPos
                End If

                '開始行の番号セット
                nColCnt = 0

                'マップ情報取得
                Dim typRow As G_MAP_ROW = Nothing
                Dim lstCell() As List(Of TYP_MAP_LIST_VALUE)
                Dim lstTcmCell() As List(Of TYP_MAP_LIST_VALUE)

                typRow.initialize()
                ReDim lstCell(MAP_COL_NUM - 1)
                For jj As Integer = 0 To lstCell.Length - 1
                    lstCell(jj) = Nothing
                Next
                ReDim lstTcmCell(1 - 1)
                For jj As Integer = 0 To lstTcmCell.Length - 1
                    lstTcmCell(jj) = Nothing
                Next

                GetMapRowData(nRec, nMen, nStage, nMapRow, typRow, lstCell, lstTcmCell)

                For nSetColCnt As Integer = nColStart To nColEnd Step nStep

                    If 0 < typRow.col(nSetColCnt).nKizuNo Then                      '表示対象疵有り
                        m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Value = mcls_Param.GetMapName(typRow.col(nSetColCnt).nTid)
                        m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Style.BackColor = mcls_Param.GetGradeColor(typRow.col(nSetColCnt).nGid)
                        m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Tag = lstCell(nSetColCnt)
                    Else
                        m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Value = vbNullString
                        m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Style.BackColor = g_ColorTanshoOk
                        m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Tag = Nothing
                        If 0 > typRow.col(nSetColCnt).nKizuNo Then
                            m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Style.BackColor = g_ColorTanshoGai
                        End If
                    End If

                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Style.SelectionBackColor = m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Style.BackColor
                    nColCnt = nColCnt + 1                   '列を進める
                Next

                '入側距離表示
                m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, nDgvRow).Value = typRow.nLen_i
                '出側距離表示
                m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Value = typRow.nLen_o

                '入側セルカラー 
                If EM_DIV_KENSA.DIV_KENSA_STOP = typRow.emKensa Then       '停止
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKensaBackColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_STOP)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKensaForeColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_STOP)
                ElseIf EM_DIV_KENSA.DIV_KENSA_NON = typRow.emKensa Then   '停止(対象外)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKensaBackColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_NON)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKensaForeColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_NON)
                ElseIf EM_DIV_KENSA.DIV_KENSA_SKIP = typRow.emKensa Then   '未検査
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKensaBackColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_SKIP)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKensaForeColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_SKIP)
                ElseIf EM_DIV_KENSA.DIV_KENSA_DBNON = typRow.emKensa Then  '未保存
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKensaBackColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_DBNON)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKensaForeColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_DBNON)
                ElseIf EM_DIV_KIKI.DIV_KIKI_JYU = typRow.emKiki Then       '重故障
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKikiBackColor(KizuForm.EM_DIV_KIKI.DIV_KIKI_JYU)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKikiForeColor(KizuForm.EM_DIV_KIKI.DIV_KIKI_JYU)
                ElseIf EM_DIV_KENSA.DIV_KENSA_EDGENG = typRow.emKensa Then  'エッジ不正
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKensaBackColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_EDGENG)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKensaForeColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_EDGENG)
                ElseIf EM_DIV_KENSA.DIV_KENSA_NG = typRow.emKensa Then     '縮退
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKensaBackColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_NG)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKensaForeColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_NG)
                ElseIf -1 = typRow.nEnable Then                            '受信遅れ(統括→表示)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = g_ColorJyusinOkure
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = Color.Black
                ElseIf EM_DIV_KIKI.DIV_KIKI_KEI = typRow.emKiki Then       '軽故障
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKikiBackColor(KizuForm.EM_DIV_KIKI.DIV_KIKI_KEI)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKikiForeColor(KizuForm.EM_DIV_KIKI.DIV_KIKI_KEI)
                ElseIf 0 = typRow.nEnable Then                             '未探傷
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = g_ColorTanshoNon
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = Color.Black
                Else                                                            '正常
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = g_ColorTanshoOk
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = Color.Black
                End If
                m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, nDgvRow).Style.SelectionBackColor = m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, nDgvRow).Style.BackColor
                m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, nDgvRow).Tag = typRow

                With m_typMapInf

                    '行単位の疵個数を表示
                    If typRow.nKizuCnt <= 0 Then
                        '疵がないときは、数を表示しない
                        .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Value = vbNullString
                    ElseIf typRow.nKizuCnt <= KZCNT_CNT_MAX Then
                        '疵が表示最大値を超えないときは、数を表示
                        .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Value = typRow.nKizuCnt.ToString()
                    Else
                        '疵が表示最大値を超えるときは、最大値を表示
                        .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Value = KZCNT_CNT_MAX.ToString()
                    End If

                    '行単位の疵個数に対応した色の表示
                    If typRow.nKizuCnt >= mcls_Param.GetCntMapCount(EM_DETECT_GR.DETECT_GR_JU) Then
                        .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Style.BackColor = mcls_Param.GetCntMapColor(EM_DETECT_GR.DETECT_GR_JU)
                    ElseIf typRow.nKizuCnt >= mcls_Param.GetCntMapCount(EM_DETECT_GR.DETECT_GR_TYU) Then
                        .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Style.BackColor = mcls_Param.GetCntMapColor(EM_DETECT_GR.DETECT_GR_TYU)
                    ElseIf typRow.nKizuCnt >= mcls_Param.GetCntMapCount(EM_DETECT_GR.DETECT_GR_KEI) Then
                        .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Style.BackColor = mcls_Param.GetCntMapColor(EM_DETECT_GR.DETECT_GR_KEI)
                    Else
                        .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Style.BackColor = Color.White
                    End If

                    ''選択時の背景色も、通常時と同じにしておく
                    .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Style.SelectionBackColor = .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Style.BackColor

                End With

                If 0 < typRow.tcm(0).nKizuNo Then                      '表示対象疵有り
                    Dim typTcmData As TYP_MAP_LIST_VALUE = lstTcmCell(0)(0)
                    typTcmData.Initialize()

                    typTcmData = lstTcmCell(0)(0)
                    Dim strDsp As String = ""
                    Select Case typTcmData.strProc
                        Case KOUTEI_CODE_2TCM   '2TCM
                            strDsp = "2"
                        Case KOUTEI_CODE_3TCM   '3TCM
                            strDsp = "3"
                    End Select
                    strDsp = strDsp & mcls_PreParam.GetDspMapName(typTcmData.strProc, typTcmData.strFFType)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.OTH1, nDgvRow).Value = strDsp
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.OTH1, nDgvRow).Style.BackColor = tMod.GetIntToColor(mcls_PreParam.GetDspGradeColor(typTcmData.strProc, typTcmData.strFFGrade))
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.OTH1, nDgvRow).Tag = lstTcmCell(0)
                Else
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.OTH1, nDgvRow).Value = vbNullString
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.OTH1, nDgvRow).Style.BackColor = g_ColorTanshoOk
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.OTH1, nDgvRow).Tag = Nothing
                    'If 0 > typRow.cyc(0).nKizuNo Then
                    '    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.OTH1, nDgvRow).Style.BackColor = g_ColorTanshoGai
                    'End If
                End If

                nDgvPos = nDgvPos + 1

            Next

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("疵マップ生成失敗[{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' マップ位置行情報取得
    ''' </summary>
    ''' <param name="nRec">レコード[1～]</param>
    ''' <param name="nMen">面[0:表,1:裏]</param>
    ''' <param name="nRow">行[0～]</param>
    ''' <returns>行情報</returns>
    ''' <remarks></remarks>
    Private Function GetMapRowData(ByVal nRec As Integer, ByVal nMen As Integer, ByVal nStage As Integer, ByVal nRow As Integer,
                                   ByRef typRow As G_MAP_ROW, ByRef lstCell() As List(Of TYP_MAP_LIST_VALUE),
                                   ByRef lstTcmCell() As List(Of TYP_MAP_LIST_VALUE)) As Boolean

        Dim bRetc As Boolean = False

        Try

            Dim typMapRow_Row As New MAP_ROW_ROW
            If Not GetMapRow_Row(nRec, nMen, nRow, nStage, typMapRow_Row) Then
                Exit Try
            End If

            typRow.initialize()
            typRow.nRec = nRec
            typRow.nMemRowPos = nRow
            typRow.emKensa = typMapRow_Row.typ.emDspKensa
            typRow.emKiki = typMapRow_Row.typ.emDspKiki
            typRow.nLen_i = typMapRow_Row.typ.nLen_i
            typRow.nLen_o = typMapRow_Row.typ.nLen_o
            typRow.nEnable = typMapRow_Row.typ.nEnable
            typRow.nCut = typMapRow_Row.typ.nCut
            If 0 < typMapRow_Row.typ.nKizuCnt Then
                '疵個数受信済み
                typRow.nKizuCnt = typMapRow_Row.typ.nKizuCnt
            Else
                typRow.nKizuCnt = 0
            End If

            If 0 = nRow Or (typMapRow_Row.typ.nKind And ROW_KIND_YOSETU) <> 0 Then
                typRow.bYousetu = True
            End If

            Dim nMapColNum As Integer = GetMapColNum(nRec)
            For ii As Integer = 0 To CInt((MAP_COL_NUM - nMapColNum) / 2) - 1
                'マップ列対象外（エッジ外）
                typRow.col(ii).nKizuNo = -2
                typRow.col(MAP_COL_NUM - ii - 1).nKizuNo = -2
            Next
            typRow.tcm(0).nKizuNo = 0

            Dim nMaxY As Integer = typRow.nLen_i * 1000
            Dim nMinY As Integer = 0
            Dim nMaxRow As Integer = MAP_ROW_NUM
            If EM_LONG_POS.BIG = nStage Then
                nMinY = nMaxY - mcls_Param.MapStageBig * 1000
                If 0 < mcls_Param.MapStageBig Then
                    nMaxRow = COIL_LEN_MAX \ mcls_Param.MapStageBig
                    If 0 <> (COIL_LEN_MAX Mod mcls_Param.MapStageBig) Then
                        nMaxRow = nMaxRow + 1
                    End If
                End If
            Else
                nMinY = nMaxY - mcls_Param.MapStageSmall * 1000
                If 0 < mcls_Param.MapStageSmall Then
                    nMaxRow = COIL_LEN_MAX \ mcls_Param.MapStageSmall + 1
                    If 0 <> (COIL_LEN_MAX Mod mcls_Param.MapStageSmall) Then
                        nMaxRow = nMaxRow + 1
                    End If
                End If
            End If
            If MAP_ROW_NUM < nMaxRow Then
                nMaxRow = MAP_ROW_NUM
            End If

            For nPnt As Integer = typMapRow_Row.typ.pnt(0) To typMapRow_Row.typ.pnt(1)
                If 0 >= nPnt Then
                    Continue For
                End If

                Dim typDefInf As DSP_TYP_DEFECTINF = Nothing
                If Not frmTansho.GetDefect(nRec, nPnt, typDefInf, mcls_Log) Then
                    Continue For
                End If

                If 0 = typDefInf.data.nKizuNo Then
                    ' 疵Noなし
                    Continue For
                End If

                If 1 <> typDefInf.data.nObjTansho Then
                    ' 探傷画面対象外(1: 対象)
                    Continue For
                End If

                If nMinY > typDefInf.data.y Then
                    ' 入側距離位置が範囲外(小さい）
                    Continue For
                End If

                If nMaxY <= typDefInf.data.y Then
                    ' 入側距離位置が範囲外（大きい）
                    ' 但し、この行が検査完了行の場合のみ、入側距離位置が範囲外（大きい）疵も表示する。
                    Dim LPos As Integer = mcls_PointInf.RowLast(nRec, nStage)
                    If (LPos <> nRow + 1) Then
                        'MAX行ではない
                        If nRow <> (nMaxRow - 1) Then
                            ' この行は検査完了行ではない(検査完了行は1オリジン)
                            Continue For
                        End If
                    End If
                End If


                If nMen <> typDefInf.data.TorB Then
                    ' 表裏区分
                    Continue For
                End If

                Dim col As Integer = typDefInf.data.nWidDiv
                Dim data As TYP_MAP_LIST_VALUE = Nothing
                data.Initialize()
                data.nRec = nRec
                data.nPnt = nPnt
                data.nKizuNo = typDefInf.data.nKizuNo
                data.nTid = typDefInf.data.nTid
                data.nTYuu = typDefInf.data.nTyuu
                data.nGid = typDefInf.data.nGid
                data.nGYuu = typDefInf.data.nGyuu
                data.nY = CInt(typDefInf.data.y)
				'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
				data.nY_Out = CInt(typDefInf.data.y_out)
				'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------                
                data.nAria = CInt(typDefInf.data.aria)
                data.nCycDiv = CInt(typDefInf.data.cycle_div)
                ' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
                data.nKouteiID = 0
                ' 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

                If lstCell(col) Is Nothing Then
                    lstCell(col) = New List(Of TYP_MAP_LIST_VALUE)
                    lstCell(col).Clear()
                End If

                Dim bAdd As Boolean = True
                For ii As Integer = 0 To lstCell(col).Count - 1
                    If nPnt = lstCell(col)(ii).nPnt Then
                        '既に登録済み
                        bAdd = False
                        Exit For
                    End If
                Next

                If bAdd Then
                    ' リスト追加
                    lstCell(col).Add(data)
                    If 1 > typMapRow_Row.typ.nKizuCnt Then      ' 疵個数が未受信の場合
                        typRow.nKizuCnt += 1
                    End If
                End If
            Next

            '代表疵算出(優先順位でソート)
            For ii As Integer = 0 To lstCell.Length - 1
                If lstCell(ii) Is Nothing Then
                    Continue For
                End If
                lstCell(ii).Sort(AddressOf SortPriorty)
                If 0 < lstCell(ii).Count Then
                    typRow.col(ii).nKizuNo = lstCell(ii)(0).nKizuNo
                    typRow.col(ii).nTid = lstCell(ii)(0).nTid
                    typRow.col(ii).nGid = lstCell(ii)(0).nGid
                End If
            Next ii


            For nPnt As Integer = typMapRow_Row.typ.pntMae(0) To typMapRow_Row.typ.pntMae(1)
                If 0 >= nPnt Then
                    Continue For
                End If

                Dim typDefInf As MAE_INF = Nothing
                If Not frmTansho.GetMaeDefect(nRec, nMen, nPnt, typDefInf, mcls_Log) Then
                    Continue For
                End If

                If 0 = typDefInf.nKizuNo Then
                    ' 疵Noなし
                    Continue For
                End If

                If nMinY > typDefInf.ff.nY Then
                    ' 入側距離位置が範囲外(小さい）
                    Continue For
                End If

                If nMaxY <= typDefInf.ff.nY Then
                    ' 入側距離位置が範囲外（大きい）
                    ' 但し、この行が検査完了行の場合のみ、入側距離位置が範囲外（大きい）疵も表示する。
                    Dim LPos As Integer = mcls_PointInf.RowLast(nRec, nStage)
                    If (LPos <> nRow + 1) Then
                        'MAX行ではない
                        If nRow <> (nMaxRow - 1) Then
                            ' この行は検査完了行ではない(検査完了行は1オリジン)
                            Continue For
                        End If
                    End If
                End If


                'If nMen <> typDefInf.data(nSpm).TorB Then
                '    ' 表裏区分
                '    Continue For
                'End If

                Dim col As Integer = 0
                Dim data As TYP_MAP_LIST_VALUE = Nothing
                data.Initialize()
                data.nRec = nRec
                data.nPnt = nPnt
                data.nKizuNo = typDefInf.ff.nMaeKizuNo
                data.nTid = typDefInf.ff.nTid

                data.nGid = typDefInf.ff.nGid
                data.nY = CInt(typDefInf.ff.nY)
				'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
				data.nY_Out = CInt(typDefInf.ff.nY_Out)
				'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
                data.nAria = 0
                data.nCycDiv = 0
                data.strFFType = tMod.ByteToString(typDefInf.ff.cT)
                data.strFFGrade = tMod.ByteToString(typDefInf.ff.cG)
                data.strProc = tMod.ByteToString(typDefInf.ff.cProc)
                data.nTYuu = mcls_PreParam.GetTypePriority(data.strProc, data.strFFType)
                data.nGYuu = mcls_PreParam.GetGradePriority(data.strProc, data.strFFGrade)
                ' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
                data.nKouteiID = typDefInf.ff.nKouteiID
                ' 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

                If lstTcmCell(col) Is Nothing Then
                    lstTcmCell(col) = New List(Of TYP_MAP_LIST_VALUE)
                    lstTcmCell(col).Clear()
                End If

                Dim bAdd As Boolean = True
                For ii As Integer = 0 To lstTcmCell(col).Count - 1
                    If nPnt = lstTcmCell(col)(ii).nPnt Then
                        '既に登録済み
                        bAdd = False
                        Exit For
                    End If
                Next
                If bAdd Then
                    ' リスト追加
                    lstTcmCell(col).Add(data)
                End If
            Next

            '代表疵算出(優先順位でソート)
            For ii As Integer = 0 To lstTcmCell.Length - 1
                If lstTcmCell(ii) Is Nothing Then
                    Continue For
                End If
                ' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
                'lstTcmCell(ii).Sort(AddressOf SortPriorty)
                lstTcmCell(ii).Sort(AddressOf SortPriortyTcm)
                ' 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<
                If 0 < lstTcmCell(ii).Count Then
                    typRow.tcm(ii).nKizuNo = lstTcmCell(ii)(0).nKizuNo
                    typRow.tcm(ii).strFFType = lstTcmCell(ii)(0).strFFType
                    typRow.tcm(ii).strFFGrade = lstTcmCell(ii)(0).strFFGrade
                End If
            Next ii

            bRetc = True

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("マップ位置行情報取得異常[{0}]", ex.Message))
        End Try

        Return bRetc
    End Function

    ''' <summary>
    ''' 行内の優先度高の疵を取得
    ''' </summary>
    ''' <param name="nMen">面</param>
    ''' <param name="lstCell">マップ情報</param>
    ''' <param name="typRow">マップ位置</param>
    ''' <remarks>優先度高の疵の列位置</remarks>
    Private Function GetMostDeffectColumn(ByVal nMen As Integer, ByVal lstCell() As List(Of TYP_MAP_LIST_VALUE), ByVal typRow As G_MAP_ROW, ByVal nRec As Integer, Optional ByVal nExCol As Integer = -1) As Integer
        Dim nGrPos As Integer = 0
        Dim nKzPos As Integer = 0
        Dim nRetrun As Integer = -1
        Dim bPosChg As Boolean = False
        Dim nGrRank(lstCell.Length - 1) As Integer
        Dim nKzRank(lstCell.Length - 1) As Integer
        Dim nStart As Integer = 0
        Dim nEnd As Integer = lstCell.Length - 1
        Dim nStep As Integer = 1

        Try

            'DS/WS基準か疵マップ左側基準かで処理が変わる
            If m_DSPos = EM_MAP_CELL.NORMAL Then    '左がDS
                nStart = 0
                nEnd = MAP_COL_NUM - 1
                nStep = 1
            Else                                    '左がWS
                nStart = MAP_COL_NUM - 1
                nEnd = 0
                nStep = -1
            End If

            nGrPos = nStart

            '優先度取得
            For nCnt As Integer = nStart To nEnd Step nStep
                nGrRank(nCnt) = Integer.MaxValue
                nKzRank(nCnt) = Integer.MaxValue
                If nCnt = nExCol Then
                    Continue For
                End If
                If lstCell(nCnt) Is Nothing Then
                    Continue For
                End If
                If 0 = lstCell(nCnt).Count Then
                    Continue For
                End If
                nGrRank(nCnt) = mcls_Param.GetGradePriority(lstCell(nCnt)(0).nGid)
                nKzRank(nCnt) = mcls_Param.GetTypePriority(lstCell(nCnt)(0).nTid)
            Next

            For nCnt As Integer = nStart To nEnd Step nStep
                If nGrRank(nGrPos) >= nGrRank(nCnt) And nGrRank(nCnt) <> Integer.MaxValue Then
                    If nKzRank(nGrPos) >= nKzRank(nCnt) Then
                        If (nKzRank(nGrPos) = nKzRank(nCnt)) Then
                            nGrPos = nGrPos
                            bPosChg = True
                        Else
                            nGrPos = nCnt
                            bPosChg = True
                        End If
                    End If
                End If
            Next
            For nCnt As Integer = nStart To nEnd Step nStep
                If nGrRank(nGrPos) > nGrRank(nCnt) Then
                    nGrPos = nCnt
                    bPosChg = True
                End If
            Next

            '0番目のチェック
            If bPosChg = False Then
                If nKzRank(nGrPos) <> 0 And nKzRank(nGrPos) < Integer.MaxValue Then
                    nGrPos = 0
                    bPosChg = True
                End If
            End If

            If bPosChg Then
                Return nGrPos
            End If

            '疵種検査
            For nCnt As Integer = nStart To nEnd Step nStep
                If nKzRank(nGrPos) > nKzRank(nCnt) Then
                    nKzPos = nCnt
                    bPosChg = True
                End If
            Next
            If bPosChg Then
                Return nKzPos
            End If
            If Not bPosChg And nKzRank(nStart) < Integer.MaxValue Then
                Return nStart
            End If

            '同じ場合はWSに寄っていく
            For nCnt As Integer = nStart To nEnd Step nStep
                If nGrRank(nCnt) < Integer.MaxValue Then
                    Return nCnt
                End If
            Next

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("行内の優先度高の疵をセット失敗[{0}]", ex.Message))
        End Try

        Return -1

    End Function


    ''' <summary>
    ''' 疵マップグリッド描画
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvMap_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)
        Dim penYousetu As New System.Drawing.Pen(g_ColorYousetu, 2)         '溶接線
        Dim penCut As New System.Drawing.Pen(g_ColorShar, 2)                'シャーカット線
        Dim penKensa As New System.Drawing.Pen(g_ColorKensa, 2)             '検査台線
        Dim penBound As New System.Drawing.Pen(Color.Black, 2)              '境界線
        Dim penSelect(DSP_DEFECT_MAX - 1) As System.Drawing.Pen             '疵色

        Dim dgvMap As tCnt_DataGridView_turn = CType(sender, tCnt_DataGridView_turn)
        Dim penPos As Integer = 0                                           '線が重なった場合のズラシ間隔保持
        Dim penRectTop As Integer = 0                                       '枠の左上
        Dim penRectLeft As Integer = 0                                      '枠の左端
        Dim bolDeep As Boolean = False

        Dim nStart As Integer
        Dim nStep As Integer
        Dim nMax As Integer

        If EM_REFRESH_MODE.AUTO = m_emViewMode Then
            '自動時のペン
            penSelect(0) = New System.Drawing.Pen(g_ColorNewKizu, 3)
            penSelect(1) = New System.Drawing.Pen(g_ColorOldKizu, 3)
        Else
            '一時停止時のペン
            penSelect(0) = New System.Drawing.Pen(g_ColorStop1st, 3)
            penSelect(1) = New System.Drawing.Pen(g_ColorStop2nd, 3)
        End If

        Try
            '溶接線、シャーカット線の表示
            Dim nRowCount As Integer = dgvMap.DisplayedRowCount(True)
            Dim nRowTop As Integer = dgvMap.FirstDisplayedScrollingRowIndex

            For ii As Integer = 0 To nRowCount - 1
                Dim nRowIdx As Integer = nRowTop + ii
                If dgvMap(0, nRowIdx).Value Is "" Then
                    'MAP情報がない場合は飛ばす(初期化後など、表示するデータがない行の場合)
                    Continue For
                End If

                Dim typRow As G_MAP_ROW
                typRow = CType(dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, nRowIdx).Tag, G_MAP_ROW)

                '溶接点、シャーカット線 重なり判定
                If dgvMap.RowCount - 1 > nRowIdx Then
                    Dim typRowN As G_MAP_ROW
                    typRowN = CType(dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, nRowIdx + 1).Tag, G_MAP_ROW)
                    If (typRow.bYousetu And (0 < typRowN.nCut)) Or
                        (typRowN.bYousetu And (0 < typRow.nCut)) Then
                        penPos = 1
                    End If
                End If

                '溶接線表示
                If typRow.bYousetu Then
                    If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                        e.Graphics.DrawRectangle(penYousetu, 0, (dgvMap.Item(0, nRowIdx).Size.Height) * (nRowIdx - nRowTop + 2) - penPos, dgvMap.Width, 1)
                    Else
                        e.Graphics.DrawRectangle(penYousetu, 0, (dgvMap.Item(0, nRowIdx).Size.Height) * (nRowIdx - nRowTop + 1) - penPos, dgvMap.Width, 1)
                    End If
                End If

                'シャーカット線表示
                If 0 < typRow.nCut Then
                    If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                        e.Graphics.DrawRectangle(penCut, 0, (dgvMap.Item(0, nRowIdx).Size.Height) * (nRowIdx - nRowTop + 1) + penPos, dgvMap.Width, 1)
                    Else
                        e.Graphics.DrawRectangle(penCut, 0, (dgvMap.Item(0, nRowIdx).Size.Height) * (nRowIdx - nRowTop + 2) + penPos, dgvMap.Width, 1)
                    End If
                End If
            Next

            '検査台の表示
            Dim bKensaLbl As Boolean = False
            Dim nMen As Integer = 0 ' 検査台 表面
            If EM_DIV_DISP.DIV_LEN_POS_DSP_BOT = m_nKensaNo Then  ' 検査台 裏面
                nMen = 1
            End If

            If EM_REFRESH_MODE.AUTO = m_emViewMode And m_bKensaDsp Then
                m_typMapInf.lblKensaPoint.BackColor = g_ColorKensa

                Dim nKensaRow As Integer = 0

                ' 20201010 検査台位置表示変更 --->>>
                'If m_emStage = EM_LONG_POS.BIG Then
                '    nKensaRow = CInt(mcls_Param.PosKensa(nMen) \ mcls_Param.MapStageBig)
                'Else
                '    nKensaRow = CInt(mcls_Param.PosKensa(nMen) \ mcls_Param.MapStageSmall)
                'End If
                nKensaRow = KensaPos - 1
                ' 20201010 検査台位置表示変更 <<<---
                penKensa.DashStyle = Drawing2D.DashStyle.Dash
                If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                    If nRowTop <= nKensaRow And (nRowTop + nRowCount) > nKensaRow Then
                        e.Graphics.DrawRectangle(penKensa, 0, (dgvMap.Item(0, 0).Size.Height) * (nKensaRow - nRowTop + 1), dgvMap.Width, 1)
                        m_typMapInf.lblKensaPoint.Top = (dgvMap.Item(0, 0).Size.Height) * (nKensaRow - nRowTop + 1)
                        bKensaLbl = True
                    End If
                Else
                    nKensaRow = dgvMap.RowCount - nKensaRow
                    If nRowTop <= nKensaRow And (nRowTop + nRowCount) >= nKensaRow Then
                        e.Graphics.DrawRectangle(penKensa, 0, (dgvMap.Item(0, 0).Size.Height) * (nKensaRow - nRowTop + 1), dgvMap.Width, 1)
                        If nRowTop < nKensaRow Then
                            m_typMapInf.lblKensaPoint.Top = (dgvMap.Item(0, 0).Size.Height) * (nKensaRow - nRowTop)
                            bKensaLbl = True
                        End If
                    End If
                End If
            End If
            m_typMapInf.lblKensaPoint.Visible = bKensaLbl
            m_typMapInf.lblKensaPoint.Refresh()


            '選択疵表示処理
            If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                nStart = nRowTop + nRowCount - 1
                nStep = -1
                nMax = nRowTop
            Else
                nStart = nRowTop
                nStep = 1
                nMax = nRowTop + nRowCount - 1
            End If

            For nRowCnt As Integer = nStart To nMax Step nStep
                For nCol As Integer = EM_MAP_COL_INF.COL_1 To EM_MAP_COL_INF.COL_8
                    If EM_MAP_COL_INF.IRIGAWA = nCol Then Continue For
                    If EM_MAP_COL_INF.KIZUNUM = nCol Then Continue For

                    Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(dgvMap.Item(nCol, nRowCnt).Tag, List(Of TYP_MAP_LIST_VALUE))
                    If lstVal IsNot Nothing Then

                        Dim bExist(2) As Boolean
                        bExist(0) = False
                        bExist(1) = False
                        For ii As Integer = 0 To lstVal.Count - 1
                            If lstVal(ii) = m_typSelectValue(0) Then
                                bExist(0) = True
                            End If
                            If lstVal(ii) = m_typSelectValue(1) Then
                                bExist(1) = True
                            End If
                        Next

                        If penSelect(1) IsNot Nothing _
                        And bExist(1) Then
                            '選択疵の枠描画
                            penRectTop = dgvMap(nCol, nRowCnt).Size.Height * (dgvMap(nCol, nRowCnt).RowIndex - nRowTop + 1)
                            penRectLeft = dgvMap(0, nRowCnt).Size.Width + dgvMap(nCol, nRowCnt).Size.Width * (nCol - 1) + 10
                            e.Graphics.DrawRectangle(penSelect(1), penRectLeft - 8, penRectTop, dgvMap.Item(nCol, nRowCnt).Size.Width - 1, dgvMap.Item(nCol, nRowCnt).Size.Height - 1)
                        End If

                        If penSelect(0) IsNot Nothing _
                        And bExist(0) Then
                            '選択疵の枠描画
                            penRectTop = dgvMap(nCol, nRowCnt).Size.Height * (dgvMap(nCol, nRowCnt).RowIndex - nRowTop + 1)
                            penRectLeft = dgvMap(0, nRowCnt).Size.Width + dgvMap(nCol, nRowCnt).Size.Width * (nCol - 1) + 10
                            e.Graphics.DrawRectangle(penSelect(0), penRectLeft - 8, penRectTop, dgvMap.Item(nCol, nRowCnt).Size.Width - 1, dgvMap.Item(nCol, nRowCnt).Size.Height - 1)
                        End If

                    End If
                Next
            Next nRowCnt

            penRectLeft = 0
            For nCol As Integer = EM_MAP_COL_INF.DEGAWA To EM_MAP_COL_INF.OTH1
                Select Case nCol
                    Case EM_MAP_COL_INF.KIZUNUM
                        e.Graphics.DrawLine(penBound, penRectLeft + 1, dgvMap.Top, penRectLeft + 1, dgvMap.Height)
                    Case EM_MAP_COL_INF.OTH1
                        e.Graphics.DrawLine(penBound, penRectLeft + 1, dgvMap.Top, penRectLeft + 1, dgvMap.Height)
                End Select
                penRectLeft = penRectLeft + dgvMap(nCol, 0).Size.Width
            Next

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("疵マップグリッド描画異常[{0}]", ex.Message))
        Finally
            If Not penYousetu Is Nothing Then penYousetu.Dispose()
            If Not penCut Is Nothing Then penCut.Dispose()
            If Not penKensa Is Nothing Then penKensa.Dispose()
            If Not penSelect(0) Is Nothing Then penSelect(0).Dispose()
            If Not penSelect(1) Is Nothing Then penSelect(1).Dispose()
            If Not penBound Is Nothing Then penBound.Dispose()
        End Try
    End Sub


    ''' <summary>
    ''' 疵マップ位置検索
    ''' </summary>
    ''' <param name="typVal"></param>
    ''' <param name="nRowR"></param>
    ''' <param name="nColR"></param>
    ''' <returns></returns>
    Private Function GetMapPos(ByVal typVal As TYP_MAP_LIST_VALUE, ByRef nRowR As Integer, ByRef nColR As Integer) As Boolean

        Dim nStartRow As Integer = 0
        Dim nEndRow As Integer = m_typMapInf.dgvMap.RowCount - 1
        Dim nStep As Integer = 1

        If 0 > typVal.nPnt Then
            Return False
        End If

        If EM_MAP_SCROOL.TAIL_TO_HEAD = MapDir Then
            nStartRow = m_typMapInf.dgvMap.RowCount - 1
            nEndRow = 0
            nStep = -1
        End If

        For nRow As Integer = nStartRow To nEndRow Step nStep
            For nCol As Integer = EM_MAP_COL_INF.COL_1 To EM_MAP_COL_INF.COL_8
                Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(m_typMapInf.dgvMap.Item(nCol, nRow).Tag, List(Of TYP_MAP_LIST_VALUE))
                If lstVal Is Nothing Then
                    Continue For
                End If
                For ii As Integer = 0 To lstVal.Count - 1
                    If typVal = lstVal(ii) Then
                        nRowR = nRow
                        nColR = nCol
                        Return True
                    End If
                Next
            Next
        Next

        Return False

    End Function

    ''' <summary>
    ''' TCMマップ位置検索
    ''' </summary>
    ''' <param name="typVal"></param>
    ''' <param name="nRowR"></param>
    ''' <param name="nColR"></param>
    ''' <returns></returns>
    Private Function GetMapTcmPos(ByVal typVal As TYP_MAP_LIST_VALUE, ByRef nRowR As Integer, ByRef nColR As Integer) As Boolean

        Dim nStartRow As Integer = 0
        Dim nEndRow As Integer = m_typMapInf.dgvMap.RowCount - 1
        Dim nStep As Integer = 1

        If 0 > typVal.nPnt Then
            Return False
        End If

        If EM_MAP_SCROOL.TAIL_TO_HEAD = MapDir Then
            nStartRow = m_typMapInf.dgvMap.RowCount - 1
            nEndRow = 0
            nStep = -1
        End If

        For nRow As Integer = nStartRow To nEndRow Step nStep
            For nCol As Integer = EM_MAP_COL_INF.OTH1 To EM_MAP_COL_INF.OTH1
                Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(m_typMapInf.dgvMap.Item(nCol, nRow).Tag, List(Of TYP_MAP_LIST_VALUE))
                If lstVal Is Nothing Then
                    Continue For
                End If
                For ii As Integer = 0 To lstVal.Count - 1
                    If typVal = lstVal(ii) Then
                        nRowR = nRow
                        nColR = nCol
                        Return True
                    End If
                Next
            Next
        Next

        Return False

    End Function

    ''' <summary>
    ''' 疵マップセル選択(マウスクリックイベント時)
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub dgvMap_CellMouseDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs)
        '無効行
        If e.RowIndex < 0 Then
            Exit Sub
        End If

        If EM_MAP_COL_INF.OTH1 = e.ColumnIndex Then
            dgvMap_TcmCellMouseDown(sender, e)
            Exit Sub
        End If

        Dim dgvTmp As tCnt_DataGridView_turn = CType(sender, tCnt_DataGridView_turn)
        Dim dgvCell As DataGridViewCell = dgvTmp.Item(e.ColumnIndex, e.RowIndex)


        Try

            '出側、入側セルは無効
            If EM_MAP_COL_INF.COL_1 > dgvCell.ColumnIndex Then
                Exit Try
            End If
            If EM_MAP_COL_INF.COL_8 < dgvCell.ColumnIndex Then
                Exit Try
            End If

            '自動更新中は無効
            If EM_REFRESH_MODE.AUTO = m_emViewMode Then
                Exit Try
            End If

            '右クリックは無効
            If e.Button = MouseButtons.Right Then
                Exit Try
            End If

            '疵がないなら無効
            If dgvCell.Value Is Nothing Then
                Exit Try
            End If
            If dgvCell.Value.ToString = "" Then
                Exit Try
            End If

            '選択セルの疵情報取得
            Dim listVal As List(Of TYP_MAP_LIST_VALUE) = CType(dgvCell.Tag, List(Of TYP_MAP_LIST_VALUE))
            If listVal Is Nothing Then
                Exit Try
            End If

            '同一疵の場合は選択しない
            Dim bSelect As Boolean = True
            If m_typSelectValue(0) = listVal(0) Then
                bSelect = False
            End If

            If e.Button = MouseButtons.Left Then
                If bSelect Then
                    '+++++++++++++++++++
                    '選択疵
                    '+++++++++++++++++++
                    For ii As Integer = m_typSelectValue.Length - 1 To 1 Step -1
                        m_typSelectValue(ii) = m_typSelectValue(ii - 1)
                    Next
                    '選択疵を現在疵へ
                    m_typSelectValue(0) = listVal(0)

                End If

                'クリックイベント発行
                RaiseEvent SelectMapCell_Click(Men, listVal(0), bSelect)

                '疵リスト表示
                SetDeepList(m_typMapInf.dgvDeep, listVal)
                '選択外となったときリストを初期化
                LstSelectClear()

            End If
        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("疵マップセル選択処理異常[{0}]", ex.Message))
        End Try

        dgvTmp.Refresh()

    End Sub

    ''' <summary>
    ''' 深さリスト設定
    ''' </summary>
    ''' <param name="dgv">深さグリッドオブジェクト</param>
    ''' <param name="lstVal">セル情報構造体</param>
    ''' <param name="bTop">True:先頭表示、false:末尾表示</param>
    ''' <returns>表示結果 True:設定完了、False:設定未完了</returns>
    ''' <remarks></remarks>
    Private Function SetDeepList(ByVal dgv As tCnt_DataGridView_turn, ByVal lstVal As List(Of TYP_MAP_LIST_VALUE), Optional ByVal bTop As Boolean = True) As Boolean
        Dim bRet As Boolean = False

        Try
            dgv.Tag = lstVal
            dgv.Rows.Clear()                                            ' 一旦表示行をクリアする
            If lstVal Is Nothing Then
                Exit Try
            End If
            dgv.RowCount = lstVal.Count                        ' 疵深さセット
            For nCnt As Integer = 0 To dgv.RowCount - 1
                dgv(EM_DEEP_LIST_COL.emNo, nCnt).Value = nCnt + 1
                dgv(EM_DEEP_LIST_COL.emGr, nCnt).Value = mcls_Param.GetGradeName(lstVal(nCnt).nGid)
                dgv(EM_DEEP_LIST_COL.emGr, nCnt).Style.BackColor = mcls_Param.GetGradeColor(lstVal(nCnt).nGid)
                dgv(EM_DEEP_LIST_COL.emType, nCnt).Value = mcls_Param.GetMapName(lstVal(nCnt).nTid)
            Next
            dgv.Columns(EM_DEEP_LIST_COL.emType).HeaderText = "疵リスト(" & lstVal.Count.ToString("#,###") & ")"

            If Not bTop AndAlso dgv.RowCount > MAX_MAPDEFLST_ROW Then
                dgv.FirstDisplayedScrollingRowIndex = dgv.RowCount - MAX_MAPDEFLST_ROW
            End If

            bRet = True
        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("深さリスト設定異常[{0}]", ex.Message))
        End Try

        dgv.Refresh()
        Return bRet
    End Function

    ''' <summary>
    ''' 深さリスト描画ペイント
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks>深さリスト表示時の選択疵の枠描画処理</remarks>
    Public Sub dgvDeep_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)
        Dim dgv As tCnt_DataGridView = CType(sender, tCnt_DataGridView)
        Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(dgv.Tag, List(Of TYP_MAP_LIST_VALUE))
        Dim penSelect(2 - 1) As System.Drawing.Pen                          '疵色

        penSelect(0) = New System.Drawing.Pen(g_ColorStop1st, 2)            '欠陥1
        penSelect(1) = New System.Drawing.Pen(g_ColorStop2nd, 2)            '欠陥2

        Try

            Dim nTopRow As Integer = dgv.FirstDisplayedScrollingRowIndex
            For ii As Integer = 0 To MAX_MAPDEFLST_ROW
                Dim nRowCnt As Integer = nTopRow + ii
                If nRowCnt > dgv.RowCount - 1 Then
                    Exit For
                End If
                If 0 > nRowCnt Then
                    Exit For
                End If
                If lstVal(nRowCnt) = m_typSelectValue(1) Then
                    dgv.Rows(nRowCnt).DefaultCellStyle.BackColor = g_ColorStop2nd
                    dgv.Rows(nRowCnt).DefaultCellStyle.SelectionBackColor = g_ColorStop2nd
                ElseIf lstVal(nRowCnt) = m_typSelectValue(0) Then
                    dgv.Rows(nRowCnt).DefaultCellStyle.BackColor = g_ColorStop1st
                    dgv.Rows(nRowCnt).DefaultCellStyle.SelectionBackColor = g_ColorStop1st
                Else
                    dgv.Rows(nRowCnt).DefaultCellStyle.BackColor = Color.Empty
                    dgv.Rows(nRowCnt).DefaultCellStyle.SelectionBackColor = Color.Empty
                End If
            Next ii

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("深さリストペイント表示エラー[{0}]", ex.Message))
        Finally
            If Not penSelect(0) Is Nothing Then penSelect(0).Dispose()
            If Not penSelect(1) Is Nothing Then penSelect(1).Dispose()
        End Try
    End Sub

    ''' <summary>
    ''' 深さリストセル選択
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub dgvDeep_CellMouseDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs)
        Dim dgvTmp As DataGridView = CType(sender, DataGridView)
        Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(dgvTmp.Tag, List(Of TYP_MAP_LIST_VALUE))

        Try
            If lstVal Is Nothing Then
                Exit Try
            End If

            If 0 > e.RowIndex Or lstVal.Count <= e.RowIndex Then
                Exit Try
            End If

            Dim bSelect As Boolean = True
            If m_typSelectValue(0) = lstVal(e.RowIndex) Then
                '同一疵の場合は選択しない
                bSelect = False
            End If

            '一時停止中、左クリック時
            If EM_REFRESH_MODE.PAUSE = m_emViewMode And e.Button = MouseButtons.Left Then
                If bSelect Then
                    For ii As Integer = m_typSelectValue.Length - 1 To 1 Step -1
                        m_typSelectValue(ii) = m_typSelectValue(ii - 1)
                    Next
                    m_typSelectValue(0) = lstVal(e.RowIndex)

                    '選択外となったときリストを初期化
                    LstSelectClear()
                End If

                'イベント発行
                RaiseEvent SelectMapCell_Click(Men, lstVal(e.RowIndex), bSelect)
            End If

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("深さリストセル選択異常[{0}]", ex.Message))
        End Try

        dgvTmp.Refresh()
        m_typMapInf.dgvMap.Refresh()
    End Sub

    ''' <summary>
    ''' TCMマップセル選択(マウスクリックイベント時)
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub dgvMap_TcmCellMouseDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs)
        '無効行
        If e.RowIndex < 0 Then
            Exit Sub
        End If

        Dim dgvTmp As tCnt_DataGridView_turn = CType(sender, tCnt_DataGridView_turn)
        Dim dgvCell As DataGridViewCell = dgvTmp.Item(e.ColumnIndex, e.RowIndex)


        Try

            'TCMマップ以外は無効
            If EM_MAP_COL_INF.OTH1 <> dgvCell.ColumnIndex Then
                Exit Try
            End If

            '自動更新中は無効
            If EM_REFRESH_MODE.AUTO = m_emViewMode Then
                Exit Try
            End If

            '右クリックは無効
            If e.Button = MouseButtons.Right Then
                Exit Try
            End If

            '疵がないなら無効
            If dgvCell.Value Is Nothing Then
                Exit Try
            End If
            If dgvCell.Value.ToString = "" Then
                Exit Try
            End If

            '選択セルの疵情報取得
            Dim listVal As List(Of TYP_MAP_LIST_VALUE) = CType(dgvCell.Tag, List(Of TYP_MAP_LIST_VALUE))
            If listVal Is Nothing Then
                Exit Try
            End If

            '同一疵の場合は選択しない
            Dim bSelect As Boolean = True
            'If m_typTcmSelectValue = listVal(0) Then
            '    bSelect = False
            'End If

            If e.Button = MouseButtons.Left Then
                If bSelect Then
                    '+++++++++++++++++++
                    '選択疵
                    '+++++++++++++++++++
                    '選択疵を現在疵へ
                    m_typTcmSelectValue = listVal(0)

                    'クリックイベント発行
                    RaiseEvent SelectMapTcmCell_Click(Men, listVal(0))

                End If

                '疵リスト表示
                SetTcmList(m_typMapInf.dgvTcm, listVal)

            End If
        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("疵マップセル選択処理異常[{0}]", ex.Message))
        End Try

        dgvTmp.Refresh()

    End Sub

    ''' <summary>
    ''' TCMリストセル選択
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub dgvTcm_CellMouseDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs)
        Dim dgvTmp As DataGridView = CType(sender, DataGridView)
        Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(dgvTmp.Tag, List(Of TYP_MAP_LIST_VALUE))

        Try
            If lstVal Is Nothing Then
                Exit Try
            End If

            If 0 > e.RowIndex Or lstVal.Count <= e.RowIndex Then
                Exit Try
            End If

            If m_typTcmSelectValue = lstVal(e.RowIndex) Then
                '同一疵の場合は選択しない
                Exit Try
            End If

            '一時停止中、左クリック時
            If EM_REFRESH_MODE.PAUSE = m_emViewMode And e.Button = MouseButtons.Left Then
                m_typTcmSelectValue = lstVal(e.RowIndex)
                'イベント発行
                RaiseEvent SelectMapTcmCell_Click(Men, lstVal(e.RowIndex))
            End If

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("深さリストセル選択異常[{0}]", ex.Message))
        End Try

        dgvTmp.Refresh()
        m_typMapInf.dgvMap.Refresh()
    End Sub

    ''' <summary>
    ''' TCMリスト設定
    ''' </summary>
    ''' <param name="dgv">深さグリッドオブジェクト</param>
    ''' <param name="lstVal">セル情報構造体</param>
    ''' <returns>表示結果 True:設定完了、False:設定未完了</returns>
    ''' <remarks></remarks>
    Private Function SetTcmList(ByVal dgv As tCnt_DataGridView_turn, ByVal lstVal As List(Of TYP_MAP_LIST_VALUE)) As Boolean
        Dim bRet As Boolean = False

        Try
            dgv.Tag = lstVal
            dgv.Rows.Clear()                                            ' 一旦表示行をクリアする
            If lstVal Is Nothing Then
                Exit Try
            End If
            dgv.RowCount = lstVal.Count                        ' 疵深さセット
            For nCnt As Integer = 0 To dgv.RowCount - 1
                dgv(EM_TCM_LIST_COL.emNo, nCnt).Value = nCnt + 1
                dgv(EM_TCM_LIST_COL.emLine, nCnt).Value = mcls_PreParam.GetProcName(lstVal(nCnt).strProc)
                dgv(EM_TCM_LIST_COL.emGr, nCnt).Value = mcls_PreParam.GetDspGradeName(lstVal(nCnt).strProc, lstVal(nCnt).strFFGrade)
                dgv(EM_TCM_LIST_COL.emGr, nCnt).Style.BackColor = tMod.GetIntToColor(mcls_PreParam.GetDspGradeColor(lstVal(nCnt).strProc, lstVal(nCnt).strFFGrade))
                dgv(EM_TCM_LIST_COL.emType, nCnt).Value = mcls_PreParam.GetDspTypeName(lstVal(nCnt).strProc, lstVal(nCnt).strFFType)
            Next
            dgv.Columns(EM_TCM_LIST_COL.emType).HeaderText = "TCM(" & lstVal.Count.ToString("#,###") & ")"
            bRet = True
        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("深さリスト設定異常[{0}]", ex.Message))
        End Try

        dgv.Refresh()
        Return bRet
    End Function

    ''' <summary>
    ''' 優先順位によるソート関数
    ''' </summary>
    ''' <param name="a"></param>
    ''' <param name="b"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function SortPriorty(ByVal a As TYP_MAP_LIST_VALUE, ByVal b As TYP_MAP_LIST_VALUE) As Integer
        ' 優先順位
        ' ① グレード優先度
        ' ② 疵種優先度
        ' ③ 面積順
        ' ④ 長手順
        ' ⑤ 疵No
        Dim dNow(5) As Double
        Dim dPer(5) As Double
        Dim bMode(5) As Boolean


        dNow(0) = CDbl(a.nGYuu)
        dPer(0) = CDbl(b.nGYuu)
        bMode(0) = False

        dNow(1) = CDbl(a.nTYuu)
        dPer(1) = CDbl(b.nTYuu)
        bMode(1) = False

        dNow(2) = CDbl(a.nAria)
        dPer(2) = CDbl(b.nAria)
        bMode(2) = True

        dNow(3) = CDbl(a.nY)
        dPer(3) = CDbl(b.nY)
        bMode(3) = False

        dNow(4) = CDbl(a.nKizuNo)
        dPer(4) = CDbl(b.nKizuNo)
        bMode(4) = False

        If dNow(0) = dPer(0) And dNow(1) = dPer(1) And dNow(2) = dPer(2) And dNow(3) = dPer(3) And dNow(4) = dPer(4) Then
            Return 0
        End If
        ' 優先度比較
        If Not SelectPriorityDetect(dNow, dPer, bMode) Then
            Return 1
        Else
            Return -1
        End If
    End Function

    ' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
    ''' <summary>
    ''' 優先順位によるソート関数(TCM用)
    ''' </summary>
    ''' <param name="a"></param>
    ''' <param name="b"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function SortPriortyTcm(ByVal a As TYP_MAP_LIST_VALUE, ByVal b As TYP_MAP_LIST_VALUE) As Integer
        ' 優先順位
        ' ① 工程種別(TCM穴(4)＞TCM疵(6))
        ' ② グレード優先度
        ' ③ 疵種優先度
        ' ④ 面積順
        ' ⑤ 長手順
        ' ⑥ 疵No
        Dim dNow(6) As Double
        Dim dPer(6) As Double
        Dim bMode(6) As Boolean

        dNow(0) = CDbl(a.nKouteiID)
        dPer(0) = CDbl(b.nKouteiID)
        bMode(0) = False

        dNow(1) = CDbl(a.nGYuu)
        dPer(1) = CDbl(b.nGYuu)
        bMode(1) = False

        dNow(2) = CDbl(a.nTYuu)
        dPer(2) = CDbl(b.nTYuu)
        bMode(2) = False

        dNow(3) = CDbl(a.nAria)
        dPer(3) = CDbl(b.nAria)
        bMode(3) = True

        dNow(4) = CDbl(a.nY)
        dPer(4) = CDbl(b.nY)
        bMode(4) = False

        dNow(5) = CDbl(a.nKizuNo)
        dPer(5) = CDbl(b.nKizuNo)
        bMode(5) = False

        If dNow(0) = dPer(0) And dNow(1) = dPer(1) And dNow(2) = dPer(2) And dNow(3) = dPer(3) And dNow(4) = dPer(4) And dNow(5) = dPer(5) Then
            Return 0
        End If
        ' 優先度比較
        If Not SelectPriorityDetect(dNow, dPer, bMode) Then
            Return 1
        Else
            Return -1
        End If
    End Function
    ' 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

    ''' <summary>
    ''' スクロール量を取得
    ''' </summary>
    ''' <returns>スクロール量</returns>
    ''' <remarks></remarks>
    Public Function GetScrollMax() As Integer
        Try
            If m_nMaxRowCnt - m_typMapInf.vsbMap.LargeChange > 0 Then
                Return m_nMaxRowCnt - m_typMapInf.vsbMap.LargeChange
            End If
        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("スクロール量を取得失敗[{0}]", ex.Message))
        End Try
        Return 0
    End Function


    ''' <summary>
    ''' マップから指定された行の入側距離を取得します
    ''' </summary>
    ''' <returns>入側距離[m] 失敗時:-1</returns>
    ''' <remarks></remarks>
    Public Function GetInCoilLen() As Integer
        Dim nLen As Integer = -1
        Dim row As Integer

        Try
            '検査台の行数取得
            Dim nKensaRow As Integer = GetKensaRow()
            If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                row = 0 + nKensaRow
            Else
                row = m_typMapInf.dgvMap.RowCount - 1 - nKensaRow
            End If

            If row < 0 Then
                Return row
            End If

            Dim obj As Object = m_typMapInf.dgvMap(EM_MAP_COL_INF.IRIGAWA, row).Value
            If obj Is Nothing = True Then
                Return -1
            End If

            Dim strLen As String = obj.ToString()
            If Integer.TryParse(strLen, nLen) = False Then
                nLen = -1
            End If
        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("マップからの出側距離取得異常[{0}]", ex.Message))
            nLen = -1
        End Try

        Return nLen
    End Function

    ''' <summary>
    ''' マップ列数取得
    ''' </summary>
    ''' <param name="nRec">レコード</param>
    ''' <returns></returns>
    Private Function GetMapColNum(ByVal nRec As Integer) As Integer
        Dim coilinf As COINFO_BASE = Nothing
        coilinf.initialize()
        If mcls_CoilInf.GetCoilInf(nRec, coilinf) Then
            Return coilinf.nMapColNum
        End If
        Return MAP_COL_NUM
    End Function


    ''' <summary>
    ''' マップの縦サイズ変更
    ''' </summary>
    Public Sub MapResizeH()
        Dim dgv As tCnt.tCnt_DataGridView = m_typMapInf.dgvMap
        Dim count As Integer = dgv.DisplayedRowCount(False)
        If (dgv.RowCount < count) Then Exit Sub

        ' マップスクロールの移動サイズ(大)は、表示行の分だけ
        Dim nLastLarge As Integer = m_typMapInf.vsbMap.LargeChange
        m_typMapInf.vsbMap.LargeChange = count

        ' 開始行を可視できる位置に変更する
        If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
            ' 上→下の場合は先頭(0)
            dgv.FirstDisplayedScrollingRowIndex = 0
        Else
            ' 下→上の場合は、可視終端行が固定して見えるように開始位置を調整
            dgv.FirstDisplayedScrollingRowIndex = dgv.RowCount - count
            If nLastLarge > m_typMapInf.vsbMap.LargeChange Then
                m_typMapInf.vsbMap.ValueProgramable = m_typMapInf.vsbMap.Value + (nLastLarge - m_typMapInf.vsbMap.LargeChange)
            End If
        End If
    End Sub

    ''' <summary>
    ''' 選択疵を取得
    ''' </summary>
    ''' <param name="n"></param>
    ''' <returns></returns>
    Public Function GetSelectKizu(ByVal n As Integer, ByRef strVal As TYP_MAP_LIST_VALUE) As Boolean
        If 0 > n Or m_typSelectValue.Length <= n Then
            Return False
        End If
        strVal = m_typSelectValue(n)
        Return True
    End Function


    ''' <summary>
    ''' TCM選択疵を取得
    ''' </summary>
    ''' <returns></returns>
    Public Function GetTcmSelectKizu(ByRef strVal As TYP_MAP_LIST_VALUE) As Boolean
        strVal = m_typTcmSelectValue
        Return True
    End Function

    ''' <summary>
    ''' マップ位置行情報取得
    ''' </summary>
    ''' <param name="nRec">レコード(1オリジン)</param>
    ''' <param name="nMen">面[0:表,1:裏]</param>
    ''' <param name="nRow">行位置(0オリジン)</param>
    ''' <param name="nStage">ステージ[0:最小,1:小,2:大]</param>
    ''' <param name="typMapRow_Row">行情報</param>
    ''' <returns>TRUE:成功,FALSE:失敗</returns>
    ''' <remarks></remarks>
    Private Function GetMapRow_Row(ByVal nRec As Integer, ByVal nMen As Integer, ByVal nRow As Integer, ByVal nStage As Integer, ByRef typMapRow_Row As MAP_ROW_ROW) As Boolean
        Dim nReadAddr As Integer = 0                           '読み込みアドレス
        Dim nRetc As ValueType = Nothing                    'リターンコード
        Dim bRet As Boolean = False

        Try
            '初期化
            typMapRow_Row.initialize()

            '無効レコード
            If nRec <= 0 Or nRec > TBL_HY_REC_MAX Then
                mcls_Log.Write(LNO.ERR, String.Format("[{0}]無効レコード[{1}]", TBL_HY_MAP_ROW_NAME, nRec))
                Exit Try
            End If

            '無効面
            If nMen < 0 Or nMen >= NUM_MEN Then
                mcls_Log.Write(LNO.ERR, String.Format("[{0}]無効面[{1}]", TBL_HY_MAP_ROW_NAME, nMen))
                Exit Try
            End If

            '無効ステージ
            If nStage < 0 Or nStage >= MAP_STAGE_NUM Then
                mcls_Log.Write(LNO.ERR, String.Format("[{0}]無効ステージ[{1}]", TBL_HY_MAP_ROW_NAME, nStage))
                Exit Try
            End If

            '無効行
            If nRow < 0 Or nRow >= MAP_ROW_NUM Then
                mcls_Log.Write(LNO.ERR, String.Format("[{0}]無効行[{1}]", TBL_HY_MAP_ROW_NAME, nRow))
                Exit Try
            End If

            '読み込みアドレス
            nReadAddr = SIZE_DSP_MAP_ROW * (nRec - 1) + SIZE_MAP_ROW_STAGE * nStage _
            + SIZE_MAP_ROW_MEN * nMen + SIZE_MAP_ROW_ROW * nRow

            '共有メモリから行情報読み込み
            typMapRow_Row = CType(ksMemReadMap(TBL_HY_MAP_ROW_NAME, nReadAddr, tMod_MemSet.SizeOf(typMapRow_Row), GetType(MAP_ROW_ROW), nRetc), MAP_ROW_ROW)
            If Not CBool(nRetc) Then
                mcls_Log.Write(LNO.ERR, String.Format("[{0}]行情報読み込み失敗[{1}]", TBL_HY_MAP_ROW_NAME, nReadAddr))
                Exit Try
            End If
            '取得成功
            bRet = True

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("マップ位置行情報取得異常[{0}] nRec[{1}] nRow[{2}] nReadAddr[{3}]", ex.Message, nRec, nRow, nReadAddr))
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 深さリスト再描画
    ''' </summary>
    Public Sub ReDrawDeepList()

        If 0 < m_typSelectValue(0).nPnt Then
            Dim nRow As Integer
            Dim nCol As Integer
            Dim bDeepDisp As Boolean = GetMapPos(m_typSelectValue(0), nRow, nCol)
            If bDeepDisp Then
                Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(m_typMapInf.dgvMap.Item(nCol, nRow).Tag, List(Of TYP_MAP_LIST_VALUE))
                ' 深さリストの表示
                SetDeepList(m_typMapInf.dgvDeep, lstVal)
                Exit Sub
            End If
        End If

    End Sub

    ''' <summary>
    ''' 検査台位置コイル情報レコード取得(トラッキング用)
    ''' </summary>
    ''' <returns></returns>
    Public Function GetKensaPosCoilRec() As Integer
        Dim nRec As Integer = 0
        Dim nPos As Integer = 0
        Dim nKensaRow As Integer = GetKensaRow()

        Dim count As Integer = m_typMapInf.dgvMap.RowCount
        '検査台の行数取得
        If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
            nPos = 0 + nKensaRow
        Else
            nPos = count - 1 - nKensaRow
        End If
        If nPos < 0 Then nPos = 0
        If nPos > (count - 1) Then nPos = (count - 1)
        nRec = CType(m_typMapInf.dgvMap(EM_MAP_COL_INF.IRIGAWA, nPos).Tag, G_MAP_ROW).nRec

        Return nRec
    End Function

    ''' <summary>
    ''' 検査台の行数取得
    ''' </summary>
    ''' <returns></returns>
    Private Function GetKensaRow() As Integer
        Dim nRow As Integer = 0
        Dim nMen As Integer = 0 ' 検査台 表面

        If EM_DIV_DISP.DIV_LEN_POS_DSP_BOT = m_nKensaNo Then  ' 検査台 裏面
            nMen = 1
        End If

        If EM_DEFECT_MODE.TRACKING = MapUpdateMode Then
            ' 20201010 検査台位置表示変更 --->>>
            ' 距離より算出するのをやめ、iniファイルに定義された値を使用する。
            'If m_emStage = EM_LONG_POS.BIG Then
            '    nRow = CInt(mcls_Param.PosKensa(nMen) \ mcls_Param.MapStageBig)
            'Else
            '    nRow = CInt(mcls_Param.PosKensa(nMen) \ mcls_Param.MapStageSmall)
            'End If
            nRow = KensaPos - 1
            ' 20201010 検査台位置表示変更 <<<---
        Else
            'リアルタイム表示時は無し
            nRow = 0
        End If
        Return nRow
    End Function

    ''' <summary>
    ''' 選択疵の位置
    ''' </summary>
    ''' <returns>0:検査台前、1:検査台後</returns>
    ''' <remarks></remarks>
    Public Function GetSelectKizuPos(ByVal nSelect As Integer) As Integer

        If 0 >= m_typSelectValue(nSelect).nPnt Then
            ' 選択無しの場合
            Return EM_SELECT_POS.emKensaAto
        End If

        Return m_posSelectValue(nSelect)
    End Function

    ' 20201010 検査台位置表示変更 --->>>
    ''' <summary>
    ''' トラッキング用表示開始位置取得
    ''' </summary>
    ''' <param name="nMen">表示面(0:表面、1:裏面)</param>
    ''' <param name="nRec">表示開始レコード(1オリジン、=0は無し)</param>
    ''' <param name="nRow">表示開始行位置(1オリジン)</param>
    ''' <returns></returns>
    Private Function GetDispStartRecAndRow(ByVal nMen As Integer, ByRef nRec As Integer, ByRef nRow As Integer) As Boolean
        Dim nStage As Integer = CInt(Stage)

        '初期化
        nRec = 0
        nRow = 0

        '検査台の現在表示レコード取得
        Dim nDispRec As Integer = mcls_PointInf.RecDisp(m_nKensaNo)
        If 0 = nDispRec Then
            Return False
        End If

        Dim nCheckRec As Integer = nDispRec                            '表示レコード
        Dim nSa As Integer
        Dim nRowPos As Integer = 0              ' 画面表示開始行
        Dim LPosBase As Integer = mcls_PointInf.RowLast(nDispRec, nStage)                 ' 検査完了行
        Dim WPosBase As Integer = mcls_PointInf.RowWrite(nDispRec, nMen, nStage) + 1      ' 現在書き込み行
        Dim DPosBase As Integer = mcls_PointInf.RowDisp(nDispRec, m_nKensaNo, nStage)     ' 現在表示行

        'トラッキング表示の場合は、表示基点～検査台位置 と 検査台表示行の差分ずらす。
        If EM_DEFECT_MODE.TRACKING = m_emMapMode Then                       ' トラッキング表示
            '表面基準の裏面表示は表の検査台位置を使用する
            Dim nMenWK As Integer = 0 ' 検査台 表面
            If EM_DIV_DISP.DIV_LEN_POS_DSP_BOT = m_nKensaNo Then  ' 検査台 裏面
                nMenWK = 1
            End If
            ' 表示基点からのずらし量算出
            If m_emStage = EM_LONG_POS.BIG Then
                nSa = KensaPos - CInt(mcls_Param.PosKensa(nMenWK) \ mcls_Param.MapStageBig) - 1
            Else
                nSa = KensaPos - CInt(mcls_Param.PosKensa(nMenWK) \ mcls_Param.MapStageSmall) - 1
            End If

            Dim nPosBase As Integer
            If 0 <> LPosBase Then
                nPosBase = LPosBase
            Else
                nPosBase = WPosBase
            End If

            nRec = nDispRec
            nRow = DPosBase + nSa

            Dim nRowZan As Integer = DPosBase + nSa - nPosBase      '表示する行が埋まっていない行数

            If 0 < nSa Then
                'まだ表示する行が埋まっていない場合
                If 0 < nRowZan Then
                    nCheckRec = nDispRec
                    For jj As Integer = 0 To TBL_HY_REC_MAX - 1
                        '次回表示レコード取得
                        nCheckRec = clsPointInf.GetNextRecNo(nCheckRec)
                        ' 画面データ無しの場合は、検索終了
                        If mcls_PointInf.DispStatus(nCheckRec) = EM_DIV_DSP_STATUS.DIV_DSP_INITIALIZE Then
                            Exit For
                        End If
                        '表示可能または表示予定
                        If mcls_PointInf.DispStatus(nCheckRec) = EM_DIV_DSP_STATUS.DIV_DSP_OK _
                            Or mcls_PointInf.DispStatus(nCheckRec) = EM_DIV_DSP_STATUS.DIV_DSP_STANDBY Then

                            Dim LPos As Integer = mcls_PointInf.RowLast(nCheckRec, nStage)                 ' 検査完了行(1オリジン、0は未完了と判断)
                            Dim WPos As Integer = mcls_PointInf.RowWrite(nCheckRec, nMen, nStage) + 1      ' 現在書き込み行(0オリジン) 他と合わせて使用に為＋１
                            Dim DPos As Integer = mcls_PointInf.RowDisp(nCheckRec, m_nKensaNo, nStage)     ' 現在表示行(1オリジン)

                            ' 表示可能ではあるが未通板の場合はスキップ
                            If 0 = LPos And 1 = WPos Then
                                Continue For
                            End If
                            ' 開始REC、開始行
                            nRec = nCheckRec
                            nRow = nRowZan

                            If 0 < LPos Then
                                nRowZan = nRowZan - LPos
                            Else
                                nRowZan = nRowZan - WPos
                            End If

                            ' 残りが無い場合は、検索終了
                            If 0 >= nRowZan Then
                                Exit For
                            End If
                        End If
                    Next jj
                End If
            ElseIf (0 > nSa) Then

                If (DPosBase + nSa) > 0 Then
                    nRec = nDispRec
                    nRow = DPosBase + nSa
                Else
                    ' 対象検査位置の現在表示行が、画面開始位置にとどいていない。
                    nRec = nDispRec
                    nRow = DPosBase + nSa

                    nCheckRec = nDispRec
                    For jj As Integer = 0 To TBL_HY_REC_MAX - 1
                        '前回表示レコード取得
                        nCheckRec = clsPointInf.GetPrevRecNo(nCheckRec)
                        ' 画面データ無しの場合は、検索終了
                        If mcls_PointInf.DispStatus(nCheckRec) = EM_DIV_DSP_STATUS.DIV_DSP_INITIALIZE Then
                            Exit For
                        End If
                        '表示可能または表示予定
                        If mcls_PointInf.DispStatus(nCheckRec) = EM_DIV_DSP_STATUS.DIV_DSP_OK _
                            Or mcls_PointInf.DispStatus(nCheckRec) = EM_DIV_DSP_STATUS.DIV_DSP_STANDBY Then

                            Dim LPos As Integer = mcls_PointInf.RowLast(nCheckRec, nStage)                 ' 検査完了行(1オリジン、0は未完了と判断)
                            Dim WPos As Integer = mcls_PointInf.RowWrite(nCheckRec, nMen, nStage) + 1      ' 現在書き込み行(0オリジン) 他と合わせて使用に為＋１
                            Dim DPos As Integer = mcls_PointInf.RowDisp(nCheckRec, m_nKensaNo, nStage)     ' 現在表示行(1オリジン)

                            ' 表示可能ではあるが未通板の場合はスキップ
                            If 0 = LPos And 1 = WPos Then
                                Continue For
                            End If
                            ' 開始REC、開始行
                            nRec = nCheckRec
                            nRow = LPos + (DPosBase + nSa)
                            Exit For

                        End If
                    Next jj
                End If
            Else
                nRec = nDispRec
                nRow = DPosBase + nSa
            End If
        Else
            nRec = nDispRec
            nRow = DPosBase
        End If

        Return True

    End Function
    ' 20201010 検査台位置表示変更 <<<---

End Class
