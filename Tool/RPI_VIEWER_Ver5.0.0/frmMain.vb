'Option Strict On	'共有メモリアクセスがあるため、Off化
Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports System.Reflection
Imports System.Collections.Generic

Imports LabelInfo = tClassLibrary.tClass.tClass_ImageDspAxisDrawner.LabelInfo


Public Class frmMain
    Implements RpiPlugin.IPropertiesProvider

    Private m_plugins As New RpiPlugin.RpiPluginManager
    Private m_active_mouse_handler As RpiPlugin.IMouseToolStrip

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private WithEvents mcls_RpiView As New tClass_ViewRpiThread                     ' RPIファイル表示クラス
    Private WithEvents mcls_ImgCutDsp As tClass_ImageRawCutDsp                      ' 画像部分拡大クラス
    Private WithEvents mcls_CheckKizu As clsCheckKizuWT = Nothing                   ' 減速、停止管理クラス (本体)
    Private mcls_Param As New clsParamManager                                       ' パラメータ管理クラス (本体)

    Private m_bDebug As Boolean = False                                             ' 開発中フラグ (開発時true)


    '' 縦横表示
    Public scrNagate As ScrollBar                                                   ' 長手側のスクロールバー（縦横可変！）
    Public scrHaba As ScrollBar                                                     ' 幅方向のスクロールバー（縦横可変！）

    ' 画像処理用フラグ
    Private mcls_StopWatch As New tClass_PerformanceCounter                         ' フレームレート算出用
    Private m_bDrawKizuOvl As Boolean = True                                        ' 疵オーバーレイ描画有無
    Private m_bDrawMemori As Boolean = True                                         ' メモリ描画有無
    Private m_bDrawEdge As Boolean = False                                          ' エッジ線描画有無
    Private m_sDrawStrLR(2) As String                                               ' 左右文字

    '' (疵接近ランプ)
    Private mcnt_Ranp(MC_RANP_CNT - 1) As Control                                   ' 疵接近ランプ コントロール配列


    '' 実績関係機能 
    Private m_bDb As Boolean = False                                                ' DBアクセス有無
    Private m_bResutl As Boolean = False                                            ' DB実績有無 (これがなければ、実績関係は表示しない)
    Private m_typCoil As DATA_COIL                                                  ' コイル情報
    Private m_lstDetect As New List(Of DATA_DETECT_INF)                             ' 疵情報リスト
    Private m_lstCut As New List(Of DATA_CUT_INF)                                   ' カット情報リスト

    '' 拡張機能 
    'Private WithEvents mcls_RecvMail As clsRecvMail = Nothing						' メールスロット受信
    Private WithEvents mcls_MailSlotServer As MailSlotServer(Of CommonQueMix)

    Private mfrm_Kiri As New frmPopKiriImg                                          ' 切り出し画像表示画面
    Private WithEvents mfrm_DefMap As New frmPopDefMap                              ' 散布図形式の全体マップ
    Private WithEvents mfrm_KizuMap As New frmPopKizuMap                            ' 疵マップ形式の全体マップ
    Private WithEvents mfrm_KizuList As New frmPopKizuList                          ' 疵一覧

    'Private WithEvents mfrm_ImgCont As New frmPopImgControl							' 輝度コントロール画面
    Private WithEvents mfrm_Kai As New frmPopKaihatu                                ' 開発用画面
    Private WithEvents mfrm_Search As New frmPopCoilSearch                          ' コイル検索画面
    Private WithEvents mfrm_GridLine As New frmPopGridLine                          ' グリッド線
    Private WithEvents mfrm_FrmCont As New frmPopControl                            ' ジャンプ画面


    Private WithEvents mfrm_DspSet As New frmPopDspSet                              ' 表示設定画面
    Private WithEvents mfrm_ReSetOvl As New frmPopResetOvl                          ' オーバーレイ手動補正画面

    Private mfrm_CoilItem As frmPopCoilItem                                         ' コイル情報一覧
    Private mfrm_Print As New frmPopPrint                                           ' 出力設定
    Private mfrm_KiriInput As New frmPopKiriInput                                   ' 切り出し画像生成
    Private WithEvents mfrm_Bookmark As New frmPopBookmark                          ' マーキング機能
    Private WithEvents mfrm_InputList As New frmPopInputList                        ' 入力一覧機能
    Private WithEvents mfrm_Setting As New frmPopSetting                            ' 設定画面

    '' 複数連動
    Private m_memTrkPtr As IntPtr = IntPtr.Zero                         ' 共有メモリポインタ(他工程間連携)
    Private m_memSyncPtr As IntPtr = IntPtr.Zero                        ' 共有メモリポインタ(簡易連携)
    Private m_nSyncOffsetY As Integer                                   ' 簡易連携ボタン押下時の左上位置[mm]
    Private m_nSyncOffsetX As Integer                                   ' 簡易連携ボタン押下時の左上位置[mm]

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 共通
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        'タイトルにRpiViewer.exe Assemblyのバージョンを足す。３つ。minorrevisionは無視。
        Dim ver As Version = Assembly.GetExecutingAssembly().GetName().Version
        Me.Text &= String.Format(" {0}.{1}.{2}", ver.Major, ver.Minor, ver.Build)


        ' 多重起動時には、最初のヤツしかダメ
        Try
            gcls_Log = New tClass.tClass_LogManager("RPI_VIEWER", , , 0, 0)
            gcls_Log.Write(tClass_LogManager.LNO.MSG, "起動")
        Catch ex As Exception
        End Try

        '===============================================
        ' 基本
        m_bDebug = IIf(0 = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "DEBUG", TASKINI_NAME), False, True)

        '初めは回転なし
        scrHaba = Me.hsBar
        scrNagate = Me.vsBar
        scrNagate.Maximum = 500         ' LargeChange以上の値を入れておかないと、勝手にLargeChangeを小さくしてしまう
        scrNagate.SmallChange = 100
        scrNagate.LargeChange = 500

        'APバージョン
        If IntPtr.Size = 8 Then         '64bit環境
            Me.Text &= " (64bit環境モード)"
        Else                            '32bit環境
            Me.Text &= " (32bit環境モード)"
        End If

        If g_bOutTrk Then
            Me.Text &= " (外部連携モード)"
        End If


        '===============================================
        ' 拡張機能
        '   拡張機能は、デフォルト表示禁止

        '''' 横向き表示
        g_nLockMode = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "LOCK", TASKINI_NAME)
        If 0 <> g_nLockMode AndAlso 1 <> g_nLockMode AndAlso 9 <> g_nLockMode Then g_nLockMode = 0
        If 0 = g_nLockMode Then     ' 非表示設定
            tsbRot.Visible = False
        End If


        '''' オプション機能表示非表示
        ' 簡易連動
        tsbSync.Visible = False     ' 簡易連動
        ' 疵接近ランプ
        If 0 = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "OPT_RANP", TASKINI_NAME) Then        ' 疵接近ランプ
            tlpRanp.Visible = False
        Else
            tlpRanp.Visible = True
        End If
        ' ジャンプ画面のみ。ファンクションは有効
        If 0 = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "OPT_JUMP", TASKINI_NAME) Then
            tsbJump.Visible = False
        Else
            tsbJump.Visible = True
        End If
        ' 任意疵選択
        If 0 = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "OPT_KIRIINPUT", TASKINI_NAME) Then
            tsbKiriInput.Visible = False
        Else
            tsbKiriInput.Visible = True
        End If
        ' 各種設定画面
        If 0 = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "OPT_SETTING", TASKINI_NAME) Then
            tsbSetting.Visible = False
        Else
            tsbSetting.Visible = True
        End If
        ' 位置選択(手入力)
        If 0 = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "OPT_INTPU_LIST", TASKINI_NAME) Then
            tsbInputList.Visible = False
        Else
            tsbInputList.Visible = True
        End If
        mfrm_Search.tProp_InputLine = tsbInputList.Visible
        mfrm_KiriInput.tProp_InputLine = tsbInputList.Visible

        '===============================================
        ' メールスロット機能
        If g_bOutTrk Then
            mcls_MailSlotServer = New MailSlotServer(Of CommonQueMix)(g_typStartArgs.sTaskName)
            Me.Text &= " (" & g_typStartArgs.sTaskName & ")"
        Else
            If 1 = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "MAIL", TASKINI_NAME) Then
                g_typStartArgs.sTaskName = TASK_NAME    ' デフォルト名称を設定

                mcls_MailSlotServer = New MailSlotServer(Of CommonQueMix)(g_typStartArgs.sTaskName)
                Me.Text &= " (" & g_typStartArgs.sTaskName & ")"
            End If
        End If

        If mcls_MailSlotServer IsNot Nothing Then
            mcls_MailSlotServer.ReadStart(Me)
        End If

        ' '' ▲
        ''g_bOutTrk = True
        'g_typStartArgs.sTaskName = "RPIV0"
        'mcls_RecvMail = New clsRecvMail(Me)




        '===============================================
        ' 共有メモリ連動
        If g_bOutTrk Then
            '''' 共有メモリ確保 (未生成なら自動で生成。生成済みなら接続だけ)
            m_memTrkPtr = tClass_GlovalMemory.AllocMapped(SIZE_TYPE_TRK, TBL_RPI_TRK_NAME)

            '''' 外部連動時の無効化類
            tsbSync.Enabled = False
            tsbSearch.Enabled = False
            tsbSearchUp.Enabled = False
            tsbSearchDown.Enabled = False
            tsbSearchOU.Enabled = False
            tsbFolder.Enabled = False

        End If




        '===============================================
        '''' RPI表示クラス定義
        ' 疵無し画像をセット
        Try
            Dim asmData As System.Reflection.Assembly = [Assembly].GetExecutingAssembly()           ' アセンブリにアクセス
            Dim semImage As IO.Stream = asmData.GetManifestResourceStream("RPI_VIEWER.疵無し.bmp") ' 埋め込まれたリソースにアクセス

            Call mcls_RpiView.SetItem_DefImg(New Bitmap(semImage))                                         ' デフォルト画像セット

        Catch ex As Exception
            'If Not mcls_Log Is Nothing Then mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "[tClass_ViewRPI] Alloc " & ex.ToString)
        End Try


        picFrameImg.BackColor = Color.Black     ' かなり有効


        ' ライブラリ内でイベントハンドラ追加後に 独自に追加するもの１ (コール順番の関係を大切に) ライブラリ前
        'AddHandler picFrameImg.Resize, AddressOf picImg_ReSize
        'AddHandler Application.Idle, AddressOf idle


        '===============================================
        ' 受け渡し用コントロール
        '' コントロールの設定が一番最後
        Dim cntl As tClass_ViewRpiThread.CNTL_ITEM
        cntl.picDspImg = picFrameImg
        cntl.vsBar = vsBar
        cntl.hsBar = hsBar

        '''''''''''''''''''''''''''''''''
        ' RPIファイル読込みクラス生成
        '' スレッドの数決定
        Dim nCpuCnt As Integer = Environment.ProcessorCount - 1 ' CPUの数(※最低1スレッドは、画面描画ように確保しておく)
        Dim nThreadNum As Integer = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "EXEC_THREAD_NUM", TASKINI_NAME)
        If nThreadNum > nCpuCnt Then nThreadNum = nCpuCnt
        If 1 > nThreadNum Then nThreadNum = 1

        ' 品質決定
        Dim bIsUseImgLibCLI As Boolean = CBool(tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "DRAW_MODE", TASKINI_NAME))
        Dim nQuarity As Integer = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "DRAW_QUARITY", TASKINI_NAME)
        'If 0 = g_nLockMode Then
        '	bIsUseImgLibCLI = False
        '	nQuarity = 0
        'End If

        'カスタムデコーダ追加
        Call mcls_RpiView.AddDecoders(m_plugins.Decoders)
        ' クラス生成
        Call mcls_RpiView.Alloc(cntl, nThreadNum, nQuarity, bIsUseImgLibCLI)

        ' 先読みバッファ定義
        mcls_RpiView.tProp_ResvNum(0) = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "RESV_NUM_PER", TASKINI_NAME)
        mcls_RpiView.tProp_ResvNum(1) = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "RESV_NUM_NEXT", TASKINI_NAME)



        '''''''''''''''''''''''''''''''''
        ' 疵接近ランプコントロールセット
        mcnt_Ranp(0) = pnlLed1
        mcnt_Ranp(1) = pnlLed2
        mcnt_Ranp(2) = pnlLed3
        mcnt_Ranp(3) = pnlLed4
        mcnt_Ranp(4) = pnlLed5
        mcnt_Ranp(5) = pnlLed6
        mcnt_Ranp(6) = pnlLed7
        mcnt_Ranp(7) = pnlLed8
        mcnt_Ranp(8) = pnlLed9


        '' ライブラリ内でイベントハンドラ追加後に 独自に追加するもの (コール順番の関係を大切に)
        'AddHandler picFrameImg.Resize, AddressOf picImg_ReSize
        'AddHandler Application.Idle, AddressOf idle


        '===============================================
        ' 一般クラス (イベント発生順番に影響するため、記述順番を考慮すること)
        ' 最優先イベント
        AddHandler picFrameImg.Click, AddressOf picFrameImg_Click

        '''' 減速、停止管理クラス
        mcls_CheckKizu = New clsCheckKizuWT
        Call mcls_CheckKizu.InitCls_Param(mcls_Param)
        Call mcls_CheckKizu.InitCls_RpiView(mcls_RpiView)
        Call mcls_CheckKizu.InitFrm_Control(mfrm_FrmCont)
        Call mcls_CheckKizu.InitData_LstDetect(m_lstDetect)

        '''' 画像部分拡大クラスの準備
        mcls_ImgCutDsp = New tClass_ImageRawCutDsp(Me, picFrameImg)
        mcls_ImgCutDsp.DefRitu = 3.0
        Dim memoriinf As New tClass_ImageRawCutDsp.MEMORI_INF
        memoriinf.initialize()
        mcls_ImgCutDsp.MemoriInf = memoriinf

        '''' 切り出し画像表示画面
        Call mfrm_Kiri.SetCls_Param(mcls_Param)

        '''' 散布図 全長マップ
        Call mfrm_DefMap.SetCls_Param(mcls_Param)
        Call mfrm_DefMap.SetCls_RpiView(mcls_RpiView)
        Call mfrm_DefMap.SetData_LstDetect(m_lstDetect)
        Call mfrm_DefMap.SetData_LstCut(m_lstCut)
        AddHandler mfrm_DefMap.tEv_MapClick, AddressOf SubMapClick
        AddHandler mfrm_DefMap.tEv_KizuClick, AddressOf SubKizuClick

        '''' 疵マップ 全長マップ
        Call mfrm_KizuMap.SetCls_Param(mcls_Param)
        Call mfrm_KizuMap.SetCls_RpiView(mcls_RpiView)
        Call mfrm_KizuMap.SetData_LstDetect(m_lstDetect)
        Call mfrm_KizuMap.SetData_LstCut(m_lstCut)
        AddHandler mfrm_KizuMap.tEv_MapClick, AddressOf SubMapClick
        AddHandler mfrm_KizuMap.tEv_KizuClick, AddressOf SubKizuClick

        '''' 疵一覧
        Call mfrm_KizuList.SetCls_Param(mcls_Param)
        Call mfrm_KizuList.SetCls_RpiView(mcls_RpiView)
        Call mfrm_KizuList.SetData_LstDetect(m_lstDetect)
        AddHandler mfrm_KizuList.tEv_KizuClick, AddressOf SubKizuClick
        AddHandler mfrm_KizuList.tEv_ReSetKizuDsp, AddressOf ReSetKizuDsp




        '''' コイル選択画面
        AddHandler mfrm_Search.tEv_RpiSelect, AddressOf SelectRpiFile

        '''' グリッド線
        AddHandler mfrm_GridLine.tEv_RePaint, AddressOf RePaint

        '''' コントロール画面
        If 0 = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "OPT_AUTODOWN", TASKINI_NAME) Then
            Call mfrm_FrmCont.Disable_AutoDown()    ' 減速機能無効化
        End If

        Call mfrm_FrmCont.SetCls_Param(mcls_Param)
        AddHandler mfrm_FrmCont.tEv_JumpNagate, AddressOf JumpNagate
        AddHandler mfrm_FrmCont.tEv_MapJumpNagate, AddressOf Map_JumpNagate
        AddHandler mfrm_FrmCont.tEv_AutoDownUmu, AddressOf AutoDownUmu
        AddHandler mfrm_FrmCont.tEv_AutoDownCancel, AddressOf AutoDownCancel

        '''' 表示設定画面
        mfrm_DspSet.SetMemoriProp(m_doc.drawmemori)
        AddHandler mfrm_DspSet.tEv_ReSetKizuOvl, AddressOf ReSetKizuOvl

        '''' オーバーレイ手動補正画面
        Call mfrm_ReSetOvl.SetData_LstDetect(m_lstDetect)
        AddHandler mfrm_ReSetOvl.tEv_ReSetKizuOvl, AddressOf ReSetKizuOvl


        '''' 開発用画面
        Call mfrm_Kai.SetCls_RpiView(mcls_RpiView)
        AddHandler mfrm_Kai.tEv_ReCreate, AddressOf ReCreateImg

        '''' 出力設定
        Call mfrm_Print.SetFrm_Main(Me)
        Call mfrm_Print.SetCls_RpiView(mcls_RpiView)


        '''' マーキング機能
        Call mfrm_Bookmark.SetCls_RpiView(mcls_RpiView)
        Call mfrm_Bookmark.SetCls_Calc(mcls_RpiView.GetCls_ImgDsp())
        Call mfrm_Bookmark.SetCnt_Pic(picFrameImg)
        AddHandler mfrm_Bookmark.tEv_JumpNagate, AddressOf JumpNagate
        AddHandler mfrm_Bookmark.tEv_JumpHaba, AddressOf JumpHaba


        '''' 入力一覧機能
        Call mfrm_InputList.SetCls_RpiView(mcls_RpiView)
        Call mfrm_InputList.SetCls_Calc(mcls_RpiView.GetCls_ImgDsp())
        Call mfrm_InputList.SetCnt_Pic(picFrameImg)
        AddHandler mfrm_InputList.tEv_JumpNagate, AddressOf JumpNagate
        AddHandler mfrm_InputList.tEv_JumpHaba, AddressOf JumpHaba
        AddHandler mfrm_InputList.tEv_RePaint, AddressOf RePaint
        Call mfrm_InputList.InitData_LstDetect(m_lstDetect)

        '''' 切り出し画像生成
        Call mfrm_KiriInput.SetCls_Pic(picFrameImg)
        Call mfrm_KiriInput.SetFrm_InputList(mfrm_InputList)



        '''' 各種設定
        Call mfrm_Setting.SetDocument(m_doc.displayshrinker)
        Call mfrm_Setting.SetCls_RpiView(mcls_RpiView)
        AddHandler mfrm_Setting.tEv_ReCreate, AddressOf ReCreateImg

        ' 初期化
        Call Init(True)
        Call InitPlugins()


        '===============================================
        ' 描画周期
        mcls_StopWatch.initCycle()
        Dim nWk As Integer = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "DISP_INTERVAL", TASKINI_NAME)
        Dim nInterval As Integer
        If 0 = nWk Then
            nInterval = 20
        ElseIf 1 = nWk Then
            nInterval = 60
        ElseIf 2 = nWk Then
            nInterval = 200
        End If
        timDsp.Interval = nInterval
        timDsp.Start()

        ''△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△
