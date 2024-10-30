Imports tClassLibrary

Public Class frmMain

#Region "ローカル定数"

    ''' <summary>
    ''' このタスク専用のiniファイルパス
    ''' </summary>
    ''' <remarks></remarks>
    Private Const mcINIFILE As String = ".\KizuBackUp.ini"

#End Region

#Region "ローカル変数"

    ''' <summary>
    ''' 統括兼蓄積PCのKizuPc.iniの保存場所
    ''' </summary>
    ''' <remarks></remarks>
    Private m_iniPcIni As String = ""
    ''' <summary>
    ''' 統括兼蓄積PCのKizuPc.iniの現在実行中の場所
    ''' </summary>
    ''' <remarks></remarks>
    Private m_iniPcIniToFPath As String = ""

    ''' <summary>
    ''' 統括兼蓄積機能_A系が動作しているPC名称
    ''' </summary>
    ''' <remarks>フォームロード時にKizuBackUp.iniから読込</remarks>
    Private m_sName_A_To As String
    ''' <summary>
    ''' 統括兼蓄積機能_B系が動作しているPC名称
    ''' </summary>
    ''' <remarks>フォームロード時にKizuBackUp.iniから読込</remarks>
    Private m_sName_B_To As String
    ''' <summary>
    ''' 統括兼蓄積機能_A系が動作しているPC名称(日本語)
    ''' </summary>
    ''' <remarks>フォームロード時にKizuBackUp.iniから読込</remarks>
    Private m_sNameJ_A_To As String
    ''' <summary>
    ''' 統括兼蓄積機能_B系が動作しているPC名称(日本語)
    ''' </summary>
    ''' <remarks>フォームロード時にKizuBackUp.iniから読込</remarks>
    Private m_sNameJ_B_To As String

    ''' <summary>
    ''' 表示PC オフライン系の台数
    ''' </summary>
    ''' <remarks>フォームロード時にKizuBackUp.iniから読込</remarks>
    Private m_nHyOffLineNum As Integer
    ''' <summary>
    ''' 表示PC オンライン系の台数
    ''' </summary>
    ''' <remarks>フォームロード時にKizuBackUp.iniから読込</remarks>
    Private m_nHyOnLineNum As Integer
    ''' <summary>
    ''' 表示オフラインPCのコンピュータ名
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sName_Off_Hy() As String
    ''' <summary>
    ''' 表示オフラインPCのコンピュータ名(日本語)
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sNameJ_Off_Hy() As String
    ''' <summary>
    ''' 表示オフライン機能の名称
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sKind_Off_Hy() As String

    ''' <summary>
    ''' 表示機能が動作しているPC名称
    ''' </summary>
    ''' <remarks>オンライン系の台数をKizuBackUp.iniから取得後、Redimする</remarks>
    Private m_sName_Hy() As String
    ''' <summary>
    ''' 表示機能が動作しているPC名称(日本語)
    ''' </summary>
    ''' <remarks>オンライン系の台数をKizuBackUp.iniから取得後、Redimする</remarks>
    Private m_sNameJ_Hy() As String
    ''' <summary>
    ''' 表示PCで動作している機能
    ''' </summary>
    ''' <remarks>オンライン系の台数をKizuBackUp.iniから取得後、Redimする</remarks>
    Private m_sKind_Hy() As String

    ''' <summary>
    ''' 操作機能_A系が動作しているPC名称
    ''' </summary>
    ''' <remarks>フォームロード時にKizuBackUp.iniから読込</remarks>
    Private m_sName_A_So As String
    ''' <summary>
    ''' 操作機能_B系が動作しているPC名称
    ''' </summary>
    ''' <remarks>フォームロード時にKizuBackUp.iniから読込</remarks>
    Private m_sName_B_So As String
    ''' <summary>
    ''' 操作機能_A系が動作しているPC名称(日本語)
    ''' </summary>
    ''' <remarks>フォームロード時にKizuBackUp.iniから読込</remarks>
    Private m_sNameJ_A_So As String
    ''' <summary>
    ''' 操作機能_B系が動作しているPC名称(日本語)
    ''' </summary>
    ''' <remarks>フォームロード時にKizuBackUp.iniから読込</remarks>
    Private m_sNameJ_B_So As String

    ''' <summary>
    ''' 全長画像処理PC オンライン台数
    ''' </summary>
    ''' <remarks></remarks>
    Private m_nRpiOnLineNum As Integer
    ''' <summary>
    ''' 全長画像処理PC オフライン台数
    ''' </summary>
    ''' <remarks></remarks>
    Private m_nRpiOffLineNum As Integer
    ''' <summary>
    ''' 全長画像処理オンラインPC名称
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sName_RpiOn() As String
    ''' <summary>
    ''' 全長画像処理オンラインPC名称(日本語)
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sNameJ_RpiOn() As String
    ''' <summary>
    ''' 全長画像処理オンラインPCで動作している機能
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sKind_RpiOn() As String
    ''' <summary>
    ''' 全長画像処理オフラインPC名称
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sName_RpiOff() As String
    ''' <summary>
    ''' 全長画像処理オフラインPC名称(日本語)
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sNameJ_RpiOff() As String
    ''' <summary>
    ''' 全長画像処理オフラインPCで動作している機能
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sKind_RpiOff() As String

    Public Sub New()

        ' この呼び出しはデザイナーで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

    End Sub

#End Region

