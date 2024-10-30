Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Runtime.InteropServices

Public Class frmHyCoilInf
	'///////////////////////////////////////////////////////
	'メンバー変数
	'///////////////////////////////////////////////////////
	Private m_typHyCoilInf As TBL_HY_COILINF
	Private WithEvents m_timer1 As New Timer	' 表示自動更新用タイマー
	Private WithEvents m_timer2 As New Timer	' 共有メモリアクセス可・不可確認タイマー

	''' <summary>
	''' フォームロード
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmHyCoilInf_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
		' 表示自動更新タイマー周期設定
		m_timer1.Interval = 100			' 100msec

		' 共有メモリにアクセス可能かチェックタイマー
		m_timer2.Interval = 100			' 100msec
		m_timer2.Start()

		'レコードコンボボックス初期化
		cmbRecord.Items.Clear()
        For ii As Integer = 0 To TBL_HY_COIL_MAX - 1
            cmbRecord.Items.Add(CStr(ii + 1))
        Next
		cmbRecord.SelectedIndex = 0

        ' dgvの初期化
        InitDgv(dgvCoInfobase, 7)      ' コイル共通情報
        InitDgv(dgvCoilBaseData, 12)     ' システム共通のコイル情報
        InitDgv(dgvProCoilInf, 65)     ' プロコンのコイル情報

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
        str &= String.Format("COIL_BASEDATA Size = {0}", Marshal.SizeOf(GetType(COIL_BASEDATA))) & vbCrLf
        str &= String.Format("PRO_TYPE_DATA_COILINF Size = {0}", Marshal.SizeOf(GetType(PRO_TYPE_DATA_COILINF))) & vbCrLf
        str &= String.Format("COINFO_BASE Size = {0}", Marshal.SizeOf(GetType(COINFO_BASE))) & vbCrLf
		str &= String.Format("COILINF_REC Size = {0}", Marshal.SizeOf(GetType(COILINF_REC))) & vbCrLf
		str &= String.Format("TBL_HY_COILINF Size = {0}", Marshal.SizeOf(GetType(TBL_HY_COILINF))) & vbCrLf
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
                        .Columns(ii).Width = 169
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
	''' <remarks>共有メモリ⇒m_typHyCoilInf</remarks>
	Private Function ReadFromShmem() As Boolean
		Dim bRet As Boolean = False

		Try
			m_typHyCoilInf = CType(ksMemRead(TBL_HY_COILINF_NAME, GetType(TBL_HY_COILINF)), TBL_HY_COILINF)

			If m_typHyCoilInf.rec Is Nothing Then
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
			If Not ksMemWrite(TBL_HY_COILINF_NAME, m_typHyCoilInf) Then
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
	''' コイル共通情報を表示・読み込み
	''' </summary>
	''' <param name="rec"></param>
	''' <param name="flg">true:表示 false:読込</param>	
	''' <remarks></remarks>
	Private Sub CoInfobase(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

        If m_typHyCoilInf.rec Is Nothing Then
            Exit Sub
        End If

        ' 対象のdgvをセット
        Dim dgv As tCnt_DataGridView = dgvCoInfobase

		With m_typHyCoilInf.rec(rec).coilinf
			For ii As Integer = 0 To dgv.RowCount - 1
				Try
					Select Case ii
						Case 0	' 管理No
							If init Then dgv(0, ii).Value = "管理No" : dgv(1, ii).Value = "" _
							Else If flg Then dgv(1, ii).Value = tMod_Common.ByteToString(.cKizukenNo) _
							Else tMod_Common.StringToByte(.cKizukenNo, CStr(dgv(1, ii).Value), .cKizukenNo.Length)
						Case 1	' 検査表面状態(表)
							If init Then dgv(0, ii).Value = "検査表面状態(表)" : dgv(1, ii).Value = "" _
							Else If flg Then dgv(1, ii).Value = .scode(0).ToString _
							Else .scode(0) = CInt(dgv(1, ii).Value)
						Case 2	' 検査表面状態(裏)
							If init Then dgv(0, ii).Value = "検査表面状態(裏)" : dgv(1, ii).Value = "" _
							Else If flg Then dgv(1, ii).Value = .scode(1).ToString _
							Else .scode(1) = CInt(dgv(1, ii).Value)
						Case 3	' 検査パターン(表)
							If init Then dgv(0, ii).Value = "検査パターン(表)" : dgv(1, ii).Value = "" _
							Else If flg Then dgv(1, ii).Value = .pcode(0).ToString _
							Else .pcode(0) = CInt(dgv(1, ii).Value)
						Case 4	' 検査パターン(裏)
							If init Then dgv(0, ii).Value = "検査パターン(裏)" : dgv(1, ii).Value = "" _
							Else If flg Then dgv(1, ii).Value = .pcode(1).ToString _
                            Else .pcode(1) = CInt(dgv(1, ii).Value)
                        Case 5  ' マップ列数
                            If init Then dgv(0, ii).Value = "マップ列数" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nMapColNum.ToString _
                            Else .nMapColNum = CInt(dgv(1, ii).Value)
                        Case 6  ' 検査状態
                            If init Then dgv(0, ii).Value = "検査状態" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nCoilKensa.ToString _
                            Else .nCoilKensa = CInt(dgv(1, ii).Value)
                        Case 7  ' 予備
                            If init Then dgv(0, ii).Value = "予備" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod_Common.ByteToString(.yobi) _
                            Else tMod_Common.StringToByte(.yobi, CStr(dgv(1, ii).Value), .yobi.Length)
                    End Select

				Catch ex As Exception
					Dim str As String = txtInfo.Text
					str &= String.Format("コイル共通情報{0} エラー idx[{1}] Error [{2}]", CStr(IIf(flg, "表示", "読込")), ii, ex.Message)
					txtInfo.Text = str
				End Try
			Next
			dgv.ClearSelection()
		End With
	End Sub


	''' <summary>
	''' システム共通のコイル情報を表示・読み込み
	''' </summary>
	''' <param name="rec"></param>
	''' <param name="flg">true:表示 false:読込</param>	
	''' <remarks></remarks>
	Private Sub CoilBaseData(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

        If m_typHyCoilInf.rec Is Nothing Then
            Exit Sub
        End If

        ' 対象のdgvをセット
        Dim dgv As tCnt_DataGridView = dgvCoilBaseData

		With m_typHyCoilInf.rec(rec).coilinf.base
			For ii As Integer = 0 To dgv.RowCount - 1
				Try
                    Select Case ii
                        Case 0  ' 命令No
                            If init Then dgv(0, ii).Value = "命令No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cMeiNo) _
                            Else tMod.StringToByte(.cMeiNo, CStr(dgv(1, ii).Value), .cMeiNo.Length)
                        Case 1  ' 入側コイルNo
                            If init Then dgv(0, ii).Value = "入側コイルNo" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cCoilNoIn) _
                            Else tMod.StringToByte(.cCoilNoIn, CStr(dgv(1, ii).Value), .cCoilNoIn.Length)
                        Case 2  ' キー情報
                            If init Then dgv(0, ii).Value = "キー情報" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cCoilKey) _
                            Else tMod.StringToByte(.cCoilKey, CStr(dgv(1, ii).Value), .cCoilKey.Length)
                        Case 3  ' 予備
                            If init Then dgv(0, ii).Value = "予備" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cYobi1) _
                            Else tMod.StringToByte(.cYobi1, CStr(dgv(1, ii).Value), .cYobi1.Length)
                        Case 4  ' 入側コイル長[mm]
                            If init Then dgv(0, ii).Value = "入側コイル長[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nLenIn.ToString _
                            Else .nLenIn = CInt(dgv(1, ii).Value)
                        Case 5  ' 板厚[μm]
                            If init Then dgv(0, ii).Value = "材料板厚[μm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nAtu.ToString _
                            Else .nAtu = CInt(dgv(1, ii).Value)
                        Case 6  ' 板幅[mm](=通常上位の板幅だが、上位情報が無い場合等は、最大板幅をセットしておく)
                            If init Then dgv(0, ii).Value = "材料板幅[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nWid.ToString _
                            Else .nWid = CInt(dgv(1, ii).Value)
                        Case 7  ' 入側フロント端板長さ[mm]
                            If init Then dgv(0, ii).Value = "入側フロント端板長さ[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nCutLenF.ToString _
                            Else .nCutLenF = CInt(dgv(1, ii).Value)
                        Case 8  ' 入側テール端板長さ[mm]
                            If init Then dgv(0, ii).Value = "入側テール端板長さ[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nCutLenT.ToString _
                            Else .nCutLenT = CInt(dgv(1, ii).Value)
                        Case 9  ' 予備
                            If init Then dgv(0, ii).Value = "予備" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod_Common.ByteToString(.cYobi2) _
                            Else tMod.StringToByte(.cYobi2, CStr(dgv(1, ii).Value), .cYobi2.Length)
                        Case 10  ' SPM伸び率[10^2%]
                            If init Then dgv(0, ii).Value = "SPM伸び率[10^2%]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .dNobiSPM.ToString _
                            Else .dNobiSPM = CDbl(dgv(1, ii).Value)
                        Case 11  ' 出側コイル長[m]
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
					str &= String.Format("システム共通のコイル情報{0} エラー idx[{1}] Error [{2}]", CStr(IIf(flg, "表示", "読込")), ii, ex.Message)
					txtInfo.Text = str

				End Try
			Next
			dgv.ClearSelection()
		End With
	End Sub


    ''' <summary>
    ''' プロコンのコイル情報を表示・読み込み
    ''' </summary>
    ''' <param name="rec"></param>
    ''' <param name="flg">true:表示 false:読込</param>	
    ''' <remarks></remarks>
    Private Sub CoilProData(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

        If m_typHyCoilInf.rec Is Nothing Then
            Exit Sub
        End If

        ' 対象のdgvをセット
        Dim dgv As tCnt_DataGridView = dgvProCoilInf

        With m_typHyCoilInf.rec(rec).coilinf.data
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    Select Case ii
                        Case 0  ' 命令No
                            If init Then dgv(0, ii).Value = "命令No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.KeyInf.cMeiNo) _
                            Else tMod.StringToByte(.CoilInf.KeyInf.cMeiNo, CStr(dgv(1, ii).Value), .CoilInf.KeyInf.cMeiNo.Length)
                        Case 1  ' 生産No
                            If init Then dgv(0, ii).Value = "生産No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.KeyInf.cCoilNo) _
                            Else tMod.StringToByte(.CoilInf.KeyInf.cCoilNo, CStr(dgv(1, ii).Value), .CoilInf.KeyInf.cCoilNo.Length)
                        Case 2  ' 送信位置
                            If init Then dgv(0, ii).Value = "送信位置" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .CoilInf.nSendPoint _
                            Else .CoilInf.nSendPoint = CInt(dgv(1, ii).Value)
                        Case 3  ' 入側論理コイル長[m]
                            If init Then dgv(0, ii).Value = "入側論理コイル長[m]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .CoilInf.nLenIn _
                            Else .CoilInf.nLenIn = CInt(dgv(1, ii).Value)
                        Case 4  ' 材料板厚[μm]
                            If init Then dgv(0, ii).Value = "材料板厚[μm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .CoilInf.nAtuZai _
                            Else .CoilInf.nAtuZai = CInt(dgv(1, ii).Value)
                        Case 5  ' 材料板幅[mm]
                            If init Then dgv(0, ii).Value = "材料板幅[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .CoilInf.nWidZai _
                            Else .CoilInf.nWidZai = CInt(dgv(1, ii).Value)
                        Case 6  ' 注文板厚[μm]
                            If init Then dgv(0, ii).Value = "注文板厚[μm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .CoilInf.nAtuSei _
                            Else .CoilInf.nAtuSei = CInt(dgv(1, ii).Value)
                        Case 7  ' 注文板幅[mm]
                            If init Then dgv(0, ii).Value = "注文板幅[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .CoilInf.nWidSei _
                            Else .CoilInf.nWidSei = CInt(dgv(1, ii).Value)
                        Case 8  ' 現工程トリム幅[mm]
                            If init Then dgv(0, ii).Value = "現工程トリム幅[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .CoilInf.nWidTrim _
                            Else .CoilInf.nWidTrim = CInt(dgv(1, ii).Value)
                        Case 9  ' SPM伸び率[10-2%]
                            If init Then dgv(0, ii).Value = "SPM伸び率[10-2%]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .CoilInf.nNobiSPM _
                            Else .CoilInf.nNobiSPM = CInt(dgv(1, ii).Value)
                        Case 10  ' TLV伸び率[10-2%]
                            If init Then dgv(0, ii).Value = "TLV伸び率[10-2%]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .CoilInf.nNobiTLV _
                            Else .CoilInf.nNobiTLV = CInt(dgv(1, ii).Value)
                        Case 11  ' 入側フロント端板長さ[mm]
                            If init Then dgv(0, ii).Value = "入側フロント端板長さ[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .CoilInf.nCutF _
                            Else .CoilInf.nCutF = CInt(dgv(1, ii).Value)
                        Case 12  ' 入側テール端板長さ[mm]
                            If init Then dgv(0, ii).Value = "入側テール端板長さ[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .CoilInf.nCutT _
                            Else .CoilInf.nCutT = CInt(dgv(1, ii).Value)
                        Case 13  ' SPMワークロール径 (上)[10-1mm]
                            If init Then dgv(0, ii).Value = "SPMﾜｰｸﾛｰﾙ径(上)[10-1mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .CoilInf.nSPMRoolKei_Up _
                            Else .CoilInf.nSPMRoolKei_Up = CInt(dgv(1, ii).Value)
                        Case 14  ' SPMワークロール径 (下)[10-1mm]
                            If init Then dgv(0, ii).Value = "SPMﾜｰｸﾛｰﾙ径(下)[10-1mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .CoilInf.nSPMRoolKei_Down _
                            Else .CoilInf.nSPMRoolKei_Down = CInt(dgv(1, ii).Value)
                        Case 15  ' 検査基準_表
                            If init Then dgv(0, ii).Value = "検査基準_表" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cKenKijun_O) _
                            Else tMod.StringToByte(.CoilInf.cKenKijun_O, CStr(dgv(1, ii).Value), .CoilInf.cKenKijun_O.Length)
                        Case 16  ' 検査基準_裏
                            If init Then dgv(0, ii).Value = "検査基準_裏" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cKenKijun_U) _
                            Else tMod.StringToByte(.CoilInf.cKenKijun_U, CStr(dgv(1, ii).Value), .CoilInf.cKenKijun_U.Length)
                        Case 17  ' クロメート命令_種類
                            If init Then dgv(0, ii).Value = "クロメート命令_種類" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cKurometo_Kind) _
                            Else tMod.StringToByte(.CoilInf.cKurometo_Kind, CStr(dgv(1, ii).Value), .CoilInf.cKurometo_Kind.Length)
                        Case 18  ' クロメート命令_有無
                            If init Then dgv(0, ii).Value = "クロメート命令_有無" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cKurometo_Umu) _
                            Else tMod.StringToByte(.CoilInf.cKurometo_Umu, CStr(dgv(1, ii).Value), .CoilInf.cKurometo_Umu.Length)
                        Case 19  ' 品種
                            If init Then dgv(0, ii).Value = "品種" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cHinCode) _
                            Else tMod.StringToByte(.CoilInf.cHinCode, CStr(dgv(1, ii).Value), .CoilInf.cHinCode.Length)
                        Case 20  ' メッキ種類
                            If init Then dgv(0, ii).Value = "メッキ種類" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cMekkiKind) _
                            Else tMod.StringToByte(.CoilInf.cMekkiKind, CStr(dgv(1, ii).Value), .CoilInf.cMekkiKind.Length)
                        Case 21  ' 主材質
                            If init Then dgv(0, ii).Value = "主材質" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cSyuZai) _
                            Else tMod.StringToByte(.CoilInf.cSyuZai, CStr(dgv(1, ii).Value), .CoilInf.cSyuZai.Length)
                        Case 22  ' ディファレンシャルマーク
                            If init Then dgv(0, ii).Value = "ディファレンシャルマーク" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cDifMaker) _
                            Else tMod.StringToByte(.CoilInf.cDifMaker, CStr(dgv(1, ii).Value), .CoilInf.cDifMaker.Length)
                        Case 23  ' 原板区分
                            If init Then dgv(0, ii).Value = "原板区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cKenban) _
                            Else tMod.StringToByte(.CoilInf.cKenban, CStr(dgv(1, ii).Value), .CoilInf.cKenban.Length)
                        Case 24  ' メッキサイクル
                            If init Then dgv(0, ii).Value = "メッキサイクル" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cMekkiCycle) _
                            Else tMod.StringToByte(.CoilInf.cMekkiCycle, CStr(dgv(1, ii).Value), .CoilInf.cMekkiCycle.Length)
                        Case 25  ' レベラー_インラインSPM
                            If init Then dgv(0, ii).Value = "レベラー_インラインSPM" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cInLineSPM) _
                            Else tMod.StringToByte(.CoilInf.cInLineSPM, CStr(dgv(1, ii).Value), .CoilInf.cInLineSPM.Length)
                        Case 26  ' 化学処理塗油
                            If init Then dgv(0, ii).Value = "化学処理塗油" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cKagakuToyu) _
                            Else tMod.StringToByte(.CoilInf.cKagakuToyu, CStr(dgv(1, ii).Value), .CoilInf.cKagakuToyu.Length)
                        Case 27  ' オフラインSPM圧下率
                            If init Then dgv(0, ii).Value = "オフラインSPM圧下率" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cAtukkaSPM) _
                            Else tMod.StringToByte(.CoilInf.cAtukkaSPM, CStr(dgv(1, ii).Value), .CoilInf.cAtukkaSPM.Length)
                        Case 28  ' メッキ区分
                            If init Then dgv(0, ii).Value = "メッキ区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cMekkiKubun) _
                            Else tMod.StringToByte(.CoilInf.cMekkiKubun, CStr(dgv(1, ii).Value), .CoilInf.cMekkiKubun.Length)
                        Case 29  ' 表面処理1桁目
                            If init Then dgv(0, ii).Value = "表面処理1桁目" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cOmoteSyori1) _
                            Else tMod.StringToByte(.CoilInf.cOmoteSyori1, CStr(dgv(1, ii).Value), .CoilInf.cOmoteSyori1.Length)
                        Case 30  ' 表面処理2桁目
                            If init Then dgv(0, ii).Value = "表面処理2桁目" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cOmoteSyori2) _
                            Else tMod.StringToByte(.CoilInf.cOmoteSyori2, CStr(dgv(1, ii).Value), .CoilInf.cOmoteSyori2.Length)
                        Case 31  ' 塗油
                            If init Then dgv(0, ii).Value = "塗油" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cToyu) _
                            Else tMod.StringToByte(.CoilInf.cToyu, CStr(dgv(1, ii).Value), .CoilInf.cToyu.Length)
                        Case 32  ' 工程記号予備
                            If init Then dgv(0, ii).Value = "工程記号予備" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cKouteiYobi) _
                            Else tMod.StringToByte(.CoilInf.cKouteiYobi, CStr(dgv(1, ii).Value), .CoilInf.cKouteiYobi.Length)
                        Case 33  ' バウンダリ合わせ
                            If init Then dgv(0, ii).Value = "バウンダリ合わせ" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cBaund) _
                            Else tMod.StringToByte(.CoilInf.cBaund, CStr(dgv(1, ii).Value), .CoilInf.cBaund.Length)
                        Case 34  ' CAPL主材質
                            If init Then dgv(0, ii).Value = "CAPL主材質" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cCaplSyuZai) _
                            Else tMod.StringToByte(.CoilInf.cCaplSyuZai, CStr(dgv(1, ii).Value), .CoilInf.cCaplSyuZai.Length)
                        Case 35  ' 目付規格
                            If init Then dgv(0, ii).Value = "目付規格" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cMetuke) _
                            Else tMod.StringToByte(.CoilInf.cMetuke, CStr(dgv(1, ii).Value), .CoilInf.cMetuke.Length)
                        Case 36  ' Ni付着量
                            If init Then dgv(0, ii).Value = "Ni付着量" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cNi) _
                            Else tMod.StringToByte(.CoilInf.cNi, CStr(dgv(1, ii).Value), .CoilInf.cNi.Length)
                        Case 37  ' TCMロール粗度区分
                            If init Then dgv(0, ii).Value = "TCMロール粗度区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cSodoTCM) _
                            Else tMod.StringToByte(.CoilInf.cSodoTCM, CStr(dgv(1, ii).Value), .CoilInf.cSodoTCM.Length)
                        Case 38  ' SPM圧下有無
                            If init Then dgv(0, ii).Value = "SPM圧下有無" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cSpmUmu) _
                            Else tMod.StringToByte(.CoilInf.cSpmUmu, CStr(dgv(1, ii).Value), .CoilInf.cSpmUmu.Length)
                        Case 39  ' 計画次工程コード
                            If init Then dgv(0, ii).Value = "計画次工程コード" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cNextLine) _
                            Else tMod.StringToByte(.CoilInf.cNextLine, CStr(dgv(1, ii).Value), .CoilInf.cNextLine.Length)
                        Case 40  ' 向先
                            If init Then dgv(0, ii).Value = "向先" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cMuke) _
                            Else tMod.StringToByte(.CoilInf.cMuke, CStr(dgv(1, ii).Value), .CoilInf.cMuke.Length)
                        Case 41  ' 用途
                            If init Then dgv(0, ii).Value = "用途" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cYouto) _
                            Else tMod.StringToByte(.CoilInf.cYouto, CStr(dgv(1, ii).Value), .CoilInf.cYouto.Length)
                        Case 42  ' CAPL表面仕上げ
                            If init Then dgv(0, ii).Value = "CAPL表面仕上げ" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cCaplSiage) _
                            Else tMod.StringToByte(.CoilInf.cCaplSiage, CStr(dgv(1, ii).Value), .CoilInf.cCaplSiage.Length)
                        Case 43  ' CAPL酸洗有無
                            If init Then dgv(0, ii).Value = "CAPL酸洗有無" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cCaplSanUmu) _
                            Else tMod.StringToByte(.CoilInf.cCaplSanUmu, CStr(dgv(1, ii).Value), .CoilInf.cCaplSanUmu.Length)
                        Case 44  ' 品種区分
                            If init Then dgv(0, ii).Value = "品種区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cHinMode) _
                            Else tMod.StringToByte(.CoilInf.cHinMode, CStr(dgv(1, ii).Value), .CoilInf.cHinMode.Length)
                        Case 45  ' GA／CR区分
                            If init Then dgv(0, ii).Value = "GA／CR区分" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cGaCrKubun) _
                            Else tMod.StringToByte(.CoilInf.cGaCrKubun, CStr(dgv(1, ii).Value), .CoilInf.cGaCrKubun.Length)
                        Case 46  ' SPM圧下実績有無
                            If init Then dgv(0, ii).Value = "SPM圧下実績有無" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cSpmJisekiUmu) _
                            Else tMod.StringToByte(.CoilInf.cSpmJisekiUmu, CStr(dgv(1, ii).Value), .CoilInf.cSpmJisekiUmu.Length)
                        Case 47  ' 工程コード
                            If init Then dgv(0, ii).Value = "工程コード" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.CoilInf.cKouteiCode) _
                            Else tMod.StringToByte(.CoilInf.cKouteiCode, CStr(dgv(1, ii).Value), .CoilInf.cKouteiCode.Length)
                        Case 48  ' ２コイル目 命令No
                            If init Then dgv(0, ii).Value = "2ｺｲﾙ目 命令No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.typInCoil(0).cMeiNo) _
                            Else tMod.StringToByte(.typInCoil(0).cMeiNo, CStr(dgv(1, ii).Value), .typInCoil(0).cMeiNo.Length)
                        Case 49  ' ２コイル目 生産No
                            If init Then dgv(0, ii).Value = "2ｺｲﾙ目 生産No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.typInCoil(0).cCoilNo) _
                            Else tMod.StringToByte(.typInCoil(0).cCoilNo, CStr(dgv(1, ii).Value), .typInCoil(0).cCoilNo.Length)
                        Case 50  ' ２コイル目 入側フロント端板長さ
                            If init Then dgv(0, ii).Value = "2ｺｲﾙ目 入側ﾌﾛﾝﾄ端板長さ" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .typInCoil(0).nFrontLen _
                            Else .typInCoil(0).nFrontLen = CInt(dgv(1, ii).Value)
                        Case 51  ' ２コイル目 工程コード
                            If init Then dgv(0, ii).Value = "2ｺｲﾙ目 工程コード" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.typInCoil(0).cKouteiCode) _
                            Else tMod.StringToByte(.typInCoil(0).cKouteiCode, CStr(dgv(1, ii).Value), .typInCoil(0).cKouteiCode.Length)
                        Case 52  ' ３コイル目 命令No
                            If init Then dgv(0, ii).Value = "3ｺｲﾙ目 命令No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.typInCoil(1).cMeiNo) _
                            Else tMod.StringToByte(.typInCoil(1).cMeiNo, CStr(dgv(1, ii).Value), .typInCoil(1).cMeiNo.Length)
                        Case 53  ' ３コイル目 生産No
                            If init Then dgv(0, ii).Value = "3ｺｲﾙ目 生産No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.typInCoil(1).cCoilNo) _
                            Else tMod.StringToByte(.typInCoil(1).cCoilNo, CStr(dgv(1, ii).Value), .typInCoil(1).cCoilNo.Length)
                        Case 54  ' ３コイル目 入側フロント端板長さ
                            If init Then dgv(0, ii).Value = "3ｺｲﾙ目 入側ﾌﾛﾝﾄ端板長さ" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .typInCoil(1).nFrontLen _
                            Else .typInCoil(1).nFrontLen = CInt(dgv(1, ii).Value)
                        Case 55  ' ３コイル目 工程コード
                            If init Then dgv(0, ii).Value = "3ｺｲﾙ目 工程コード" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.typInCoil(1).cKouteiCode) _
                            Else tMod.StringToByte(.typInCoil(1).cKouteiCode, CStr(dgv(1, ii).Value), .typInCoil(1).cKouteiCode.Length)
                        Case 56  ' ４コイル目 命令No
                            If init Then dgv(0, ii).Value = "4ｺｲﾙ目 命令No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.typInCoil(2).cMeiNo) _
                            Else tMod.StringToByte(.typInCoil(2).cMeiNo, CStr(dgv(1, ii).Value), .typInCoil(2).cMeiNo.Length)
                        Case 57  ' ４コイル目 生産No
                            If init Then dgv(0, ii).Value = "4ｺｲﾙ目 生産No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.typInCoil(2).cCoilNo) _
                            Else tMod.StringToByte(.typInCoil(2).cCoilNo, CStr(dgv(1, ii).Value), .typInCoil(2).cCoilNo.Length)
                        Case 58  ' ４コイル目 入側フロント端板長さ
                            If init Then dgv(0, ii).Value = "4ｺｲﾙ目 入側ﾌﾛﾝﾄ端板長さ" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .typInCoil(2).nFrontLen _
                            Else .typInCoil(2).nFrontLen = CInt(dgv(1, ii).Value)
                        Case 59  ' ４コイル目 工程コード
                            If init Then dgv(0, ii).Value = "4ｺｲﾙ目 工程コード" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.typInCoil(2).cKouteiCode) _
                            Else tMod.StringToByte(.typInCoil(2).cKouteiCode, CStr(dgv(1, ii).Value), .typInCoil(2).cKouteiCode.Length)
                        Case 60  ' ５コイル目 命令No
                            If init Then dgv(0, ii).Value = "5ｺｲﾙ目 命令No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.typInCoil(3).cMeiNo) _
                            Else tMod.StringToByte(.typInCoil(3).cMeiNo, CStr(dgv(1, ii).Value), .typInCoil(3).cMeiNo.Length)
                        Case 61  ' ５コイル目 生産No
                            If init Then dgv(0, ii).Value = "5ｺｲﾙ目 生産No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.typInCoil(3).cCoilNo) _
                            Else tMod.StringToByte(.typInCoil(3).cCoilNo, CStr(dgv(1, ii).Value), .typInCoil(3).cCoilNo.Length)
                        Case 62  ' ５コイル目 入側フロント端板長さ
                            If init Then dgv(0, ii).Value = "5ｺｲﾙ目 入側ﾌﾛﾝﾄ端板長さ" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .typInCoil(3).nFrontLen _
                            Else .typInCoil(3).nFrontLen = CInt(dgv(1, ii).Value)
                        Case 63  ' ５コイル目 工程コード
                            If init Then dgv(0, ii).Value = "5ｺｲﾙ目 工程コード" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.typInCoil(3).cKouteiCode) _
                            Else tMod.StringToByte(.typInCoil(3).cKouteiCode, CStr(dgv(1, ii).Value), .typInCoil(2).cKouteiCode.Length)
                        Case 64  ' 予備
                            If init Then dgv(0, ii).Value = "予備" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cYobi) _
                            Else tMod.StringToByte(.cYobi, CStr(dgv(1, ii).Value), .cYobi.Length)
                    End Select

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("プロコンのコイル情報{0} エラー idx[{1}] Error [{2}]", CStr(IIf(flg, "表示", "読込")), ii, ex.Message)
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

        ' コイル情報共通
        CoInfobase(0, True, True)
        ' システム共通のコイル情報
        CoilBaseData(0, True, True)
        ' プロコンのコイル情報
        CoilProData(0, True, True)
    End Sub

	''' <summary>
	''' dgvからデータを読み込む
	''' </summary>
	''' <remarks></remarks>
	Private Sub ReadFromDgv()
        Dim rec As Integer = cmbRecord.SelectedIndex

        ' コイル情報共通
        CoInfobase(rec, False, False)
		' システム共通のコイル情報
		CoilBaseData(rec, False, False)
        ' プロコンのコイル情報
        CoilProData(rec, False, False)

    End Sub

	''' <summary>
	''' 共有メモリの内容を表示する
	''' </summary>
	''' <remarks></remarks>
	Private Sub DispShmem()
        Dim rec As Integer = cmbRecord.SelectedIndex

        ' コイル情報共通
        CoInfobase(rec, False, True)
		' システム共通のコイル情報
		CoilBaseData(rec, False, True)
        ' プロコンのコイル情報
        CoilProData(rec, False, True)

    End Sub

    ''' <summary>
    ''' 画面表示状態のテキストを返す
    ''' </summary>
    ''' <param name="emDispStatus"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetDispStatusText(ByVal emDispStatus As EM_DIV_DSP_STATUS) As String
        Dim strRet As String = ""
        Select Case emDispStatus
            Case EM_DIV_DSP_STATUS.DIV_DSP_INITIALIZE
                strRet = "データなし"
            Case EM_DIV_DSP_STATUS.DIV_DSP_NG
                strRet = "表示不可"
            Case EM_DIV_DSP_STATUS.DIV_DSP_OK
                strRet = "表示可"
            Case EM_DIV_DSP_STATUS.DIV_DSP_STANDBY
                strRet = "表示予定"
        End Select
        Return strRet
    End Function

    Private Sub frmHyCoilInf_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        m_timer1.Stop()
        m_timer1.Dispose()
    End Sub
End Class