'*******************************************************************************
'　疵マップクラス
'	[Ver]
'		Ver.01    2010/11/04  初版
'
'	[メモ]
'		疵マップ表示クラス
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
''' 疵マップ表示
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

    '///////////////////////////////////////////////////////////////////////////
    '構造体
    '///////////////////////////////////////////////////////////////////////////
    '疵マップ構造体
    Public Structure TYP_MAP_INF
        Dim dgvMap As tCnt_DataGridView_turn                ' 疵マップ
        Dim dgvDeep As tCnt_DataGridView_turn               ' 疵深さリスト
        Dim dgvTcm As tCnt_DataGridView_turn                ' TCMリスト
        Dim vsbMap As tCnt_VScrollBar                       ' スクロールバー
    End Structure


    ''' <summary>
    ''' マップ上の疵情報
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure MAP_DEFECT
        Dim strKizukenNo As String                                              ' 管理No
        Dim nKizuNo As Integer                                                  ' 疵No
        Dim nCol As EM_MAP_COL_INF                                              ' 疵マップ上の列番号 0オリジン
        Dim nRow As Integer                                                     ' 疵マップ上の行番号 0オリジン
    End Structure

    ''' <summary>
    ''' マップ行情報
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure MAP_ROW_DATA
        Dim MapHeader As MAP_HEADER                                             ' マップヘッダ情報
        Dim MapInf As G_MAP_ROW                                                 ' マップ情報
    End Structure

    ''' <summary>
    ''' マップヘッダ情報
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure MAP_HEADER
        Dim strKizukenNo As String                                              ' 管理No
        Dim bIsSharTop As Boolean                                               ' シャーカット先頭位置有無
        Dim bIsSharBot As Boolean                                               ' シャーカット尾端位置有無
        Dim bIsIshuten As Boolean                                               ' 異種点
        Dim bIsIshutenBot As Boolean                                            ' 異種点下側
        Dim bIsTanpanCut As Boolean                                             ' 端板カット有無
    End Structure

    ''' <summary>
    ''' 疵マップ値
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure MAP_VALUE
        Dim strKizukenNo As String                                              ' 管理No
        Dim nKizuNo As Integer                                                  ' 疵No
        Dim nTypeID As Integer                                                  ' 疵種のEdasysID
        Dim nGradeID As Integer                                                 ' グレードのEdasysID
        Dim strMapName As String                                                ' マップ表示名

        ''' <summary>
        ''' マップ値の文字列取得
        ''' </summary>
        ''' <returns>疵種マップ名を取得する</returns>
        ''' <remarks></remarks>
        Public Overrides Function ToString() As String
            Return strMapName                                                   ' DataGridViewの表示用にToStrignをオーバーライドする
        End Function
    End Structure


    '///////////////////////////////////////////////////////////////////////////
    'メンバ変数
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                                    ' リソース開放済みフラウグ True:開放済み
    Private m_emStage As EM_LONG_POS                                            ' 長手間隔(1:小、2:大)
    Private m_emMapDir As EM_MAP_SCROOL                                         ' マップの進行方向
    Private m_emSide As EM_MAP_SIDE                                             ' ラベルDS位置[NORMAL:左がDS、REVERSE:左がWS]
    Private m_DSPos As EM_MAP_CELL                                              ' 疵マップDS位置[NORMAL:左がDS、REVERSE:左がWS]
    Private m_nDspMen As Integer                                                ' 表示面
	'// 2023.11.17 最終工程検査装置高機能化その２ -------------------->>>>>	
	Private m_emWidthNo As EM_MAP_WIDTH_NO                          ' マップ幅位置番号[NORMAL:左から1,2,・・・、REVERSE:右から1,2,・・・]
	'// 2023.11.17 最終工程検査装置高機能化その２ <<<<<--------------------

	Private m_typMapInf As TYP_MAP_INF                                          ' 疵マップ
    Private mcls_Log As tClass_LogManager                                       ' ログ管理
    Private mcls_Param As clsParamManager                                       ' パラメータ管理
    Private mcls_PreParam As clsPreParamManager                                 ' 前工程パラメータ管理
    Private mcls_DataMap As clsDataMap                                          ' 疵マップデータクラス

    Private m_typSelectValue(DSP_DEFECT_MAX - 1) As TYP_MAP_LIST_VALUE          '選択疵
    Private m_typTcmSelectValue As TYP_MAP_LIST_VALUE                           'TCM選択疵
    Private m_nMaxRowCnt As Integer                                             '最大行数

    Private m_aryColor() As Color                                               ' 選択色一覧

    Private Const DEFECT_DEGAWA As Boolean = False                              ' 出側距離表示するか false:表示しない true:表示する

    '---------------------------------------------------------------------------
    ' プロパティ
    '---------------------------------------------------------------------------
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
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_2).HeaderText = "2"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_3).HeaderText = "3"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_4).HeaderText = "4"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_5).HeaderText = "5"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_6).HeaderText = "6"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_7).HeaderText = "7"
			'         Else
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_2).HeaderText = "7"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_3).HeaderText = "6"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_4).HeaderText = "5"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_5).HeaderText = "4"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_6).HeaderText = "3"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_7).HeaderText = "2"
			'         End If
			'// 2023.11.17 最終工程検査装置高機能化その２ -------------------->>>>>		
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
	''' 選択色の設定と取得
	''' </summary>
	''' <param name="index">選択順 0:最新 1:一つ前</param>
	''' <value>選択色</value>
	''' <returns>選択色</returns>
	''' <remarks></remarks>
	Public Property SelectColor(ByVal index As Integer) As Color
        Get
            Return m_aryColor(index)
        End Get
        Set(ByVal value As Color)
            m_aryColor(index) = value
        End Set
    End Property

    ''' <summary>
    ''' 疵マップデータクラス設定
    ''' </summary>
    Public WriteOnly Property DataMap() As clsDataMap
        Set(value As clsDataMap)
            mcls_DataMap = value
        End Set
    End Property


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

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="nMen"></param>
    ''' <param name="pntKizu"></param>
    ''' <remarks></remarks>
    Public Event SelectMapCell_Click(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)

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

    ''' <summary>
    ''' 疵マップを作成します
    ''' </summary>
    ''' <param name="typMap"></param>
    ''' <param name="param"></param>
    ''' <param name="log"></param>
    ''' <remarks></remarks>
    Public Sub New(ByVal typMap As TYP_MAP_INF, ByRef param As clsParamManager, ByRef preparam As clsPreParamManager, ByRef log As tClass_LogManager)
        Try
            m_typMapInf = typMap       '疵マップ情報
            mcls_Param = param
            mcls_PreParam = preparam
            mcls_Log = log
            mcls_DataMap = Nothing


            For ii As Integer = 0 To m_typSelectValue.Length - 1
                m_typSelectValue(ii).Initialize()
            Next
            m_typTcmSelectValue.Initialize()

            ReDim m_aryColor(DSP_DEFECT_MAX - 1)

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
            mcls_Log.Write(LNO.ERR, String.Format("疵マップ表示生成異常 [{0}]", ex.Message))
        End Try
    End Sub