#Region "フォームロード"

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        ''=============================================
        '' iniファイルその他から初期設定を読込

        '''' ラインID
        KizuLibCLI.KizuMod.SetLineID()


        '''' 共通項目のiniファイル値読み込み
        m_iniPcIni = tMod.ReadIniStr("COMMON", "PCINI", mcINIFILE)
        m_iniPcIniToFPath = tMod.ReadIniStr("COMMON", "PCINITO", mcINIFILE)



        '''' 画面固定フォーマットをつくる
        ' 統括兼蓄積機能
        m_sName_A_To = tMod.ReadIniStr("TO", "PCNAME_A", mcINIFILE)
        m_sName_B_To = tMod.ReadIniStr("TO", "PCNAME_B", mcINIFILE)
        m_sNameJ_A_To = tMod.ReadIniStr("TO", "PCNAME_J_A", mcINIFILE)
        m_sNameJ_B_To = tMod.ReadIniStr("TO", "PCNAME_J_B", mcINIFILE)

        ' 表示機能
        m_nHyOffLineNum = tMod.ReadIniInt("HY", "HY_OFF_NUM", mcINIFILE)
        ReDim m_sName_Off_Hy(m_nHyOffLineNum - 1)
        ReDim m_sNameJ_Off_Hy(m_nHyOffLineNum - 1)
        ReDim m_sKind_Off_Hy(m_nHyOffLineNum - 1)
        For ii As Integer = 0 To m_nHyOffLineNum - 1
            m_sName_Off_Hy(ii) = tMod.ReadIniStr("HY", "PCNAME_OFF_" & ii + 1, mcINIFILE)
            m_sNameJ_Off_Hy(ii) = tMod.ReadIniStr("HY", "PCNAME_J_OFF_" & ii + 1, mcINIFILE)
            m_sKind_Off_Hy(ii) = tMod.ReadIniStr("HY", "KINDNAME_OFF_" & ii + 1, mcINIFILE)
        Next
        m_nHyOnLineNum = tMod.ReadIniInt("HY", "HY_ON_NUM", mcINIFILE)
        ReDim m_sName_Hy(m_nHyOnLineNum - 1)
        ReDim m_sNameJ_Hy(m_nHyOnLineNum - 1)
        ReDim m_sKind_Hy(m_nHyOnLineNum - 1)
        For ii As Integer = 0 To m_nHyOnLineNum - 1
            m_sName_Hy(ii) = tMod.ReadIniStr("HY", "PCNAME_" & ii + 1, mcINIFILE)
            m_sNameJ_Hy(ii) = tMod.ReadIniStr("HY", "PCNAME_J_" & ii + 1, mcINIFILE)
            m_sKind_Hy(ii) = tMod.ReadIniStr("HY", "KINDNAME_" & ii + 1, mcINIFILE)
        Next

        ' 操作機能
        m_sName_A_So = tMod.ReadIniStr("SO", "PCNAME_A", mcINIFILE)
        m_sName_B_So = tMod.ReadIniStr("SO", "PCNAME_B", mcINIFILE)
        m_sNameJ_A_So = tMod.ReadIniStr("SO", "PCNAME_J_A", mcINIFILE)
        m_sNameJ_B_So = tMod.ReadIniStr("SO", "PCNAME_J_B", mcINIFILE)

        ' 全長画像処理機能
        m_nRpiOnLineNum = tMod.ReadIniInt("RPI", "RPI_ON_NUM", mcINIFILE)
        ReDim m_sName_RpiOn(m_nRpiOnLineNum - 1)
        ReDim m_sNameJ_RpiOn(m_nRpiOnLineNum - 1)
        ReDim m_sKind_RpiOn(m_nRpiOnLineNum - 1)
        m_nRpiOffLineNum = tMod.ReadIniInt("RPI", "RPI_OFF_NUM", mcINIFILE)
        ReDim m_sName_RpiOff(m_nRpiOffLineNum - 1)
        ReDim m_sNameJ_RpiOff(m_nRpiOffLineNum - 1)
        ReDim m_sKind_RpiOff(m_nRpiOffLineNum - 1)

        For ii As Integer = 0 To m_nRpiOnLineNum - 1
            m_sName_RpiOn(ii) = tMod.ReadIniStr("RPI", "PCNAME_ON_" & ii + 1, mcINIFILE)
            m_sNameJ_RpiOn(ii) = tMod.ReadIniStr("RPI", "PCNAME_J_ON_" & ii + 1, mcINIFILE)
            m_sKind_RpiOn(ii) = tMod.ReadIniStr("RPI", "KINDNAME_ON_" & ii + 1, mcINIFILE)
        Next
        For ii As Integer = 0 To m_nRpiOffLineNum - 1
            m_sName_RpiOff(ii) = tMod.ReadIniStr("RPI", "PCNAME_OFF_" & ii + 1, mcINIFILE)
            m_sNameJ_RpiOff(ii) = tMod.ReadIniStr("RPI", "PCNAME_J_OFF_" & ii + 1, mcINIFILE)
            m_sKind_RpiOff(ii) = tMod.ReadIniStr("RPI", "KINDNAME_OFF_" & ii + 1, mcINIFILE)
        Next



        ''=============================================
        '' 統括兼蓄積機能
        lblTo_A.Text = m_sNameJ_A_To
        lblTo_B.Text = m_sNameJ_B_To
        btnTO_ChangeTo.Text = String.Format(btnTO_ChangeTo.Text, m_sNameJ_A_To)
        btnTO_ChangeBk.Text = String.Format(btnTO_ChangeBk.Text, m_sNameJ_B_To)



        ' ======================
        ' 表示機能
        'btnHy_Change.Text = String.Format(btnHy_Change.Text, m_sNameJ_Off_Hy)
        'btnHy_ChangeOff.Text = String.Format(btnHy_ChangeOff.Text, m_sNameJ_Off_Hy, m_sKind_Off_Hy)
        cmbHy_Select.Items.Clear()
        For ii As Integer = 0 To m_nHyOnLineNum - 1
            Dim cls As New tClass_CmbItem(ii, m_sKind_Hy(ii))
            cmbHy_Select.Items.Add(cls)
        Next
        cmbHy_Select.SelectedIndex = 0



        ' ======================
        ' 操作機能
        btnSO_ChangeSo.Text = String.Format(btnSO_ChangeSo.Text, m_sNameJ_A_So)
        btnSO_ChangeKa.Text = String.Format(btnSO_ChangeKa.Text, m_sNameJ_B_So)



        ' ======================
        ' 全長画像処理機能
        ' DBから状態取得出来たら、ボタン表示を更新する
        cmbRpi_Select.Items.Clear()
        For ii As Integer = 0 To m_nRpiOnLineNum - 1
            Dim cls As New tClass_CmbItem(ii, m_sKind_RpiOn(ii))
            cmbRpi_Select.Items.Add(cls)
        Next
        cmbRpi_Select.SelectedIndex = 0



    End Sub

    ''' <summary>
    ''' フォームロード終了時に1回だけ非同期実行
    ''' </summary>
    ''' <remarks></remarks>
    Protected Overrides Sub LoadEnding()
        ' 元々のメソッドを実行
        MyBase.LoadEnding()
        Application.DoEvents()


        ' ここから拡張分
        Call CheckTo_Status()       ' 統括兼蓄積機能 状態チェック

        Call CheckHy_Status()       ' 表示機能初期チェック

        Call CheckSo_Status()       ' 操作機能初期チェック

        Call CheckRpi_Status()      ' 全長画像機能初期チェック

        Application.DoEvents()      ' 画面がちゃんとでるようにする

        SetMsgBox(True, "バックアップ切替支援")

    End Sub

#End Region


#Region "統括兼蓄積機能"

    ''' <summary>
    ''' 統括兼蓄積機能 状態チェック
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CheckTo_Status()
        Dim strPCNameWk As String = tMod.ReadIniStr("TO", "PCNAME_BACK", mcINIFILE)
        Dim strPcName As String = tMod.tMod_Common.GetNetBiosComputerName()     ' PC名称
        If strPcName <> strPCNameWk Then
            lblTo_NowBk.Text = "不明(操作対象外PC)"
            lblTo_NowBk.BackColor = Color.Tomato
        Else

            '''' 現在の操作PCで実行の機能を把握する
            Dim strPath As String = tMod.ReadIniStr("TO", "INIPC_FPATH", mcINIFILE)
            Dim nWk As Integer = tMod.ReadIniInt("NOWID", "ID", strPath)


            '''' チェック
            If nWk = 0 Then
                lblTo_NowBk.Text = "バックアップ機能"
                lblTo_NowBk.BackColor = Color.DodgerBlue
            ElseIf nWk = 1 Then
                lblTo_NowBk.Text = "統括兼蓄積機能"
                lblTo_NowBk.BackColor = Color.Tomato
            Else
                lblTo_NowBk.Text = "不明"
                lblTo_NowBk.BackColor = Color.Red
            End If

        End If



        '''' DB状態取得
        ' A系
        Dim bWk As Boolean = tClass.tClass_SQLServer.OpenCheck(TASKINI_NAME, INI_DB_A)
        SetTi_Status(bWk, lblTO_DbStatusA)

        ' B系
        bWk = tClass.tClass_SQLServer.OpenCheck(TASKINI_NAME, INI_DB_B)
        SetTi_Status(bWk, lblTO_DbStatusB)

    End Sub

    ''' <summary>
    ''' DB状態をセット
    ''' </summary>
    ''' <param name="bMode">true:正常 false:異常</param>
    ''' <remarks></remarks>
    Private Sub SetTi_Status(ByVal bMode As Boolean, ByVal lbl As Label)
        If bMode Then
            lbl.Text = "正常"
            lbl.BackColor = Color.DodgerBlue
        Else
            lbl.Text = "異常"
            lbl.BackColor = Color.Tomato
        End If
    End Sub

    ''' <summary>
    ''' 統括兼蓄積機能 切替実行
    ''' </summary>
    ''' <param name="sender">クリックされたボタン</param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnTO_ChangeBk_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTO_ChangeBk.Click, btnTO_ChangeTo.Click
        Dim strMsg As String = ""           ' メッセージ用
        Dim strIniKey As String = INI_DB_A  ' iniファイルのDBキー

        '''' 実行できるのは、バックアップPCのみ
        Dim strPCNameWk As String = tMod.ReadIniStr("TO", "PCNAME_BACK", mcINIFILE)
        Dim strPcName As String = tMod.tMod_Common.GetNetBiosComputerName()     ' PC名称
        If strPcName <> strPCNameWk Then
            MsgBox(String.Format("本操作は、{0}で実行してください。", m_sNameJ_B_To), MsgBoxStyle.OkOnly Or MsgBoxStyle.Critical)
            Return
        End If

        '''' A系→B系 or B系→A系
        If sender Is btnTO_ChangeTo Then        ' B→A
            strIniKey = INI_DB_A
            strMsg = String.Format("【{0}(正規機)で 統括兼蓄積機能 を実行】", m_sNameJ_A_To)

        ElseIf sender Is btnTO_ChangeBk Then    ' A→B
            strIniKey = INI_DB_B
            strMsg = String.Format("【{0}で 統括兼蓄積機能 を実行】", m_sNameJ_B_To)

        End If

        '''' 確認
        Dim retc As MsgBoxResult = MsgBox("統括兼蓄積機能の切替を実施します。" & vbCrLf & strMsg, _
            MsgBoxStyle.OkCancel Or MsgBoxStyle.DefaultButton2 Or MsgBoxStyle.Question, "統括兼蓄積機能切替")
        If retc <> MsgBoxResult.Ok Then Return


        ' =======================================================
        ' DB操作
        ' =======================================================
        '''' 情報の取得
        Dim strSQL As String
        Dim nPcKind_A As Integer = tMod.ReadIniInt("TO", "PCKIND_A", mcINIFILE)
        Dim nPcKind_B As Integer = tMod.ReadIniInt("TO", "PCKIND_B", mcINIFILE)
        Dim nPcid_A As Integer = tMod.ReadIniInt("TO", "PCID_A", mcINIFILE)
        Dim nPcid_B As Integer = tMod.ReadIniInt("TO", "PCID_B", mcINIFILE)
        Dim nKikiid_HB_TO As Integer = tMod.ReadIniInt("TO", "KIKIDI_HB_TO", mcINIFILE)
        Dim nKikiid_HB_BK As Integer = tMod.ReadIniInt("TO", "KIKIDI_HB_BK", mcINIFILE)
        If 0 = nPcKind_A Or 0 = nPcKind_B Or 0 = nPcid_A Or 0 = nPcid_B Then
            SetMsgBox(False, "統括兼蓄積機能切替に必要なPC_INF情報が足りません。")
            Return
        End If

        '''' PCIDを書換
        Dim cls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, strIniKey)
        Using cls_DB
            If Not cls_DB.Open() Then
                SetMsgBox(False, "データベース(" & strIniKey & ") に接続できません")
                Return
            End If

            ' B系→A系のみ
            If strIniKey = INI_DB_A Then
                '''' バックアップ機のPCINFテーブルを書換
                strSQL = "UPDATE " & DB_PC_INF & " SET PCID=" & nPcid_A & " WHERE PCKIND=" & nPcKind_A
                cls_DB.DMLExecute(strSQL)
                strSQL = "UPDATE " & DB_PC_INF & " SET PCID=" & nPcid_B & " WHERE PCKIND=" & nPcKind_B
                cls_DB.DMLExecute(strSQL)
            End If

            ' A系→B系のみ
            If strIniKey = INI_DB_B Then
                '''' バックアップ機のPCINFテーブルを書換
                strSQL = "UPDATE " & DB_PC_INF & " SET PCID=" & nPcid_B & " WHERE PCKIND=" & nPcKind_A
                cls_DB.DMLExecute(strSQL)
                strSQL = "UPDATE " & DB_PC_INF & " SET PCID=" & nPcid_A & " WHERE PCKIND=" & nPcKind_B
                cls_DB.DMLExecute(strSQL)
            End If

            '''' バックアップ用PCのハートビート監視を書き換え
            ' B系→A系のみ
            If sender Is btnTO_ChangeTo And nKikiid_HB_TO <> 0 Then
                strSQL = "UPDATE " & DB_STATUS_MASTER & " SET 回線確認有無=0, 初期値=1 WHERE 機器ID=" & nKikiid_HB_TO
                cls_DB.DMLExecute(strSQL)
            End If
            If sender Is btnTO_ChangeTo And nKikiid_HB_BK <> 0 Then
                strSQL = "UPDATE " & DB_STATUS_MASTER & " SET 回線確認有無=1, 初期値=1 WHERE 機器ID=" & nKikiid_HB_BK
                cls_DB.DMLExecute(strSQL)
            End If


            ' A系→B系のみ
            If sender Is btnTO_ChangeBk And nKikiid_HB_TO <> 0 Then
                strSQL = "UPDATE " & DB_STATUS_MASTER & " SET 回線確認有無=1, 初期値=0 WHERE 機器ID=" & nKikiid_HB_TO
                cls_DB.DMLExecute(strSQL)
            End If
            If sender Is btnTO_ChangeBk And nKikiid_HB_BK <> 0 Then
                strSQL = "UPDATE " & DB_STATUS_MASTER & " SET 回線確認有無=0, 初期値=0 WHERE 機器ID=" & nKikiid_HB_BK
                cls_DB.DMLExecute(strSQL)
            End If

            cls_DB.Close()
        End Using


        '★紙の声 IPアドレス変更するんだから、DBセッション値の書き換えは不要では・・・
        ' =======================================================
        ' 統括兼蓄積PCの全KizuPc.iniのDBセッション値を書換
        ' =======================================================
        ''For Each strFPath As String In System.IO.Directory.GetFiles(m_iniPcIni, "*KizuPc.ini")

        ''    tMod.WriteIni("DB", "SESSION", strFPath, strIniKey)
        ''Next


        ' =======================================================
        ' バックアップPCで実行するKizuTask.iniの切替
        ' =======================================================
        Dim strPath As String = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("TO", "INIPC", mcINIFILE))
        '' 自分の中のKizuTask.iniを切り替える
        strPath = tMod.ReadIniStr("TO", "INIPC_FPATH", mcINIFILE)
        If sender Is btnTO_ChangeTo Then        'A系
            tMod.WriteIni("NOWID", "ID", strPath, "0")
        Else
            tMod.WriteIni("NOWID", "ID", strPath, "1")
        End If



        ' ======================
        ' エンド処理
        ' ======================
        SetMsgBox(True, "統括兼蓄積機能切替が完了しました。" & vbCrLf & "全PCをシャットダウンしてください。")
        retc = MsgBox("統括兼蓄積機能の切替えが完了しました。" & vbCrLf & "全PCをシャットダウンします。", _
           MsgBoxStyle.OkOnly Or MsgBoxStyle.Information, "統括兼蓄積機能切替")

        Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
        que.inisialize()
        que.nLineNo = 0

        '''' 自分にシャットダウン通知
        ' KS_MASTER/KS_SLAVE のどちらが動いているかわからないので、両方に通知
        que.nEventNo = FACT_KS_MASTER_06
        que.nLineNo = 0
        Call KizuLibCLI.KizuMod.Send_Mail(KS_MASTER, "*", que)
        ' 全員にシャットダウン
        que.nEventNo = FACT_KS_SLAVE_04
        Call KizuLibCLI.KizuMod.Send_Mail(KS_SLAVE, "*", que)


        ''''' その他全PCに再起動要求
        'que.nEventNo = FACT_KS_SLAVE_05
        'que.data(0) = 0
        'Call KizuLibCLI.KizuMod.Send_Mail(KS_SLAVE, "*", que)


        ' KS_MASTERがメール送信後の待ち時間[秒]。(TO_DEFECTの停止完了を待ち合わせる時間)
        ' ※停止処理が終了する前にIPアドレスが変更されるとDBアクセス不可となり、システム終了が正常に行われない。
        '   IP変更前にシャットダウン処理が動作してしまうため、待ち時間-10msecとする
        Dim nWk As Integer = tMod.ReadIniInt("KS_MASTER", "STOP_AFFTER_WAIT_TIME", strPath) * 1000 - 10
        Threading.Thread.Sleep(nWk)

        ' ======================
        ' バックアップ機のIPアドレス変更
        ' ======================
        ' どうもIPアドレスが変わるとUDPでもメールが飛ばないので、この位置に移動したよ
        If sender Is btnTO_ChangeTo Then       'A系
            tMod.SetIpAddress(tMod.ReadIniStr("TO", "IP_B", mcINIFILE), tMod.ReadIniStr("TO", "IP_ADAPTER", mcINIFILE))
        Else
            tMod.SetIpAddress(tMod.ReadIniStr("TO", "IP_A", mcINIFILE), tMod.ReadIniStr("TO", "IP_ADAPTER", mcINIFILE))
        End If

    End Sub

    ''' <summary>
    ''' B系→A系 マスター同期
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnTO_Synclo_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTO_Synclo.Click
        '''' 確認
        Dim retc As MsgBoxResult = MsgBox("マスターの同期を実施します。", _
          MsgBoxStyle.OkCancel Or MsgBoxStyle.DefaultButton2 Or MsgBoxStyle.Question, "統括兼蓄積機能切替")
        If retc <> MsgBoxResult.Ok Then Return

        '' データベースがチェック
        If Not tClass.tClass_SQLServer.OpenCheck(TASKINI_NAME, INI_DB_A) Or Not tClass.tClass_SQLServer.OpenCheck(TASKINI_NAME, INI_DB_B) Then
            MsgBox(String.Format("{0} 及び {1} が動作中の場合のみ、実行可能です。", m_sNameJ_A_To, m_sNameJ_B_To))
            Return
        End If

        '' メールを通知
        Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
        que.inisialize()
        que.nLineNo = 0

        '''' DB同期管理に 処理依頼
        que.nEventNo = FACT_KS_EQALDB_01
        que.nLineNo = 0
        que.data(0) = 1     'B→A (通常定義と逆)
        que.data(1) = 1     'マスター同期
        Call KizuLibCLI.KizuMod.Send_Mail(KS_EQALDB, "*", que)
    End Sub

    ''' <summary>
    ''' B系→A系 実績移動
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnTO_Move_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTO_Move.Click
        '''' 確認
        Dim retc As MsgBoxResult = MsgBox("実績の移動を実施します。", _
         MsgBoxStyle.OkCancel Or MsgBoxStyle.DefaultButton2 Or MsgBoxStyle.Question, "統括兼蓄積機能切替")
        If retc <> MsgBoxResult.Ok Then Return

        '' データベースがチェック
        If Not tClass.tClass_SQLServer.OpenCheck(TASKINI_NAME, INI_DB_A) Or Not tClass.tClass_SQLServer.OpenCheck(TASKINI_NAME, INI_DB_B) Then
            MsgBox(String.Format("{0} 及び {1} が動作中の場合のみ、実行可能です。", m_sNameJ_A_To, m_sNameJ_B_To))
            Return
        End If

        '' メールを通知
        Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
        que.inisialize()
        que.nLineNo = 0

        '''' 即時実行
        que.nEventNo = FACT_KS_EQALDB_01
        que.nLineNo = 0
        que.data(0) = 1     'B→A (通常定義と逆)
        que.data(1) = 2     '実績移動
        Call KizuLibCLI.KizuMod.Send_Mail(KS_EQALDB, "*", que)
    End Sub

#End Region

#Region "表示機能"

    ''' <summary>
    ''' 表示機能 状態チェック
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CheckHy_Status()
        Dim nHyOn As Integer = -1
        Dim nHyOff As Integer = -1
        Dim strPcName As String = tMod.tMod_Common.GetNetBiosComputerName()     ' PC名称
        For ii As Integer = 0 To m_nHyOnLineNum - 1
            If strPcName = m_sName_Hy(ii) Then nHyOn = ii
        Next
        For ii As Integer = 0 To m_nHyOffLineNum - 1
            If strPcName = m_sName_Off_Hy(ii) Then nHyOff = ii
        Next
        If -1 = nHyOn And -1 = nHyOff Then
            ' オンライン／オフラインのどちらかのPCが一致するはず
            Me.lblHy_Now01.Text = "不明(操作対象外PC)"
            Me.lblHy_Now01.BackColor = Color.Tomato
            'Me.lblHy_Now02.Text = "不明(操作対象外PC)"
            'Me.lblHy_Now02.BackColor = Color.Tomato

            btnHy_Change.Text = String.Format(btnHy_Change.Text, m_sNameJ_Off_Hy(0))
            btnHy_ChangeOff.Text = String.Format(btnHy_ChangeOff.Text, m_sNameJ_Off_Hy(0), m_sKind_Off_Hy(0))

            Return
        End If


        '''' 現在の表示オフラインPCで実行の機能を把握する
        'Dim strWk As String
        'strWk = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_HY_OFF", mcINIFILE))
        'Dim nPcId_OffNow As Integer = tMod.ReadIniInt("COMMON", "PCID", strWk)

        Dim strWk As String
        Dim nPcId_OffNow(m_nHyOffLineNum - 1) As Integer       ' オフラインPCで現在動作しているPCID
        Dim nPcId_Off(m_nHyOffLineNum - 1) As Integer          ' オフラインPCのPCID
        Dim nPcId_On(m_nHyOnLineNum - 1) As Integer            ' オンラインPCのPCID
        For ii As Integer = 0 To m_nHyOffLineNum - 1
            strWk = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_HY_OFF_" & ii + 1, mcINIFILE))
            nPcId_OffNow(ii) = tMod.ReadIniInt("COMMON", "PCID", strWk)
            nPcId_Off(ii) = tMod.ReadIniInt("HY", "PCID_HY_OFF_" & ii + 1, mcINIFILE)
        Next
        For ii As Integer = 0 To m_nRpiOnLineNum - 1
            nPcId_On(ii) = tMod.ReadIniInt("HY", "PCID_HY_ON_" & ii + 1, mcINIFILE)
        Next


        '''' チェック
        '  バックアップ戻し時は、オンラインPCを特定(KizuPc.iniのPCIDから特定)
        If -1 = nHyOff Then
            For ii As Integer = 0 To m_nHyOffLineNum - 1
                If nPcId_OffNow(ii) = nPcId_On(nHyOn) Then nHyOff = ii 'オフラインPCを特定
        Next
        End If


        '''' チェック
        Dim bHy As Boolean = False
        Dim lblHy_NowOff(m_nHyOffLineNum - 1) As Label
        lblHy_NowOff(0) = lblHy_Now01
        'lblHy_NowOff(1) = lblHy_Now02

        For ii As Integer = 0 To m_nHyOffLineNum - 1
            bHy = False
            For jj As Integer = 0 To m_nRpiOnLineNum - 1
                If nPcId_OffNow(ii) = nPcId_On(jj) Then
                    lblHy_NowOff(ii).Text = m_sKind_Hy(jj)
                    lblHy_NowOff(ii).BackColor = Color.Tomato

                    bHy = True
            End If
        Next
            If Not bHy Then
                lblHy_NowOff(ii).Text = m_sKind_Off_Hy(ii)
                lblHy_NowOff(ii).BackColor = Color.DodgerBlue
            End If
        Next


        ''''' チェック
        'nHyOn = False
        'For ii As Integer = 0 To m_nHyOnLineNum - 1
        '    If nPcId_OffNow = nPcId(ii) Then
        '        lblHy_NowOff01.Text = m_sKind_Hy(ii)
        '        lblHy_NowOff01.BackColor = Color.Tomato

        '        nHyOn = True
        '    End If
        'Next
        'For ii As Integer = 0 To m_nHyOffLineNum - 1
        '    If nPcId_OffNow = nPcId(ii) Then
        '        lblHy_NowOff01.Text = m_sKind_Off_Hy(ii)
        '        lblHy_NowOff01.BackColor = Color.DodgerBlue
        '    End If
        'Next

        If nHyOff <> -1 Then
            ' ボタンの表示文字を更新
            btnHy_Change.Text = String.Format(btnRpi_Change.Text, m_sNameJ_Off_Hy(nHyOff))
            btnHy_ChangeOff.Text = String.Format(btnRpi_ChangeOff.Text, m_sNameJ_Off_Hy(nHyOff), m_sKind_Off_Hy(nHyOff))
        Else
            btnHy_Change.Text = String.Format(btnRpi_Change.Text, m_sNameJ_Off_Hy(0))
            btnHy_ChangeOff.Text = String.Format(btnRpi_ChangeOff.Text, m_sNameJ_Off_Hy(0), m_sKind_Off_Hy(0))
        End If

    End Sub

    ''' <summary>
    ''' 表示機能 切替実行(B系→A系)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnHy_ChangeOff_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnHy_ChangeOff.Click
        Call ExecHyChange(-1)
    End Sub
    ''' <summary>
    ''' 表示機能 切替実行(A系→B系)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnHy_Change_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnHy_Change.Click
        Dim nId As Integer = cmbHy_Select.SelectedIndex
        Call ExecHyChange(nId)
    End Sub

    ''' <summary>
    ''' 表示切替
    ''' </summary>
    ''' <param name="nId">-1:予備  0～:オンライン系</param>
    ''' <remarks></remarks>
    Private Sub ExecHyChange(ByVal nId As Integer)
        Dim strMsg As String = ""                       ' メッセージ用
        Dim strSQL As String
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing


        '''' 実行できるのは、表示オフラインPCのみ
        Dim nHyOn As Integer = -1
        Dim nHyOff As Integer = -1
        Dim strPcName As String = tMod.tMod_Common.GetNetBiosComputerName()     ' PC名称
        For ii As Integer = 0 To m_nHyOnLineNum - 1
            If strPcName = m_sName_Hy(ii) Then nHyOn = ii
        Next
        For ii As Integer = 0 To m_nHyOffLineNum - 1
            If strPcName = m_sName_Off_Hy(ii) Then nHyOff = ii
        Next
        If (-1 = nHyOff And -1 <> nId) Or (-1 = nHyOn And -1 = nId) Then
            'MsgBox(String.Format("本操作は、{0}、または{1}で実行してください。", m_sNameJ_Off_Hy(0), m_sNameJ_Off_Hy(1)), MsgBoxStyle.OkOnly Or MsgBoxStyle.Critical)
            MsgBox(String.Format("本操作は、{0}で実行してください。", m_sNameJ_Off_Hy(0)), MsgBoxStyle.OkOnly Or MsgBoxStyle.Critical)
            Return
        End If

         

        '''' 現在の表示オフラインPCで実行の機能を把握する
        Dim strWk As String
        Dim bRpi As Boolean = False                             ' 切替実行の可否判断(True:切替済 False:切替未)
        Dim nPcId_OffNow(m_nHyOffLineNum - 1) As Integer       ' オフラインPCで現在動作しているPCID
        Dim nPcId_Off(m_nHyOffLineNum - 1) As Integer          ' オフラインPCのPCID
        Dim nPcId_On(m_nHyOnLineNum - 1) As Integer            ' オンラインPCのPCID
        For ii As Integer = 0 To m_nHyOffLineNum - 1
            strWk = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_HY_OFF_" & ii + 1, mcINIFILE))
            nPcId_OffNow(ii) = tMod.ReadIniInt("COMMON", "PCID", strWk)
            nPcId_Off(ii) = tMod.ReadIniInt("HY", "PCID_HY_OFF_" & ii + 1, mcINIFILE)
        Next
        For ii As Integer = 0 To m_nHyOnLineNum - 1
            nPcId_On(ii) = tMod.ReadIniInt("HY", "PCID_HY_ON_" & ii + 1, mcINIFILE)
        Next
        Dim nGroupNoOff As Integer = tMod.ReadIniInt("HY", "GROUPNO_OFF_" & nHyOff + 1, mcINIFILE)
        Dim nGroupNoOn As Integer = tMod.ReadIniInt("HY", "GROUPNO_ON_" & nId + 1, mcINIFILE)


        '''' チェック(PCIDにて実施)
        '  バックアップ切替時は、オンラインPCを特定(切替先に指定したPCから特定)
        '  バックアップ戻し時は、オンラインPCを特定(KizuPc.iniのPCIDから特定)
        If -1 = nId Then        'バックアップ戻し
            If -1 <> nHyOn Then
                nHyOff = -1        'PCIDで再チェック
                For ii As Integer = 0 To m_nHyOffLineNum - 1
                    If nPcId_OffNow(ii) = nPcId_On(nHyOn) Then nHyOff = ii 'オフラインPCを特定
                Next
            End If
        Else                    'バックアップ切替
            If -1 <> nHyOff Then
                If nPcId_OffNow(nHyOff) = nPcId_Off(nHyOff) Then
                    ' バックアップ切替未実施時のみ切り替え許可
                    nHyOn = nId    'オンラインPC
                    bRpi = False
                    For ii As Integer = 0 To m_nHyOffLineNum - 1
                        If nPcId_OffNow(ii) = nPcId_On(nHyOn) Then bRpi = True
                    Next

                    Dim nGroupNoDb As Integer           ' DB取得のGroupNo
                    ' バックアップ切替対象PCが判定PCとして動作中は切替不可
                    Using cls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
                        If Not cls_DB.Open() Then
                            SetMsgBox(False, "データベースに接続できません")
                            Return
                        End If

                        '''' PCINFテーブルを書換
                        strSQL = "SELECT GroupNo FROM " & DB_PC_INF & " WHERE PCID=" & nPcId_OffNow(nHyOff)
                        sqlRead = cls_DB.SelectExecute(strSQL)
                        If sqlRead Is Nothing Then bRpi = True ' 切替不可
                        Try
                            ' データ表示
                            sqlRead.Read()

                            nGroupNoDb = sqlRead.GetValue(0)
                            If nGroupNoOff <> nGroupNoDb Then bRpi = True ' 切替不可
                        Catch ex As Exception

                        Finally

                            ' DB開放
                            If Not sqlRead Is Nothing Then sqlRead.Close()
                            Call cls_DB.Dispose()

                        End Try

                    End Using

                    If bRpi Then nHyOff = -1 ' 切替対象のオンラインPCは既に有り


                Else
                    nHyOn = -1     '切替済み
                End If
            End If
        End If

        If -1 = nHyOn Or -1 = nHyOff Then
            If -1 = nHyOn Then strMsg = String.Format("{0}は、切替済みです。バックアップ戻し実施後に実行してください。", m_sNameJ_Off_Hy(nHyOff))
            If -1 = nHyOff Then strMsg = String.Format("{0}は、切替済みです。切替対象を確認後、実行してください。", m_sNameJ_Hy(nHyOn))
            MsgBox(strMsg, MsgBoxStyle.OkOnly Or MsgBoxStyle.Critical)
            Return
        End If


        '''' どれ
        If -1 = nId Then        '予備
            strMsg = String.Format("【{0} で {1} を実行】", m_sNameJ_Off_Hy(nHyOff), m_sKind_Off_Hy(nHyOff))
        Else
            strMsg = String.Format("【{0} で {1} を実行】", m_sNameJ_Off_Hy(nHyOff), m_sKind_Hy(nId))
        End If


        '''' 確認
        Dim retc As MsgBoxResult = MsgBox("表示機能の切替を実施します。" & vbCrLf & strMsg, _
           MsgBoxStyle.OkCancel Or MsgBoxStyle.DefaultButton2 Or MsgBoxStyle.Question, "表示機能切替")
        If retc <> MsgBoxResult.Ok Then Return




        ' ======================
        ' 表示オフラインPC用KizuPc.iniを変更
        ' ======================
        'Dim strTo As String
        'Dim strFrom As String
        'strTo = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_HY_OFF", mcINIFILE))
        'If -1 = nId Then        '予備
        '    strFrom = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_FROM_OFF", mcINIFILE))
        'Else
        '    strFrom = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_FROM_" & nId + 1, mcINIFILE))
        'End If
        Dim strTo As String
        Dim strFrom As String
        strTo = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_HY_OFF_" & nHyOff + 1, mcINIFILE))
        If -1 = nId Then        '予備
            strFrom = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_FROM_OFF_" & nHyOff + 1, mcINIFILE))
        Else
            strFrom = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_FROM_" & nId + 1, mcINIFILE))
        End If

        ' KizuPc.iniを切替PC用で上書き
        IO.File.Copy(strFrom, strTo, True)

        ' 今の表示オフラインPCにも即時コピーしておく(統括のKizuPC.iniが変わっただけでは、次立ち上がりで問題があった為の処置)
        IO.File.Copy(strTo, PCINI_NAME, True)


        ' ======================
        ' PC名を変更
        ' ======================
        If -1 = nId Then        '予備
            tMod.SetNetBiosComputerName(m_sName_Off_Hy(nHyOff))
        Else
            tMod.SetNetBiosComputerName(m_sName_Hy(nId))
        End If


        ' ======================
        ' エンド処理
        ' ======================
        SetMsgBox(True, "表示機能切替が完了しました。" & vbCrLf & m_sNameJ_Off_Hy(nHyOff) & "をシャットダウンしてください。")
        retc = MsgBox("表示機能の切替えが完了しました。" & vbCrLf & m_sNameJ_Off_Hy(nHyOff) & "をシャットダウンします。",
           MsgBoxStyle.OkOnly Or MsgBoxStyle.Information, "表示機能切替")

        '''' 自分にシャットダウン通知
        Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
        que.inisialize()
        que.nLineNo = 0
        que.nEventNo = FACT_KS_SLAVE_04
        Call KizuLibCLI.KizuMod.Send_Mail(KS_SLAVE, ".", que)

    End Sub

#End Region

#Region "操作機能"

    ''' <summary>
    ''' 操作機能 状態チェック
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CheckSo_Status()
        Dim strPCNameA As String
        strPCNameA = tMod.ReadIniStr("SO", "PCNAME_A", mcINIFILE)
        Dim strPCNameB As String
        strPCNameB = tMod.ReadIniStr("SO", "PCNAME_B", mcINIFILE)
        Dim strPcName As String = tMod.tMod_Common.GetNetBiosComputerName()     ' PC名称
        If strPcName <> strPCNameA And strPcName <> strPCNameB Then
            lblSo_NowKa.Text = "不明(操作対象外PC)"
            lblSo_NowKa.BackColor = Color.Tomato
            Return
        End If

        '''' 解析PCで実行中の機能はどれ？
        Dim strPath As String = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("SO", "INI_TO", mcINIFILE))

        Dim nPcId_Now As Integer = tMod.ReadIniInt("COMMON", "PCID", strPath)
        Dim nPcId_A As Integer = tMod.ReadIniInt("SO", "PCID_A", mcINIFILE)

        '''' チェック
        If nPcId_Now = nPcId_A Then
            lblSo_NowKa.Text = "操作機能"
            lblSo_NowKa.BackColor = Color.Tomato
        Else
            lblSo_NowKa.Text = "解析機能"
            lblSo_NowKa.BackColor = Color.DodgerBlue
        End If

    End Sub

    ''' <summary>
    ''' 操作機能 切替実行
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSo_Change_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSO_ChangeSo.Click, btnSO_ChangeKa.Click
        Dim strMsg As String = ""                       ' メッセージ用

        '''' 実行できるのは、解析PCのみ
        Dim strPCNameA As String
        strPCNameA = tMod.ReadIniStr("SO", "PCNAME_A", mcINIFILE)
        Dim strPCNameB As String
        strPCNameB = tMod.ReadIniStr("SO", "PCNAME_B", mcINIFILE)
        Dim strPcName As String = tMod.tMod_Common.GetNetBiosComputerName()     ' PC名称
        If strPcName <> strPCNameA And strPcName <> strPCNameB Then
            MsgBox(String.Format("本操作は、{0}で実行してください。", m_sNameJ_B_So), MsgBoxStyle.OkOnly Or MsgBoxStyle.Critical)
            Return
        End If

        '''' どれ
        If sender Is btnSO_ChangeSo Then        'A系
            strMsg = String.Format("【{0} で 操作機能 を実行】", m_sNameJ_A_So)
        Else                                    'B系
            strMsg = String.Format("【{0} で 操作機能 を実行】", m_sNameJ_B_So)
        End If

        '''' 確認
        Dim retc As MsgBoxResult = MsgBox("操作機能の切替を実施します。" & vbCrLf & strMsg, _
           MsgBoxStyle.OkCancel Or MsgBoxStyle.DefaultButton2 Or MsgBoxStyle.Question, "操作機能切替")
        If retc <> MsgBoxResult.Ok Then Return

        ' ======================
        ' 解析PC用KizuPc.iniを変更
        ' ======================
        Dim strTo As String
        Dim strFrom As String
        strTo = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("SO", "INI_TO", mcINIFILE))
        If sender Is btnSO_ChangeSo Then        'A系
            strFrom = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("SO", "INI_FROM_B", mcINIFILE))
        Else                                    'B系
            strFrom = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("SO", "INI_FROM_A", mcINIFILE))
        End If

        ' KizuPc.iniを切替PC用で上書き
        IO.File.Copy(strFrom, strTo, True)

        ' 今の解析PCにも即時コピーしておく(統括のKizuPC.iniが変わっただけでは、次立ち上がりで問題があった為の処置)
        IO.File.Copy(strTo, PCINI_NAME, True)

        ' ======================
        ' PC名を変更
        ' ======================
        If sender Is btnSO_ChangeSo Then        'A系
            tMod.SetNetBiosComputerName(m_sName_B_So)
        Else                                    'B系
            tMod.SetNetBiosComputerName(m_sName_A_So)
        End If

        ' ======================
        ' エンド処理
        ' ======================
        SetMsgBox(True, "操作機能切替が完了しました。" & vbCrLf & m_sNameJ_B_So & "をシャットダウンしてください。")
        retc = MsgBox("操作機能の切替えが完了しました。" & vbCrLf & m_sNameJ_B_So & "をシャットダウンします。", _
           MsgBoxStyle.OkOnly Or MsgBoxStyle.Information, "操作機能切替")

        '''' 自分にシャットダウン通知
        Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
        que.inisialize()
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()
        que.nEventNo = FACT_KS_SLAVE_04
        Call KizuLibCLI.KizuMod.Send_Mail(KS_SLAVE, ".", que)

    End Sub

#End Region

#Region "全長画像処理機能"

    ''' <summary>
    ''' 全長画像処理機能 状態チェック
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CheckRpi_Status()
        '''' 実行できるのは、判定バックアップPCのみ
        ' PC名称から実施PCを特定
        '  バックアップ切替時は、オフラインPCを特定
        '  バックアップ戻し時は、オンラインPCを特定
        Dim nRpiOn As Integer = -1
        Dim nRpiOff As Integer = -1
        Dim strPcName As String = tMod.tMod_Common.GetNetBiosComputerName()     ' PC名称
        For ii As Integer = 0 To m_nRpiOnLineNum - 1
            If strPcName = m_sName_RpiOn(ii) Then nRpiOn = ii
        Next
        For ii As Integer = 0 To m_nRpiOffLineNum - 1
            If strPcName = m_sName_RpiOff(ii) Then nRpiOff = ii
        Next
        If -1 = nRpiOn And -1 = nRpiOff Then
            lblRpi_Now.Text = "不明(操作対象外PC)"
            lblRpi_Now.BackColor = Color.Tomato
            btnRpi_Change.Text = String.Format(btnRpi_Change.Text, m_sNameJ_RpiOff(0))
            btnRpi_ChangeOff.Text = String.Format(btnRpi_ChangeOff.Text, m_sNameJ_RpiOff(0), m_sKind_RpiOff(0))

            Return
        End If


        '''' 現在の全長画像オフラインPCで実行の機能を把握する
        Dim strWk As String
        Dim nPcId_OffNow(m_nRpiOffLineNum - 1) As Integer       ' オフラインPCで現在動作しているPCID
        Dim nPcId_Off(m_nRpiOffLineNum - 1) As Integer          ' オフラインPCのPCID
        Dim nPcId_On(m_nRpiOnLineNum - 1) As Integer            ' オンラインPCのPCID
        For ii As Integer = 0 To m_nRpiOffLineNum - 1
            strWk = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("RPI", "INI_RPI_OFF_" & ii + 1, mcINIFILE))
            nPcId_OffNow(ii) = tMod.ReadIniInt("COMMON", "PCID", strWk)
            nPcId_Off(ii) = tMod.ReadIniInt("RPI", "PCID_RPI_OFF_" & ii + 1, mcINIFILE)
        Next
        For ii As Integer = 0 To m_nRpiOnLineNum - 1
            nPcId_On(ii) = tMod.ReadIniInt("RPI", "PCID_RPI_ON_" & ii + 1, mcINIFILE)
        Next


        '''' チェック
        '  バックアップ戻し時は、オンラインPCを特定(KizuPc.iniのPCIDから特定)
        If -1 = nRpiOff Then
            For ii As Integer = 0 To m_nRpiOffLineNum - 1
                If nPcId_OffNow(ii) = nPcId_On(nRpiOn) Then nRpiOff = ii 'オフラインPCを特定
            Next
        End If


        '''' チェック
        Dim bRpi As Boolean = False
        Dim lblRpi_NowOff(m_nRpiOffLineNum - 1) As Label
        lblRpi_NowOff(0) = lblRpi_Now

        For ii As Integer = 0 To m_nRpiOffLineNum - 1
            bRpi = False
            For jj As Integer = 0 To m_nRpiOnLineNum - 1
                If nPcId_OffNow(ii) = nPcId_On(jj) Then
                    lblRpi_NowOff(ii).Text = m_sKind_RpiOn(jj)
                    lblRpi_NowOff(ii).BackColor = Color.Tomato

                    bRpi = True
                End If
            Next
            If Not bRpi Then
                lblRpi_NowOff(ii).Text = m_sKind_RpiOff(ii)
                lblRpi_NowOff(ii).BackColor = Color.DodgerBlue
            End If
        Next

        If nRpiOff <> -1 Then
            ' ボタンの表示文字を更新
            btnRpi_Change.Text = String.Format(btnRpi_Change.Text, m_sNameJ_RpiOff(nRpiOff))
            btnRpi_ChangeOff.Text = String.Format(btnRpi_ChangeOff.Text, m_sNameJ_RpiOff(nRpiOff), m_sKind_RpiOff(nRpiOff))
        Else
            btnRpi_Change.Text = String.Format(btnRpi_Change.Text, m_sNameJ_RpiOff(0))
            btnRpi_ChangeOff.Text = String.Format(btnRpi_ChangeOff.Text, m_sNameJ_RpiOff(0), m_sKind_RpiOff(0))
        End If

    End Sub

    ''' <summary>
    ''' 全長画像処理機能 切替実行(B系→A系)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRpi_ChangeOff_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRpi_ChangeOff.Click
        Call ExecRpiChange(sender, e, -1)
    End Sub
    ''' <summary>
    ''' 全長画像処理機能 切替実行(A系→B系)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRpi_Change_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRpi_Change.Click
        Dim nId As Integer = cmbRpi_Select.SelectedIndex
        Call ExecRpiChange(sender, e, nId)
    End Sub

    ''' <summary>
    ''' 全長画像切替
    ''' </summary>
    ''' <param name="nId">-1:予備  0～:オンライン系</param>
    ''' <remarks></remarks>
    Private Sub ExecRpiChange(ByVal sender As System.Object, ByVal e As System.EventArgs, ByVal nId As Integer)
        Dim strMsg As String = ""                       ' メッセージ用
        Dim strSQL As String
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing


        '''' 実行できるのは、判定バックアップPCのみ
        ' PC名称から実施PCを特定
        '  バックアップ切替時は、オフラインPCを特定
        '  バックアップ戻し時は、オンラインPCを特定
        Dim nRpiOn As Integer = -1
        Dim nRpiOff As Integer = -1
        Dim strPcName As String = tMod.tMod_Common.GetNetBiosComputerName()     ' PC名称
        For ii As Integer = 0 To m_nRpiOnLineNum - 1
            If strPcName = m_sName_RpiOn(ii) Then nRpiOn = ii
        Next
        For ii As Integer = 0 To m_nRpiOffLineNum - 1
            If strPcName = m_sName_RpiOff(ii) Then nRpiOff = ii
        Next
        If (-1 = nRpiOff And -1 <> nId) Or (-1 = nRpiOn And -1 = nId) Then
            MsgBox(String.Format("本操作は、{0}で実行してください。", m_sNameJ_RpiOff(0)), MsgBoxStyle.OkOnly Or MsgBoxStyle.Critical)
            Return
        End If


        '''' 現在の全長画像オフラインPCで実行の機能を把握する
        Dim strWk As String
        Dim bRpi As Boolean = False                             ' 切替実行の可否判断(True:切替済 False:切替未)
        Dim nPcId_OffNow(m_nRpiOffLineNum - 1) As Integer       ' オフラインPCで現在動作しているPCID
        Dim nPcId_Off(m_nRpiOffLineNum - 1) As Integer          ' オフラインPCのPCID
        Dim nPcId_On(m_nRpiOnLineNum - 1) As Integer            ' オンラインPCのPCID
        For ii As Integer = 0 To m_nRpiOffLineNum - 1
            strWk = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("RPI", "INI_RPI_OFF_" & ii + 1, mcINIFILE))
            nPcId_OffNow(ii) = tMod.ReadIniInt("COMMON", "PCID", strWk)
            nPcId_Off(ii) = tMod.ReadIniInt("RPI", "PCID_RPI_OFF_" & ii + 1, mcINIFILE)
        Next
        For ii As Integer = 0 To m_nRpiOnLineNum - 1
            nPcId_On(ii) = tMod.ReadIniInt("RPI", "PCID_RPI_ON_" & ii + 1, mcINIFILE)
        Next
        Dim nGroupNoOff As Integer = tMod.ReadIniInt("RPI", "GROUPNO_OFF_" & nRpiOff + 1, mcINIFILE)
        Dim nGroupNoOn As Integer = tMod.ReadIniInt("RPI", "GROUPNO_ON_" & nId + 1, mcINIFILE)


        '''' チェック(PCIDにて実施)
        '  バックアップ切替時は、オンラインPCを特定(切替先に指定したPCから特定)
        '  バックアップ戻し時は、オンラインPCを特定(KizuPc.iniのPCIDから特定)
        If -1 = nId Then        'バックアップ戻し
            If -1 <> nRpiOn Then
                nRpiOff = -1        'PCIDで再チェック
                For ii As Integer = 0 To m_nRpiOffLineNum - 1
                    If nPcId_OffNow(ii) = nPcId_On(nRpiOn) Then nRpiOff = ii 'オフラインPCを特定
                Next
            End If
        Else                    'バックアップ切替
            If -1 <> nRpiOff Then
                If nPcId_OffNow(nRpiOff) = nPcId_Off(nRpiOff) Then
                    ' バックアップ切替未実施時のみ切り替え許可
                    nRpiOn = nId    'オンラインPC
                    bRpi = False
                    For ii As Integer = 0 To m_nRpiOffLineNum - 1
                        If nPcId_OffNow(ii) = nPcId_On(nRpiOn) Then bRpi = True
                    Next

                    Dim nGroupNoDb As Integer           ' DB取得のGroupNo
                    ' バックアップ切替対象PCが判定PCとして動作中は切替不可
                    Using cls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
                        If Not cls_DB.Open() Then
                            SetMsgBox(False, "データベースに接続できません")
                            Return
                        End If

                        '''' PCINFテーブルを書換
                        strSQL = "SELECT GroupNo FROM " & DB_PC_INF & " WHERE PCID=" & nPcId_OffNow(nRpiOff)
                        sqlRead = cls_DB.SelectExecute(strSQL)
                        If sqlRead Is Nothing Then bRpi = True ' 切替不可
                        Try
                            ' データ表示
                            sqlRead.Read()

                            nGroupNoDb = sqlRead.GetValue(0)
                            If nGroupNoOff <> nGroupNoDb Then bRpi = True ' 切替不可
                        Catch ex As Exception

                        Finally

                            ' DB開放
                            If Not sqlRead Is Nothing Then sqlRead.Close()
                            Call cls_DB.Dispose()

                        End Try

                    End Using

                    If bRpi Then nRpiOff = -1 ' 切替対象のオンラインPCは既に有り


                Else
                    nRpiOn = -1     '切替済み
                End If
            End If
        End If

        If -1 = nRpiOn Or -1 = nRpiOff Then
            If -1 = nRpiOn Then strMsg = String.Format("{0}は、切替済みです。バックアップ戻し実施後に実行してください。", m_sNameJ_RpiOff(nRpiOff))
            If -1 = nRpiOff Then strMsg = String.Format("{0}は、切替済みです。切替対象を確認後、実行してください。", m_sNameJ_RpiOn(nRpiOn))
            MsgBox(strMsg, MsgBoxStyle.OkOnly Or MsgBoxStyle.Critical)
            Return
        End If



        '''' どれ
        If -1 = nId Then        '予備
            strMsg = String.Format("【{0} で {1} を実行】", m_sNameJ_RpiOff(nRpiOff), m_sKind_RpiOff(nRpiOff))
        Else
            strMsg = String.Format("【{0} で {1} を実行】", m_sNameJ_RpiOff(nRpiOff), m_sKind_RpiOn(nId))
        End If


        '''' 確認
        Dim retc As MsgBoxResult = MsgBox("全長画像機能の切替を実施します。" & vbCrLf & strMsg,
           MsgBoxStyle.OkCancel Or MsgBoxStyle.DefaultButton2 Or MsgBoxStyle.Question, "全長画像機能切替")
        If retc <> MsgBoxResult.Ok Then Return

        Dim nPcId_A As Integer = nPcId_Off(nRpiOff)     '切替対象のオフラインPCID

        Using cls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
            If Not cls_DB.Open() Then
                SetMsgBox(False, "データベースに接続できません")
                Return
            End If

            '''' PCIDを書換
            If -1 = nId Then        'バックアップ戻し
                '''' PCINFテーブルを書換
                strSQL = "UPDATE " & DB_PC_INF & " SET GroupNo=" & nGroupNoOff & " WHERE PCID=" & nPcId_A
                cls_DB.DMLExecute(strSQL)
            Else                    'バックアップ切替
                '''' PCINFテーブルを書換
                strSQL = "UPDATE " & DB_PC_INF & " SET GroupNo=" & nGroupNoOn & " WHERE PCID=" & nPcId_A
                cls_DB.DMLExecute(strSQL)
            End If

            cls_DB.Close()
        End Using



        ' ======================
        ' 全長画像オフラインPC用KizuPc.iniを変更
        ' ======================
        Dim strTo As String
        Dim strFrom As String
        strTo = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("RPI", "INI_RPI_OFF_" & nRpiOff + 1, mcINIFILE))
        If -1 = nId Then        '予備
            strFrom = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("RPI", "INI_FROM_OFF_" & nRpiOff + 1, mcINIFILE))
        Else
            strFrom = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("RPI", "INI_FROM_ON_" & nId + 1, mcINIFILE))
        End If

        ' KizuPc.iniを切替PC用で上書き
        IO.File.Copy(strFrom, strTo, True)

        ' 今の全長画像オフラインPCにも即時コピーしておく(統括のKizuPC.iniが変わっただけでは、次立ち上がりで問題があった為の処置)
        IO.File.Copy(strTo, PCINI_NAME, True)


        ' ======================
        ' PC名を変更
        ' ======================
        If -1 = nId Then        '予備
            tMod.SetNetBiosComputerName(m_sName_RpiOff(nRpiOff))
        Else
            tMod.SetNetBiosComputerName(m_sName_RpiOn(nId))
        End If


        ' ======================
        ' エンド処理
        ' ======================
        SetMsgBox(True, "全長画像機能切替が完了しました。" & vbCrLf & m_sNameJ_RpiOff(nRpiOff) & "をシャットダウンしてください。")
        retc = MsgBox("全長画像機能の切替えが完了しました。" & vbCrLf & m_sNameJ_RpiOff(nRpiOff) & "をシャットダウンします。",
           MsgBoxStyle.OkOnly Or MsgBoxStyle.Information, "全長画像機能切替")

        '''' 自分にシャットダウン通知
        Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
        que.inisialize()
        que.nLineNo = 0
        que.nEventNo = FACT_KS_SLAVE_04
        Call KizuLibCLI.KizuMod.Send_Mail(KS_SLAVE, ".", que)


        ' =======================================================
        ' バックアップPCで実行するKizuTask.iniの切替
        ' =======================================================
        Dim strPath As String
        '' 自分の中のKizuTask.iniを切り替える
        strPath = tMod.ReadIniStr("TO", "INIPC_FPATH", mcINIFILE)

        ' ======================
        ' バックアップ機のIPアドレス変更
        ' ======================
        ' どうもIPアドレスが変わるとUDPでもメールが飛ばないので、この位置に移動したよ
        Dim jj As Integer = 1
        While True
            Dim sAdapterNameas As String = ""
            Dim sIpAddress As String = ""
            sAdapterNameas = tMod.ReadIniStr("RPI", "IP_ADAPTER_" & jj, mcINIFILE)
            If sAdapterNameas = "" Then Exit While

            '''' IPセット
            ' IP取得
            If sender Is btnRpi_ChangeOff Then
                sIpAddress = tMod.ReadIniStr("RPI", "IP_OFF_" & nRpiOff + 1 & "_" & jj & "_1", mcINIFILE)
            Else
                sIpAddress = tMod.ReadIniStr("RPI", "IP_ON_" & nRpiOn + 1 & "_" & jj & "_1", mcINIFILE)
            End If
            If sIpAddress = "" Then Exit While

            tMod.SetIpAddress(sIpAddress, sAdapterNameas)

            Dim kk As Integer = 2
            While True
                sIpAddress = ""

                '''' IP追加
                ' IP取得
                If sender Is btnRpi_ChangeOff Then
                    sIpAddress = tMod.ReadIniStr("RPI", "IP_OFF_" & nRpiOff + 1 & "_" & jj & "_" & kk, mcINIFILE)
                Else
                    sIpAddress = tMod.ReadIniStr("RPI", "IP_ON_" & nRpiOn + 1 & "_" & jj & "_" & kk, mcINIFILE)
                End If
                If sIpAddress = "" Then Exit While

                AddIpAddress(sIpAddress, sAdapterNameas)

                kk += 1
            End While
            jj += 1
        End While


    End Sub

