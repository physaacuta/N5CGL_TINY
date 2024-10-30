'*******************************************************************************
'   探傷画面
'
'  [Ver]
'       Ver.01  2013/12/18  初版
'
'  [メモ]
'
'*******************************************************************************
Option Strict On

Imports KizuLibCLI
Imports KizuLibCLI.KizuMod
Imports KizuForm

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Reflection
Imports tClassLibrary.tClass.tClass_LogManager



''' <summary>
''' 探傷画面クラス
''' </summary>
''' <remarks></remarks>
Public Class frmTansho
    '///////////////////////////////////////////////////////////////////////////
    '定数
    '///////////////////////////////////////////////////////////////////////////
    Private Const MAX_FUNC_TAB As Integer = 3
    Private Const MAX_COIL_TAB As Integer = 1
    Private Const PATH_DEFAULT_IMG As String = "HY_TANSHO.疵無し.bmp"           'デフォルト画像のパス

    '///////////////////////////////////////////////////////////////////////////
    '構造体
    '///////////////////////////////////////////////////////////////////////////

    '///////////////////////////////////////////////////////////////////////////
    'メンバー変数
    '///////////////////////////////////////////////////////////////////////////
    Private WithEvents m_timMap As Timer                            '疵マップ更新タイマー
    Private WithEvents m_timStatus As Timer                         'ステータス情報更新タイマー
    Private WithEvents m_timCoil As Timer                           'コイル情報更新タイマー
    Private WithEvents m_timBrink As Timer                          'ブリンク表示タイマー

    Private mcls_Log As tClass_LogManager                           'ログ管理

    Private mcls_Param As clsParamManager                           'パラメータ管理
    Private mcls_PreParam As clsPreParamManager                     '前工程パラメータ管理クラス

    Private mcls_PointInf As clsPointInf                            'ポインタ情報
    Private mcls_DispMap As clsDispMap                              '疵マップ表示
    Private mcls_DispCoil As clsDispCoil                            'コイル情報表示
    Private mcls_DispStatus As clsDispStatus                        'ステータス情報表示
    Private mcls_DispDefect() As clsDispDefect                      '疵情報表示
    Private mcls_Alarm As clsDispAlarm                              '警告表示

    Private mfrm_PopImage As frmDefectImage                         '疵拡大画像表示
    Private mfrm_PopTcmImage As frmDefectImage2                     '疵拡大画像表示(TCM用)
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	Private mfrm_LongPdList As frmLongPdList                        '長周期欠陥一覧
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	Private m_nFormID As Integer = 0                                'フォーム識別番号
    Private m_nMapValue As Integer                                  'スクロール値(同期用)

    Private m_bScrollSync As Boolean = False                        'スクロール同期有無 (True:同期 False:非同期)
    Private m_bEnaDspCoilInf As Boolean = True                      'コイル情報表示正常フラグ (True:正常 False:異常)
    Private m_nPauseRec As Integer                                  '一時停止時現在レコード
    Private m_nMonitor As Integer                                   '画面番号

    Private m_nTcmImageWide As Integer = 25                         'TCM疵画像表示ゾーンの横幅
    Private m_bDspAlarm As Boolean = False                          '疵接近ランプ表示有無(True:表示、False:非表示)
    '///////////////////////////////////////////////////////////////////////////
    '追加イベント
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 更新モード変更
    ''' </summary>
    ''' <param name="sender">フォーム</param>
    ''' <param name="emRefreshMode">変更する更新モード</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeRefreshMode(ByVal sender As frmTansho, ByVal emRefreshMode As EM_REFRESH_MODE)

    ''' <summary>
    ''' 疵マップ間隔変更
    ''' </summary>
    ''' <param name="sender">フォーム</param>
    ''' <param name="stage">長手間隔</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeMapInterval(ByVal sender As frmTansho, ByVal stage As EM_LONG_POS)

    '''' <summary>
    '''' 疵画像表示モード変更
    '''' </summary>
    '''' <param name="sender">変更のあった探傷画面</param>
    '''' 	<param name="emMode">表示モード</param>
    '''' <remarks></remarks>
    'Public Event ev_ChangeDefectMode(ByVal sender As frmTansho, ByVal emMode As EM_DEFECT_MODE)

    ''' <summary>
    ''' オーバーレイ表示変更
    ''' </summary>
    ''' <param name="sender">変更のあった探傷画面</param>
    ''' <param name="bVisible">変更後の表示 True:表示 False:非表示</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeOverlyVisible(ByVal sender As frmTansho, ByVal bVisible As Boolean)

    ''' <summary>
    ''' 目盛り表示変更
    ''' </summary>
    ''' <param name="sender">変更のあった探傷画面</param>
    ''' <param name="bVisible">変更後の表示 True:表示 False:非表示</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeMemoryVisible(ByVal sender As frmTansho, ByVal bVisible As Boolean)

    ''' <summary>
    ''' 拡大表示変更
    ''' </summary>
    ''' <param name="sender">変更のあった探傷画面</param>
    ''' <param name="bEnable">変更後の表示 True:有効 False:無効</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeImageZoom(ByVal sender As frmTansho, ByVal bEnable As Boolean)

    ''' <summary>
    ''' 拡大表示倍率変更
    ''' </summary>
    ''' <param name="sender">変更のあった探傷画面</param>
    ''' <param name="nRate">表示倍率</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeZoomRate(ByVal sender As frmTansho, ByVal nRate As Integer)

    ''' <summary>
    ''' 同期モード変更
    ''' </summary>
    ''' <param name="sender">変更のあった探傷画面</param>
    ''' <param name="bSync">変更後のモード True:同期 False:非同期</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeSyncMode(ByVal sender As frmTansho, ByVal bSync As Boolean)

    ''' <summary>
    ''' ボタンタブ同期変更
    ''' </summary>
    ''' <param name="sender">変更のあった探傷画面</param>
    ''' <param name="bSync">変更後のモード True:同期 False:非同期</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeFuncSync(ByVal sender As frmTansho, ByVal bSync As Boolean)

    ''' <summary>
    ''' 画面更新モード同期変更
    ''' </summary>
    ''' <param name="sender">変更のあった探傷画面</param>
    ''' <param name="bSync">変更後のモード True:同期 False:非同期</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeModeSync(ByVal sender As frmTansho, ByVal bSync As Boolean)

    ''' <summary>
    ''' 縦間隔切替同期変更
    ''' </summary>
    ''' <param name="sender">変更のあった探傷画面</param>
    ''' <param name="bSync">変更後のモード True:同期 False:非同期</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeScaleSync(ByVal sender As frmTansho, ByVal bSync As Boolean)

    ''' <summary>
    ''' スクロール同期変更
    ''' </summary>
    ''' <param name="sender">変更のあった探傷画面</param>
    ''' <param name="bSync">変更後のモード True:同期 False:非同期</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeScrollSync(ByVal sender As frmTansho, ByVal bSync As Boolean)

    ''' <summary>
    ''' スクロール量変更
    ''' </summary>
    ''' <param name="sender">変更のあった探傷画面</param>
    ''' <param name="nValue">スクロール量</param>
    ''' <param name="emScrool">スクロール方向</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeScrollValue(ByVal sender As frmTansho, ByVal nValue As Integer, ByVal emScrool As EM_MAP_SCROOL)

    ''' <summary>
    '''  ファンクション処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Event ev_PressFunctionKey(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyEventArgs)


#Region "プロパティ"
    '///////////////////////////////////////////////////////////////////////////
    'プロパティ
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' この探傷画面のIDを取得します
    ''' </summary>
    ''' <returns>識別番号</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property FormID() As Integer
        Get
            Return m_nFormID
        End Get
    End Property

    ''' <summary>
    ''' 検査台番号を取得します
    ''' </summary>
    ''' <returns>検査台番号</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property KensaNo() As Integer
        Get
            If Not mcls_DispMap Is Nothing Then
                Return mcls_DispMap.KensaNo
            End If
            Return 0
        End Get
    End Property

    ''' <summary>
    ''' 疵マップ更新間隔設定と取得
    ''' </summary>
    ''' <value>更新間隔[ms]</value>
    ''' <returns>更新間隔[ms]</returns>
    ''' <remarks></remarks>
    Public Property MapInterval() As Integer
        Get
            Return m_timMap.Interval
        End Get
        Set(ByVal value As Integer)
            If Not m_timMap Is Nothing Then
                m_timMap.Interval = value
            End If
        End Set
    End Property

    ''' <summary>
    ''' ステータス情報更新間隔設定
    ''' </summary>
    ''' <value>更新間隔[ms]</value>
    ''' <returns>更新間隔[ms]</returns>
    ''' <remarks></remarks>
    Public Property StatusInterval() As Integer
        Get
            Return m_timStatus.Interval
        End Get
        Set(ByVal value As Integer)
            If Not m_timStatus Is Nothing Then
                m_timStatus.Interval = value
            End If
        End Set
    End Property

    ''' <summary>
    ''' コイル情報更新間隔設定
    ''' </summary>
    ''' <value>更新間隔[ms]</value>
    ''' <returns>更新間隔[ms]</returns>
    ''' <remarks></remarks>
    Public Property CoilInterval() As Integer
        Get
            Return m_timCoil.Interval
        End Get
        Set(ByVal value As Integer)
            If Not m_timCoil Is Nothing Then
                m_timCoil.Interval = value
            End If
        End Set
    End Property

    ''' <summary>
    ''' 疵マップのスクロール方向
    ''' </summary>
    ''' <value>False:下 True:上</value>
    ''' <returns>False:下 True:上</returns>
    ''' <remarks></remarks>
    Public Property ScrollDrection() As EM_MAP_SCROOL
        Get
            Return mcls_DispMap.MapDir
        End Get
        Set(ByVal value As EM_MAP_SCROOL)
            If Not mcls_DispMap Is Nothing Then
                mcls_DispMap.MapDir = value
            End If
        End Set
    End Property

    ''' <summary>
    ''' 表示間隔区分を取得します
    ''' </summary>
    ''' <returns>表示間隔</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property Stage() As EM_LONG_POS
        Get
            Return mcls_DispMap.Stage
        End Get
    End Property

    ''' <summary>
    ''' 表示表裏面番号を取得します
    ''' </summary>
    ''' <returns>表裏面</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property Men() As Integer
        Get
            Return mcls_DispMap.Men
        End Get
    End Property

    ''' <summary>
    ''' 疵マップDS位置
    ''' </summary>
    ''' <value>NORMAL:左がDS、REVERSE:左がWS</value>
    ''' <returns>NORMAL:左がDS、REVERSE:左がWS</returns>
    ''' <remarks></remarks>
    Public Property DSPos() As EM_MAP_CELL
        Get
            Return mcls_DispMap.DSPos
        End Get
        Set(ByVal value As EM_MAP_CELL)
            If Not mcls_DispMap Is Nothing Then
                mcls_DispMap.DSPos() = value
            End If
        End Set
    End Property


    ''' <summary>
    ''' DB変更通知
    ''' </summary>
    ''' <value>True：接続、False：未接続</value>
    ''' <returns>True：接続、False：未接続</returns>
    ''' <remarks></remarks>
    Public Property DBStatus() As Boolean
        Get
            Return mcls_DispStatus.DbStatus
        End Get
        Set(ByVal value As Boolean)
            If Not mcls_DispStatus Is Nothing Then
                mcls_DispStatus.DbStatus = value
            End If
        End Set
    End Property

    ''' <summary>
    ''' 疵マップスクロール同期/非同期
    ''' </summary>
    ''' <value>True：同期、False：非同期</value>
    ''' <returns>True：同期、False：非同期</returns>
    ''' <remarks></remarks>
    Public Property ScrollSync() As Boolean
        Get
            Return m_bScrollSync
        End Get
        Set(ByVal value As Boolean)
            m_bScrollSync = value
            If m_bScrollSync Then
                btnF02.BackColor = Color.Aqua
            Else
                btnF02.BackColor = Color.FromKnownColor(KnownColor.Control)
            End If
        End Set
    End Property

    ''' <summary>
    ''' オーバーレイ表示設定ON/OFF
    ''' </summary>
    ''' <value>True：表示、False：非表示</value>
    ''' <returns>True：表示、False：非表示</returns>
    ''' <remarks></remarks>
    Public Property Overlay() As Boolean
        Get
            Return mcls_DispDefect(0).OverlayVisible
        End Get
        Set(ByVal value As Boolean)

            ' mcls_DispDefect のオーバーレイ表示を設定
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.OverlayVisible = value
            Next

            ' 拡大画像 のオーバーレイ表示を設定
            mfrm_PopImage.SetVisibleOverlay(value)
            ' 拡大画像(TCM) のオーバーレイ表示を設定
            mfrm_PopTcmImage.SetVisibleOverlay(value)

            ' その他変更する対象があれば、ここに記述する

            ' 切替ボタンの色変更処理
            If value Then
                ' 表示時のボタン色設定
                btnF07.BackColor = Color.Cyan
            Else
                ' 非表示時のボタン色設定
                btnF07.BackColor = Color.FromKnownColor(KnownColor.Control)
            End If

        End Set
    End Property

    ''' <summary>
    ''' 目盛り表示設定ON/OFF
    ''' </summary>
    ''' <value>True：表示、False：非表示</value>
    ''' <returns>True：表示、False：非表示</returns>
    ''' <remarks></remarks>
    Public Property Memori() As Boolean
        Get
            Return mcls_DispDefect(0).MemoriVisible
        End Get
        Set(ByVal value As Boolean)

            ' mcls_DispDefect の目盛り表示を設定
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.MemoriVisible = value
            Next

            ' 拡大画像 の目盛り表示を設定
            mfrm_PopImage.SetVisibleMemory(value)
            ' 拡大画像(TCM) の目盛り表示を設定
            mfrm_PopTcmImage.SetVisibleMemory(value)

            ' その他変更する対象があれば、ここに記述する

            ' 切替ボタンの色変更処理
            If value Then
                ' 表示時のボタン色設定
                btnF08.BackColor = Color.Cyan
            Else
                ' 非表示時のボタン色設定
                btnF08.BackColor = Color.FromKnownColor(KnownColor.Control)
            End If

        End Set
    End Property

    ''' <summary>
    ''' 画面番号取得
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property MonitorNo() As Integer
        Get
            Return m_nMonitor
        End Get
    End Property

#End Region

    '///////////////////////////////////////////////////////////////////////////
    'メソッド
    '///////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="nFormID">画面ID</param>
    ''' <param name="clsLog">ログ管理</param>
    ''' <param name="clsParam">パラメータ管理</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal nFormID As Integer, ByRef clsLog As tClass_LogManager, ByRef clsParam As clsParamManager, ByRef cls_PreParam As clsPreParamManager)

        'この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()
        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        Dim s As Size
        s.Width = WIN_WIDTH_MAX
        s.Height = WIN_HEIGHT_MAX
        Me.MaximumSize = s
        s.Width = WIN_WIDTH_MIN
        s.Height = WIN_HEIGHT_MIN
        Me.MinimumSize = s

        Try
            '初期化
            m_nFormID = nFormID
            mcls_Log = clsLog
            mcls_Param = clsParam
            mcls_PreParam = cls_PreParam
            m_nMonitor = 1

            '-----------------------------------------------
            'タイマー
            '-----------------------------------------------
            m_timMap = New Timer()                          '表面疵マップ更新タイマー
            m_timStatus = New Timer()                       'ステータス情報更新タイマー
            m_timCoil = New Timer()                         'コイル情報更新タイマー

            '-----------------------------------------------
            'ポインタ情報
            '-----------------------------------------------
            mcls_PointInf = New clsPointInf(clsLog)

			'-----------------------------------------------
			'ステータス情報
			'-----------------------------------------------
			mcls_DispStatus = InitStatusInf(ucStatus, UcCoil1)
			'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
			mcls_DispStatus.LongPdGuidanceMessage = mcls_Param.Pro_LongPd_GuidanceMessage
			mcls_DispStatus.LongPdGuidanceForeColor = mcls_Param.Pro_LongPd_GuidanceForeColor
			mcls_DispStatus.LongPdGuidanceBackColor = mcls_Param.Pro_LongPd_GuidanceBackColor
			'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

			'-----------------------------------------------
			'コイル情報
			'-----------------------------------------------
			mcls_DispCoil = InitCoilInf(UcCoil1)

            '-----------------------------------------------
            '疵拡大画像画面
            '-----------------------------------------------
            ' タスク名(HY_TANSHO) からini名セクション決定
            Dim strSection As String = GetMyTaskName() & "_FORM" & nFormID
            Dim bPicDirect As Boolean = CBool(tMod_IniFile.ReadIniInt(strSection, "DEFIMAGE_DIRECT", TASKINI_NAME, 0))
            mfrm_PopImage = New frmDefectImage(clsLog, mcls_Param, PATH_DEFAULT_IMG, strSection, DSP_KIRI_IMAGE_MAX, bPicDirect)
            mfrm_PopImage.Owner = Me

            '-----------------------------------------------
            'TCM疵画像画面
            '-----------------------------------------------
            ' タスク名(HY_TANSHO) からini名セクション決定
            mfrm_PopTcmImage = New frmDefectImage2(clsLog, mcls_PreParam, PATH_DEFAULT_IMG, strSection, 1, bPicDirect)
            mfrm_PopTcmImage.Owner = Me

			'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
			'-----------------------------------------------
			'長周期欠陥一覧画面
			'-----------------------------------------------
			mfrm_LongPdList = New frmLongPdList(clsLog, clsParam)
			mfrm_LongPdList.Owner = Me
			mfrm_LongPdList.Visible = False
			'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

			'-----------------------------------------------
			'疵表示
			'-----------------------------------------------
			ReDim mcls_DispDefect(DSP_DEFECT_MAX - 1)
            mcls_DispDefect(0) = InitDefInf(UcDefect1, g_ColorNewKizu, 1)      '最新の疵
            mcls_DispDefect(1) = InitDefInf(UcDefect2, g_ColorOldKizu, 2)      '１つ前の疵

            '-----------------------------------------------
            'マップ表示
            '-----------------------------------------------
            Dim typMapInf As clsDispMap.TYP_MAP_INF = Nothing
            With typMapInf
                .dgvMap = dgvMap                                            '表面疵マップ
                .dgvDeep = dgvDeep                                          '疵深さリスト
                .dgvTcm = dgvOth1                                           'TCMリスト
                .vsbMap = vsbMap                                            'スクロールバー
                .lblKensaPoint = lblKensaPoint
            End With

            mcls_DispMap = New clsDispMap(typMapInf, clsParam, cls_PreParam, clsLog, mcls_PointInf, mcls_DispCoil, nFormID)

            vsbMap.Enabled = False
            vsbMap.Value = 0

            '画面設定
            GetIniSetting(nFormID)                                              '設定値取得

            '疵マップのライン色設定
            lblShearTitle.ForeColor = g_ColorShar                               'シャーカット線
            lblYousetuTitle.ForeColor = g_ColorYousetu                          '溶接線


            '-----------------------------------------------
            '疵接近ランプ
            '-----------------------------------------------
            Dim lblLamp() As Label = {lblAlarm1, lblAlarm2, lblAlarm3, lblAlarm4, lblAlarm5, lblAlarm6, lblAlarm7, lblAlarm8}
            Dim typAlarmDsp As clsDispAlarm.TYP_ALARM_DSP = Nothing
            With typAlarmDsp
                .lblLamp = lblLamp
            End With
            mcls_Alarm = New clsDispAlarm(typAlarmDsp, clsLog)
            If m_bDspAlarm Then
                pnlAlarm.Visible = True
            Else
                pnlAlarm.Visible = False
            End If

            '-----------------------------------------------
            'イベントの設定
            '-----------------------------------------------
            'マップ自動更新
            AddHandler mcls_DispMap.SelectMapCell_Auto, AddressOf frmTansho_SelectMapCellAuto
            AddHandler mcls_DispMap.SelectTcmMapCell_Auto, AddressOf frmTansho_SelectTcmMapCellAuto
            'マップ自動更新(データが無い場合初期化)
            AddHandler mcls_DispMap.SelectMapCell_AutoDspInit, AddressOf frmTansho_SelectMapCellAutoDspInit
            AddHandler mcls_DispMap.SelectTcmMapCell_AutoDspInit, AddressOf frmTansho_SelectTcmMapCellAutoDspInit
            'マップ選択
            AddHandler mcls_DispMap.SelectMapCell_Click, AddressOf frmTansho_SelectMapCellClick
            AddHandler mcls_DispMap.SelectMapTcmCell_Click, AddressOf frmTansho_SelectTcmMapCellClick
            'スクロールバー
            AddHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged
            'マウスホイールスクロール
            AddHandler dgvMap.MouseWheel, AddressOf dgvMap_MouseWheel
            '疵画像ダブルクリック
            AddHandler UcDefect1.picImg1.DoubleClick, AddressOf pic_DoubleClick
            AddHandler UcDefect1.picImg2.DoubleClick, AddressOf pic_DoubleClick
            AddHandler UcDefect2.picImg1.DoubleClick, AddressOf pic_DoubleClick
            AddHandler UcDefect2.picImg2.DoubleClick, AddressOf pic_DoubleClick

        Catch ex As Exception
            If Not mcls_Log Is Nothing Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("探傷画面生成異常[{0}],[{1}]", nFormID, ex.Message))
            End If
        End Try
    End Sub

    '***************************************************************************
    ' メインモジュール呼び出し関数(外部呼出し)
    '***************************************************************************
    ''' <summary>
    ''' リソース破棄
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub frmTansho_Disposed()
        Try
            Dim strSection As String = HY_TANSHO & "_FORM" & m_nFormID          'セクション名作成

            '///////////////////////////////////////////////
            'KizuLocal.iniの設定
            '///////////////////////////////////////////////
            If Not mcls_DispMap Is Nothing Then
                tMod_IniFile.WriteIni(strSection, "LONG_POS_INF", LOCALINI_NAME, CType(mcls_DispMap.Stage, Integer).ToString)
            End If

            If Not mcls_DispDefect Is Nothing Then
                tMod_IniFile.WriteIni(strSection, "OVERLAY_VIEW_INF", LOCALINI_NAME, IIf(mcls_DispDefect(0).OverlayVisible, 1, 0).ToString)
                tMod_IniFile.WriteIni(strSection, "SCALE_VIEW_INF", LOCALINI_NAME, IIf(mcls_DispDefect(0).MemoriVisible, 1, 0).ToString)
                'tMod_IniFile.WriteIni(strSection, "SCROLL_VIEW_INF", LOCALINI_NAME, IIf(ScrollSync = True, 1, 0).ToString)
            End If

            'イベント解除
            RemoveHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged
            RemoveHandler mcls_DispMap.SelectMapCell_Auto, AddressOf frmTansho_SelectMapCellAuto
            RemoveHandler mcls_DispMap.SelectMapCell_AutoDspInit, AddressOf frmTansho_SelectMapCellAutoDspInit
            RemoveHandler mcls_DispMap.SelectMapCell_Click, AddressOf frmTansho_SelectMapCellClick
            RemoveHandler dgvMap.MouseWheel, AddressOf dgvMap_MouseWheel
            RemoveHandler UcDefect1.picImg1.DoubleClick, AddressOf pic_DoubleClick
            RemoveHandler UcDefect2.picImg1.DoubleClick, AddressOf pic_DoubleClick

            'タイマー破棄
            If Not m_timMap Is Nothing Then
                m_timMap.Stop()
                m_timMap.Dispose()
                m_timMap = Nothing
            End If
            If Not m_timStatus Is Nothing Then
                m_timStatus.Stop()
                m_timStatus.Dispose()
                m_timStatus = Nothing
            End If
            If Not m_timCoil Is Nothing Then
                m_timCoil.Stop()
                m_timCoil.Dispose()
                m_timCoil = Nothing
            End If

            'サブクラス解放
            If Not mcls_PointInf Is Nothing Then
                mcls_PointInf.Dispose()
                mcls_PointInf = Nothing
            End If
            If Not mcls_DispMap Is Nothing Then
                mcls_DispMap.Dispose()
                mcls_DispMap = Nothing
            End If
            If Not mcls_DispDefect Is Nothing Then
                For Each dsp As clsDispDefect In mcls_DispDefect
                    If Not dsp Is Nothing Then
                        dsp.DisposeImage()
                        dsp.Dispose()
                    End If
                Next
                mcls_DispDefect = Nothing
            End If
            If Not mcls_DispCoil Is Nothing Then
                mcls_DispCoil.Dispose()
                mcls_DispCoil = Nothing
            End If
            If Not mcls_DispStatus Is Nothing Then
                mcls_DispStatus.Dispose()
                mcls_DispStatus = Nothing
            End If
            If Not mfrm_PopImage Is Nothing Then
                mfrm_PopImage.Dispose()
                mfrm_PopImage = Nothing
            End If
			If Not mfrm_PopTcmImage Is Nothing Then
				mfrm_PopTcmImage.Dispose()
				mfrm_PopTcmImage = Nothing
			End If
			'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
			If Not mfrm_LongPdList Is Nothing Then
				mfrm_LongPdList.Dispose()
				mfrm_LongPdList = Nothing
			End If
			'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

			Me.Dispose()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("開放イベント異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 表示初期化
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInit()
        Try

            mcls_DispMap.ClearMap()
            mcls_DispMap.ClearDeep()
            mcls_DispMap.ClearTcm()
            mcls_DispMap.ClearMaxRowCnt()

            '-------------------------------------------------------------------
            '各表示の初期化
            '-------------------------------------------------------------------
            InitLabel()
            'DspMapInfInit()                                                    'マップ情報初期化
            mcls_DispStatus.DspInit()                                           'ステータス情報初期化
            mcls_DispCoil.DspInit()                                             'コイル情報初期化
            mcls_DispCoil.DispCoilLen("")
            mcls_DispCoil.DispMen(Men)                                          '面情報表示
            dgvMap.Refresh()                                                    '疵マップ再描画
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.DspInit()
            Next

            ChangeRefreshMode(EM_REFRESH_MODE.AUTO)

            SetMapInterval(mcls_DispMap.Stage)

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("探傷画面全初期化異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵画像表示ゾーンの横幅を設定
    ''' </summary>
    ''' <param name="nWidth">画像表示ゾーンの横幅[mm]</param>
    ''' <remarks></remarks>
    Public Sub SetImageWidth(ByVal nWidth As Integer)
        Try
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.ImageWidth = nWidth
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像ゾーンの設定異常 Width={0}[{1}]", nWidth, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' TCM疵画像表示ゾーンの横幅を設定
    ''' </summary>
    ''' <param name="nWidth">画像表示ゾーンの横幅[mm]</param>
    ''' <remarks></remarks>
    Public Sub SetTcmImageWidth(ByVal nWidth As Integer)
        Try
            m_nTcmImageWide = nWidth
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("Tcm疵画像ゾーンの設定異常 Width={0}[{1}]", nWidth, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' マップスクロール
    ''' </summary>
    ''' <param name="nValue">スクロール値</param>
    ''' <remarks></remarks>
    Public Sub SetScrollValue(ByVal nValue As Integer, ByVal emMapScrool As EM_MAP_SCROOL)
        Dim nData As Integer = nValue
        Try
            vsbMap.ValueProgramable = nValue
            m_nMapValue = nValue
            If m_bScrollSync Then                                               'スクロール同期の場合のみ実行
                SetScroll(nData)
            End If
            'End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("スクロール量設定異常 value={0}[{1}]", nValue, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' ブリンク表示
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub m_timBrink_Tick()
        Try
            If mcls_DispMap.RefreshMode = EM_REFRESH_MODE.PAUSE Then
                If lblMode.BackColor = Color.Red And lblMode.ForeColor = Color.White Then
                    lblMode.BackColor = g_ColorControl
                    lblMode.ForeColor = Color.Black
                ElseIf lblMode.BackColor = g_ColorControl And lblMode.ForeColor = Color.Black Then
                    lblMode.BackColor = Color.Red
                    lblMode.ForeColor = Color.White
                End If
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("ブリンク表示で例外発生[{0}]", ex.Message))
        End Try
    End Sub

    '***************************************************************************
    ' イベントハンドラ関数
    '***************************************************************************
    ''' <summary>
    ''' 探傷画面ロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmTansho_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Try

            DspInit()                                                           '画面全初期化実行

            mcls_DispCoil.Load()

            vsbMap.Enabled = False
            vsbMap.Value = 0

            m_timStatus.Start()

            mcls_DispStatus.DspStatus(Men)                                'ステータス表示

            AddHandler TableLayoutPanel8.Resize, AddressOf MapReSize
            MapReSize(Nothing, Nothing)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("探傷画面ロード異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' キー操作
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub frmTansho_KeyDown_(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyDown
        RaiseEvent ev_PressFunctionKey(sender, e)
    End Sub

    ''' <summary>
    ''' ステータス情報更新
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub m_timStatus_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_timStatus.Tick
		Try
			'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
			mcls_DispStatus.LongPdGuidanceMessage = mcls_Param.Pro_LongPd_GuidanceMessage
			mcls_DispStatus.LongPdGuidanceForeColor = mcls_Param.Pro_LongPd_GuidanceForeColor
			mcls_DispStatus.LongPdGuidanceBackColor = mcls_Param.Pro_LongPd_GuidanceBackColor
			'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

			mcls_DispStatus.DspStatus(Men)                                    'ステータス表示
		Catch ex As Exception
			mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("ステータス表示異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' コイル情報更新
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub m_timCoil_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_timCoil.Tick
        Try
            Dim nRecNo As Integer
            Dim coilinf As COINFO_BASE = Nothing

            '初期化
            coilinf.initialize()

            If EM_DEFECT_MODE.TRACKING = mcls_DispMap.MapUpdateMode Then
                nRecNo = mcls_DispMap.GetKensaPosCoilRec()
            Else
                nRecNo = mcls_PointInf.RecDisp(KensaNo)
            End If

            If 0 >= nRecNo Then
                Exit Try
            End If

            If mcls_DispCoil.GetCoilInf(nRecNo, coilinf) Then             'コイル情報テーブル読込
                mcls_DispCoil.DspCoil(nRecNo, coilinf, Men)               'コイル情報表示
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("コイル情報更新タイムアウト異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵マップ更新
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub m_timMap_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_timMap.Tick

        Try
            If m_bDspAlarm And EM_REFRESH_MODE.AUTO = mcls_DispMap.RefreshMode Then
                '警報ガイダンス表示有かつ自動更新
                pnlAlarm.Visible = True
                TableLayoutPanel7.Visible = False

                Dim nDspPos As Integer = 0 ' 検査台 表面表示
                If EM_DIV_DISP.DIV_LEN_POS_DSP_BOT = KensaNo Then  ' 検査台 裏面表示
                    nDspPos = 1
                End If

                mcls_Alarm.DspDefLamp(nDspPos, Men, CBool(IIf(DSPos = EM_MAP_CELL.NORMAL, True, False)))
            Else
                pnlAlarm.Visible = False
                TableLayoutPanel7.Visible = True
            End If

            '表示モード
            Select Case mcls_DispMap.RefreshMode
                Case EM_REFRESH_MODE.AUTO
                    ' ポインタ情報読み込み
                    mcls_PointInf.ReadHyPoint()
                    ' 疵MAP更新
                    mcls_DispMap.DspData(Men)

                    '' 疵画像タイトルラベル設定
                    If EM_DEFECT_MODE.TRACKING = mcls_DispMap.MapUpdateMode Then
                        Dim nKizu1 As Integer = mcls_DispMap.GetSelectKizuPos(EM_IMAGE_POS.IMG1)
                        Dim nKizu2 As Integer = mcls_DispMap.GetSelectKizuPos(EM_IMAGE_POS.IMG2)

                        If 0 = nKizu1 And 0 = nKizu2 Then
                            UcDefect1.lblDefTitle.Text = "前疵"
                            UcDefect1.lblDefTitle.Tag = False
                            UcDefect2.lblDefTitle.Text = "前々疵"
                            UcDefect2.lblDefTitle.Tag = False
                        ElseIf 0 < nKizu1 And 0 = nKizu2 Then
                            UcDefect1.lblDefTitle.Text = "次疵"
                            UcDefect1.lblDefTitle.Tag = True
                            UcDefect2.lblDefTitle.Text = "前疵"
                            UcDefect2.lblDefTitle.Tag = False
                        Else
                            UcDefect1.lblDefTitle.Text = "次疵"
                            UcDefect1.lblDefTitle.Tag = True
                            UcDefect2.lblDefTitle.Text = "次々疵"
                            UcDefect2.lblDefTitle.Tag = True
                        End If
                    Else
                        ' 疵画像タイトルラベル設定
                        UcDefect1.lblDefTitle.Text = "最新の疵"
                        UcDefect1.lblDefTitle.Tag = True
                        UcDefect2.lblDefTitle.Text = "一つ前の疵"
                        UcDefect2.lblDefTitle.Tag = True
                    End If

                    '距離
                    Dim nLen As Integer = mcls_DispMap.GetInCoilLen()
                    If nLen >= 0 Then
                        mcls_DispCoil.DispCoilLen(nLen.ToString)
                    End If

                    ' 画像の表示リトライを行う(画像表示時に画像ファイルがまだ生成されていない場合があるため）
                    For Each def As clsDispDefect In mcls_DispDefect
                        def.DspDefectRetry()
                    Next

                Case EM_REFRESH_MODE.PAUSE
                    ' 一時停止中に特定の条件となった場合は、一時停止を自動的に解除する処理
                    ' 検査台位置のレコード番号を取得し、一時停止した時のレコード番号と比較
                    Using clsPointTmp As New clsPointInf(mcls_Log)
                        clsPointTmp.ReadHyPoint()
                        Dim nNowRec As Integer = clsPointTmp.RecDisp(KensaNo)       '現在レコード

                        If nNowRec <> m_nPauseRec Then      '現在レコードと一時停止時のレコードが異なる
                            SelectF01()                     '一時停止解除
                        End If

                    End Using

            End Select
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ更新タイムアウト異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' フォームクローズ(システムボタン)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub frmTansho_FormClosing(sender As Object, e As FormClosingEventArgs) Handles MyBase.FormClosing
        'クローズボタンによるフォーム閉はキャンセル。
        e.Cancel = True
        'タスク終了
        EndNotice()
    End Sub

    ''' <summary>
    ''' 疵マップ/疵セル選択イベント(オート時)
    ''' </summary>
    ''' <param name="nMen">表裏面</param>
    ''' <param name="pntKizu">選択疵情報</param>
    ''' <param name="emInf">疵情報位置</param>
    ''' <remarks></remarks>
    Private Sub frmTansho_SelectMapCellAuto(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE, ByVal emInf As clsDispMap.EM_DSP_KIZU_INF)
        Try
            mcls_DispDefect(CType(emInf, Integer)).DspDefect(nMen, pntKizu, True)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ/疵セル選択イベントで例外発生 表裏面={0}[{1}]", nMen, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵マップ/疵セル選択消去イベント(オート時)
    ''' </summary>
    ''' <param name="emInf">疵情報位置</param>
    ''' <remarks></remarks>
    Private Sub frmTansho_SelectMapCellAutoDspInit(ByVal emInf As clsDispMap.EM_DSP_KIZU_INF)
        Try
            mcls_DispDefect(CType(emInf, Integer)).DspInit()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ/疵セル選択消去イベントで例外発生 表裏面={0}[{1}]", Men, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵マップ/疵セル選択(TCM)イベント(オート時)
    ''' </summary>
    ''' <param name="nMen">表裏面</param>
    ''' <param name="pntKizu">選択疵情報</param>
    ''' <remarks></remarks>
    Private Sub frmTansho_SelectTcmMapCellAuto(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)
        Try
            DispDefectTcmImg(nMen, pntKizu)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ/疵セル選択(TCM)イベントで例外発生 表裏面={0}[{1}]", nMen, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵マップ/疵セル選択(TCM)消去イベント(オート時)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub frmTansho_SelectTcmMapCellAutoDspInit()
        Try
            If mfrm_PopTcmImage.Visible Then                               'TCM疵画像拡大表示中
                mfrm_PopTcmImage.CloseFrom()                               'TCM拡大画像OFFに
            End If
            mcls_DispMap.ClearTcm()
            dgvOth1.Refresh()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ/疵セル選択(TCM)消去イベントで例外発生 表裏面={0}[{1}]", Men, ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' 疵マップ/疵セル選択イベント(選択時)
    ''' </summary>
    ''' <param name="nMen">表裏面</param>
    ''' <param name="pntKizu">選択疵情報</param>
    ''' <remarks></remarks>
    Private Sub frmTansho_SelectMapCellClick(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE, ByVal bSelect As Boolean)
        Dim nLen As Integer = 0
        Dim coilinf As COINFO_BASE = New COINFO_BASE

        Try
            If EM_REFRESH_MODE.PAUSE <> mcls_DispMap.RefreshMode Then
                Exit Try
            End If


            If bSelect Then
                For nCnt As Integer = mcls_DispDefect.Count - 1 To 1 Step -1
                    Dim strVal As TYP_MAP_LIST_VALUE = Nothing
                    If mcls_DispMap.GetSelectKizu(nCnt, strVal) AndAlso 0 < strVal.nKizuNo Then
                        mcls_DispDefect(nCnt).DspDefect(nMen, mcls_DispDefect(nCnt - 1).CurrentDefect, False)
                    Else
                        mcls_DispDefect(nCnt).DspInit()
                    End If
                Next

                '疵情報を表示
                mcls_DispDefect(0).DspDefect(nMen, pntKizu, False)
            End If

            If mcls_DispCoil.GetCoilInf(pntKizu.nRec, coilinf) Then    'コイル情報テーブル読込
                mcls_DispCoil.DspCoil(pntKizu.nRec, coilinf, Men)      'コイル情報表示
            End If

            If mfrm_PopImage.Visible Then                              '疵画像拡大表示中
                'ダブルクリック以外は最新疵の情報を表示
                mfrm_PopImage.DispImgNo = 1
                DispDefectImg()
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ/疵セル選択イベント 表裏面={0}[{1}]", nMen, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵画像ダブルクリック
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub pic_DoubleClick(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Try

            If EM_REFRESH_MODE.PAUSE <> mcls_DispMap.RefreshMode Then
                '一時停止中ではない
                Exit Try
            End If

            For Each dsp As clsDispDefect In mcls_DispDefect
                If Not dsp.ImageZoom Then Continue For
                For Each picbox As PictureBox In dsp.DefectViewInf.picImg
                    If sender Is picbox Then
                        ' ダブルクリックしたPictureBoxと一致したら、疵画像拡大表示
                        dsp.ShowPopup(mfrm_PopImage)
                        Exit Try
                    End If
                Next
            Next

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像ダブルクリック異常[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' 疵マップスクロール
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub vsbMap_ValueChanged(ByVal sender As System.Object, ByVal e As System.Boolean)
        Dim value As Integer = vsbMap.Value
        Try
            If Not e Then
                SetScroll(value)
                RaiseEvent ev_ChangeScrollValue(Me, value, mcls_DispMap.MapDir) '疵マップスクロール値変更
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップスクロールイベント異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵マップマウススクロール
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub dgvMap_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
        Dim nValue As Integer = vsbMap.Value
        Try

            If e.Delta > 0 Then
                nValue = nValue - 3
                If nValue < vsbMap.Minimum Then nValue = vsbMap.Minimum
            Else
                nValue = nValue + 3
                If nValue > vsbMap.Maximum - vsbMap.LargeChange + 1 Then nValue = vsbMap.Maximum - vsbMap.LargeChange + 1
            End If

            vsbMap.Value = nValue

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マウススクロールイベント処理異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵マップリサイズ
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub MapReSize(ByVal sender As Object, ByVal e As System.EventArgs)
        mcls_DispMap.MapResizeH()
    End Sub

    ''' <summary>
    ''' 疵マップ/疵セル選択イベント(選択時)
    ''' </summary>
    ''' <param name="nMen">表裏面</param>
    ''' <param name="pntKizu">選択疵情報</param>
    ''' <remarks></remarks>
    Private Sub frmTansho_SelectTcmMapCellClick(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)
        Dim nLen As Integer = 0
        'Dim coilinf As COINFO_BASE = New COINFO_BASE

        Try
            If EM_REFRESH_MODE.PAUSE <> mcls_DispMap.RefreshMode Then
                Exit Try
            End If

            'If mcls_DispCoil.GetCoilInf(pntKizu.nRec, coilinf) Then    'コイル情報テーブル読込
            '    mcls_DispCoil.DspCoil(pntKizu.nRec, coilinf, Men)      'コイル情報表示
            'End If

            DispDefectTcmImg(nMen, pntKizu)

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ/疵セル選択イベント 表裏面={0}[{1}]", nMen, ex.Message))
        End Try
    End Sub


    '***************************************************************************
    ' 内部関数
    '***************************************************************************
    ''' <summary>
    ''' 設定値取得
    ''' </summary>
    ''' <param name="nFormID">画面識別番号</param>
    ''' <remarks></remarks>
    Private Sub GetIniSetting(ByVal nFormID As Integer)
        Try
            'セクション名決定
            Dim strSection As String = HY_TANSHO & "_FORM" & nFormID

            '  ----------------------- KizuTask.iniファイルからの読み込み  -----------------------

            '表示面番号
            Dim nMenNo As Integer = CInt(IIf(tMod_IniFile.ReadIniInt(strSection, "FROM_MEN_NUM", TASKINI_NAME, 0) = 0, 0, 1))
            Select Case nMenNo
                Case 0                                                          '表面
                    'lblFace.Text = "TOP"
                    'lblFace.BackColor = Color.GreenYellow
                Case 1                                                          '裏面
                    'lblFace.Text = "BOT"
                    'lblFace.BackColor = Color.HotPink
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("表示面番号異常[{0}],[{1}]", nFormID, nMenNo))
                    nMenNo = 1
                    'lblFace.Text = "TOP"
                    'lblFace.BackColor = Color.GreenYellow
            End Select
            mcls_DispMap.Men = nMenNo

            'モニター位置
            Dim nFormPos As Integer = tMod_IniFile.ReadIniInt(strSection, "FROM_POS_NUM", TASKINI_NAME, 0)
            Dim clsMonitor() As Screen = Screen.AllScreens()                    'モニター一覧
            If 0 >= nFormPos Or clsMonitor.Length < nFormPos Then                '表示したいモニターがない場合
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("モニター位置異常[{0}],[{1}]", nFormID, nFormPos))
                nFormPos = 1                                                    'プライマリディスプレイに表示
            End If
            m_nMonitor = nFormPos
            Me.Bounds = clsMonitor(nFormPos - 1).Bounds

            '通板ポイント位置
            Dim nKensaNo As Integer = tMod_IniFile.ReadIniInt(strSection, "KENSA_NUM", TASKINI_NAME, 0)
            If nKensaNo < 0 OrElse nKensaNo > MAX_DISP_POS Then
                nKensaNo = 0
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("通板ポイント位置異常[{0}],[{1}]", nFormID, nKensaNo))
            End If
            mcls_DispMap.KensaNo = nKensaNo

            '検査台名称
            Dim strKensaName As String = tMod_IniFile.ReadIniStr(strSection, "KENSA_NAME", TASKINI_NAME)
            Me.Text = String.Format("[{0}] 探傷画面", SYSTEM_TITLE)

            '疵マップ進行方向
            Dim nMapScrollDir As Integer = tMod_IniFile.ReadIniInt(strSection, "MAP_SCROLL_DIRECT", TASKINI_NAME, 0)
            Select Case nMapScrollDir
                Case 0
                    mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD            '下から上
                Case 1
                    mcls_DispMap.MapDir = EM_MAP_SCROOL.HEAD_TO_TAIL            '上から下
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ進行方向異常[{0}],[{1}]", nFormID, nMapScrollDir))
                    mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD
            End Select


            '疵ラベルDS位置
            Dim nMapSide As Integer = tMod_IniFile.ReadIniInt(strSection, "MAP_VIEW_DIRECT", TASKINI_NAME, 0)
            Select Case nMapSide
                Case 0
                    mcls_DispMap.Side = EM_MAP_SIDE.NORMAL                      '左がDS
                Case 1
                    mcls_DispMap.Side = EM_MAP_SIDE.REVERSE                     '左がWS
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵ラベル反転異常[{0}],[{1}]", nFormID, nMapSide))
                    mcls_DispMap.Side = EM_MAP_SIDE.NORMAL
            End Select

			'// 2023.11.17 最終工程検査装置高機能化その２ -------------------->>>>>
			' 疵マップ幅位置番号
			Dim nMapViewWidthNo As Integer = tMod_IniFile.ReadIniInt(strSection, "MAP_VIEW_WIDTH_NO", TASKINI_NAME, 0)
			Select Case nMapViewWidthNo
				Case 0
					mcls_DispMap.WidthNo = EM_MAP_WIDTH_NO.NORMAL                      '左から1,2,・・・
				Case 1
					mcls_DispMap.WidthNo = EM_MAP_WIDTH_NO.REVERSE                     '右から1,2,・・・
				Case Else
					mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ幅位置番号[{0}],[{1}]", nFormID, nMapViewWidthNo))
					mcls_DispMap.WidthNo = EM_MAP_WIDTH_NO.NORMAL
			End Select
			'// 2023.11.17 最終工程検査装置高機能化その２ <<<<<--------------------

            '疵マップDS位置
            Dim nMapCell As Integer = tMod_IniFile.ReadIniInt(strSection, "MAP_CELL_DIRECT", TASKINI_NAME, 0)
            Select Case nMapCell
                Case 0
                    mcls_DispMap.DSPos = EM_MAP_CELL.NORMAL                     '左がDS
                Case 1
                    mcls_DispMap.DSPos = EM_MAP_CELL.REVERSE                    '左がWS
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ反転異常[{0}],[{1}]", nFormID, nMapCell))
                    mcls_DispMap.DSPos = EM_MAP_CELL.NORMAL
            End Select

            '疵画像上下反転
            Dim nImgVMode As EM_IMAGE_TOP = CType(tMod_IniFile.ReadIniInt(strSection, "IMAGE_TOP_BOTTOM_INF", TASKINI_NAME, 0), EM_IMAGE_TOP)
            Select Case nImgVMode
                Case EM_IMAGE_TOP.NORMAL                                        '反転無し
                Case EM_IMAGE_TOP.REVERSE                                       '反転有り
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像上下反転異常[{0}],[{1}]", nFormID, nImgVMode))
                    nImgVMode = EM_IMAGE_TOP.NORMAL
            End Select
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.IsTurnTB = nImgVMode
            Next

            '疵画像左右反転
            Dim nImgHMode As EM_IMAGE_SIDE = CType(tMod_IniFile.ReadIniInt(strSection, "IMAGE_SIDE_INF", TASKINI_NAME, 0), EM_IMAGE_SIDE)
            Select Case nImgHMode
                Case EM_IMAGE_SIDE.NORMAL                                       '反転無し
                Case EM_IMAGE_SIDE.REVERSE                                      '反転有り
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像左右反転異常[{0}],[{1}]", nFormID, nImgHMode))
                    nImgHMode = EM_IMAGE_SIDE.NORMAL
            End Select
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.IsTurnLR = nImgHMode
            Next

            ''疵画像90度回転設定
            'Dim nImg90 As EM_IMAGE_90 = CType(tMod_IniFile.ReadIniInt(strSection, "IMAGE_90_INF", TASKINI_NAME, 0), EM_IMAGE_90)
            'Select Case nImg90
            '    Case EM_IMAGE_90.NORMAL                                         '反転無し
            '    Case EM_IMAGE_90.LOTATE                                         '反転有り
            '    Case Else
            '        mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像左右反転異常[{0}],[{1}]", nFormID, nImg90))
            '        nImg90 = EM_IMAGE_90.NORMAL
            'End Select
            'For Each dsp As clsDispDefect In mcls_DispDefect
            '    dsp.IsTurn90 = nImg90
            'Next

            '疵画像ラベルDS位置
            Dim nImgSide As EM_MAP_SIDE = CType(tMod_IniFile.ReadIniInt(strSection, "IMAGE_VIEW_DIRECT", TASKINI_NAME, 0), EM_MAP_SIDE)
            Select Case nImgSide
                Case EM_MAP_SIDE.NORMAL                                         '左がDS
                Case EM_MAP_SIDE.REVERSE                                        '左がWS
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像ラベルDS位置異常[{0}],[{1}]", nFormID, nImgSide))
                    nImgSide = EM_MAP_SIDE.NORMAL
            End Select
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.Side = nImgSide
            Next

            'TCM疵画像上下反転
            Dim nTcmImgVMode As EM_IMAGE_TOP = CType(tMod_IniFile.ReadIniInt(strSection, "TCM_IMAGE_TOP_BOTTOM_INF", TASKINI_NAME, 0), EM_IMAGE_TOP)
            Select Case nTcmImgVMode
                Case EM_IMAGE_TOP.NORMAL                                        '反転無し
                Case EM_IMAGE_TOP.REVERSE                                       '反転有り
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM疵画像上下反転異常[{0}],[{1}]", nFormID, nTcmImgVMode))
                    nTcmImgVMode = EM_IMAGE_TOP.NORMAL
            End Select
            mfrm_PopTcmImage.IsTurnTB = nTcmImgVMode                                            '上下反転設定


            'TCM疵画像左右反転
            Dim nTcmImgHMode As EM_IMAGE_SIDE = CType(tMod_IniFile.ReadIniInt(strSection, "TCM_IMAGE_SIDE_INF", TASKINI_NAME, 0), EM_IMAGE_SIDE)
            Select Case nTcmImgHMode
                Case EM_IMAGE_SIDE.NORMAL                                       '反転無し
                Case EM_IMAGE_SIDE.REVERSE                                      '反転有り
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM疵画像左右反転異常[{0}],[{1}]", nFormID, nTcmImgHMode))
                    nTcmImgHMode = EM_IMAGE_SIDE.NORMAL
            End Select
            mfrm_PopTcmImage.IsTurnLR = nTcmImgHMode                                                               '左右反転設定

            ''TCM疵画像90度回転設定
            'Dim nTcmImg90 As EM_IMAGE_90 = CType(tMod_IniFile.ReadIniInt(strSection, "TCM_IMAGE_90_INF", TASKINI_NAME, 0), EM_IMAGE_90)
            'Select Case nTcmImg90
            '    Case EM_IMAGE_90.NORMAL                                         '反転無し
            '    Case EM_IMAGE_90.LOTATE                                         '反転有り
            '    Case Else
            '        mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM疵画像左右反転異常[{0}],[{1}]", nFormID, nTcmImg90))
            '        nTcmImg90 = EM_IMAGE_90.NORMAL
            'End Select
            'mfrm_PopTcmImage.IsRotate90 = nTcmImg90                                          '90度回転設定()

            'TCM疵画像ラベルDS位置
            Dim nTcmImgSide As EM_MAP_SIDE = CType(tMod_IniFile.ReadIniInt(strSection, "TCM_IMAGE_VIEW_DIRECT", TASKINI_NAME, 0), EM_MAP_SIDE)
            Select Case nTcmImgSide
                Case EM_MAP_SIDE.NORMAL                                         '左がDS
                Case EM_MAP_SIDE.REVERSE                                        '左がWS
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM疵画像ラベルDS位置異常[{0}],[{1}]", nFormID, nTcmImgSide))
                    nTcmImgSide = EM_MAP_SIDE.NORMAL
            End Select
            mfrm_PopTcmImage.Side = nTcmImgSide                                                    '疵画像ラベルDS位置

            '疵マップ上の検査台表示
            mcls_DispMap.KensaDsp = CBool(tMod_IniFile.ReadIniInt(strSection, "MAP_KENSA_DISP", TASKINI_NAME, 0))

            '探傷画面疵表示更新モード
            Dim nMapUpdateMode As EM_DEFECT_MODE = CType(tMod_IniFile.ReadIniInt(strSection, "MAP_DISP_MODE", TASKINI_NAME, 0), EM_DEFECT_MODE)
            Select Case nMapUpdateMode
                Case EM_DEFECT_MODE.TRACKING                                    'トラッキング表示
                Case EM_DEFECT_MODE.REALTIME                                    'リアルタイム表示
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ更新モード異常[{0}],[{1}]", nFormID, nMapUpdateMode))
                    nMapUpdateMode = EM_DEFECT_MODE.TRACKING
            End Select
            mcls_DispMap.MapUpdateMode = nMapUpdateMode

            '警報ガイダンス欄表示
            m_bDspAlarm = CBool(tMod_IniFile.ReadIniInt(strSection, "APPROACH_ALARM_DISP", TASKINI_NAME, 0))

            ' 20201010 検査台位置表示変更 --->>>
            '-------------------------------------------------------------------
            ' 疵マップの検査台位置
            '-------------------------------------------------------------------
            Dim nKensaRow As Integer = tMod.ReadIniInt(strSection, "MAP_KENSA_POS", TASKINI_NAME, 1)
            If nKensaRow < 1 Then                                               ' アンダーフロー
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini MAP_KENSA_POS異常[{0}] [{1}]", nFormID, nKensaRow))
                nKensaRow = 1
            ElseIf TANSHO_MAP_DSP_ROW_MAX < nKensaRow Then                      ' オーバーフロー (仕様)
                nKensaRow = TANSHO_MAP_DSP_ROW_MAX
            End If
            mcls_DispMap.KensaPos = nKensaRow
            ' 20201010 検査台位置表示変更 <<<---


            '  ----------------------- KizuLocal.iniファイルからの読み込み -----------------------

            'マップ切替
            Dim nLongPosInf As EM_LONG_POS = CType(tMod_IniFile.ReadIniInt(strSection, "LONG_POS_INF", LOCALINI_NAME, 0), EM_LONG_POS)
            Select Case nLongPosInf
                Case EM_LONG_POS.SMALL                                          '表示間隔小
                Case EM_LONG_POS.BIG                                            '表示間隔大
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップ切替異常[{0}],[{1}]", nFormID, nLongPosInf))
                    nLongPosInf = EM_LONG_POS.SMALL
            End Select
            mcls_DispMap.Stage = nLongPosInf

            'オーバーレイ表示切替
            Dim nOverLay As Integer = tMod_IniFile.ReadIniInt(strSection, "OVERLAY_VIEW_INF", LOCALINI_NAME, 1)
            Select Case nOverLay
                Case 0                                                          '表示無し
                    Overlay = False
                Case 1                                                          '表示有り
                    Overlay = True
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("オーバーレイ切替異常[{0}],[{1}]", nFormID, nOverLay))
                    Overlay = True
            End Select

            '目盛り表示切替
            Dim nScale As Integer = tMod_IniFile.ReadIniInt(strSection, "SCALE_VIEW_INF", LOCALINI_NAME, 1)
            Select Case nScale
                Case 0                                                          '表示無し
                    Memori = False
                Case 1                                                          '表示有り
                    Memori = True
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("スケール切替異常[{0}],[{1}]", nFormID, nScale))
                    Memori = True
            End Select

            ''スクロール同期設定読込
            'Dim nScroll As Integer = tMod_IniFile.ReadIniInt(strSection, "SCROLL_VIEW_INF", LOCALINI_NAME, 1)
            'Select Case nScroll
            '    Case 0
            '        ScrollSync = False                                          'スクロール同期なし
            '    Case 1
            '        ScrollSync = True                                           'スクロール同期あり
            '    Case Else
            '        mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("スクロール切替異常[{0}],[{1}]", nFormID, nScroll))
            '        ScrollSync = True                                           'スクロール同期あり
            'End Select

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("設定値取得異常[{0}],[{1}]", nFormID, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵情報表示エリア初期化
    ''' </summary>
    ''' <param name="ucDef"></param>
    ''' <param name="clr"></param>
    ''' <param name="nImgNo"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function InitDefInf(ByVal ucDef As ucDefect, ByVal clr As Color, ByVal nImgNo As Integer) As clsDispDefect
        Dim typDefInf As clsDispDefect.TYP_DEFECT_DSP = Nothing
        Dim clsDispDefect As clsDispDefect = Nothing

        Try
            With typDefInf
                .Initialize()
                .nNo = nImgNo

                .tblLayoutPnl = ucDef.tblLayoutPnl

                .lblTitleL(0) = ucDef.lblImgLeft01
                .lblTitleL(1) = ucDef.lblImgLeft02

                .lblTitleR(0) = ucDef.lblImgRight01
                .lblTitleR(1) = ucDef.lblImgRight02

                .lblType = ucDef.lblType
                .lblGrade = ucDef.lblGrade

                .lblY = ucDef.lblY
                .lblKizuNo = ucDef.lblKizuNo
                .lblYName = ucDef.lblYName
                .lblKizuNoName = ucDef.lblKizuNoName

                .dgvInfo = ucDef.dgvDefInfo

                .picImg(0) = ucDef.picImg1
                .picImg(1) = ucDef.picImg2

                .lblImg(0) = ucDef.lblImg01
                .lblImg(1) = ucDef.lblImg02
                .lblCamNo = ucDef.lblCamNo

                .clrLine = clr
            End With

            clsDispDefect = New clsDispDefect(typDefInf, mcls_Param, mcls_Log, mcls_PointInf, PATH_DEFAULT_IMG)
            clsDispDefect.SetScaleConfig(TANSHO_SCALE_INTVAL_X, TANSHO_SCALE_NUM_X, TANSHO_SCALE_INTVAL_Y, TANSHO_SCALE_NUM_Y)
            clsDispDefect.ImageZoom = True

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("疵情報表示エリア初期化[{0}],[{1}]", m_nFormID, ex.Message))
        End Try

        Return clsDispDefect

    End Function

    ''' <summary>
    ''' ステータス表示エリア初期化
    ''' </summary>
    ''' <param name="usStat"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function InitStatusInf(ByVal usStat As ucStatus, ByVal ucCoil As ucCoil) As clsDispStatus
        Dim clsDispStatus As clsDispStatus = Nothing
        Dim typStatusDsp As clsDispStatus.TYP_STATUS_DSP = Nothing

        Try
            With typStatusDsp
                .lblGaidans = usStat.lblMsgGuaid        'ガイダンス
                .lblKadou = usStat.lblKadou             '稼動状態
                .lblKensa = usStat.lblKensa             '検査状態
                .lblKiki = usStat.lblKiki               '機器状態
            End With
            clsDispStatus = New clsDispStatus(typStatusDsp, mcls_Log)

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("ステータス表示エリア初期化異常[{0}],[{1}]", m_nFormID, ex.Message))
        End Try

        Return clsDispStatus

    End Function

    ''' <summary>
    ''' コイル情報表示エリア初期化
    ''' </summary>
    ''' <param name="ucCoil"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function InitCoilInf(ByVal ucCoil As ucCoil) As clsDispCoil
        Dim clsDispCoil As clsDispCoil = Nothing
        Dim typCoilDsp As clsDispCoil.TYP_COIL_DSP = Nothing

        Try
            With typCoilDsp
                .dgvInf1 = ucCoil.TCnt_DataGridView1
                .dgvInf2 = ucCoil.TCnt_DataGridView2
                .tblLayoutPnl = ucCoil.TableLayoutPanel1
            End With

            clsDispCoil = New clsDispCoil(typCoilDsp, mcls_Log, mcls_Param)

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("コイル情報表示エリア初期化異常[{0}],[{1}]", m_nFormID, ex.Message))
        End Try

        Return clsDispCoil

    End Function

    ''' <summary>
    ''' 更新ﾓｰﾄﾞ変更処理(自動更新/一時停止)
    ''' </summary>
    ''' <param name="mode">更新ﾓｰﾄﾞ</param>
    ''' <remarks></remarks>
    Private Sub ChangeRefreshMode(ByVal mode As EM_REFRESH_MODE)
        Try
            mcls_DispMap.RefreshMode = mode                                     'モードセット
            mcls_Log.Write(LNO.MSG, String.Format("調査[開始] Minimum[{0}] Maximum[{1}] LargeChange[{2}] GetScrollMax[{3}] Value[{4}]", vsbMap.Minimum, vsbMap.Maximum, vsbMap.LargeChange, mcls_DispMap.GetScrollMax(), vsbMap.Value))
            vsbMap.Minimum = 0
            vsbMap.Maximum = mcls_DispMap.GetScrollMax() + (vsbMap.LargeChange - 1)
            If mcls_DispMap.MapDir = EM_MAP_SCROOL.HEAD_TO_TAIL Then
                vsbMap.ValueProgramable = vsbMap.Minimum
            Else
                vsbMap.ValueProgramable = vsbMap.Maximum - vsbMap.LargeChange + 1
            End If
            mcls_Log.Write(LNO.MSG, String.Format("調査[変更] Minimum[{0}] Maximum[{1}] LargeChange[{2}] GetScrollMax[{3}] Value[{4}]", vsbMap.Minimum, vsbMap.Maximum, vsbMap.LargeChange, mcls_DispMap.GetScrollMax(), vsbMap.Value))
            If mcls_DispMap.RefreshMode = EM_REFRESH_MODE.AUTO Then
                'TCM選択疵は更新前に初期化する。自動更新復帰時に表示させるため
                mcls_DispMap.ClearTcm()
                If mfrm_PopTcmImage.Visible Then                                'TCM疵画像拡大表示中
                    mfrm_PopTcmImage.CloseFrom()                                'TCM拡大画像OFFに
                End If

                m_timMap.Start()                                                '疵マップ更新開始
                m_timMap_Tick(m_timMap, New System.EventArgs())                 '疵マップ表示
                m_timCoil.Start()                                               'コイル情報更新開始
                m_timCoil_Tick(m_timCoil, New System.EventArgs())               'コイル情報表示

                '一時停止解除で拡大OFF
                If mfrm_PopImage.Visible Then                                   '疵画像拡大表示中
                    mfrm_PopImage.CloseFrom()                                   '拡大画像OFFに
                End If

            Else
                m_timCoil.Stop()                                                'コイル情報更新終了
                m_nPauseRec = mcls_PointInf.RecDisp(mcls_DispMap.KensaNo)       '一時停止時現在レコード

            End If
            mcls_Log.Write(LNO.MSG, String.Format("調査[切替] Minimum[{0}] Maximum[{1}] LargeChange[{2}] GetScrollMax[{3}] Value[{4}]", vsbMap.Minimum, vsbMap.Maximum, vsbMap.LargeChange, mcls_DispMap.GetScrollMax(), vsbMap.Value))
            SetRefreshModeControl(mcls_DispMap.RefreshMode)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ更新モードボタン押下異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵マップ行間隔の設定
    ''' </summary>
    ''' <param name="emStage">変更後の間隔 1:表示(小) 2:表示(大)</param>
    ''' <remarks></remarks>
    Private Sub SetMapInterval(ByVal emStage As EM_LONG_POS)
        Try
            'MAP表示間隔を設定
            mcls_DispMap.Stage = emStage
            'mcls_PointInf.ReadHyPoint()

            '-------------------------------------------------------------------
            'ボタン表示変更
            '-------------------------------------------------------------------
            If EM_LONG_POS.BIG = Stage Then
                btnF03.Text = mcls_Param.MapStageBig.ToString & "m→" & mcls_Param.MapStageSmall.ToString & "m"
            Else
                btnF03.Text = mcls_Param.MapStageSmall.ToString & "m→" & mcls_Param.MapStageBig.ToString & "m"
            End If

            mcls_DispMap.DspData(Men)

            'vsbMap.Minimum = 0
            'RemoveHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged
            'vsbMap.Maximum = mcls_DispMap.GetScrollMax() + (vsbMap.LargeChange - 1)
            'AddHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged

            'If mcls_DispMap.MapDir = EM_MAP_SCROOL.HEAD_TO_TAIL Then
            '    vsbMap.ValueProgramable = 0
            'Else
            '    vsbMap.ValueProgramable = mcls_DispMap.GetScrollMax()
            '    'vsbMap_ValueChanged(vsbMap, True)
            'End If

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップ間隔設定異常 value={0}[{1}]", emStage, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵マップ更新モードに合わせてコントロール表示を設定します
    ''' </summary>
    ''' <param name="mode">更新モード</param>
    ''' <remarks></remarks>
    Private Sub SetRefreshModeControl(ByVal mode As EM_REFRESH_MODE)
        Try
            Select Case mode
                Case EM_REFRESH_MODE.AUTO                                       '自働更新の場合
                    vsbMap.Enabled = False
                    'If mcls_DispMap.MapUpdateMode = EM_DEFECT_MODE.TRACKING Then
                    '    lblKensaPoint.Visible = True
                    'End If
                    btnF01.Text = "一時停止"
                    btnF01.BackColor = Color.FromKnownColor(KnownColor.Control)
                    btnF01.ForeColor = Color.Black
                    lblMode.Text = "自動更新中"
                    lblMode.BackColor = Color.Blue
                    lblMode.ForeColor = Color.Yellow
                    UcDefect1.lblDefTitle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    UcDefect2.lblDefTitle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

                    UcDefect1.TitleColor = g_ColorNewKizu
                    UcDefect2.TitleColor = g_ColorOldKizu
                    UcDefect1.lblDefTitle.ForeColor = Color.FromArgb(0, 0, 192)
                    UcDefect2.lblDefTitle.ForeColor = Color.FromArgb(0, 0, 192)
                    UcDefect1.lblDefTitle.Tag = False
                    UcDefect2.lblDefTitle.Tag = False


                Case EM_REFRESH_MODE.PAUSE                                      '一時停止の場合
                    vsbMap.Enabled = True
                    'If mcls_DispMap.MapUpdateMode = EM_DEFECT_MODE.TRACKING Then
                    '    lblKensaPoint.Visible = False
                    'End If
                    btnF01.Text = "停止解除"
                    btnF01.BackColor = Color.Red
                    btnF01.ForeColor = Color.Yellow
                    lblMode.Text = "一時停止中"
                    lblMode.BackColor = Color.Red
                    lblMode.ForeColor = Color.White

                    UcDefect1.Title = "現在の選択疵"
                    UcDefect2.Title = "一つ前の選択疵"
                    UcDefect1.lblDefTitle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    UcDefect2.lblDefTitle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    UcDefect1.TitleColor = g_ColorStop1st
                    UcDefect2.TitleColor = g_ColorStop2nd
                    UcDefect1.lblDefTitle.ForeColor = Color.FromArgb(0, 0, 192)
                    UcDefect2.lblDefTitle.ForeColor = Color.FromArgb(0, 0, 192)
                    UcDefect1.lblDefTitle.Tag = False
                    UcDefect2.lblDefTitle.Tag = False

            End Select

            If mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD Then            '上向きにコイルが進む場合
                vsbMap.ValueProgramable = vsbMap.Maximum - vsbMap.LargeChange + 1
            Else                                                                '下向きにコイルが進む場合
                vsbMap.ValueProgramable = 0
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("更新モード毎コントロール設定異常 mode={0}[{1}]", mode, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 表示ラベル初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitLabel()
        Try
            UcDefect1.lblDefTitle.Tag = False
            UcDefect2.lblDefTitle.Tag = False
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("表示ラベル初期化[{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' 疵マップスクロール処理
    ''' </summary>
    ''' <param name="nValue">スクロール量</param>
    ''' <remarks></remarks>
    Private Sub SetScroll(ByVal nValue As Integer)
        Try

            If mcls_DispMap.RefreshMode = EM_REFRESH_MODE.AUTO Then Exit Sub '自働更新中はスクロールさせない

            Dim value As Integer = nValue
            If mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD Then value = vsbMap.Maximum - (vsbMap.LargeChange - 1) - nValue '進行方上

            Dim nMaxRowNum As Integer
            mcls_DispMap.SetMapData(Men, -value, nMaxRowNum)

            'dgvMap.Update()
            dgvMap.Refresh()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップスクロール処理異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵画像拡大表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispDefectImg()
        Try

            For Each dsp As clsDispDefect In mcls_DispDefect
                If dsp.DefectViewInf.nNo = mfrm_PopImage.DispImgNo Then
                    dsp.ShowPopup(mfrm_PopImage)
                    Exit Try
                End If
            Next

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップスクロールイベント異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵画像拡大表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispDefectTcmImg(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)
        Try
            ShowPopupTcm(pntKizu.nRec, nMen, pntKizu.nPnt, mfrm_PopTcmImage)

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップスクロールイベント異常[{0}]", ex.Message))
        End Try
    End Sub

    '***************************************************************************
    ' ファンクションキー関連
    '***************************************************************************
    ''' <summary>
    ''' ファンクション切替
    ''' </summary>
    ''' <param name="FncNo">ファンクションNo (0オリジン)</param>
    ''' <param name="enable">ファンクションボタンの有効無効状態</param>
    ''' <param name="onKey">同時に押されているキー</param>
    ''' <remarks></remarks>
    Private Sub frmLog_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton

        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' 0オリジン
            Case 0                              'F1
                Call SelectF01()
            Case 1                              'F2
                Call SelectF02()
            Case 2                              'F3
                Call SelectF03()
            Case 3                              'F4
                Call SelectF04()
            Case 4                              'F5
                Call SelectF05()
            Case 5                              'F6
                Call SelectF06()
            Case 6                              'F7
                Call SelectF07()
            Case 7                              'F8
                Call SelectF08()
			Case 8                              'F9
				'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
				'Call SelectF09()
				Call SelectF09()
				'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
			Case 9                              'F10
                Call SelectF10()
            Case 10                             'F11                
                Call SelectF11()
            Case 11
                If onKey = Keys.Shift Then                                                     '
                    'シフトキーを押している場合は、タスクを終了
                    EndNotice()
                Else
                    SelectF12()
                End If
        End Select
    End Sub

    ''' <summary>
    ''' F1:更新モードボタン[一時停止/自動更新]
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF01()
        Try
            mcls_DispMap.checkcheck = True
            SetMapInterval(mcls_DispMap.Stage)                               '長手間隔設定

            '更新モード変更
            If EM_REFRESH_MODE.AUTO = mcls_DispMap.RefreshMode Then             '自動更新
                '一時停止にする
                ChangeRefreshMode(EM_REFRESH_MODE.PAUSE)
            Else                                                                '一時停止
                '自動更新にする
                ChangeRefreshMode(EM_REFRESH_MODE.AUTO)
                If mfrm_PopImage.Visible Then                               '疵画像拡大表示中
                    mfrm_PopImage.CloseFrom()                               '拡大画像OFFに
                End If
            End If
            mcls_DispMap.checkcheck = False
            dgvMap.Refresh()
            dgvMap.Focus()
            RaiseEvent ev_ChangeRefreshMode(Me, mcls_DispMap.RefreshMode)       '更新モード変更
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("更新モードボタン押下異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' スクロール同期ボタン
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF02()
        Try
            dgvMap.Focus()
            Me.ScrollSync = Not m_bScrollSync
            RaiseEvent ev_ChangeScrollSync(Me, Me.ScrollSync)                   'スクロール同期変更
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("スクロール同期ボタン押下異常[{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' F3:マップ切替ボタン[5m⇒30m]/[30m⇒5m]
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF03()
        Try
            If EM_LONG_POS.SMALL = Stage Then
                SetMapInterval(EM_LONG_POS.BIG)
            Else
                SetMapInterval(EM_LONG_POS.SMALL)
            End If

            ' 現在マップに表示している位置を新しいMAP表示間隔でも表示できるようにする。
            Dim nValue As Integer = (vsbMap.Value)
            If mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD Then
                '進行方向が下から上の場合は、下の位置を基準にする
                nValue = vsbMap.Maximum - vsbMap.LargeChange - nValue + 1
            End If
            'MAP表示間隔の比率
            Dim nMag As Integer = CInt(mcls_Param.MapStageBig / mcls_Param.MapStageSmall)
            If EM_LONG_POS.BIG = Stage Then
                ' MAP間隔が小→大
                nValue = nValue \ nMag
            Else
                ' MAP間隔が大→小
                nValue = nValue * nMag
            End If
            If mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD Then
                nValue = mcls_DispMap.GetScrollMax() - nValue
            End If
            If mcls_DispMap.GetScrollMax() < nValue Then
                nValue = mcls_DispMap.GetScrollMax()
            ElseIf 0 > nValue Then
                nValue = 0
            End If
            vsbMap.Minimum = 0
            RemoveHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged
            vsbMap.Maximum = mcls_DispMap.GetScrollMax() + (vsbMap.LargeChange - 1)
            vsbMap.ValueProgramable = nValue
            SetScroll(nValue)
            AddHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged

            mcls_DispMap.ReDrawDeepList()

            dgvMap.Focus()
            RaiseEvent ev_ChangeMapInterval(Me, Stage)          'マップ間隔変更
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップ切替ボタン押下異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' F4:疵混入率ボタン
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF04()
        Try
            '疵混入率表示
            Dim frm As New frmMixedRate(mcls_Log)

            '疵混入率表示
            frm.ShowDialog()
            frm.Dispose()
            frm = Nothing
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵混入率ボタン押下異常[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' F5:欠陥警報解除ボタン
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF05()
        Try
            '欠陥警報出力解除要求を送信
            Dim bRet As DialogResult
            Call DspMsg("欠陥検出警報をOFFしますか？", bRet)
            If System.Windows.Forms.DialogResult.OK = bRet Then
                Call SendAlarmMail(1)
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("欠陥警報解除ボタン押下異常[{0}]", ex.Message))
        End Try

    End Sub
    ''' <summary>
    ''' 確認メッセージ表示
    ''' </summary>
    ''' <param name="strMsg">表示メッセージ</param>
    ''' <param name="bRet">戻り値</param>
    ''' <remarks></remarks>
    Public Overridable Sub DspMsg(ByVal strMsg As String, ByRef bRet As DialogResult)

        Dim frmMsg As tClassForm.frmPop_MsgBox
        frmMsg = New tClassForm.frmPop_MsgBox(strMsg, "確認", , True, True, System.Windows.Forms.DialogResult.OK, System.Windows.Forms.DialogResult.Cancel)

        'メッセージ表示
        bRet = frmMsg.ShowDialog()
        frmMsg.Dispose()
        frmMsg = Nothing

    End Sub

    ''' <summary>
    ''' F6:装置警報解除ボタン
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF06()
        Try
            '警報出力解除要求を送信
            SendAlarmMail(0)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("警報解除ボタン押下異常[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' F7:オーバーレイ表示・非表示切替ボタン
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF07()
        Try
            ' オーバーレイの表示・非表示状態を反転させる
            Overlay = Not Overlay

            dgvMap.Focus()
            RaiseEvent ev_ChangeOverlyVisible(Me, Overlay)   'オーバーレイ表示変更
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("オーバーレイ表示・非表示切替ボタン押下異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' F8:目盛り表示・非表示切替ボタン
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF08()
        Try
            ' 目盛りの表示・非表示状態を反転させる
            Memori = Not Memori
            dgvMap.Focus()
            RaiseEvent ev_ChangeMemoryVisible(Me, Memori)   'スケール表示変更
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("目盛り表示・非表示切替ボタン押下異常[{0}]", ex.Message))

        End Try

    End Sub


	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	''' <summary>
	''' F9:長周期ボタン
	''' </summary>
	''' <remarks></remarks>
	Private Sub SelectF09()
		Try
			If mfrm_LongPdList.Visible = True Then
				' 表示中は何もしない
				Exit Sub
			End If

			' 表示中レコード
			Dim nRecs() As Integer = mcls_DispMap.MapDispRec()

			' 表示対象コイルの管理NoとコイルNoを取得
			Dim lstManagementNo As New List(Of String)
			Dim lstCoilNo As New List(Of String)

			For ii As Integer = 0 To nRecs.Length() - 1
				If 0 = nRecs(ii) Then
					Exit For
				End If

				lstManagementNo.Add(mcls_PointInf.KizukenNo(nRecs(ii)).TrimEnd(CType(vbNullChar, Char)).TrimEnd)

				Dim typCoil As COINFO_BASE = Nothing
				If False = mcls_DispCoil.GetCoilInf(nRecs(ii), typCoil) Then
					Exit Try
				End If

				lstCoilNo.Add(tMod_Common.ByteToString(typCoil.base.cCoilNoIn).TrimEnd(CType(vbNullChar, Char)).TrimEnd)
			Next

			'長周期欠陥一覧


			'長周期欠陥一覧表示
			If True = mfrm_LongPdList.PreDispProc(lstManagementNo, lstCoilNo, IIf(mcls_DispMap.Side = EM_MAP_SIDE.REVERSE, True, False)) Then
				mfrm_LongPdList.Visible = True
			End If

		Catch ex As Exception
			mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("長周期欠陥一覧ボタン押下異常[{0}]", ex.Message))
		End Try

	End Sub
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	''' <summary>
	''' F10:画面印刷ボタン
	''' </summary>
	''' <remarks></remarks>
	Private Sub SelectF10()
        Try
            Call PrintOut()
            dgvMap.Focus()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("画面印刷ボタン押下異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' F11:画面保存ボタン
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF11()
        Try
            Call HardCopy(IMG_PATH, "探傷画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            dgvMap.Focus()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("画面保存ボタン押下異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' F12:オペレータ画面ボタン
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF12()
        Try
            'とりあえずオペレータ画面を終了する。
            Dim que As COMMON_QUE_S = Nothing
            que.inisialize()
            que.nLineNo = KizuMod.GetLineID()
            ' オペレータ画面終了 
            que.nEventNo = FACT_KS_END
            ''メール送信 
            Call KizuLibCLI.KizuMod.Send_Mail(SO_GAMENN, ".", que)

            System.Threading.Thread.Sleep(100)      'スリープ[100ms]

            'オペレータ画面ボタン
            Dim sCmd As String      ' 起動コマンド
            sCmd = String.Format("{0}{1}", START_FORM_POS, m_nMonitor)
            Dim newProcess As System.Diagnostics.Process                        ' RpiViewer プロセス
            Try
                newProcess = Process.Start(SO_GAMENN, sCmd)

            Catch ex As Exception
                Log(LNO.ERR, String.Format("探傷 オペレータ画面ボタン押下異常[{0}]", ex.Message))
                Exit Sub
            Finally

            End Try

            'タスク終了
            EndNotice()

            'If Not CBool(tClass_TaskCheck.StartExe("SO_GAMENN")) Then
            '    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("オペレータ画面起動異常[{0}]", "SO_GAMENN"))
            'Else
            '    'タスク終了
            '    EndNotice()
            'End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("オペレータ画面ボタン押下異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 終了通知(自分へ)
    ''' </summary>
    Private Sub EndNotice()
        Dim que As COMMON_QUE_S = Nothing
        que.nEventNo = FACT_KS_END
        que.nLineNo = KizuMod.GetLineID()
        KizuMod.Send_Mail(KizuMod.GetMyTaskName(), ".", que)
    End Sub

    ''' <summary>
    ''' 警報解除要求送信
    ''' </summary>
    ''' <param name="nVal"></param>
    ''' <remarks></remarks>
    Private Sub SendAlarmMail(ByVal nVal As Integer)
        '警報解除
        Dim que As COMMON_QUE_L = Nothing

        '警報解除通知
        que.inisialize()
        que.nEventNo = FACT_TO_DEFECT_20
        que.nLineNo = KizuMod.GetLineID()
        que.data(0) = nVal
        que.data(1) = KensaNo

        If Not LOCAL Then
            '本番
            If 0 <> KizuMod.Send_Mail(TO_DEFECT, KizuMod.Tokatu, que) Then
                KizuMod.SysLog(SYSNO_MAIL_SNED_ERR, String.Format("[{0}]", HY_TANSHO))
            End If
        Else
            'テスト
            If 0 <> KizuMod.Send_Mail(TO_DEFECT, ".", que) Then
                KizuMod.SysLog(SYSNO_MAIL_SNED_ERR, String.Format("[{0}]", HY_TANSHO))
            End If
        End If

    End Sub


    '***************************************************************************
    ' その他
    '***************************************************************************
    ''' <summary>
    ''' 疵情報取得
    ''' </summary>
    ''' <param name="nRec">レコード(1オリジン)</param>
    ''' <param name="nPnt">疵情報テーブルへのポインタ位置(1オリジン)</param>
    ''' <param name="typDefInf">疵情報</param>
    ''' <param name="clslog">ログ</param>
    ''' <returns>TRUE:成功,FALSE:失敗</returns>
    ''' <remarks></remarks>
    Public Shared Function GetDefect(ByVal nRec As Integer, ByVal nPnt As Integer, ByRef typDefInf As DSP_TYP_DEFECTINF, ByVal clslog As tClass_LogManager) As Boolean
        Dim nReadAddr As Integer = 0
        Dim nRetc As ValueType = 0
        Dim bRet As Boolean = False

        Try
            '無効レコード
            If nRec <= 0 Or nRec > TBL_HY_REC_MAX Then
                clslog.Write(LNO.ERR, String.Format("[{0}]無効レコード[{1}]", TBL_HY_DEFECTINF_NAME, nRec))
                Exit Try
            End If

            '無効ポインタ
            If nPnt <= 0 Or nPnt > MAX_MAP_DEFECT_NUM Then
                clslog.Write(LNO.ERR, String.Format("[{0}]無効ポインタ[{1}]", TBL_HY_DEFECTINF_NAME, nPnt))
                Exit Try
            End If

            '疵情報ポインタ読み込み[共有メモリ]
            nReadAddr = SIZE_DSP_MAP_DEFECTINF * (nRec - 1) + SIZE_DEFECTINF_HEADER + SIZE_DSP_TYP_DEFECTINF * (nPnt - 1)
            typDefInf = CType(ksMemReadMap(TBL_HY_DEFECTINF_NAME, nReadAddr, tMod_MemSet.SizeOf(typDefInf), GetType(DSP_TYP_DEFECTINF), nRetc), DSP_TYP_DEFECTINF)
            If Not CBool(nRetc) Then
                clslog.Write(LNO.ERR, String.Format("[{0}]疵情報ポインタ読み込み失敗[{1}]", TBL_HY_DEFECTINF_NAME, nReadAddr))
                Exit Try
            End If

            '取得成功
            bRet = True
        Catch ex As Exception
            clslog.Write(LNO.ERR, String.Format("疵情報表示異常[{0}] nRec[{1}] pnt[{2}] nReadAddr[{3}]", ex.Message, nRec, nPnt, nReadAddr))
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' 前工程疵情報取得
    ''' </summary>
    ''' <param name="nRec">レコード(1オリジン)</param>
    ''' <param name="nPnt">疵情報テーブルへのポインタ位置(1オリジン)</param>
    ''' <param name="typDefInf">疵情報</param>
    ''' <param name="clslog">ログ</param>
    ''' <returns>TRUE:成功,FALSE:失敗</returns>
    ''' <remarks></remarks>
    Public Shared Function GetMaeDefect(ByVal nRec As Integer, ByVal nMen As Integer, ByVal nPnt As Integer, ByRef typDefInf As MAE_INF, ByVal clslog As tClass_LogManager) As Boolean
        Dim nReadAddr As Integer = 0
        Dim nRetc As ValueType = 0
        Dim bRet As Boolean = False

        Try
            '無効レコード
            If nRec <= 0 Or nRec > TBL_HY_REC_MAX Then
                clslog.Write(LNO.ERR, String.Format("[{0}]無効レコード[{1}]", TBL_HY_MAE_INF_NAME, nRec))
                Exit Try
            End If

            '無効ポインタ
            If nPnt <= 0 Or nPnt > CInt(COIL_LEN_MAX / MAP_REPRES_MIN) Then
                clslog.Write(LNO.ERR, String.Format("[{0}]無効ポインタ[{1}]", TBL_HY_MAE_INF_NAME, nPnt))
                Exit Try
            End If

            '疵情報ポインタ読み込み[共有メモリ]
            nReadAddr = SIZE_REC * (nRec - 1) + SIZE_MEN * nMen + 4 + SIZE_MAE_INF * (nPnt - 1)
            typDefInf = CType(ksMemReadMap(TBL_HY_MAE_INF_NAME, nReadAddr, tMod_MemSet.SizeOf(typDefInf), GetType(MAE_INF), nRetc), MAE_INF)
            If Not CBool(nRetc) Then
                clslog.Write(LNO.ERR, String.Format("[{0}]疵情報ポインタ読み込み失敗[{1}]", TBL_HY_MAE_INF_NAME, nReadAddr))
                Exit Try
            End If

            '取得成功
            bRet = True
        Catch ex As Exception
            clslog.Write(LNO.ERR, String.Format("前工程疵情報表示異常[{0}] nRec[{1}] pnt[{2}] nReadAddr[{3}]", ex.Message, nRec, nPnt, nReadAddr))
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' TCM疵拡大画像表示
    ''' </summary>
    ''' <param name="clsPopup"></param>
    ''' <remarks></remarks>
    Public Sub ShowPopupTcm(ByVal nRec As Integer, ByVal nMen As Integer, ByVal nPnt As Integer, ByVal clsPopup As frmDefectImage2)
        Dim clsImg(1 - 1) As Image
        Dim tmpTitle(1 - 1) As String
        Dim nRetc As ValueType = 0
        Try
            Dim typDefInf As MAE_INF = Nothing
            If Not GetMaeDefect(nRec, nMen, nPnt, typDefInf, mcls_Log) Then
                Exit Sub
            End If
            For ii As Integer = 0 To clsImg.Length - 1
                clsImg(ii) = GetTcmImage(typDefInf, CType(ii + 1, EM_IMAGE_NO), tmpTitle(ii))
            Next
            '---------------------------
            '疵画像拡大表示
            '---------------------------
            With clsPopup
                .DispImgNo = 1                                      '画像No.
                .SetVisibleMemory(Memori)                           'メモリの表示有無設定
                .SetVisibleOverlay(Overlay)                         'オーバーレイの表示有無設定
                .SetColor = Color.Cyan                              'メモリ、オーバレイ色

                ''疵情報
                .SetDefectData(typDefInf.nKizuNo, typDefInf.ff, typDefInf.inf.data, clsImg, tmpTitle)
                '表示倍率
                .ZoomRate = CInt((100.0 * typDefInf.inf.data.w_res / m_nTcmImageWide) * 100.0)
                '目盛り描画詳細
                .SetMemoryConfig(TANSHO_SCALE_INTVAL_X, TANSHO_SCALE_NUM_X * 2, TANSHO_SCALE_INTVAL_Y, TANSHO_SCALE_NUM_Y * 2)
                .DspUpDate(True)                                                          '表示
            End With

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM疵拡大画像表示異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 画像取得(探傷画面)
    ''' </summary>
    ''' <param name="typDefInf">疵情報</param>
    ''' <param name="emImgNo">画像位置</param>
    ''' <param name="strTitle">疵タイトル</param>
    ''' <returns>画像イメージ</returns>
    ''' <remarks></remarks>
    Private Function GetTcmImage(ByVal typDefInf As MAE_INF, ByVal emImgNo As EM_IMAGE_NO, ByRef strTitle As String) As Image
        Dim imgData As Image = Nothing

        Try
            '初期化
            strTitle = ""
            Dim strImgPath As String = Nothing

            Select Case emImgNo
                Case EM_IMAGE_NO.S
                    strImgPath = typDefInf.inf.image_path_1
                    strTitle = ""
                Case EM_IMAGE_NO.R1
                    strImgPath = typDefInf.inf.image_path_2
                    strTitle = ""
                Case EM_IMAGE_NO.R2
                    strImgPath = typDefInf.inf.image_path_3
                    strTitle = ""
                Case EM_IMAGE_NO.R3
                    strImgPath = typDefInf.inf.image_path_4
                    strTitle = ""
                Case Else
                    strTitle = String.Empty
                    Return Nothing
            End Select

            If 0 = strImgPath.Length Then
                '画像保存パスが未指定(画像ファイルなし)
                Return Nothing
            End If

            Return tClass_BmpJpeg.BmpFileLoad(strImgPath)                   ' 画像読み込み

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM画像取得異常[{0}]", ex.Message))
        End Try

        Return imgData
    End Function

End Class