#Region " IDisposable Support "
    ''' <summary>
    ''' オブジェクトの破棄
    ''' </summary>
    ''' <param name="disposing">True:明示的な呼び出し False:暗黙の呼び出し</param>
    ''' <remarks></remarks>
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        If Not m_bIsDesposed Then
            'オブジェクト破棄
        End If
        m_bIsDesposed = True                                                'Dispose済みとする
    End Sub

    ' このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
    Public Sub Dispose() Implements IDisposable.Dispose
        ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
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
            .RowCount = JISSEKI_MAP_DSP_ROW_MAX
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

            ''背景色
            '.BackgroundColor = Color.White

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
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function DspData(ByVal nMen As Integer) As Boolean
        Dim bReturn As Boolean = False

        Try
            If mcls_DataMap Is Nothing Then
                Exit Try
            End If

            m_nMaxRowCnt = 0

            Dim nSelectValue(m_typSelectValue.Length - 1) As TYP_MAP_LIST_VALUE
            For ii As Integer = 0 To m_typSelectValue.Length - 1
                nSelectValue(ii) = m_typSelectValue(ii)
            Next
            Dim nTcmSelectValue As TYP_MAP_LIST_VALUE = m_typTcmSelectValue

            '疵マップ設定
            If Not SetMapData(nMen, 0, m_nMaxRowCnt) Then
                Exit Try
            End If

            If 0 > m_typSelectValue(0).nPnt Then
                '最新疵を見つける
                GetSearchKizuFirst(nMen)
            End If

            '疵自動選択イベント発生
            If 0 < m_typSelectValue(0).nPnt And nSelectValue(0) <> m_typSelectValue(0) Then
                ' 最新疵選択が変化した場合はマップ位置の変化を確認
                Dim nRow As Integer
                Dim nCol As Integer

                Dim bDeepDisp As Boolean = GetMapPos(m_typSelectValue(0), nRow, nCol)
                If bDeepDisp AndAlso Not IsVisibleOnMap(m_typSelectValue(0)) Then
                    '選択疵が可視領域にない
                    bDeepDisp = False
                End If
                If Not bDeepDisp Then
                    Dim kizukey As clsDataMap.KIZU_KEY
                    kizukey.nKizuNo = m_typSelectValue(0).nKizuNo
                    kizukey.nStage = Stage
                    If Not mcls_DataMap.ListKizuNoKeyUmu(kizukey) Then
                        Exit Try
                    End If
                    Dim listkeyWk As clsDataMap.LIST_KEY = mcls_DataMap.ListGetKizuNoKey(kizukey)
                    Dim nValue As Integer = listkeyWk.nRow
                    If (m_nMaxRowCnt - nValue) < m_typMapInf.dgvMap.DisplayedRowCount(False) Then
                        nValue = m_nMaxRowCnt - m_typMapInf.dgvMap.DisplayedRowCount(False)
                    End If

                    'If Not SetMapData(nMen, listkey.nRow, m_nMaxRowCnt) Then
                    Dim nMaxRowNum As Integer
                    If Not SetMapData(nMen, nValue, nMaxRowNum) Then
                        Exit Try
                    End If
                    GetMapPos(m_typSelectValue(0), nRow, nCol)
                End If
            End If

            Dim listKey As clsDataMap.LIST_KEY

            Dim bDeepFlg As Boolean = False
            listKey.initialize()
            For ii As Integer = m_typSelectValue.Length - 1 To 0 Step -1
                '' 疵選択が変化した場合はマップ位置の変化を確認
                If 0 < m_typSelectValue(ii).nPnt Then
                    Dim kizukey As clsDataMap.KIZU_KEY
                    kizukey.nKizuNo = m_typSelectValue(ii).nKizuNo
                    kizukey.nStage = Stage
                    If Not mcls_DataMap.ListKizuNoKeyUmu(kizukey) Then
                        Continue For
                    Else
                        bDeepFlg = True
                    End If
                    listKey = mcls_DataMap.ListGetKizuNoKey(kizukey)
                End If
            Next
            If bDeepFlg Then
                Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = Nothing
                Dim bLstRet As Boolean = mcls_DataMap.GetMapDefectList(listKey.nCol, listKey.nRow, listKey.nMen, listKey.emStage, lstVal)
                If Not bLstRet Then
                    lstVal = Nothing
                End If
                ' 深さリストの表示
                SetDeepList(m_typMapInf.dgvDeep, lstVal)
            End If

            '選択外となったときリストを初期化
            If Not bDeepFlg Then
                ClearDeep()
            End If
            m_typMapInf.dgvDeep.Refresh()

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
        Dim bCycDelFlg As Boolean = False
        For ii As Integer = m_typSelectValue.Length - 1 To 0 Step -1
            '' 疵選択が変化した場合はマップ位置の変化を確認
            'If 0 < m_typSelectValue(ii).nPnt And 2 <> m_typSelectValue(ii).nCycDiv Then
            If 0 < m_typSelectValue(ii).nPnt Then
                Dim kizukey As clsDataMap.KIZU_KEY
                kizukey.nKizuNo = m_typSelectValue(ii).nKizuNo
                kizukey.nStage = Stage
                If Not mcls_DataMap.ListKizuNoKeyUmu(kizukey) Then
                    Continue For
                Else
                    bDeepDelFlg = True
                End If
            End If
        Next

        If Not bDeepDelFlg Then
            ClearDeep()
        End If
        m_typMapInf.dgvDeep.Refresh()

    End Sub

    ''' <summary>
    ''' 疵マップ情報表示
    ''' </summary>
    ''' <param name="nMove">移動量</param>
    ''' <remarks></remarks>
    Public Function SetMapData(ByVal nMen As Integer, ByVal nMove As Integer, ByRef nMaxRowNum As Integer) As Boolean
        Dim nRowPos As Integer                              '行位置
        Dim nDgvPos As Integer = 0                          'DGV行位置
        Dim nCoilCnt As Integer = 0                         '表示コイル数
        Dim bStart As Boolean = False                       '処理開始フラグ
        Dim nKensaPos As Integer = 0                        '検査台位置
        Dim nRowCheck As Integer = 0
        Dim nRowCount As Integer = 0
        Dim emStage As EM_LONG_POS = Stage
        Dim nRowCnt As Integer = 0
        Dim bRet As Boolean = False


        Try
            If mcls_DataMap Is Nothing Then
                Exit Try
            End If

            '初期化
            ClearMap()

            nRowPos = nMove

            DspMap(nMen, emStage, nRowPos, nDgvPos)

            If 0 <= nDgvPos And m_typMapInf.dgvMap.RowCount > nDgvPos Then
                'データがセットされなかった行を無効行にする
                While m_typMapInf.dgvMap.RowCount > nDgvPos
                    Dim nRow As Integer = nDgvPos
                    If EM_MAP_SCROOL.TAIL_TO_HEAD <> MapDir Then         '上から下
                        nRow = m_typMapInf.dgvMap.RowCount - 1 - nDgvPos
                    End If
                    DspRowDisable(nRow)
                    nDgvPos = nDgvPos + 1
                End While
            End If

            nMaxRowNum = mcls_DataMap.GetRowCount(emStage)
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

        '-------------------------------------------------------------------
        ' 検索列方向の決定
        '-------------------------------------------------------------------
        Dim bDirCol As Boolean = CBool(IIf(DSPos = EM_MAP_CELL.NORMAL, True, False))
        Dim nColS As Integer = 0        '開始列は左端(0)
        ' TODO 左端のデータを常に選択
        If Not bDirCol Then
            nColS = MAP_COL_NUM - 1     '検索列方向が「右から左」の場合は、開始列を右端
        End If

        '-------------------------------------------------------------------
        ' 検索行方向の決定
        '-------------------------------------------------------------------
        Dim bDirRow As Boolean = True   'データ上の上から下
        Dim nRowS As Integer = 0

        '-------------------------------------------------------------------
        ' コイル上の左上疵を選択
        '-------------------------------------------------------------------
        Dim lstKey As New List(Of clsDataMap.LIST_KEY)
        Dim lstVal As New List(Of TYP_MAP_LIST_VALUE)
        If Not mcls_DataMap.FindDefect(nColS, 0, bDirCol, bDirRow, m_typSelectValue.Count, lstKey, lstVal, nMen, Stage) Then
            Return False
        End If

        Dim nCnt As Integer = 0
        For ii As Integer = 0 To lstVal.Count - 1
            If 0 < lstVal(ii).nKizuNo Then
                m_typSelectValue(nCnt) = lstVal(ii)
                nCnt = nCnt + 1
                If m_typSelectValue.Count = nCnt Then
                    Exit For
                End If
            End If
        Next

        Return CBool(IIf(0 < nCnt, True, False))

    End Function

    ''' <summary>
    ''' マップ情報セット
    ''' </summary>
    ''' <param name="nMapPos">現在表示行</param>
    ''' <param name="nDgvPos">DGV表示位置</param>
    ''' <remarks></remarks>
    Private Sub DspMap(ByVal nMen As Integer, ByVal emStage As EM_LONG_POS, ByVal nMapPos As Integer, ByRef nDgvPos As Integer)
        Dim nColStart As Integer                            '開始列インデックス
        Dim nColEnd As Integer                              '終了列インデックス
        Dim nStep As Integer                                '列ステップ間隔
        Dim nColCnt As Integer                              '表示COLカウンタ
        Dim nRowStart As Integer


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

        Try
            nRowStart = nMapPos

            For nMapRow As Integer = nRowStart To mcls_DataMap.GetRowCount(Stage) - 1 Step 1

                If nDgvPos >= m_typMapInf.dgvMap.RowCount Then
                    Exit For
                End If

                Dim nDgvRow As Integer = nDgvPos
                If EM_MAP_SCROOL.TAIL_TO_HEAD <> MapDir Then         '下から上
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

                GetMapRowData(nMen, emStage, nMapRow, typRow, lstCell, lstTcmCell)

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
                If EM_DIV_KENSA.DIV_KENSA_NON = typRow.emKensa Then        '検査外
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKensaBackColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_NON)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKensaForeColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_NON)
                ElseIf EM_DIV_KENSA.DIV_KENSA_STOP = typRow.emKensa Then    '停止
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKensaBackColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_STOP)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKensaForeColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_STOP)
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
                ElseIf EM_DIV_KIKI.DIV_KIKI_KEI = typRow.emKiki Then       '軽故障
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKikiBackColor(KizuForm.EM_DIV_KIKI.DIV_KIKI_KEI)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKikiForeColor(KizuForm.EM_DIV_KIKI.DIV_KIKI_KEI)
                Else                                                        '正常
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = g_ColorTanshoOk
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = Color.Black
                End If
                m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.SelectionBackColor = m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, nDgvRow).Style.BackColor
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
                    Dim typTcmData As TYP_MAP_LIST_VALUE = Nothing
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
    ''' <param name="nMen">面[0:表,1:裏]</param>
    ''' <param name="nRow">行[0～]</param>
    ''' <returns>行情報</returns>
    ''' <remarks></remarks>
    Private Function GetMapRowData(ByVal nMen As Integer, ByVal emStage As EM_LONG_POS, ByVal nRow As Integer,
                                   ByRef typRow As G_MAP_ROW, ByRef lstVal() As List(Of TYP_MAP_LIST_VALUE),
                                   ByRef lstTcmCell() As List(Of TYP_MAP_LIST_VALUE)) As Boolean

        Dim bRetc As Boolean = False

        Try
            typRow.initialize()
            If Not mcls_DataMap.ReadRow(typRow, nMen, emStage, nRow) Then
                Exit Try
            End If
            typRow.nMemRowPos = nRow

            For nCol As Integer = 0 To lstVal.Length - 1
                Dim bLstRet As Boolean = mcls_DataMap.GetMapDefectList(nCol, nRow, nMen, emStage, lstVal(nCol))
                If Not bLstRet Then
                    lstVal(nCol) = Nothing
                End If
            Next

            For nCol As Integer = 0 To lstTcmCell.Length - 1
                Dim bLstRet As Boolean = mcls_DataMap.GetMapDefectTcmList(nCol, nRow, nMen, emStage, lstTcmCell(nCol))
                If Not bLstRet Then
                    lstTcmCell(nCol) = Nothing
                End If
            Next

            bRetc = True

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("マップ位置行情報取得異常[{0}]", ex.Message))
        End Try

        Return bRetc
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

        Dim nStart As Integer
        Dim nStep As Integer
        Dim nMax As Integer


        penSelect(0) = New System.Drawing.Pen(m_aryColor(0), 3)
        penSelect(1) = New System.Drawing.Pen(m_aryColor(1), 3)

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
    ''' 疵マップ位置(TCM)検索
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
            Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(dgvCell.Tag, List(Of TYP_MAP_LIST_VALUE))

            If lstVal Is Nothing Then
                Exit Try
            End If

            '同一疵の場合は選択しない
            Dim bSelect As Boolean = True
            If m_typSelectValue(0) = lstVal(0) Then
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
                    m_typSelectValue(0) = lstVal(0)

                    '選択外となったときリストを初期化
                    LstSelectClear()

                    'クリックイベント発行
                    RaiseEvent SelectMapCell_Click(Men, lstVal(0))

                End If

                '疵リスト表示
                SetDeepList(m_typMapInf.dgvDeep, lstVal)

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
                dgv(0, nCnt).Value = nCnt + 1
                dgv(1, nCnt).Value = mcls_Param.GetGradeName(lstVal(nCnt).nGid)
                dgv(1, nCnt).Style.BackColor = mcls_Param.GetGradeColor(lstVal(nCnt).nGid)
                dgv(2, nCnt).Value = mcls_Param.GetMapName(lstVal(nCnt).nTid)
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
            If nTopRow < 0 Then nTopRow = 0
            For ii As Integer = 0 To MAX_MAPDEFLST_ROW
                Dim nRowCnt As Integer = nTopRow + ii
                If nRowCnt > dgv.RowCount - 1 Then
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

            If m_typSelectValue(0) = lstVal(e.RowIndex) Then
                '同一疵の場合は選択しない
                Exit Try
            End If

            '左クリック時
            If e.Button = MouseButtons.Left Then
                For ii As Integer = m_typSelectValue.Length - 1 To 1 Step -1
                    m_typSelectValue(ii) = m_typSelectValue(ii - 1)
                Next
                m_typSelectValue(0) = lstVal(e.RowIndex)

                '選択外となったときリストを初期化
                LstSelectClear()

                'イベント発行
                RaiseEvent SelectMapCell_Click(Men, lstVal(e.RowIndex))
            End If

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("深さリストセル選択異常[{0}]", ex.Message))
        End Try

        dgvTmp.Refresh()
        m_typMapInf.dgvMap.Refresh()
    End Sub


    ''' <summary>
    ''' 疵マップセル選択(マウスクリックイベント時)
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

            ''自動更新中は無効
            'If EM_REFRESH_MODE.AUTO = m_emViewMode Then
            '    Exit Try
            'End If

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

            '左クリック時
            If e.Button = MouseButtons.Left Then
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
    ''' 
    ''' </summary>
    ''' <returns></returns>
    Public Function GetMapRowTop() As Integer
        With m_typMapInf
            Dim nFirst As Integer = .dgvMap.FirstDisplayedScrollingRowIndex
            Dim nStart As Integer = nFirst
            Dim nEnd As Integer = nFirst + .dgvMap.DisplayedRowCount(False) - 1
            Dim nStep As Integer = 1
            If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                nStart = nFirst + .dgvMap.DisplayedRowCount(False) - 1
                nEnd = nFirst
                nStep = -1
            End If
            For ii As Integer = nStart To nEnd Step nStep
                If .dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, ii).Tag Is Nothing Then
                    Continue For
                End If
                Dim typRow As G_MAP_ROW = CType(.dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, ii).Tag, G_MAP_ROW)
                Return typRow.nMemRowPos
            Next
        End With
        Return -1
    End Function

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <returns></returns>
    Public Function GetMapRowBot() As Integer
        With m_typMapInf
            Dim nFirst As Integer = .dgvMap.FirstDisplayedScrollingRowIndex
            Dim nStart As Integer = nFirst + .dgvMap.DisplayedRowCount(False) - 1
            Dim nEnd As Integer = nFirst
            Dim nStep As Integer = -1
            If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                nStart = nFirst
                nEnd = nFirst + .dgvMap.DisplayedRowCount(False) - 1
                nStep = 1
            End If
            For ii As Integer = nStart To nEnd Step nStep
                If .dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, ii).Tag Is Nothing Then
                    Continue For
                End If
                Dim typRow As G_MAP_ROW = CType(.dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, ii).Tag, G_MAP_ROW)
                Return typRow.nMemRowPos
            Next
        End With
        Return -1
    End Function

    ''' <summary>
    ''' マップの縦サイズ変更
    ''' </summary>
    Public Sub MapResizeH()
        Dim dgv As tCnt.tCnt_DataGridView = m_typMapInf.dgvMap
        Dim count As Integer = dgv.DisplayedRowCount(False)
        Dim nValue As Integer
        If (dgv.RowCount < count) Then Exit Sub
        ' マップスクロールの移動サイズ(大)は、表示行の分だけ
        Dim nLastLarge As Integer = m_typMapInf.vsbMap.LargeChange
        m_typMapInf.vsbMap.LargeChange = count
        Dim nDiff As Integer = nLastLarge - count
        If 0 = nDiff Then
            '変化なし
            Exit Sub
        End If

        ' 開始行を可視できる位置に変更する
        If EM_MAP_SCROOL.HEAD_TO_TAIL <> MapDir Then
            ' 上→下の場合は先頭(0)
            dgv.FirstDisplayedScrollingRowIndex = 0
            'm_typMapInf.vsbMap.ValueProgramable = 0
            If 0 > nDiff Then
                m_typMapInf.vsbMap.ValueProgramable = m_typMapInf.vsbMap.Value + nDiff
            End If
            nValue = m_typMapInf.vsbMap.Value
        Else
            ' 下→上の場合は、可視終端行が固定して見えるように開始位置を調整
            dgv.FirstDisplayedScrollingRowIndex = dgv.RowCount - count
            'If nLastLarge > m_typMapInf.vsbMap.LargeChange Then
            '    m_typMapInf.vsbMap.ValueProgramable = m_typMapInf.vsbMap.Value + (nLastLarge - m_typMapInf.vsbMap.LargeChange)
            'End If
            If 0 > nDiff Then
                m_typMapInf.vsbMap.ValueProgramable = m_typMapInf.vsbMap.Value + nDiff
            End If
            nValue = m_typMapInf.vsbMap.Maximum - (m_typMapInf.vsbMap.LargeChange - 1) - m_typMapInf.vsbMap.Value
        End If
        'DspData(Men)
        If 0 <= nValue Then
            Dim nMaxRowCnt As Integer
            SetMapData(Men, nValue, nMaxRowCnt)
        End If
    End Sub


    ''' <summary>
    ''' マップ選択クリア
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ClearSelect()
        Try
            For ii As Integer = 0 To m_typSelectValue.Length - 1
                m_typSelectValue(ii).Initialize()
            Next ii
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップ選択クリア異常 [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' 選択疵を取得
    ''' </summary>
    ''' <param name="n">0:最新の選択疵、1:1つ前の選択疵</param>
    ''' <returns></returns>
    Public Function GetSelectKizu(ByVal n As Integer, ByRef typVal As TYP_MAP_LIST_VALUE) As Boolean
        If 0 > n Or m_typSelectValue.Length <= n Then
            Return False
        End If
        typVal = m_typSelectValue(n)
        Return True
    End Function


    ''' <summary>
    ''' 疵の選択を指定方向に設定します
    ''' </summary>
    ''' <param name="nMen">0:表面 1:裏面</param>
    ''' <param name="bDir">False:前、True:次</param>
    ''' <returns>True:選択OK、False:選択NG</returns>
    ''' <remarks></remarks>
    Public Function SetSelection(ByVal nMen As Integer, ByVal bDir As Boolean) As Boolean
        Dim bResult As Boolean = False
        Try

            ' 最新の選択疵を取得
            Dim kizukey As clsDataMap.KIZU_KEY
            kizukey.nKizuNo = m_typSelectValue(0).nKizuNo
            kizukey.nStage = Stage
            If Not mcls_DataMap.ListKizuNoKeyUmu(kizukey) Then
                Exit Try
            End If
            Dim listkey As clsDataMap.LIST_KEY = mcls_DataMap.ListGetKizuNoKey(kizukey)

            '-------------------------------------------------------------------
            ' 検索列方向の決定
            '-------------------------------------------------------------------
            Dim bDirCol As Boolean = CBool(IIf(DSPos = EM_MAP_CELL.NORMAL, True, False))
            If Not bDir Then
                bDirCol = Not bDirCol
            End If
            '-------------------------------------------------------------------
            ' 移動方向の疵を取得
            '-------------------------------------------------------------------
            Dim lstKey As New List(Of clsDataMap.LIST_KEY)
            Dim lstVal As New List(Of TYP_MAP_LIST_VALUE)

            ' 検索件数は自分を含めて2件。
            Dim bRet As Boolean = mcls_DataMap.FindDefect(listkey, m_typSelectValue(0), bDirCol, bDir, 3, lstKey, lstVal, nMen, Stage)
            If Not bRet Or lstVal.Count < 2 Then
                ' 検索失敗、または、検索結果が2件未満
                Exit Try
            End If

            For ii As Integer = m_typSelectValue.Length - 1 To 1 Step -1             ' 疵を一つ前の選択疵にする
                m_typSelectValue(ii) = m_typSelectValue(ii - 1)
            Next
            m_typSelectValue(0) = lstVal(1)

            If Not IsVisibleOnMap(m_typSelectValue(0)) Then
                ' 選択疵を含むセルがマップに表示されていない
                Dim nValue As Integer = lstKey(1).nRow
                If bDir AndAlso nValue > (m_typMapInf.dgvMap.DisplayedRowCount(False) - 1) Then
                    nValue = nValue - (m_typMapInf.dgvMap.DisplayedRowCount(False) - 1)
                End If
                If (m_nMaxRowCnt - nValue) < m_typMapInf.dgvMap.DisplayedRowCount(False) Then
                    nValue = m_nMaxRowCnt - m_typMapInf.dgvMap.DisplayedRowCount(False)
                End If
                Dim nMaxRowNum As Integer
                SetMapData(nMen, nValue, nMaxRowNum)
                If MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD Then
                    m_typMapInf.vsbMap.ValueProgramable = nValue
                Else
                    m_typMapInf.vsbMap.ValueProgramable = nMaxRowNum - (nValue + m_typMapInf.dgvMap.DisplayedRowCount(False))
                End If
            Else
                m_typMapInf.dgvMap.Refresh()
            End If

            'クリックイベント発行
            RaiseEvent SelectMapCell_Click(nMen, m_typSelectValue(0))

            LstSelectClear()

            If lstKey(0) <> lstKey(1) Then
                'リストキーが異なる場合は、深さリストを更新
                Dim nRow As Integer
                Dim nCol As Integer
                Dim bDeepDisp As Boolean = GetMapPos(m_typSelectValue(0), nRow, nCol)
                If bDeepDisp Then
                    ' 深さリスト表示
                    SetDeepList(m_typMapInf.dgvDeep, CType(m_typMapInf.dgvMap.Item(nCol, nRow).Tag, List(Of TYP_MAP_LIST_VALUE)), bDir)
                End If
            End If

            ' 選択疵を深さリスト上に可視となるようにする
            DeepDispVisible(m_typSelectValue(0))

            bResult = True

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵の選択変更中に例外 [{0}] Dir={1}", ex.Message, bDir))
        End Try

        Return bResult
    End Function


    ''' <summary>
    ''' 疵マップの可視内に含まれているかを確認
    ''' </summary>
    ''' <param name="val">疵情報</param>
    ''' <returns></returns>
    Private Function IsVisibleOnMap(ByVal val As TYP_MAP_LIST_VALUE) As Boolean

        ' 最新の選択疵を取得
        Dim kizukey As clsDataMap.KIZU_KEY
        kizukey.nKizuNo = val.nKizuNo
        kizukey.nStage = Stage
        If Not mcls_DataMap.ListKizuNoKeyUmu(kizukey) Then
            Return False
        End If
        Dim listkey As clsDataMap.LIST_KEY = mcls_DataMap.ListGetKizuNoKey(kizukey)

        Dim nTopRow As Integer = GetMapRowTop()
        Dim nBotRow As Integer = GetMapRowBot()
        If nTopRow <= listkey.nRow And nBotRow >= listkey.nRow Then
            Return True
        End If

        Return False

    End Function

    ''' <summary>
    ''' 指定した疵がリスト上に存在する場合は可視できるようにする
    ''' </summary>
    ''' <param name="val">疵情報</param>
    Private Sub DeepDispVisible(ByVal val As TYP_MAP_LIST_VALUE)

        With m_typMapInf.dgvDeep
            Dim lst As List(Of TYP_MAP_LIST_VALUE) = CType(.Tag, List(Of TYP_MAP_LIST_VALUE))
            If lst Is Nothing Then
                Exit Sub
            End If
            Dim nDispCount As Integer = .DisplayedRowCount(False)
            If lst.Count <= nDispCount Then
                .Refresh()
                Exit Sub
            End If

            For ii As Integer = 0 To lst.Count - 1
                If lst(ii) <> val Then
                    Continue For
                End If
                If ii < .FirstDisplayedScrollingRowIndex Then
                    .FirstDisplayedScrollingRowIndex = ii
                ElseIf ii > .FirstDisplayedScrollingRowIndex + nDispCount - 1 Then
                    .FirstDisplayedScrollingRowIndex = ii - nDispCount + 1
                Else
                    .Refresh()
                End If
                Exit For
            Next
        End With
    End Sub

    ''' <summary>
    ''' 次疵、前疵の有無を確認
    ''' </summary>
    ''' <param name="nMen">0:表、1:裏</param>
    ''' <param name="val">疵情報</param>
    ''' <param name="bDir">True:次、false:前</param>
    ''' <returns></returns>
    Public Function IsExistEachDef(ByVal nMen As Integer, ByVal val As TYP_MAP_LIST_VALUE, ByVal bDir As Boolean) As Boolean

        Dim bExist As Boolean = False

        Try

            ' 最新の選択疵を取得
            Dim kizukey As clsDataMap.KIZU_KEY
            kizukey.nKizuNo = val.nKizuNo
            kizukey.nStage = Stage
            If Not mcls_DataMap.ListKizuNoKeyUmu(kizukey) Then
                Exit Try
            End If
            Dim listkey As clsDataMap.LIST_KEY = mcls_DataMap.ListGetKizuNoKey(kizukey)

            '-------------------------------------------------------------------
            ' 検索列方向の決定
            '-------------------------------------------------------------------
            Dim bDirCol As Boolean = CBool(IIf(DSPos = EM_MAP_CELL.NORMAL, True, False))
            If Not bDir Then
                bDirCol = Not bDirCol
            End If

            '-------------------------------------------------------------------
            ' 検索行方向の決定
            '-------------------------------------------------------------------
            Dim bDirRow As Boolean = True   'データ上の上から下
            If Not bDir Then
                bDirRow = Not bDirRow
            End If

            '-------------------------------------------------------------------
            ' 疵を検索
            '-------------------------------------------------------------------
            Dim lstKey As New List(Of clsDataMap.LIST_KEY)
            Dim lstVal As New List(Of TYP_MAP_LIST_VALUE)
            ' 検索件数は自分を含めて2件。
            Dim bRet As Boolean = mcls_DataMap.FindDefect(listkey, val, bDirCol, bDirRow, 2, lstKey, lstVal, nMen, Stage)
            If bRet AndAlso 1 < lstVal.Count Then
                ' 検索結果が1件以上
                bExist = True
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("次疵/前疵の有無を確認中に例外 [{0}] Dir={1}", ex.Message, bDir))
        End Try

        Return bExist
    End Function

    ''' <summary>
    ''' 疵マップ行を無効表示します
    ''' </summary>
    ''' <param name="nRowNo">行番号 0オリジン</param>
    ''' <remarks></remarks>
    Private Sub DspRowDisable(ByVal nRowNo As Integer)
        Try

            For col As Integer = 0 To m_typMapInf.dgvMap.ColumnCount - 1                ' コントロール色に設定する
                With m_typMapInf.dgvMap(col, nRowNo)
                    .Value = ""
                    .Style.BackColor = g_ColorControl                                   ' 背景色の設定
                    .Style.SelectionBackColor = g_ColorControl                          ' 選択時背景色の設定
                    If EM_MAP_COL_INF.IRIGAWA = col Then
                        .Tag = Nothing
                    End If
                End With
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ行表示無効化異常 [{0}]", ex.Message))
        End Try
    End Sub

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

End Class
