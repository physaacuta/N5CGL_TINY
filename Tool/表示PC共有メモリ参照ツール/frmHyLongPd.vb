Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary.tCnt

''' <summary>
''' ステータス情報
''' </summary>
''' <remarks></remarks>
Public Class frmHyLongPd
	'///////////////////////////////////////////////////////
	'メンバー変数
	'///////////////////////////////////////////////////////
	Private m_typHyLongPd As TBL_HY_LONGPD

	Private WithEvents m_tmAuto As New Timer
	Private WithEvents m_timer2 As New Timer
	'///////////////////////////////////////////////////////
	'メンバー関数
	'///////////////////////////////////////////////////////
	''' <summary>
	''' データグリッド初期化
	''' </summary>
	''' <param name="dgvCtrl"></param>
	''' <remarks></remarks>
	Private Sub InitdgvRec(ByVal dgvCtrl As tCnt_DataGridView)
		'初期設定
		dgvCtrl.dgvInitialize(dgvCtrl)

		'基本設定
		With dgvCtrl
			'+++++++++++++++++++++++++++
			'セル設定
			'+++++++++++++++++++++++++++
			.DefaultCellStyle().SelectionBackColor = Color.Transparent
			.DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
			'+++++++++++++++++++++++++++
			'行設定
			'+++++++++++++++++++++++++++
			.RowCount = TBL_HY_COIL_MAX
			.RowTemplate.Height = 20
			'+++++++++++++++++++++++++++
			'列設定
			'+++++++++++++++++++++++++++
			.AllowUserToResizeColumns = True
			.ColumnCount = 2
			.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
			.ColumnHeadersHeight = 20
			'初期設定
			For ii As Integer = 0 To .ColumnCount - 1
				.Columns(ii).HeaderText = ""
				.Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
				Select Case ii
					Case 0
						.Columns(ii).Width = 30
					Case 1
						.Columns(ii).Width = 150
					Case Else
						.Columns(ii).Width = 50
				End Select
				.Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
				.Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
			Next

			'個別設定
			.Columns(0).HeaderText = "REC"
			.Columns(1).HeaderText = "長周期発生フラグ"
		End With
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
	''' 共有メモリアクセス可能かチェック
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub m_timer2_tick(ByVal sender As Object, ByVal e As EventArgs) Handles m_timer2.Tick
		' 対象の共有メモリを定期的に読み込みしてみて、アクセスできるかどうか確認する。
		' あまり大きな共有メモリを読み込むと負荷になるので注意
		Dim nRetc As ValueType = 0
		Dim tmp As TBL_HY_LONGPD = CType(ksMemRead(TBL_HY_LONGPD_NAME, 0, GetType(TBL_HY_LONGPD), nRetc), TBL_HY_LONGPD)
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
	''' データ読み込み
	''' </summary>
	''' <remarks></remarks>
	Private Sub ReadData()
        Dim strTmp As String

        Try
			'共有メモリ読み込み
			m_typHyLongPd = CType(KizuLibCLI.KizuMod.ksMemRead(TBL_HY_LONGPD_NAME, GetType(TBL_HY_LONGPD)), TBL_HY_LONGPD)

			'データ表示
			With m_typHyLongPd
				'ステータス
				txtGuidanceFlag.Text = CStr(.nGuidanceFlag)

				'コイル単位
				For ii As Integer = 0 To dgvRec.RowCount - 1
					dgvRec(0, ii).Value = (ii + 1).ToString
					dgvRec(1, ii).Value = m_typHyLongPd.rec(ii).nLongPdOccurrenceFlag
				Next
			End With
		Catch ex As Exception
			strTmp = String.Format("データ読み込み異常[{0}]", ex.Message)
			'MsgBox(strTmp)
		End Try
	End Sub

	''' <summary>
	''' データ書き込み
	''' </summary>
	''' <remarks></remarks>
	Private Sub WriteData()
		Dim strTmp As String

		Try
			'データ取得
			With m_typHyLongPd
				'ステータス
				.nGuidanceFlag = CInt(txtGuidanceFlag.Text)

				For ii As Integer = 0 To dgvRec.RowCount - 1
					.rec(ii).nLongPdOccurrenceFlag = CInt(dgvRec(1, ii).Value)
				Next
			End With

			'共有メモリ書き込み
			If Not KizuLibCLI.KizuMod.ksMemWrite(TBL_HY_LONGPD_NAME, m_typHyLongPd) Then
				strTmp = String.Format("共有メモリ書き込み異常[{0}]", TBL_HY_LONGPD_NAME)
				MsgBox(strTmp)
			End If
		Catch ex As Exception
			strTmp = String.Format("データ書き込み異常[{0}]", ex.Message)
			MsgBox(strTmp)
		End Try
	End Sub

	'///////////////////////////////////////////////////////
	'イベント関数
	'///////////////////////////////////////////////////////
	''' <summary>
	''' 画面ロード
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmStatus_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
		'初期化
		InitdgvRec(dgvRec)

		' 共有メモリにアクセス可能かチェックタイマー
		m_timer2.Interval = 100         ' 100msec
		m_timer2.Start()

		' 自動更新を有効にする・・・必要ない場合はコメントにする
		chkUpdate.Checked = True

		'データ読み込み
		ReadData()
	End Sub

	''' <summary>
	''' 読み込みボタン
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnRead_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnRead.Click
		'データ読み込み
		ReadData()
	End Sub

	''' <summary>
	''' 書き込みボタン
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnWrite_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnWrite.Click
		'データ書き込み
		WriteData()
		'データ読み込み
		ReadData()
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
			m_tmAuto.Start()

			' 読み込み・書き込みボタンを無効にする
			btnRead.Enabled = False
			btnWrite.Enabled = False

		Else
			' チェックが解除された時
			chk.BackColor = SystemColors.Control
			m_tmAuto.Stop()

			If txtTitle.BackColor <> Color.Red Then
				' 共有メモリアクセス可能な場合
				btnRead.Enabled = True
				btnWrite.Enabled = True
			End If
		End If
	End Sub

	''' <summary>
	''' 自動更新処理
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub m_tmAuto_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles m_tmAuto.Tick
		'データ読み込み
		ReadData()
	End Sub

    Private Sub frmHyLongPd_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        m_tmAuto.Stop()
        m_tmAuto.Dispose()
    End Sub
End Class
