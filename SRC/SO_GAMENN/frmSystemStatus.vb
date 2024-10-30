'*********************************************************************************
' 検査開始停止設定画面
'	[Ver]
'		Ver.01    2012/07/13  初版
'
'	[メモ]
'
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports KizuLibCLI

Public Class frmSystemStatus

    ''' <summary>
    ''' コンボボックスに表示するデータ
    ''' </summary>
    ''' <remarks>検査表面状態・検査パターン 用</remarks>
    Public Class cls_CmbItem
        ''' <summary>主キー</summary>
        Public id As String                                 ' 主キー
        ''' <summary>紐付く名称</summary>
        Public name As String                               ' 紐付く名称
        ''' <summary>メールスロット送信データ</summary>
        Public sendid As Integer                           ' メールスロット送信データ

        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="strID">主キー</param>
        ''' <param name="strName">紐付く名称</param>
        ''' <param name="nSendID">メールスロット送信データ</param>		
        ''' <remarks></remarks>
        Public Sub New(ByVal strID As String, ByVal strName As String, ByVal nSendID As Integer)
            id = strID
            name = strName
            sendid = nSendID
        End Sub

        ''' <summary>
        ''' 構造体の表示名称
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overrides Function ToString() As String
            Return id
        End Function
    End Class

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const CHECK_TIME As Integer = 1000              '監視タイマー監視間隔(msec)
    Private Const INIT_TIMEOUT_TIME As Integer = 10000      '画面起動時タイムアウト時間(msec)
    Private Const TIMEOUT_TIME As Integer = 150000          '応答待ちタイムアウト
    Private Const STATUS_WAIT As Integer = -1               '応答待ちステータス
    Private Const STATUS_TIMEOUT As Integer = -2            '応答タイムアウトステータス
    Private Const COILINF_WAIT As Integer = -3              'コイル情報応答待ちスータス
    Private Const COILINF_TIMEOUT As Integer = -4           'コイル情報応答待ちタイムアウト
    Private COnst STATUS_FIRST_TIMEOUT As Integer = -5      '初回応答待ちタイムアウト
    Private Const PLG_NON As Integer = 0                    'PLG無し
    Private Const MAX_CAMSET_NO As Integer = 20             'カメラセットNo最大値

    '片面検査条件
    Private Enum EM_KENSA_UMU
        nKataMenNo = 0  '片面検査不許可
        nKataMenYes     '片面検査許可
    End Enum

    'メール応答フラグ
    Public Enum EM_ANSER_FLG
        nWaite = 0      '応答待ち
        nAnser          '応答完了
        nUpdata         '更新要求
        nCoilWaite      'コイル情報応答待ち
        nFirstWaite     '初回応答待ち
    End Enum

    'コンボボックス種別
    Private Enum EM_COMBO_MODE
        nScodeTop = 0   '検査表面状態 表
        nPcodeTop = 1   '検査パターン 表
        nScodeBot = 2   '検査表面状態 裏
        nPcodeBot = 3   '検査パターン 裏
    End Enum

    'コイル情報反映先
    Private Enum EM_TARGET_COIL
        nNow = 0            '現コイル
        nNext               '次コイル
    End Enum

    ''' <summary>
    ''' ロジックパララン検査状態
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_LP_KENSA
        nKensaStop = 0              '停止
        nKensaWaitStop = 1          '停止中
        nKensaWaitStart = 2         '開始中
        nKensaStart = 3             '検査中
        nNoAnser = 98               '応答無し
        nGetData = 99               '状態取得中
    End Enum

    ''' <summary>
    ''' 検査種別
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_KENSA_KIND
        nPC1 = 0                'PC01
        nPC2 = 1                'PC02
        nNormal                 '通常検査
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_nUntenStatus As EM_DIV_UNTEN      '運転モード(1:通常,2:テスト,3:校正,4:シミュレーション)
    Private m_nKadouStatus As EM_DIV_KADOU      '稼動状態(1:初期化中,2:停止中,3:稼動中又はテスト中,4:ﾘﾄﾗｲ中)
    Private m_nPlgStatus As EM_DIV_PLG          'PLG区分(1:ライン,2:表校正,3:裏校正,4:テスト)
    Private m_nKatamenStatus As Integer = -1    '片面検査許可(0:検査不許可,1:検査許可)
    Private m_nAnser As EM_ANSER_FLG            'メール応答フラグ(0:応答待ち,1:応答完了,2:更新要求)
    Private m_bLock As Boolean                  '画面更新中フラグ(True:画面更新中,False:画面更新中でない)
    Private m_bCoilReqFlg As Boolean = False    'コイル要求フラグ
    Private m_emLPStatusPc(NUM_HANTEI_PARA - 1) As EM_LP_KENSA  ' ロジックパララン情報

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 外部プロパティ
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' 運転モード取得
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("☆検査開始停止設定画面プロパティ") _
     , System.ComponentModel.Description("運転モード取得")>
    Public Property tProp_UntenStatus() As Integer
        Get
            Return m_nUntenStatus
        End Get
        Set(ByVal Value As Integer)
            m_nUntenStatus = CType(Value, EM_DIV_UNTEN)
        End Set
    End Property


    ''' <summary>
    ''' 稼動状態取得
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("☆検査開始停止設定画面プロパティ") _
     , System.ComponentModel.Description("稼動状態モード取得")>
    Public Property tProp_KadouStatus() As Integer
        Get
            Return m_nKadouStatus
        End Get
        Set(ByVal Value As Integer)
            m_nKadouStatus = CType(Value, EM_DIV_KADOU)
        End Set
    End Property

    ''' <summary>
    ''' PLG区分取得
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("☆検査開始停止設定画面プロパティ") _
     , System.ComponentModel.Description("PLG区分取得")>
    Public Property tProp_PlgStatus() As Integer
        Get
            Return m_nPlgStatus
        End Get
        Set(ByVal Value As Integer)
            m_nPlgStatus = CType(Value, EM_DIV_PLG)
        End Set
    End Property

    ''' <summary>
    ''' 片面検査条件
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("☆検査開始停止設定画面プロパティ") _
     , System.ComponentModel.Description("片面検査条件")>
    Public Property tProp_KatamenStatus() As Integer
        Get
            Return m_nKatamenStatus
        End Get
        Set(ByVal Value As Integer)
            m_nKatamenStatus = Value
        End Set
    End Property

    ''' <summary>
    ''' 応答フラグ取得
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("☆検査開始停止設定画面プロパティ") _
     , System.ComponentModel.Description("応答フラグ取得")>
    Public Property tProp_Anser() As EM_ANSER_FLG
        Get
            Return m_nAnser
        End Get
        Set(ByVal Value As EM_ANSER_FLG)
            m_nAnser = Value
        End Set
    End Property

    ''' <summary>
    ''' ロジックパラランPC検査状態
    ''' </summary>
    ''' <param name="nPcNo">取得対象のPC番号</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("☆検査開始停止設定画面プロパティ") _
     , System.ComponentModel.Description("ロジックパラランPC01検査状態")>
    Public Property tProp_LPStatusPc(ByVal nPcNo As Integer) As Integer
        Get
            If NUM_HANTEI_PARA < nPcNo Or 0 > nPcNo Then
                Return -1
            End If

            Return m_emLPStatusPc(nPcNo)
        End Get
        Set(ByVal Value As Integer)
            If NUM_HANTEI_PARA < nPcNo Or 0 > nPcNo Then
                Return
            End If

            If False = [Enum].IsDefined(GetType(EM_LP_KENSA), Value) Then   '定義しているロジックパラランの状態以外
                Return
            End If

            m_emLPStatusPc(nPcNo) = CType(Value, EM_LP_KENSA)
        End Set
    End Property

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmSystemStatus_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus] フォームロード開始"))

        Dim sw As New Stopwatch

        sw.Start()

        ' 閉じるボタンイベント
        MyBase.AddFunctionButton(cmdEnd, 11)

        ' タイトル設定
        Call SetTitleName(Me)

        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]タイトル設定 [経過時間][{0}]", sw.Elapsed))

        '初期設定
        Call InitDsp()
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]初期設定 [経過時間][{0}]", sw.Elapsed))

        '検査表面状態Noのコンボボックス作成
        Call SetScode()
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]検査表面状態Noのコンボボックス作成 [経過時間][{0}]", sw.Elapsed))

        '検査パターンコードのコンボボックス作成
        Call SetPcode()
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]検査パターンコードのコンボボックス作成 [経過時間][{0}]", sw.Elapsed))

        'ステータス初期化
        Call IntiStatus()
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]ステータス初期化 [経過時間][{0}]", sw.Elapsed))

        '応答待ちタイマーセット
        Call SetTimer(INIT_TIMEOUT_TIME, EM_ANSER_FLG.nFirstWaite)
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]応答待ちタイマーセット [経過時間][{0}]", sw.Elapsed))

        'カメラセットのコンボボックス作成
        Call SetCamSet()
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]カメラセットのコンボボックス作成 [経過時間][{0}]", sw.Elapsed))

        '統括へ稼動状態取得要求
        Dim que As KizuLibCLI.COMMON_QUE_STRMIX = Nothing

        '送信データ作成
        que.nEventNo = FACT_TO_DEFECT_04
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()
        'que.cdata1 = System.Environment.MachineName
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]送信データ作成 [経過時間][{0}]", sw.Elapsed))


        '稼動状態取得要求送信
        Call SendMail(que, TO_DEFECT, "稼動状態取得要求")
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]稼動状態取得要求送信 [経過時間][{0}]", sw.Elapsed))

        '状態取得中
        Call SetKadouStatus(STATUS_WAIT)
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]状態取得 [経過時間][{0}]", sw.Elapsed))

        'ロジックパラランPC検査状態再セット
        For ii As Integer = 0 To NUM_HANTEI_PARA - 1
            Call SetLpStatus(ii, m_emLPStatusPc)
        Next

        'エラーメッセージクリア
        lblErrMsg.Text = ""

        sw.Stop()

        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus] フォームロード終了"))

    End Sub

    ''' <summary>
    ''' ステータス初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub IntiStatus()

        m_nUntenStatus = 0  '運転モード
        m_nKadouStatus = 0  '稼動状態
        m_nPlgStatus = 0    'PLG
        m_nKatamenStatus = -1   '片面許可/不許可

    End Sub

    ''' <summary>
    ''' 終了
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub cmdEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmdEnd.Click
        Me.Enabled = False
        Me.Hide()
    End Sub

    ''' <summary>
    ''' 初期設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitDsp()

        '運転モードラジオボタン初期設定
        rdoNormal.Tag = EM_DIV_UNTEN.DIV_UNTEN_NOMAL
        rdoSamp.Tag = EM_DIV_UNTEN.DIV_UNTEN_SAMP
        rdoCamera.Tag = EM_DIV_UNTEN.DIV_UNTEN_CAMERA
        rdoSmyu.Tag = EM_DIV_UNTEN.DIV_UNTEN_SMYU


        'イベント追加
        AddHandler rdoNormal.CheckedChanged, AddressOf rdoUnten_CheckedChanged
        AddHandler rdoSamp.CheckedChanged, AddressOf rdoUnten_CheckedChanged
        AddHandler rdoCamera.CheckedChanged, AddressOf rdoUnten_CheckedChanged
        AddHandler rdoSmyu.CheckedChanged, AddressOf rdoUnten_CheckedChanged


        'PLGのラジオボタン初期設定
        rdoPlgLine.Tag = EM_DIV_PLG.DIV_PLG_LINE
        rdoPlgTop.Tag = EM_DIV_PLG.DIV_PLG_TOP
        rdoPlgBot.Tag = EM_DIV_PLG.DIV_PLG_BOT
        rdoPlgTest.Tag = EM_DIV_PLG.DIV_PLG_TEST

        'イベント追加
        AddHandler rdoPlgLine.CheckedChanged, AddressOf rdoPlg_CheckedChanged
        AddHandler rdoPlgTop.CheckedChanged, AddressOf rdoPlg_CheckedChanged
        AddHandler rdoPlgBot.CheckedChanged, AddressOf rdoPlg_CheckedChanged
        AddHandler rdoPlgTest.CheckedChanged, AddressOf rdoPlg_CheckedChanged

        '片面検査ラジオボタン初期設定
        rdoKatamenNo.Tag = EM_KENSA_UMU.nKataMenNo
        rdoKatamenYes.Tag = EM_KENSA_UMU.nKataMenYes

        'イベント追加
        AddHandler rdoKatamenNo.CheckedChanged, AddressOf rdoKatamen__CheckedChanged
        AddHandler rdoKatamenYes.CheckedChanged, AddressOf rdoKatamen__CheckedChanged

        'コンボボックス初期設定
        cmbSCodeTop.Tag = EM_COMBO_MODE.nScodeTop   '検査表面状態 表
        cmbPCodeTop.Tag = EM_COMBO_MODE.nPcodeTop   '検査パターン 表
        cmbSCodeBot.Tag = EM_COMBO_MODE.nScodeBot   '検査表面状態 裏
        cmbPCodeBot.Tag = EM_COMBO_MODE.nPcodeBot   '検査パターン 裏

        'イベント追加
        AddHandler cmbSCodeTop.SelectedIndexChanged, AddressOf cmbData_SelectedIndexChanged
        AddHandler cmbPCodeTop.SelectedIndexChanged, AddressOf cmbData_SelectedIndexChanged
        AddHandler cmbSCodeBot.SelectedIndexChanged, AddressOf cmbData_SelectedIndexChanged
        AddHandler cmbPCodeBot.SelectedIndexChanged, AddressOf cmbData_SelectedIndexChanged

        '監視タイマーセット
        timCheck.Interval = CHECK_TIME
        timCheck.Enabled = True

        AddHandler modMain.tEV_MailSlotRecv_FACT_SO_GAMENN_01, AddressOf ReceiveFACT_SO_GAMENN_01

    End Sub


    ''' <summary>
    ''' 検査開始ボタン
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub btnKensaStart_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnKensaStart.Click

        SendKensaStart(EM_KENSA_KIND.nNormal, FACT_TO_DEFECT_02)

    End Sub


    ''' <summary>
    ''' 確認メッセージ表示
    ''' </summary>
    ''' <param name="strMsg"></param>
    ''' <remarks></remarks>
    Public Overridable Sub DspMsg(ByVal strMsg As String)

        Dim frmMsg As tClassForm.frmPop_MsgBox

        frmMsg = New tClassForm.frmPop_MsgBox(strMsg, "確認", , True, False, System.Windows.Forms.DialogResult.OK)

        'メッセージ表示
        frmMsg.ShowDialog()
        frmMsg.Dispose()
        frmMsg = Nothing

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
    ''' 検査終了ボタン
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub btnKensaEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnKensaEnd.Click

        SendKensaStop(EM_KENSA_KIND.nNormal, FACT_TO_DEFECT_03)

    End Sub

    ''' <summary>
    ''' システム終了ボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSystemEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSystemEnd.Click

        Dim que As KizuLibCLI.COMMON_QUE_STRMIX = Nothing
        Dim bRet As DialogResult


        'システム終了確認
        Call DspMsg("システムを終了しますか？", bRet)
        If System.Windows.Forms.DialogResult.OK <> bRet Then
            Exit Sub
        End If

        'パスワード入力チェック
        'パスワード確認
        Dim frmPass As New tClassForm.frmPop_Pass(PASSWORD)
        bRet = frmPass.ShowDialog
        If System.Windows.Forms.DialogResult.OK <> bRet Then
            Exit Sub
        End If

        '送信データ設定
        que.nEventNo = FACT_KS_MASTER_06
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()

        'システム終了メール送信
        Call SendMail(que, KS_MASTER, "システム終了要求")

    End Sub

    ''' <summary>
    ''' 受信待ちタイマーセット
    ''' </summary>
    ''' <param name="nVal">インターバル</param>
    ''' <param name="nWaite">応答待ち定数</param>
    ''' <remarks></remarks>
    Private Sub SetTimer(ByVal nVal As Integer, ByVal nWaite As EM_ANSER_FLG)

        'タイマーセット
        timTimeOut.Interval = nVal
        timTimeOut.Enabled = True

        'プログレスバー設定
        prbTimeOut.Maximum = CInt(nVal / CHECK_TIME)
        prbTimeOut.Value = 0

        '応答待ち
        m_nAnser = nWaite

        '一旦タイマーをSTOP
        timCheck.Stop()
        timCheck.Start()

    End Sub


    ''' <summary>
    ''' 運転モード押下チェック
    ''' </summary>
    ''' <remarks></remarks>
    Private Function CheckUnten() As Boolean

        '通常運転
        If rdoNormal.Checked Then
            Return True
        End If

        'テスト用
        If rdoSamp.Checked Then
            Return True
        End If

        '校正
        If rdoCamera.Checked Then
            Return True
        End If

        'シミュレーション
        If rdoSmyu.Checked Then
            Return True
        End If

        Return False
    End Function
    ''' <summary>
    ''' PLG区分押下チェック
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckPlg() As Boolean

        'ライン
        If rdoPlgLine.Checked Then
            Return True
        End If

        'PLGTOP
        If rdoPlgTop.Checked Then
            Return True
        End If

        'PLGBOT
        If rdoPlgBot.Checked Then
            Return True
        End If

        'テスト
        If rdoPlgTest.Checked Then
            Return True
        End If

        Return False
    End Function
    ''' <summary>
    ''' 片面検査条件押下チェック
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckKatamen() As Boolean

        '片面検査不許可
        If rdoKatamenNo.Checked Then
            Return True
        End If

        '片面検査許可
        If rdoKatamenYes.Checked Then
            Return True
        End If
        Return False
    End Function

    ''' <summary>
    ''' メール送信処理
    ''' </summary>
    ''' <param name="que">送信データ</param>
    ''' <param name="strTaskName">タスク名</param>
    ''' <param name="strEventName">起動要因名称</param>
    ''' <remarks></remarks>
    Private Sub SendMail(ByVal que As KizuLibCLI.COMMON_QUE_STRMIX, ByVal strTaskName As String, ByVal strEventName As String)

        Dim nRet As Integer

        '検査開始要求送信
        If Not LOCAL Then
            '本番
            nRet = KizuLibCLI.KizuMod.Send_Mail(strTaskName, KizuLibCLI.KizuMod.Tokatu, que)
        Else
            'テスト
            nRet = KizuLibCLI.KizuMod.Send_Mail(strTaskName, ".", que)
        End If

        If 0 <> nRet Then
            If NOA_OFFLINE <> g_NOA Then
                KizuLibCLI.KizuMod.SysLog(SYSNO_MAIL_SNED_ERR, String.Format("[{0}]", strEventName & " err_code = " & nRet))
            End If
        End If

    End Sub


    ''' <summary>
    ''' 監視処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub timCheck_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timCheck.Tick

        '応答がない場合は、終了
        If EM_ANSER_FLG.nFirstWaite = m_nAnser Or EM_ANSER_FLG.nWaite = m_nAnser Or EM_ANSER_FLG.nCoilWaite = m_nAnser Then
            prbTimeOut.Value += 1
            Exit Sub
        End If

        '更新フラグON(定周期で表示する場合は、稼動停止中でもチェックイベントを有効にするため)
        m_bLock = True

        '稼動状況セット
        Call SetKadouStatus(CInt(m_nKadouStatus))

        '運転モードセット
        Call SetUntenStatus(m_nUntenStatus)

        'PLG区分セット
        Call SetPlgStatus(m_nPlgStatus)

        '片面検査状態セット
        Call SetKatamenStatus(m_nKatamenStatus)

        'ロジックパララン状態セット
        For ii As Integer = 0 To NUM_HANTEI_PARA - 1
            Call SetLpStatus(ii, m_emLPStatusPc)
        Next

        timTimeOut.Enabled = False
        prbTimeOut.Value = 0

        '更新フラグOFF
        m_bLock = False

    End Sub

    ''' <summary>
    ''' 稼動状態取得タイムアウト処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub timTimeOut_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timTimeOut.Tick

        '応答タイムアウト
        If EM_ANSER_FLG.nFirstWaite = m_nAnser Then
            Call SetKadouStatus(STATUS_FIRST_TIMEOUT)
        ElseIf EM_ANSER_FLG.nWaite = m_nAnser Then
            Call SetKadouStatus(STATUS_TIMEOUT)
        ElseIf EM_ANSER_FLG.nCoilWaite = m_nAnser Then
            Call SetKadouStatus(COILINF_TIMEOUT)
        End If

        prbTimeOut.Value = prbTimeOut.Maximum

        timTimeOut.Enabled = False

        m_nAnser = EM_ANSER_FLG.nAnser
    End Sub

    ''' <summary>
    ''' 稼動状況セット
    ''' </summary>
    ''' <param name="nVal">ステータス</param>
    ''' <remarks></remarks>
    Private Sub SetKadouStatus(ByVal nVal As Integer)

        Select Case nVal

            Case STATUS_WAIT                '状態取得中
                lblSystemStatus.Text = "状態取得中"
                lblSystemStatus.BackColor = Color.Lime
                lblSystemStatus.ForeColor = Color.Black
                btnKensaStart.Enabled = False
                btnKensaEnd.Enabled = False
                btnSystemEnd.Enabled = False
                cmbSCodeTop.Enabled = False
                lblSCodeTop.Enabled = False
                cmbPCodeTop.Enabled = False
                lblPCodeTop.Enabled = False
                cmbSCodeBot.Enabled = False
                lblSCodeBot.Enabled = False
                cmbPCodeBot.Enabled = False
                lblPCodeBot.Enabled = False
                tProp_LPStatusPc(0) = EM_LP_KENSA.nGetData
                tProp_LPStatusPc(1) = EM_LP_KENSA.nGetData

            Case STATUS_FIRST_TIMEOUT       '初回タイムアウト
                lblSystemStatus.Text = "応答無し"
                lblSystemStatus.BackColor = Color.Magenta
                lblSystemStatus.ForeColor = Color.Yellow
                btnKensaStart.Enabled = False
                btnKensaEnd.Enabled = True
                btnSystemEnd.Enabled = False
                cmbSCodeTop.Enabled = False
                lblSCodeTop.Enabled = False
                cmbPCodeTop.Enabled = False
                lblPCodeTop.Enabled = False
                cmbSCodeBot.Enabled = False
                lblSCodeBot.Enabled = False
                cmbPCodeBot.Enabled = False
                lblPCodeBot.Enabled = False
                tProp_LPStatusPc(0) = EM_LP_KENSA.nNoAnser
                tProp_LPStatusPc(1) = EM_LP_KENSA.nNoAnser

            Case STATUS_TIMEOUT                 'タイムアウト
                lblSystemStatus.Text = "応答無し"
                lblSystemStatus.BackColor = Color.Magenta
                lblSystemStatus.ForeColor = Color.Yellow
                btnKensaStart.Enabled = False
                btnKensaEnd.Enabled = True
                btnSystemEnd.Enabled = False
                cmbSCodeTop.Enabled = False
                lblSCodeTop.Enabled = False
                cmbPCodeTop.Enabled = False
                lblPCodeTop.Enabled = False
                cmbSCodeBot.Enabled = False
                lblSCodeBot.Enabled = False
                cmbPCodeBot.Enabled = False
                lblPCodeBot.Enabled = False

            Case EM_DIV_KADOU.DIV_KADOU_INIT    '初期化
                lblSystemStatus.Text = "初期化中"
                lblSystemStatus.BackColor = g_ColorKadouInit
                lblSystemStatus.ForeColor = Color.Yellow
                btnKensaStart.Enabled = False
                btnKensaEnd.Enabled = False
                btnSystemEnd.Enabled = False
                cmbSCodeTop.Enabled = False
                lblSCodeTop.Enabled = False
                cmbPCodeTop.Enabled = False
                lblPCodeTop.Enabled = False
                cmbSCodeBot.Enabled = False
                lblSCodeBot.Enabled = False
                cmbPCodeBot.Enabled = False
                lblPCodeBot.Enabled = False

            Case EM_DIV_KADOU.DIV_KADOU_STOP    '停止
                lblSystemStatus.Text = "停止"
                lblSystemStatus.BackColor = g_ColorKadouStop
                lblSystemStatus.ForeColor = Color.Yellow

                btnKensaEnd.Enabled = False
                btnSystemEnd.Enabled = True

                btnKensaStart.Enabled = True
                cmbSCodeTop.Enabled = True
                lblSCodeTop.Enabled = True
                cmbPCodeTop.Enabled = True
                lblPCodeTop.Enabled = True
                cmbSCodeBot.Enabled = True
                lblSCodeBot.Enabled = True
                cmbPCodeBot.Enabled = True
                lblPCodeBot.Enabled = True

            Case EM_DIV_KADOU.DIV_KADOU_START   '稼動中又はテスト中

                If m_nUntenStatus = EM_DIV_UNTEN.DIV_UNTEN_NOMAL Then
                    lblSystemStatus.Text = "稼動中"
                Else
                    lblSystemStatus.Text = "テスト中"
                End If

                lblSystemStatus.BackColor = g_ColorKadouStart
                lblSystemStatus.ForeColor = Color.Yellow
                btnKensaStart.Enabled = False
                btnKensaEnd.Enabled = True
                btnSystemEnd.Enabled = False
                cmbSCodeTop.Enabled = False
                lblSCodeTop.Enabled = False
                cmbPCodeTop.Enabled = False
                lblPCodeTop.Enabled = False
                cmbSCodeBot.Enabled = False
                lblSCodeBot.Enabled = False
                cmbPCodeBot.Enabled = False
                lblPCodeBot.Enabled = False

            Case EM_DIV_KADOU.DIV_KADOU_RETRY   'ﾘﾄﾗｲ中
                lblSystemStatus.Text = "リトライ中"
                lblSystemStatus.BackColor = g_ColorKadouRetry
                lblSystemStatus.ForeColor = Color.Yellow
                btnKensaStart.Enabled = False
                btnKensaEnd.Enabled = True
                btnSystemEnd.Enabled = False
                cmbSCodeTop.Enabled = False
                lblSCodeTop.Enabled = False
                cmbPCodeTop.Enabled = False
                lblPCodeTop.Enabled = False
                cmbSCodeBot.Enabled = False
                lblSCodeBot.Enabled = False
                cmbPCodeBot.Enabled = False
                lblPCodeBot.Enabled = False

        End Select

    End Sub

    ''' <summary>
    ''' 運転モードセット
    ''' </summary>
    ''' <param name="nVal">ステータス</param>
    ''' <remarks></remarks>
    Private Sub SetUntenStatus(ByVal nVal As Integer)

        Select Case nVal
            Case EM_DIV_UNTEN.DIV_UNTEN_NOMAL   '通常
                rdoNormal.Checked = True
            Case EM_DIV_UNTEN.DIV_UNTEN_SAMP    'テスト
                rdoSamp.Checked = True
            Case EM_DIV_UNTEN.DIV_UNTEN_CAMERA  '校正
                rdoCamera.Checked = True
            Case EM_DIV_UNTEN.DIV_UNTEN_SMYU    'シミュレーション
                rdoSmyu.Checked = True
            Case Else

                '全ての項目のチェックを外す
                rdoNormal.Checked = False
                rdoSamp.Checked = False
                rdoCamera.Checked = False
                rdoSmyu.Checked = False
        End Select

    End Sub
    ''' <summary>
    ''' PLG区分セット
    ''' </summary>
    ''' <param name="nVal">ステータス</param>
    ''' <remarks></remarks>
    Private Sub SetPlgStatus(ByVal nVal As Integer)

        Select Case nVal
            Case EM_DIV_PLG.DIV_PLG_LINE    'ライン
                rdoPlgLine.Checked = True
            Case EM_DIV_PLG.DIV_PLG_TOP     '表校正
                rdoPlgTop.Checked = True
            Case EM_DIV_PLG.DIV_PLG_BOT     '裏校正
                rdoPlgBot.Checked = True
            Case EM_DIV_PLG.DIV_PLG_TEST    'テスト
                rdoPlgTest.Checked = True
            Case Else

                '全ての項目のチェックを外す
                rdoPlgLine.Checked = False
                rdoPlgTop.Checked = False
                rdoPlgBot.Checked = False
                rdoPlgTest.Checked = False
        End Select

    End Sub

    ''' <summary>
    ''' 片面検査状態をセット
    ''' </summary>
    ''' <param name="nVal"></param>
    ''' <remarks></remarks>
    Private Sub SetKatamenStatus(ByVal nVal As Integer)

        Select Case nVal
            Case EM_KENSA_UMU.nKataMenNo    '片面検査不許可
                rdoKatamenNo.Checked = True
            Case EM_KENSA_UMU.nKataMenYes   '片面検査許可
                rdoKatamenYes.Checked = True
            Case Else

                '全ての項目のチェックを外す
                rdoKatamenNo.Checked = False
                rdoKatamenYes.Checked = False
        End Select

    End Sub

    ''' <summary>
    ''' ロジックパララン状態セット
    ''' </summary>
    ''' <param name="nKind">対象PC(0:PC1,0以外:PC2)</param>
    ''' <param name="nVal">ロジックパララン状態</param>
    ''' <remarks></remarks>
    Private Sub SetLpStatus(ByVal nKind As Integer, ByVal nVal() As EM_LP_KENSA)

        Dim lblLPStart As Label
        Dim btnLpStart As Button
        Dim btnLpEnd As Button
        Dim cmbCamera1 As ComboBox
        Dim cmbCamera2 As ComboBox
        Dim nMyPcIdx As Integer
        Dim nOtherPcIdx As Integer

        '　対象PC判定
        If 0 = nKind Then
            lblLPStart = CType(lblLPStatusPC1, Label)
            btnLpStart = CType(btnLPStartPC1, Button)
            btnLpEnd = CType(btnLPEndPC1, Button)
            cmbCamera1 = CType(cmbCamera1PC1, ComboBox)
            cmbCamera2 = CType(cmbCamera2PC1, ComboBox)

            nMyPcIdx = 0
            nOtherPcIdx = 1
        Else
            lblLPStart = CType(lblLPStatusPC2, Label)
            btnLpStart = CType(btnLPStartPC2, Button)
            btnLpEnd = CType(btnLPEndPC2, Button)
            cmbCamera1 = CType(cmbCamera1PC2, ComboBox)
            cmbCamera2 = CType(cmbCamera2PC2, ComboBox)

            nMyPcIdx = 1
            nOtherPcIdx = 0
        End If

        ' コンボボックス設定
        If EM_DIV_KADOU.DIV_KADOU_START = m_nKadouStatus _
           AndAlso (EM_LP_KENSA.nKensaWaitStart = nVal(nOtherPcIdx) _
                    OrElse EM_LP_KENSA.nKensaWaitStop = nVal(nOtherPcIdx)) Then         ' 稼動状態：稼動中 and ロジックパララン開始中、停止中（別PC）
            cmbCamera1.Enabled = False
            cmbCamera2.Enabled = False
        ElseIf (EM_DIV_KADOU.DIV_KADOU_STOP = m_nKadouStatus _
            OrElse EM_DIV_KADOU.DIV_KADOU_START = m_nKadouStatus) _
            AndAlso EM_LP_KENSA.nKensaStop = nVal(nMyPcIdx) Then                        ' 稼動状態：稼動中or停止中 and ロジックパララン停止 
            cmbCamera1.Enabled = True
            cmbCamera2.Enabled = True
        Else
            cmbCamera1.Enabled = False
            cmbCamera2.Enabled = False
        End If

        ' 開始、停止ボタン設定
        If EM_DIV_KADOU.DIV_KADOU_START = m_nKadouStatus _
           AndAlso (EM_LP_KENSA.nKensaWaitStart = nVal(nOtherPcIdx) _
                    OrElse EM_LP_KENSA.nKensaWaitStop = nVal(nOtherPcIdx)) Then         ' 稼動状態：稼動中 and ロジックパララン開始中、停止中（別PC）
            btnLpStart.Enabled = False
            btnLpEnd.Enabled = False
        ElseIf EM_DIV_KADOU.DIV_KADOU_START = m_nKadouStatus _
           AndAlso EM_LP_KENSA.nKensaStop = nVal(nMyPcIdx) Then                         ' 稼動状態：稼動中 and ロジックパララン停止
            btnLpStart.Enabled = True
            btnLpEnd.Enabled = False
        ElseIf EM_DIV_KADOU.DIV_KADOU_START = m_nKadouStatus _
           AndAlso EM_LP_KENSA.nKensaStart = nVal(nMyPcIdx) Then                        ' 稼動状態：稼動中 and ロジックパララン検査中
            btnLpStart.Enabled = False
            btnLpEnd.Enabled = True
        Else                            ' 上記以外
            btnLpStart.Enabled = False
            btnLpEnd.Enabled = False
        End If

        ' 状態ラベル設定
        Select Case nVal(nMyPcIdx)
            Case EM_LP_KENSA.nKensaStop         '停止
                lblLPStart.Text = "停止"
                lblLPStart.BackColor = g_ColorKadouStop
                lblLPStart.ForeColor = Color.Yellow

            Case EM_LP_KENSA.nKensaWaitStop     '停止中
                lblLPStart.Text = "停止中"
                lblLPStart.BackColor = g_ColorKadouStop
                lblLPStart.ForeColor = Color.Yellow

            Case EM_LP_KENSA.nKensaWaitStart    '開始中
                lblLPStart.Text = "開始中"
                lblLPStart.BackColor = Color.Lime
                lblLPStart.ForeColor = Color.Black

            Case EM_LP_KENSA.nKensaStart        '検査中
                lblLPStart.Text = "検査中"
                lblLPStart.BackColor = g_ColorKadouStart
                lblLPStart.ForeColor = Color.Yellow

            Case EM_LP_KENSA.nNoAnser           '応答無し
                lblLPStart.Text = "応答無し"
                lblLPStart.BackColor = Color.Magenta
                lblLPStart.ForeColor = Color.Yellow

            Case EM_LP_KENSA.nGetData           '状態取得中
                lblLPStart.Text = "状態取得中"
                lblLPStart.BackColor = Color.Lime
                lblLPStart.ForeColor = Color.Black

        End Select

    End Sub


    ''' <summary>
    ''' 運転モードラジオボタンチェックイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub rdoUnten_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Dim rdoData As RadioButton = CType(sender, RadioButton)

        ''画面更新中でない場合かつ稼動停止中以外またはコイル情報待ちの場合、操作無効
        If (m_bLock = False And EM_DIV_KADOU.DIV_KADOU_STOP <> m_nKadouStatus) _
            Or EM_ANSER_FLG.nCoilWaite = m_nAnser Then
            '元に戻す
            Call SetUntenStatus(m_nUntenStatus)
            Exit Sub
        End If

        'チェックが外れた場合は、初期化
        If Not rdoData.Checked Then
            rdoData.BackColor = Color.FromKnownColor(KnownColor.Control)
            rdoData.ForeColor = Color.Black
            Exit Sub
        End If

        Select Case CType(rdoData.Tag, EM_DIV_UNTEN)

            Case EM_DIV_UNTEN.DIV_UNTEN_STOP    '停止
                rdoData.BackColor = Color.FromKnownColor(KnownColor.Control)
                rdoData.ForeColor = Color.Black
            Case EM_DIV_UNTEN.DIV_UNTEN_NOMAL   '通常
                rdoData.BackColor = g_ColorUntenNomal
                rdoData.ForeColor = Color.Yellow
            Case EM_DIV_UNTEN.DIV_UNTEN_SAMP    'テスト
                rdoData.BackColor = g_ColorUntenSamp
                rdoData.ForeColor = Color.Yellow
            Case EM_DIV_UNTEN.DIV_UNTEN_CAMERA  '校正
                rdoData.BackColor = g_ColorUntenCamera
                rdoData.ForeColor = Color.Yellow
            Case EM_DIV_UNTEN.DIV_UNTEN_SMYU    'シミュレーション
                rdoData.BackColor = g_ColorUntenSmyu
                rdoData.ForeColor = Color.Yellow
            Case Else
                rdoData.BackColor = Color.FromKnownColor(KnownColor.Control)
                rdoData.ForeColor = Color.Black
        End Select

        'ステータスセット
        m_nUntenStatus = CType(rdoData.Tag, EM_DIV_UNTEN)


    End Sub

    ''' <summary>
    ''' PLGラジオボタンチェックイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub rdoPlg_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Dim rdoData As RadioButton = CType(sender, RadioButton)

        ''画面更新中でない場合かつ稼動停止中以外またはコイル情報待ちの場合、操作無効
        If (m_bLock = False And EM_DIV_KADOU.DIV_KADOU_STOP <> m_nKadouStatus) _
            Or EM_ANSER_FLG.nCoilWaite = m_nAnser Then

            '元に戻す
            Call SetPlgStatus(m_nPlgStatus)
            Exit Sub
        End If

        'チェックが外れた場合は、初期化
        If Not rdoData.Checked Then
            rdoData.BackColor = Color.FromKnownColor(KnownColor.Control)
            rdoData.ForeColor = Color.Black
            Exit Sub
        End If

        Select Case CType(rdoData.Tag, EM_DIV_PLG)
            Case EM_DIV_PLG.DIV_PLG_LINE    'ライン
                rdoData.BackColor = g_ColorPlgLine
                rdoData.ForeColor = Color.Yellow
            Case EM_DIV_PLG.DIV_PLG_TOP     '表校正
                rdoData.BackColor = g_ColorPlgTop
                rdoData.ForeColor = Color.Yellow
            Case EM_DIV_PLG.DIV_PLG_BOT     '裏校正
                rdoData.BackColor = g_ColorPlgBot
                rdoData.ForeColor = Color.Yellow
            Case EM_DIV_PLG.DIV_PLG_TEST    'テスト
                rdoData.BackColor = g_ColorPlgTest
                rdoData.ForeColor = Color.Yellow
            Case Else
                rdoData.BackColor = Color.FromKnownColor(KnownColor.Control)
                rdoData.ForeColor = Color.Black
        End Select

        'ステータスセット
        m_nPlgStatus = CType(rdoData.Tag, EM_DIV_PLG)

    End Sub

    ''' <summary>
    ''' 片面検査有無チェックイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub rdoKatamen__CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Dim rdoData As RadioButton = CType(sender, RadioButton)

        ''画面更新中でない場合かつ稼動停止中以外またはコイル情報待ちの場合、操作無効
        If (m_bLock = False And EM_DIV_KADOU.DIV_KADOU_STOP <> m_nKadouStatus) _
            Or EM_ANSER_FLG.nCoilWaite = m_nAnser Then
            ''元に戻す
            Call SetKatamenStatus(m_nKatamenStatus)
            Exit Sub
        End If

        'チェックが外れた場合は、初期化
        If Not rdoData.Checked Then
            rdoData.BackColor = Color.FromKnownColor(KnownColor.Control)
            rdoData.ForeColor = Color.Black
            Exit Sub
        End If

        Select Case CInt(rdoData.Tag)
            Case EM_KENSA_UMU.nKataMenNo
                rdoData.BackColor = Color.Magenta
                rdoData.ForeColor = Color.Yellow
            Case EM_KENSA_UMU.nKataMenYes
                rdoData.BackColor = Color.Blue
                rdoData.ForeColor = Color.Yellow
        End Select

        'ステータスセット
        m_nKatamenStatus = CInt(rdoData.Tag)

    End Sub

    ''' <summary>
    ''' 検査表面状態コンボボックス作成
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetScode()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String

        'SQL文作成
        strSql = ""
        strSql &= " SELECT SCODE"
        strSql &= " ,検査表面状態名称"
        strSql &= " FROM " & DB_SCODE_MASTER
        strSql &= " WHERE 有効判定 = 1"
        strSql &= " ORDER BY SCODE ASC "

        Try

            ' DB接続
            If Not tcls_DB.Open() Then Exit Sub

            'SQL実行
            sqlRead = tcls_DB.SelectExecute(strSql)
            If sqlRead Is Nothing Then Exit Try

            'コンボボックス生成
            cmbSCodeTop.Items.Clear()
            cmbSCodeBot.Items.Clear()
            While sqlRead.Read()
                cmbSCodeTop.Items.Add(New cls_CmbItem(CStr(sqlRead.GetValue(0)), CStr(sqlRead.GetValue(1)), CInt(sqlRead.GetValue(0))))
                cmbSCodeBot.Items.Add(New cls_CmbItem(CStr(sqlRead.GetValue(0)), CStr(sqlRead.GetValue(1)), CInt(sqlRead.GetValue(0))))
            End While

            'デフォルト値を表示
            If cmbSCodeTop.Items.Count > 0 Then
                cmbSCodeTop.SelectedIndex = cmbSCodeTop.Items.Count - 1
            End If
            If cmbSCodeBot.Items.Count > 0 Then
                cmbSCodeBot.SelectedIndex = cmbSCodeBot.Items.Count - 1
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "SCODE_MASTER取得失敗")
        Finally
            If Not sqlRead Is Nothing Then sqlRead.Close()
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try


    End Sub

    ''' <summary>
    ''' 検査パターンコードのコンボボックス作成
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetPcode()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String

        'SQL文作成
        strSql = ""
        strSql &= " SELECT PCODE"
        strSql &= " ,検査パターン名称"
        strSql &= " FROm " & DB_PCODE_MASTER
        strSql &= " WHERE 有効判定 = 1"
        strSql &= " ORDER BY PCODE ASC "

        Try

            ' DB接続
            If Not tcls_DB.Open() Then Exit Sub

            'SQL実行
            sqlRead = tcls_DB.SelectExecute(strSql)
            If sqlRead Is Nothing Then Exit Try

            'コンボボックス生成
            cmbPCodeTop.Items.Clear()
            cmbPCodeBot.Items.Clear()
            While sqlRead.Read()
                cmbPCodeTop.Items.Add(New cls_CmbItem(CStr(sqlRead.GetValue(0)), CStr(sqlRead.GetValue(1)), CInt(sqlRead.GetValue(0))))
                cmbPCodeBot.Items.Add(New cls_CmbItem(CStr(sqlRead.GetValue(0)), CStr(sqlRead.GetValue(1)), CInt(sqlRead.GetValue(0))))
            End While

            'デフォルト値を表示
            If cmbPCodeTop.Items.Count > 0 Then
                cmbPCodeTop.SelectedIndex = cmbPCodeTop.Items.Count - 1
            End If
            If cmbPCodeBot.Items.Count > 0 Then
                cmbPCodeBot.SelectedIndex = cmbPCodeBot.Items.Count - 1
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "PCODE_MASTER取得失敗")
        Finally
            If Not sqlRead Is Nothing Then sqlRead.Close()
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try


    End Sub


    ''' <summary>
    ''' コンボボックス選択
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbData_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Dim cmbData As ComboBox = CType(sender, ComboBox)

        Select Case CType(cmbData.Tag, EM_COMBO_MODE)

            Case EM_COMBO_MODE.nScodeTop    '検査表面状態 表

                '検査表面状態名称を表示
                lblSCodeTop.Text = CType(cmbData.Items(cmbData.SelectedIndex), cls_CmbItem).name

            Case EM_COMBO_MODE.nPcodeTop    '検査パターン 表

                '検査パターン名称を表示
                lblPCodeTop.Text = CType(cmbData.Items(cmbData.SelectedIndex), cls_CmbItem).name

            Case EM_COMBO_MODE.nScodeBot    '検査表面状態 裏

                '検査表面状態名称を表示
                lblSCodeBot.Text = CType(cmbData.Items(cmbData.SelectedIndex), cls_CmbItem).name

            Case EM_COMBO_MODE.nPcodeBot    '検査パターン 裏

                '検査パターン名称を表示
                lblPCodeBot.Text = CType(cmbData.Items(cmbData.SelectedIndex), cls_CmbItem).name


        End Select

    End Sub


    ''' <summary>
    ''' 稼動状態通知受信処理
    ''' </summary>
    ''' <param name="que_l"></param>
    ''' <remarks></remarks>
    Private Sub ReceiveFACT_SO_GAMENN_01(ByVal que_l As COMMON_QUE_L)
        '検査停止開始設定画面が表示中なら設定

        If tProp_Anser = frmSystemStatus.EM_ANSER_FLG.nFirstWaite OrElse tProp_Anser = frmSystemStatus.EM_ANSER_FLG.nWaite Then
            tProp_Anser = frmSystemStatus.EM_ANSER_FLG.nAnser   '応答完了
        Else

            '現在の状態が停止中の場合は、無効
            If tProp_KadouStatus = EM_DIV_KADOU.DIV_KADOU_STOP Then

                ' ロジックパラランの状態のみ更新
                tProp_LPStatusPc(0) = que_l.data(4) 'ロジックパラランPC01検査状態
                tProp_LPStatusPc(1) = que_l.data(5) 'ロジックパラランPC02検査状態

                'ロジックパララン状態セット
                For ii As Integer = 0 To NUM_HANTEI_PARA - 1
                    Call SetLpStatus(ii, m_emLPStatusPc)
                Next

                Exit Sub
            End If
            tProp_Anser = frmSystemStatus.EM_ANSER_FLG.nUpdata  '更新要求
        End If

        tProp_UntenStatus = que_l.data(0)   '運転モード
        tProp_PlgStatus = que_l.data(1)     'PLG区分
        tProp_KatamenStatus = que_l.data(2) '片面検査許可、不許可
        tProp_KadouStatus = que_l.data(3)   '稼動状態
        tProp_LPStatusPc(0) = que_l.data(4) 'ロジックパラランPC01検査状態
        tProp_LPStatusPc(1) = que_l.data(5) 'ロジックパラランPC02検査状態
    End Sub

    ''' <summary>
    ''' ファンクションボタン
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="onKey"></param>
    Private Sub frmSystemStatus_tEv_FunctionButton(FncNo As Integer, onKey As Keys) Handles MyBase.tEv_FunctionButton
        ' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 11                             ' F12 閉じる
                Me.Dispose()
        End Select
    End Sub

    ''' <summary>
    ''' 検査開始（PC01）ボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnLPStartPC1_Click(sender As Object, e As EventArgs) Handles btnLPStartPC1.Click

        SendKensaStart(EM_KENSA_KIND.nPC1, FACT_TO_DEFECT_21)

    End Sub

    ''' <summary>
    ''' 検査停止（PC01）ボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnLPEndPC1_Click(sender As Object, e As EventArgs) Handles btnLPEndPC1.Click

        SendKensaStop(EM_KENSA_KIND.nPC1, FACT_TO_DEFECT_22)

    End Sub

    ''' <summary>
    ''' 検査開始（PC02）ボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnLPStartPC2_Click(sender As Object, e As EventArgs) Handles btnLPStartPC2.Click

        SendKensaStart(EM_KENSA_KIND.nPC2, FACT_TO_DEFECT_21)

    End Sub

    ''' <summary>
    ''' 検査停止（PC02）ボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnLPEndPC2_Click(sender As Object, e As EventArgs) Handles btnLPEndPC2.Click

        SendKensaStop(EM_KENSA_KIND.nPC2, FACT_TO_DEFECT_22)

    End Sub

    ''' <summary>
    ''' 検査開始送信
    ''' </summary>
    ''' <param name="nKind">検査種別</param>
    ''' <param name="nEventNo">イベント番号</param>
    ''' <remarks></remarks>
    Private Sub SendKensaStart(ByVal nKind As EM_KENSA_KIND, ByVal nEventNo As Integer)
        Dim que As KizuLibCLI.COMMON_QUE_STRMIX = Nothing
        Dim strMsg As String
        Dim strSend As String

        '選択チェック
        If Not CheckUnten() Then
            Call DspMsg("運転モードを選択してください。")
            Exit Sub
        End If

        If Not CheckPlg() Then
            Call DspMsg("PLGを選択してください。")
            Exit Sub
        End If

        If Not CheckKatamen() Then
            Call DspMsg("片面検査条件を選択してください。")
            Exit Sub
        End If

        If Not UpdCamSetNo(nKind) Then
            lblErrMsg.Text = "DB更新失敗　カメラ保持に失敗しました。"
        Else
            lblErrMsg.Text = ""
        End If

        '応答待ちタイマーセット
        Call SetTimer(TIMEOUT_TIME, EM_ANSER_FLG.nWaite)

        '送信データ設定
        que = SetStartQue(nKind, nEventNo)

        strMsg = " 運転ﾓｰﾄﾞ = " & que.idata(0)
        strMsg &= " PLGﾓｰﾄﾞ = " & que.idata(1)
        strMsg &= " 片面検査許可不許可 = " & que.idata(2)
        strMsg &= String.Format(" 検査表面状態表,裏 = {0},{1}", que.idata(3), que.idata(4))
        strMsg &= String.Format(" 検査ﾊﾟﾀｰﾝ表,裏 = {0},{1}", que.idata(5), que.idata(6))

        If EM_KENSA_KIND.nNormal = nKind Then
            strMsg &= String.Format(" ﾊﾟﾗﾗﾝPC01_ｶﾒﾗ01,02 = {0},{1}", que.idata(7), que.idata(8))
            strMsg &= String.Format(" ﾊﾟﾗﾗﾝPC02_ｶﾒﾗ01,02 = {0},{1}", que.idata(9), que.idata(10))
            strSend = "検査開始要求"
        Else
            strMsg &= " 検査開始区分 = " & que.idata(7)
            strMsg &= String.Format(" ﾊﾟﾗﾗﾝPC0{0}_ｶﾒﾗ01,02 = {1},{2}", que.idata(7) + 1, que.idata(8), que.idata(9))
            strSend = "途中開始要求"
        End If

        strMsg = String.Format("{0}", strSend) & strMsg

        '検査開始シスログ
        If NOA_OFFLINE <> g_NOA Then
            KizuLibCLI.KizuMod.SysLog(SYSNO_SYSTEM_START, String.Format("[{0}]", strMsg))
        End If

        '検査開始要求送信
        Call SendMail(que, TO_DEFECT, strSend)

        'ステータス初期化
        Call IntiStatus()

        '状態取得中
        Call SetKadouStatus(STATUS_WAIT)

        'ロジックパラランPC検査状態再セット
        For ii As Integer = 0 To NUM_HANTEI_PARA - 1
            Call SetLpStatus(ii, m_emLPStatusPc)
        Next

    End Sub

    ''' <summary>
    ''' 検査停止送信
    ''' </summary>
    ''' <param name="nKind">検査種別</param>
    ''' <param name="nEventNo">イベント番号</param>
    ''' <remarks></remarks>
    Private Sub SendKensaStop(ByVal nKind As EM_KENSA_KIND, ByVal nEventNo As Integer)
        Dim que As KizuLibCLI.COMMON_QUE_STRMIX = Nothing
        Dim bRet As DialogResult
        Dim strMsg As String
        Dim strSend As String

        '検査終了確認
        Call DspMsg("検査を終了しますか？", bRet)
        If System.Windows.Forms.DialogResult.OK <> bRet Then
            Exit Sub
        End If

        'ステータス初期化
        Call IntiStatus()

        '応答待ちタイマーセット
        Call SetTimer(TIMEOUT_TIME, EM_ANSER_FLG.nWaite)

        '状態取得中
        Call SetKadouStatus(STATUS_WAIT)

        'ロジックパラランPC検査状態再セット
        For ii As Integer = 0 To NUM_HANTEI_PARA - 1
            Call SetLpStatus(ii, m_emLPStatusPc)
        Next

        '送信データ設定
        strMsg = ""
        que.inisialize()
        que.nEventNo = nEventNo
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()
        If EM_KENSA_KIND.nNormal = nKind Then
            strSend = "検査停止要求"
        Else
            que.idata(0) = nKind
            strMsg = " 検査停止区分 = " & que.idata(0)
            strSend = "途中停止要求"
        End If

        strMsg = String.Format("{0}", strSend) & strMsg

        '検査停止要求送信
        Call SendMail(que, TO_DEFECT, strSend)

        '検査停止シスログ
        If NOA_OFFLINE <> g_NOA Then
            KizuLibCLI.KizuMod.SysLog(SYSNO_SYSTEM_STOP, String.Format("[{0}]", strMsg))
        End If

        'エラーメッセージ初期化
        lblErrMsg.Text = ""

    End Sub

    ''' <summary>
    '''QUE情報作成
    ''' </summary>
    ''' <param name="nKind">検査種別</param>
    ''' <param name="nEventNo">イベント番号</param>
    ''' <remarks></remarks>
    Public Function SetStartQue(ByVal nKind As EM_KENSA_KIND, ByVal nEventNo As Integer) As KizuLibCLI.COMMON_QUE_STRMIX
        Dim que As KizuLibCLI.COMMON_QUE_STRMIX = Nothing

        '送信データ設定
        que.inisialize()
        que.nEventNo = nEventNo
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()
        que.idata(0) = m_nUntenStatus    '運転モード
        que.idata(1) = m_nPlgStatus      'PLGモード
        que.idata(2) = m_nKatamenStatus  '片面検査許可不許可

        '初期表面状態コード 表
        If -1 <> cmbSCodeTop.SelectedIndex Then
            que.idata(3) = CType(cmbSCodeTop.Items(cmbSCodeTop.SelectedIndex), cls_CmbItem).sendid
        Else
            '実際は、ありえないが念のため
            que.idata(3) = 1
        End If

        '初期表面状態コード 裏
        If -1 <> cmbSCodeBot.SelectedIndex Then
            que.idata(4) = CType(cmbSCodeBot.Items(cmbSCodeBot.SelectedIndex), cls_CmbItem).sendid
        Else
            '実際は、ありえないが念のため
            que.idata(4) = 1
        End If

        '初期検査パターンコード　表
        If -1 <> cmbPCodeTop.SelectedIndex Then
            que.idata(5) = CType(cmbPCodeTop.Items(cmbPCodeTop.SelectedIndex), cls_CmbItem).sendid
        Else
            '実際は、ありえないが念のため
            que.idata(5) = 1
        End If

        '初期検査パターンコード 裏
        If -1 <> cmbPCodeBot.SelectedIndex Then
            que.idata(6) = CType(cmbPCodeBot.Items(cmbPCodeBot.SelectedIndex), cls_CmbItem).sendid
        Else
            '実際は、ありえないが念のため
            que.idata(6) = 1
        End If

        If EM_KENSA_KIND.nNormal = nKind Then
            que.idata(7) = CType(cmbCamera1PC1.SelectedIndex, Integer)  'ロジックパララン機能01_入力カメラ01
            que.idata(8) = CType(cmbCamera2PC1.SelectedIndex, Integer)  'ロジックパララン機能01_入力カメラ02
            que.idata(9) = CType(cmbCamera1PC2.SelectedIndex, Integer)  'ロジックパララン機能02_入力カメラ01
            que.idata(10) = CType(cmbCamera2PC2.SelectedIndex, Integer) 'ロジックパララン機能02_入力カメラ02
        ElseIf EM_KENSA_KIND.nPC1 = nKind Then
            que.idata(7) = nKind                                        '検査開始区分
            que.idata(8) = CType(cmbCamera1PC1.SelectedIndex, Integer)  'ロジックパララン機能01_入力カメラ01
            que.idata(9) = CType(cmbCamera2PC1.SelectedIndex, Integer)  'ロジックパララン機能01_入力カメラ02
        Else
            que.idata(7) = nKind                                        '検査開始区分
            que.idata(8) = CType(cmbCamera1PC2.SelectedIndex, Integer)  'ロジックパララン機能02_入力カメラ01
            que.idata(9) = CType(cmbCamera2PC2.SelectedIndex, Integer)  'ロジックパララン機能02_入力カメラ02
        End If

        Return que
    End Function

    ''' <summary>
    ''' 画面カメラセット設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetCamSetIndex()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String

        'SQL文作成
        strSql = " SELECT ロジックパララン機能01_入力カメラ01"
        strSql &= " ,ロジックパララン機能01_入力カメラ02"
        strSql &= " ,ロジックパララン機能02_入力カメラ01"
        strSql &= " ,ロジックパララン機能02_入力カメラ02"
        strSql &= " FROM " & DB_COMMON_MASTER

        Try
            ' DB接続
            If Not tcls_DB.Open() Then Exit Sub

            'SQL実行
            sqlRead = tcls_DB.SelectExecute(strSql)
            If sqlRead Is Nothing Then Exit Try

            With sqlRead
                .Read()
                cmbCamera1PC1.SelectedIndex = GetCamSetIndex(CType(.GetValue(0), Integer))
                cmbCamera2PC1.SelectedIndex = GetCamSetIndex(CType(.GetValue(1), Integer))
                cmbCamera1PC2.SelectedIndex = GetCamSetIndex(CType(.GetValue(2), Integer))
                cmbCamera2PC2.SelectedIndex = GetCamSetIndex(CType(.GetValue(3), Integer))
            End With

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "ロジックパララン 入力カメラ設定取得失敗")
        Finally
            If Not sqlRead Is Nothing Then sqlRead.Close()
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub

    ''' <summary>
    ''' 引数の値からカメラセットコンボボックスに設定するIndexを取得
    ''' </summary>
    ''' <param name="nValue">Index変換値</param>
    ''' <remarks></remarks>
    Private Function GetCamSetIndex(ByVal nValue As Integer) As Integer

        'カメラセットNoの範囲外であれば未選択に設定
        If MAX_CAMSET_NO < nValue Or 0 > nValue Then
            Return 0
        End If

        Return nValue

    End Function

    ''' <summary>
    ''' カメラセット設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetCamSet()
        'コンボボックス初期化
        cmbCamera1PC1.Items.Clear()
        cmbCamera2PC1.Items.Clear()
        cmbCamera1PC2.Items.Clear()
        cmbCamera2PC2.Items.Clear()

        '未選択時の値
        cmbCamera1PC1.Items.Add(" ")
        cmbCamera2PC1.Items.Add(" ")
        cmbCamera1PC2.Items.Add(" ")
        cmbCamera2PC2.Items.Add(" ")

        'カメラセットMAX分選択項目を設定
        For ii As Integer = 1 To MAX_CAMSET_NO
            cmbCamera1PC1.Items.Add(ii)
            cmbCamera2PC1.Items.Add(ii)
            cmbCamera1PC2.Items.Add(ii)
            cmbCamera2PC2.Items.Add(ii)
        Next

        'デフォルト値を設定
        cmbCamera1PC1.SelectedIndex = 0
        cmbCamera2PC1.SelectedIndex = 0
        cmbCamera1PC2.SelectedIndex = 0
        cmbCamera2PC2.SelectedIndex = 0

        '現在のカメラセット状態を画面に設定
        Call SetCamSetIndex()

    End Sub

    ''' <summary>
    ''' 入力カメラ設定更新
    ''' </summary>
    ''' <remarks></remarks>
    Private Function UpdCamSetNo(ByVal nKind As EM_KENSA_KIND) As Boolean
        Dim bResult As Boolean = True
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSql As String

        'SQL文作成
        strSql = "UPDATE " & DB_COMMON_MASTER
        strSql &= " SET "
        If EM_KENSA_KIND.nPC1 = nKind Or EM_KENSA_KIND.nNormal = nKind Then
            strSql &= tClass.tClass_SQLServer.sqlAddUpdate("ロジックパララン機能01_入力カメラ01", cmbCamera1PC1.SelectedIndex)
            strSql &= tClass.tClass_SQLServer.sqlAddUpdate("ロジックパララン機能01_入力カメラ02", cmbCamera2PC1.SelectedIndex)
        End If

        If EM_KENSA_KIND.nPC2 = nKind Or EM_KENSA_KIND.nNormal = nKind Then
            strSql &= tClass.tClass_SQLServer.sqlAddUpdate("ロジックパララン機能02_入力カメラ01", cmbCamera1PC2.SelectedIndex)
            strSql &= tClass.tClass_SQLServer.sqlAddUpdate("ロジックパララン機能02_入力カメラ02", cmbCamera2PC2.SelectedIndex)
        End If

        tClass.tClass_SQLServer.sqlColumnTrim(strSql)

        Try
            ' DB接続
            If Not tcls_DB.Open() Then Return False

            'SQL実行
            If 0 > tcls_DB.DMLExecute(strSql) Then
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "ロジックパララン 入力カメラ設定更新失敗")
                bResult = False
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "ロジックパララン 入力カメラ設定更新失敗")
            bResult = False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        Return bResult

    End Function

End Class
