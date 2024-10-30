Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Runtime.InteropServices


''' <summary>
''' コイル情報
''' </summary>
''' <remarks></remarks>
Public Class frmToCoilTbl
	'///////////////////////////////////////////////////////
	'メンバー変数
	'///////////////////////////////////////////////////////
	Private m_typToCoilTbl As TO_COIL_TBL
	Private WithEvents m_timer1 As New Timer	' 表示自動更新用タイマー
	Private WithEvents m_timer2 As New Timer	' 共有メモリアクセス可・不可確認タイマー

	''' <summary>
	''' フォームロード
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmToCoilTbl_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

		' 表示自動更新タイマー周期設定
		m_timer1.Interval = 100			' 100msec

		' 共有メモリにアクセス可能かチェックタイマー
		m_timer2.Interval = 100			' 100msec
		m_timer2.Start()

		

		'レコードコンボボックス初期化
		cmbRecord.Items.Clear()
		For ii As Integer = 0 To 1
			cmbRecord.Items.Add(CStr(ii + 1))
		Next
		cmbRecord.SelectedIndex = 0

        ' dgvの初期化
        InitDgv(dgvCoilSetting, 13)
        InitDgv(dgvCoilBaseData, 13)
        InitDgv(dgvVal, 77)

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
		str = String.Format("COIL_SETTING SIZE Size = {0}", Marshal.SizeOf(GetType(COIL_SETTING))) & vbCrLf
		str &= String.Format("COIL_BASEDATA Size = {0}", Marshal.SizeOf(GetType(COIL_BASEDATA))) & vbCrLf
        str &= String.Format("TO_COIL_REC Size = {0}", Marshal.SizeOf(GetType(TO_COIL_TBL.TO_COIL_REC))) & vbCrLf
        str &= String.Format("TO_COIL_TBL Size = {0}", Marshal.SizeOf(GetType(TO_COIL_TBL))) & vbCrLf
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
		Dim tmp As COIL_SETTING = CType(ksMemRead(TBL_TO_COIL_NAME, 0, GetType(COIL_SETTING), nRetc), COIL_SETTING)
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
			.RowCount = RowCount
			.RowTemplate.Height = 20

			.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue			' 選択
			.DefaultCellStyle().ForeColor = Color.Black
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue		' 1行おきに色替え

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
						.Columns(ii).Width = 240
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
	''' <remarks>共有メモリ⇒m_typToCoilTbl</remarks>
	Private Function ReadFromShmem() As Boolean
		Dim bRet As Boolean = False

		Try
			m_typToCoilTbl = CType(ksMemRead(TBL_TO_COIL_NAME, GetType(TO_COIL_TBL)), TO_COIL_TBL)

			If m_typToCoilTbl.rec Is Nothing Then
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
	''' <remarks>m_typToCoilTbl⇒共有メモリ</remarks>
	Private Function WriteToShmem() As Boolean
		Dim bRet As Boolean = False

		Try
			If Not ksMemWrite(TBL_TO_COIL_NAME, m_typToCoilTbl) Then
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
	''' 表示を初期化する
	''' </summary>
	''' <remarks></remarks>
	Private Sub DispInit()
		txtcID.Text = ""
		' コイル基本情報表示初期化
		CoilSetting(0, True, True)
		' コイル基本情報表示初期化
		CoilBaseData(0, True, True)
		' コイル基本情報表示初期化
		CoilVal(0, True, True)
	End Sub

	''' <summary>
	''' コイル基本設定を表示・読み込み
	''' </summary>
	''' <param name="rec"></param>
	''' <param name="flg">true:表示 false:読込</param>	
	''' <remarks></remarks>
	Private Sub CoilSetting(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

        If m_typToCoilTbl.rec Is Nothing Then
            Exit Sub
        End If

        ' 対象のdgvをセット
        Dim dgv As tCnt_DataGridView = dgvCoilSetting

		With m_typToCoilTbl.rec(rec).setting
			For ii As Integer = 0 To dgv.RowCount - 1
				Try
					Select Case ii
						Case 0	' ウォーマ材区分
							If init Then dgv(0, ii).Value = "ウォーマ材区分" : dgv(1, ii).Value = "" _
							Else If flg Then dgv(1, ii).Value = .bWalm.ToString _
							Else .bWalm = CByte(dgv(1, ii).Value)
						Case 1	' 検査表面状態 デフォルト値使用(表)
							If init Then dgv(0, ii).Value = "検査表面状態 デフォルト値使用(表)" : dgv(1, ii).Value = "" _
							Else If flg Then dgv(1, ii).Value = .bDefScode(0).ToString _
							Else .bDefScode(0) = CByte(dgv(1, ii).Value)
						Case 2	' 検査表面状態 デフォルト値使用(裏)
							If init Then dgv(0, ii).Value = "検査表面状態 デフォルト値使用(裏)" : dgv(1, ii).Value = "" _
							Else If flg Then dgv(1, ii).Value = .bDefScode(1).ToString _
							Else .bDefScode(1) = CByte(dgv(1, ii).Value)
						Case 3	' 検査パターン デフォルト値使用(表)
							If init Then dgv(0, ii).Value = "検査パターン デフォルト値使用(表)" : dgv(1, ii).Value = "" _
							Else If flg Then dgv(1, ii).Value = .bDefPcode(0).ToString _
							Else .bDefPcode(0) = CByte(dgv(1, ii).Value)
						Case 4	' 検査パターン デフォルト値使用(裏)
							If init Then dgv(0, ii).Value = "検査パターン デフォルト値使用(裏)" : dgv(1, ii).Value = "" _
							Else If flg Then dgv(1, ii).Value = .bDefPcode(1).ToString _
                            Else .bDefPcode(1) = CByte(dgv(1, ii).Value)
                        Case 5  ' 検査表面状態(表)
                            If init Then dgv(0, ii).Value = "検査表面状態(表)" : dgv(1, ii).Value = "" _
							Else If flg Then dgv(1, ii).Value = .scode(0).ToString _
							Else .scode(0) = CInt(dgv(1, ii).Value)
                        Case 6  ' 検査表面状態(裏)
                            If init Then dgv(0, ii).Value = "検査表面状態(裏)" : dgv(1, ii).Value = "" _
							Else If flg Then dgv(1, ii).Value = .scode(1).ToString _
							Else .scode(1) = CInt(dgv(1, ii).Value)
                        Case 7  ' 検査パターン(表)
                            If init Then dgv(0, ii).Value = "検査パターン(表)" : dgv(1, ii).Value = "" _
							Else If flg Then dgv(1, ii).Value = .pcode(0).ToString _
							Else .pcode(0) = CInt(dgv(1, ii).Value)
                        Case 8  ' 検査パターン(裏)
                            If init Then dgv(0, ii).Value = "検査パターン(裏)" : dgv(1, ii).Value = "" _
							Else If flg Then dgv(1, ii).Value = .pcode(1).ToString _
                            Else .pcode(1) = CInt(dgv(1, ii).Value)
                        Case 9  ' 検査表面状態(表)(SPM開放時)
                            If init Then dgv(0, ii).Value = "検査表面状態(表)(SPM開放時)" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .scode_spmoff(0).ToString _
                            Else .scode_spmoff(0) = CInt(dgv(1, ii).Value)
                        Case 10  ' 検査表面状態(裏)(SPM開放時)
                            If init Then dgv(0, ii).Value = "検査表面状態(裏)(SPM開放時)" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .scode_spmoff(1).ToString _
                            Else .scode_spmoff(1) = CInt(dgv(1, ii).Value)
                        Case 11  ' 検査パターン(表)(SPM開放時)
                            If init Then dgv(0, ii).Value = "検査パターン(表)(SPM開放時)" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .pcode_spmoff(0).ToString _
                            Else .pcode_spmoff(0) = CInt(dgv(1, ii).Value)
                        Case 12  ' 検査パターン(裏)(SPM開放時)
                            If init Then dgv(0, ii).Value = "検査パターン(裏)(SPM開放時)" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .pcode_spmoff(1).ToString _
                            Else .pcode_spmoff(1) = CInt(dgv(1, ii).Value)
                    End Select

                Catch ex As Exception
					Dim str As String = txtInfo.Text
					str &= String.Format("コイル基本設定{0} エラー idx[{1}] Error [{2}]", CStr(IIf(flg, "表示", "読込")), ii, ex.Message)
					txtInfo.Text = str
				End Try
			Next
		End With

	End Sub

	''' <summary>
	''' コイル基本情報を表示・読み込み
	''' </summary>
	''' <param name="rec"></param>
	''' <param name="flg">true:表示 false:読込</param>	
	''' <remarks></remarks>
	Private Sub CoilBaseData(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

        If m_typToCoilTbl.rec Is Nothing Then
            Exit Sub
        End If

        ' 対象のdgvをセット
        Dim dgv As tCnt_DataGridView = dgvCoilBaseData

		With m_typToCoilTbl.rec(rec).basedata
			For ii As Integer = 0 To dgv.RowCount - 1
				Try
					Select Case ii
						Case 0	' 命令No
							If init Then dgv(0, ii).Value = "命令No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cMeiNo) _
                            Else tMod.StringToByte(.cMeiNo, dgv(1, ii).Value.ToString, .cMeiNo.Length)
                        Case 1  ' 入側コイルNo
                            If init Then dgv(0, ii).Value = "入側コイルNo" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod_Common.ByteToString(.cCoilNoIn) _
                            Else tMod.StringToByte(.cCoilNoIn, CStr(dgv(1, ii).Value), .cCoilNoIn.Length)
                        Case 2  ' キー情報
                            If init Then dgv(0, ii).Value = "キー情報" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod_Common.ByteToString(.cCoilKey) _
                            Else tMod.StringToByte(.cCoilKey, CStr(dgv(1, ii).Value), .cCoilKey.Length)
                        Case 3  ' 予備
                            If init Then dgv(0, ii).Value = "予備" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cYobi1) _
                            Else tMod.StringToByte(.cYobi1, CStr(dgv(1, ii).Value), .cYobi1.Length)
                        Case 4 ' 入側コイル長
                            If init Then dgv(0, ii).Value = "入側コイル長[m]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nLenIn.ToString _
                            Else .nLenIn = CInt(dgv(1, ii).Value)
                        Case 5 ' 板厚
                            If init Then dgv(0, ii).Value = "板厚[μm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nAtu.ToString _
                            Else .nAtu = CInt(dgv(1, ii).Value)
                        Case 6 ' 板幅
                            If init Then dgv(0, ii).Value = "板幅[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nWid.ToString _
                            Else .nWid = CInt(dgv(1, ii).Value)
                        Case 7 ' 入側フロント端板長さ
                            If init Then dgv(0, ii).Value = "入側フロント端板長さ[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nCutLenF.ToString _
                            Else .nCutLenF = CInt(dgv(1, ii).Value)
                        Case 8 ' 入側テール端板長さ
                            If init Then dgv(0, ii).Value = "入側テール端板長さ[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nCutLenT.ToString _
                            Else .nCutLenT = CInt(dgv(1, ii).Value)
                        Case 9  ' 予備
                            If init Then dgv(0, ii).Value = "予備" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod_Common.ByteToString(.cYobi2) _
                            Else tMod.StringToByte(.cYobi2, CStr(dgv(1, ii).Value), .cYobi2.Length)
                        Case 10  ' SPM伸び率
                            If init Then dgv(0, ii).Value = "SPM伸び率[10^2%]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .dNobiSPM.ToString _
                            Else .dNobiSPM = CDbl(dgv(1, ii).Value)
                        Case 11  ' 出側コイル長[m](SPM圧下無)
                            If init Then dgv(0, ii).Value = "出側コイル長[m]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nLenOut.ToString _
                            Else .nLenOut = CInt(dgv(1, ii).Value)
                        Case 12  ' 予備
                            If init Then dgv(0, ii).Value = "予備" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod_Common.ByteToString(.cYobi3) _
                            Else tMod.StringToByte(.cYobi3, CStr(dgv(1, ii).Value), .cYobi3.Length)

                    End Select

				Catch ex As Exception
					Dim str As String = txtInfo.Text
					str &= String.Format("コイル基本情報{0} エラー idx[{1}] Error [{2}]", CStr(IIf(flg, "表示", "読込")), ii, ex.Message)
					txtInfo.Text = str

				End Try
			Next
		End With

	End Sub

	''' <summary>
	''' 受信データ部を表示・読み込み
	''' </summary>
	''' <param name="rec"></param>
	''' <param name="flg">true:表示 false:読込</param>	
	''' <remarks></remarks>
	Private Sub CoilVal(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

        If m_typToCoilTbl.rec Is Nothing Then
            Exit Sub
        End If

        ' 対象のdgvをセット
        Dim dgv As tCnt_DataGridView = dgvVal

        Dim typProCoilInf As New PRO_TYPE_COILINF
        Dim obj As Object = typProCoilInf

        If Not init Then
            ' 初期化ではない場合
            tMod.MemSet(m_typToCoilTbl.rec(rec).val, 0, obj, tMod.SizeOf(typProCoilInf))
            typProCoilInf = CType(obj, PRO_TYPE_COILINF)
        End If

        With typProCoilInf
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    Select Case ii
                        Case 0  ' サブヘッダー
                            If init Then dgv(0, ii).Value = "サブヘッダー" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod_Common.ByteToString(.head.bHead) _
                            Else tMod_Common.StringToByte(.head.bHead, CStr(dgv(1, ii).Value), .head.bHead.Length)
                        Case 1  ' データ長
                            If init Then dgv(0, ii).Value = "データ長" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .head.nLen _
                            Else .head.nLen = CShort(dgv(1, ii).Value)
                        Case 2  ' TC
                            If init Then dgv(0, ii).Value = "TC" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .head.nTC _
                            Else .head.nTC = CInt(dgv(1, ii).Value)
                        Case 3  ' 伝文区分
                            If init Then dgv(0, ii).Value = "伝文区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.Common.nKubun _
                            Else .data.Common.nKubun = CShort(dgv(1, ii).Value)
                        Case 4  ' 送信時刻　年
                            If init Then dgv(0, ii).Value = "送信時刻　年" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.Common.nDate(0) _
                            Else .data.Common.nDate(0) = CShort(dgv(1, ii).Value)
                        Case 5  ' 送信時刻　月
                            If init Then dgv(0, ii).Value = "送信時刻　月" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.Common.nDate(1) _
                            Else .data.Common.nDate(1) = CShort(dgv(1, ii).Value)
                        Case 6  ' 送信時刻　日
                            If init Then dgv(0, ii).Value = "送信時刻　日" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.Common.nDate(2) _
                            Else .data.Common.nDate(2) = CShort(dgv(1, ii).Value)
                        Case 7  ' 送信時刻　時
                            If init Then dgv(0, ii).Value = "送信時刻　時" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.Common.nDate(3) _
                            Else .data.Common.nDate(3) = CShort(dgv(1, ii).Value)
                        Case 8  ' 送信時刻　分
                            If init Then dgv(0, ii).Value = "送信時刻　分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.Common.nDate(4) _
                            Else .data.Common.nDate(4) = CShort(dgv(1, ii).Value)
                        Case 9  ' 送信時刻　秒
                            If init Then dgv(0, ii).Value = "送信時刻　秒" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.Common.nDate(5) _
                            Else .data.Common.nDate(5) = CShort(dgv(1, ii).Value)
                        Case 10  ' グラントフラグ
                            If init Then dgv(0, ii).Value = "グラントフラグ" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.Common.nGrandFlg _
                            Else .data.Common.nGrandFlg = CShort(dgv(1, ii).Value)
                        Case 11  ' ヘッダー予備
                            If init Then dgv(0, ii).Value = "ヘッダー予備" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.Common.cYobi) _
                            Else tMod.StringToByte(.data.Common.cYobi, CStr(dgv(1, ii).Value), .data.Common.cYobi.Length)
                        Case 12  ' 命令No
                            If init Then dgv(0, ii).Value = "命令No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.KeyInf.cMeiNo) _
                            Else tMod.StringToByte(.data.CoilInf.KeyInf.cMeiNo, CStr(dgv(1, ii).Value), .data.CoilInf.KeyInf.cMeiNo.Length)
                        Case 13  ' 生産No
                            If init Then dgv(0, ii).Value = "生産No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.KeyInf.cCoilNo) _
                            Else tMod.StringToByte(.data.CoilInf.KeyInf.cCoilNo, CStr(dgv(1, ii).Value), .data.CoilInf.KeyInf.cCoilNo.Length)
                        Case 14  ' 送信位置
                            If init Then dgv(0, ii).Value = "送信位置" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.CoilInf.nSendPoint _
                            Else .data.CoilInf.nSendPoint = CInt(dgv(1, ii).Value)
                        Case 15  ' 入側論理コイル長[m]
                            If init Then dgv(0, ii).Value = "入側論理コイル長[m]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.CoilInf.nLenIn _
                            Else .data.CoilInf.nLenIn = CInt(dgv(1, ii).Value)
                        Case 16  ' 材料板厚[μm]
                            If init Then dgv(0, ii).Value = "材料板厚[μm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.CoilInf.nAtuZai _
                            Else .data.CoilInf.nAtuZai = CInt(dgv(1, ii).Value)
                        Case 17  ' 材料板幅[mm]
                            If init Then dgv(0, ii).Value = "材料板幅[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.CoilInf.nWidZai _
                            Else .data.CoilInf.nWidZai = CInt(dgv(1, ii).Value)
                        Case 18  ' 注文板厚[μm]
                            If init Then dgv(0, ii).Value = "注文板厚[μm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.CoilInf.nAtuSei _
                            Else .data.CoilInf.nAtuSei = CInt(dgv(1, ii).Value)
                        Case 19  ' 注文板幅[mm]
                            If init Then dgv(0, ii).Value = "注文板幅[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.CoilInf.nWidSei _
                            Else .data.CoilInf.nWidSei = CInt(dgv(1, ii).Value)
                        Case 20  ' 現工程トリム幅[mm]
                            If init Then dgv(0, ii).Value = "現工程トリム幅[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.CoilInf.nWidTrim _
                            Else .data.CoilInf.nWidTrim = CInt(dgv(1, ii).Value)
                        Case 21  ' SPM伸び率[10-2%]
                            If init Then dgv(0, ii).Value = "SPM伸び率[10-2%]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.CoilInf.nNobiSPM _
                            Else .data.CoilInf.nNobiSPM = CInt(dgv(1, ii).Value)
                        Case 22  ' TLV伸び率[10-2%]
                            If init Then dgv(0, ii).Value = "TLV伸び率[10-2%]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.CoilInf.nNobiTLV _
                            Else .data.CoilInf.nNobiTLV = CInt(dgv(1, ii).Value)
                        Case 23  ' 入側フロント端板長さ[mm]
                            If init Then dgv(0, ii).Value = "入側フロント端板長さ[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.CoilInf.nCutF _
                            Else .data.CoilInf.nCutF = CInt(dgv(1, ii).Value)
                        Case 24  ' 入側テール端板長さ[mm]
                            If init Then dgv(0, ii).Value = "入側テール端板長さ[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.CoilInf.nCutT _
                            Else .data.CoilInf.nCutT = CInt(dgv(1, ii).Value)
                        Case 25  ' SPMワークロール径 (上)[10-1mm]
                            If init Then dgv(0, ii).Value = "SPMﾜｰｸﾛｰﾙ径(上)[10-1mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.CoilInf.nSPMRoolKei_Up _
                            Else .data.CoilInf.nSPMRoolKei_Up = CInt(dgv(1, ii).Value)
                        Case 26  ' SPMワークロール径 (下)[10-1mm]
                            If init Then dgv(0, ii).Value = "SPMﾜｰｸﾛｰﾙ径(下)[10-1mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.CoilInf.nSPMRoolKei_Down _
                            Else .data.CoilInf.nSPMRoolKei_Down = CInt(dgv(1, ii).Value)
                        Case 27  ' 検査基準_表
                            If init Then dgv(0, ii).Value = "検査基準_表" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cKenKijun_O) _
                            Else tMod.StringToByte(.data.CoilInf.cKenKijun_O, CStr(dgv(1, ii).Value), .data.CoilInf.cKenKijun_O.Length)
                        Case 28  ' 検査基準_裏
                            If init Then dgv(0, ii).Value = "検査基準_裏" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cKenKijun_U) _
                            Else tMod.StringToByte(.data.CoilInf.cKenKijun_U, CStr(dgv(1, ii).Value), .data.CoilInf.cKenKijun_U.Length)
                        Case 29  ' クロメート命令_種類
                            If init Then dgv(0, ii).Value = "クロメート命令_種類" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cKurometo_Kind) _
                            Else tMod.StringToByte(.data.CoilInf.cKurometo_Kind, CStr(dgv(1, ii).Value), .data.CoilInf.cKurometo_Kind.Length)
                        Case 30  ' クロメート命令_有無
                            If init Then dgv(0, ii).Value = "クロメート命令_有無" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cKurometo_Umu) _
                            Else tMod.StringToByte(.data.CoilInf.cKurometo_Umu, CStr(dgv(1, ii).Value), .data.CoilInf.cKurometo_Umu.Length)
                        Case 31  ' 品種
                            If init Then dgv(0, ii).Value = "品種" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cHinCode) _
                            Else tMod.StringToByte(.data.CoilInf.cHinCode, CStr(dgv(1, ii).Value), .data.CoilInf.cHinCode.Length)
                        Case 32  ' メッキ種類
                            If init Then dgv(0, ii).Value = "メッキ種類" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cMekkiKind) _
                            Else tMod.StringToByte(.data.CoilInf.cMekkiKind, CStr(dgv(1, ii).Value), .data.CoilInf.cMekkiKind.Length)
                        Case 33  ' 主材質
                            If init Then dgv(0, ii).Value = "主材質" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cSyuZai) _
                            Else tMod.StringToByte(.data.CoilInf.cSyuZai, CStr(dgv(1, ii).Value), .data.CoilInf.cSyuZai.Length)
                        Case 34  ' ディファレンシャルマーク
                            If init Then dgv(0, ii).Value = "ディファレンシャルマーク" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cDifMaker) _
                            Else tMod.StringToByte(.data.CoilInf.cDifMaker, CStr(dgv(1, ii).Value), .data.CoilInf.cDifMaker.Length)
                        Case 35  ' 原板区分
                            If init Then dgv(0, ii).Value = "原板区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cKenban) _
                            Else tMod.StringToByte(.data.CoilInf.cKenban, CStr(dgv(1, ii).Value), .data.CoilInf.cKenban.Length)
                        Case 36  ' メッキサイクル
                            If init Then dgv(0, ii).Value = "メッキサイクル" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cMekkiCycle) _
                            Else tMod.StringToByte(.data.CoilInf.cMekkiCycle, CStr(dgv(1, ii).Value), .data.CoilInf.cMekkiCycle.Length)
                        Case 37  ' レベラー_インラインSPM
                            If init Then dgv(0, ii).Value = "レベラー_インラインSPM" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cInLineSPM) _
                            Else tMod.StringToByte(.data.CoilInf.cInLineSPM, CStr(dgv(1, ii).Value), .data.CoilInf.cInLineSPM.Length)
                        Case 38  ' 化学処理塗油
                            If init Then dgv(0, ii).Value = "化学処理塗油" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cKagakuToyu) _
                            Else tMod.StringToByte(.data.CoilInf.cKagakuToyu, CStr(dgv(1, ii).Value), .data.CoilInf.cKagakuToyu.Length)
                        Case 39  ' オフラインSPM圧下率
                            If init Then dgv(0, ii).Value = "オフラインSPM圧下率" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cAtukkaSPM) _
                            Else tMod.StringToByte(.data.CoilInf.cAtukkaSPM, CStr(dgv(1, ii).Value), .data.CoilInf.cAtukkaSPM.Length)
                        Case 40  ' メッキ区分
                            If init Then dgv(0, ii).Value = "メッキ区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cMekkiKubun) _
                            Else tMod.StringToByte(.data.CoilInf.cMekkiKubun, CStr(dgv(1, ii).Value), .data.CoilInf.cMekkiKubun.Length)
                        Case 41  ' 表面処理1桁目
                            If init Then dgv(0, ii).Value = "表面処理1桁目" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cOmoteSyori1) _
                            Else tMod.StringToByte(.data.CoilInf.cOmoteSyori1, CStr(dgv(1, ii).Value), .data.CoilInf.cOmoteSyori1.Length)
                        Case 42  ' 表面処理2桁目
                            If init Then dgv(0, ii).Value = "表面処理2桁目" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cOmoteSyori2) _
                            Else tMod.StringToByte(.data.CoilInf.cOmoteSyori2, CStr(dgv(1, ii).Value), .data.CoilInf.cOmoteSyori2.Length)
                        Case 43  ' 塗油
                            If init Then dgv(0, ii).Value = "塗油" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cToyu) _
                            Else tMod.StringToByte(.data.CoilInf.cToyu, CStr(dgv(1, ii).Value), .data.CoilInf.cToyu.Length)
                        Case 44  ' 工程記号予備
                            If init Then dgv(0, ii).Value = "工程記号予備" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cKouteiYobi) _
                            Else tMod.StringToByte(.data.CoilInf.cKouteiYobi, CStr(dgv(1, ii).Value), .data.CoilInf.cKouteiYobi.Length)
                        Case 45  ' バウンダリ合わせ
                            If init Then dgv(0, ii).Value = "バウンダリ合わせ" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cBaund) _
                            Else tMod.StringToByte(.data.CoilInf.cBaund, CStr(dgv(1, ii).Value), .data.CoilInf.cBaund.Length)
                        Case 46  ' CAPL主材質
                            If init Then dgv(0, ii).Value = "CAPL主材質" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cCaplSyuZai) _
                            Else tMod.StringToByte(.data.CoilInf.cCaplSyuZai, CStr(dgv(1, ii).Value), .data.CoilInf.cCaplSyuZai.Length)
                        Case 47  ' 目付規格
                            If init Then dgv(0, ii).Value = "目付規格" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cMetuke) _
                            Else tMod.StringToByte(.data.CoilInf.cMetuke, CStr(dgv(1, ii).Value), .data.CoilInf.cMetuke.Length)
                        Case 48  ' Ni付着量
                            If init Then dgv(0, ii).Value = "Ni付着量" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cNi) _
                            Else tMod.StringToByte(.data.CoilInf.cNi, CStr(dgv(1, ii).Value), .data.CoilInf.cNi.Length)
                        Case 49  ' TCMロール粗度区分
                            If init Then dgv(0, ii).Value = "TCMロール粗度区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cSodoTCM) _
                            Else tMod.StringToByte(.data.CoilInf.cSodoTCM, CStr(dgv(1, ii).Value), .data.CoilInf.cSodoTCM.Length)
                        Case 50  ' SPM圧下有無
                            If init Then dgv(0, ii).Value = "SPM圧下有無" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cSpmUmu) _
                            Else tMod.StringToByte(.data.CoilInf.cSpmUmu, CStr(dgv(1, ii).Value), .data.CoilInf.cSpmUmu.Length)
                        Case 51  ' 計画次工程コード
                            If init Then dgv(0, ii).Value = "計画次工程コード" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cNextLine) _
                            Else tMod.StringToByte(.data.CoilInf.cNextLine, CStr(dgv(1, ii).Value), .data.CoilInf.cNextLine.Length)
                        Case 52  ' 向先
                            If init Then dgv(0, ii).Value = "向先" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cMuke) _
                            Else tMod.StringToByte(.data.CoilInf.cMuke, CStr(dgv(1, ii).Value), .data.CoilInf.cMuke.Length)
                        Case 53  ' 用途
                            If init Then dgv(0, ii).Value = "用途" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cYouto) _
                            Else tMod.StringToByte(.data.CoilInf.cYouto, CStr(dgv(1, ii).Value), .data.CoilInf.cYouto.Length)
                        Case 54  ' CAPL表面仕上げ
                            If init Then dgv(0, ii).Value = "CAPL表面仕上げ" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cCaplSiage) _
                            Else tMod.StringToByte(.data.CoilInf.cCaplSiage, CStr(dgv(1, ii).Value), .data.CoilInf.cCaplSiage.Length)
                        Case 55  ' CAPL酸洗有無
                            If init Then dgv(0, ii).Value = "CAPL酸洗有無" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cCaplSanUmu) _
                            Else tMod.StringToByte(.data.CoilInf.cCaplSanUmu, CStr(dgv(1, ii).Value), .data.CoilInf.cCaplSanUmu.Length)
                        Case 56  ' 品種区分
                            If init Then dgv(0, ii).Value = "品種区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cHinMode) _
                            Else tMod.StringToByte(.data.CoilInf.cHinMode, CStr(dgv(1, ii).Value), .data.CoilInf.cHinMode.Length)
                        Case 57  ' GA／CR区分
                            If init Then dgv(0, ii).Value = "GA／CR区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cGaCrKubun) _
                            Else tMod.StringToByte(.data.CoilInf.cGaCrKubun, CStr(dgv(1, ii).Value), .data.CoilInf.cGaCrKubun.Length)
                        Case 58  ' SPM圧下実績有無
                            If init Then dgv(0, ii).Value = "SPM圧下実績有無" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cSpmJisekiUmu) _
                            Else tMod.StringToByte(.data.CoilInf.cSpmJisekiUmu, CStr(dgv(1, ii).Value), .data.CoilInf.cSpmJisekiUmu.Length)
                        Case 59  ' 工程コード
                            If init Then dgv(0, ii).Value = "工程コード" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.CoilInf.cKouteiCode) _
                            Else tMod.StringToByte(.data.CoilInf.cKouteiCode, CStr(dgv(1, ii).Value), .data.CoilInf.cKouteiCode.Length)
                        Case 60  ' ２コイル目 命令No
                            If init Then dgv(0, ii).Value = "2ｺｲﾙ目 命令No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.typInCoil(0).cMeiNo) _
                            Else tMod.StringToByte(.data.typInCoil(0).cMeiNo, CStr(dgv(1, ii).Value), .data.typInCoil(0).cMeiNo.Length)
                        Case 61  ' ２コイル目 生産No
                            If init Then dgv(0, ii).Value = "2ｺｲﾙ目 生産No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.typInCoil(0).cCoilNo) _
                            Else tMod.StringToByte(.data.typInCoil(0).cCoilNo, CStr(dgv(1, ii).Value), .data.typInCoil(0).cCoilNo.Length)
                        Case 62  ' ２コイル目 入側フロント端板長さ
                            If init Then dgv(0, ii).Value = "2ｺｲﾙ目 入側ﾌﾛﾝﾄ端板長さ" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.typInCoil(0).nFrontLen _
                            Else .data.typInCoil(0).nFrontLen = CInt(dgv(1, ii).Value)
                        Case 63  ' ２コイル目 工程コード
                            If init Then dgv(0, ii).Value = "2ｺｲﾙ目 工程コード" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.typInCoil(0).cKouteiCode) _
                            Else tMod.StringToByte(.data.typInCoil(0).cKouteiCode, CStr(dgv(1, ii).Value), .data.typInCoil(0).cKouteiCode.Length)
                        Case 64  ' ３コイル目 命令No
                            If init Then dgv(0, ii).Value = "3ｺｲﾙ目 命令No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.typInCoil(1).cMeiNo) _
                            Else tMod.StringToByte(.data.typInCoil(1).cMeiNo, CStr(dgv(1, ii).Value), .data.typInCoil(1).cMeiNo.Length)
                        Case 65  ' ３コイル目 生産No
                            If init Then dgv(0, ii).Value = "3ｺｲﾙ目 生産No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.typInCoil(1).cCoilNo) _
                            Else tMod.StringToByte(.data.typInCoil(1).cCoilNo, CStr(dgv(1, ii).Value), .data.typInCoil(1).cCoilNo.Length)
                        Case 66  ' ３コイル目 入側フロント端板長さ
                            If init Then dgv(0, ii).Value = "3ｺｲﾙ目 入側ﾌﾛﾝﾄ端板長さ" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.typInCoil(1).nFrontLen _
                            Else .data.typInCoil(1).nFrontLen = CInt(dgv(1, ii).Value)
                        Case 67  ' ３コイル目 工程コード
                            If init Then dgv(0, ii).Value = "3ｺｲﾙ目 工程コード" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.typInCoil(1).cKouteiCode) _
                            Else tMod.StringToByte(.data.typInCoil(1).cKouteiCode, CStr(dgv(1, ii).Value), .data.typInCoil(1).cKouteiCode.Length)
                        Case 68  ' ４コイル目 命令No
                            If init Then dgv(0, ii).Value = "4ｺｲﾙ目 命令No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.typInCoil(2).cMeiNo) _
                            Else tMod.StringToByte(.data.typInCoil(2).cMeiNo, CStr(dgv(1, ii).Value), .data.typInCoil(2).cMeiNo.Length)
                        Case 69  ' ４コイル目 生産No
                            If init Then dgv(0, ii).Value = "4ｺｲﾙ目 生産No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.typInCoil(2).cCoilNo) _
                            Else tMod.StringToByte(.data.typInCoil(2).cCoilNo, CStr(dgv(1, ii).Value), .data.typInCoil(2).cCoilNo.Length)
                        Case 70  ' ４コイル目 入側フロント端板長さ
                            If init Then dgv(0, ii).Value = "4ｺｲﾙ目 入側ﾌﾛﾝﾄ端板長さ" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.typInCoil(2).nFrontLen _
                            Else .data.typInCoil(2).nFrontLen = CInt(dgv(1, ii).Value)
                        Case 71  ' ４コイル目 工程コード
                            If init Then dgv(0, ii).Value = "4ｺｲﾙ目 工程コード" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.typInCoil(2).cKouteiCode) _
                            Else tMod.StringToByte(.data.typInCoil(2).cKouteiCode, CStr(dgv(1, ii).Value), .data.typInCoil(2).cKouteiCode.Length)
                        Case 72  ' ５コイル目 命令No
                            If init Then dgv(0, ii).Value = "5ｺｲﾙ目 命令No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.typInCoil(3).cMeiNo) _
                            Else tMod.StringToByte(.data.typInCoil(3).cMeiNo, CStr(dgv(1, ii).Value), .data.typInCoil(3).cMeiNo.Length)
                        Case 73  ' ５コイル目 生産No
                            If init Then dgv(0, ii).Value = "5ｺｲﾙ目 生産No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.typInCoil(3).cCoilNo) _
                            Else tMod.StringToByte(.data.typInCoil(3).cCoilNo, CStr(dgv(1, ii).Value), .data.typInCoil(3).cCoilNo.Length)
                        Case 74  ' ５コイル目 入側フロント端板長さ
                            If init Then dgv(0, ii).Value = "5ｺｲﾙ目 入側ﾌﾛﾝﾄ端板長さ" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .data.typInCoil(3).nFrontLen _
                            Else .data.typInCoil(3).nFrontLen = CInt(dgv(1, ii).Value)
                        Case 75  ' ５コイル目 工程コード
                            If init Then dgv(0, ii).Value = "5ｺｲﾙ目 工程コード" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.typInCoil(3).cKouteiCode) _
                            Else tMod.StringToByte(.data.typInCoil(3).cKouteiCode, CStr(dgv(1, ii).Value), .data.typInCoil(2).cKouteiCode.Length)
                        Case 76  ' 予備
                            If init Then dgv(0, ii).Value = "予備" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.data.cYobi) _
                            Else tMod.StringToByte(.data.cYobi, CStr(dgv(1, ii).Value), .data.cYobi.Length)
                    End Select

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("受信データ部{0} エラー idx[{1}] Error [{2}]", CStr(IIf(flg, "表示", "読込")), ii, ex.Message)
                    txtInfo.Text = str

                End Try
            Next

            If Not init Then
                ' 初期化では無い時
                If Not flg Then
                    ' dgvから読み込み
                    ' typProCoilInf ⇒ val へセット
                    tMod.MemSet(typProCoilInf, m_typToCoilTbl.rec(rec).val, 0, tMod.SizeOf(typProCoilInf))
                End If

            End If

        End With

    End Sub


	''' <summary>
	''' dgvからデータを読み込む
	''' </summary>
	''' <remarks></remarks>
	Private Sub ReadFromDgv()
		Dim rec As Integer = cmbRecord.SelectedIndex

        tMod_Common.StringToByte(m_typToCoilTbl.rec(rec).basedata.cCoilKey, txtcID.Text, m_typToCoilTbl.rec(rec).basedata.cCoilKey.Length)

        CoilBaseData(rec, False, False)
		CoilSetting(rec, False, False)
		CoilVal(rec, False, False)
	End Sub

	''' <summary>
	''' 共有メモリの内容を表示する
	''' </summary>
	''' <remarks></remarks>
	Private Sub DispShmem()
		Dim rec As Integer = cmbRecord.SelectedIndex

        txtcID.Text = tMod_Common.ByteToString(m_typToCoilTbl.rec(rec).basedata.cCoilKey)

        CoilBaseData(rec, False, True)
		CoilSetting(rec, False, True)
		CoilVal(rec, False, True)

	End Sub

End Class