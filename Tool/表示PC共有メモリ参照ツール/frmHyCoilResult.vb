Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Runtime.InteropServices

Public Class frmHyCoilResult
    '///////////////////////////////////////////////////////
    'メンバー変数
    '///////////////////////////////////////////////////////
    Private m_typHyCoilResult As TBL_HY_COILRESULT
    Private WithEvents m_timer1 As New Timer    ' 表示自動更新用タイマー
    Private WithEvents m_timer2 As New Timer    ' 共有メモリアクセス可・不可確認タイマー

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmHyCoilOutInf_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ' 表示自動更新タイマー周期設定
        m_timer1.Interval = 100         ' 100msec

        ' 共有メモリにアクセス可能かチェックタイマー
        m_timer2.Interval = 100         ' 100msec
        m_timer2.Start()

        'レコードコンボボックス初期化
        cmbRecord.Items.Clear()
        For ii As Integer = 0 To TBL_HY_COIL_MAX - 1
            cmbRecord.Items.Add(CStr(ii + 1))
        Next
        cmbRecord.SelectedIndex = 0

        ' dgvの初期化
        InitDgv(dgvCoilResult, 4)                       ' コイル実績
        InitDgv_CutInf(dgvCutInf, MAX_SHACUT_COUNT)     ' カット情報

        ' 表示内容クリア
        DispInit()

        ' 共有メモリの全体を読み込む
        If ReadFromShmem() Then
            ' 共有メモリの内容を表示
            DispShmem()
        End If

        ' 自動更新を有効にする・・・必要ない場合はコメントにする
        chkUpdate.Checked = True

        ' 共有メモリサイズ情報表示
        Dim str As String = ""
        str = String.Format("TO_SO_DATA_CUT Size = {0}", Marshal.SizeOf(GetType(TO_SO_DATA_CUT))) & vbCrLf
        str &= String.Format("COILRESULT_REC Size = {0}", Marshal.SizeOf(GetType(COILRESULT_REC))) & vbCrLf
        str &= String.Format("TBL_HY_COILRESULT Size = {0}", Marshal.SizeOf(GetType(TBL_HY_COILRESULT))) & vbCrLf

        txtInfo.Text = str

        txtTitle.Select()

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
        Dim tmp As COINFO_BASE = CType(ksMemRead(TBL_HY_COILINF_NAME, 0, GetType(COINFO_BASE), nRetc), COINFO_BASE)
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

        If m_typHyCoilResult.rec Is Nothing Then
            Exit Sub
        End If

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
            .RowCount = RowCount
            .RowTemplate.Height = 20

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
                        .Columns(ii).Width = 160
                    Case 1
                        .Columns(ii).Width = 140
                    Case 2
                        .Columns(ii).Width = 140
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
    ''' DataGridView初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="RowCount"></param>
    ''' <remarks></remarks>
    Private Sub InitDgv_CutInf(ByVal dgv As tCnt_DataGridView, ByVal RowCount As Integer)

        If m_typHyCoilResult.rec Is Nothing Then
            Exit Sub
        End If

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
            .RowCount = RowCount
            .RowTemplate.Height = 20

            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue         ' 選択
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1行おきに色替え

            '列設定
            .ColumnCount = 2
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
                        .Columns(ii).Width = 50
                    Case 1
                        .Columns(ii).Width = 280
                End Select
            Next ii

            .Columns(0).HeaderText = "区分"
            .Columns(1).HeaderText = "位置"

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
    ''' レコード コンボボックス変更時
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbRecord_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbRecord.SelectedIndexChanged
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

        ' 共有メモリの全体を読み込む
        If ReadFromShmem() Then
            ' dgvからデータを読み込む
            ReadFromDgv()

            ' 共有メモリに書き込む
            WriteToShmem()

            ' 共有メモリの全体を読み込む
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

        Try
            m_typHyCoilResult = CType(ksMemRead(TBL_HY_COILRESULT_NAME, GetType(TBL_HY_COILRESULT)), TBL_HY_COILRESULT)

            If m_typHyCoilResult.rec Is Nothing Then
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
            str &= String.Format("ReadFromShmem Error [{0}]", ex.Message)
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
            If Not ksMemWrite(TBL_HY_COILRESULT_NAME, m_typHyCoilResult) Then
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
    ''' コイル実績を表示・読み込み
    ''' </summary>
    ''' <param name="rec"></param>
    ''' <param name="flg">true:表示 false:読込</param>	
    ''' <remarks></remarks>
    Private Sub CoilBaseData(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

        If m_typHyCoilResult.rec Is Nothing Then
            Exit Sub
        End If

        ' 対象のdgvをセット
        Dim dgv As tCnt_DataGridView = dgvCoilResult

        With m_typHyCoilResult.rec(rec)
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    Select Case ii
                        Case 0  ' φω実測長
                            If init Then dgv(0, ii).Value = "φω実測長" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nLen _
                            Else .nLen = CInt(dgv(1, ii).Value)
                        Case 1  ' カット情報セット数
                            If init Then dgv(0, ii).Value = "カット情報セット数" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nCutCnt _
                            Else .nCutCnt = CInt(dgv(1, ii).Value)
                    End Select

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("コイル実績{0} エラー idx[{1}] Error [{2}]", CStr(IIf(flg, "表示", "読込")), ii, ex.Message)
                    txtInfo.Text = str

                End Try
            Next
        End With
    End Sub

    ''' <summary>
    ''' カット情報を表示・読み込み
    ''' </summary>
    ''' <param name="rec"></param>
    ''' <param name="flg">true:表示 false:読込</param>	
    ''' <remarks></remarks>
    Private Sub CountInf(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

        If m_typHyCoilResult.rec Is Nothing Then
            Exit Sub
        End If

        ' 対象のdgvをセット
        Dim dgv As tCnt_DataGridView = dgvCutInf

        With m_typHyCoilResult.rec(rec)
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    If init Then
                        dgv(0, ii).Value = ""
                        dgv(1, ii).Value = ""
                    ElseIf flg Then
                        dgv(0, ii).Value = .cut(ii).nMode
                        dgv(1, ii).Value = .cut(ii).nPos
                    Else
                        .cut(ii).nMode = CInt(dgv(0, ii).Value)
                        .cut(ii).nPos = CInt(dgv(1, ii).Value)
                    End If

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("カット情報{0} エラー idx[{1}] Error [{2}]", CStr(IIf(flg, "表示", "読込")), ii, ex.Message)
                    txtInfo.Text = str

                End Try
            Next
        End With
    End Sub


    ''' <summary>
    ''' 表示を初期化する
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispInit()
        ' システム共通のコイル情報
        CoilBaseData(0, True, True)
        ' カウント情報
        CountInf(0, True, True)
    End Sub

    ''' <summary>
    ''' dgvからデータを読み込む
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadFromDgv()
        Dim rec As Integer = cmbRecord.SelectedIndex

        ' システム共通のコイル情報
        CoilBaseData(rec, False, False)
        ' カウント情報
        CountInf(rec, False, False)

    End Sub

    ''' <summary>
    ''' 共有メモリの内容を表示する
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispShmem()
        Dim rec As Integer = cmbRecord.SelectedIndex

        ' システム共通のコイル情報
        CoilBaseData(rec, False, True)
        ' カウント情報
        CountInf(rec, False, True)

    End Sub

    Private Sub frmHyCoilResult_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        m_timer1.Stop()
        m_timer1.Dispose()
    End Sub
End Class