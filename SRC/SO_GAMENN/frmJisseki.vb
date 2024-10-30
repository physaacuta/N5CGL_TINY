'*********************************************************************************
' 検査実績画面
'	[Ver]
'		Ver.01    2012/09/01  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tClass.tClass_LogManager

Public Class frmJisseki
    '---------------------------------------------------------------------------
    ' 列挙体定義
    '---------------------------------------------------------------------------
    Public Enum EM_DIV_FUNC_TYPE
        DIV_SELECT_0 = 0            'ファンクションボタンTYPE_0（通常）
        DIV_SELECT_1 = 1            'ファンクションボタンTYPE_1（サブメニュー）
    End Enum


    '---------------------------------------------------------------------------
    ' 構造体定義
    '---------------------------------------------------------------------------
    'ファンクションボタン切り替え前情報保存用構造体
    Private Structure FUNC_INF
        Dim strText As String       'ファンクションボタン名称
        Dim bEnabled As Boolean     'ファンクションボタン有効／無効
        Public Sub initialize()
            strText = ""
            bEnabled = False
        End Sub
    End Structure

    Private Structure MENU_INF
        Dim func() As FUNC_INF
        Public Sub initialize(ByVal n As Integer)
            ReDim func(n)
            For ii As Integer = 0 To func.Length - 1
                func(ii).initialize()
            Next
        End Sub
    End Structure


    '画像形式
    Public Enum EM_DSP_IMAGE_FORMAT
        nJpeg = 0       'JPEG
        nBmp            'BMP
    End Enum

    '---------------------------------------------------------------------------
    ' 定数定義
    '---------------------------------------------------------------------------
    Private Const RSC_IMG_DEFAULT As String = "SO_GAMENN.疵無し.bmp"       ' デフォルト画像のリースパス
    Private Const KEY_OVERLAY_VISIBLE As String = "OVERLAY_VISIBLE"        ' KizuLocal.iniキー名 オーバーレイ＆スケール表示有無
    'Private Const KEY_SCALE_VISIBLE As String = "SCALE_VISIBLE"            ' KizuLocal.iniキー名 スケール表示有無

    '---------------------------------------------------------------------------
    ' メンバ変数
    '---------------------------------------------------------------------------
    Private m_nMapInterval As Integer = -1                                  ' マップ間隔(-1:初期状態,0:最小間隔,1:最大間隔)
    Private mcls_Param As clsParamManager                                   ' パラメータ管理
    Private mcls_PreParam As clsPreParamManager                             ' 前工程パラメータ管理クラス

    Private mcls_DispMap As clsDispMap                                      ' 疵マップ表示クラス
    Private mcls_DataMap As clsDataMap                                      ' 疵マップデータクラス
    Private mcls_DispDefect() As clsDispDefect                              ' 疵情報表示クラス
    Private mcls_DispCoil As clsDispCoil                                    ' コイル情報表示クラス

    ' サブ画面
    Private mfrm_Search As frmSearch                                        ' 実績検索画面
    Private mfrm_PopImage As frmDefectImage                                 ' 疵拡大画像表示
    Private mfrm_PopTcmImage As frmDefectImage2                             '疵拡大画像表示(TCM用)
    '    Private m_typMap As clsDispMap.TYP_MAP_INF                              ' 疵マップ
    Private m_nMenuMode As Integer                                          ' メニュー画面状態
    Private m_typMenu() As MENU_INF

    'Private m_nSelectMen As Integer                                         ' 直近の疵選択面(0:表、1:裏)

    Private m_emImageTop(NUM_MEN - 1) As EM_IMAGE_TOP                       ' 画像の上下反転
    Private m_emImageSide(NUM_MEN - 1) As EM_IMAGE_SIDE                     ' 画像の左右反転

    Private m_nTcmImageWide As Integer = 25                                 'TCM疵画像表示ゾーンの横幅

#Region "プロパティ"
    '---------------------------------------------------------------------------
    ' プロパティ
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' 疵マップのスクロール方向
    ''' </summary>
    ''' <returns>False:下 True:上</returns>
    ''' <remarks></remarks>
    Public Property ScrollDrection() As EM_MAP_SCROOL
        Get
            ' ※面単位の方向を取得するなら、mcls_DispMapではなく、専用で変数を持つべき
            ' 最終的には修正する
            Return mcls_DispMap.MapDir
        End Get
        Set(ByVal value As EM_MAP_SCROOL)
            mcls_DispMap.MapDir = value
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
    ''' 表示面取得
    ''' </summary>
    ''' <returns>0:表 1:裏</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property Men() As Integer
        Get
            Return mcls_DispMap.Men
        End Get
    End Property

    ''' <summary>
    ''' オーバーレイ＆目盛り表示設定ON/OFF
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
                dsp.MemoriVisible = value
            Next

            ' 拡大画像 のオーバーレイ＆目盛り表示を設定
            mfrm_PopImage.SetVisibleOverlay(value)
            mfrm_PopImage.SetVisibleMemory(value)
            ' 拡大画像(TCM) のオーバーレイ表示を設定
            mfrm_PopTcmImage.SetVisibleOverlay(value)
            mfrm_PopTcmImage.SetVisibleMemory(value)

            ' その他変更する対象があれば、ここに記述する

            ' 切替ボタンの色変更処理
            If EM_DIV_FUNC_TYPE.DIV_SELECT_1 = m_nMenuMode Then
                If value Then
                    ' 表示時のボタン色設定
                    btnF06.BackColor = Color.Cyan
                Else
                    ' 非表示時のボタン色設定
                    btnF06.BackColor = Color.FromKnownColor(KnownColor.Control)
                End If
            End If

        End Set
    End Property

    '''' <summary>
    '''' 目盛り表示設定ON/OFF
    '''' </summary>
    '''' <value>True：表示、False：非表示</value>
    '''' <returns>True：表示、False：非表示</returns>
    '''' <remarks></remarks>
    'Public Property Memori() As Boolean
    '    Get
    '        Return mcls_DispDefect(0).MemoriVisible
    '    End Get
    '    Set(ByVal value As Boolean)

    '        ' mcls_DispDefect の目盛り表示を設定
    '        For Each dsp As clsDispDefect In mcls_DispDefect
    '            dsp.MemoriVisible = value
    '        Next

    '        ' 拡大画像 の目盛り表示を設定
    '        mfrm_PopImage.SetVisibleMemory(value)

    '        ' その他変更する対象があれば、ここに記述する

    '        ' 切替ボタンの色変更処理
    '        If EM_DIV_FUNC_TYPE.DIV_SELECT_0 = m_nMenuMode Then
    '            If value Then
    '                ' 表示時のボタン色設定
    '                btnF07.BackColor = Color.Cyan
    '            Else
    '                ' 非表示時のボタン色設定
    '                btnF07.BackColor = Color.FromKnownColor(KnownColor.Control)
    '            End If
    '        End If

    '    End Set
    'End Property


