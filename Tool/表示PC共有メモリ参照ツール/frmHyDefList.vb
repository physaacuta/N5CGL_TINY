Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Runtime.InteropServices

Public Class frmHyDefList
	'///////////////////////////////////////////////////////
	'メンバー変数
	'///////////////////////////////////////////////////////
	Private m_typHyDeflist As TBL_HY_DEFLIST

	Private WithEvents m_timer1 As New Timer	' 表示自動更新用タイマー
	Private WithEvents m_timer2 As New Timer	' 共有メモリアクセス可・不可確認タイマー

	''' <summary>
	''' フォームロード
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmHyDefList_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
		' 表示自動更新タイマー周期設定
		m_timer1.Interval = 100			' 100msec

		' 共有メモリにアクセス可能かチェックタイマー
		m_timer2.Interval = 100			' 100msec
		m_timer2.Start()

		' ポインター nud 初期値セット
		nudRec_O.Maximum = HY_DEFLIST_REC_MAX
		nudRec_O.Minimum = 1
		nudRec_O.Value = 1

		nudRec_U.Maximum = HY_DEFLIST_REC_MAX
		nudRec_U.Minimum = 1
        nudRec_U.Value = 1

        ' dgvの初期化
        InitDgv(dgvDeflist_o, 10)	   ' 疵一覧情報表面
		InitDgv(dgvDeflist_u, 10)      ' 疵一覧情報裏面
        InitDgvdefect(dgvDefect_o, 61)      ' 疵一覧情報表面
        InitDgvdefect(dgvDefect_u, 61)      ' 疵一覧情報表面
        InitDgvPath(dgvPath_o, 4)
        InitDgvPath(dgvPath_u, 4)

        ' 表示内容クリア
        'DispInit()

        ' 共有メモリの全体を読み込む
        If ReadFromShmem() Then
            DispInit()
            ' 共有メモリの内容を表示
            DispShmem()
        End If

        ' 自動更新を有効にする・・・必要ない場合はコメントにする
        chkUpdate.Checked = True

		' 共有メモリサイズ情報表示
		Dim str As String = ""
		str = String.Format("TBL_HY_DEFLIST Size = {0}", Marshal.SizeOf(GetType(TBL_HY_DEFLIST))) & vbCrLf
		str &= String.Format("DSP_LIST_HEADER Size = {0}", Marshal.SizeOf(GetType(DSP_LIST_HEADER))) & vbCrLf
        str &= String.Format("DSP_LIST_DEFECT Size = {0}", Marshal.SizeOf(GetType(DSP_LIST_DEFECT))) & vbCrLf
        str &= String.Format("DSP_LIST_DATA Size = {0}", Marshal.SizeOf(GetType(DSP_LIST_DATA))) & vbCrLf
		str &= String.Format("HY_DEFLIST_REC_MAX = {0}", HY_DEFLIST_REC_MAX) & vbCrLf
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
            Dim nRec_o As Integer = m_typHyDeflist.rec.head.nPos(0)
            Dim nRec_u As Integer = m_typHyDeflist.rec.head.nPos(1)

            If nRec_o <> 0 Then nudRec_O.Value = nRec_o Else nudRec_O.Value = 1
			If nRec_u <> 0 Then nudRec_U.Value = nRec_u Else nudRec_U.Value = 1


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
		Dim tmp As TBL_HY_DEFLIST = CType(ksMemRead(TBL_HY_DEFLIST_NAME, 0, GetType(TBL_HY_DEFLIST), nRetc), TBL_HY_DEFLIST)
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
                        .Columns(ii).Width = 140
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
    ''' ポインタ変更自
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub nudPoint_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles nudRec_O.ValueChanged, nudRec_U.ValueChanged
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
		If ReadFromShmemAll() Then
			' dgvからデータを読み込む dgv->m_typHyDeflist
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
		Dim nRetc As ValueType = 0

		Try
			m_typHyDeflist = CType(ksMemRead(TBL_HY_DEFLIST_NAME, 0, GetType(TBL_HY_DEFLIST), nRetc), TBL_HY_DEFLIST)
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
		Dim nRetc As ValueType = 0

		Try
			m_typHyDeflist = CType(ksMemRead(TBL_HY_DEFLIST_NAME, 0, GetType(TBL_HY_DEFLIST), nRetc), TBL_HY_DEFLIST)
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
	''' 共有メモリに書き込み
	''' </summary>
	''' <returns>true:書き込み成功 false:書き込み失敗</returns>
	''' <remarks>m_typHyCoilInf⇒共有メモリ</remarks>
	Private Function WriteToShmem() As Boolean
		Dim bRet As Boolean = False

		Try
			If Not ksMemWrite(TBL_HY_DEFLIST_NAME, m_typHyDeflist) Then
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
	''' 疵一覧情報 表面を表示・読み込み
	''' </summary>
	''' <param name="rec"></param>
	''' <param name="flg">true:表示 false:読込</param>	
	''' <remarks></remarks>
	Private Sub Deflist_o(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

		' 対象のdgvをセット
		Dim dgv As tCnt_DataGridView = dgvDeflist_o

        With m_typHyDeflist.rec.data(0).DfData(rec)
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    Select Case ii
                        Case 0  ' 管理No
                            If init Then dgv(0, ii).Value = "管理No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cKanriNo _
                            Else .cKanriNo = CStr(dgv(1, ii).Value)
                        Case 1  ' コイルNo
                            If init Then dgv(0, ii).Value = "コイルNo" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cCoilNo) _
                            Else tMod.StringToByte(.cCoilNo, CStr(dgv(1, ii).Value), .cCoilNo.Length)
                        Case 2  ' 命令No
                            If init Then dgv(0, ii).Value = "命令No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cMeiNo) _
                            Else tMod.StringToByte(.cMeiNo, CStr(dgv(1, ii).Value), .cMeiNo.Length)
                        Case 3  ' 予備
                            If init Then dgv(0, ii).Value = "予備" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cYobi1 _
                            Else .cYobi1 = CStr(dgv(1, ii).Value)
                    End Select

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("疵一覧情報 表面{0} エラー idx[{1}] Error [{2}]", CStr(IIf(flg, "表示", "読込")), ii, ex.Message)
                    txtInfo.Text = str
                End Try
            Next
        End With
    End Sub

	''' <summary>
	''' 疵一覧情報 表面を表示・読み込み
	''' </summary>
	''' <param name="rec"></param>
	''' <param name="flg">true:表示 false:読込</param>	
	''' <remarks></remarks>
	Private Sub Deflist_u(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

		' 対象のdgvをセット
		Dim dgv As tCnt_DataGridView = dgvDeflist_u

        With m_typHyDeflist.rec.data(1).DfData(rec)
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    Select Case ii
                        Case 0  ' 管理No
                            If init Then dgv(0, ii).Value = "管理No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cKanriNo _
                            Else .cKanriNo = CStr(dgv(1, ii).Value)
                        Case 1  ' コイルNo
                            If init Then dgv(0, ii).Value = "コイルNo" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cCoilNo) _
                            Else tMod.StringToByte(.cCoilNo, CStr(dgv(1, ii).Value), .cCoilNo.Length)
                        Case 2  ' 命令No
                            If init Then dgv(0, ii).Value = "命令No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cMeiNo) _
                            Else tMod.StringToByte(.cMeiNo, CStr(dgv(1, ii).Value), .cMeiNo.Length)
                        Case 3  ' 予備
                            If init Then dgv(0, ii).Value = "予備" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cYobi1 _
                            Else .cYobi1 = CStr(dgv(1, ii).Value)
                    End Select

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("疵一覧情報 表面{0} エラー idx[{1}] Error [{2}]", CStr(IIf(flg, "表示", "読込")), ii, ex.Message)
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
		' 最新レコード位置
		txtRec_o.Text = ""
		txtRec_u.Text = ""
		' 疵一覧情報表面
		Deflist_o(0, True, True)
        ' 疵一覧情報裏面
        Deflist_u(0, True, True)

        DefectInf(dgvDefect_o, 0, 0, 0, True, True)
        DefectInf(dgvDefect_u, 1, 0, 0, True, True)

        path(dgvPath_o, 0, 0, True, True)
        path(dgvPath_u, 0, 0, True, True)
    End Sub

    ''' <summary>
    ''' dgvからデータを読み込む
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadFromDgv()
		Dim rec_o As Integer = CInt(nudRec_O.Value) - 1
		Dim rec_u As Integer = CInt(nudRec_U.Value) - 1

        ' 最新レコード位置
        m_typHyDeflist.rec.head.nPos(0) = CInt(txtRec_o.Text)
        m_typHyDeflist.rec.head.nPos(1) = CInt(txtRec_u.Text)

        ' 疵一覧情報表面
        Deflist_o(rec_o, False, False)
		' 疵一覧情報裏面
		Deflist_u(rec_u, False, False)

        DefectInf(dgvDefect_o, 0, 0, rec_o, False, False)
        DefectInf(dgvDefect_u, 1, 0, rec_u, False, False)

        path(dgvPath_o, 0, rec_o, False, False)
        path(dgvPath_u, 1, rec_u, False, False)

    End Sub

	''' <summary>
	''' 共有メモリの内容を表示する
	''' </summary>
	''' <remarks></remarks>
	Private Sub DispShmem()
		Dim rec_o As Integer = CInt(nudRec_O.Value) - 1
		Dim rec_u As Integer = CInt(nudRec_U.Value) - 1

        If rec_o < 0 Then rec_o = 0
        If rec_u < 0 Then rec_u = 0

        ' 最新レコード位置
        txtRec_o.Text = m_typHyDeflist.rec.head.nPos(0).ToString
        txtRec_u.Text = m_typHyDeflist.rec.head.nPos(1).ToString

        ' 疵一覧情報表面
        Deflist_o(rec_o, False, True)
		' 疵一覧情報裏面
		Deflist_u(rec_u, False, True)


        DefectInf(dgvDefect_o, 0, 0, rec_o, False, True)
        DefectInf(dgvDefect_u, 1, 0, rec_u, False, True)

        path(dgvPath_o, 0, rec_o, False, True)
        path(dgvPath_u, 1, rec_u, False, True)

    End Sub

    ''' <summary>
    ''' DataGridView初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="RowCount"></param>
    ''' <remarks></remarks>
    Private Sub InitDgvdefect(ByVal dgv As tCnt_DataGridView, ByVal RowCount As Integer)
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
                        .Columns(ii).Width = 80
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
                        .Columns(ii).Width = 180
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
    ''' 疵情報を表示・読み込み
    ''' </summary>
    ''' <param name="rec"></param>
    ''' <param name="flg">true:表示 false:読込</param>	
    ''' <remarks></remarks>
    Private Sub DefectInf(ByVal dgv As tCnt_DataGridView, ByVal nMen As Integer, ByVal nSpm As Integer, ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

        '' 対象のdgvをセット
        'Dim dgv As tCnt_DataGridView = dgvDefect1

        With m_typHyDeflist.rec.data(nMen).DfData(rec).typ.data
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    Select Case ii
                        Case 0  ' 疵No
                            If init Then dgv(0, ii).Value = "疵No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nKizuNo.ToString _
                            Else .nKizuNo = CInt(dgv(1, ii).Value)
                        Case 1  ' 表裏区分
                            If init Then dgv(0, ii).Value = "表裏区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .TorB.ToString _
                            Else .TorB = CUShort(dgv(1, ii).Value)
                        Case 2  ' 疵種EdasysID
                            If init Then dgv(0, ii).Value = "疵種EdasysID" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nTid.ToString _
                            Else .nTid = CUShort(dgv(1, ii).Value)
                        Case 3  ' グレードEdasysID
                            If init Then dgv(0, ii).Value = "グレードEdasysID" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nGid.ToString _
                            Else .nGid = CUShort(dgv(1, ii).Value)
                        Case 4  ' 周期疵区分
                            If init Then dgv(0, ii).Value = "周期疵区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cycle_div.ToString _
                            Else .cycle_div = CUShort(dgv(1, ii).Value)
                        Case 5  ' DSWS区分
                            If init Then dgv(0, ii).Value = "DSWS区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .dsws.ToString _
                            Else .dsws = CUShort(dgv(1, ii).Value)
                        Case 6  ' カメラセット
                            If init Then dgv(0, ii).Value = "カメラセット" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .camset.ToString _
                            Else .camset = CUShort(dgv(1, ii).Value)
                        Case 7  ' Fからの位置[mm]
                            If init Then dgv(0, ii).Value = "Fからの位置[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .y.ToString _
                            Else .y = CSng(dgv(1, ii).Value)
                        Case 8  ' 出側からの位置[mm]
                            If init Then dgv(0, ii).Value = "出側からの位置[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .y_out.ToString _
                            Else .y_out = CSng(dgv(1, ii).Value)
                        Case 9  ' DS側エッジからの位置[mm]
                            If init Then dgv(0, ii).Value = "DS側エッジからの位置[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .ds.ToString _
                            Else .ds = CSng(dgv(1, ii).Value)
                        Case 10 ' WS側エッジからの位置[mm]
                            If init Then dgv(0, ii).Value = "WS側エッジからの位置[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .ws.ToString _
                            Else .ws = CSng(dgv(1, ii).Value)
                        Case 11 ' DS側板センターからの距離[mm]
                            If init Then dgv(0, ii).Value = "DS側板センターからの距離[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .dsc.ToString _
                            Else .dsc = CSng(dgv(1, ii).Value)
                        Case 12 ' WS側板センターからの距離[mm]
                            If init Then dgv(0, ii).Value = "WS側板センターからの距離[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .wsc.ToString _
                            Else .wsc = CSng(dgv(1, ii).Value)
                        Case 13 ' 速度[mpm]
                            If init Then dgv(0, ii).Value = "速度[mpm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .speed.ToString _
                            Else .speed = CInt(dgv(1, ii).Value)
                        Case 14 ' ピッチ[mm]
                            If init Then dgv(0, ii).Value = "ピッチ[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .pitch.ToString _
                            Else .pitch = CInt(dgv(1, ii).Value)
                        Case 15 ' ロール径[mm]
                            If init Then dgv(0, ii).Value = "ロール径[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .roolkei.ToString _
                            Else .roolkei = CInt(dgv(1, ii).Value)
                        Case 16 ' 面積[mm^2]
                            If init Then dgv(0, ii).Value = "面積[mm^2]" : dgv(1, ii).Value = "" _
                             Else If flg Then dgv(1, ii).Value = .aria.ToString _
                             Else .aria = CSng(dgv(1, ii).Value)
                        Case 17 ' 疵幅[mm]
                            If init Then dgv(0, ii).Value = "疵幅[mm]" : dgv(1, ii).Value = "" _
                             Else If flg Then dgv(1, ii).Value = .kizu_wid.ToString _
                             Else .kizu_wid = CInt(dgv(1, ii).Value)
                        Case 18 ' 疵長さ[mm]
                            If init Then dgv(0, ii).Value = "疵長さ[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .kizu_len.ToString _
                            Else .kizu_len = CInt(dgv(1, ii).Value)
                        Case 19 ' 縦分解能[mm/pixel]
                            If init Then dgv(0, ii).Value = "縦分解能[mm/pixel]" : dgv(1, ii).Value = "" _
                             Else If flg Then dgv(1, ii).Value = .h_res.ToString _
                             Else .h_res = CSng(dgv(1, ii).Value)
                        Case 20 ' 横分解能[mm/pixel]
                            If init Then dgv(0, ii).Value = "横分解能[mm/pixel]" : dgv(1, ii).Value = "" _
                             Else If flg Then dgv(1, ii).Value = .w_res.ToString _
                             Else .w_res = CSng(dgv(1, ii).Value)
                        Case 21  ' フレームNo
                            If init Then dgv(0, ii).Value = "フレームNo" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nFrameNo.ToString _
                            Else .nFrameNo = CInt(dgv(1, ii).Value)
                        Case 22  ' フレーム外接Xmin[pixel]
                            If init Then dgv(0, ii).Value = "フレーム外接Xmin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .frm_x_min.ToString _
                            Else .frm_x_min = CShort(dgv(1, ii).Value)
                        Case 23  ' フレーム外接Xmax[pixel]
                            If init Then dgv(0, ii).Value = "フレーム外接Xmax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .frm_x_max.ToString _
                            Else .frm_x_max = CShort(dgv(1, ii).Value)
                        Case 24  ' フレーム外接Ymin[pixel]
                            If init Then dgv(0, ii).Value = "フレーム外接Ymin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .frm_y_min.ToString _
                            Else .frm_y_min = CShort(dgv(1, ii).Value)
                        Case 25  ' フレーム外接Ymax[pixel]
                            If init Then dgv(0, ii).Value = "フレーム外接Ymax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .frm_y_max.ToString _
                            Else .frm_y_max = CShort(dgv(1, ii).Value)
                        Case 26  ' 間引きフレーム外接Xmin[pixel]
                            If init Then dgv(0, ii).Value = "間引きフレーム外接Xmin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cull_frm_x_min.ToString _
                            Else .cull_frm_x_min = CShort(dgv(1, ii).Value)
                        Case 27  ' 間引きフレーム外接Xmax[pixel]
                            If init Then dgv(0, ii).Value = "間引きフレーム外接Xmax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cull_frm_x_max.ToString _
                            Else .cull_frm_x_max = CShort(dgv(1, ii).Value)
                        Case 28  ' 間引きフレーム外接Ymin[pixel]
                            If init Then dgv(0, ii).Value = "間引きフレーム外接Ymin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cull_frm_y_min.ToString _
                            Else .cull_frm_y_min = CShort(dgv(1, ii).Value)
                        Case 29  ' 間引きフレーム外接Ymax[pixel]
                            If init Then dgv(0, ii).Value = "間引きフレーム外接Ymax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cull_frm_y_max.ToString _
                            Else .cull_frm_y_max = CShort(dgv(1, ii).Value)
                        Case 30  ' 外接Xmin[pixel]
                            If init Then dgv(0, ii).Value = "外接Xmin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .box_x_min.ToString _
                            Else .box_x_min = CShort(dgv(1, ii).Value)
                        Case 31  ' 外接Xmax[pixel]
                            If init Then dgv(0, ii).Value = "外接Xmax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .box_x_max.ToString _
                            Else .box_x_max = CShort(dgv(1, ii).Value)
                        Case 32  ' 外接Ymin[pixel]
                            If init Then dgv(0, ii).Value = "外接Ymin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .box_y_min.ToString _
                            Else .box_y_min = CShort(dgv(1, ii).Value)
                        Case 33  '外接Ymax[pixel]
                            If init Then dgv(0, ii).Value = "外接Ymax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .box_y_max.ToString _
                            Else .box_y_max = CShort(dgv(1, ii).Value)
                        Case 34  '疵中心位置X[pixel]
                            If init Then dgv(0, ii).Value = "疵中心位置X[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .center_x.ToString _
                            Else .center_x = CShort(dgv(1, ii).Value)
                        Case 35  '疵中心位置Y[pixel]
                            If init Then dgv(0, ii).Value = "疵中心位置Y[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .center_y.ToString _
                            Else .center_y = CShort(dgv(1, ii).Value)
                        Case 36 '縦画素数[pixel]
                            If init Then dgv(0, ii).Value = "縦画素数[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .h_size.ToString _
                            Else .h_size = CUShort(dgv(1, ii).Value)
                        Case 37 '横画素数[pixel]
                            If init Then dgv(0, ii).Value = "横画素数[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .w_size.ToString _
                            Else .w_size = CUShort(dgv(1, ii).Value)
                        Case 38  '元画像_外接Xmin[pixel]
                            If init Then dgv(0, ii).Value = "元画像_外接Xmin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_box_x_min.ToString _
                            Else .base_box_x_min = CShort(dgv(1, ii).Value)
                        Case 39  '元画像_外接Xmax[pixel]
                            If init Then dgv(0, ii).Value = "元画像_外接Xmax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_box_x_max.ToString _
                            Else .base_box_x_max = CShort(dgv(1, ii).Value)
                        Case 40  '元画像_外接Ymin[pixel]
                            If init Then dgv(0, ii).Value = "元画像_外接Ymin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_box_y_min.ToString _
                            Else .base_box_y_min = CShort(dgv(1, ii).Value)
                        Case 41  '元画像_外接Ymax[pixel]
                            If init Then dgv(0, ii).Value = "元画像_外接Ymax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_box_y_max.ToString _
                            Else .base_box_y_max = CShort(dgv(1, ii).Value)
                        Case 42  '元画像_疵中心位置X[pixel]
                            If init Then dgv(0, ii).Value = "元画像_疵中心位置X[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_center_x.ToString _
                            Else .base_center_x = CShort(dgv(1, ii).Value)
                        Case 43  '元画像_疵中心位置Y[pixel]
                            If init Then dgv(0, ii).Value = "元画像_疵中心位置Y[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_center_y.ToString _
                            Else .base_center_y = CShort(dgv(1, ii).Value)
                        Case 44 ' 元画像_縦画素数[pixel]
                            If init Then dgv(0, ii).Value = "元画像_縦画素数[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_h_size.ToString _
                            Else .h_size = CUShort(dgv(1, ii).Value)
                        Case 45 ' 元画像_横画素数[pixel]
                            If init Then dgv(0, ii).Value = "元画像_横画素数[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_w_size.ToString _
                            Else .w_size = CUShort(dgv(1, ii).Value)
                        Case 46 ' 元画像_縦分解能[mm/pixel]
                            If init Then dgv(0, ii).Value = "元画像_縦分解能[mm/pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_h_res.ToString _
                            Else .w_size = CUShort(dgv(1, ii).Value)
                        Case 47 ' 元画像_横分解能[mm/pixel]
                            If init Then dgv(0, ii).Value = "元画像_横分解能[mm/pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_w_res.ToString _
                            Else .w_size = CUShort(dgv(1, ii).Value)
                        Case 48 ' 縮小率
                            If init Then dgv(0, ii).Value = "縮小率" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nShrinkRatio.ToString _
                            Else .w_size = CUShort(dgv(1, ii).Value)
                        Case 49 '疵種優先度
                            If init Then dgv(0, ii).Value = "疵種優先度" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nTyuu.ToString _
                            Else .nTyuu = CUShort(dgv(1, ii).Value)
                        Case 50 'グレード優先度
                            If init Then dgv(0, ii).Value = "グレード優先度" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nGyuu.ToString _
                            Else .nGyuu = CUShort(dgv(1, ii).Value)
                        Case 51 '幅方向絶対位置
                            If init Then dgv(0, ii).Value = "幅方向絶対位置" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nWidAbsPos.ToString _
                            Else .nWidAbsPos = CShort(dgv(1, ii).Value)
                        Case 52 '幅分割区分
                            If init Then dgv(0, ii).Value = "幅分割区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nWidDiv.ToString _
                            Else .nWidDiv = CShort(dgv(1, ii).Value)
                        Case 53 '探傷画面対象疵
                            If init Then dgv(0, ii).Value = "探傷画面対象疵" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nObjTansho.ToString _
                            Else .nObjTansho = CShort(dgv(1, ii).Value)
                        Case 54 '重欠陥一覧画面対象疵
                            If init Then dgv(0, ii).Value = "重欠陥一覧画面対象疵" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nObjDeflist.ToString _
                            Else .nObjDeflist = CShort(dgv(1, ii).Value)
                        Case 55 '重欠陥一覧強調表示対象疵
                            If init Then dgv(0, ii).Value = "重欠陥一覧強調表示対象疵" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nModeDeflist.ToString _
                            Else .nModeDeflist = CShort(dgv(1, ii).Value)
                        Case 56 '疵発生時刻 年
                            If init Then dgv(0, ii).Value = "疵発生時刻 年" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .time(0).ToString _
                            Else .time(0) = CShort(dgv(1, ii).Value)
                        Case 57 '疵発生時刻 月
                            If init Then dgv(0, ii).Value = "疵発生時刻 月" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .time(1).ToString _
                            Else .time(1) = CShort(dgv(1, ii).Value)
                        Case 58 '疵発生時刻 日
                            If init Then dgv(0, ii).Value = "疵発生時刻 日" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .time(2).ToString _
                            Else .time(2) = CShort(dgv(1, ii).Value)
                        Case 59 '疵発生時刻 時
                            If init Then dgv(0, ii).Value = "疵発生時刻 時" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .time(3).ToString _
                            Else .time(3) = CShort(dgv(1, ii).Value)
                        Case 60 '疵発生時刻 分
                            If init Then dgv(0, ii).Value = "疵発生時刻 分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .time(4).ToString _
                            Else .time(4) = CShort(dgv(1, ii).Value)
                        Case 61 '疵発生時刻 秒
                            If init Then dgv(0, ii).Value = "疵発生時刻 秒" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .time(5).ToString _
                            Else .time(5) = CShort(dgv(1, ii).Value)
                    End Select

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("疵情報(SPM圧下無){0} エラー idx[{1}] Error [{2}]", CStr(IIf(flg, "表示", "読込")), ii, ex.Message)
                    txtInfo.Text = str
                End Try
            Next
            dgv.ClearSelection()
        End With
    End Sub
    ''' <summary>
    ''' 画像パスを表示・読み込み
    ''' </summary>
    ''' <param name="rec"></param>
    ''' <param name="flg">true:表示 false:読込</param>	
    ''' <remarks></remarks>
    Private Sub path(ByVal dgv As tCnt_DataGridView, ByVal nMen As Integer, ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

        With m_typHyDeflist.rec.data(nMen).DfData(rec).typ
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    Select Case ii

                        Case 0  ' 画像のパス[正]
                            If init Then dgv(0, ii).Value = "画像のパス[正]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .image_path_s _
                            Else .image_path_s = CStr(dgv(1, ii).Value)
                        Case 1  ' 画像のパス[乱]
                            If init Then dgv(0, ii).Value = "画像のパス[乱]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .image_path_r _
                            Else .image_path_r = CStr(dgv(1, ii).Value)
                        Case 2  ' 画像のパス[乱2]
                            If init Then dgv(0, ii).Value = "画像のパス[乱2]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .image_path_r2 _
                            Else .image_path_r2 = CStr(dgv(1, ii).Value)
                        Case 3  ' 画像のパス[乱3]
                            If init Then dgv(0, ii).Value = "画像のパス[乱3]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .image_path_r3 _
                            Else .image_path_r3 = CStr(dgv(1, ii).Value)

                    End Select

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("システム共通のコイル情報{0} エラー idx[{1}] Error [{2}]", CStr(IIf(flg, "表示", "読込")), ii, ex.Message)
                    txtInfo.Text = str

                End Try
            Next
            dgv.ClearSelection()
        End With
    End Sub

    ''' <summary>
    ''' レコード コンボボックス変更時
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbSpm_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)
        ' 読み込み処理
        btnRead.PerformClick()
    End Sub

    Private Sub frmHyDefList_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        m_timer1.Stop()
        m_timer1.Dispose()
    End Sub
End Class