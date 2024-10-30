'*********************************************************************************
'　疵一覧画面基底クラス
'	[Ver]
'		Ver.01    2008/07/25  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports KizuLibCLI
Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tMod

Public Class frmCglDefList_Base
    '///////////////////////////////////////////////////////////////////////////
    '定数
    '///////////////////////////////////////////////////////////////////////////
    Private Const PATH_DEFAULT_IMG As String = "HY_DEFLIST.疵無し.bmp"         'デフォルト画像のパス
    Private Const DEFAULT_GAMEN_SIZE_X As Integer = 1280
    Private Const DEFAULT_GAMEN_SIZE_Y As Integer = 1024

    Private Const DEFLIST_DISP_DEFECT_MAX As Integer = 25                      '表示最大疵数

    Public Structure TYP_DSP_INDEX
        Dim strKanriNo As String    ' 管理No
        Dim nKizuSeq As Integer     ' 疵連番
        Dim nspmode As Integer      ' 強調表示対象フラグ   (0:強調枠無し   1:強調枠表示)   パトライト出力対象疵
        'イニシャライズ
        Public Sub initialize()
            strKanriNo = ""
            nKizuSeq = 0
            nspmode = 0
        End Sub
    End Structure

    '疵画像表示位置情報構造体
    Private Structure IMAGE_DSP_POS_INF
        Dim nRealPoint As Integer   '表示している疵一覧のポインタ
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ デリゲート関数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public Shadows Event DspDefList(ByVal nDspPattern As modMain.EM_DSP_PATTERN, ByVal nOuDiv As Integer)        '探傷画面表示
    Private Delegate Sub m_deleChangeParam(ByVal nMode As Integer)  '別スレッドからのコントロールアクセス(パラメータ再読み込み)
    Private Delegate Sub m_deleDspInit()                            '別スレッドからのコントロールアクセス(画面初期化処理)
    Private Delegate Sub m_deleExit()                               '別スレッドからのコントロールアクセス(タスク終了処理)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバークラス
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass_LogManager                           'ログ管理
    Private mcls_ParamManager As clsParamManager                    'パラメータ管理
    Private mcls_ProductNoBackColor As clsProductBackColor          '生産番号背景色設定クラス
    Private mfrm_PopImage As frmDefectImage                         '疵拡大画像表示
    Private mcls_KizuInf() As clsKizuInf                            '疵情報クラス
    Private WithEvents mfrm_setting As frmSetting                   '表示設定ポップアップ画面
    '///////////////////////////////////////////////////////////////////////////
    'メンバー変数
    '///////////////////////////////////////////////////////////////////////////
    Private m_nDspMode As Integer                                   '表裏区分0:表 1:裏
    Private m_nMode As Integer = -1                                 '表示モード (-1:初期状態, 0:自動,1:一時停止)
    Private m_bDebug As Boolean = False                             'デバッグモード
    Private m_bChangeParam As Boolean                               'パラメータセット中フラグ
    Private m_nPCDiv As Integer                                     'PC区分(0:表面検査台,1:裏面検査台)
    Private m_nImagePattern As EM_DSP_PATTERN                       '画像表示パターン
    Private m_typDefInf As TBL_HY_DEFLIST = Nothing                 '疵情報一覧テーブル
    Private m_nStop_newrec As Integer                               '一時停止したときの最新レコードNO
    Private m_nImageWidth As Integer = 25                           '表示エリアの横幅[mm]
    Private m_nImageRate As Integer = 100                           '表示倍率（100：1倍）

    Private m_nMonitor As Integer                                   '画面番号

    Private m_lstKizuInf As List(Of clsKizuInf.TYP_DEFECT_DSP)      '疵情報リスト
    Private m_typDspIndex() As TYP_DSP_INDEX

    Private m_typImageDspPosInf As IMAGE_DSP_POS_INF                '最新の疵画像表示位置情報

    Private m_nPageNo As Integer                                    '一時停止ページ数

    '///////////////////////////////////////////////////////////////////////////
    '追加イベント
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 一時停止変更
    ''' </summary>
    ''' <param name="nOuDiv">面(0:表、1:裏)</param>
    ''' <param name="nStop"></param>
    Public Event StopSelectedIndexChanged(ByVal nOuDiv As Integer, ByVal nStop As Integer)

    ''' <summary>
    ''' ポップアップ変更
    ''' </summary>
    ''' <param name="nOuDiv">面(0:表、1:裏)</param>
    ''' <param name="nPopUp"></param>
    Public Event PopSelectedIndexChanged(ByVal nOuDiv As Integer, ByVal nPopUp As Integer)

    ''' <summary>
    ''' 表示モードチェンジ
    ''' </summary>
    ''' <param name="nOuDiv">面(0:表、1:裏)</param>
    ''' <param name="nPattern"></param>
    Public Event ModeClickEvent(ByVal nOuDiv As Integer, ByVal nPattern As modMain.EM_DSP_PATTERN)

    ''' <summary>
    ''' オーバレイ表示チェンジ
    ''' <param name="bVisible">変更後の表示 True:表示 False:非表示</param>
    ''' </summary>
    Public Event ev_ChangeOverlyVisible(ByVal bVisible As Boolean)

    ''' <summary>
    ''' 目盛り表示変更
    ''' </summary>
    ''' <param name="bVisible">変更後の表示 True:表示 False:非表示</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeMemoriVisible(ByVal bVisible As Boolean)

    Public Event ev_ChangeFilter(ByVal nVal As Integer)

    '///////////////////////////////////////////////////////////////////////////
    'メソッド
    '///////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()

        'この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()
        ' InitializeComponent() 呼び出しの後で初期化を追加します。

    End Sub

    ''' <summary>
    ''' オブジェクトの破棄
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmCglDefList_Base_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        SetOverlay()
        SetScale()
    End Sub

    ''' <summary>
    ''' 疵情報コントロールをセット
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub SetKizuInf(ByRef lstKizuInf As List(Of clsKizuInf.TYP_DEFECT_DSP))

        'データセット
        m_lstKizuInf = lstKizuInf

    End Sub

    ''' <summary>
    ''' 疵情報クラス生成
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CreateKizu()

        Dim ii As Integer

        ReDim mcls_KizuInf(m_lstKizuInf.Count - 1)

        For ii = 0 To m_lstKizuInf.Count - 1

            mcls_KizuInf(ii) = New clsKizuInf(m_lstKizuInf(ii), mcls_ParamManager, mcls_Log, PATH_DEFAULT_IMG)

            ''ピクチャーのイベントをセット
            AddHandler m_lstKizuInf(ii).picImg(0).DoubleClick, AddressOf picImg_DoubleClick
            AddHandler m_lstKizuInf(ii).picImg(1).DoubleClick, AddressOf picImg_DoubleClick

        Next ii

        For Each dsp As clsKizuInf In mcls_KizuInf
            dsp.ImageWidth = m_nImageWidth
            dsp.DspRate = CSng(m_nImageRate / 100)
        Next

        m_lstKizuInf.Clear()
        m_lstKizuInf = Nothing

    End Sub

    ''' <summary>
    ''' フォーム初期設定
    ''' </summary>
    ''' <param name="nDspMode">表裏区分(0:表,1:裏)</param>
    ''' <param name="nMyID">モニタ番号(1～)</param>
    ''' <param name="clsLog">ログクラス</param>
    ''' <param name="clsParam">パラメータクラス</param>
    ''' <param name="clsPnoBGC">生産番号色変えクラス</param>
    ''' <param name="nImageWidth">表示疵画像定義(画像100pixel横幅設定[mm])</param>
    ''' <param name="nImageRate">表示倍率(1倍の時100、2倍は200)</param>
    ''' <param name="nPattern">表示パターン</param>
    Public Sub DspInitForm(ByVal nDspMode As Integer, ByVal nMyID As Integer, ByRef clsLog As tClass.tClass_LogManager, ByRef clsParam As clsParamManager,
                           ByVal clsPnoBGC As clsProductBackColor, ByVal nImageWidth As Integer, ByVal nImageRate As Integer, ByVal nPattern As modMain.EM_DSP_PATTERN)
        Try

            Dim ii As Integer = 0    'ループカウンタ

            Dim monitor() As System.Windows.Forms.Screen
            monitor = System.Windows.Forms.Screen.AllScreens()
            Me.Bounds = monitor(nMyID - 1).Bounds

            Me.Width = DEFAULT_GAMEN_SIZE_X
            Me.Height = DEFAULT_GAMEN_SIZE_Y

            'メンバー変数にセット
            m_nDspMode = nDspMode
            mcls_Log = clsLog
            mcls_ParamManager = clsParam
            mcls_ProductNoBackColor = clsPnoBGC
            m_nImagePattern = nPattern
            m_nImageWidth = nImageWidth
            m_nImageRate = nImageRate


            ReDim m_typDspIndex(m_lstKizuInf.Count - 1)
            For ii = 0 To m_lstKizuInf.Count - 1
                m_typDspIndex(ii).initialize()
            Next

            '-----------------------------------------------
            '表示設定画面
            '-----------------------------------------------
            mfrm_setting = New frmSetting()

            '-----------------------------------------------
            '疵拡大画像画面
            '-----------------------------------------------
            ' タスク名(HY_TANSHO) からini名セクション決定
            Dim strSection As String = KizuMod.GetMyTaskName()
            Dim bPicDirect As Boolean = CBool(tMod.tMod_IniFile.ReadIniInt(strSection, "DEFIMAGE_DIRECT", TASKINI_NAME, 0))
            mfrm_PopImage = New frmDefectImage(clsLog, mcls_ParamManager, PATH_DEFAULT_IMG, strSection, DSP_KIRI_IMAGE_MAX, bPicDirect)
            mfrm_PopImage.Owner = Me

            '疵情報クラス登録
            Call CreateKizu()

            '初期表示設定
            Call DspInitProcess()

            If 0 = m_nDspMode Then
                mcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "表面 重欠陥一覧画面　パターン[" & m_nImagePattern & "]")
            Else
                mcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "裏面 重欠陥一覧画面  パターン[" & m_nImagePattern & "]")
            End If

            '自動更新モード
            Call ChangeMode()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("フォーム初期設定[{0}][{1}][{2}][{3}]", nDspMode, nMyID, nPattern, ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' 初期設定
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInitProcess()

        Dim ii As Integer = 0    'ループカウンタ

        '表裏区別
        If 0 = m_nDspMode Then
            lblFace.Text = "表面"
            lblFace.ForeColor = Color.Yellow
            lblFace.BackColor = Color.Blue
        Else
            lblFace.Text = "裏面"
            lblFace.ForeColor = Color.Black
            lblFace.BackColor = Color.Orange
        End If

        'デバッグ情報取得
        m_bDebug = CBool(IIf(0 = tMod.ReadIniInt(HY_DEFLIST, "DEBUG", TASKINI_NAME), False, True))

        If m_bDebug Then

            'オフライン
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            'Me.TopMost = True
            Me.TopMost = False
        Else

            'オンライン
            Me.tProp_FromFormatMode = gem_FromFormatMode.iONLINE
        End If
        Me.ControlBox = False
        Me.DoubleBuffered = True

        '定周期疵情報取得タイマー
        Dim nTimer As Integer = tMod.ReadIniInt(HY_DEFLIST, "DSP_INTERVAL", TASKINI_NAME, 2000)
        If 0 >= nTimer Then             ' 異常値
            timReal.Interval = 2000     ' デフォルト値を設定
        Else
            timReal.Interval = nTimer
        End If

        timReal.Enabled = True

        If 0 = m_nDspMode Then

            '疵画像ラベルDS位置
            Dim nImgSide As EM_MAP_SIDE = CType(tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_DIRECT_TOP", TASKINI_NAME, 0), EM_MAP_SIDE)
            Select Case nImgSide
                Case EM_MAP_SIDE.NORMAL                                         '左がDS
                Case EM_MAP_SIDE.REVERSE                                        '左がWS
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像ラベルDS位置異常[{0}],[{1}]", m_nDspMode, nImgSide))
                    nImgSide = EM_MAP_SIDE.NORMAL
            End Select
            For Each dsp As clsKizuInf In mcls_KizuInf
                dsp.Side = nImgSide
            Next

            '疵画像上下反転
            Dim nImgVMode As EM_IMAGE_TOP = CType(tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_UP_TOP", TASKINI_NAME, 0), EM_IMAGE_TOP)
            Select Case nImgVMode
                Case EM_IMAGE_TOP.NORMAL                                        '反転無し
                Case EM_IMAGE_TOP.REVERSE                                       '反転有り
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像上下反転異常[{0}][{1}]", m_nDspMode, nImgVMode))
                    nImgVMode = EM_IMAGE_TOP.NORMAL
            End Select
            For Each dsp As clsKizuInf In mcls_KizuInf
                dsp.IsTurnTB = nImgVMode
            Next

            '疵画像左右反転
            Dim nImgHMode As EM_IMAGE_SIDE = CType(tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_SIDE_TOP", TASKINI_NAME, 0), EM_IMAGE_SIDE)
            Select Case nImgHMode
                Case EM_IMAGE_SIDE.NORMAL                                       '反転無し
                Case EM_IMAGE_SIDE.REVERSE                                      '反転有り
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像左右反転異常[{0}[{1}]]", m_nDspMode, nImgHMode))
                    nImgHMode = EM_IMAGE_SIDE.NORMAL
            End Select
            For Each dsp As clsKizuInf In mcls_KizuInf
                dsp.IsTurnLR = nImgHMode
            Next

            '画面で設定したポップアップのタイマーの値
            mfrm_setting.PopUpCloseTime = tMod.ReadIniInt(HY_DEFLIST, "POP_TIMER_TOP", LOCALINI_NAME)

            '画面で設定した一時停止のタイマーの値
            mfrm_setting.StopCloseTime = tMod.ReadIniInt(HY_DEFLIST, "STOP_TIMER_TOP", LOCALINI_NAME)

            '画面で設定したオーバーレイの値
            Dim nOverLay As Integer = tMod.ReadIniInt(HY_DEFLIST, "OVERLAY_VIEW_TOP", LOCALINI_NAME)
            If 1 = nOverLay Then
                Overlay = True
            Else
                Overlay = False
            End If

            '画面で設定したスケールの値
            Dim nScale As Integer = tMod.ReadIniInt(HY_DEFLIST, "SCALE_VIEW_TOP", LOCALINI_NAME)
            If 1 = nScale Then
                Memori = True
            Else
                Memori = False
            End If
        Else

            '疵画像ラベルDS位置
            Dim nImgSide As EM_MAP_SIDE = CType(tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_DIRECT_BOT", TASKINI_NAME, 0), EM_MAP_SIDE)
            Select Case nImgSide
                Case EM_MAP_SIDE.NORMAL                                         '左がDS
                Case EM_MAP_SIDE.REVERSE                                        '左がWS
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像ラベルDS位置異常[{0}],[{1}]", m_nDspMode, nImgSide))
                    nImgSide = EM_MAP_SIDE.NORMAL
            End Select
            For Each dsp As clsKizuInf In mcls_KizuInf
                dsp.Side = nImgSide
            Next

            '疵画像上下反転
            Dim nImgVMode As EM_IMAGE_TOP = CType(tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_UP_BOT", TASKINI_NAME, 0), EM_IMAGE_TOP)
            Select Case nImgVMode
                Case EM_IMAGE_TOP.NORMAL                                        '反転無し
                Case EM_IMAGE_TOP.REVERSE                                       '反転有り
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像上下反転異常[{0}][{1}]", m_nDspMode, nImgVMode))
                    nImgVMode = EM_IMAGE_TOP.NORMAL
            End Select
            For Each dsp As clsKizuInf In mcls_KizuInf
                dsp.IsTurnTB = nImgVMode
            Next

            '疵画像左右反転
            Dim nImgHMode As EM_IMAGE_SIDE = CType(tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_SIDE_BOT", TASKINI_NAME, 0), EM_IMAGE_SIDE)
            Select Case nImgHMode
                Case EM_IMAGE_SIDE.NORMAL                                       '反転無し
                Case EM_IMAGE_SIDE.REVERSE                                      '反転有り
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像左右反転異常[{0}][{1}]", m_nDspMode, nImgHMode))
                    nImgHMode = EM_IMAGE_SIDE.NORMAL
            End Select
            For Each dsp As clsKizuInf In mcls_KizuInf
                dsp.IsTurnLR = nImgHMode
            Next

            '画面で設定したポップアップのタイマーの値
            mfrm_setting.PopUpCloseTime = tMod.ReadIniInt(HY_DEFLIST, "POP_TIMER_BOT", LOCALINI_NAME)

            '画面で設定した一時停止のタイマーの値
            mfrm_setting.StopCloseTime = tMod.ReadIniInt(HY_DEFLIST, "STOP_TIMER_BOT", LOCALINI_NAME)

            '画面で設定したオーバーレイの値
            Dim nOverLay As Integer = tMod.ReadIniInt(HY_DEFLIST, "OVERLAY_VIEW_BOT", LOCALINI_NAME)
            If 1 = nOverLay Then
                Overlay = True
            Else
                Overlay = False
            End If

            '画面で設定したスケールの値
            Dim nScale As Integer = tMod.ReadIniInt(HY_DEFLIST, "SCALE_VIEW_BOT", LOCALINI_NAME)
            If 1 = nScale Then
                Memori = True
            Else
                Memori = False
            End If
        End If

        ''PC区分
        m_nPCDiv = ReadIniInt(HY_DEFLIST, "PCDIV", TASKINI_NAME)

        '画面タイトル
        Me.Text = CStr(IIf(0 = m_nPCDiv, "表検査台 重欠陥一覧画面", "裏検査台 重欠陥一覧画面"))
        Call SetTitleName(Me)

        'ページ設定
        m_nPageNo = 1
        btnF01.Enabled = False
        btnF02.Enabled = False
    End Sub

    ''' <summary>
    ''' 初期化処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspInit()

        Dim ii As Integer

        '疵情報初期化
        For ii = 0 To UBound(mcls_KizuInf)
            mcls_KizuInf(ii).DspInit(m_bDebug)
        Next ii

        '疵画像表示位置情報初期化
        m_typImageDspPosInf.nRealPoint = -1

        m_nStop_newrec = 0
    End Sub

    ''' <summary>
    ''' オーバーレイ表示設定ON/OFF
    ''' </summary>
    ''' <value>True：表示、False：非表示</value>
    ''' <returns>True：表示、False：非表示</returns>
    ''' <remarks></remarks>
    Public Property Overlay() As Boolean
        Get
            Return mcls_KizuInf(0).OverlayVisible
        End Get
        Set(ByVal value As Boolean)

            ' mcls_DispDefect のオーバーレイ表示を設定
            For Each dsp As clsKizuInf In mcls_KizuInf
                dsp.OverlayVisible = value
            Next

            ' 拡大画像 のオーバーレイ表示を設定
            mfrm_PopImage.SetVisibleOverlay(value)

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
            Return mcls_KizuInf(0).MemoriVisible
        End Get
        Set(ByVal value As Boolean)

            ' mcls_DispDefect の目盛り表示を設定
            For Each dsp As clsKizuInf In mcls_KizuInf
                dsp.MemoriVisible = value
            Next

            ' 拡大画像 の目盛り表示を設定
            mfrm_PopImage.SetVisibleMemory(value)

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
    ''' チェックボックス 背景色変更
    ''' </summary>
    ''' <param name="chk">チェックボックス</param>
    ''' <remarks></remarks>
    Private Sub SetChkBackColor_Change(ByVal chk As CheckBox)
        If chk.Checked Then                 ' ON
            chk.BackColor = Color.Cyan
        Else                                ' OFF
            chk.BackColor = Drawing.SystemColors.Control
        End If
    End Sub

    ''' <summary>
    ''' 一時停止タイマー設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetStopTime()

        Dim strKey As String = CStr(IIf(0 = m_nDspMode, "STOP_TIMER_TOP", "STOP_TIMER_BOT"))
        modMain.WriteIniData(strKey, CStr(mfrm_setting.StopCloseTime))

    End Sub

    ''' <summary>
    ''' ポップアップタイマー設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetPopUpTime()

        Dim strKey As String = CStr(IIf(0 = m_nDspMode, "POP_TIMER_TOP", "POP_TIMER_BOT"))
        modMain.WriteIniData(strKey, CStr(mfrm_setting.PopUpCloseTime))

    End Sub

    ''' <summary>
    ''' オーバーレイ設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetOverlay()

        Dim strKey As String = CStr(IIf(0 = m_nDspMode, "OVERLAY_VIEW_TOP", "OVERLAY_VIEW_BOT"))
        Dim strValue As String = IIf(Overlay, "1", "0")
        modMain.WriteIniData(strKey, strValue)

    End Sub

    ''' <summary>
    ''' スケール設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetScale()

        Dim strKey As String = CStr(IIf(0 = m_nDspMode, "SCALE_VIEW_TOP", "SCALE_VIEW_BOT"))
        Dim strValue As String = IIf(Memori, "1", "0")
        modMain.WriteIniData(strKey, strValue)

    End Sub

    ''' <summary>
    ''' コンボボックスで指定した値を該当するタイマーにセットし、起動する
    ''' </summary>
    ''' <param name="cmb">値を設定するコンボボックス</param>
    ''' <param name="tim">値を反映させたいタイマーのオブジェクト</param>
    ''' <remarks></remarks>
    Private Sub TimerSet(ByVal cmb As ComboBox, ByVal tim As System.Windows.Forms.Timer)

        'タイマー設定
        If 0 < CType(cmb.Items(cmb.SelectedIndex), frmSetting.cls_CmbItem).time Then
            tim.Interval = CType(cmb.Items(cmb.SelectedIndex), frmSetting.cls_CmbItem).time * 1000

            If tim.Enabled = False Then
                tim.Enabled = True
            Else
                tim.Start()
            End If
        End If

    End Sub

    ''' <summary>
    ''' 疵情報取得タイマー
    ''' </summary>
    Private Sub tmrReal_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timReal.Tick

        '画面更新処理
        Call DspPointGet()
        If m_nMode = EM_REFRESH_MODE.AUTO Then
            Call DspData(False)
        End If

    End Sub

    ''' <summary>
    ''' 一時停止解除タイマー
    ''' </summary>
    Private Sub tmrStop_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timStop.Tick

        '自動モードへ
        Call ChangeMode()

    End Sub


    ''' <summary>
    ''' 一時停止解除タイマー
    ''' </summary>
    Private Sub tmrPopUp_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timPopUp.Tick

        '疵拡大画像画面非表示
        Call ClosePopUp()

    End Sub

    Private Sub ChangeSetting()
        '一時停止タイマー設定
        Call SetStopTime()
        '全パターン
        RaiseEvent StopSelectedIndexChanged(m_nDspMode, mfrm_setting.StopCloseTime)

        'ポップアップタイマー設定
        Call SetPopUpTime()
        '全パターン
        RaiseEvent PopSelectedIndexChanged(m_nDspMode, mfrm_setting.PopUpCloseTime)

    End Sub

    ''' <summary>
    ''' モード表示切替
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ChangeMode()

        If EM_REFRESH_MODE.AUTO = m_nMode Then

            ''自動更新→一時停止
            btnF05.Text = "一時停止" & vbNewLine & "解除"
            btnF05.BackColor = g_ColorControl
            btnF05.ForeColor = Color.Black

            lblMode.Text = "一時停止中"
            lblMode.BackColor = Color.Red
            lblMode.ForeColor = Color.White

            '前、次ページボタン押下設定
            m_nPageNo = 1

            '次ページの表示が可能か
            Dim dPageMax As Decimal = CDec(DEFLIST_DISP_DEFECT_MAX / mcls_KizuInf.Length)
            Dim bNextPageFlg As Boolean = NextPageCheck(m_nPageNo + 1, CInt(dPageMax)) '現在のページから２ページ先以降に疵情報があるか
            If HY_DEFLIST_REC_MAX > mcls_KizuInf.Length AndAlso bNextPageFlg Then
                btnF01.Enabled = False
                btnF02.Enabled = True
            Else
                btnF01.Enabled = False
                btnF02.Enabled = False
            End If

            '一時停止解除タイマー設定
            Call TimerSet(mfrm_setting.cmbStop, timStop)
            '一時停止へ
            m_nMode = EM_REFRESH_MODE.PAUSE
            '定周期疵情報取得タイマー停止
            timReal.Enabled = False
            '表示する疵情報のポインタを取得する
            Call DspPointGet()
            ''画面更新処理
            Call DspData()
            'ブリンクタイマー開始
            timMode.Enabled = True
            timMode.Start()

        Else

            '一時停止→自動更新
            timMode.Stop()
            btnF05.Text = "一時停止"
            btnF05.BackColor = g_ColorControl
            btnF05.ForeColor = Color.Black

            lblMode.Text = "自動更新中"
            lblMode.BackColor = Color.Blue
            lblMode.ForeColor = Color.Yellow

            '前、次ページボタン押下設定
            m_nPageNo = 1
            btnF01.Enabled = False
            btnF02.Enabled = False

            '疵情報初期化
            DspInit()

            '一時停止解除タイマー終了
            timStop.Enabled = False
            '自動更新へ
            m_nMode = EM_REFRESH_MODE.AUTO
            '表示する疵情報のポインタを取得する
            Call DspPointGet()
            '画面更新処理
            Call DspData()
            '定周期疵情報取得タイマー開始
            timReal.Enabled = True
            'ブリンクタイマー停止
            timMode.Enabled = False

        End If
    End Sub


    ''' <summary>
    ''' 疵拡大画像画面非表示
    ''' </summary>
    Private Sub ClosePopUp()
        If mfrm_PopImage.Visible Then                               '疵画像拡大表示中
            mfrm_PopImage.CloseFrom()                               '拡大画像OFFに
        End If
    End Sub

    ''' <summary>
    ''' 表示する疵情報のポインタを取得する
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspPointGet()

        Try
            '疵情報一覧テーブル読み込み。
            m_typDefInf = CType(KizuLibCLI.KizuMod.ksMemRead(TBL_HY_DEFLIST_NAME, GetType(TBL_HY_DEFLIST)), TBL_HY_DEFLIST)

            '生産番号背景色紐付け
            mcls_ProductNoBackColor.ReadPnoBGinit()

            '自動更新中はレコードNOを保持する
            If EM_REFRESH_MODE.PAUSE <> m_nMode Then
                m_nStop_newrec = m_typDefInf.rec.head.nPos(m_nDspMode)
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵種ポインタ取得エラー [{0}]", ex.Message))
        End Try

    End Sub


    ''' <summary>
    ''' 画面更新処理
    ''' </summary>
    ''' <param name="bUpdateFlg">強制再描画フラグ</param>
    ''' <remarks></remarks>
    Private Sub DspData(Optional ByVal bUpdateFlg As Boolean = True)

        Dim ii As Integer                           'カウンタ
        Dim nRecNo As Integer                       'レコード位置
        Dim nIndex As Integer = 0                   '疵画像表示インデックス
        Dim nPnt As Integer = 0                     '表示開始ポインタ
        Dim nRecNoChk As Integer = 0                '最新レコードセット
        Dim nIndexChk As Integer = 0                '画像表示最新インデックスセット
        Dim nDsipCnt As Integer = 0                 '画面表示疵情報数

        Try
            'レコードがなければ終了
            If 0 = m_typDefInf.rec.head.nPos(m_nDspMode) Then

                'データ無しなら初期化
                For ii = 0 To mcls_KizuInf.Length - 1
                    m_typDspIndex(ii).initialize()
                    mcls_KizuInf(ii).DspInit(m_bDebug)
                    mcls_KizuInf(ii).ChangePnlImage(g_ColorControl)
                Next ii

                '疵画像表示位置情報初期化
                m_typImageDspPosInf.nRealPoint = -1

                Exit Sub
            End If

            '一時停止中か
            If EM_REFRESH_MODE.PAUSE <> m_nMode Then
                '自動更新中はポインタを更新し、疵一覧情報テーブルの最新レコードでデータ格納レコードを算出する
                nRecNo = (m_typDefInf.rec.head.nPos(m_nDspMode) - 1)
            Else
                ''一時停止中は一時停止した時の最新レコードでデータ格納レコードを算出する
                nRecNo = m_nStop_newrec - 1
            End If

            If nRecNo = -1 Then
                nRecNo = HY_DEFLIST_REC_MAX - 1
            End If

            ''最新レコード更新あり or 任意呼出時
            If m_typImageDspPosInf.nRealPoint <> nRecNo OrElse bUpdateFlg = True Then

                '表示データ数を取得
                nDsipCnt = mcls_KizuInf.Length * (m_nPageNo - 1)

                '開始ポインタを設定
                nPnt = nRecNo - nDsipCnt
                If nPnt < 0 Then nPnt = HY_DEFLIST_REC_MAX - Math.Abs(nPnt)

                '画面に表示する疵情報分ループ（画面左端疵情報欄から順に最新の疵情報を格納していく）
                For nIndex = 0 To mcls_KizuInf.Length - 1

                    Dim typData As DSP_LIST_DEFECT = m_typDefInf.rec.data(m_nDspMode).DfData(nPnt)

                    If 0 < typData.typ.data.nKizuNo And nDsipCnt < HY_DEFLIST_REC_MAX Then

                        '疵情報表示
                        Dim col As Color = mcls_ProductNoBackColor.SetPNoBackColor(typData.cKanriNo)
                        mcls_KizuInf(nIndex).DspDefect(m_nDspMode, nPnt, typData, col, m_nImagePattern)

                        '表示情報セット
                        m_typDspIndex(nIndex).strKanriNo = typData.cKanriNo
                        m_typDspIndex(nIndex).nKizuSeq = typData.typ.data.nKizuNo
                        m_typDspIndex(nIndex).nspmode = typData.typ.data.nModeDeflist
                        '強調枠表示
                        If 1 = m_typDspIndex(nIndex).nspmode Then
                            mcls_KizuInf(nIndex).ChangePnlImage(g_ColLstSp)
                        Else
                            mcls_KizuInf(nIndex).ChangePnlImage(g_ColorControl)
                        End If

                        '最新ポインタになったら、表示疵画像情報更新
                        If nPnt = nRecNo Then
                            m_typImageDspPosInf.nRealPoint = nRecNo
                            If 1 = m_typDspIndex(nIndex).nspmode Then
                                mcls_KizuInf(nIndex).ChangePnlImage(g_ColLstNewSp)
                            Else
                                mcls_KizuInf(nIndex).ChangePnlImage(g_ColLstNewKizu)
                            End If

                        End If

                    Else
                        'データなし(初期化)
                        mcls_KizuInf(nIndex).DspInit(m_bDebug)
                        mcls_KizuInf(nIndex).ChangePnlImage(g_ColorControl)
                        m_typDspIndex(nIndex).initialize()
                    End If

                    '新しい疵情報から画面に設定する為、1つ前の疵情報を取得
                    nPnt -= 1
                    If nPnt < 0 Then nPnt = HY_DEFLIST_REC_MAX - 1

                    '表示データ数加算
                    nDsipCnt += 1
                Next nIndex
            End If

            '疵画像表示に変化が無いか比較
            nRecNoChk = m_typImageDspPosInf.nRealPoint   '最新レコードセット（0オリジン）
            If 0 <= nRecNoChk AndAlso EM_REFRESH_MODE.AUTO = m_nMode Then
                For nIndex = 0 To mcls_KizuInf.Length - 1
                    'データ有り
                    Dim typData As DSP_LIST_DEFECT = m_typDefInf.rec.data(m_nDspMode).DfData(nRecNoChk)
                    If 0 < typData.typ.data.nKizuNo Then

                        '表示している疵とテーブルの該当順番の疵が同じか？
                        If m_typDspIndex(nIndex).strKanriNo = typData.cKanriNo And
                               m_typDspIndex(nIndex).nKizuSeq = typData.typ.data.nKizuNo And
                               m_typDspIndex(nIndex).nspmode = typData.typ.data.nModeDeflist Then
                        Else

                            '疵情報表示
                            Dim col As Color = mcls_ProductNoBackColor.SetPNoBackColor(typData.cKanriNo)
                            mcls_KizuInf(nIndex).DspDefect(m_nDspMode, nRecNoChk, typData, col, m_nImagePattern)

                            '表示情報セット
                            m_typDspIndex(nIndex).strKanriNo = typData.cKanriNo
                            m_typDspIndex(nIndex).nKizuSeq = typData.typ.data.nKizuNo
                            m_typDspIndex(nIndex).nspmode = typData.typ.data.nModeDeflist

                            If m_typImageDspPosInf.nRealPoint = nRecNoChk Then
                                '強調枠表示
                                If 1 = typData.typ.data.nModeDeflist Then
                                    mcls_KizuInf(nIndex).ChangePnlImage(g_ColLstNewSp)
                                Else
                                    mcls_KizuInf(nIndex).ChangePnlImage(g_ColLstNewKizu)
                                End If
                            Else
                                '強調枠表示
                                If 1 = typData.typ.data.nModeDeflist Then
                                    mcls_KizuInf(nIndex).ChangePnlImage(g_ColLstSp)
                                Else
                                    mcls_KizuInf(nIndex).ChangePnlImage(g_ColorControl)
                                End If
                            End If
                        End If
                    Else
                        'データなし(初期化)
                        mcls_KizuInf(nIndex).DspInit()
                        mcls_KizuInf(nIndex).ChangePnlImage(g_ColorControl)
                        m_typDspIndex(nIndex).initialize()
                    End If

                    nRecNoChk -= 1
                    If 0 > nRecNoChk Then nRecNoChk = HY_DEFLIST_REC_MAX - 1

                Next nIndex

            End If

            '検査台位置判定処理
            Dim strBaseKanriNo As String = ""
            Dim nBaseLen As Integer = 0
            '検査台位置情報取得
            If GetKensaPosCoil(strBaseKanriNo, nBaseLen) Then
                For ii = 0 To mcls_KizuInf.Length - 1

                    '疵NOが格納されてなかったら表示しない
                    If 0 = m_typDspIndex(ii).nKizuSeq Then
                        mcls_KizuInf(ii).SetKensaLabel("", g_ColorControl)
                    Else
                        mcls_KizuInf(ii).SetKensaLabel("検査台前", g_ColLstKensadaiMae)

                        '疵検管理NOの比較
                        If m_typDspIndex(ii).strKanriNo = strBaseKanriNo Then
                            '疵情報の溶接からの位置とコイル先頭から検査台までの位置を判定
                            If mcls_KizuInf(ii).GetDefectKeyInf.typ.data.y / 1000 < nBaseLen Then
                                mcls_KizuInf(ii).SetKensaLabel("検査台後", g_ColorControl)
                            End If
                        ElseIf m_typDspIndex(ii).strKanriNo < strBaseKanriNo.Trim Then
                            mcls_KizuInf(ii).SetKensaLabel("検査台後", g_ColorControl)
                        End If

                    End If
                Next ii
            Else
                For ii = 0 To mcls_KizuInf.Length - 1

                    '検査台まで到達していない。または初期化でデータがない
                    mcls_KizuInf(ii).SetKensaLabel("", g_ColorControl)

                Next ii
            End If

            For ii = 0 To mcls_KizuInf.Length - 1

                'レコードが格納されてなかったら表示しない
                If 0 < m_typDspIndex(ii).nKizuSeq Then
                    If mcls_KizuInf(ii).IsDspDefault(EM_IMAGE_POS.IMG1) Or mcls_KizuInf(ii).IsDspDefault(EM_IMAGE_POS.IMG2) Then
                        mcls_KizuInf(ii).DspDefectRetry(m_nImagePattern)
                    End If
                End If
            Next ii
            System.GC.Collect()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("画面更新エラー [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' 検査台位置情報取得
    ''' </summary>
    ''' <param name="strKanriNo">管理No</param>
    ''' <param name="nLen">検査台位置[m]</param>
    ''' <returns></returns>
    Private Function GetKensaPosCoil(ByRef strKanriNo As String, ByRef nLen As Integer) As Boolean
        Dim bRet As Boolean = False
        Dim nKensa As Integer
        ' 初期化
        strKanriNo = ""
        nLen = 0

        '検査台位置を取得
        If 0 = m_nPCDiv Then
            nKensa = EM_DIV_DISP.DIV_LEN_POS_DSP_TOP
        Else
            nKensa = EM_DIV_DISP.DIV_LEN_POS_DSP_BOT
        End If

        Try
            'ポインタ情報読み込み
            Dim typHyPoint As TBL_HY_POINT = CType(KizuMod.ksMemRead(TBL_HY_POINT_NAME, GetType(TBL_HY_POINT)), TBL_HY_POINT)

            '表示基点レコード取得
            Dim nBaseRec As Integer = typHyPoint.nRecDisp(nKensa)
            '表示基点にコイルがなければ処理終了
            If 0 = nBaseRec Then Exit Try
            '表示基点コイル通板距離取得
            Dim nBaseLen As Integer = typHyPoint.pnt(nBaseRec - 1).typ.nLenDisp(nKensa).nLen
            '検査台位置通板距離
            Dim nLenWk As Integer = nBaseLen - mcls_ParamManager.PosKensa(m_nPCDiv)
            '「通板距離」と「表示基点～検査台位置」を比較
            If 0 < nLenWk Then
                '表示基点通過コイルが検査台も通過中
                nLen = nBaseLen - mcls_ParamManager.PosKensa(m_nPCDiv)
                strKanriNo = tMod_Common.ByteToString(typHyPoint.pnt(nBaseRec - 1).cKizukenNo).TrimEnd(CType(vbNullChar, Char))
                bRet = True
            Else
                '前回表示レコード取得
                Dim nPrevRec As Integer = GetPointPrevRecNo(nBaseRec)

                '表示対象レコード収集
                For ii As Integer = 0 To TBL_HY_REC_MAX - 1     'レコード数分
                    If ii > DSP_COIL_MAX Then                   '表示数分
                        Exit For
                    End If
                    If 0 >= nPrevRec Then                       '前レコード無し
                        Exit For
                    End If

                    '表示可能または表示予定
                    If typHyPoint.pnt(nPrevRec - 1).emDispStatus = EM_DIV_DSP_STATUS.DIV_DSP_OK _
                    Or typHyPoint.pnt(nPrevRec - 1).emDispStatus = EM_DIV_DSP_STATUS.DIV_DSP_STANDBY Then
                        nLenWk = nLenWk + typHyPoint.pnt(nPrevRec - 1).typ.nLenDisp(nKensa).nLen
                        If 0 < nLenWk Then
                            'このコイルが検査台を通過中
                            nLen = nLenWk
                            strKanriNo = tMod_Common.ByteToString(typHyPoint.pnt(nPrevRec - 1).cKizukenNo).TrimEnd(CType(vbNullChar, Char))
                            bRet = True
                            Exit For
                        End If
                    End If
                    '前回表示レコード取得
                    nPrevRec = GetPointPrevRecNo(nPrevRec)
                Next
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("検査台位置情報取得エラー [{0}]", ex.Message))
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 前レコード取得
    ''' </summary>
    ''' <param name="nRecNo">レコード</param>
    ''' <returns>前レコード</returns>
    ''' <remarks></remarks>
    Private Function GetPointPrevRecNo(ByVal nRecNo As Integer) As Integer
        nRecNo -= 1

        If nRecNo < 1 Then                                  '最小値アンダー
            nRecNo = TBL_HY_REC_MAX                         '末尾
        End If

        If TBL_HY_REC_MAX < nRecNo Then                     '不正値
            nRecNo = TBL_HY_REC_MAX                         '末尾
        End If

        Return nRecNo
    End Function

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
    Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton

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
                Call SelectF09()
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
    ''' F1：
    ''' </summary>
    Private Sub SelectF01()
        Try
            m_nPageNo -= 1

            '前ページの設定
            If 1 >= m_nPageNo Then     '最初のページか
                btnF01.Enabled = False
            Else
                btnF01.Enabled = True
            End If

            '次ページボタン設定
            btnF02.Enabled = True

            DspData()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F1処理 エラー [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F2：
    ''' </summary>
    Private Sub SelectF02()
        Try
            m_nPageNo += 1

            '前ページボタン設定
            btnF01.Enabled = True

            '次ページボタン設定
            Dim dPageMax As Decimal = CDec(DEFLIST_DISP_DEFECT_MAX / mcls_KizuInf.Length)
            Dim bNextPageFlg As Boolean = NextPageCheck(m_nPageNo + 1, CInt(dPageMax)) '現在のページから２ページ先以降に疵情報があるか
            If dPageMax <= m_nPageNo Or bNextPageFlg = False Then
                btnF02.Enabled = False
            Else
                btnF02.Enabled = True
            End If

            DspData()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F2処理 エラー [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F3：
    ''' </summary>
    Private Sub SelectF03()
        Try

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F3処理 エラー [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F4：
    ''' </summary>
    Private Sub SelectF04()
        Try

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F4処理 エラー [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F5：
    ''' </summary>
    Private Sub SelectF05()
        Try
            ''一時停止イベント発生
            RaiseEvent ModeClickEvent(m_nDspMode, m_nImagePattern)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F5処理 エラー [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F6：
    ''' </summary>
    Private Sub SelectF06()
        Try

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F6処理 エラー [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F7：オーバーレイ
    ''' </summary>
    Private Sub SelectF07()
        Try
            'オーバーレイ表示切替えイベント発生
            RaiseEvent ev_ChangeOverlyVisible(Not Overlay)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F7処理 エラー [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F8：目盛り
    ''' </summary>
    Private Sub SelectF08()
        Try
            '目盛り表示切替えイベント発生
            RaiseEvent ev_ChangeMemoriVisible(Not Memori)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F8処理 エラー [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F9：
    ''' </summary>
    Private Sub SelectF09()
        Try
            If mfrm_setting.ShowDialog(Me) = Windows.Forms.DialogResult.OK Then
                ChangeSetting()
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F9処理 エラー [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F10：
    ''' </summary>
    Private Sub SelectF10()
        Try

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F10処理 エラー [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F11：
    ''' </summary>
    Private Sub SelectF11()
        Try

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F11処理 エラー [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F12：
    ''' </summary>
    Private Sub SelectF12()
        Try

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F12処理 エラー [{0}]", ex.Message))
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
    ''' 一時停止中ラベルのブリンク処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub timMode_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timMode.Tick

        'ラベル表示色変更
        If Color.Red = lblMode.BackColor Then
            lblMode.BackColor = g_ColorControl
            lblMode.ForeColor = Color.Black
        Else
            lblMode.BackColor = Color.Red
            lblMode.ForeColor = Color.White
        End If

    End Sub

    ''' <summary>
    ''' 一時停止タイマー変更
    ''' </summary>
    ''' <param name="nVal">値</param>
    ''' <remarks></remarks>
    Public Sub ChangeStopTimer(ByVal nVal As Integer)

        mfrm_setting.StopCloseTime = nVal

    End Sub

    ''' <summary>
    ''' ポップアップ画像タイマー変更
    ''' </summary>
    ''' <param name="nVal">値</param>
    ''' <remarks></remarks>
    Public Sub ChangePopTimer(ByVal nVal As Integer)

        mfrm_setting.PopUpCloseTime = nVal

    End Sub

    ''' <summary>
    ''' 疵画像ダブルクリック
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub picImg_DoubleClick(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Try

            'If EM_REFRESH_MODE.PAUSE <> mcls_DispMap.RefreshMode Then
            '    '一時停止中ではない
            '    Exit Try
            'End If

            For Each dsp As clsKizuInf In mcls_KizuInf
                If Not dsp.ImageZoom Then Continue For
                For Each picbox As PictureBox In dsp.DefectViewInf.picImg
                    If sender Is picbox Then
                        ' ダブルクリックしたPictureBoxと一致したら、疵画像拡大表示
                        dsp.ShowPopup(mfrm_PopImage)
                        'ポップアップ表示解除タイマー終了
                        timPopUp.Enabled = False
                        'ポップアップ表示解除タイマー設定
                        Call TimerSet(mfrm_setting.cmbPopUp, timPopUp)

                        Exit Try
                    End If
                Next
            Next

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像ダブルクリック異常[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' 引数で指定されたページからページまで疵情報が存在するかどうか判定する
    ''' </summary>
    ''' <param name="nStartPage">チェック開始ページ</param>
    ''' <param name="nEndPage">チェック終了ページ</param>
    ''' <remarks></remarks>
    Private Function NextPageCheck(ByVal nStartPage As Integer, ByVal nEndPage As Integer) As Boolean

        '最終ページ - 1までループ
        For ii As Integer = nStartPage To nEndPage
            '次ページ情報を取得
            Dim nDsipCnt As Integer = mcls_KizuInf.Length * (ii - 1)
            Dim nNextRec As Integer = (m_nStop_newrec - 1) - nDsipCnt
            If nNextRec < 0 Then nNextRec = HY_DEFLIST_REC_MAX - Math.Abs(nNextRec)

            '次ページを表示した場合、表示可能な疵情報があるか判定
            For jj As Integer = 0 To mcls_KizuInf.Length - 1

                '表示可能な疵情報が存在
                If 0 < m_typDefInf.rec.data(m_nDspMode).DfData(nNextRec).typ.data.nKizuNo Then
                    Return True
                End If

                nNextRec = nNextRec - 1
                If nNextRec < 0 Then nNextRec = HY_DEFLIST_REC_MAX - 1

            Next jj

        Next ii

        Return False

    End Function

    ''' <summary>
    ''' 画面更新
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspUpdate()
        DspData()
    End Sub

End Class