#End Region

    '///////////////////////////////////////////////////////////////////////////
    'メソッド
    '///////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()
        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()
        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        Dim s As Size
        s.Width = WIN_WIDTH_MAX
        s.Height = WIN_HEIGHT_MAX
        Me.MaximumSize = s
        s.Width = WIN_WIDTH_MIN
        s.Height = WIN_HEIGHT_MIN
        Me.MinimumSize = s

        Dim sw As New Stopwatch
        sw.Reset()
        sw.Start()

        Try
            '-------------------------------------------------------------------
            ' メンバ初期化
            '-------------------------------------------------------------------
            mcls_Param = New clsParamManager(gcls_Log)                                  ' パラメータ管理
            mcls_PreParam = New clsPreParamManager(gcls_Log)                            ' 前工程パラメータ管理
            mfrm_Search = New frmSearch(mcls_Param, mcls_PreParam, gcls_Log)                           ' 検索画面

            '-----------------------------------------------
            'コイル情報
            '-----------------------------------------------
            mcls_DispCoil = InitCoilInf(UcCoil1)

            '-----------------------------------------------
            '疵拡大画像画面
            '-----------------------------------------------
            Dim bPicDirect As Boolean = CBool(tMod.ReadIniInt(SO_GAMENN, "DEFIMAGE_DIRECT", TASKINI_NAME, 0))
            mfrm_PopImage = New frmDefectImage(gcls_Log, mcls_Param, RSC_IMG_DEFAULT, SO_GAMENN, JISSEKI_DSP_KIRI_IMAGE_MAX, bPicDirect)
            mfrm_PopImage.Owner = Me

            '-----------------------------------------------
            'TCM疵画像画面
            '-----------------------------------------------
            ' タスク名(HY_TANSHO) からini名セクション決定
            mfrm_PopTcmImage = New frmDefectImage2(gcls_Log, mcls_PreParam, RSC_IMG_DEFAULT, SO_GAMENN, 1, bPicDirect)
            mfrm_PopTcmImage.Owner = Me

            '-----------------------------------------------
            '疵表示
            '-----------------------------------------------
            ReDim mcls_DispDefect(DSP_DEFECT_MAX - 1)
            mcls_DispDefect(0) = InitDefInf(UcDefect1, g_ColorNewKizu, 1)    ' 最新疵
            mcls_DispDefect(1) = InitDefInf(UcDefect2, g_ColorOldKizu, 2)    ' 1個前の疵

            '-----------------------------------------------
            'マップ表示
            '-----------------------------------------------
            Dim typMapInf As clsDispMap.TYP_MAP_INF = Nothing
            With typMapInf
                .dgvMap = dgvMap                                            '表面疵マップ
                .dgvDeep = dgvDeep                                          '疵深さリスト
                .dgvTcm = dgvOth1                                           'TCMリスト
                .vsbMap = vsbMap                                            'スクロールバー
            End With
            mcls_DispMap = New clsDispMap(typMapInf, mcls_Param, mcls_PreParam, gcls_Log)
            mcls_DispMap.Men = 0

            vsbMap.Enabled = False
            vsbMap.Value = 0

            ''疵マップのライン色設定
            'lblSharTitle.ForeColor = g_ColorShar                            'シャーカット線
            'lblYousetuTitle.ForeColor = g_ColorYousetu                      '溶接線
            '表面から表示開始
            'm_nSelectMen = 0

            '-------------------------------------------------------------------
            ' 表示の初期化
            '-------------------------------------------------------------------
            mcls_DataMap = New clsDataMap(mcls_Param, mcls_PreParam, gcls_Log)               ' 疵マップデータ

            'ファンクションボタン情報初期設定
            ReDim m_typMenu(1)
            FunctionKey_Init()

            '-----------------------------------------------
            'イベントの設定
            '-----------------------------------------------
            'マップ自動更新
            AddHandler mcls_DispMap.SelectMapCell_Auto, AddressOf frmJisseki_SelectMapCellAuto
            'マップ自動更新(データが無い場合初期化)
            AddHandler mcls_DispMap.SelectMapCell_AutoDspInit, AddressOf frmJisseki_SelectMapCellAutoDspInit
            'マップ選択
            AddHandler mcls_DispMap.SelectMapCell_Click, AddressOf frmJisseki_SelectMapCellClick
            AddHandler mcls_DispMap.SelectMapTcmCell_Click, AddressOf frmJisseki_SelectTcmMapCellClick
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
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績画面 コンストラクタ中に例外 [{0}]", ex.Message))
        End Try

        sw.Stop()
        gcls_Log.Write(tClass_LogManager.LNO.INF, String.Format("frmJisseki new 時間[{0}]", sw.Elapsed))

    End Sub

    '///////////////////////////////////////////////////////////////////////////
    'メインモジュール呼び出し関数(外部呼出し)
    '///////////////////////////////////////////////////////////////////////////
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
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像ゾーンの設定異常 Width={0}[{1}]", nWidth, ex.Message))
        End Try
    End Sub


    '***************************************************************************
    ' イベントハンドラ関数
    '***************************************************************************
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmJisseki_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Try

            mcls_DispCoil.Load()

            Dim s As System.Windows.Forms.Screen = System.Windows.Forms.Screen.FromControl(Me.Owner)
            Me.Location = s.Bounds.Location
            Me.Height = Me.Owner.Height
            Me.Width = Me.Owner.Width

            If NOA_ONLINE <> g_NOA Then                                         ' デバッグ時
                ' フォーム状態
                Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
                Me.tProp_EndButtonEnable = False
                Me.FormBorderStyle = Windows.Forms.FormBorderStyle.Sizable      ' フォームサイズ変更可能
            End If

            ' 画面タイトルセット
            SetJissekiTitle(Men)

            While (Not mcls_Param.ReadAllParam())                               ' パラメータ読み込めるまで、粘る
                System.Threading.Thread.Sleep(1000)
            End While
            While (Not mcls_PreParam.ReadAll())                               ' パラメータ読み込めるまで、粘る
                System.Threading.Thread.Sleep(1000)
            End While

            ' 各表示間隔設定
            mcls_DataMap.Interval(EM_LONG_POS.SMALL) = mcls_Param.MapStageSmall
            mcls_DataMap.Interval(EM_LONG_POS.BIG) = mcls_Param.MapStageBig

            mcls_DispMap.SelectColor(0) = g_ColorStop1st
            mcls_DispMap.SelectColor(1) = g_ColorStop2nd

            ' 表示間隔小にて起動
            SetMapInterval(EM_LONG_POS.SMALL)

            '-------------------------------------------------------------------
            ' オーバーレイの表示設定
            '-------------------------------------------------------------------
            Dim nOverLay As Integer = tMod.ReadIniInt(SO_GAMENN, KEY_OVERLAY_VISIBLE, LOCALINI_NAME, 1)
            Select Case nOverLay
                Case 0
                    Overlay = False
                Case 1
                    Overlay = True
                Case Else
                    Overlay = True
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("オーバーレイ切替異常[{0}]", nOverLay))
            End Select

            ''-------------------------------------------------------------------
            '' スケールの表示設定
            ''-------------------------------------------------------------------
            'Dim nScale As Integer = tMod.ReadIniInt(SO_GAMENN, KEY_SCALE_VISIBLE, LOCALINI_NAME, 1)
            'Select Case nScale
            '    Case 0
            '        Memori = False
            '    Case 1
            '        Memori = True
            '    Case Else
            '        Memori = True
            '        gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("スケール切替異常[{0}]", nScale))
            'End Select

            '-------------------------------------------------------------------
            ' 設定ファイル読込(KizuTask.ini読み込み)
            '-------------------------------------------------------------------
            GetIniSetting(Men)

            '-------------------------------------------------------------------
            ' 表示初期化
            '-------------------------------------------------------------------
            DspInit()

            AddHandler TableLayoutPanel4.Resize, AddressOf MapReSize
            MapReSize(Nothing, Nothing)

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績画面 フォームロード中に例外 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' オブジェクトの破棄
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmJisseki_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        Try
            '-------------------------------------------------------------------
            ' KizuLocal.iniへの保存
            '-------------------------------------------------------------------
            If Not mcls_DispDefect Is Nothing Then
                tMod.WriteIni(SO_GAMENN, KEY_OVERLAY_VISIBLE, LOCALINI_NAME, IIf(mcls_DispDefect(0).OverlayVisible, 1, 0).ToString)
                'tMod.WriteIni(SO_GAMENN, KEY_SCALE_VISIBLE, LOCALINI_NAME, IIf(mcls_DispDefect(0).MemoriVisible, 1, 0).ToString)
            End If

            'イベント解除
            RemoveHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged
            RemoveHandler mcls_DispMap.SelectMapCell_Auto, AddressOf frmJisseki_SelectMapCellAuto
            RemoveHandler mcls_DispMap.SelectMapCell_AutoDspInit, AddressOf frmJisseki_SelectMapCellAutoDspInit
            RemoveHandler mcls_DispMap.SelectMapCell_Click, AddressOf frmJisseki_SelectMapCellClick
            RemoveHandler dgvMap.MouseWheel, AddressOf dgvMap_MouseWheel
            RemoveHandler UcDefect1.picImg1.DoubleClick, AddressOf pic_DoubleClick
            RemoveHandler UcDefect1.picImg2.DoubleClick, AddressOf pic_DoubleClick
            RemoveHandler UcDefect2.picImg1.DoubleClick, AddressOf pic_DoubleClick
            RemoveHandler UcDefect2.picImg2.DoubleClick, AddressOf pic_DoubleClick

            '-------------------------------------------------------------------
            ' 資源解放
            '-------------------------------------------------------------------
            If Not mfrm_Search Is Nothing Then                            ' ファイル保存もやるので、早めに開放
                mfrm_Search.SaveSetting()
                mfrm_Search.Dispose()
                mfrm_Search = Nothing
            End If
            If Not mcls_DispDefect Is Nothing Then
                For Each dsp As clsDispDefect In mcls_DispDefect
                    If Not dsp Is Nothing Then
                        dsp.Dispose()
                    End If
                Next
                mcls_DispDefect = Nothing
            End If
            If Not mcls_DataMap Is Nothing Then
                mcls_DataMap.Dispose()
                mcls_DataMap = Nothing
            End If
            If Not mfrm_PopImage Is Nothing Then
                mfrm_PopImage.Dispose()
                mfrm_PopImage = Nothing
            End If
            If Not mfrm_PopTcmImage Is Nothing Then
                mfrm_PopTcmImage.Dispose()
                mfrm_PopTcmImage = Nothing
            End If
            If Not mcls_Param Is Nothing Then                               ' パラメータ管理クラスは一番最後にやること
                mcls_Param.Dispose()
                mcls_Param = Nothing
            End If
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績画面 オブジェクト破棄中に例外 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵マップ/疵セル選択イベント(オート時)
    ''' </summary>
    ''' <param name="nMen">表裏面</param>
    ''' <param name="pntKizu">選択疵情報</param>
    ''' <param name="emInf">疵情報位置</param>
    ''' <remarks></remarks>
    Private Sub frmJisseki_SelectMapCellAuto(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE, ByVal emInf As clsDispMap.EM_DSP_KIZU_INF)
        Try
            mcls_DispDefect(CType(emInf, Integer)).DspDefect(nMen, pntKizu, True)

            If emInf = clsDispMap.EM_DSP_KIZU_INF.DSP_JI1 Then
                SetDefectButton(nMen, pntKizu)
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ/疵セル選択イベントで例外発生 表裏面={0}[{1}]", nMen, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵マップ/疵セル選択消去イベント(オート時)
    ''' </summary>
    ''' <param name="emInf">疵情報位置</param>
    ''' <remarks></remarks>
    Private Sub frmJisseki_SelectMapCellAutoDspInit(ByVal emInf As clsDispMap.EM_DSP_KIZU_INF)
        Try
            mcls_DispDefect(CType(emInf, Integer)).DspInit()
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ/疵セル選択消去イベントで例外発生 表裏面={0}[{1}]", MEN, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵マップ/疵セル選択イベント(選択時)
    ''' </summary>
    ''' <param name="nMen">表裏面</param>
    ''' <param name="pntKizu">選択疵情報</param>
    ''' <remarks></remarks>
    Private Sub frmJisseki_SelectMapCellClick(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)
        Dim nLen As Integer = 0
        Dim coilinf As COINFO_BASE = New COINFO_BASE

        Try

            For nCnt As Integer = mcls_DispDefect.Count - 1 To 1 Step -1
                'If 0 < mcls_DispMap.GetSelectKizu(nCnt) Then
                Dim strVal As TYP_MAP_LIST_VALUE = Nothing
                If mcls_DispMap.GetSelectKizu(nCnt, strVal) AndAlso 0 < strVal.nKizuNo Then
                    mcls_DispDefect(nCnt).DspDefect(nMen, mcls_DispDefect(nCnt - 1).CurrentDefect, False)
                Else
                    mcls_DispDefect(nCnt).DspInit()
                End If
            Next

            '疵情報を表示
            mcls_DispDefect(0).DspDefect(nMen, pntKizu, False)

            SetDefectButton(nMen, pntKizu)

            If mfrm_PopImage.Visible Then                               '疵画像拡大表示中
                'ダブルクリック以外は最新疵の情報を表示
                mfrm_PopImage.DispImgNo = 1
                DispDefectImg()
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ/疵セル選択イベント 表裏面={0}[{1}]", nMen, ex.Message))
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
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像ダブルクリック異常[{0}]", ex.Message))
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
                vsbMap.ValueProgramable = value
                SetScroll(value)
            End If
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップスクロールイベント異常[{0}]", ex.Message))
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
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マウススクロールイベント処理異常[{0}]", ex.Message))
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
    Private Sub frmJisseki_SelectTcmMapCellClick(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)
        Dim nLen As Integer = 0
        'Dim coilinf As COINFO_BASE = New COINFO_BASE

        Try
            'If EM_REFRESH_MODE.PAUSE <> mcls_DispMap.RefreshMode Then
            '    Exit Try
            'End If

            'If mcls_DispCoil.GetCoilInf(pntKizu.nRec, coilinf) Then    'コイル情報テーブル読込
            '    mcls_DispCoil.DspCoil(pntKizu.nRec, coilinf, Men)      'コイル情報表示
            'End If

            DispDefectTcmImg(nMen, pntKizu)

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ/疵セル選択イベント(TCM) 表裏面={0}[{1}]", nMen, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' ファンクションメニュー切替
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmJisseki_KeyDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyDown
        'shiftキー以外は終了
        If Not e.Shift Then Exit Sub
        'ファンクションボタンの現状保存と表示
        FuncInfoSaveDisp()
    End Sub

    ''' <summary>
    ''' 切り替えボタン押下（shiftキー押下時と同様の処理）
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnShift_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnShift.Click
        'ファンクションボタンの現状保存と表示
        FuncInfoSaveDisp()
    End Sub


    '***************************************************************************
    ' 内部関数
    '***************************************************************************

    ''' <summary>
    ''' 設定値取得
    ''' </summary>
    ''' <param name="nMen">面[0:表、1:裏]</param>
    Private Sub GetIniSetting(ByVal nMen As Integer)
        Try
            Dim nVal As Integer = 0

            '-------------------------------------------------------------------
            ' 疵マップコイル向き(表裏共通)
            '-------------------------------------------------------------------
            nVal = tMod.ReadIniInt(SO_GAMENN, "MAP_SCROLL_DIRECT", TASKINI_NAME, 0)
            Select Case nVal
                Case 0
                    mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD            '下から上
                Case 1
                    mcls_DispMap.MapDir = EM_MAP_SCROOL.HEAD_TO_TAIL            '上から下
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini MAP_SCROLL_DIRECT異常 値={0}", nVal))
                    mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD
            End Select

            '-------------------------------------------------------------------
            ' 疵ラベルDS位置(表/裏)
            '-------------------------------------------------------------------
            Dim strMapViewDirect() As String = {"MAP_VIEW_DIRECT_O", "MAP_VIEW_DIRECT_U"}
            nVal = tMod.ReadIniInt(SO_GAMENN, strMapViewDirect(nMen), TASKINI_NAME, 0)
            Select Case nVal
                Case 0
                    mcls_DispMap.Side = EM_MAP_SIDE.NORMAL                      '左がDS
                Case 1
                    mcls_DispMap.Side = EM_MAP_SIDE.REVERSE                     '左がWS
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini {1} 値={0}", nVal, strMapViewDirect(nMen)))
                    mcls_DispMap.Side = EM_MAP_SIDE.NORMAL
            End Select

            '-------------------------------------------------------------------
            ' 疵マップDS位置(表/裏)
            '-------------------------------------------------------------------
            Dim strMapCellDirect() As String = {"MAP_CELL_DIRECT_O", "MAP_CELL_DIRECT_U"}
            nVal = tMod.ReadIniInt(SO_GAMENN, strMapCellDirect(nMen), TASKINI_NAME, 0)
            Select Case nVal
                Case 0
                    mcls_DispMap.DSPos = EM_MAP_CELL.NORMAL                     '左がDS
                Case 1
                    mcls_DispMap.DSPos = EM_MAP_CELL.REVERSE                    '左がWS
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini {1} 値={0}", nVal, strMapCellDirect(nMen)))
                    mcls_DispMap.DSPos = EM_MAP_CELL.NORMAL
            End Select

			'// 2023.11.17 最終工程検査装置高機能化その２ -------------------->>>>>
			'-------------------------------------------------------------------
			' 疵マップ幅位置番号(表/裏)
			'-------------------------------------------------------------------
			Dim strMapViewWidthNo() As String = {"MAP_VIEW_WIDTH_NO_O", "MAP_VIEW_WIDTH_NO_U"}
			Dim nMapViewWidthNo As Integer = tMod.ReadIniInt(SO_GAMENN, strMapViewWidthNo(nMen), TASKINI_NAME, 0)
			Select Case nMapViewWidthNo
				Case 0
					mcls_DispMap.WidthNo = EM_MAP_WIDTH_NO.NORMAL                      '左から1,2,・・・
				Case 1
					mcls_DispMap.WidthNo = EM_MAP_WIDTH_NO.REVERSE                     '右から1,2,・・・
				Case Else
					gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini {1} 値={0}", nMapViewWidthNo, strMapViewWidthNo(nMen)))
					mcls_DispMap.WidthNo = EM_MAP_WIDTH_NO.NORMAL
			End Select
			'// 2023.11.17 最終工程検査装置高機能化その２ <<<<<--------------------

			'-------------------------------------------------------------------
			' 疵画像上下反転(表/裏)
			'-------------------------------------------------------------------
			Dim strImageTopBot() As String = {"IMAGE_TOP_BOTTOM_INF_O", "IMAGE_TOP_BOTTOM_INF_U"}
            m_emImageTop(nMen) = CType(tMod.ReadIniInt(SO_GAMENN, strImageTopBot(nMen), TASKINI_NAME, 0), EM_IMAGE_TOP)
            Select Case m_emImageTop(nMen)
                Case EM_IMAGE_TOP.NORMAL                                        '反転無し
                Case EM_IMAGE_TOP.REVERSE                                       '反転有り
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini {1} 値={0}", m_emImageTop(nMen), strImageTopBot(nMen)))
                    m_emImageTop(nMen) = EM_IMAGE_TOP.NORMAL
            End Select
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.IsTurnTB = m_emImageTop(nMen)
            Next

            '-------------------------------------------------------------------
            ' 疵画像左右反転(表/裏)
            '-------------------------------------------------------------------
            Dim strImageSide() As String = {"IMAGE_SIDE_INF_O", "IMAGE_SIDE_INF_U"}
            m_emImageSide(nMen) = CType(tMod.ReadIniInt(SO_GAMENN, strImageSide(nMen), TASKINI_NAME, 0), EM_IMAGE_SIDE)
            Select Case m_emImageSide(nMen)
                Case EM_IMAGE_SIDE.NORMAL                                       '反転無し
                Case EM_IMAGE_SIDE.REVERSE                                      '反転有り
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini {1} 値={0}", m_emImageSide(nMen), strImageSide(nMen)))
                    m_emImageSide(nMen) = EM_IMAGE_SIDE.NORMAL
            End Select
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.IsTurnLR = m_emImageSide(nMen)
            Next

            '-------------------------------------------------------------------
            ' 疵画像ラベルDS位置(表/裏)
            '-------------------------------------------------------------------
            Dim strImageView() As String = {"IMAGE_VIEW_DIRECT_O", "IMAGE_VIEW_DIRECT_U"}
            Dim nImgSide As EM_MAP_SIDE = CType(tMod.ReadIniInt(SO_GAMENN, strImageView(nMen), TASKINI_NAME, 0), EM_MAP_SIDE)
            Select Case nImgSide
                Case EM_MAP_SIDE.NORMAL                                         '左がDS
                Case EM_MAP_SIDE.REVERSE                                        '左がWS
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini {1} 値={0}", nImgSide, strImageView(nMen)))
                    nImgSide = EM_MAP_SIDE.NORMAL
            End Select
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.Side = nImgSide
            Next

            '-------------------------------------------------------------------
            'TCM疵画像上下反転
            '-------------------------------------------------------------------
            Dim nTcmImgVMode As EM_IMAGE_TOP = CType(tMod.ReadIniInt(SO_GAMENN, "TCM_IMAGE_TOP_BOTTOM_INF", TASKINI_NAME, 0), EM_IMAGE_TOP)
            Select Case nTcmImgVMode
                Case EM_IMAGE_TOP.NORMAL                                        '反転無し
                Case EM_IMAGE_TOP.REVERSE                                       '反転有り
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini TCM疵画像上下反転異常[{0}]", nTcmImgVMode))
                    nTcmImgVMode = EM_IMAGE_TOP.NORMAL
            End Select
            mfrm_PopTcmImage.IsTurnTB = nTcmImgVMode                                            '上下反転設定


            '-------------------------------------------------------------------
            'TCM疵画像左右反転
            '-------------------------------------------------------------------
            Dim nTcmImgHMode As EM_IMAGE_SIDE = CType(tMod.ReadIniInt(SO_GAMENN, "TCM_IMAGE_SIDE_INF", TASKINI_NAME, 0), EM_IMAGE_SIDE)
            Select Case nTcmImgHMode
                Case EM_IMAGE_SIDE.NORMAL                                       '反転無し
                Case EM_IMAGE_SIDE.REVERSE                                      '反転有り
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini TCM疵画像左右反転異常[{0}]", nTcmImgHMode))
                    nTcmImgHMode = EM_IMAGE_SIDE.NORMAL
            End Select
            mfrm_PopTcmImage.IsTurnLR = nTcmImgHMode                                                               '左右反転設定

            '-------------------------------------------------------------------
            'TCM疵画像ラベルDS位置
            '-------------------------------------------------------------------
            Dim nTcmImgSide As EM_MAP_SIDE = CType(tMod.ReadIniInt(SO_GAMENN, "TCM_IMAGE_VIEW_DIRECT", TASKINI_NAME, 0), EM_MAP_SIDE)
            Select Case nTcmImgSide
                Case EM_MAP_SIDE.NORMAL                                         '左がDS
                Case EM_MAP_SIDE.REVERSE                                        '左がWS
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini TCM疵画像ラベルDS位置異常[{0}]", nTcmImgSide))
                    nTcmImgSide = EM_MAP_SIDE.NORMAL
            End Select
            mfrm_PopTcmImage.Side = nTcmImgSide                                                    '疵画像ラベルDS位置

            '-------------------------------------------------------------------
            ' 画像100pixel横幅設定
            '-------------------------------------------------------------------
            nVal = tMod.ReadIniInt(SO_GAMENN, "IMAGE_WIDTH", TASKINI_NAME, 100)
            If nVal < 0 Then                                                    ' 異常値
                gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini IMAGE_WIDTH異常 値={0}", nVal))
                nVal = 100
            End If
            For Each dsp As clsDispDefect In mcls_DispDefect                       ' 画像倍率反映
                dsp.ImageWidth = nVal
            Next

            '-------------------------------------------------------------------
            ' 画像100pixel横幅設定
            '-------------------------------------------------------------------
            nVal = tMod.ReadIniInt(SO_GAMENN, "TCM_IMAGE_WIDTH", TASKINI_NAME, 100)
            If nVal < 0 Then                                                    ' 異常値
                gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini TCM_IMAGE_WIDTH異常 値={0}", nVal))
                nVal = 100
            End If
            m_nTcmImageWide = nVal

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績画面 設定ファイル読み込み中に例外 [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' 疵画像情報の初期化
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

            clsDispDefect = New clsDispDefect(typDefInf, mcls_Param, gcls_Log, RSC_IMG_DEFAULT)
            clsDispDefect.SetScaleConfig(JISSEKI_SCALE_INTVAL_X, JISSEKI_SCALE_NUM_X, JISSEKI_SCALE_INTVAL_Y, JISSEKI_SCALE_NUM_Y)
            clsDispDefect.ImageZoom = True

        Catch ex As Exception
            gcls_Log.Write(LNO.ERR, String.Format("疵情報表示エリア初期化[{0}],[{1}]", nImgNo, ex.Message))
        End Try

        Return clsDispDefect

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
                .lblKensa = ucCoil.lblKensa
                .lblKiki = ucCoil.lblKiki
                .tblLayoutPnl = ucCoil.TableLayoutPanel1
            End With

            clsDispCoil = New clsDispCoil(typCoilDsp, gcls_Log, mcls_Param)

        Catch ex As Exception
            gcls_Log.Write(LNO.ERR, String.Format("コイル情報表示エリア初期化異常[{0}]", ex.Message))
        End Try

        Return clsDispCoil

    End Function


    ''' <summary>
    ''' 疵マップ行間隔の設定
    ''' </summary>
    ''' <param name="emStage">変更後の間隔 1:表示(小) 2:表示(大)</param>
    ''' <remarks></remarks>
    Private Sub SetMapInterval(ByVal emStage As EM_LONG_POS)
        Try

            'MAP表示間隔を設定
            mcls_DispMap.Stage = emStage

            '-------------------------------------------------------------------
            'ボタン表示変更
            '-------------------------------------------------------------------
            Dim strBtn As String
            If EM_LONG_POS.BIG = mcls_DispMap.Stage Then
                strBtn = mcls_Param.MapStageBig.ToString & "m→" & mcls_Param.MapStageSmall.ToString & "m"
            Else
                strBtn = mcls_Param.MapStageSmall.ToString & "m→" & mcls_Param.MapStageBig.ToString & "m"
            End If
            If EM_DIV_FUNC_TYPE.DIV_SELECT_0 = m_nMenuMode Then
                btnF08.Text = strBtn
            Else
                m_typMenu(EM_DIV_FUNC_TYPE.DIV_SELECT_1).func(8 - 1).strText = strBtn
            End If

            mcls_DispMap.DspData(Men)


        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップ間隔設定異常 value={0}[{1}]", emStage, ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' 実績画面初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspInit()
        Try

            mcls_DispMap.ClearMap()
            mcls_DispMap.ClearDeep()
            mcls_DispMap.ClearTcm()

            mcls_DispCoil.DspInit()

            For Each dsp As clsDispDefect In mcls_DispDefect                    ' 疵情報初期化
                dsp.DspInit()
            Next

            ' ファンクションキー操作を初期化
            btnF01.Enabled = False
            btnF02.Enabled = False
            btnF03.Enabled = False
            btnF04.Enabled = False
            btnF05.Enabled = True
            btnF06.Enabled = False
            btnF07.Enabled = False
            btnF08.Enabled = False
            btnF09.Enabled = False
            btnF10.Enabled = False
            btnF11.Enabled = False
            btnF12.Enabled = True

            vsbMap.ValueProgramable = 0                                         ' スクロールバー初期化

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績画面初期化異常 [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' 実績画面の画面タイトルを設定する
    ''' </summary>
    ''' <param name="nMen">表示面</param>
    ''' <remarks></remarks>
    Private Sub SetJissekiTitle(ByVal nMen As Integer)
        ' 画面タイトルセット
        If 0 = nMen Then
            Me.Text = "検査実績画面(表面)"
        Else
            Me.Text = "検査実績画面(裏面)"
        End If

        SetTitleName(Me)
    End Sub

    ''' <summary>
    ''' 疵マップスクロール処理
    ''' </summary>
    ''' <param name="nValue">スクロール量</param>
    ''' <remarks></remarks>
    Private Sub SetScroll(ByVal nValue As Integer)
        Try
            Dim value As Integer = nValue
            If mcls_DispMap.MapDir = EM_MAP_SCROOL.HEAD_TO_TAIL Then value = vsbMap.Maximum - (vsbMap.LargeChange - 1) - nValue '進行方上

            Dim nMaxRowNum As Integer
            mcls_DispMap.SetMapData(Men, value, nMaxRowNum)

            dgvMap.Update()
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップスクロール処理異常[{0}]", ex.Message))
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
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップスクロールイベント異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵画像拡大表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispDefectTcmImg(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)
        Try
            Dim typDefInf As TYP_MAE_DEFECT_DATA = Nothing
            Dim key As clsDataMap.DEFECT_KEY
            key.nKizuNo = pntKizu.nKizuNo

            mcls_DataMap.TcmDefectData(nMen, key, typDefInf)


            ShowPopupTcm(typDefInf, mfrm_PopTcmImage)

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップスクロールイベント異常[{0}]", ex.Message))
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
    Private Sub frmJisseki_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' 0オリジン

            Case 0                              'F1
                Call SelectF1()
            Case 1                              'F2
                Call SelectF2()
            Case 2                              'F3
                Call SelectF3()
            Case 3                              'F4
                Call SelectF4()
            Case 4                              'F5
                Call SelectF5()
            Case 5                              'F6
                Call SelectF6()
            Case 6                              'F7
                Call SelectF7()
            Case 7                              'F8
                Call SelectF8()
            Case 8                              'F9
                Call SelectF9()
            Case 9                              'F10
                Call SelectF10()
            Case 10                             'F11                
                Call SelectF11()
            Case 11
                MyBase.ExecFromClose()          ' 終了
        End Select
    End Sub

    ''' <summary>
    ''' ファンクションボタン Ｆ１
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF1()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0, EM_DIV_FUNC_TYPE.DIV_SELECT_1
                '前コイル
                All_Popup_Close(True)
                btnF01.Enabled = False
                ReadPreviousCoil()
            Case Else
        End Select

    End Sub

    ''' <summary>
    ''' ファンクションボタン Ｆ２
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF2()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0, EM_DIV_FUNC_TYPE.DIV_SELECT_1
                '次コイル
                All_Popup_Close(True)
                btnF02.Enabled = False
                ReadNextCoil()

            Case Else
        End Select
    End Sub


    ''' <summary>
    ''' ファンクションボタン Ｆ３
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF3()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0, EM_DIV_FUNC_TYPE.DIV_SELECT_1
                ' 前疵
                mcls_DispMap.SetSelection(Men, False)
            Case Else
        End Select


    End Sub

    ''' <summary>
    ''' ファンクションボタン Ｆ４
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF4()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0, EM_DIV_FUNC_TYPE.DIV_SELECT_1
                ' 次疵
                mcls_DispMap.SetSelection(Men, True)
            Case Else
        End Select
    End Sub

    ''' <summary>
    ''' ファンクションボタン Ｆ５
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF5()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0, EM_DIV_FUNC_TYPE.DIV_SELECT_1
                frmSearchShowDialog()

            Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
            Case Else
        End Select

    End Sub

    ''' <summary>
    ''' ファンクションボタン Ｆ６
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF6()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                Dim frm As New frmMixedRate(gcls_Log, mcls_DataMap.KizukenNo_In)

                'データチェック
                If frm.ChkData(mcls_DataMap.KizukenNo_In) Then

                    'データあり
                    frm.ShowDialog()
                    frm.Dispose()
                    frm = Nothing
                Else

                    'データなし
                    Dim frmMsg As New tClassForm.frmPop_MsgBox("データが存在しません", Me.Text)
                    frmMsg.ShowDialog()
                    frm.Dispose()
                    frm = Nothing

                End If
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
                ' 目盛りの表示・非表示状態を反転させる
                Overlay = Not Overlay
                'Memori = Not Memori
                dgvMap.Focus()
            Case Else
        End Select

    End Sub

    ''' <summary>
    ''' ファンクションボタン Ｆ７
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF7()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                All_Popup_Close(True)
                ''HACK　2015/02/02/18 現在の疵の位置を覚えておく
                'Dim key As clsDataMap.LIST_KEY = mcls_DispMapDefLst.CurrentKey
                'gcls_Log.Write(tClass_LogManager.LNO.INF, String.Format("面切替ボタン key = [{0}]", key.ToString))

                ''HACK　2015/01/06 シャーカット線初期化
                'mcls_DispMap.DspLineInit()

                ''HACK　2015/02/23  ResizeするとvsbMap.Valueが０になり、表/裏面切替がうまくいかない
                ''ResizeScrollSize()      

                If mcls_DispMap.Men = 0 Then
                    mcls_DispMap.Men = 1
                    'm_nSelectMen = 1
                    SetJissekiTitle(mcls_DispMap.Men)
                Else
                    mcls_DispMap.Men = 0
                    'm_nSelectMen = 0
                    SetJissekiTitle(mcls_DispMap.Men)
                End If

                SetMen(mcls_DispMap.Men)

            Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
                All_Popup_Close(True)
                ''エッジ検出状況画面
                Dim frm As New frmEdgeDetect(gcls_Log, mcls_DataMap.CoilInfo, mcls_DataMap.TurnTB, mcls_DataMap.KizukenNo_In)
                'データチェック
                If frm.ChkData_CoilInf(mcls_DataMap.KizukenNo_In) Then
                    'データあり
                    frm.ShowDialog()
                    frm.Dispose()
                    frm = Nothing
                Else

                    'データなし
                    Dim frmMsg As New tClassForm.frmPop_MsgBox("データが存在しません", Me.Text)
                    frmMsg.ShowDialog()
                    frm.Dispose()
                    frm = Nothing

                End If

            Case Else
        End Select
    End Sub

    ''' <summary>
    ''' ファンクションボタン Ｆ８
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF8()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                All_Popup_Close(False)

                If EM_LONG_POS.SMALL = Stage Then
                    SetMapInterval(EM_LONG_POS.BIG)
                Else
                    SetMapInterval(EM_LONG_POS.SMALL)
                End If

                ' 現在マップに表示している位置を新しいMAP表示間隔でも表示できるようにする。
                Dim nValue As Integer = (vsbMap.Value)
                If mcls_DispMap.MapDir = EM_MAP_SCROOL.HEAD_TO_TAIL Then
                    '進行方向が上から下の場合は、下の位置を基準にする
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
                If mcls_DispMap.MapDir = EM_MAP_SCROOL.HEAD_TO_TAIL Then
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

            Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
                All_Popup_Close(True)
                ' コイルトレンド画面
                Dim frm As New frmCoilTrand(mcls_DataMap.CoilInfo, mcls_DataMap.TurnTB, gcls_Log)
                frm.Owner = Me
                frm.ShowDialog(Me)
                frm.Dispose()
                frm = Nothing
            Case Else
        End Select

    End Sub

    ''' <summary>
    ''' ファンクションボタン Ｆ９
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF9()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                Dim frmDefMap As New frmDefMapJisseki(mcls_DataMap.KizukenNo_In, mcls_DataMap.CoilLength, mcls_DataMap.CoilWidth, mcls_DataMap.CoilInfo.bCorrect, mcls_Param, gcls_Log)

                'コイル情報セット
                frmDefMap.SetCoilData(mcls_DataMap.CoilInfo)
                'カット情報セット
                frmDefMap.SetCutData(mcls_DataMap.CutDataLst)
                '疵情報リストセット
                frmDefMap.SetDefectData(0, mcls_DataMap.DefectDataLst(0).Values.ToList)
                frmDefMap.SetDefectData(1, mcls_DataMap.DefectDataLst(1).Values.ToList)
                'コイル反転有無セット
                frmDefMap.SetTurnTB(mcls_DataMap.TurnTB)

                frmDefMap.DspData()
                frmDefMap.Owner = Me
                frmDefMap.ShowDialog()

                frmDefMap.Dispose()
                frmDefMap = Nothing
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
                'コイル情報表示
                Dim frm As New frmDspCoil(mcls_DataMap.KizukenNo_In)
                frm.ShowDialog()
                frm.Dispose()
                frm = Nothing
            Case Else
        End Select

    End Sub

    ''' <summary>
    ''' ファンクションボタン Ｆ１０
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF10()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                ' ハードコピー
                Call HardCopy(IMG_PATH, "実績画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
            Case Else
        End Select

    End Sub

    ''' <summary>
    ''' ファンクションボタン Ｆ１１
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF11()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                ' 画面印刷
                Call PrintOut()
			Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
				'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
				Dim frm As New frmLongPdList(gcls_Log, mcls_Param, False)

				Dim lstKanriNo As New List(Of String)
				Dim lstCoilNo As New List(Of String)

				lstKanriNo.Add(mcls_DataMap.CoilInfo().strKanriNo)
				lstCoilNo.Add(mcls_DataMap.CoilInfo().strCoilNo)
				frm.Owner = Me
				frm.tProp_Moveable = True
				frm.PreDispProc(lstKanriNo, lstCoilNo, IIf(mcls_DispMap.Side = EM_MAP_SIDE.REVERSE, True, False))
				frm.ShowDialog()
				frm.Dispose()
				frm = Nothing
				'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
			Case Else
		End Select

    End Sub

    ''' <summary>
    ''' ファンクションボタン情報初期設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub FunctionKey_Init()

        For ii As Integer = 0 To m_typMenu.Length - 1

            m_typMenu(ii).initialize(my_btnFunction.Length)

            With m_typMenu(ii)
                Select Case ii
                    Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                        'ボタン名称
                        .func(0).strText = "前材"
                        .func(1).strText = "次材"
                        .func(2).strText = "前疵"
                        .func(3).strText = "次疵"
                        .func(4).strText = "検索"
                        .func(5).strText = "疵混入率"
                        .func(6).strText = "裏面表示"
                        .func(7).strText = "5m->30m"
                        .func(8).strText = "疵マップ"
                        .func(9).strText = "画面保存"
                        .func(10).strText = "画面印刷"
                        .func(11).strText = "メニュー"
                        '初期値セット
                        .func(4).bEnabled = True
                        .func(11).bEnabled = True

                    Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
                        'ボタン名称
                        .func(0).strText = "前材"
                        .func(1).strText = "次材"
                        .func(2).strText = "前疵"
                        .func(3).strText = "次疵"
                        .func(4).strText = "検索"
                        .func(5).strText = "ｵｰﾊﾞｰﾚｲ"
                        .func(6).strText = "ｴｯｼﾞ状況"
                        .func(7).strText = "ｺｲﾙﾄﾚﾝﾄﾞ"
                        .func(8).strText = "ｺｲﾙ情報"
						.func(9).strText = ""
						'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>						
						'.func(10).strText = ""
						.func(10).strText = "長周期"
						'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
						.func(11).strText = "メニュー"
                        '初期値セット
                        .func(4).bEnabled = True
						.func(5).bEnabled = True
						.func(11).bEnabled = True
                End Select
            End With
        Next

        For ii As Integer = 0 To my_btnFunction.Length - 1
            With m_typMenu(m_nMenuMode)
                my_btnFunction(ii).Enabled = .func(ii).bEnabled
                my_btnFunction(ii).Text = .func(ii).strText
            End With
        Next


    End Sub

    ''' <summary>
    ''' ファンクションボタンの現状保存と表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub FuncInfoSaveDisp()

        If mcls_Param Is Nothing Then Exit Sub

        For ii As Integer = 0 To my_btnFunction.Length - 1
            With m_typMenu(m_nMenuMode)
                .func(ii).bEnabled = my_btnFunction(ii).Enabled
                .func(ii).strText = my_btnFunction(ii).Text
            End With
        Next

        m_nMenuMode = CInt(IIf(EM_DIV_FUNC_TYPE.DIV_SELECT_0 = m_nMenuMode, EM_DIV_FUNC_TYPE.DIV_SELECT_1, EM_DIV_FUNC_TYPE.DIV_SELECT_0))

        For ii As Integer = 0 To my_btnFunction.Length - 1
            With m_typMenu(m_nMenuMode)
                my_btnFunction(ii).Enabled = .func(ii).bEnabled
                my_btnFunction(ii).Text = .func(ii).strText
            End With
        Next

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                btnF06.BackColor = Color.FromKnownColor(KnownColor.Control)
                btnF07.BackColor = Color.FromKnownColor(KnownColor.Control)

            Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
                'ボタン色替(オーバーレイ)
                If Overlay Then
                    btnF06.BackColor = Color.Cyan
                Else
                    btnF06.BackColor = Color.FromKnownColor(KnownColor.Control)
                End If
                ''ボタン色替(スケール)
                'If Memori Then
                '    btnF07.BackColor = Color.Cyan
                'Else
                '    btnF07.BackColor = Color.FromKnownColor(KnownColor.Control)
                'End If
        End Select

    End Sub

    ''' <summary>
    ''' 前コイル情報読み込み処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadPreviousCoil()

        mfrm_Search.ReadPreviousCoil()
        If mfrm_Search.ReturnCode = frmSearch.SEARCH_RETURN.emSELECT_COIL Then
            DspResult()
        End If

    End Sub


    ''' <summary>
    ''' 次コイル情報読み込み処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadNextCoil()

        mfrm_Search.ReadNextCoil()
        If mfrm_Search.ReturnCode = frmSearch.SEARCH_RETURN.emSELECT_COIL Then
            DspResult()
        End If

    End Sub


    ''' <summary>
    ''' 検索画面表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub frmSearchShowDialog()

        All_Popup_Close(True)
        mfrm_Search.ShowDialog()      ' 実績検索画面呼出

        If mfrm_Search.ReturnCode = frmSearch.SEARCH_RETURN.emSELECT_COIL Then

            'iniファイルの設定の反映は一番最初に行う必要がある
            '表示設定を読み込む
            GetIniSetting(Men)

            'コイル情報を初期化
            mcls_DispCoil.DspInit()

            DspResult()
        End If
    End Sub


    ''' <summary>
    ''' 前疵/次疵ボタン設定
    ''' </summary>
    ''' <param name="nMen"></param>
    ''' <param name="val"></param>
    Private Sub SetDefectButton(ByVal nMen As Integer, ByVal val As TYP_MAP_LIST_VALUE)

        btnF03.Enabled = mcls_DispMap.IsExistEachDef(nMen, val, False)
        btnF04.Enabled = mcls_DispMap.IsExistEachDef(nMen, val, True)
        If EM_DIV_FUNC_TYPE.DIV_SELECT_0 = m_nMenuMode Then
            m_typMenu(EM_DIV_FUNC_TYPE.DIV_SELECT_1).func(3 - 1).bEnabled = btnF03.Enabled
            m_typMenu(EM_DIV_FUNC_TYPE.DIV_SELECT_1).func(4 - 1).bEnabled = btnF04.Enabled
        Else
            m_typMenu(EM_DIV_FUNC_TYPE.DIV_SELECT_0).func(3 - 1).bEnabled = btnF03.Enabled
            m_typMenu(EM_DIV_FUNC_TYPE.DIV_SELECT_0).func(4 - 1).bEnabled = btnF04.Enabled
        End If

    End Sub

    ''' <summary>
    ''' 表示面切り替え
    ''' </summary>
    ''' <param name="nMen">表示する面</param>
    ''' <remarks></remarks>
    Private Sub SetMen(ByVal nMen As Integer)
        Try
            '-------------------------------------------------------------------
            ' 表示面の変更
            '-------------------------------------------------------------------
            If nMen = 0 Then                                                    ' 表面表示
                mcls_DispMap.Men = 0
                btnF07.Text = "裏面表示"
            Else                                                                ' 裏面表示時は表面に設定
                mcls_DispMap.Men = 1
                btnF07.Text = "表面表示"
            End If

            '切り替え前表示位置取得
            Dim nValue As Integer = vsbMap.Value

            '-------------------------------------------------------------------
            ' パラメータ初期化
            '-------------------------------------------------------------------
            GetIniSetting(nMen)
            '-------------------------------------------------------------------
            ' 選択初期化
            '-------------------------------------------------------------------
            mcls_DispMap.ClearMap()
            mcls_DispMap.ClearDeep()
            mcls_DispMap.ClearTcm()

            mcls_DispCoil.DspInit()

            For Each dsp As clsDispDefect In mcls_DispDefect                    ' 疵情報初期化
                dsp.DspInit()
            Next

            'コイル情報表示
            mcls_DispCoil.DspCoil(mcls_DataMap.CoilInfo, nMen)

            mcls_DispMap.ClearSelect()                              ' 選択疵を解除

            mcls_DispMap.DspData(nMen)

            RemoveHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged
            vsbMap.ValueProgramable = nValue
            SetScroll(nValue)
            AddHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("表示面切り替え中に例外 [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' 実績表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspResult()
        Try
            With mfrm_Search

                DspInit()

                ' ファンクションキー操作を実績表示に合わせて再設定
                ' F1からF5は全メニュー共通
                btnF01.Enabled = .ExistPrevious()           '前コイル
                btnF02.Enabled = .ExistNext()               '次コイル
                btnF03.Enabled = False                      '前疵は一旦無効
                btnF04.Enabled = False                      '後疵も一旦無効
                btnF05.Enabled = True

                Select Case m_nMenuMode
                    Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                        btnF06.Enabled = True
                        btnF07.Enabled = True
                        btnF08.Enabled = True
                        btnF09.Enabled = True
                        btnF10.Enabled = True
                        btnF11.Enabled = IIf(tClass.tClass_HeadCopy.IsCheckPrinter, True, False)
                        btnF12.Enabled = True
                        With m_typMenu(EM_DIV_FUNC_TYPE.DIV_SELECT_1)
                            .func(0).bEnabled = btnF01.Enabled
                            .func(1).bEnabled = btnF02.Enabled
                            .func(2).bEnabled = btnF03.Enabled
                            .func(3).bEnabled = btnF04.Enabled
                            .func(4).bEnabled = btnF05.Enabled
                            .func(5).bEnabled = True
                            .func(6).bEnabled = True                'エッジ検出有効
                            .func(7).bEnabled = True                'コイルトレンド有効
                            .func(8).bEnabled = True                'コイル情報有効
							.func(9).bEnabled = False
							'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>							
							'.func(10).bEnabled = False
							.func(10).bEnabled = True               ' 長周期一覧
							'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
							.func(11).bEnabled = True
                        End With

                    Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
                        btnF06.Enabled = True
                        btnF07.Enabled = True                       'エッジ検出有効
                        btnF08.Enabled = True                       'コイルトレンド有効
                        btnF09.Enabled = True                       'コイル情報有効
						btnF10.Enabled = False
						'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>							
						'btnF11.Enabled = False
						btnF11.Enabled = True                       '長周期一覧
						'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
						btnF12.Enabled = True
                        With m_typMenu(EM_DIV_FUNC_TYPE.DIV_SELECT_1)
                            .func(0).bEnabled = btnF01.Enabled
                            .func(1).bEnabled = btnF02.Enabled
                            .func(2).bEnabled = btnF03.Enabled
                            .func(3).bEnabled = btnF04.Enabled
                            .func(4).bEnabled = btnF05.Enabled
                            .func(5).bEnabled = True
                            .func(6).bEnabled = True
                            .func(7).bEnabled = True
                            .func(8).bEnabled = True
                            .func(9).bEnabled = True
                            .func(10).bEnabled = IIf(tClass.tClass_HeadCopy.IsCheckPrinter, True, False)
                            .func(11).bEnabled = True
                        End With
                End Select

                ' 今までのマップデータを破棄する
                mcls_DataMap.Dispose()
                mcls_DataMap = Nothing

                ' 新たなマップデータ取得
                mcls_DataMap = .DataMap
                mcls_DispMap.DataMap = mcls_DataMap
                For Each dsp As clsDispDefect In mcls_DispDefect
                    If Not dsp Is Nothing Then
                        dsp.DataMap = mcls_DataMap
                    End If
                Next

                Dim bTurnLR As Boolean = CBool(m_emImageSide(Men))
                Dim bTurnTop As Boolean = CBool(m_emImageTop(Men))
                For Each dsp As clsDispDefect In mcls_DispDefect
                    ' 画像の左右反転方向を設定します
                    If bTurnLR Xor .IsTurnDS Then
                        dsp.IsTurnLR = EM_IMAGE_SIDE.REVERSE
                    Else
                        dsp.IsTurnLR = EM_IMAGE_SIDE.NORMAL
                    End If
                    ' 画像の上下反転有無を設定します
                    If bTurnTop Xor .IsTurnNagate Then
                        dsp.IsTurnTB = EM_IMAGE_TOP.REVERSE
                    Else
                        dsp.IsTurnTB = EM_IMAGE_TOP.NORMAL
                    End If
                Next

                'コイル情報表示
                mcls_DispCoil.DspCoil(mcls_DataMap.CoilInfo, Men)

                mcls_DispMap.ClearSelect()                              ' 選択疵を解除

                SetMapInterval(Stage)

                vsbMap.Enabled = True
                vsbMap.Minimum = 0
                RemoveHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged
                vsbMap.Maximum = mcls_DispMap.GetScrollMax() + (vsbMap.LargeChange - 1)
                If mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD Then
                    vsbMap.ValueProgramable = mcls_DispMap.GetMapRowTop()
                Else
                    vsbMap.ValueProgramable = vsbMap.Maximum - mcls_DispMap.GetMapRowBot()
                End If

                AddHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged
                mcls_DispMap.MapResizeH()
            End With

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("コイル検索中に例外 [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' ポップアップクローズ
    ''' </summary>
    ''' <param name="btyp">false:マップ上のパネルのみ、true:パネル＋フォーム</param>
    ''' <remarks></remarks>
    Private Sub All_Popup_Close(ByRef btyp As Boolean)
        If btyp Then
            If mfrm_PopImage.Visible Then                               '疵画像拡大表示中
                mfrm_PopImage.CloseFrom()                               '拡大画像OFFに
            End If
            If mfrm_PopTcmImage.Visible Then                            '疵画像拡大表示中
                mfrm_PopTcmImage.CloseFrom()                            '拡大画像OFFに
            End If
        End If
    End Sub

    ''' <summary>
    ''' TCM疵拡大画像表示
    ''' </summary>
    ''' <param name="clsPopup"></param>
    ''' <remarks></remarks>
    Public Sub ShowPopupTcm(ByVal typDefInf As TYP_MAE_DEFECT_DATA, ByVal clsPopup As frmDefectImage2)
        Dim clsImg(1 - 1) As Image
        Dim tmpTitle(1 - 1) As String
        Dim nRetc As ValueType = 0
        Dim nImgTyp As Integer = EM_DSP_IMAGE_FORMAT.nJpeg
        Try
            '画像形式取得(0:JPEG、1:BMP)
            nImgTyp = mcls_PreParam.GetProcImgTyp(tMod.ByteToString(typDefInf.ff.cProc))
            '// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
            If KOUTEI_CODE_2TCM = tMod.ByteToString(typDefInf.ff.cProc) Then
                '前工程管理No取得
                Dim strMaeKizukenNo As String = tMod.ByteToString(typDefInf.ff.cMaeKizukenNo)
                Dim strID As String = Nothing
                If strMaeKizukenNo.Length > 2 Then
                    '管理NoよりφωID取得
                    strID = strMaeKizukenNo.Substring(0, 2)
                    '既設2TCM穴検の場合は、無条件でBITMAPをセット。
                    If strID = KIZUKEN_CODE_2TCM Then
                        nImgTyp = 1
                    End If
                End If
            End If
            '// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
            '画像取得
            For ii As Integer = 0 To clsImg.Length - 1
                clsImg(ii) = mcls_DataMap.GetTcmImage(typDefInf.strKanriNo, typDefInf.nKizuNo, typDefInf.data, nImgTyp)
            Next
            '---------------------------
            '疵画像拡大表示
            '---------------------------
            With clsPopup
                .DispImgNo = 1                                      '画像No.
                .SetVisibleMemory(Overlay)                           'メモリの表示有無設定
                .SetVisibleOverlay(Overlay)                         'オーバーレイの表示有無設定
                .SetColor = Color.Cyan                              'メモリ、オーバレイ色

                ''疵情報
                .SetDefectData(typDefInf.nKizuNo, typDefInf.ff, typDefInf.data, clsImg, tmpTitle)
                '表示倍率
                .ZoomRate = CInt((100.0 * typDefInf.data.w_res / m_nTcmImageWide) * 100.0)
                '目盛り描画詳細
                .SetMemoryConfig(TANSHO_SCALE_INTVAL_X, TANSHO_SCALE_NUM_X * 2, TANSHO_SCALE_INTVAL_Y, TANSHO_SCALE_NUM_Y * 2)
                .DspUpDate(True)                                                          '表示
            End With

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM疵拡大画像表示異常[{0}]", ex.Message))
        End Try
    End Sub


End Class