#If DEBUG Then
        'Call SelectRpiFile("C:\●デモ用ソフト\1CGL全長表示\RPIファイル\1420130306 124037@men=0@Angle=1@No=0@5-4465      .rpi", True)
        'Call SelectRpiFile("E:\★データ有りRPI\3220131107 192311@men=0@Color=1@No=0@N029530     .rpi", True)
#End If
    End Sub

    Private Function newPluginDspLen() As RpiPlugin.IPluginActivatable
        Dim dsplen As New frmPlDspLen(Me)
        dsplen.InitFrm_Control(mfrm_FrmCont)
        Return dsplen
    End Function

    Private Sub InitPlugins()
        'TODO : よくあるスプラッシュスクリーンでも表示するか？

        '内蔵プラグインの初期追加
        m_plugins.AddPlugin(New DrawMemori())    '目盛りの描画
        m_plugins.AddPlugin(New frmPopImgControl(Me))       '画像補正
        m_plugins.AddPlugin(newPluginDspLen)    '距離計測

        '外部プラグインの読み込み。
        'Using reg As Microsoft.Win32.RegistryKey = Microsoft.Win32.Registry.CurrentUser.OpenSubKey("SOFTWARE\TEX\RPI_VIEWER", False)
        '    If reg IsNot Nothing Then
        For Each dll As String In System.IO.Directory.GetFiles(System.IO.Directory.GetCurrentDirectory(), "*plugin*.dll", IO.SearchOption.AllDirectories)
            'If reg.GetValue(dll.ToString()) IsNot Nothing Then
            m_plugins.LoadDll(dll)
            '            End If
        Next
        '    End If
        'End Using

        m_plugins.Init(mcls_RpiView, Me)

        'ツールバーの実体が定義済み(=内蔵プラグイン用)の、ツールストリップ追加
        For Each ev As RpiPlugin.IEvToolStrip In m_plugins.EvToolStrips
            Dim name As String = ev.ToolStripName
            If name = "tsbMemori" Then
                ev.InitToolbar(tsbMemori, Nothing)
            End If
            If name = "tsbBrite" Then
                ev.InitToolbar(tsbBrite, Nothing)
            End If
        Next


        'ボタン入力がほしいプラグイン連中の、ツールストリップ追加
        '拡大ボタンの右側に挿入していく。
        Dim mevIndex As Integer = Me.tlsMainMenu.Items.IndexOf(tsbDspBig)
        mev_selectors.Add(New KeyValuePair(Of ToolStripButton, RpiPlugin.IMouseToolStrip)(tsbDspBig, Nothing))
        For Each mev As RpiPlugin.IMouseToolStrip In m_plugins.MouseInputs
            Dim tsv As ToolStripButton = mev.InitToolbar()
            If tsv Is Nothing Then Continue For

            AddHandler tsv.Click, AddressOf tsb_MouseR_Click

            mevIndex += 1
            mev_selectors.Add(New KeyValuePair(Of ToolStripButton, RpiPlugin.IMouseToolStrip)(tsv, mev))
            Me.tlsMainMenu.Items.Insert(mevIndex, tsv)
        Next


        '追加される一般ToolStrip
        Dim toolstrips() As ToolStrip = New ToolStrip() {tlsMainMenu, tlsMainMenu2}
        For Each plts As RpiPlugin.IToolStrip In m_plugins.ToolStrips
            plts.InitToolbar(toolstrips)
        Next

    End Sub

    Dim mev_selectors As New List(Of KeyValuePair(Of ToolStripButton, RpiPlugin.IMouseToolStrip))

    ''' <summary>
    ''' クローズ
    ''' </summary>
    Private Sub frmMain_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        timDsp.Stop()
        'Call mfrm_ImgCont.Save_last_Control_Ini()
        Call mfrm_KizuMap.Save_last_Control_Ini()

        ' メールスロット終了
        If mcls_MailSlotServer IsNot Nothing Then
            mcls_MailSlotServer.Dispose()
            mcls_MailSlotServer = Nothing
        End If

        ' 共有メモリ開放
        If m_memTrkPtr <> IntPtr.Zero Then
            tClass_GlovalMemory.FreeMapped(m_memTrkPtr, TBL_RPI_TRK_NAME)
        End If

        m_doc.drawmemori.Save()
        m_doc.imagecontrol.Save()

    End Sub
    '''' <summary>
    '''' サーバータイマー 
    '''' </summary>
    '''' <param name="source"></param>
    '''' <param name="e"></param>
    '''' <remarks></remarks>
    'Private Sub OnTimedEvent(ByVal source As Object, ByVal e As Timers.ElapsedEventArgs)
    '	timDsp_Tick(source, Nothing)
    'End Sub


    ''' <summary>
    ''' 初期化
    ''' </summary>
    ''' <param name="bInit">ホントに初期化時true</param>
    ''' <remarks></remarks>
    Private Sub Init(ByVal bInit As Boolean)
        ' 画像表示エリア
        Call mcls_RpiView.Init(bInit)


        ' 情報表示エリア
        If bInit Then spcMain.Panel1Collapsed = True
        lsvRpiHeader.Items.Clear()
        tsbInfMode.Visible = m_bDebug
        tsbImgQuarty.Visible = m_bDebug

        ' ステータス表示エリア
        If bInit Then tscRate.SelectedIndex = 0
        tscSpeed.SelectedItem = "0"

        ' チャンネル切り替え
        tscChannel.Items.Clear()
        tspbReadRpi.Value = 0
        tspbReadRpi.ForeColor = Color.Red

        ' 部分拡大
        m_bIsImgDrag = False
        mcls_ImgCutDsp.DspStop = True       ' 部分拡大が表示しっぱなしとなる為、一度強制的に表示ポップアップを閉じる
        mcls_ImgCutDsp.DspStop = False

        ' マウス処理プラグインがアクティブなら、デフォルトにする
        If m_active_mouse_handler IsNot Nothing Then
            m_active_mouse_handler.Activate(False)
            m_active_mouse_handler = Nothing
        End If

        ' 実績
        m_typCoil = Nothing
        m_lstDetect.Clear()
        m_lstCut.Clear()

        ' 疵接近ランプ
        For ii As Integer = 0 To mcnt_Ranp.Length - 1
            mcnt_Ranp(ii).BackColor = SystemColors.ControlDark
        Next

    End Sub



    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ RPI 初期表示関係

    ''' <summary>
    ''' RPIファイルをドラッグで指定する (ドラッグ)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_DragEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DragEventArgs) Handles Me.DragEnter
        ' ドラッグで有効にしておかないとドロップイベントが走らない
        ' フォームプロパティで AllowDrop を有効にしておかないとダメ
        If e.Data.GetDataPresent(DataFormats.FileDrop) Then
            e.Effect = DragDropEffects.Copy
        Else
            e.Effect = DragDropEffects.None
        End If
    End Sub

    ''' <summary>
    ''' RPIファイルをドラッグで指定する (ドロップ)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_DragDrop(ByVal sender As Object, ByVal e As System.Windows.Forms.DragEventArgs) Handles Me.DragDrop
        If Not e.Data.GetDataPresent(DataFormats.FileDrop) Then Return

        '' ドラッグされたデータ形式を調べ、ファイルのときはコピーとする
        Dim fileName As String() = CType(e.Data.GetData(DataFormats.FileDrop, False), String())
        Dim strFPath As String = fileName(0) ' 複数指定時を考慮して、最初の１コを使う


        ' RPIファイル実行
        Call SelectRpiFile(strFPath, True, "")
    End Sub


    ''' <summary>
    ''' テスト用スクロール端ボタン (とりあえず、画像再描画にしておく)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRituNon_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRituNon.Click
        ' 32bitだと先読みでエラーになって、描画できない場合がある対策
        Call ReCreateImg()
    End Sub



    ''' <summary>
    ''' RPIファイル検索＋選択
    ''' </summary>
    ''' <param name="sIniDbName">対象のINIファイルセクション</param>
    ''' <param name="sKanriNo">管理No</param>
    ''' <param name="nMen">面(0,1)</param>
    ''' <param name="nNum">角度、色 (0オリジン)</param>
    ''' <remarks></remarks>
    Private Sub SelectRpiFile(ByVal sIniDbName As String, ByVal sKanriNo As String, ByVal nMen As Integer, ByVal nNum As Integer)

        ' 検索
        Const MAXNUM As Integer = 10
        Dim sRpiForder(MAXNUM - 1) As String    ' 検索フォルダ
        For jj As Integer = 0 To MAXNUM - 1
            sRpiForder(jj) = tMod.tMod_IniFile.ReadIniStr(sIniDbName, String.Format("S_{0}_{1}", nMen, jj + 1), TASKINI_NAME)
        Next

        ' フォーマット生成
        Dim sRpiFormat As String = tMod.tMod_IniFile.ReadIniStr(sIniDbName, "RPI_FORMAT", TASKINI_NAME)
        ' ファイル検索文字列
        Dim sSearchName As String = sRpiFormat.Replace("{S_FOLDER_NAME}", GetFolderName(sKanriNo))
        sSearchName = m_reParen.Replace(sSearchName, "*")
        sSearchName = String.Format(sSearchName, sKanriNo, nMen, nNum + 1)
        ' 文字列が.rpiで終わっていなければ"*.rpi"を末尾につける
        If Not sSearchName.EndsWith(".rpi", StringComparison.CurrentCultureIgnoreCase) Then sSearchName &= "*.rpi"


        ' 全フォルダーサーチ
        Dim sFullPath As String = ""
        For jj As Integer = 0 To MAXNUM - 1
            If "" = sRpiForder(jj) Then Continue For

            Dim sFpath() As String = IO.Directory.GetFiles(sRpiForder(jj), sSearchName, IO.SearchOption.TopDirectoryOnly)
            If 1 = sFpath.Length Then       ' 複数ある場合どうしよう。ありえないはずだけど

                sFullPath = sFpath(0)
                Exit For
            End If
        Next

        ' 実行
        If "" <> sFullPath Then
            Call SelectRpiFile(sFullPath, True, sKanriNo)
        End If
    End Sub

    ''' <summary>
    ''' RPIファイル選択
    ''' </summary>
    ''' <param name="strFPath">フルパス</param>
    ''' <param name="bInit">true:新コイル  false:表裏変更のみ</param>
    ''' <param name="sKanriNo">管理No</param>
    ''' <remarks></remarks>
    Private Sub SelectRpiFile(ByVal strFPath As String, Optional ByVal bInit As Boolean = True, Optional ByVal sKanriNo As String = "")

        Dim bChangeLine As Boolean = True   ' 前回と異なるライン時true
        Dim sOldLine As String = ""         ' 前回ライン

        If Not gcls_Log Is Nothing Then
            gcls_Log.Write(tClass_LogManager.LNO.MSG, String.Format("RPIファイル選択[{0}] init:{1} KanriNo:{2}", strFPath, bInit, sKanriNo))

        End If

        If "" = strFPath Then
            Call Init(True)     ' 初期化
            '選択位置一覧クリア
            Call mfrm_InputList.NewCoil(String.Empty, 0, String.Empty)
            Call RePaint()      ' 画面描画
            Return
        End If
        '===============================================
        ' 事前準備



        '' 現在位置取得
        Dim dDspPos As Single = 0      ' 現在表示位置
        If Not bInit Then
            dDspPos = mcls_RpiView.GetPos_DpToLen_mm(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y)
        End If

        '' 前回と異なるライン？ １段
        If mcls_RpiView.GetRpi_Header.IsTHeader Then
            sOldLine = mcls_RpiView.GetRpi_THeader.cSysName
        End If

        '' ここまで来たらRPIファイル正常 
        ' 一度前回のがあるかも知れないので、クローズ
        For Each pRpiOpen As RpiPlugin.IRpiOpen In m_plugins.RpiOpens
            pRpiOpen.OnRpiClose()
        Next

        mcls_RpiView.RpiClose()
        Call mcls_CheckKizu.ThreadStop()
        '更新タイマも止める。
        timThreadRpiRead.Stop()



        '' デフォルト管理No適用(ようはRPIファイル内に埋め込みのやつ)
        If "" = sKanriNo Then
            'RPIファイル管理Noの付け替え
            sKanriNo = tClass_ReadRPIFile.READ_FROM_RPI_FILE
        End If


        '' オープン
        If Not mcls_RpiView.RpiOpen(strFPath, , , , sKanriNo) Then
            '' Ver4.4.2 MsgBox(String.Format("RPIファイル読み込み失敗2 {0}", strFPath))
            Call Init(True)
            Return
        End If

        '''' その他 初期処理準備
        Me.Text = ""
        Call DspRpiFile(bInit)
        Me.Text &= String.Format(" [{0}]", strFPath)


        'mcls_RpiView.tProp_LenOff(0) = -1000
        'mcls_RpiView.tProp_LenOff(1) = 100000


        ''分解能後から変更対応(検証)
        'Dim t As tClass_ReadRPI.RPI_HEADER = mcls_RpiView.GetRpi_Cls().PropRpiHeader()
        't.res_y = 0.01
        'mcls_RpiView.GetRpi_Cls().PropRpiHeader() = t
        'mcls_RpiView.PropCls_ImgDsp.SetImgInf(t.width, t.height, t.res_x, t.res_y, 0, 0, 1, 1)




        '' 前回と異なるライン？ ２段
        If mcls_RpiView.GetRpi_THeader.cSysName = sOldLine Then bChangeLine = False


        '===============================================
        '''' DBから情報取得
        Dim sSession As String = "DB_" & mcls_RpiView.GetRpi_THeader.cSysName
        Dim nWk As Integer = tMod.tMod_IniFile.ReadIniInt(sSession, "ENABLE_DB", TASKINI_NAME)
        m_bDb = IIf(nWk = 0, False, True)



        '===============================================
        '' DBアクセス
        'If m_bDb And m_bDrawKizuOvl And mcls_RpiView.GetRpi_Header.IsTHeader Then
        If m_bDb And mcls_RpiView.GetRpi_Header.IsTHeader Then

            '' DBから情報取得
            mcls_Param.Set_DbSession(sSession)
            m_bResutl = mcls_Param.ReadAll()

            '' コイル情報関係
            m_typCoil = Nothing
            mcls_Param.ReadZi_CoilInf(mcls_RpiView, m_typCoil)

            '' 疵情報関係
            m_lstDetect.Clear()
            mcls_RpiView.ResetOverLayInf()
            mcls_Param.ReadZi_DefectInf(mcls_RpiView, m_lstDetect)

            '' カット情報
            m_lstCut.Clear()
            mcls_Param.ReadZi_CutInf(mcls_RpiView, m_lstCut)

            '' 情報反転設定
            mcls_Param.ReadZi_DefectInfReverce(mcls_RpiView, m_lstDetect, m_lstCut)

            '' その他情報
            Dim nVal As Integer = mcls_Param.ReadZi_DefPitch(mcls_RpiView)      ' 未指定の場合は0。使い元で養生すること
            Call mfrm_FrmCont.ChangeDefVal(nVal)
            Call mfrm_GridLine.ChangeDefVal(nVal)

        Else
            m_bResutl = False
            ' 対象外なら初期化のみ
            m_lstDetect.Clear()
            mcls_RpiView.ResetOverLayInf()
        End If



        '===============================================
        '''' サブフォーム初期化
        If Not bChangeLine Then
            ' 疵種フィルタ反映
            Call mfrm_KizuList.FilterList()
        End If

        If bInit Then
            Call mfrm_Bookmark.MarkDel(-1) '全部
        End If

        ' コイルNoの抽出
        Dim sRpiFileName As String = System.IO.Path.GetFileNameWithoutExtension(strFPath)
        Dim arrRpiFileName As String() = sRpiFileName.Split("@"c)
        Dim sCoilNo As String = String.Empty
        If 1 < arrRpiFileName.Length Then
            sCoilNo = arrRpiFileName(arrRpiFileName.Length - 1)
        End If
        Call mfrm_InputList.NewCoil(mcls_RpiView.GetRpi_THeader.cKizukenNo, mcls_RpiView.GetRpi_THeader.nMen, sCoilNo)

        '' コイル情報、疵情報クラス関係再セット
        Call ReverceDsp(bChangeLine)

        '' オーバーレイ手動補正関係
        If mcls_RpiView.GetRpi_Header.IsTHeader Then
            Call mfrm_ReSetOvl.ChangeCoil(mcls_RpiView.GetRpi_THeader.nStartCamset, mcls_RpiView.GetRpi_THeader.nCamsetNum)
        End If


        '===============================================
        '''' ライン固有設定読込み
        If 0 = tMod.tMod_IniFile.ReadIniInt(sSession, "OPT_SYNC", TASKINI_NAME) Then        ' 非表示設定
            tsbSync.Visible = False
        Else
            tsbSync.Visible = True
        End If


        '===============================================
        '''' 描画更新
        ' 表裏間距離補正対応
        Call mcls_RpiView.ResizePaintAria(Nothing, Nothing)     ' 一度正しい値を算出

        mcls_RpiView.tProp_ImgOffY = 0
        mcls_RpiView.tProp_LabelOffsetYmm = 0
        If mcls_RpiView.GetRpi_Header.IsTHeader Then
            Dim tHeader As tClass_ReadRPI.RPI_T_HEADER = mcls_RpiView.GetRpi_THeader

            Dim tpos_px As Integer = tHeader.nFTposCfNo(0) * mcls_RpiView.GetRpi_Header.height + tHeader.nFTposPixel(0)
            If tpos_px <> 0 Then
                Call mcls_RpiView.ResizePaintAria(Nothing, Nothing)     ' スクロールバー量再計算

                If dDspPos = 0 Then
                    dDspPos = -250
                End If

                mcls_RpiView.tProp_LabelOffsetYmm = -mcls_RpiView.GetPos_RpToLen_Y(tpos_px)

                bInit = False

            Else
                'If tHeader.nStartLen < 3 * mcls_RpiView.GetRpi_Header.res_y * mcls_RpiView.GetRpi_Header.height Then
                If tHeader.nStartLen < (mcls_RpiView.GetDspAriaInf.nDspFrameCnt - 1) * mcls_RpiView.GetRpi_Header.res_y * mcls_RpiView.GetRpi_Header.height Then
                    mcls_RpiView.tProp_ImgOffY = mcls_RpiView.GetPos_LenToRp_Y(tHeader.nStartLen)   ' 表裏間距離最適化対応
                    Call mcls_RpiView.ResizePaintAria(Nothing, Nothing)     ' スクロールバー量再計算
                End If
            End If
        End If

        ' 表示位置決定
        Dim nDspPos As Integer
        If bInit Then
            nDspPos = vsBar.Minimum
        Else
            nDspPos = mcls_RpiView.GetPos_LenToDp_mm(dDspPos)
        End If
        Call mcls_RpiView.ScrollH(nDspPos, True)
        If Not bInit Then Call mcls_RpiView.ReCreateImg() ' 作り直し


        mcls_CheckKizu.Enable = mfrm_FrmCont.AutoDownUmu
        Call mcls_CheckKizu.ThreadStart(Threading.ThreadPriority.Highest)


        ' メモリ調整 厚板対応。超暫定的。拡大したときとかもどうするか 今後考慮かな     △
        Dim ImgInf As tClass.tClass_ImageDsp.IMG_INF = mcls_RpiView.GetImg_ImgInf()
        If 3000.0 <= ImgInf.dSizeW Then
            m_doc.drawmemori.g_nScalStringX = 5
        Else
            m_doc.drawmemori.g_nScalStringX = 2
        End If


        For Each pRpiOpen As RpiPlugin.IRpiOpen In m_plugins.RpiOpens
            pRpiOpen.OnRpiOpen(mcls_RpiView.GetRpi_Cls)
        Next


        ' スクロール値が0時、vsBar_ValueChangedでリペントが走らない為の対策 (念の為に両方再描画)
        Call RePaint()


        Me.Activate()
    End Sub



    ''' <summary>
    ''' RPIファイル 初期読み込み処理
    ''' </summary>
    ''' <param name="bInit">true:新コイル  false:表裏変更のみ</param>
    ''' <remarks></remarks>
    Private Sub DspRpiFile(ByVal bInit As Boolean)
        Dim nMen As Integer = -1    ' 面 -1:不明    0:表  1:裏

        '===============================================
        '' RPIクラス設定
        Dim typ As tClass.tClass_ReadRPI.RPI_HEADER = mcls_RpiView.GetRpi_Header

        '' TEXヘッダー
        If typ.IsTHeader Then
            Dim tH As tClass.tClass_ReadRPI.RPI_T_HEADER = mcls_RpiView.GetRpi_THeader
            nMen = tH.nMen
        Else

        End If


        '===============================================
        '''' 左欄更新
        nudFrameNo.Value = 1
        nudFrameNo.Maximum = typ.nframe

        '''' ヘッダー情報部を表示
        Call DspHeader()


        '''' ライン別設定
        Dim sSession As String = "DB_" & mcls_RpiView.GetRpi_THeader.cSysName
        If Not bInit Then Return ' 表裏変更のみならここまで


        '===============================================
        '' 表示方向
        If 0 = tMod.tMod_IniFile.ReadIniInt(sSession, "DorW", TASKINI_NAME) Then
            mcls_RpiView.SetProp_DSWS(0)
            tsbDSWS.Checked = False
        Else
            mcls_RpiView.SetProp_DSWS(1)
            tsbDSWS.Checked = True
        End If
        If -1 <> nMen Then
            If 0 = tMod.tMod_IniFile.ReadIniInt(sSession, String.Format("DorW_{0}", nMen), TASKINI_NAME) Then
                mcls_RpiView.SetProp_DSWS(0)
                tsbDSWS.Checked = False
            Else
                mcls_RpiView.SetProp_DSWS(1)
                tsbDSWS.Checked = True
            End If
        End If

        If 0 = tMod.tMod_IniFile.ReadIniInt(sSession, "TorB", TASKINI_NAME) Then
            mcls_RpiView.SetProp_TB(0)
            tsbTB.Checked = False
        Else
            mcls_RpiView.SetProp_TB(1)
            tsbTB.Checked = True
        End If
        If -1 <> nMen Then
            If 0 = tMod.tMod_IniFile.ReadIniInt(sSession, String.Format("TorB_{0}", nMen), TASKINI_NAME) Then
                mcls_RpiView.SetProp_TB(0)
                tsbTB.Checked = False
            Else
                mcls_RpiView.SetProp_TB(1)
                tsbTB.Checked = True
            End If
        End If


        If 0 = tMod.tMod_IniFile.ReadIniInt(sSession, "Rotate", TASKINI_NAME) Then
            SetRotate(False)
            tsbRot.Checked = False
        Else
            SetRotate(True)
            tsbRot.Checked = True
        End If
        If -1 <> nMen Then
            If 0 = tMod.tMod_IniFile.ReadIniInt(sSession, String.Format("Rotate_{0}", nMen), TASKINI_NAME) Then
                SetRotate(False)
                tsbRot.Checked = False
            Else
                SetRotate(True)
                tsbRot.Checked = True
            End If
        End If

        '' 左右文字
        m_sDrawStrLR(0) = tMod.tMod_IniFile.ReadIniStr(sSession, "STRING_L", TASKINI_NAME)
        m_sDrawStrLR(1) = tMod.tMod_IniFile.ReadIniStr(sSession, "STRING_R", TASKINI_NAME)



        '''' ステータスバー内を更新
        tslFrameAllCnt.Text = typ.nframe.ToString
        tscChannel.Items.Clear()
        For ii As Integer = 0 To mcls_RpiView.GetRpi_Header.nchannel - 1
            tscChannel.Items.Add(String.Format("{0}ch", ii))
        Next
        tscChannel.SelectedIndex = 0
        tspbReadRpi.Value = 0
        tspbReadRpi.ForeColor = Color.Red
        tspbReadRpi.Maximum = typ.nframe
        timThreadRpiRead.Start()            ' RPIファイル取り込み状況開始
    End Sub

    ''' <summary>
    ''' 非同期RPIファイル読込み状況
    ''' </summary>
    Private Sub timThreadRpiRead_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timThreadRpiRead.Tick
        Dim typ As tClass.tClass_ReadRPI.RPI_HEADER = mcls_RpiView.GetRpi_Header
        Dim nCnt As Integer = mcls_RpiView.GetRpi_Cls.GetRpiFrameCount()
        If nCnt < typ.nframe Then
            tspbReadRpi.Value = nCnt
        Else
            timThreadRpiRead.Stop()
            tspbReadRpi.Value = typ.nframe
            tspbReadRpi.ForeColor = SystemColors.Highlight
        End If
    End Sub


    ''' <summary>
    ''' 表示方向変更時などのサブ画面一括反映
    ''' </summary>
    ''' <param name="bCangeLine">true:新コイルでラインも異なる</param>
    ''' <remarks></remarks>
    Private Sub ReverceDsp(Optional ByVal bCangeLine As Boolean = False)
        mcls_Param.ReadZi_DefectInfReverce(mcls_RpiView, m_lstDetect, m_lstCut)     ' 反転設定

        Dim bCoilItemDsp As Boolean = False     ' コイル一覧画面 表示中？
        If Not mfrm_CoilItem Is Nothing AndAlso mfrm_CoilItem.Visible Then
            bCoilItemDsp = True
        End If


        '' コイル情報、疵情報クラス関係再セット
        Call mfrm_KizuMap.SetData_Coil(m_typCoil)
        Call mfrm_KizuMap.MapDisp()
        Call mfrm_DefMap.MapDisp()

        If bCangeLine Then
            Call mfrm_KizuList.NewCoil()
            Call mfrm_FrmCont.ChangeLine()

            ' 別ラインなので再生成
            If bCoilItemDsp Then
                mfrm_CoilItem.Close()
                mfrm_CoilItem = New frmPopCoilItem
                Call mfrm_CoilItem.SetCls_RpiView(mcls_RpiView)
                mfrm_CoilItem.Show(Me)
            End If
        Else
            If bCoilItemDsp Then
                mfrm_CoilItem.ReDsp()
            End If
        End If
        Call mfrm_KizuList.MapDisp()

        'Call mfrm_Setting.ReChange(bCangeLine)
        m_doc.displayshrinker.Reset(mcls_RpiView.GetRpi_Header.res_x, mcls_RpiView.GetRpi_Header.res_y)
        m_doc.displayshrinker.Emit()

        Call mcls_CheckKizu.CoilChange()
        Dim typ As tClass.tClass_ReadRPI.RPI_HEADER = mcls_RpiView.GetRpi_Header
        If typ.IsTHeader Then
            Call mcls_CheckKizu.MenChange(mcls_RpiView.GetRpi_THeader.nMen)
        End If

    End Sub

    Private m_doc As New GlobalProperties
    Public ReadOnly Property Properties As RpiPlugin.PeropertiesBase Implements RpiPlugin.IPropertiesProvider.Document
        Get
            Return m_doc
        End Get
    End Property


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メールスロット受信

    ''' <summary>
    ''' メールスロット受信
    ''' </summary>
    ''' <param name="que"></param>
    ''' <remarks></remarks>
    Private Sub mcls_RecvMail_tEv_RecvMail(ByVal que As CommonQueMix) Handles mcls_MailSlotServer.tEv_RecvMail

        'MsgBox(String.Format("{0}/{1},{2}", que.nEventNo, que.idata(0), que.idata(1)))
        Select Case que.nEventNo
            ' 基本系 -------------------------------------------------
            Case FACT_RPIV_END      ' 終了
                Me.Close()

            Case FACT_RPIV_DSPING       ' RPIファイル表示
                ' MIX_S[0] : RPIフォルダパス (※ここにフルパスを入れてもOK)
                ' MIX_S[1] : RPIファイル名1
                ' MIX_S[2] : RPIファイル名2	(※S[2]にデータがセットされている場合は、内部的には単純にS[0]～[2]を連結するだけとする)

                ' MIX_I[0] : 複数チャンネル時 (0オリジン)
                ' MIX_I[1] : Yモード (0:無視 1:指定値(先頭) 2:指定値(中央) 3:現在から差分)
                ' MIX_I[2] : Y方向[mm]
                ' MIX_I[3] : Xモード (0:無視 1:指定値(先頭) 2:指定値(中央) 3:現在から差分)
                ' MIX_I[4] : X方向[mm] (-1:移動させない)
                ' MIX_I[5] : X方向[mm]がどっちの指定から？(0:左端  1:センターから)

                ' 以下３項目の引数 (0:無視 -9:現表示に対して反転 -1:元画像に対して反転無し(要はiniファイルで反転させたやつも戻す) 1:元画像に対して反転 2:iniファイル設定後に対して反転)
                ' MIX_I[6] : 左右反転
                ' MIX_I[7] : 上下反転
                ' MIX_I[8] : 90度反転
                Call SelectRpiFile(que)

            Case FACT_RPIV_ACTIVE       ' RPIビューアー アクティブ
                Me.Activate()

                ' MIX_S[0] : 対象のINIファイルセクション
                ' MIX_S[1] : 管理No

                ' MIX_I[0] : 面(0,1)
                ' MIX_I[1] : 角度、色 (0オリジン)
            Case FACT_RPIV_SEARCH_DSPING    ' RPIファイル検索＋表示
                Call SelectRpiFile(que.cdata1, que.cdata2, que.idata(0), que.idata(1))


                ' 画面操作系 -------------------------------------------------
            Case FACT_RPIV_DSP_MOVE     ' 指定位置
                ' MIX_I[0] : Yモード (0:無視 1:指定値(先頭) 2:指定値(中央) 3:現在から差分)
                ' MIX_I[1] : Y方向[mm]
                ' MIX_I[2] : Xモード (0:無視 1:指定値(先頭) 2:指定値(中央) 3:現在から差分)
                ' MIX_I[3] : X方向[mm] (-1:移動させない)
                ' MIX_I[4] : X方向[mm]がどっちの指定から？(0:左端  1:センターから)
                If 0 <> que.idata(0) Then JumpNagate(que.idata(0), que.idata(1))
                If 0 <> que.idata(2) Then JumpHaba(que.idata(2), que.idata(3), que.idata(4))


                '----------------------------------------------------
                ' 反転
                ' 以下３項目の引数 (0:無視 -9:現表示に対して反転 -1:元画像に対して反転無し(要はiniファイルで反転させたやつも戻す) 1:元画像に対して反転 2:iniファイル設定後に対して反転)
                ' MIX_I[0] : 左右反転
                ' MIX_I[1] : 上下反転
                ' MIX_I[2] : 90度反転
            Case FACT_RPIV_DSP_TURN     ' 表示方向切替
                If 0 <> que.idata(0) Then Call TurnDSWS(que.idata(0))
                If 0 <> que.idata(1) Then Call TurnTB(que.idata(1))
                If 0 <> que.idata(2) Then Call TurnRotate(que.idata(2))
        End Select

    End Sub



    ''' <summary>
    ''' RPIファイル選択
    ''' </summary>
    ''' <param name="que">メールスロット</param>
    ''' <remarks></remarks>
    Private Sub SelectRpiFile(ByVal que As CommonQueMix)

        ' MIX_S[0] : RPIフォルダパス (※ここにフルパスを入れてもOK)
        ' MIX_S[1] : RPIファイル名1
        ' MIX_S[2] : RPIファイル名2	(※S[2]にデータがセットされている場合は、内部的には単純にS[0]～[2]を連結するだけとする)

        ' MIX_I[0] : 複数チャンネル時 (0オリジン)
        ' MIX_I[1] : Yモード (0:無視 1:指定値(先頭) 2:指定値(中央) 3:現在から差分)
        ' MIX_I[2] : Y方向[mm]
        ' MIX_I[3] : Xモード (0:無視 1:指定値(先頭) 2:指定値(中央) 3:現在から差分)
        ' MIX_I[4] : X方向[mm] (-1:移動させない)
        ' MIX_I[5] : X方向[mm]がどっちの指定から？(0:左端  1:センターから)

        ' 以下３項目の引数 (0:無視 -9:現表示に対して反転 -1:元画像に対して反転無し(要はiniファイルで反転させたやつも戻す) 1:元画像に対して反転 2:iniファイル設定後に対して反転)
        ' MIX_I[6] : 左右反転
        ' MIX_I[7] : 上下反転
        ' MIX_I[8] : 90度反転
        'que.cdata1 = "c:\2420130828 134704@men=0@Angle=1@No=0@846382  .rpi"

        ' チャンネル指定
        mcls_RpiView.GetRpi_Cls.PropRpiActiveCh = que.idata(0)

        ' RPIファイル読み込み
        Dim sFPath As String
        If 0 = que.cdata3.Length Then
            sFPath = IO.Path.Combine(que.cdata1, que.cdata2)
        Else
            sFPath = que.cdata1 & que.cdata2 & que.cdata3
        End If
        Call SelectRpiFile(sFPath)


        ' 反転処理	※この時点では、iniファイル設定で反転させた後
        If 0 <> que.idata(6) Then Call TurnDSWS(que.idata(6))
        If 0 <> que.idata(7) Then Call TurnTB(que.idata(7))
        If 0 <> que.idata(8) Then Call TurnRotate(que.idata(8))

        ' 初期表示位置
        If 0 <> que.idata(1) Then JumpNagate(que.idata(1), que.idata(2))
        If 0 <> que.idata(3) Then JumpHaba(que.idata(3), que.idata(4), que.idata(5))


        ' 再描画
        Call ReverceDsp()
        Call ReCreateImg()
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 左欄部分
#Region "左欄部分"
    ''' <summary>
    ''' RPIヘッダー表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspHeader()
        '' リスト初期化
        lsvRpiHeader.Items.Clear()
        '' データ表示
        Dim typ As tClass.tClass_ReadRPI.RPI_HEADER = mcls_RpiView.GetRpi_Header
        Dim itm As ListViewItem


        itm = lsvRpiHeader.Items.Add("画像形式")
        If typ.emCompType = tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_BMP Then
            itm.SubItems.Add("非圧縮 bmp")
        ElseIf typ.emCompType = tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_RAW Then
            itm.SubItems.Add("非圧縮 raw")
        ElseIf typ.emCompType = tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_JPEG Then
            itm.SubItems.Add("圧縮 jpg")
        ElseIf typ.emCompType = tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_LAC Then
            itm.SubItems.Add("圧縮 LAC")
        ElseIf typ.emCompType = tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_NUC Then
            itm.SubItems.Add("圧縮 NUC")
        ElseIf typ.emCompType = tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_LAC5 Then
            itm.SubItems.Add("圧縮 LAC5")
        Else
            itm.SubItems.Add(tClass.tClass_ReadRPI.GetString(typ.emCompType))
        End If

        itm = lsvRpiHeader.Items.Add("分解能有無")
        If typ.IsRes Then
            itm.SubItems.Add("有り")
        Else
            itm.SubItems.Add("無し")
        End If

        itm = lsvRpiHeader.Items.Add("ﾌﾚｰﾑ情報")
        If typ.IsFrm Then
            itm.SubItems.Add("有り")
        Else
            itm.SubItems.Add("無し")
        End If

        itm = lsvRpiHeader.Items.Add("タイプ")
        itm.SubItems.Add(typ.type)

        itm = lsvRpiHeader.Items.Add("header_size")
        itm.SubItems.Add(String.Format("{0}", typ.header_size))

        itm = lsvRpiHeader.Items.Add("width")
        itm.SubItems.Add(String.Format("{0}", typ.width))

        itm = lsvRpiHeader.Items.Add("height")
        itm.SubItems.Add(String.Format("{0}", typ.height))

        itm = lsvRpiHeader.Items.Add("nchannel")
        itm.SubItems.Add(String.Format("{0}", typ.nchannel))

        itm = lsvRpiHeader.Items.Add("pixel_nbit")
        itm.SubItems.Add(String.Format("{0}", typ.pixel_nbit))

        itm = lsvRpiHeader.Items.Add("pitch_nbyte")
        itm.SubItems.Add(String.Format("{0}", typ.pitch_nbyte))

        itm = lsvRpiHeader.Items.Add("nframe")
        itm.SubItems.Add(String.Format("{0}", typ.nframe))

        itm = lsvRpiHeader.Items.Add("frame_nbyte")
        itm.SubItems.Add(String.Format("{0}", typ.frame_nbyte))

        itm = lsvRpiHeader.Items.Add("frame_start_pos")
        itm.SubItems.Add(String.Format("{0}", typ.frame_start_pos))

        itm = lsvRpiHeader.Items.Add("frame_pitch_nbyte")
        itm.SubItems.Add(String.Format("{0}", typ.frame_pitch_nbyte))

        itm = lsvRpiHeader.Items.Add("ﾌﾚｰﾑ情報種別")
        itm.SubItems.Add(String.Format("{0}", typ.frame_info_type))

        itm = lsvRpiHeader.Items.Add("reso_x")
        itm.SubItems.Add(String.Format("{0}", typ.res_x))

        itm = lsvRpiHeader.Items.Add("reso_y")
        itm.SubItems.Add(String.Format("{0}", typ.res_y))


        'TEXヘッダー有り
        If typ.IsTHeader Then
            Dim tH As tClass.tClass_ReadRPI.RPI_T_HEADER = mcls_RpiView.GetRpi_THeader
            Dim nNumCam As Integer          ' カメラ情報数

            If tH.nTHeaderVer = 1 Then
                nNumCam = 8 - 1     ' Ver1 : 8台
            Else
                nNumCam = 16 - 1    ' Ver2 : 16台
            End If

            Me.Text = String.Format("対象システム={0}, 疵検管理No={1}, 面={2}", tH.cSysName, tH.cKizukenNo, tH.nMen)

            lsvRpiHeader.Items.Add("【TEXﾍｯﾀﾞ】-------------------------")

            itm = lsvRpiHeader.Items.Add("ヘッダーサイズ") : itm.SubItems.Add(String.Format("{0}", tH.nTHeaderSize))
            itm = lsvRpiHeader.Items.Add("バージョン") : itm.SubItems.Add(String.Format("{0}", tH.nTHeaderVer))
            itm = lsvRpiHeader.Items.Add("システム略称") : itm.SubItems.Add(String.Format("{0}", tH.cSysName))
            itm = lsvRpiHeader.Items.Add("疵検管理No") : itm.SubItems.Add(String.Format("{0}", tH.cKizukenNo))
            itm = lsvRpiHeader.Items.Add("ﾌﾚｰﾑ情報ﾀｲﾌﾟ") : itm.SubItems.Add(String.Format("{0}", tH.frame_info_type))
            itm = lsvRpiHeader.Items.Add("ﾌﾚｰﾑ情報ｻｲｽﾞ") : itm.SubItems.Add(String.Format("{0}", tH.frame_info_size))
            itm = lsvRpiHeader.Items.Add("ｺｲﾙ先頭ﾌﾚｰﾑNo") : itm.SubItems.Add(String.Format("{0}", tH.nFrameNumInit))
            itm = lsvRpiHeader.Items.Add("このｺｲﾙ先頭FNo") : itm.SubItems.Add(String.Format("{0}", tH.nFrameNumStart))
            itm = lsvRpiHeader.Items.Add("ｺｲﾙ単位何ﾌｧｲﾙ目") : itm.SubItems.Add(String.Format("{0}", tH.nFileNum))
            itm = lsvRpiHeader.Items.Add("ﾗｲﾝｾﾝﾀｰ画素") : itm.SubItems.Add(String.Format("{0}", tH.nLineCenter))
            itm = lsvRpiHeader.Items.Add("左端ｶﾒﾗｾｯﾄ番号") : itm.SubItems.Add(String.Format("{0}", tH.nStartCamset))
            itm = lsvRpiHeader.Items.Add("幅方向ﾌﾚｰﾑ結合数") : itm.SubItems.Add(String.Format("{0}", tH.nCamsetNum))
            itm = lsvRpiHeader.Items.Add("カメラ角度") : itm.SubItems.Add(String.Format("{0}", tH.nCamAngle))
            itm = lsvRpiHeader.Items.Add("表裏区分") : itm.SubItems.Add(String.Format("{0}", tH.nMen))
            For ii As Integer = 0 To nNumCam
                itm = lsvRpiHeader.Items.Add(String.Format(" {0} 基準位置", ii + 1)) : itm.SubItems.Add(String.Format("{0}", tH.caminfo(ii).dBaseXoffset))
                itm = lsvRpiHeader.Items.Add(String.Format(" {0} 基準画素", ii + 1)) : itm.SubItems.Add(String.Format("{0}", tH.caminfo(ii).nBaseLeft))
                itm = lsvRpiHeader.Items.Add(String.Format(" {0} 右OLV量", ii + 1)) : itm.SubItems.Add(String.Format("{0}", tH.caminfo(ii).nOverLap))
            Next
            itm = lsvRpiHeader.Items.Add("先頭ﾌﾚｰﾑ長手位置") : itm.SubItems.Add(String.Format("{0}", tH.nStartLen))
            itm = lsvRpiHeader.Items.Add("画像形式") : itm.SubItems.Add(String.Format("{0}", tH.nColorType))
            itm = lsvRpiHeader.Items.Add("疵検タイプ") : itm.SubItems.Add(String.Format("{0}", tH.nSysType))
            itm = lsvRpiHeader.Items.Add("先端ﾌﾚｰﾑNo") : itm.SubItems.Add(String.Format("{0}", tH.nFTposCfNo(0)))
            itm = lsvRpiHeader.Items.Add("先端ﾌﾚｰﾑ内pixel") : itm.SubItems.Add(String.Format("{0}", tH.nFTposPixel(0)))
            itm = lsvRpiHeader.Items.Add("後端ﾌﾚｰﾑNo") : itm.SubItems.Add(String.Format("{0}", tH.nFTposCfNo(1)))
            itm = lsvRpiHeader.Items.Add("後端ﾌﾚｰﾑ内pixel") : itm.SubItems.Add(String.Format("{0}", tH.nFTposPixel(1)))

        End If

    End Sub

    ''' <summary>
    ''' 画像保存
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSave_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSave.Click
        'Dim nNow As Integer = CInt(mcls_RpiView.GetDspAriaInf.nDspFno)
        Dim nNow As Integer = CInt(nudFrameNo.Value)
        Dim sSaveName As String = String.Format("{0}.bmp", nNow)

        Try
            Dim Buf As tClass_ViewRpiThread.CLS_IMG_INF = mcls_RpiView.GetCls_Img_inf(nNow)
            Dim Img As Bitmap = Buf.pImgRaw
            Dim bRet As Boolean = tClass.tClass_BmpJpeg.BmpFileSave(sSaveName, Img)

        Catch ex As Exception
            'Img.Save(sSaveName, Drawing.Imaging.ImageFormat.Bmp)
        End Try


        ' 生情報
        sSaveName = String.Format("{0}.bin", nNow)
        Dim bwk() As Byte = mcls_RpiView.GetRpi_Cls().ReadFile_ImgByte(nNow)
        IO.File.WriteAllBytes(sSaveName, bwk)

    End Sub
#End Region



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 共通基本処理

    ''' <summary>
    ''' 再描画
    ''' </summary>
    Private Sub RePaint()
        Call mcls_RpiView.RePaint()
    End Sub

    ''' <summary>
    ''' 画像再生成
    ''' </summary>
    Private Sub ReCreateImg()
        ' 画像を変更するため 画像を作り直し
        Call mcls_RpiView.ReCreateImg()
        Call mcls_RpiView.RePaint()
    End Sub

    ''' <summary>
    ''' 画像サイズ変更
    ''' </summary>
    Private Sub ReSizeImg(ByVal s As Size) Handles mcls_RpiView.tEv_ReSize
        'm_bResizeSkip = True

        If s.Width <> picFrameImg.ClientSize.Width Then
            ' リサイズ完了時には 画像を作り直し
            Call mcls_RpiView.ReCreateImg()
        End If

        ' 変更結果を他クラスに通知
        Call RePaint()  ' 再描画
    End Sub

    ''' <summary>
    ''' オーバーレイ再セット
    ''' </summary>
    Private Sub ReSetKizuOvl()
        If Not m_bDb Or Not m_bDrawKizuOvl Then Return
        Call SetKizuOvl(mcls_RpiView.GetDspAriaInf.nDspFno + 1, mcls_RpiView.GetDspAriaInf.nDspFnoEnd + 1)
        Call RePaint()
    End Sub

    ''' <summary>
    ''' 疵表示状態変更
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReSetKizuDsp()
        If Not m_bDb Or Not m_bDrawKizuOvl Then Return

        Call mfrm_KizuMap.SetData_Coil(m_typCoil)
        Call mfrm_DefMap.MapDisp()
        Call mfrm_KizuMap.MapDisp()
        Call mfrm_KizuList.MapDisp()

        Call SetKizuOvl(mcls_RpiView.GetDspAriaInf.nDspFno + 1, mcls_RpiView.GetDspAriaInf.nDspFnoEnd + 1)
        Call RePaint()

    End Sub



    ''' <summary>
    ''' 画面表示位置を取得 [mm]
    ''' </summary>
    ''' <param name="nXY">0:X 1:Y</param>
    ''' <param name="dRate">0:先端  1:尾端</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetPos(ByVal nXY As Integer, ByVal dRate As Double) As Integer
        Dim DspInf As tClass_ViewRpiThread.DSP_ARIA_INF = mcls_RpiView.GetDspAriaInf
        Dim nWkDp As Integer
        Dim nAriaDp As Integer  ' 表示エリア
        If 0 = nXY Then
            nWkDp = mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X
            nAriaDp = mcls_RpiView.GetImg_AriaInf.nAriaWidth

        Else
            nWkDp = mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y
            nAriaDp = mcls_RpiView.GetImg_AriaInf.nAriaHeight

        End If
        Return CInt(mcls_RpiView.GetPos_DpToLen_mm(CInt(nWkDp + nAriaDp * dRate)))
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 描画

    ''' <summary>
    ''' ピクチャーボックス クリック
    ''' </summary>
    Private Sub picFrameImg_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        If Not mcls_RpiView.IsRpi() Then Return

        If m_active_mouse_handler IsNot Nothing Then
            If m_active_mouse_handler.OnMouseClick(e) Then
                Return
            End If
        End If


        ' 再描画
        Call mcls_RpiView.RePaint()
    End Sub

    ''' <summary>
    ''' 画像表示クラス描画完了イベント
    ''' </summary>
    Private Sub mcls_RpiView_tEv_Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs) Handles mcls_RpiView.tEv_Paint_End
        ' ベース情報取得
        Dim ImgInf As tClass.tClass_ImageDsp.IMG_INF = mcls_RpiView.GetImg_ImgInf()
        Dim ImgKind As tClass.tClass_ImageDsp.IMG_KIND = mcls_RpiView.GetImg_ImgKind()
        Dim AriaInf As tClass.tClass_ImageDsp.ARIA_INF = mcls_RpiView.GetImg_AriaInf()
        Dim CalcAns As tClass.tClass_ImageDsp.CALC_ANS = mcls_RpiView.GetImg_CalcAns()

        Dim DspInf As tClass_ViewRpiThread.DSP_ARIA_INF = mcls_RpiView.GetDspAriaInf()

        For Each d As RpiPlugin.IDrawer In m_plugins.Drawers
            d.OnDraw(e.Graphics)
        Next


        ' 基本
        Dim cBsh As SolidBrush = New SolidBrush(Color.Cyan)
        Dim cFnt As Font = New System.Drawing.Font("ＭＳ ゴシック", 16, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
        Dim cPen As Pen = New System.Drawing.Pen(Color.Cyan, 2)


        ' ================================================
        ' サンプル専用
        If 1 = g_nLockMode Then
            If mcls_RpiView.PropRotate90 Then

                If 20000 < mcls_RpiView.GetPos_DpToLen_Y(DspInf.DspPixelTotalOffset.Y) Then
                    e.Graphics.FillRectangle(Brushes.Black, 0, 0, picFrameImg.ClientSize.Width, picFrameImg.ClientSize.Height)
                End If

                cBsh.Color = Color.Red
                Using wkFnt As Font = New System.Drawing.Font("ＭＳ ゴシック", 50, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
                    e.Graphics.DrawString("サンプル版 (先端20mのみ)", wkFnt, cBsh, 10, 10)
                End Using

            End If
        End If





        ' ================================================
        ' 左右文字描画
        Dim nIndex As Integer
        cBsh.Color = Color.Cyan
        '左描画
        nIndex = IIf(mcls_RpiView.GetCls_ImgDsp.tProp_ImgKind_IsTurnLR, 1, 0)
        If Not mcls_RpiView.PropRotate90 Then
            e.Graphics.DrawString(m_sDrawStrLR(nIndex), cFnt, cBsh, 5, 5)
        Else
            e.Graphics.DrawString(m_sDrawStrLR(nIndex), cFnt, cBsh, 5, picFrameImg.ClientSize.Height - 20)
        End If
        '右描画
        nIndex = IIf(mcls_RpiView.GetCls_ImgDsp.tProp_ImgKind_IsTurnLR, 0, 1)
        If Not mcls_RpiView.PropRotate90 Then
            e.Graphics.DrawString(m_sDrawStrLR(nIndex), cFnt, cBsh, picFrameImg.ClientSize.Width - 20, 5)
        Else
            e.Graphics.DrawString(m_sDrawStrLR(nIndex), cFnt, cBsh, 5, 5)
        End If



        ' ================================================
        ' エッジ位置
        Try
            ' 描画種類
            Dim nAddDsp As Integer = 0                                          ' 1:上下に小さく 2:縦全体 0:無し 
            If mcls_RpiView.GetRpi_Header().IsTHeader AndAlso 1 = mcls_RpiView.GetRpi_THeader().frame_info_type Then
                If m_bDrawEdge Then
                    nAddDsp = 2
                Else
                    nAddDsp = 1
                End If
            End If


            '' 追加描画
            If 0 <> nAddDsp Then
                Dim nWk As Integer
                Dim typFinfo As tClass_ReadRPI.T_RPI_FRAME_INFO_T1
                If 1 = nAddDsp Then
                    Using wkPen As Pen = New System.Drawing.Pen(Color.Magenta, 2)

                        Dim nSFno As Integer = DspInf.nDspFno + 1
                        Dim nEndFno As Integer = mcls_RpiView.GetOffset_TotalToOne_FrameNo(DspInf.DspPixelTotalOffset.Y + picFrameImg.ClientSize.Height) + 1
                        If nSFno <= 0 Then nSFno = 1
                        If nSFno >= mcls_RpiView.GetRpi_Header().nframe Then nSFno = mcls_RpiView.GetRpi_Header().nframe
                        If nEndFno <= 0 Then nEndFno = 1
                        If nEndFno >= mcls_RpiView.GetRpi_Header().nframe Then nEndFno = mcls_RpiView.GetRpi_Header().nframe


                        ' エッジ位置表示 上
                        If nSFno > 0 Then
                            typFinfo = mcls_RpiView.GetRpi_Cls().GetRpiFrameAdd1(nSFno) ' 付加情報
                            For ii As Integer = 0 To 1  ' 左右
                                nWk = mcls_RpiView.GetPosition(typFinfo.nEdgeIdx(ii), 0).X  ' 左
                                If Not mcls_RpiView.PropRotate90 Then
                                    e.Graphics.DrawLine(wkPen, nWk, 0, nWk, 5)
                                Else
                                    e.Graphics.DrawLine(wkPen, 0, nWk, 5, nWk)
                                End If

                            Next
                        End If

                        ' エッジ位置表示 下
                        typFinfo = mcls_RpiView.GetRpi_Cls().GetRpiFrameAdd1(nEndFno)  ' 付加情報
                        For ii As Integer = 0 To 1  ' 左右
                            nWk = mcls_RpiView.GetPosition(typFinfo.nEdgeIdx(ii), 0).X  ' 左
                            If Not mcls_RpiView.PropRotate90 Then
                                e.Graphics.DrawLine(wkPen, nWk, picFrameImg.ClientSize.Height - 5, nWk, picFrameImg.ClientSize.Height)
                            Else
                                e.Graphics.DrawLine(wkPen, picFrameImg.ClientSize.Width - 5, nWk, picFrameImg.ClientSize.Width, nWk)
                            End If

                        Next
                    End Using
                ElseIf 2 = nAddDsp Then
                    Using wkPen As Pen = New System.Drawing.Pen(Color.Magenta, 1)
                        For ii As Integer = DspInf.nDspFno To DspInf.nDspFnoEnd
                            If ii < 0 Then Continue For
                            If ii >= mcls_RpiView.GetRpi_Header().nframe Then Continue For

                            typFinfo = mcls_RpiView.GetRpi_Cls().GetRpiFrameAdd1(ii + 1)    ' 付加情報
                            If typFinfo.nEdgeIdx Is Nothing Then Continue For

                            Dim ys As Integer = mcls_RpiView.GetOffsetY_OneToTotal(ii, 0)
                            Dim ye As Integer = mcls_RpiView.GetOffsetY_OneToTotal(ii + 1, 0)
                            ys -= DspInf.DspPixelTotalOffset.Y
                            ye -= DspInf.DspPixelTotalOffset.Y

                            For jj As Integer = 0 To 1  ' 左右
                                nWk = mcls_RpiView.GetPosition(typFinfo.nEdgeIdx(jj), 0).X  ' 左
                                If Not mcls_RpiView.PropRotate90 Then
                                    e.Graphics.DrawLine(wkPen, nWk, ys, nWk, ye)
                                Else
                                    e.Graphics.DrawLine(wkPen, ys, nWk, ye, nWk)
                                End If
                            Next
                        Next
                    End Using
                End If

            End If
        Catch ex As Exception
        End Try

        ' ================================================
        ' グリッド線

        If mfrm_GridLine.Visible Then
            Dim nWk As Integer

            ' 長手、幅を縦横に変換
            Dim nIntvalX As Integer
            Dim nIntvalY As Integer
            Dim nFrmX As Integer
            Dim nFrmY As Integer
            If Not mcls_RpiView.PropRotate90 Then
                nIntvalX = mfrm_GridLine.g_nX
                nIntvalY = mfrm_GridLine.g_nY
                nFrmX = picFrameImg.Size.Width
                nFrmY = picFrameImg.Size.Height
            Else
                nIntvalX = mfrm_GridLine.g_nY
                nIntvalY = mfrm_GridLine.g_nX
                nFrmX = picFrameImg.Size.Height
                nFrmY = picFrameImg.Size.Width
            End If

            ' フォームの縦、横の考え方のまま
            Dim nDspTotalOffY As Integer = DspInf.DspPixelTotalOffset.Y + mcls_RpiView.GetPos_LenToDp_Y(mfrm_GridLine.g_nOffsetY)
            Dim nDspTotalOffX As Integer = DspInf.DspPixelTotalOffset.X + mcls_RpiView.GetPos_LenToDp_X(mfrm_GridLine.g_nOffsetX)

            Dim nY(2 - 1) As Integer  ' メモリ描画先頭/尾端のmm
            nY(0) = CInt(mcls_RpiView.GetPos_DpToLen_mm(nDspTotalOffY) / mfrm_GridLine.g_nY) * mfrm_GridLine.g_nY
            nY(1) = CInt(mcls_RpiView.GetPos_DpToLen_mm(nDspTotalOffY + nFrmY))

            Dim nX(2 - 1) As Integer  ' メモリ描画先頭/尾端のmm
            nX(0) = CInt(mcls_RpiView.GetPos_DpToLen_X(nDspTotalOffX) / mfrm_GridLine.g_nX) * mfrm_GridLine.g_nX
            nX(1) = CInt(mcls_RpiView.GetPos_DpToLen_X(nDspTotalOffX + nFrmX))

            ' 描画
            Using wkPen As Pen = New System.Drawing.Pen(Color.Red, 1)
                wkPen.DashStyle = Drawing2D.DashStyle.Dash

                For ii As Integer = nY(0) To nY(1) Step mfrm_GridLine.g_nY
                    nWk = mcls_RpiView.GetPos_LenToDp_Y(ii) - nDspTotalOffY    ' mmからpixelに変換

                    If Not mcls_RpiView.PropRotate90 Then
                        e.Graphics.DrawLine(wkPen, 0, nWk, nFrmX, nWk)
                    Else
                        e.Graphics.DrawLine(wkPen, nWk, 0, nWk, nFrmX)
                    End If
                Next

                For ii As Integer = nX(0) To nX(1) Step mfrm_GridLine.g_nX
                    nWk = mcls_RpiView.GetPos_LenToDp_X(ii) - nDspTotalOffX    ' mmからpixelに変換
                    If Not mcls_RpiView.PropRotate90 Then
                        e.Graphics.DrawLine(wkPen, nWk, 0, nWk, nFrmY)
                    Else
                        e.Graphics.DrawLine(wkPen, 0, nWk, nFrmY, nWk)
                    End If
                Next
            End Using
        End If

        ' =================================
        ' カット線  ※90度未対応
        If 0 <> m_lstCut.Count Then
            Dim nMmS As Single = mcls_RpiView.GetPos_DpToLen_mm(DspInf.DspPixelTotalOffset.Y)
            Dim nMmE As Single = mcls_RpiView.GetPos_DpToLen_mm(DspInf.DspPixelTotalOffset.Y + picFrameImg.ClientSize.Height) + 1
            Dim nDspTotalOffY As Integer = DspInf.DspPixelTotalOffset.Y + mcls_RpiView.GetPos_LenToDp_Y(mfrm_GridLine.g_nOffsetY)

            For ii As Integer = 0 To m_lstCut.Count - 1
                Dim wk As DATA_CUT_INF = m_lstCut(ii)
                If wk.nY >= nMmS And wk.nY <= nMmE Then
                    Using wkPen As Pen = New System.Drawing.Pen(Color.Green, 2)
                        wkPen.DashStyle = Drawing2D.DashStyle.Dash

                        Dim nWk As Integer = mcls_RpiView.GetPos_LenToDp_mm(wk.nY) - nDspTotalOffY  ' mmからpixelに変換

                        e.Graphics.DrawLine(wkPen, 0, nWk, picFrameImg.ClientSize.Width, nWk)
                    End Using
                End If
            Next
        End If




        ' =================================
        ' ブックマーク
        Call mfrm_Bookmark.PaintPos(e.Graphics, DspInf.nDspFno, DspInf.nDspFrameCnt)


        ' =================================
        ' 選択位置
        Call mfrm_InputList.PaintPos(e.Graphics, DspInf.nDspFno, DspInf.nDspFrameCnt)


        ' ================================================
        ' TEXヘッダー有りの時のみ 付加情報使って画面追加描画
        Using wkFnt As Font = New System.Drawing.Font("ＭＳ ゴシック", 10, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
            '' 判定
            Dim nDspFno As Integer = DspInf.nDspFno     ' 表示先頭フレームNo (0オリジン)
            Dim bAddDsp As Boolean = m_bDebug
            If Not mcls_RpiView.GetRpi_Header().IsTHeader Then bAddDsp = False
            If 1 <> mcls_RpiView.GetRpi_THeader().frame_info_type Then bAddDsp = False
            If nDspFno < 0 Then bAddDsp = False
            If nDspFno >= mcls_RpiView.GetRpi_Header().nframe Then bAddDsp = False
            '' 追加描画
            If bAddDsp Then
                Dim sWk As String = ""
                Dim typFinfo As tClass_ReadRPI.T_RPI_FRAME_INFO_T1 = mcls_RpiView.GetRpi_Cls().GetRpiFrameAdd1(nDspFno + 1) ' 付加情報


                ' フレーム情報
                Dim nX As Integer = AriaInf.nAriaWidth - 100
                Dim nY As Integer = 5

                sWk = String.Format("Fno={0}", typFinfo.nFNo)
                e.Graphics.DrawString(sWk, wkFnt, cBsh, nX, nY)
                nY += 10
                sWk = String.Format("len={0}", typFinfo.nFromLen / 1000)
                e.Graphics.DrawString(sWk, wkFnt, cBsh, nX, nY)
                nY += 10

                sWk = String.Format("mpm={0}", CInt(typFinfo.nSpeed * 60.0 / 1000))
                e.Graphics.DrawString(sWk, wkFnt, cBsh, nX, nY) '
                nY += 10
                sWk = String.Format("vRes={0:0.00000}", typFinfo.vRes)
                e.Graphics.DrawString(sWk, wkFnt, cBsh, nX, nY)
            End If
        End Using
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ もろもろ RPI 操作

    ''' <summary>
    ''' 画像生成イベント (別スレッド)
    ''' </summary>
    Private Sub mcls_RpiView_tEv_CreatingImg(ByVal p As tClass_ViewRpiThread.CLS_IMG_INF, ByRef img As Bitmap) Handles mcls_RpiView.tEv_CreatingImg
        'img.Save(String.Format(".\r{0}.bmp", p.nIndex), Drawing.Imaging.ImageFormat.Bmp)
        For Each f As RpiPlugin.IFilterBitmap In m_plugins.FilterBitmaps
            f.OnFilterImage(img, p)
        Next

        '=====================================

        'img.Save(String.Format(".\d{0}.bmp", p.nIndex), Drawing.Imaging.ImageFormat.Bmp)

    End Sub


    ''' <summary>
    ''' 幅方向軸用に、中心から、Interval間隔（両方向）でラベルを作成する
    ''' </summary>
    ''' <param name="pxCcenter">中心</param>
    ''' <param name="mmInterval">メモリ間隔</param>
    ''' <param name="nCycle">文字線間隔</param>
    ''' <param name="rateDw">mm->px比率</param>
    ''' <returns>メモリ座標・ラベル配列</returns>
    ''' <remarks>ラベルはメモリの２点毎で作成</remarks>
    Function GenerateHabaLabels(ByVal dspxTotalOffset As Integer, ByVal pxCcenter As Integer, ByVal mmInterval As Integer, ByVal nCycle As Integer, ByVal rateDw As Double) _
     As List(Of LabelInfo)

        Dim rlist As New List(Of LabelInfo)

        '中心
        rlist.Add(New LabelInfo(pxCcenter - dspxTotalOffset, "0mm"))

        'プラスマイナス15本
        For ii As Integer = 1 To 30
            For jj As Integer = -1 To 1 Step 2 'プラスマイナス
                Dim pp As Integer = CInt(pxCcenter + jj * ii * mmInterval * rateDw) - dspxTotalOffset
                'ラベルは １飛ばしで表示
                'If 0 = (ii Mod 2) Then
                If 0 = (ii Mod nCycle) Then
                    rlist.Add(New LabelInfo(pp, CInt(ii * mmInterval).ToString()))
                Else
                    'メモリのみ
                    rlist.Add(New LabelInfo(pp, ""))
                End If
            Next
        Next
        Return rlist
    End Function


    ''' <summary>
    ''' 表示先頭フレームNo変更
    ''' </summary>
    ''' <param name="nFrameNoStart">今回の表示先頭フレームNo(1オリジン)</param>
    ''' <param name="nFrameNoEnd">今回の表示後端フレームNo(1オリジン)</param> 
    Private Sub mcls_RpiView_tEv_ChangeDspFrameNo(ByVal nFrameNoStart As Integer, ByVal nFrameNoEnd As Integer) Handles mcls_RpiView.tEv_ChangeDspFrameNo
        ' 疵オーバーレイ
        If m_bDb And m_bDrawKizuOvl Then
            SetKizuOvl(nFrameNoStart, nFrameNoEnd)
        End If

        ' ステータス情報エリア更新
        tslFrameNowCnt.Text = CStr(mcls_RpiView.GetDspAriaInf.nDspFno + 1)
        tlsMainMenu.Refresh()
    End Sub

    ''' <summary>
    ''' マウスホイール
    ''' </summary>
    Private Sub frmMain_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles Me.MouseWheel ', spcMain.MouseWheel
        Dim evDetail As RpiPlugin.IMouseInputDetail = TryCast(m_active_mouse_handler, RpiPlugin.IMouseInputDetail)
        If evDetail IsNot Nothing Then
            If evDetail.OnMouseWheel(e) Then
                Return
            End If
        End If

        If mcls_ImgCutDsp.IsDsp Then Return ' 右クリック中は、拡大表示中の為、スクロールしない
        Dim nVal As Integer = scrNagate.Value
        If e.Delta > 0 Then
            nVal -= scrNagate.SmallChange
        Else
            nVal += scrNagate.SmallChange
        End If

        nVal = LimitByScrollMinMax(scrNagate, nVal)

        Call scrNagate_ValueChanged(nVal, True)
    End Sub
    ''' <summary>
    ''' 縦スクロール
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub vsBar_ValueChanged(ByVal sender As Object, ByVal e As Boolean) Handles vsBar.ValueChanged
        If e = True Then Return ' プログラム中で変更した場合は、無処理

        ' スクロール移動
        Dim nVal As Integer = CType(sender, ScrollBar).Value
        If vsBar Is scrNagate Then
            Call scrNagate_ValueChanged(nVal, e)


            ''''' 連携用 として共有メモリに位置書込み
            'Call SetMemValLen()

        Else
            Call scrHaba_ValueChanged(nVal, e)
        End If
    End Sub
    ''' <summary>
    ''' 長手スクロール
    ''' </summary>
    ''' <param name="nVal">変更値</param>
    ''' <param name="bIsProgramable">プログラムで勝手に変えた場合true</param>
    ''' <remarks></remarks>
    Private Sub scrNagate_ValueChanged(ByVal nVal As Integer, ByVal bIsProgramable As Boolean)
        'If m_skip Then
        '	Threading.Thread.Sleep(100)
        '	'Return
        'End If
        'm_skip = True

        If nVal <> mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y Then
            Call mcls_RpiView.ScrollH(nVal, bIsProgramable)     ' スクロールかプログラムかは、引数により変更
            Call mcls_RpiView.RePaint()
        End If
    End Sub

    ''' <summary>
    ''' 横スクロール
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub hsBar_ValueChanged(ByVal sender As Object, ByVal e As Boolean) Handles hsBar.ValueChanged
        If e = True Then Return ' プログラム中で変更した場合は、無処理

        ' スクロール移動
        Dim nVal As Integer = CType(sender, ScrollBar).Value
        If hsBar Is scrHaba Then
            Call scrHaba_ValueChanged(nVal, e)
        Else
            Call scrNagate_ValueChanged(nVal, e)
        End If
    End Sub
    ''' <summary>
    ''' 幅スクロール
    ''' </summary>
    ''' <param name="nVal">変更値</param>
    ''' <param name="bIsProgramable">プログラムで勝手に変えた場合true</param>
    ''' <remarks></remarks>
    Private Sub scrHaba_ValueChanged(ByVal nVal As Integer, ByVal bIsProgramable As Boolean)
        If nVal <> mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X Then
            Call mcls_RpiView.ScrollW(nVal, bIsProgramable)     ' スクロールかプログラムかは、引数により変更
            Call mcls_RpiView.RePaint()
        End If
    End Sub


    Enum JumpMode
        None
        AbsMM
        AbsMM_Center
        Relative
    End Enum
    ''' <summary>
    ''' 指定位置移動 長手方向
    ''' </summary>
    ''' <param name="mode">Yモード (0:無視 1:指定値(先頭) 2:指定値(中央) 3:現在から差分)</param>
    ''' <param name="len">[mm]</param>
    Private Sub JumpNagate(ByVal mode As Integer, ByVal len As Single)
        If 0 = mode Then Exit Sub

        Dim nPixel As Integer   ' pixel
        If 1 = mode Then
            '絶対移動では、オフセットの影響をうけるので、mmが正解。
            nPixel = mcls_RpiView.GetPos_LenToDp_mm(len)

        ElseIf 2 = mode Then
            '絶対移動では、オフセットの影響をうけるので、mmが正解。
            Dim nWkPix As Integer = mcls_RpiView.GetPos_LenToDp_mm(len)
            nPixel = nWkPix - CInt(picFrameImg.ClientSize.Height / 2)

        ElseIf 3 = mode Then
            'こいつは純粋な距離なので、オフセットをかけてはいけない。
            Dim nWkPix As Integer = mcls_RpiView.GetPos_LenToDp_Y(len)
            nPixel = nWkPix + mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y
        End If


        'Dim nPixelY As Integer = mcls_RpiView.GetPos_LenToDp_Y(nPosY)
        nPixel = LimitByScrollMinMax(scrNagate, nPixel)
        scrNagate_ValueChanged(nPixel, True)
    End Sub
    ''' <summary>
    ''' 指定位置移動 幅方向
    ''' </summary>
    ''' <param name="mode">Xモード (0:無視 1:指定値(先頭) 2:指定値(中央) 3:現在から差分)</param> 
    ''' <param name="len">[mm]</param>
    ''' <param name="lenMode">lenがどっちの指定から？(0:左端  1:センターから)</param>
    Private Sub JumpHaba(ByVal mode As Integer, ByVal len As Integer, ByVal lenMode As Integer)
        If 0 = mode Then Exit Sub

        If 1 = lenMode Then
            ' センター基準で来ているため、左端基準に直す
            len += CInt(mcls_RpiView.GetPos_RpToLen_X(mcls_RpiView.GetImg_ImgInf().nOneImgW) / 2)
        End If


        Dim nPixel As Integer   ' pixel
        Dim nWkPix As Integer = mcls_RpiView.GetPos_LenToDp_X(len)
        If 1 = mode Then
            nPixel = nWkPix

        ElseIf 2 = mode Then
            nPixel = nWkPix - CInt(picFrameImg.ClientSize.Width / 2)

        ElseIf 3 = mode Then
            nPixel = nWkPix + mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X
        End If


        nPixel = LimitByScrollMinMax(scrHaba, nPixel)
        scrHaba_ValueChanged(nPixel, True)
    End Sub

    ''' <summary>
    ''' 指定位置にジャンプ
    ''' </summary>
    ''' <param name="nMode">-1:先頭 1:尾端 0:任意位置</param>
    ''' <param name="nVal">nMode0のときのみ。差分距離</param>
    ''' <remarks></remarks>
    Private Sub Map_JumpNagate(ByVal nMode As Integer, ByVal nVal As Integer)
        Dim mmJumpTo As Single = 0
        If 0 = nMode Then
            Call JumpNagate(3, nVal)
        ElseIf -1 = nMode Then
            mmJumpTo = mcls_RpiView.GetPos_DpToLen_mm(vsBar.Minimum)
            If mmJumpTo < 0 Then mmJumpTo = 0
            Call JumpNagate(1, mmJumpTo)
        ElseIf 1 = nMode Then
            Dim tHeader As tClass_ReadRPI.RPI_T_HEADER = mcls_RpiView.GetRpi_THeader
            Dim pxTailpos As Integer = tHeader.nFTposCfNo(1) * mcls_RpiView.GetRpi_Header.height + tHeader.nFTposPixel(1)
            If pxTailpos <> 0 Then
                mmJumpTo = mcls_RpiView.GetPos_RpToLen_mm(pxTailpos - mcls_RpiView.GetPos_DpToRp_Y(mcls_RpiView.GetImg_AriaInf.nAriaHeight))
            Else
                mmJumpTo = mcls_RpiView.GetPos_DpToLen_Y(vsBar.MaximumVirtual)
            End If
            Call JumpNagate(1, mmJumpTo)
        End If
    End Sub



    ''' <summary>
    ''' マップクリック (表示座標系) ※全長画像の座標 = センターは画像のセンター
    ''' </summary>
    ''' <param name="pos">[mm]</param>
    ''' <remarks></remarks>
    Private Sub SubMapClick(ByVal pos As System.Drawing.Point) Handles mfrm_KizuMap.tEv_MapClick
        Call JumpNagate(2, pos.Y)       ' 先頭から
        Call JumpHaba(2, pos.X, 1)      ' センター基準から
    End Sub

    ''' <summary>
    ''' マップクリック (疵座標系) ※φωの座標 = センターは板センター
    ''' </summary>
    ''' <param name="pos">[mm]</param>
    ''' <remarks></remarks>
    Private Sub SubKizuClick(ByVal pos As System.Drawing.Point)
        pos.Y += CInt((mcls_RpiView.GetRpi_THeader.nFTposCfNo(0) * mcls_RpiView.GetRpi_Header.height + mcls_RpiView.GetRpi_THeader.nFTposPixel(0)) * mcls_RpiView.GetRpi_Header.res_y)

        Call JumpNagate(2, pos.Y)       ' 先頭から
        Call JumpHaba(2, pos.X, 1)      ' センター基準から
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ちょっとしたイベント


    ''' <summary>
    ''' フレーム情報読込
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub nudFrameNo_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles nudFrameNo.ValueChanged
        If Not mcls_RpiView.IsRpi Then Return

        ' 左欄
        Dim fno As Integer = CInt(nudFrameNo.Value)
        Dim head As tClass.tClass_ReadRPI.RPI_HEADER = mcls_RpiView.GetRpi_Header
        Dim inf As tClass.tClass_ReadRPI.RPI_FRAME_INF = mcls_RpiView.GetRpi_FrameInf(fno)

        lblImgPos.Text = inf.img_pos.ToString
        lblImgSize.Text = inf.img_size.ToString
        lblInfPos.Text = inf.inf_pos.ToString

        ' エッジ情報
        If head.frame_info_type = tClass.tClass_ReadRPI.EM_FRM_TYPE.EDGE_PER_FRAME Then
            Dim typ As tClass.tClass_ReadRPI.EDGE_PER_FRAME = mcls_RpiView.ReadFile_FrameInfo_EdgePerFrame(fno)
            lblEdge.Text = String.Format("{0}, {1}", typ.edge_left, typ.edge_right)
        Else
            lblEdge.Text = ""
        End If
    End Sub


    ''' <summary>
    ''' RPI情報エリア 表示非表示切替
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub tsbInfMode_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbInfMode.Click
        spcMain.Panel1Collapsed = Not spcMain.Panel1Collapsed
    End Sub



    Dim m_frmPopRpiPath As New frmPopRpiPath

    ''' <summary>
    ''' フォルダ表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub tsbFolder_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbFolder.Click
        m_frmPopRpiPath.ShowToggle(Me)
    End Sub


    ''' <summary>
    ''' オーバーレイ
    ''' </summary>
    Private Sub tsbOVL_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbOVL.Click
        m_bDrawKizuOvl = Not m_bDrawKizuOvl

        If m_bDb And m_bDrawKizuOvl Then
            SetKizuOvl(mcls_RpiView.GetDspAriaInf.nDspFno + 1, mcls_RpiView.GetDspAriaInf.nDspFnoEnd + 1)
        Else
            mcls_RpiView.ResetOverLayInf()
        End If
        Call RePaint()
    End Sub
    ''' <summary>
    ''' エッジ位置描画
    ''' </summary>
    Private Sub tsbEdge_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbEdge.Click
        m_bDrawEdge = Not m_bDrawEdge
        Call RePaint()
    End Sub






    ''' <summary>
    ''' 倍率変更
    ''' </summary>
    Private Sub tscRate_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles tscRate.SelectedIndexChanged
        Dim dRate As Double = 1.0

        '''' 現在中心位置を覚えておく
        Dim nX As Integer = GetPos(0, 0.5)  ' [mm]
        Dim nY As Integer = GetPos(1, 0.5)  ' [mm]


        '''' 表示倍率 決定
        Select Case tscRate.SelectedIndex
            Case 0          ' 幅全部
                dRate = 1
            Case 1          ' 元画像1/4
                dRate = 0.25
            Case 2          ' 元画像1/2
                dRate = 0.5
            Case 3          ' 元画像1倍
                dRate = 1
            Case 4          ' 2倍
                dRate = 2
            Case 5          ' 3倍
                dRate = 3
            Case 6          ' 4倍
                dRate = 4
            Case 7          ' 5倍
                dRate = 5
            Case Else
        End Select

        ' 画像表示モード
        If tscRate.SelectedIndex = 0 Then       ' 表示エリア基準
            Call mcls_RpiView.SetProp_ImgKind_StrechType(tClass.tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechWidth)
        Else                                    ' 画像サイズ基準
            Call mcls_RpiView.SetProp_ImgKind_StrechType(tClass.tClass_ImageDsp_Calc.EM_IMG_STRECH.emNormal)
        End If

        ' ズーム設定
        Call mcls_RpiView.SetProp_Zoom(dRate)

        ' イメージを即時作り直し
        Call mcls_RpiView.ResizePaintAria(Nothing, Nothing)
        Call ReCreateImg()


        '''' 部分拡大表示クラスにも表示倍率が変更された事を通知
        'Call mcls_ImgCutDsp.SetItemRes(mcls_RpiView.GetImg_CalcAns.FinalRes.dW, mcls_RpiView.GetImg_CalcAns.FinalRes.dH)		' 現在の倍率をセット


        '''' 元の中心位置を基準に移動
        Call JumpNagate(2, nY)
        Call JumpHaba(2, nX, 0)

    End Sub

    ''' <summary>
    ''' チャンネル切り替え
    ''' </summary>
    Private Sub tscChannel_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles tscChannel.SelectedIndexChanged
        If mcls_RpiView.GetRpi_Cls.PropRpiActiveCh = tscChannel.SelectedIndex Then Return

        mcls_RpiView.GetRpi_Cls.PropRpiActiveCh = tscChannel.SelectedIndex  'チャンネル切り替え

        ' イメージを即時作り直し
        Call ReCreateImg()
    End Sub


    ''' <summary>
    ''' 開発用
    ''' </summary>
    Private Sub tsbImgQuarty_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbImgQuarty.Click
        mfrm_Kai.ShowToggle(Me)
    End Sub

    ''' <summary>
    ''' FT 反転
    ''' </summary>
    Private Sub tsbTB_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbTB.Click
        mcls_RpiView.SetProp_TB(IIf(tsbTB.Checked, 1, 0))

        'Call mcls_Param.ReadZi_DefectInfReverce(mcls_RpiView, m_lstDetect)	' 反転設定
        Call ReverceDsp()
        Call ReCreateImg()
    End Sub
    ''' <summary>
    ''' DS WS反転
    ''' </summary>
    Private Sub tsbDSWS_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbDSWS.Click
        mcls_RpiView.SetProp_DSWS(IIf(tsbDSWS.Checked, 1, 0))

        'Call mcls_Param.ReadZi_DefectInfReverce(mcls_RpiView, m_lstDetect)	' 反転設定
        Call ReverceDsp()
        Call ReCreateImg()
    End Sub
    ''' <summary>
    ''' 回転の変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub tsbRot_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbRot.Click
        Call SetRotate(Not mcls_RpiView.PropRotate90)
    End Sub
    Private Sub SetRotate(ByVal bUmu As Boolean)

        '描画側にも伝達
        mcls_RpiView.PropRotate90 = bUmu

        '回転したので、｛長手・幅｝軸と｛縦・横｝のスクロールバーの対応をとりなおす
        If mcls_RpiView.PropRotate90 Then
            scrHaba = Me.vsBar
            scrNagate = Me.hsBar
        Else
            scrHaba = Me.hsBar
            scrNagate = Me.vsBar
        End If

        'スクロール量も変化する。
        scrNagate.Maximum = 500         ' LargeChange以上の値を入れておかないと、勝手にLargeChangeを小さくしてしまう
        scrNagate.SmallChange = 100
        scrNagate.LargeChange = 500

        scrHaba.Maximum = 500         ' LargeChange以上の値を入れておかないと、勝手にLargeChangeを小さくしてしまう
        scrHaba.SmallChange = 1
        scrHaba.LargeChange = 1

        'スクロールバーを作り直す
        Call mcls_RpiView.ResizePaintAria(Me, Nothing)

        '描画用イメージの作り直し＆再描画。
        Call ReCreateImg()
    End Sub


    ''' <summary>
    ''' 左右反転
    ''' </summary>
    ''' <param name="nMode">
    ''' -9:現表示に対して反転 
    ''' -1:元画像に対して反転無し(要はiniファイルで反転させたやつも戻す) 
    ''' 1:元画像に対して反転 
    ''' 2:iniファイル設定後に対して反転
    ''' </param>
    ''' <remarks></remarks>
    Private Sub TurnDSWS(ByVal nMode As Integer)
        If 0 = nMode Then Return
        Dim bTurn As Boolean = False        ' 変換結果

        '''' 変換判定
        ' 強制的に反転
        If -9 = nMode Then
            bTurn = Not tsbDSWS.Checked
        End If
        ' 強制的に反転無し = 絶対反転無し
        If -1 = nMode Then
            bTurn = False
        End If
        ' 生で判定 = 絶対反転
        If 1 = nMode Then
            bTurn = True
        End If
        ' Ini設定
        If 2 = nMode Then
            Dim nIni As Integer = tMod.tMod_IniFile.ReadIniInt("DB_" & mcls_RpiView.GetRpi_THeader.cSysName, "DorW", TASKINI_NAME)
            bTurn = Not CBool(nIni)
        End If


        '''' 結果反映
        If tsbDSWS.Checked = bTurn Then Return
        tsbDSWS.Checked = bTurn
        tsbDSWS_Click(Nothing, Nothing)
    End Sub

    ''' <summary>
    ''' 上下反転
    ''' </summary>
    ''' <param name="nMode">
    ''' -9:現表示に対して反転 
    ''' -1:元画像に対して反転無し(要はiniファイルで反転させたやつも戻す) 
    ''' 1:元画像に対して反転 
    ''' 2:iniファイル設定後に対して反転
    ''' </param>
    ''' <remarks></remarks>
    Private Sub TurnTB(ByVal nMode As Integer)
        If 0 = nMode Then Return
        Dim bTurn As Boolean = False        ' 変換結果

        '''' 変換判定
        ' 強制的に反転
        If -9 = nMode Then
            bTurn = Not tsbTB.Checked
        End If
        ' 強制的に反転無し = 絶対反転無し
        If -1 = nMode Then
            bTurn = False
        End If
        ' 生で判定 = 絶対反転
        If 1 = nMode Then
            bTurn = True
        End If
        ' Ini設定
        If 2 = nMode Then
            Dim nIni As Integer = tMod.tMod_IniFile.ReadIniInt("DB_" & mcls_RpiView.GetRpi_THeader.cSysName, "TorB", TASKINI_NAME)
            bTurn = Not CBool(nIni)
        End If


        '''' 結果反映
        If tsbTB.Checked = bTurn Then Return
        tsbTB.Checked = bTurn
        tsbTB_Click(Nothing, Nothing)
    End Sub

    ''' <summary>
    ''' 90度反転
    ''' </summary>
    ''' <param name="nMode">
    ''' -9:現表示に対して反転 
    ''' -1:元画像に対して反転無し(要はiniファイルで反転させたやつも戻す) 
    ''' 1:元画像に対して反転 
    ''' 2:iniファイル設定後に対して反転
    ''' </param>
    ''' <remarks></remarks>
    Private Sub TurnRotate(ByVal nMode As Integer)
        If 0 = nMode Then Return
        Dim bTurn As Boolean = False        ' 変換結果

        '''' 変換判定
        ' 強制的に反転
        If -9 = nMode Then
            bTurn = Not tsbRot.Checked
        End If
        ' 強制的に反転無し = 絶対反転無し
        If -1 = nMode Then
            bTurn = False
        End If
        ' 生で判定 = 絶対反転
        If 1 = nMode Then
            bTurn = True
        End If
        ' Ini設定
        If 2 = nMode Then
            Dim nIni As Integer = tMod.tMod_IniFile.ReadIniInt("DB_" & mcls_RpiView.GetRpi_THeader.cSysName, "Rotate", TASKINI_NAME)
            bTurn = Not CBool(nIni)
        End If


        '''' 結果反映
        If tsbRot.Checked = bTurn Then Return
        tsbRot.Checked = bTurn
        tsbRot_Click(Nothing, Nothing)
    End Sub


    ''' <summary>
    ''' 自動減速キャンセル
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub AutoDownCancel()
        ' ライン速度が0のときだけは、キャンセル扱い
        mcls_CheckKizu.AutoStopCancel()
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 表示画像 マウス操作関係

    Dim m_bIsImgDrag As Boolean = False             ' 画像を左クリックでつかんでいる最中？
    Dim m_PointS As Point                           ' 開始したマウス地点

    ''' <summary>
    ''' マウスカーソル
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub picFrameImg_MouseMove(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles picFrameImg.MouseMove
        Dim CalcAns As tClass.tClass_ImageDsp.CALC_ANS = mcls_RpiView.GetImg_CalcAns()
        Dim DspInf As tClass_ViewRpiThread.DSP_ARIA_INF = mcls_RpiView.GetDspAriaInf

        Dim nowPos As Point = picFrameImg.PointToClient(Windows.Forms.Cursor.Position)      ' 現在のカーソル位置

        ' 先頭からの元画像系Pixel
        Dim pixel As Point
        pixel.X = mcls_RpiView.GetPos_DpToRp_X(DspInf.DspPixelTotalOffset.X + nowPos.X)  'CInt((DspInf.DspPixelTotalOffset.X / CalcAns.RateDsp.dW) + (nowPos.X / CalcAns.RateDsp.dW))
        pixel.Y = mcls_RpiView.GetPos_DpToRp_Y(DspInf.DspPixelTotalOffset.Y + nowPos.Y) 'CInt((DspInf.DspPixelTotalOffset.Y / CalcAns.RateDsp.dH) + (nowPos.Y / CalcAns.RateDsp.dH))
        tslTotalPixel.Text = String.Format("{0:#,##0}/ {1:#,##0}", pixel.X, pixel.Y)

        ' 表示画像のm
        Dim pixelF As PointF
        pixelF.X = mcls_RpiView.GetPos_DpToLen_X(DspInf.DspPixelTotalOffset.X + nowPos.X) 'CSng(DspInf.DspPixelTotalOffset.X * CalcAns.FinalRes.dW) + CSng(nowPos.X * CalcAns.FinalRes.dW)
        pixelF.Y = mcls_RpiView.GetPos_DpToLen_mm(DspInf.DspPixelTotalOffset.Y + nowPos.Y) 'CSng(DspInf.DspPixelTotalOffset.Y * CalcAns.FinalRes.dH) + CSng(nowPos.Y * CalcAns.FinalRes.dH)
        tslTotalMm.Text = String.Format("{0:#,##0.000}/ {1:#,##0.000}", pixelF.X / 1000, pixelF.Y / 1000)

        Dim evDetail As RpiPlugin.IMouseInputDetail = TryCast(m_active_mouse_handler, RpiPlugin.IMouseInputDetail)
        If evDetail IsNot Nothing Then
            If evDetail.OnMouseMove(e) Then
                Return
            End If
        End If



        '''' 画像移動
        If m_bIsImgDrag Then
            Dim PointE As Point = picFrameImg.PointToClient(Windows.Forms.Cursor.Position)
            Dim p As Point

            If hsBar.Enabled Then
                p.X = LimitByScrollMinMax(hsBar, (m_PointS.X - PointE.X) + hsBar.Value)
                If Not hsBar.Value = p.X Then   ' △
                    If hsBar Is scrHaba Then
                        Call mcls_RpiView.ScrollW(p.X, True)
                    Else
                        Call mcls_RpiView.ScrollH(p.X, True)
                    End If
                End If
            End If

            If vsBar.Enabled Then
                p.Y = LimitByScrollMinMax(vsBar, (m_PointS.Y - PointE.Y) + vsBar.Value)

                If Not vsBar.Value = p.Y Then   ' △
                    If vsBar Is scrNagate Then
                        Call mcls_RpiView.ScrollH(p.Y, True)
                    Else
                        Call mcls_RpiView.ScrollW(p.Y, True)
                    End If
                End If
            End If

            ' 現在値を再セット
            m_PointS = picFrameImg.PointToClient(Windows.Forms.Cursor.Position)

            ' 全部終わったら再描画
            Call RePaint()
        End If
    End Sub


    ''' <summary>
    ''' マウスダウン
    ''' </summary>
    Private Sub picFrameImg_MouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles picFrameImg.MouseDown
        Dim evDetail As RpiPlugin.IMouseInputDetail = TryCast(m_active_mouse_handler, RpiPlugin.IMouseInputDetail)
        If evDetail IsNot Nothing Then
            If evDetail.OnMouseDown(e) Then
                Return
            End If
        End If

        If e.Button = Windows.Forms.MouseButtons.Left Then
            m_bIsImgDrag = True
            m_PointS = picFrameImg.PointToClient(Windows.Forms.Cursor.Position)
        End If
    End Sub


    ''' <summary>
    ''' マスウアップ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub picFrameImg_MouseUp(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles picFrameImg.MouseUp
        Dim evDetail As RpiPlugin.IMouseInputDetail = TryCast(m_active_mouse_handler, RpiPlugin.IMouseInputDetail)
        If evDetail IsNot Nothing Then
            If evDetail.OnMouseUp(e) Then
                Return
            End If
        End If

        If e.Button = Windows.Forms.MouseButtons.Left Then
            m_bIsImgDrag = False
        End If

        ' ボタンとかにフォーカスを当てておかないと、
        btnRituNon.Select()
    End Sub

    ''' <summary>
    ''' 自動スクロール(再描画) ＆ 遅延描画対応
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub timDsp_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles timDsp.Tick
        mcls_StopWatch.CounterCycle()


        If mcls_RpiView.IsRpi() Then
            Dim bMyMove As Boolean = True           ' 自分で通板時true。外部連動時false
            Dim nSpeed As Integer = 0               ' 今回のライン速度
            'If IsNumeric(tscSpeed.Text) Then nSpeed = CInt(tscSpeed.Text) 
            nSpeed = mfrm_FrmCont.LineSpeed

            Dim nIndex As Integer = scrNagate.Value ' 前回の距離 Y[DP]
            Dim nNewIndex As Integer                ' 今回の進み量 Y[DP]
            Dim nNewIndexX As Integer               ' 今回の進み量 X[DP]

            Dim bMyTaskActive As Boolean = Form.ActiveForm IsNot Nothing        ' 自タスク自体がアクティブ時 true


            ' ================================================
            ' 連携用
            '''' 他工程状態に追従 
            If Not m_memTrkPtr.Equals(IntPtr.Zero) AndAlso Not bMyTaskActive Then
                bMyMove = False

                '' 共有メモリ読込
                Dim tMemVal As TYPE_TRK_ITEM                        ' 自工程の情報構造体
                Dim nMemOffset As Integer = SIZE_TYPE_HEAD + SIZE_TYPE_ITEM * g_typStartArgs.nId        ' 自構造体位置
                tClass_GlovalMemory.GetMappedT(m_memTrkPtr, nMemOffset, tMemVal)

                ' 前回と同じ距離なら
                'If tMemVal.nY = Int(mcls_RpiView.GetPos_DpToLen_Y(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y)) And _
                'tMemVal.nX = Int(mcls_RpiView.GetPos_DpToLen_X(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X)) Then
                If 1 >= Math.Abs(tMemVal.nY - Int(mcls_RpiView.GetPos_DpToLen_Y(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y))) And _
                1 >= Math.Abs(tMemVal.nX - Int(mcls_RpiView.GetPos_DpToLen_X(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X))) Then
                    nSpeed = 0
                Else
                    nNewIndex = mcls_RpiView.GetPos_LenToDp_Y(tMemVal.nY)
                    nNewIndex = LimitByScrollMinMax(scrNagate, nNewIndex)

                    nNewIndexX = mcls_RpiView.GetPos_LenToDp_X(tMemVal.nX)
                    nNewIndexX = LimitByScrollMinMax(scrHaba, nNewIndexX)

                    nSpeed = 1 ' 連動して動かすように、0以外をセット
                End If
            End If

            '''' 簡易連動に追従 
            If Not m_memSyncPtr.Equals(IntPtr.Zero) AndAlso Not bMyTaskActive Then
                bMyMove = False

                Dim tMemVal As TYPE_SYNC                        ' 自工程の情報構造体
                tClass_GlovalMemory.GetMappedT(m_memSyncPtr, 0, tMemVal)

                ' 前回と同じ距離なら
                'If tMemVal.nY = Int(mcls_RpiView.GetPos_DpToLen_Y(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y)) - m_nSyncOffsetY And _
                'tMemVal.nX = Int(mcls_RpiView.GetPos_DpToLen_X(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X)) - m_nSyncOffsetX Then
                If 1 >= Math.Abs(tMemVal.nY - (Int(mcls_RpiView.GetPos_DpToLen_Y(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y)) - m_nSyncOffsetY)) And _
                1 >= Math.Abs(tMemVal.nX - (Int(mcls_RpiView.GetPos_DpToLen_X(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X)) - m_nSyncOffsetX)) Then
                    nSpeed = 0
                Else
                    nNewIndex = mcls_RpiView.GetPos_LenToDp_Y(tMemVal.nY + m_nSyncOffsetY)  ' オフセット分を加算
                    nNewIndex = LimitByScrollMinMax(scrNagate, nNewIndex)

                    nNewIndexX = mcls_RpiView.GetPos_LenToDp_X(tMemVal.nX + m_nSyncOffsetX)  ' オフセット分を加算
                    nNewIndexX = LimitByScrollMinMax(scrHaba, nNewIndexX)

                    nSpeed = 1 ' 連動して動かすように、0以外をセット
                End If
            End If


            'System.Console.WriteLine("{0}  {1}", bMyTaskActive, nNewIndex)


            ' ================================================
            ' 普通の処理
            '' 今回の進み量算出
            If bMyMove Then     ' 連携無しか、自アクティブ時のみ
                Dim nwk As Integer
                Dim dwk As Double

                dwk = nSpeed / 60.0         '[mm/msec]
                dwk = dwk * mcls_StopWatch.GetCycleTime()   ' タイマー時間辺り これだけ進む
                nwk = mcls_RpiView.GetPos_LenToDp_Y(dwk)   'dwk / mcls_RpiView.GetImg_CalcAns().FinalRes.dH		' 表示座標系のpixel
                If 0 = nwk AndAlso 0 < nSpeed Then nwk = 1
                If 0 = nwk AndAlso 0 > nSpeed Then nwk = -1
                nNewIndex = LimitByScrollMinMax(scrNagate, nIndex + nwk)
                'If nNewIndex <> nIndex + nwk Then
                '	nNewIndex = scrNagate.Minimum
                'End If
            End If


            ' ================================================
            ' 移動
            If nSpeed = 0 Then
                If Not mcls_RpiView.GetDspAriaInf_DspAllOk() Then
                    'Call mcls_RpiView.ScrollH(nIndex, True)
                    Call mcls_RpiView.ReSetImg(mcls_RpiView.GetDspAriaInf.nDspFno)
                    Call RePaint()
                End If
            Else
                Call mcls_RpiView.ScrollH(nNewIndex, True)
                If Not bMyMove Then mcls_RpiView.ScrollW(nNewIndexX, True) ' 連動のときのみ

                Call RePaint()
            End If



            ' ================================================
            ' エンド処理
            '''' 疵マップ変更 (画面中央位置を渡す)
            Dim nCenter As Integer = CInt(nIndex + (picFrameImg.ClientSize.Height / 2)) + 1  ' +1は、微妙に19.9999になる対策。
            Call mfrm_KizuMap.DspJumpRow(CInt(mcls_RpiView.GetPos_DpToLen_Y(nCenter)))

            '' 表示フレームレート
            tslFPS.Text = String.Format("{0:0.0}", (1000.0 / mcls_StopWatch.GetAveTime()))



            ' ==========================================
            ' 疵接近ランプ描画
            Dim nRanpGr() As Integer = mcls_CheckKizu.RanpStat()    ' 疵接近ランプのgidを取得
            For ii As Integer = 0 To mcnt_Ranp.Length - 1
                If 0 = nRanpGr(ii) Then
                    mcnt_Ranp(ii).BackColor = SystemColors.ControlDark
                Else
                    Dim gr As clsParamManager.PARAM_GRADE = mcls_Param.GetParamGrade(nRanpGr(ii))
                    mcnt_Ranp(ii).BackColor = gr.clrDspColor
                End If
            Next


            ''''' 連携用 として共有メモリに位置書込み
            Call SetMem_ValLen()
        End If
    End Sub

    ''' <summary>
    ''' 自動送り速度
    ''' </summary>
    Private Sub tscSpeed_TextChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles tscSpeed.TextChanged
        mfrm_FrmCont.LineSpeed = CInt(tscSpeed.Text)

        ' ライン速度0mpmの時には、一旦フォーカスをはずす。
        If 0 = CInt(tscSpeed.Text) Then
            btnRituNon.Select()
        End If
    End Sub

    ''' <summary>
    ''' 共有メモリ位置書込み
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetMem_ValLen()
        If Not m_memTrkPtr.Equals(IntPtr.Zero) Then
            Call SetMemTrk_ValLen()
        End If
        If Not m_memSyncPtr.Equals(IntPtr.Zero) Then
            Call SetMemSync_ValLen()
        End If
    End Sub



    ''' <summary>
    ''' 上下限チェック
    ''' </summary>
    ''' <param name="scr"></param>
    ''' <param name="val"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Shared Function LimitByScrollMinMax(ByVal scr As ScrollBar, ByVal val As Integer) As Integer
        If val > (scr.Maximum - scr.LargeChange + 1) Then Return scr.Maximum - scr.LargeChange + 1
        If val < scr.Minimum Then Return scr.Minimum
        Return val
    End Function


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 疵オーバーレイ

    ''' <summary>
    ''' 疵オーバーレイ情報セット
    ''' </summary>
    ''' <param name="nDspFrameNoS">開始開始フレームNo(1オリジン)</param>
    ''' <param name="nDspFrameNoE">対象終了フレームNo(1オリジン)</param>
    ''' <remarks></remarks>
    Private Sub SetKizuOvl(ByVal nDspFrameNoS As Integer, ByVal nDspFrameNoE As Integer)
        '''' 前回情報初期化
        mcls_RpiView.ResetOverLayInf()

        '''' 新しいオーバーレイ情報
        Dim oly As tClass.tClass_ImageDsp.OVERLAY_INF = Nothing
        oly.bIsDsp = m_bDrawKizuOvl
        oly.LineWidth = 1
        oly.HoseiX = 5
        oly.HoseiY = 5

        For Each data As DATA_DETECT_INF In m_lstDetect
            ' 今回の表示対象？
            If Not data.bDsp Then Continue For
            If nDspFrameNoS > data.nFrameNo Or nDspFrameNoE < data.nFrameNo Then Continue For
            Dim gr As clsParamManager.PARAM_GRADE = mcls_Param.GetParamGrade(data.gId)


            ' 枠
            oly.Tag = data
            oly.ovlPos1 = New Point(data.nFrameXmin + mcls_RpiView.GetRpi_THeader.caminfo(data.nCamPos).nBaseLeft, _
               data.nFrameYmin + (data.nFrameNo - nDspFrameNoS) * mcls_RpiView.GetRpi_Header.height)
            oly.ovlPos2 = New Point(data.nFrameXmax + mcls_RpiView.GetRpi_THeader.caminfo(data.nCamPos).nBaseLeft, _
             data.nFrameYmax + (data.nFrameNo - nDspFrameNoS) * mcls_RpiView.GetRpi_Header.height)
            oly.Color = gr.clrDspColor



            ' 疵名
            If mfrm_DspSet.g_bTypeDsp Then
                Dim ty As clsParamManager.PARAM_TYPE = mcls_Param.GetParamType(data.tId)
                oly.bIsDspType = True
                oly.cBsh = New SolidBrush(oly.Color)
                oly.cFnt = New System.Drawing.Font("ＭＳ ゴシック", mfrm_DspSet.g_nTypeFSize, System.Drawing.FontStyle.Regular, GraphicsUnit.Pixel)
                oly.sTypeName = ty.sSName
            End If


            ' クラスにデータセット
            mcls_RpiView.SetOverLayInf(oly)
        Next

    End Sub

    Private Sub tcls_tEv_OverLay_Click(ByVal sender As tClass_ImageDsp.OVERLAY_INF, ByVal e As System.Drawing.Rectangle) Handles mcls_RpiView.tEv_OverLay_Click
        Dim sWk As String
        '''' 長さ測定中であれば、オーバーレイクリックは無効化
        'If mfrm_DspLen.Visible Then Return
        '''' 虫眼鏡中であれば、無効
        If mcls_ImgCutDsp.IsDsp Then Return


        Dim sSession As String = "DB_" & mcls_RpiView.GetRpi_THeader.cSysName
        Dim nImgCnt As Integer = tMod.tMod_IniFile.ReadIniInt(sSession, "SELECT_IMG_CNT", TASKINI_NAME, 2)   '未指定の場合は、正乱２画像
        If 0 = nImgCnt Then Return


        '''' 切り出し画像ポップアップ表示準備
        Dim kiridata As DETECT_KIRI_DATA = Nothing
        kiridata.initialize()
        kiridata.data = CType(sender.Tag, DATA_DETECT_INF)


        '''' 画像データ取得
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, sSession)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        kiridata.nImgCnt = nImgCnt  '未指定の場合は、正乱２画像


        Try

            '' DB接続
            If Not tcls_DB.Open() Then
                MsgBox("DBと接続失敗。疵オーバーレイの表示はできません。")
                Return
            End If


            '' SQL生成
            sWk = "SELECT_IMG_" & mcls_RpiView.GetRpi_THeader.nMen.ToString
            Dim sSQL As String = tMod.tMod_IniFile.ReadIniStr(sSession, sWk, TASKINI_NAME)
            sWk = String.Format(sSQL, mcls_RpiView.GetRpi_THeader.cKizukenNo, kiridata.data.nNo)

            sqlRead = tcls_DB.SelectExecute(sWk)
            If sqlRead Is Nothing Then Return
            'sqlRead.Read()
            If Not sqlRead.Read() Then      ' Ver443
                kiridata.nImgCnt = 0
                Exit Try
            End If

            '' 画像表示
            kiridata.dResY = sqlRead.GetFloat(0)
            kiridata.dResX = sqlRead.GetFloat(1)
            kiridata.nImgSizeY = sqlRead.GetInt32(2)
            kiridata.nImgSizeX = sqlRead.GetInt32(3)
            kiridata.nImgCenterX = sqlRead.GetInt32(4)
            kiridata.nImgCenterY = sqlRead.GetInt32(5)

            Dim nIndex As Integer = 6
            For ii As Integer = 0 To kiridata.nImgCnt - 1
                kiridata.nImgSize(ii) = sqlRead.GetInt32(nIndex)
                nIndex += 1
            Next
            For ii As Integer = 0 To kiridata.nImgCnt - 1
                If 0 <> kiridata.nImgSize(ii) Then kiridata.imgAngle(ii) = tClass.tClass_BmpJpeg.DataToImage(CType(sqlRead.GetValue(nIndex), Byte()))
                nIndex += 1
            Next
        Catch ex As Exception
            'Return
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try



        '''' 切り出し画像表示画面生成
        Call mfrm_Kiri.Set_DbSession(sSession)
        Call mfrm_Kiri.SetProp_ImgKinf(Me.mcls_RpiView.GetImg_ImgKind)

        Call mfrm_Kiri.SetItem_DetectKiriData(kiridata)
        Call mfrm_Kiri.Show(Me)
        Call mfrm_Kiri.DspUpDate()
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 部分拡大
    ''' <summary>
    ''' 拡大表示エリアが描画された
    ''' </summary>
    ''' <param name="tCls_Calc">計算クラス</param>
    ''' <param name="img">画像配列(X方向の連結は未対応)</param>
    Private Sub mcls_ImgCutDsp_tEv_DispDataSet(ByRef tCls_Calc As tClassLibrary.tClass.tClass_ImageDsp_Calc, ByRef img() As System.Drawing.Image) Handles mcls_ImgCutDsp.tEv_DispDataSet
        Dim DspAriaInf As tClass_ViewRpiThread.DSP_ARIA_INF = mcls_RpiView.GetDspAriaInf

        '' データ生成＆セット
        tCls_Calc = mcls_RpiView.GetCls_ImgDsp()
        ReDim img(DspAriaInf.nDspFrameCnt - 1)
        For ii As Integer = 0 To img.Length - 1
            Dim imginf As tClass_ViewRpiThread.CLS_IMG_INF = mcls_RpiView.GetCls_Img_inf(ii + DspAriaInf.nDspFno + 1)

            If imginf Is Nothing Then Continue For
            img(ii) = imginf.pImgRaw
        Next
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 散布図
    ''' <summary>
    ''' 散布図
    ''' </summary>
    Private Sub tsbDefMap_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbDefMap.Click
        If Not mfrm_DefMap.Visible Then
            If Not m_bResutl Then Return

            Call mfrm_DefMap.Show(Me)
            Call mfrm_DefMap.MapDisp()
        Else
            mfrm_DefMap.Visible = False
        End If
        'Me.Select()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 疵マップ
    ''' <summary>
    ''' 疵マップ
    ''' </summary>
    Private Sub tsbKizuMap_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbKizuMap.Click
        If Not mfrm_KizuMap.Visible Then
            If Not m_bResutl Then Return

            Call mfrm_KizuMap.Show(Me)
            Call mfrm_KizuMap.MapDisp()
        Else
            mfrm_KizuMap.Visible = False
        End If
        'Me.Select()
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 疵一覧
    ''' <summary>
    ''' 疵一覧
    ''' </summary>
    Private Sub tsbKizuList_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbKizuList.Click
        If Not mfrm_KizuList.Visible Then
            If Not m_bResutl Then Return

            Call mfrm_KizuList.Show(Me)
            Call mfrm_KizuList.MapDisp()
        Else
            mfrm_KizuList.Visible = False
        End If
        'Me.Select()
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ グリッド線
    Private Sub tsbGridLine_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbGridLine.Click
        mfrm_GridLine.ShowToggle(Me)

        Me.Select()
        ' 全部終わったら再描画
        Call RePaint()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ジャンプ画面
    Private Sub tsbJump_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbJump.Click
        Call mfrm_FrmCont.ShowToggle(Me)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 任意切り出し

    Private Sub tsbKiriInput_Click(sender As System.Object, e As System.EventArgs) Handles tsbKiriInput.Click
        Call mfrm_KiriInput.ShowToggle(Me)
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ブックマーク
    Private Sub tsbBookmark_Click(sender As System.Object, e As System.EventArgs) Handles tsbBookmark.Click
        mfrm_Bookmark.ShowToggle(Me)
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 入力一覧
    Private Sub tsbInputList_Click(sender As System.Object, e As System.EventArgs) Handles tsbInputList.Click
        Call mfrm_InputList.ShowToggle(Me)
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ コイル情報一覧

    ''' <summary>
    ''' コイル情報一覧
    ''' </summary>
    Private Sub tsbCoilItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbCoilItem.Click
        If mfrm_CoilItem Is Nothing OrElse Not mfrm_CoilItem.Visible Then
            If Not m_bResutl Then Return

            mfrm_CoilItem = New frmPopCoilItem
            Call mfrm_CoilItem.SetCls_RpiView(mcls_RpiView)
            mfrm_CoilItem.Show(Me)
        Else
            mfrm_CoilItem.Close()
            mfrm_CoilItem = Nothing
        End If
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 表示設定

    ''' <summary>
    ''' 表示設定
    ''' </summary>
    Private Sub tsbDspSet_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbDspSet.Click
        Call mfrm_DspSet.ShowToggle(Me)
    End Sub

    ''' <summary>
    ''' オーバーレイ手動補正設定
    ''' </summary>
    Private Sub tsbReSetOvl_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbReSetOVL.Click
        Call mfrm_ReSetOvl.ShowToggle(Me)
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 各種設定画面
    ''' <summary>
    ''' 各種設定画面表示
    ''' </summary>
    Private Sub tsbSetting_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbSetting.Click
        Call mfrm_Setting.ShowToggle(Me)
    End Sub



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ コントロール設定
    Private Sub AutoDownUmu()
        mcls_CheckKizu.Enable = mfrm_FrmCont.AutoDownUmu
    End Sub






    ''' <summary>
    ''' ツールボタンのクリック。画面のマウス入力をとる連中
    ''' </summary>
    Private Sub tsb_MouseR_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbDspBig.Click
        'アクティブにするボタン
        Dim fobj As RpiPlugin.IMouseToolStrip = Nothing
        Dim bFoundHandler As Boolean = False

        For Each tsv As KeyValuePair(Of ToolStripButton, RpiPlugin.IMouseToolStrip) In mev_selectors
            If tsv.Key Is sender Then
                'だれ（どのプラグイン）のボタンが押されたか分かった。

                If Not tsv.Key.Checked Then
                    'すでにアクティブなボタンをまた押した
                    '→無視。（tsbBigのアクティブ化でもよいかも）
                    tsv.Key.Checked = True
                    Exit Sub
                End If

                If tsv.Value IsNot Nothing Then
                    'ボタンに対応するクラスがみつかった
                    If tsv.Value.Activate(True) Then
                        fobj = tsv.Value
                        bFoundHandler = True
                    End If

                    'アクティブ化が拒否された
                    Exit For
                Else
                    bFoundHandler = True
                End If
            End If
        Next

        If Not bFoundHandler Then
            Me.Select()
            Exit Sub
        End If

        'チェックボックスをラジオボタンらしくする。
        For Each tsv As KeyValuePair(Of ToolStripButton, RpiPlugin.IMouseToolStrip) In mev_selectors
            If tsv.Key IsNot sender Then
                tsv.Key.Checked = False
            End If
        Next

        'いままでActiveだったクラスに、失効通知
        If fobj IsNot m_active_mouse_handler Then
            If m_active_mouse_handler IsNot Nothing Then
                m_active_mouse_handler.Activate(False)
            End If
        End If

        mcls_ImgCutDsp.DspStop = (fobj IsNot Nothing)
        m_active_mouse_handler = fobj

        'フォームにキーフォーカスを持ってくる
        Me.Select()

        ' 全部終わったら再描画
        'Call RePaint()
    End Sub





    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ コイル検索


    ''' <summary>
    ''' コイル検索画面表示
    ''' </summary>
    Private Sub tsbSearch_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbSearch.Click
        Call mfrm_Search.Show(Nothing)
        mfrm_Search.Activate()
    End Sub
    ''' <summary>
    ''' コイルリスト上のコイルに切替
    ''' </summary>
    Private Sub tsbSearchUp_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbSearchUp.Click
        Call mfrm_Search.SelectChangeCoil(-1)   ' 上は手前なので、－ 注意
    End Sub
    Private Sub tsbSearchDown_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbSearchDown.Click
        Call mfrm_Search.SelectChangeCoil(1)
    End Sub
    ''' <summary>
    ''' 表裏反転
    ''' </summary>
    Private Sub tsbSearchOU_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbSearchOU.Click
        'If Not mcls_RpiView.IsRpi Then Return

        Call mfrm_Search.SelectChangeMen()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 出力設定


    ''' <summary>
    ''' 出力
    ''' </summary>
    Private Sub tsbPrint_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbPrint.Click
        Call mfrm_Print.ShowToggle(Me)
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 連携用

    ''' <summary>
    ''' 共有メモリに書込み
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetMemTrk_ValLen()
        '''' 自Exeがアクティブの場合のみ、結果書込み
        ' ヘッダー部
        If Not Nothing Is Form.ActiveForm Then
            Dim tMemHead As New TYPE_TRK_HEAD
            ' セット
            tMemHead.nActiveID = g_typStartArgs.nId

            ' 書込
            tClass_GlovalMemory.SetMappedT(m_memTrkPtr, 0, tMemHead)

            Dim tMemVal As New TYPE_TRK_ITEM                        ' 自工程の情報構造体
            Dim nMemOffset As Integer = SIZE_TYPE_HEAD + SIZE_TYPE_ITEM * g_typStartArgs.nId        ' 自構造体位置

            ' セット
            tMemVal.nY = CInt(mcls_RpiView.GetPos_DpToLen_Y(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y))
            tMemVal.nX = CInt(mcls_RpiView.GetPos_DpToLen_X(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X))
            ' 書込
            tClass_GlovalMemory.SetMappedT(m_memTrkPtr, nMemOffset, tMemVal)
        End If
    End Sub


    ''' <summary>
    ''' 簡易連携 共有メモリ接続
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub tsbSync_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbSync.Click
        '''' 外部連携時は、不可
        If g_bOutTrk Then
            tsbSync.Checked = False
            Return
        End If

        '''' 共有メモリ操作
        If tsbSync.Checked Then
            '''' 共有メモリ生成 (連携の中で１つでも出来れば問題なし)
            m_memSyncPtr = tClass_GlovalMemory.AllocMapped(SIZE_TYPE_SYNC, TBL_RPI_SYNC_NAME)   ' Lock or Open
            If IntPtr.Zero = m_memSyncPtr Then
                MsgBox("共有メモリ生成失敗")
                tsbSync.Checked = False
                Return
            End If


            Dim tMemVal As TYPE_SYNC                        ' 自工程の情報構造体
            tClass_GlovalMemory.GetMappedT(m_memSyncPtr, 0, tMemVal)



            ' 誤差算出
            m_nSyncOffsetY = CInt(mcls_RpiView.GetPos_DpToLen_Y(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y)) - tMemVal.nY
            m_nSyncOffsetX = CInt(mcls_RpiView.GetPos_DpToLen_X(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X)) - tMemVal.nX

        Else
            If IntPtr.Zero <> m_memSyncPtr Then
                tClass_GlovalMemory.FreeMapped(m_memSyncPtr, TBL_RPI_SYNC_NAME)
                m_memSyncPtr = IntPtr.Zero
                m_nSyncOffsetY = 0
                m_nSyncOffsetX = 0
            End If
        End If
    End Sub
    ''' <summary>
    ''' 共有メモリに書込み
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetMemSync_ValLen()
        ' 自Exeがアクティブの場合のみ、結果書込み
        If Not Nothing Is Form.ActiveForm Then
            Dim tMemVal As New TYPE_SYNC                        ' 自工程の情報構造体
            tMemVal.nY = CInt(mcls_RpiView.GetPos_DpToLen_Y(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y)) - m_nSyncOffsetY  ' オフセット分は無しにした値
            tMemVal.nX = CInt(mcls_RpiView.GetPos_DpToLen_X(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X)) - m_nSyncOffsetX

            ' 書込
            tClass_GlovalMemory.SetMappedT(m_memSyncPtr, 0, tMemVal)
        End If
    End Sub
End Class

