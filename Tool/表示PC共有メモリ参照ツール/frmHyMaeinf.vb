Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Runtime.InteropServices

Public Class frmHyMaeinf
    '///////////////////////////////////////////////////////
    'メンバー変数
    '///////////////////////////////////////////////////////
    Private m_typHyMaeInf As TBL_HY_MAE_INF
    Private m_typMaeInf As MAE_INF
    Private m_num As Integer ' 現在の格納件数

    Private WithEvents m_timer1 As New Timer    ' 表示自動更新用タイマー
    Private WithEvents m_timer2 As New Timer    ' 共有メモリアクセス可・不可確認タイマー

    Private m_rec As Integer = 0                ' レコード
    Private m_nMen As Integer = 0               ' 表裏
    Private m_nPnt As Integer = 0               ' ポインタ

    Public Sub New()

        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        m_rec = 0
        m_nMen = 0
        m_nPnt = 1
    End Sub

    Public Sub New(ByVal rec As Integer, ByVal men As Integer, ByVal pnt As Integer, ByVal spm As Integer)
        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        m_rec = rec
        m_nMen = men
        m_nPnt = pnt
    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmHyDefectInf_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ' 表示自動更新タイマー周期設定
        m_timer1.Interval = 100         ' 100msec

        ' 共有メモリにアクセス可能かチェックタイマー
        m_timer2.Interval = 100         ' 100msec
        m_timer2.Start()

        ' ポインター nud 初期値セット
        nudPoint.Maximum = CInt(COIL_LEN_MAX / MAP_REPRES_MIN)
        nudPoint.Minimum = 1
        nudPoint.Value = m_nPnt

        'レコードコンボボックス初期化
        cmbRecord.Items.Clear()
        For ii As Integer = 0 To TBL_HY_REC_MAX - 1
            cmbRecord.Items.Add(CStr(ii + 1))
        Next
        cmbRecord.SelectedIndex = m_rec
        '面
        cmbMen.Items.Add("表")
        cmbMen.Items.Add("裏")
        cmbMen.SelectedIndex = m_nMen

        ' dgvの初期化
		'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
		'' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
		''InitDgv(dgvFF, 13)
		'InitDgv(dgvFF, 14)
		'' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
		InitDgv(dgvFF, 15)
		'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
        InitDgv(dgvMaeInf, 21)
        InitDgvPath(dgvPath, 4)

        ' 表示内容クリア
        DispInit()

        ' 共有メモリの全体を読み込む
        If ReadFromShmemAll() Then
            ' 共有メモリの内容を表示
            DispShmem()
        End If

        ' 自動更新を有効にする・・・必要ない場合はコメントにする
        chkUpdate.Checked = True

        ' 共有メモリサイズ情報表示
        Dim str As String = ""
        str = String.Format("TBL_HY_MAE_INF Size = {0}", Marshal.SizeOf(GetType(TBL_HY_MAE_INF))) & vbCrLf
        str &= String.Format("MAE_INF Size = {0}", Marshal.SizeOf(GetType(MAE_INF))) & vbCrLf
        str &= String.Format("DSP_TYP_MAE_FFINF Size = {0}", Marshal.SizeOf(GetType(DSP_TYP_MAE_FFINF))) & vbCrLf
        str &= String.Format("DSP_TYP_MAE_KIZUINF Size = {0}", Marshal.SizeOf(GetType(DSP_TYP_MAE_KIZUINF))) & vbCrLf
        str &= String.Format("DSP_TYP_MAE_DEFECTINF Size = {0}", Marshal.SizeOf(GetType(DSP_TYP_MAE_DEFECTINF))) & vbCrLf
        txtInfo.Text = str
    End Sub


    ''' <summary>
    ''' 自動更新処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_timer1_tick(ByVal sender As Object, ByVal e As EventArgs) Handles m_timer1.Tick
        ' 共有メモリの全体を読み込む
        If ReadFromShmem() Then
            ' 共有メモリの内容を表示
            DispShmem()
        End If
    End Sub

    ''' <summary>
    ''' 共有メモリアクセス可能かチェック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_timer2_tick(ByVal sender As Object, ByVal e As EventArgs) Handles m_timer2.Tick
        ' 対象の共有メモリを定期的に読み込みしてみて、アクセスできるかどうか確認する。
        ' あまり大きな共有メモリを読み込むと負荷になるので注意
        Dim nRetc As ValueType = 0

        Dim nTopByte As Integer
        nTopByte = Marshal.SizeOf(GetType(REC)) * 0 + Marshal.SizeOf(GetType(MEN)) * 0
        Dim tmp As Integer = CType(ksMemRead(TBL_HY_MAE_INF_NAME, nTopByte, GetType(Integer), nRetc), Integer)
        If CBool(nRetc) Then
            ' 共有メモリにアクセス可能
            DspShmemStatus(True)
            ' 自動更新がOFFになっている場合は 読み込み・書き込みボタンを有効にする
            If Not chkUpdate.Checked Then
                btnRead.Enabled = True
                btnWrite.Enabled = True
            End If
        Else
            ' 共有メモリにアクセスできない
            DspShmemStatus(False)
            ' 読み込み・書き込みボタンを無効にする
            btnRead.Enabled = False
            btnWrite.Enabled = False
        End If
    End Sub

    ''' <summary>
    ''' DataGridView初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="RowCount"></param>
    ''' <remarks></remarks>
    Private Sub InitDgv(ByVal dgv As tCnt_DataGridView, ByVal RowCount As Integer)
        '基本設定
        dgv.dgvInitialize(dgv)

        With dgv
            'スクロールバーの表示
            .ScrollBars = ScrollBars.Both

            .SelectionMode = DataGridViewSelectionMode.FullRowSelect

            '選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.Transparent
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '行設定
            .RowTemplate.Height = 16
            .RowCount = RowCount

            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue         ' 選択
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1行おきに色替え

            '列設定
            .ColumnCount = 3
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20
            For ii As Integer = 0 To .ColumnCount - 1
                .Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                .Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(ii).HeaderText = ""
                Select Case ii
                    Case 0
                        .Columns(ii).Width = 180
                    Case 1
                        .Columns(ii).Width = 130
                    Case 2
                        .Columns(ii).Width = 120
                End Select
            Next ii
            .Columns(0).ReadOnly = True
            .Columns(0).Frozen = True
            .Columns(2).ReadOnly = True
            .Columns(2).Visible = False

            .Columns(0).HeaderText = "項目"
            .Columns(1).HeaderText = "値"
            .Columns(2).HeaderText = "変数名"

        End With

    End Sub

    ''' <summary>
    ''' Path用DataGridView初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="RowCount"></param>
    ''' <remarks></remarks>
    Private Sub InitDgvPath(ByVal dgv As tCnt_DataGridView, ByVal RowCount As Integer)
        '基本設定
        dgv.dgvInitialize(dgv)

        With dgv
            'スクロールバーの表示
            .ScrollBars = ScrollBars.Both

            .SelectionMode = DataGridViewSelectionMode.FullRowSelect

            '選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.Transparent
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '行設定
            .RowTemplate.Height = 16
            .RowCount = RowCount

            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue         ' 選択
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1行おきに色替え

            '列設定
            .ColumnCount = 3
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20
            For ii As Integer = 0 To .ColumnCount - 1
                .Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                .Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(ii).HeaderText = ""
                Select Case ii
                    Case 0
                        .Columns(ii).Width = 100
                    Case 1
                        .Columns(ii).Width = 464
                    Case 2
                        .Columns(ii).Width = 120
                End Select
            Next ii
            .Columns(0).ReadOnly = True
            .Columns(0).Frozen = True
            .Columns(2).ReadOnly = True
            .Columns(2).Visible = False

            .Columns(0).HeaderText = "項目"
            .Columns(1).HeaderText = "値"
            .Columns(2).HeaderText = "変数名"

        End With

    End Sub

    ''' <summary>
    ''' 自動更新のチェックボックスが変化した時の処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkUpdate_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkUpdate.CheckedChanged
        Dim chk As CheckBox = CType(sender, CheckBox)

        If chk.Checked Then
            ' チェックされた時
            chk.BackColor = Color.Cyan
            m_timer1.Start()

            ' 読み込み・書き込みボタンを無効にする
            btnRead.Enabled = False
            btnWrite.Enabled = False

        Else
            ' チェックが解除された時
            chk.BackColor = SystemColors.Control
            m_timer1.Stop()

            If txtTitle.BackColor <> Color.Red Then
                ' 共有メモリアクセス可能な場合
                btnRead.Enabled = True
                btnWrite.Enabled = True
            End If
        End If
    End Sub

    ''' <summary>
    ''' コンボボックス等変更時
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub param_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbRecord.SelectedIndexChanged, cmbMen.SelectedIndexChanged, nudPoint.ValueChanged
        ' 読み込み処理
        btnRead.PerformClick()
    End Sub

    ''' <summary>
    ''' 共有メモリが読み込める・読み込めないをタイトルの背景カラーで表す
    ''' </summary>
    ''' <param name="flg">true:読込可 False:読込不可</param>
    ''' <remarks>読込可:LimeGreen 読込不可:Red</remarks>
    Private Sub DspShmemStatus(ByVal flg As Boolean)
        If flg Then
            ' 読み込み可能
            txtTitle.BackColor = Color.LimeGreen
        Else
            txtTitle.BackColor = Color.Red
        End If
    End Sub

    ''' <summary>
    ''' 読み込みボタン
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
        ' 共有メモリの全体を読み込む
        If ReadFromShmem() Then
            ' 共有メモリの内容を表示
            DispShmem()
        End If

    End Sub

    ''' <summary>
    ''' 書き込みボタン
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnWrite_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnWrite.Click

        ' レコード・ポイント取得
        Dim rec As Integer = cmbRecord.SelectedIndex
        Dim men As Integer = cmbMen.SelectedIndex
        Dim pnt As Integer = CInt(nudPoint.Value) - 1

        ' 共有メモリの全体を読み込む
        If ReadFromShmemAll() Then
            ReadFromDgv()

            m_typHyMaeInf.rec(rec).men(men).num = m_num
            m_typHyMaeInf.rec(rec).men(men).maeinf(pnt) = m_typMaeInf

            ' 共有メモリに書き込む
            WriteToShmem()

            ' 共有メモリの一部を読み込む
            If ReadFromShmem() Then
                ' 共有メモリの内容を表示
                DispShmem()
            Else
                ' 初期状態を表示
                ' DispInit()
            End If

        End If

    End Sub


    ''' <summary>
    ''' 共有メモリから読み込み
    ''' </summary>
    ''' <returns>true:読み込み成功 False:読み込み失敗</returns>
    ''' <remarks>共有メモリ⇒m_typHyCoilInf</remarks>
    Private Function ReadFromShmem() As Boolean
        Dim bRet As Boolean = False
        Dim nRetc As ValueType = 0

        ' 表示するコイルレコード・ポイント取得
        Dim rec As Integer = cmbRecord.SelectedIndex
        If rec < 0 Then rec = 0
        Dim men As Integer = cmbMen.SelectedIndex
        If men < 0 Then men = 0
        Dim pnt As Integer = CInt(nudPoint.Value) - 1

        ' 読み込む位置の計算
        Dim nTopByte As Integer

        Try
            ' 共有メモリ ヘッダ取得位置計算
            nTopByte = Marshal.SizeOf(GetType(REC)) * rec + Marshal.SizeOf(GetType(MEN)) * men
            m_num = CType(ksMemRead(TBL_HY_MAE_INF_NAME, nTopByte, GetType(Integer), nRetc), Integer)
            If Not CBool(nRetc) Then
                ' 読み込み失敗
                DspShmemStatus(False)
                Exit Try
            End If

            ' 共有メモリ ポイント取得位置計算
            nTopByte = Marshal.SizeOf(GetType(REC)) * rec + Marshal.SizeOf(GetType(MEN)) * men +
                Marshal.SizeOf(GetType(Integer)) + Marshal.SizeOf(GetType(MAE_INF)) * pnt
            m_typMaeInf = CType(ksMemRead(TBL_HY_MAE_INF_NAME, nTopByte, GetType(MAE_INF), nRetc), MAE_INF)
            If Not CBool(nRetc) Then
                ' 読み込み失敗
                DspShmemStatus(False)
                Exit Try
            End If

            bRet = True
            DspShmemStatus(True)

        Catch ex As Exception
            ' 共有メモリ読み込み失敗
            DspShmemStatus(False)
            Dim str As String = txtInfo.Text
            str &= String.Format("ReadFromShmem Error [{0}]", ex.Message)
            txtInfo.Text = str
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' 共有メモリ全体を読み込む
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function ReadFromShmemAll() As Boolean
        Dim bRet As Boolean = False

        Try
            m_typHyMaeInf = CType(ksMemRead(TBL_HY_MAE_INF_NAME, GetType(TBL_HY_MAE_INF)), TBL_HY_MAE_INF)

            If m_typHyMaeInf.rec Is Nothing Then
                ' 共有メモリの読み込みに失敗している場合は rec が nothing になる
                bRet = False
                DspShmemStatus(False)
            Else
                bRet = True
                DspShmemStatus(True)
            End If
        Catch ex As Exception
            ' 共有メモリ読み込み失敗
            DspShmemStatus(False)
            Dim str As String = txtInfo.Text
            str &= String.Format("ReadFromShmemAll Error [{0}]", ex.Message)
            txtInfo.Text = str
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 共有メモリに書き込み
    ''' </summary>
    ''' <returns>true:書き込み成功 false:書き込み失敗</returns>
    ''' <remarks>m_typHyCoilInf⇒共有メモリ</remarks>
    Private Function WriteToShmem() As Boolean
        Dim bRet As Boolean = False

        Try
            If Not ksMemWrite(TBL_HY_MAE_INF_NAME, m_typHyMaeInf) Then
                bRet = False
                DspShmemStatus(False)
            Else
                bRet = True
                DspShmemStatus(True)
            End If
        Catch ex As Exception
            bRet = False
            DspShmemStatus(False)
            Dim str As String = txtInfo.Text
            str &= String.Format("WriteToShmem Error [{0}]", ex.Message)
            txtInfo.Text = str
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' FF情報を表示・読み込み
    ''' </summary>
    ''' <param name="flg">true:表示 false:読込</param>	
    ''' <remarks></remarks>
    Private Sub FFInf(ByVal init As Boolean, ByVal flg As Boolean)

        ' 対象のdgvをセット
        Dim dgv As tCnt_DataGridView = dgvFF

        With m_typMaeInf.ff
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    Select Case ii
                        Case 0
                            If init Then dgv(0, ii).Value = "FF管理No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod_Common.ByteToString(.cMaeKizukenNo) _
                            Else tMod_Common.StringToByte(.cMaeKizukenNo, CStr(dgv(1, ii).Value), .cMaeKizukenNo.Length)
                        Case 1
                            If init Then dgv(0, ii).Value = "FF疵No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nMaeKizuNo.ToString _
                            Else .nMaeKizuNo = CInt(dgv(1, ii).Value)
                        Case 2
                            If init Then dgv(0, ii).Value = "FF疵種" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod_Common.ByteToString(.cT) _
                            Else tMod_Common.StringToByte(.cT, CStr(dgv(1, ii).Value), .cT.Length)
                        Case 3
                            If init Then dgv(0, ii).Value = "FFグレード" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod_Common.ByteToString(.cG) _
                            Else tMod_Common.StringToByte(.cG, CStr(dgv(1, ii).Value), .cG.Length)
                        Case 4
                            If init Then dgv(0, ii).Value = "FF疵検出工程" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod_Common.ByteToString(.cProc) _
                            Else tMod_Common.StringToByte(.cProc, CStr(dgv(1, ii).Value), .cProc.Length)
                        Case 5
                            If init Then dgv(0, ii).Value = "予備" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod_Common.ByteToString(.yobi1) _
                            Else tMod_Common.StringToByte(.yobi1, CStr(dgv(1, ii).Value), .yobi1.Length)
                        Case 6
                            If init Then dgv(0, ii).Value = "FF疵区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nDiv.ToString _
                            Else .nDiv = CInt(dgv(1, ii).Value)
                        Case 7
                            If init Then dgv(0, ii).Value = "FF疵長さ方向位置" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nPos.ToString _
                            Else .nPos = CInt(dgv(1, ii).Value)
                        Case 8
                            If init Then dgv(0, ii).Value = "FF疵巾方向位置" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nWidth.ToString _
                            Else .nWidth = CInt(dgv(1, ii).Value)
                        Case 9
                            If init Then dgv(0, ii).Value = "FF疵長さ" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nLen.ToString _
                            Else .nLen = CInt(dgv(1, ii).Value)
                        Case 10
                            If init Then dgv(0, ii).Value = "最終長手位置" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nY.ToString _
                            Else .nY = CInt(dgv(1, ii).Value)
                        Case 11
                            If init Then dgv(0, ii).Value = "MAE_TYPE_NAMEのID" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nTid.ToString _
                            Else .nTid = CShort(dgv(1, ii).Value)
                        Case 12
                            If init Then dgv(0, ii).Value = "MAE_GRADE_NAMEのID" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nGid.ToString _
                            Else .nGid = CShort(dgv(1, ii).Value)
                            ' <20221030><溝口><2TCM実機化に伴う下工程の改造> --->>>
                        Case 13
                            If init Then dgv(0, ii).Value = "工程種別" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nKouteiID.ToString _
                            Else .nKouteiID = CShort(dgv(1, ii).Value)
                            ' <20221030><溝口><2TCM実機化に伴う下工程の改造> <<<---

							'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
						Case 14
							If init Then dgv(0, ii).Value = "最終出側長手位置" : dgv(1, ii).Value = "" _
							Else If flg Then dgv(1, ii).Value = .nY_Out.ToString _
							Else .nY_Out = CInt(dgv(1, ii).Value)
							'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
                    End Select

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("FF情報{0} エラー idx[{1}] Error [{2}]", CStr(IIf(flg, "表示", "読込")), ii, ex.Message)
                    txtInfo.Text = str
                End Try
            Next
            dgv.ClearSelection()
        End With
    End Sub

    ''' <summary>
    ''' 前工程疵情報を表示・読み込み
    ''' </summary>
    ''' <param name="flg">true:表示 false:読込</param>	
    ''' <remarks></remarks>
    Private Sub MaeInf(ByVal init As Boolean, ByVal flg As Boolean)

        ' 対象のdgvをセット
        Dim dgv As tCnt_DataGridView = dgvMaeInf

        With m_typMaeInf.inf.data
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    Select Case ii
                        Case 0
                            If init Then dgv(0, ii).Value = "画像形式" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nImageFlg.ToString _
                            Else .nImageFlg = CShort(dgv(1, ii).Value)
                        Case 1
                            If init Then dgv(0, ii).Value = "事前編集フラグ" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nDspMode.ToString _
                            Else .nDspMode = CShort(dgv(1, ii).Value)
                        Case 2
                            If init Then dgv(0, ii).Value = "表裏区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .men.ToString _
                            Else .men = CUShort(dgv(1, ii).Value)
                        Case 3
                            If init Then dgv(0, ii).Value = "周期疵区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cycle_div.ToString _
                            Else .cycle_div = CUShort(dgv(1, ii).Value)
                        Case 4
                            If init Then dgv(0, ii).Value = "DSWS区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .dsws.ToString _
                            Else .dsws = CUShort(dgv(1, ii).Value)
                        Case 5
                            If init Then dgv(0, ii).Value = "予備" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .yobi.ToString _
                            Else .yobi = CUShort(dgv(1, ii).Value)
                        Case 6
                            If init Then dgv(0, ii).Value = "速度[mpm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .speed.ToString _
                            Else .speed = CInt(dgv(1, ii).Value)
                        Case 7
                            If init Then dgv(0, ii).Value = "ピッチ[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .pitch.ToString _
                            Else .pitch = CInt(dgv(1, ii).Value)
                        Case 8
                            If init Then dgv(0, ii).Value = "ロール径[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .roolkei.ToString _
                            Else .roolkei = CSng(dgv(1, ii).Value)
                        Case 9
                            If init Then dgv(0, ii).Value = "疵幅[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .box_wid.ToString _
                            Else .box_wid = CShort(dgv(1, ii).Value)
                        Case 10
                            If init Then dgv(0, ii).Value = "疵長さ[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .box_len.ToString _
                            Else .box_len = CShort(dgv(1, ii).Value)
                        Case 11
                            If init Then dgv(0, ii).Value = "縦分解能[mm/pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .h_res.ToString _
                            Else .h_res = CSng(dgv(1, ii).Value)
                        Case 12
                            If init Then dgv(0, ii).Value = "横分解能[mm/pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .w_res.ToString _
                            Else .w_res = CSng(dgv(1, ii).Value)
                        Case 13
                            If init Then dgv(0, ii).Value = "外接Xmin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .box_x_min.ToString _
                            Else .box_x_min = CShort(dgv(1, ii).Value)
                        Case 14
                            If init Then dgv(0, ii).Value = "外接Xmax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .box_x_max.ToString _
                            Else .box_x_max = CShort(dgv(1, ii).Value)
                        Case 15
                            If init Then dgv(0, ii).Value = "外接Ymin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .box_y_min.ToString _
                            Else .box_y_min = CShort(dgv(1, ii).Value)
                        Case 16
                            If init Then dgv(0, ii).Value = "外接Ymax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .box_y_max.ToString _
                            Else .box_y_max = CShort(dgv(1, ii).Value)
                        Case 17
                            If init Then dgv(0, ii).Value = "疵中心位置X[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .center_x.ToString _
                            Else .center_x = CShort(dgv(1, ii).Value)
                        Case 18
                            If init Then dgv(0, ii).Value = "疵中心位置Y[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .center_y.ToString _
                            Else .center_y = CShort(dgv(1, ii).Value)
                        Case 19
                            If init Then dgv(0, ii).Value = "縦画素数[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .h_size.ToString _
                            Else .h_size = CUShort(dgv(1, ii).Value)
                        Case 20
                            If init Then dgv(0, ii).Value = "横画素数[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .w_size.ToString _
                            Else .w_size = CUShort(dgv(1, ii).Value)
                            'Case 21
                            '    If init Then dgv(0, ii).Value = "最終長手位置[mm]" : dgv(1, ii).Value = "" _
                            '    Else If flg Then dgv(1, ii).Value = .nY.ToString _
                            '    Else .nY = CInt(dgv(1, ii).Value)
                            'Case 22
                            '    If init Then dgv(0, ii).Value = "MAE_TYPE_NAMEのID" : dgv(1, ii).Value = "" _
                            '    Else If flg Then dgv(1, ii).Value = .nTid.ToString _
                            '    Else .nTid = CShort(dgv(1, ii).Value)
                            'Case 23
                            '    If init Then dgv(0, ii).Value = "MAE_GRADE_NAMEのID" : dgv(1, ii).Value = "" _
                            '    Else If flg Then dgv(1, ii).Value = .nGid.ToString _
                            '    Else .nGid = CShort(dgv(1, ii).Value)
                    End Select

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("前工程疵情報{0} エラー idx[{1}] Error [{2}]", CStr(IIf(flg, "表示", "読込")), ii, ex.Message)
                    txtInfo.Text = str
                End Try
            Next
            dgv.ClearSelection()
        End With
    End Sub


    ''' <summary>
    ''' 画像パスを表示・読み込み
    ''' </summary>
    ''' <param name="flg">true:表示 false:読込</param>	
    ''' <remarks></remarks>
    Private Sub path(ByVal init As Boolean, ByVal flg As Boolean)

        ' 対象のdgvをセット
        Dim dgv As tCnt_DataGridView = dgvPath

        With m_typMaeInf.inf
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    Select Case ii
                        Case 0  ' 画像のパス1 
                            If init Then dgv(0, ii).Value = "画像のパス1" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .image_path_1 _
                            Else .image_path_1 = CStr(dgv(1, ii).Value)
                        Case 1  ' 画像のパス2
                            If init Then dgv(0, ii).Value = "画像のパス2" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .image_path_2 _
                            Else .image_path_2 = CStr(dgv(1, ii).Value)
                        Case 2  ' 画像のパス3
                            If init Then dgv(0, ii).Value = "画像のパス3" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .image_path_3 _
                            Else .image_path_3 = CStr(dgv(1, ii).Value)
                        Case 3  ' 画像のパス4
                            If init Then dgv(0, ii).Value = "画像のパス4" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .image_path_4 _
                            Else .image_path_4 = CStr(dgv(1, ii).Value)
                    End Select

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("画像パス{0} エラー idx[{1}] Error [{2}]", CStr(IIf(flg, "表示", "読込")), ii, ex.Message)
                    txtInfo.Text = str

                End Try
            Next
            dgv.ClearSelection()
        End With
    End Sub

    ''' <summary>
    ''' 表示を初期化する
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispInit()
        txtNum.Text = ""
        txtMaeDsp.Text = ""

        FFInf(True, True)
        MaeInf(True, True)
        path(True, True)
    End Sub

    ''' <summary>
    ''' dgvからデータを読み込む
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadFromDgv()
        FFInf(False, False)
        MaeInf(False, False)
        path(False, False)

        If IsNumeric(txtNum.Text) Then
            m_num = CInt(txtNum.Text)
        End If

        If IsNumeric(txtMaeDsp.Text) Then
            m_typMaeInf.nMaeDsp = CInt(txtMaeDsp.Text)
        End If
    End Sub

    ''' <summary>
    ''' 共有メモリの内容を表示する
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispShmem()
        txtNum.Text = m_num.ToString
        txtMaeDsp.Text = m_typMaeInf.nMaeDsp.ToString

        FFInf(False, True)
        MaeInf(False, True)
        path(False, True)
    End Sub

    Private Sub frmHyMaeinf_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        m_timer1.Stop()
        m_timer1.Dispose()
    End Sub
End Class