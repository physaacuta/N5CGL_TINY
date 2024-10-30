'*********************************************************************************
' 疵マップ実績画面
'	[Ver]
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On
Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmDefMapJisseki
    Implements IDisposable                          ' デストラクタ定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 外部イベント
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ファンクションボタン押下イベント (ベースの置き換え)
    ''' </summary>
    ''' <param name="FncNo">0オリジン</param>
    ''' <remarks></remarks>
    Public Shadows Event tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys)
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const nLabelHosei As Integer = 12                                   'ラベル描画位置微調整

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバークラス
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Param As clsParamManager                           'パラメータ管理クラス
    Private mcls_Log As tClass.tClass_LogManager                    'ログ管理クラス

    Private mcls_DspDefCoil As clsDspDefCoil                        'コイル情報表示クラス
    Private mcls_MixedRateInCoil As clsMixedRateInCoil              '入側コイル単位　混入率情報クラス
    Private mcls_MixedRate As clsMixedRate                          'カット単位　混入率情報クラス
    Private mcls_DspDefMap(NUM_MEN - 1) As clsDspDefMap             '疵マップ表示情報クラス

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_bLoad As Boolean = False                  '画面ロード中フラグ
    Private m_bCorrect As Boolean = False               '補正率使用(True:使用、False:不使用)

    Private m_nPageNo As Integer = 0                    '現在表示中のページ番号(0オリジン)
    Private m_nMarkerLabel As Integer                   '表示マーカーラベルID
    Private m_nMarkerColor As Integer                   '表示マーカーカラーID

    Private m_cKizukenNo As String                      ' 出側管理No
    Private m_nCoilLen As Integer                       ' コイル実績長 [mm]
    Private m_nCoilWidth As Integer                     ' 板幅 [mm]

    Private m_nIDCnt As Integer = 0                     'カットID表示位置

    'ファンクションボタン定義
    Private Enum EM_BTN_MODE
        nPre = 0          '一時停止
        nNext             '同期
        nEnd = 11         'マップ変更
    End Enum

    ' コイル情報グリッド列定義
    Private Enum EM_GRID_COIL_COL
        emOrderNo = 0           '命令No
        emProductNo             '生産番号
        emCoilLen               'コイル長
        emSizeZai               '材料
        emHin                   '品種
        emYouto                 '用途
        emNextLine              '次工程
        emBase                  '検査基準

        emColEnd                ' 最終列 ※列数計算用
    End Enum

    ''' <summary>
    ''' 画面表示コンボボックスに表示するデータ
    ''' </summary>
    ''' <remarks>表示名称</remarks>
    Public Class cls_CmbMakerItem

        ''' <summary>nMode</summary>
        Public mode As Integer          'nMode
        ''' <summary>表示名</summary>
        Public name As String           '表示名

        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="nMode">モード</param>
        ''' <param name="strName">表示名</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal nMode As Integer, ByVal strName As String)
            mode = nMode
            name = strName
        End Sub

        ''' <summary>
        ''' 構造体の表示名称
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overrides Function ToString() As String
            Return name
        End Function
    End Class

    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="cKizukenNo">管理No</param>
    ''' <param name="nCoilLen">コイル実績長 [mm]</param>
    ''' <param name="nCoilWidth">板幅 [mm]</param>
    ''' <param name="bCorrect">補正率使用(True:使用、False:不使用)</param>
    ''' <param name="cls_Param">パラメータ管理</param>
    ''' <param name="cls_Log">ログ管理</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal cKizukenNo As String, ByVal nCoilLen As Integer, ByVal nCoilWidth As Integer, ByVal bCorrect As Boolean, ByRef cls_Param As clsParamManager, ByRef cls_Log As tClass_LogManager)


        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        'タイトル設定
        Call SetTitleName(Me)

        'ポップアップ画面 低解像度対応
        Call SetPcHeight(Me)
        Call SetPcWidth(Me)

        Me.FormBorderStyle = Windows.Forms.FormBorderStyle.FixedSingle      ' フォームサイズ変更不可

        '遠隔時
        If NOA_ONLINE <> g_NOA Then
            Me.tProp_Moveable = True                                                ' 画面移動許可
        End If

        mcls_Log = cls_Log              ' ログ管理クラス
        mcls_Param = cls_Param          ' パラメータ管理クラス

        m_cKizukenNo = cKizukenNo       ' 管理No
        m_bCorrect = bCorrect           ' 補正率使用
        m_nCoilLen = nCoilLen           ' コイル実績長 [mm]
        m_nCoilWidth = nCoilWidth       ' 板幅 [mm]
        If COIL_WID_MAX < m_nCoilWidth Then
            m_nCoilWidth = COIL_WID_MAX
        End If

        m_nMarkerLabel = tMod.ReadIniInt(SO_GAMENN, "MARKER_LABEL", LOCALINI_NAME, 0)
        m_nMarkerColor = tMod.ReadIniInt(SO_GAMENN, "MARKER_COLOR", LOCALINI_NAME, 0)

        '表示設定コンボボックス設定
        '疵マーカーラベル
        cmbMarkerLabel.Items.Add(New cls_CmbMakerItem(EM_MARKER_MODE.nMarkerModeNone, "なし"))
        cmbMarkerLabel.Items.Add(New cls_CmbMakerItem(EM_MARKER_MODE.nMarkerModeType, "疵種"))
        cmbMarkerLabel.Items.Add(New cls_CmbMakerItem(EM_MARKER_MODE.nMarkerModeGrade, "ｸﾞﾚｰﾄﾞ"))
        cmbMarkerLabel.SelectedIndex = m_nMarkerLabel

        '疵マーカー色替え
        cmbMarkerColor.Items.Add(New cls_CmbMakerItem(EM_MARKER_COLOR_MODE.nMarkerColorModeType, "疵種"))
        cmbMarkerColor.Items.Add(New cls_CmbMakerItem(EM_MARKER_COLOR_MODE.nMarkerColorModeGrade, "ｸﾞﾚｰﾄﾞ"))
        cmbMarkerColor.SelectedIndex = m_nMarkerColor

        'クラス設定
        Call CreateCoilInf()        'コイル情報クラス
        Call CreateMixRateInCoil()  '入側コイル単位　疵混入率情報管理クラス
        Call CreateMixRate()        'カット単位　疵混入率情報管理クラス
        Call CreateDefMap()         '疵マップ表示情報管理

        '初期表示設定
        DspInitProcess()

        ' ボタン定義
        MyBase.AddFunctionButton(btnPre, EM_BTN_MODE.nPre)
        MyBase.AddFunctionButton(btnNext, EM_BTN_MODE.nNext)
        MyBase.AddFunctionButton(btnEnd, EM_BTN_MODE.nEnd)

        '-------------------------------------------------------------------
        ' 疵マップ周囲・目盛りの描画
        '-------------------------------------------------------------------
        '画面初期表示（空白）
        Call DspInit(True)

        For ii As Integer = 0 To mcls_DspDefMap.Length - 1
            mcls_DspDefMap(ii).tProp_nCoilLen = m_nCoilLen
            mcls_DspDefMap(ii).tProp_nCoilWid = m_nCoilWidth
            mcls_DspDefMap(ii).DrawKizuMapFrame()
            mcls_DspDefMap(ii).SetDrawPaint()
        Next

        m_bLoad = False
        Application.DoEvents()

        '疵マップ実績画面表示（タイマーで起動）
        tmrDsp.Enabled = True

    End Sub

    ''' <summary>
    ''' 設定値取得
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspInitProcess()
        Try
            '疵マップコイル向き(表裏共通)
            mcls_DspDefMap(0).tProp_MapDir = CBool(IIf(0 = tMod.ReadIniInt(SO_GAMENN, "MAP_SCROLL_DIRECT", TASKINI_NAME), True, False))
            mcls_DspDefMap(1).tProp_MapDir = CBool(IIf(0 = tMod.ReadIniInt(SO_GAMENN, "MAP_SCROLL_DIRECT", TASKINI_NAME), True, False))

            '疵マップ左右反転（表）
            mcls_DspDefMap(0).tProp_TurnLR = CBool(IIf(0 = tMod.ReadIniInt(SO_GAMENN, "DEFMAP_SIDE_TOP", TASKINI_NAME), False, True))

            '疵マップ左右反転（裏）
            mcls_DspDefMap(1).tProp_TurnLR = CBool(IIf(0 = tMod.ReadIniInt(SO_GAMENN, "DEFMAP_SIDE_BOT", TASKINI_NAME), False, True))


            '画面で設定した疵マーカーラベルコンボボックスのIndex値
            Select Case m_nMarkerLabel
                Case EM_MARKER_MODE.nMarkerModeType
                    mcls_DspDefMap(0).tProp_MarkerLabel = EM_MARKER_MODE.nMarkerModeType
                    mcls_DspDefMap(1).tProp_MarkerLabel = EM_MARKER_MODE.nMarkerModeType
                Case EM_MARKER_MODE.nMarkerModeGrade
                    mcls_DspDefMap(0).tProp_MarkerLabel = EM_MARKER_MODE.nMarkerModeGrade
                    mcls_DspDefMap(1).tProp_MarkerLabel = EM_MARKER_MODE.nMarkerModeGrade
                Case Else
                    mcls_DspDefMap(0).tProp_MarkerLabel = EM_MARKER_MODE.nMarkerModeNone
                    mcls_DspDefMap(1).tProp_MarkerLabel = EM_MARKER_MODE.nMarkerModeNone
            End Select

            '画面で設定した疵マーカーカラーコンボボックスのIndex値
            Select Case m_nMarkerColor
                Case EM_MARKER_COLOR_MODE.nMarkerColorModeType
                    mcls_DspDefMap(0).tProp_MarkerColor = EM_MARKER_COLOR_MODE.nMarkerColorModeType
                    mcls_DspDefMap(1).tProp_MarkerColor = EM_MARKER_COLOR_MODE.nMarkerColorModeType
                Case Else
                    mcls_DspDefMap(0).tProp_MarkerColor = EM_MARKER_COLOR_MODE.nMarkerColorModeGrade
                    mcls_DspDefMap(1).tProp_MarkerColor = EM_MARKER_COLOR_MODE.nMarkerColorModeGrade
            End Select
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("設定値取得異常[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' コイル情報クラス生成
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CreateCoilInf()
        Try
            Dim typCtrl As clsDspDefCoil.TYP_COIL_DSP = Nothing

            With typCtrl
                .dgvCoilInf = dgvCoilInf
            End With

            mcls_DspDefCoil = New clsDspDefCoil(typCtrl, mcls_Log)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("コイル情報クラス生成異常[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' 入側コイル単位　混入率情報管理クラス生成
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CreateMixRateInCoil()
        Try
            Dim typCtrl As clsMixedRateInCoil.RATE_CONTROL_DATA = Nothing

            '混入率情報セット
            With typCtrl
                .dgvMixedRateIn = dgvMixedRateIn     '混入率データグリッド
                .lblNgMsg = lblNgMsg                 '異常ガイダンス表示用
                .bCorrect = m_bCorrect               '
            End With

            mcls_MixedRateInCoil = New clsMixedRateInCoil(typCtrl, mcls_Log)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("入側コイル単位　混入率情報管理クラス生成異常[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' カット単位　混入率情報管理クラス生成
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CreateMixRate()
        Try
            Dim typCtrl As clsMixedRate.RATE_CONTROL_DATA = Nothing

            '混入率情報セット
            With typCtrl
                .lblID = lblCutNo                    'カット実績のID
                .dgvMixedRate = dgvMixedRate         '混入率データグリッド
                .lblNgMsg = lblNgMsg                 '異常ガイダンス表示用
                .btnPre = btnPre                     '疵混入率前項
                .btnNext = btnNext                   '混入率次項
            End With

            mcls_MixedRate = New clsMixedRate(typCtrl, mcls_Log)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("混入率情報管理クラス生成異常[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' 疵マップ情報管理クラス生成
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CreateDefMap()
        Try
            ReDim mcls_DspDefMap(NUM_MEN - 1)

            Dim typMapInfO As clsDspDefMap.TYP_MAP_CTRL = Nothing
            With typMapInfO
                .picMap = picMapTop
                .lblDs = lblDs_O
                .lblWs = lblWs_O
                '疵深さリスト
            End With
            mcls_DspDefMap(0) = New clsDspDefMap(typMapInfO, 0, mcls_Param, mcls_Log)

            Dim typMapInfU As clsDspDefMap.TYP_MAP_CTRL = Nothing
            With typMapInfU
                .picMap = picMapBot
                .lblDs = lblDs_U
                .lblWs = lblWs_U
            End With
            mcls_DspDefMap(1) = New clsDspDefMap(typMapInfU, 1, mcls_Param, mcls_Log)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ情報管理クラス生成異常[{0}]", ex.Message))
        End Try
    End Sub



    ''' <summary>
    ''' コイル情報設定
    ''' </summary>
    ''' <param name="data"></param>
    ''' <remarks></remarks>
    Public Sub SetCoilData(ByVal data As clsDataMap.TYP_COIL_INF)
        Try
            'm_typCoil = data
            mcls_DspDefCoil.tProp_typCoil = data
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ画面 カット位置情報設定異常 [{0}] ", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' 疵情報設定
    ''' </summary>
    ''' <param name="nMen">0:表 1:裏</param>
    ''' <param name="lstData">疵情報</param>
    ''' <remarks></remarks>
    Public Sub SetDefectData(ByVal nMen As Integer, ByRef lstData As List(Of DETECT_BASE_DATA))
        Try
            lstData.Sort(AddressOf SortUnPriorty)
            mcls_DspDefMap(nMen).SetDefectData(lstData)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ画面 疵情報設定異常 [{0}] Men=[{1}]", ex.Message, nMen))
        End Try
    End Sub

    ''' <summary>
    ''' カット位置情報設定
    ''' </summary>
    ''' <param name="lstData"></param>
    ''' <remarks></remarks>
    Public Sub SetCutData(ByRef lstData As List(Of CUT_INF_DATA))
        Try
            For ii As Integer = 0 To mcls_DspDefMap.Length - 1
                mcls_DspDefMap(ii).SetCutData(lstData)
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ画面 カット位置情報設定異常 [{0}] ", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' コイル反転有無設定
    ''' </summary>
    ''' <param name="bTurn"></param>
    ''' <remarks></remarks>
    Public Sub SetTurnTB(ByVal bTurn As Boolean)
        Try
            '疵マップ上下反転（表）
            mcls_DspDefMap(0).tProp_TurnTB = bTurn

            '疵マップ上下反転（裏）
            mcls_DspDefMap(1).tProp_TurnTB = bTurn
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ画面 コイル反転有無設定異常 [{0}] ", ex.Message))
        End Try
    End Sub

    '***************************************************************************
    ' フォームイベント
    '***************************************************************************

    ''' <summary>
    ''' この画面の資源解放イベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDefectMap_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        Try

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ画面 資源解放異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 初期化処理
    ''' </summary>
    ''' <param name="blnExec">ボタン無効判定(True:無効化、False:ボタン無効化無し)</param>
    ''' <remarks></remarks>
    Private Sub DspInit(ByVal blnExec As Boolean)
        Dim ii As Integer

        '画面基本情報初期表示
        Call mcls_DspDefCoil.DspInit()

        '入側コイル単位　疵混入率初期表示
        Call mcls_MixedRateInCoil.DspInit()
        Call mcls_MixedRateInCoil.ResizeMixedRate()

        'カット単位　疵混入率初期表示
        Call mcls_MixedRate.DspInit()
        Call mcls_MixedRate.ResizeMixedRate()

        '疵マップ初期表示
        For ii = 0 To NUM_MEN - 1
            mcls_DspDefMap(ii).tProp_nCoilLen = 0          '初期表示のコイル長は0とする
            mcls_DspDefMap(ii).tProp_nCoilWid = 0          '初期表示のコイル幅は0とする。
            mcls_DspDefMap(ii).DspInit()
            mcls_DspDefMap(ii).SetDrawPaint()
        Next ii

        'ボタン無効化処理
        If blnExec Then
            btnPre.Enabled = Not blnExec           '前ページボタン
            btnNext.Enabled = Not blnExec          '次ページボタン
        End If

    End Sub

    ''' <summary>
    ''' データ表示
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspData()

        Dim bRet As Boolean

        'ボタン無効化
        btnPre.Enabled = False            '前ページボタン
        btnNext.Enabled = False           '次ページボタン

        '異常ガイダンス表示初期化
        DspNgGuid(False)

        Call mcls_DspDefCoil.DspData()

        '入側コイル単位　疵混入率情報表示
        bRet = mcls_MixedRateInCoil.DspData(m_cKizukenNo)
        If bRet = False Then
            '画面異常ガイダンス表示
            Call DspNgGuid(True)
        End If

        '★疵混入率情報表示
        'ガイダンス表示
        'EnableControl(True)
        'Call DspWaitInf("カット情報取得中", 1)
        '対象疵検NoのカットIDを全て取得
        bRet = mcls_MixedRate.GetCutID(m_cKizukenNo)
        'Call DspWaitProg(1)

        If bRet = False Then
            '疵混入率情報取得NG時は異常ガイダンス表示を行う。
            'ガイダンス非表示
            EnableControl(False)

            '画面異常ガイダンス表示
            Call DspNgGuid(True)
        Else
            '初めのカットID番号をセット
            mcls_MixedRate.tProp_IDCnt = m_nIDCnt

            '疵混入率情報表示
            Call DspRate()
        End If
        For ii As Integer = 0 To mcls_DspDefMap.Length - 1
            mcls_DspDefMap(ii).SetDrawPaint()
        Next
        'ガイダンス非表示
        EnableControl(False)

    End Sub

    ''' <summary>
    ''' 初期化処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspRate()

        Dim bRet_Rate As Boolean     '混入率表示判定

        'Call DspWaitInf("疵混入率一覧情報取得中", 1)
        '疵混入率情報表示
        bRet_Rate = mcls_MixedRate.DspRateData(m_cKizukenNo)
        'Call DspWaitProg(1)

        '疵混入率情報取得NG時は異常ガイダンス表示を行う。
        If bRet_Rate = False Then
            '画面異常ガイダンス表示
            Call DspNgGuid(True)
        End If

    End Sub

    ''' <summary>
    ''' フォームクローズ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDefMapJisseki_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        e.Cancel = True
        Me.Visible = False
    End Sub

    '''' <summary>
    '''' 疵混入率前項ボタンクリック
    '''' </summary>
    '''' <param name="sender"></param>
    '''' <param name="e"></param>
    '''' <remarks></remarks>
    'Private Sub btnPre_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnPre.Click

    '    'ボタンが無効の場合は終了する。
    '    If btnPre.Enabled = False Then Exit Sub

    '    '前ページ処理
    '    Call ChangePage(True)

    'End Sub

    '''' <summary>
    '''' 疵混入率次項ボタンクリック
    '''' </summary>
    '''' <param name="sender"></param>
    '''' <param name="e"></param>
    '''' <remarks></remarks>
    'Private Sub btnNext_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnNext.Click

    '    'ボタンが無効の場合は終了する。
    '    If btnNext.Enabled = False Then Exit Sub

    '    '前ページ処理
    '    Call ChangePage(False)

    'End Sub

    ''' <summary>
    ''' 前ページ・次ページボタン押下時処理
    ''' </summary>
    ''' <param name="btn">True:前ページ False:次ページ</param>
    ''' <remarks></remarks>
    Private Sub ChangePage(ByVal btn As Boolean)

        '前ページ押下時はレコード番号を減算し、次ページ押下時は加算する
        m_nIDCnt += CInt(IIf(True = btn, -1, 1))

        'カットID番号をセット
        mcls_MixedRate.tProp_IDCnt = m_nIDCnt

        '異常ガイダンス表示初期化
        DspNgGuid(False)

        ''ガイダンス表示
        'EnableControl(True)

        '疵混入率情報表示
        Call DspRate()

        'ガイダンス非表示
        EnableControl(False)

    End Sub

    ''' <summary>
    ''' 閉じるボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnEnd.Click

        Me.Close()

    End Sub

    ''' <summary>
    ''' 疵マーカーラベルコンボボックス値変更時はiniファイルに設定値を保存する
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbMarkerLabel_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cmbMarkerLabel.SelectedIndexChanged

        If m_bLoad Then Exit Sub

        'コンボボックス選択が現在選択値と変わらない場合、処理を抜ける
        If m_nMarkerLabel = CType(cmbMarkerLabel.Items(cmbMarkerLabel.SelectedIndex), cls_CmbMakerItem).mode Then Exit Sub

        'コンボボックス設定値保存
        m_nMarkerLabel = CType(cmbMarkerLabel.Items(cmbMarkerLabel.SelectedIndex), cls_CmbMakerItem).mode
        tMod.WriteIni(SO_GAMENN, "MARKER_LABEL", LOCALINI_NAME, CStr(m_nMarkerLabel))

        For ii As Integer = 0 To mcls_DspDefMap.Length - 1
            mcls_DspDefMap(ii).tProp_MarkerLabel = CType(m_nMarkerLabel, EM_MARKER_MODE)
            mcls_DspDefMap(ii).SetDrawPaint
        Next
        Me.Refresh()

    End Sub

    ''' <summary>
    ''' 疵マーカー色替えコンボボックス値変更時はiniファイルに設定値を保存する
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbMarkerColor_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cmbMarkerColor.SelectedIndexChanged

        If m_bLoad Then Exit Sub

        'コンボボックス選択が現在選択値と変わらない場合、処理を抜ける
        If m_nMarkerColor = CType(cmbMarkerColor.Items(cmbMarkerColor.SelectedIndex), cls_CmbMakerItem).mode Then Exit Sub

        'コンボボックス設定値保存
        m_nMarkerColor = CType(cmbMarkerColor.Items(cmbMarkerColor.SelectedIndex), cls_CmbMakerItem).mode
        tMod.WriteIni(SO_GAMENN, "MARKER_COLOR", LOCALINI_NAME, CStr(m_nMarkerColor))

        For ii As Integer = 0 To mcls_DspDefMap.Length - 1
            mcls_DspDefMap(ii).tProp_MarkerColor = CType(m_nMarkerColor, EM_MARKER_COLOR_MODE)
            mcls_DspDefMap(ii).SetDrawPaint()
        Next
        Me.Refresh()

    End Sub

    ''' <summary>
    ''' データ取得中
    ''' </summary>
    ''' <param name="blnExec"></param>
    ''' <remarks></remarks>
    Private Sub EnableControl(ByVal blnExec As Boolean)

        'データ取得中。。。
        pnlMsg.Visible = blnExec
        grbGuid.Visible = blnExec

        'ガイダンス表示初期化
        If Not blnExec Then
            lblMsg.Text = ""
            prbMsg.Value = 0
        End If
        'リフレッシュイベントで表示
        pnlMsg.Refresh()
        grbGuid.Refresh()
        lblMsg.Refresh()
        prbMsg.Refresh()

    End Sub

    ''' <summary>
    ''' 異常結果ガイダンス表示
    ''' </summary>
    ''' <param name="blnExec"></param>
    ''' <remarks></remarks>
    Private Sub DspNgGuid(ByVal blnExec As Boolean)

        '異常結果ガイダンス
        grbNgGuid.Visible = blnExec
        If Not blnExec Then lblNgMsg.Text = ""

        'リフレッシュイベントで表示
        grbNgGuid.Refresh()
        lblNgMsg.Refresh()

    End Sub

    ''' <summary>
    ''' データ取得状況表示中
    ''' </summary>
    ''' <param name="strVal"></param>
    ''' <param name="iMax"></param>
    ''' <remarks></remarks>
    Private Sub DspWaitInf(ByVal strVal As String, ByVal iMax As Long)
        lblMsg.Text = strVal
        prbMsg.Minimum = 0
        prbMsg.Maximum = CInt(IIf(iMax > prbMsg.Minimum, iMax, prbMsg.Minimum + 1))
        prbMsg.Value = 0

        'リフレッシュイベントで表示
        lblMsg.Refresh()
        prbMsg.Refresh()

    End Sub

    ''' <summary>
    ''' データ取得中ガイダンス 更新
    ''' </summary>
    ''' <param name="nVal">進捗値</param>
    ''' <remarks></remarks>
    Private Sub DspWaitProg(ByVal nVal As Integer)

        '進捗値セット
        prbMsg.Value = CInt(IIf(nVal < prbMsg.Maximum, nVal, prbMsg.Maximum))

    End Sub

    ''' <summary>
    ''' 疵マップ実績画面表示（画面起動時のダイヤログ表示対応）
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub tmrDsp_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles tmrDsp.Tick

    End Sub

    ''' <summary>
    ''' ファンクションボタン押下イベント
    ''' </summary>
    ''' <param name="FncNo">0オリジン</param>
    ''' <param name="onKey">ボタンクリック時も取れるよ</param>
    ''' <remarks></remarks>
    Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton

        '' ファンクション切替
        Select Case FncNo                       ' 0オリジン
            Case 0                              ' 疵混入率前項
                'ボタンが無効の場合は終了する。
                If btnPre.Enabled = False Then Exit Sub

                '前ページ処理
                Call ChangePage(True)
            Case 1                              ' 疵混入率次項
                'ボタンが無効の場合は終了する。
                If btnNext.Enabled = False Then Exit Sub

                '前ページ処理
                Call ChangePage(False)
            Case 11                             ' 終了
                Me.Close()
        End Select

        '呼び元へ通知
        RaiseEvent tEv_FunctionButton(FncNo, onKey)

    End Sub


    ''' <summary>
    ''' 画面リサイズ処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDefMapJisseki_Resize(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Resize
        ' 画面ロード中イベントは無視
        If m_bLoad Then Exit Sub

        Try
            Me.SuspendLayout()

            ' コイル情報表示調整
            Call mcls_DspDefCoil.ResizeGridCoil()

            ' 入側コイル単位　疵混入率 表示調整
            Call mcls_MixedRateInCoil.ResizeMixedRate()

            ' カット単位　疵混入率 表示調整
            Call mcls_MixedRate.ResizeMixedRate()

            For ii As Integer = 0 To mcls_DspDefMap.Length - 1
                mcls_DspDefMap(ii).DrawKizuMapFrame()
            Next

            Me.Refresh()

        Catch ex As Exception

        Finally
            Me.ResumeLayout()
        End Try
    End Sub

    ''' <summary>
    ''' 優先順位の低い順によるソート関数
    ''' </summary>
    ''' <param name="x"></param>
    ''' <param name="y"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function SortUnPriorty(ByVal x As DETECT_BASE_DATA, ByVal y As DETECT_BASE_DATA) As Integer
        ' 優先順位
        ' ① グレード優先度
        ' ② 疵種優先度
        ' ③ 面積順
        ' ④ 長手順
        ' ⑤ 疵No
        Dim dBot(5) As Double
        Dim dPer(5) As Double
        Dim bMode(5) As Boolean


        dBot(0) = CDbl(x.nGyuu)
        dPer(0) = CDbl(y.nGyuu)
        bMode(0) = True

        dBot(1) = CDbl(x.nTyuu)
        dPer(1) = CDbl(y.nTyuu)
        bMode(1) = True

        dBot(2) = CDbl(x.aria)
        dPer(2) = CDbl(y.aria)
        bMode(2) = False

        dBot(3) = CDbl(x.y)
        dPer(3) = CDbl(y.y)
        bMode(3) = True

        dBot(4) = CDbl(x.nKizuNo)
        dPer(4) = CDbl(y.nKizuNo)
        bMode(4) = False

        If dBot(0) = dPer(0) And dBot(1) = dPer(1) And dBot(2) = dPer(2) And dBot(3) = dPer(3) And dBot(4) = dPer(4) Then
            Return 0
        End If
        ' 優先度比較
        If Not SelectPriorityDetect(dBot, dPer, bMode) Then
            Return 1
        Else
            Return -1
        End If
    End Function

    '********************************************************************
    ' C++の使いやすいモジュールの展開
    '********************************************************************
    ''' <summary>
    ''' 欠陥優先度比較 Ex (各変数配列に情報をセット)
    ''' </summary>
    ''' <param name="pNow">比較する新情報を順番にセット(配列の先頭が最優先) </param>
    ''' <param name="pPre">比較する旧情報を順番にセット(配列の先頭が最優先) </param>
    ''' <param name="bMode">比較条件 (true:新旧比較したときに大きい方が強い。 false:新旧比較したときに小さい方が強い)</param>
    ''' <returns>true:新欠陥の方が強い  false:旧欠陥の方が強い</returns>
    Public Function SelectPriorityDetect(ByVal pNow() As Double, ByVal pPre() As Double, ByVal bMode() As Boolean) As Boolean
        For ii As Integer = 0 To pNow.Length - 1
            If pNow(ii) = pPre(ii) Then Continue For

            If bMode(ii) Then
                '// 大きい方が強い
                If pNow(ii) > pPre(ii) Then
                    Return True
                Else
                    Return False
                End If
            Else
                '// 小さい方が強い
                If pNow(ii) < pPre(ii) Then
                    Return True
                Else
                    Return False
                End If
            End If
        Next
        Return False    '// 全部一緒なら 旧欠陥を優先
    End Function

End Class