#End Region


    ''' <summary>
    ''' メッセージボックスにメッセージ書き込み
    ''' </summary>
    ''' <param name="bln">メッセージ色[True:黒/False:赤]</param>
    ''' <param name="strMsg">メッセージ内容</param>
    ''' <remarks></remarks>
    Private Sub SetMsgBox(ByVal bln As Boolean, ByVal strMsg As String)
        lblMsgBox.Text = strMsg
        If bln Then
            lblMsgBox.ForeColor = Color.Black
        Else
            lblMsgBox.ForeColor = Color.Red
        End If
        System.Windows.Forms.Application.DoEvents()
    End Sub

    ''' <summary>
    ''' IPアドレスを追加する
    ''' </summary>
    ''' <param name="Ip">変更を行うIPアドレス (127.0.0.1形式)</param>
    ''' <param name="AdapterName">変更対象のアダプタ名称 (ネットワーク接続プロパティで表示されている名称を指定)</param>
    ''' <param name="SubNetMask">変更を行うサブネットマスク</param>
    Public Sub AddIpAddress(ByVal Ip As String, Optional ByVal AdapterName As String = "ローカル エリア接続", Optional ByVal SubNetMask As String = "255.255.255.0")
        Dim strCmd As String = ""        ' 生成するDOSコマンド

        '''' コマンド生成
        strCmd = "netsh interface ip add "
        strCmd &= "address name = """ & AdapterName & """ "         '対象の奴
        strCmd &= "addr=" & Ip & " "
        strCmd &= "mask=" & SubNetMask & ""

        '''' コマンド実行
        Shell(strCmd, AppWinStyle.Hide)
    End Sub

    ''' <summary>
    ''' IPアドレスを削除する
    ''' </summary>
    ''' <param name="Ip">変更を行うIPアドレス (127.0.0.1形式)</param>
    ''' <param name="AdapterName">変更対象のアダプタ名称 (ネットワーク接続プロパティで表示されている名称を指定)</param>
    Public Sub DeleteIpAddress(ByVal Ip As String, Optional ByVal AdapterName As String = "ローカル エリア接続")
        Dim strCmd As String = ""        ' 生成するDOSコマンド

        '''' コマンド生成
        strCmd = "netsh interface ip delete "
        strCmd &= "address name = """ & AdapterName & """ "         '対象の奴
        strCmd &= "addr=" & Ip & " "

        '''' コマンド実行
        Shell(strCmd, AppWinStyle.Hide)
    End Sub

    ''' <summary>
    ''' 画面 閉じようとした
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMenu_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        ' メインスレッド終了通知
        modMain.TaskEnd()
    End Sub

End Class
