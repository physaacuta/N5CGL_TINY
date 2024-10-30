Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary.tCnt

''' <summary>
''' ステータス情報
''' </summary>
''' <remarks></remarks>
Public Class frmHyStatus
    '///////////////////////////////////////////////////////
    'メンバー変数
    '///////////////////////////////////////////////////////
    Private m_typHyStatus As TBL_HY_STATUS

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
    Private Sub InitdgvNgInf(ByVal dgvCtrl As tCnt_DataGridView)
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
            .RowCount = 5
            .RowTemplate.Height = 20
            '+++++++++++++++++++++++++++
            '列設定
            '+++++++++++++++++++++++++++
            .AllowUserToResizeColumns = True
            .ColumnCount = 9
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20
            '初期設定
            For ii As Integer = 0 To .ColumnCount - 1
                .Columns(ii).HeaderText = ""
				.Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
				Select Case ii
					Case 2
						.Columns(ii).Width = 65
					Case 3, 4
						.Columns(ii).Width = 90
					Case 6
						.Columns(ii).Width = 140
					Case Else
						.Columns(ii).Width = 55
				End Select
				.Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            Next
            '個別設定
            .Columns(0).HeaderText = "機器ID"
            .Columns(1).HeaderText = "PCID"
            .Columns(2).HeaderText = "異常範囲"
            .Columns(3).HeaderText = "検査異常度"
            .Columns(4).HeaderText = "機器異常度"
            .Columns(5).HeaderText = "優先度"
            .Columns(6).HeaderText = "異常時のメッセージ"
            .Columns(6).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(7).HeaderText = "PC区分"
            .Columns(8).HeaderText = "現状態"
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
		Dim tmp As TBL_HY_STATUS = CType(ksMemRead(TBL_HY_STATUS_NAME, 0, GetType(TBL_HY_STATUS), nRetc), TBL_HY_STATUS)
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
	''' 運転状態のテキストを返す
	''' </summary>
	''' <param name="emUnten"></param>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function GetUntenText(ByVal emUnten As EM_DIV_UNTEN) As String
		Dim strRet As String = ""
		Select Case emUnten
			Case EM_DIV_UNTEN.DIV_UNTEN_CAMERA
				strRet = "校正モード(カメラ調整)"
			Case EM_DIV_UNTEN.DIV_UNTEN_NOMAL
				strRet = "通常運転モード"
			Case EM_DIV_UNTEN.DIV_UNTEN_SAMP
				strRet = "テスト運転モード(サンプル撮影)"
			Case EM_DIV_UNTEN.DIV_UNTEN_SMYU
				strRet = "シミュレーションモード"
			Case EM_DIV_UNTEN.DIV_UNTEN_STOP
				strRet = "停止モード"
		End Select
		Return strRet
	End Function

	''' <summary>
	''' 稼動状態のテキストを返す
	''' </summary>
	''' <param name="emKadou"></param>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function GetKadouText(ByVal emKadou As EM_DIV_KADOU) As String
		Dim strRet As String = ""
		Select Case emKadou
			Case EM_DIV_KADOU.DIV_KADOU_INIT
				strRet = "初期化中"
			Case EM_DIV_KADOU.DIV_KADOU_RETRY
				strRet = "リトライ中"
			Case EM_DIV_KADOU.DIV_KADOU_START
				strRet = "稼動中&テスト中"
			Case EM_DIV_KADOU.DIV_KADOU_STOP
				strRet = "停止中"
		End Select
		Return strRet
	End Function

	''' <summary>
	''' 検査状態のテキストを返す
	''' </summary>
	''' <param name="emKensa"></param>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function GetKensaText(ByVal emKensa As EM_DIV_KENSA) As String
		Dim strRet As String = ""
		Select Case emKensa
			Case EM_DIV_KENSA.DIV_KENSA_NON
				strRet = "停止(対象外)"
			Case EM_DIV_KENSA.DIV_KENSA_OK
				strRet = "正常"
			Case EM_DIV_KENSA.DIV_KENSA_NG
				strRet = "縮退"
			Case EM_DIV_KENSA.DIV_KENSA_MENTE
				strRet = "メンテ中(プロビジョン)"
			Case EM_DIV_KENSA.DIV_KENSA_EDGENG
				strRet = "エッジ不正"
			Case EM_DIV_KENSA.DIV_KENSA_DBNON
				strRet = "DB実績未保存"
			Case EM_DIV_KENSA.DIV_KENSA_SKIP
				strRet = "未検査区間"
			Case EM_DIV_KENSA.DIV_KENSA_STOP
				strRet = "停止(片面異常の場合、検査停止中)"
		End Select
		Return strRet

	End Function

    ''' <summary>
    ''' 機器異常度のテキストを返す
    ''' </summary>
    ''' <param name="emKiki"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetKikiText(ByVal emKiki As EM_DIV_KIKI) As String
        Dim strRet As String = ""
        Select Case emKiki
            Case EM_DIV_KIKI.DIV_KIKI_OK
                strRet = "正常"
            Case EM_DIV_KIKI.DIV_KIKI_KEI
                strRet = "軽故障"
            Case EM_DIV_KIKI.DIV_KIKI_TYU
                strRet = "中故障(プロビジョン)"
            Case EM_DIV_KIKI.DIV_KIKI_JYU
                strRet = "重故障"
        End Select
        Return strRet
    End Function

    ''' <summary>
    ''' 回線状態のテキストを返す
    ''' </summary>
    ''' <param name="nSt"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetConnectText(ByVal nSt As Integer) As String
        Dim strRet As String = ""
        Select Case nSt
            Case 0
                strRet = "正常"
            Case 1
                strRet = "異常"
        End Select
        Return strRet
    End Function


    ''' <summary>
    ''' データ読み込み
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadData()
        Dim strTmp As String

        Try
            '共有メモリ読み込み
            m_typHyStatus = CType(KizuLibCLI.KizuMod.ksMemRead(TBL_HY_STATUS_NAME, GetType(TBL_HY_STATUS)), TBL_HY_STATUS)

            'データ表示
            With m_typHyStatus
                'ステータス
                txtUnten.Text = CStr(.emUnten)
                txtUntenText.Text = GetUntenText(.emUnten)

                txtKadou.Text = CStr(.emKadou)
                txtKadouText.Text = GetKadouText(.emKadou)

                ' ステータス 検査異常
                txtKensa0.Text = CStr(.status.st_kensa(0))
                txtKensa0Text.Text = GetKensaText(.status.st_kensa(0))
                txtKensa1.Text = CStr(.status.st_kensa(1))
                txtKensa1Text.Text = GetKensaText(.status.st_kensa(1))

                ' 機器異常
                txtKiki0.Text = CStr(.status.st_kiki(0))
                txtKiki0Text.Text = GetKikiText(.status.st_kiki(0))
                txtKiki1.Text = CStr(.status.st_kiki(1))
                txtKiki1Text.Text = GetKikiText(.status.st_kiki(1))

                ' 異常セット件数
                txtCnt.Text = CStr(.status.nCnt)

                '機器情報
                For ii As Integer = 0 To dgvNgInf.RowCount - 1
                    dgvNgInf(0, ii).Value = .status.typErr(ii).id
                    dgvNgInf(1, ii).Value = .status.typErr(ii).pcid
                    dgvNgInf(2, ii).Value = .status.typErr(ii).area
                    dgvNgInf(3, ii).Value = CInt(.status.typErr(ii).Kensa)
                    dgvNgInf(4, ii).Value = CInt(.status.typErr(ii).Kiki)
                    dgvNgInf(5, ii).Value = .status.typErr(ii).yusen
                    dgvNgInf(6, ii).Value = .status.typErr(ii).msg
                    dgvNgInf(7, ii).Value = .status.typErr(ii).pcmode
                    dgvNgInf(8, ii).Value = .status.typErr(ii).stat
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
            With m_typHyStatus
                'ステータス
                .emUnten = CType(txtUnten.Text, EM_DIV_UNTEN)
                .emKadou = CType(txtKadou.Text, EM_DIV_KADOU)

                .status.st_kensa(0) = CType(txtKensa0.Text, EM_DIV_KENSA)
                .status.st_kensa(1) = CType(txtKensa1.Text, EM_DIV_KENSA)
                .status.st_kiki(0) = CType(txtKiki0.Text, EM_DIV_KIKI)
                .status.st_kiki(1) = CType(txtKiki1.Text, EM_DIV_KIKI)
                .status.nCnt = CInt(txtCnt.Text)

                '機器情報
                For ii As Integer = 0 To dgvNgInf.RowCount - 1
                    .status.typErr(ii).id = CInt(dgvNgInf(0, ii).Value)
                    .status.typErr(ii).pcid = CInt(dgvNgInf(1, ii).Value)
                    .status.typErr(ii).area = CInt(dgvNgInf(2, ii).Value)
                    .status.typErr(ii).Kensa = CType(dgvNgInf(3, ii).Value, EM_DIV_KENSA)
                    .status.typErr(ii).Kiki = CType(dgvNgInf(4, ii).Value, EM_DIV_KIKI)
                    .status.typErr(ii).yusen = CInt(dgvNgInf(5, ii).Value)
                    .status.typErr(ii).msg = CStr(dgvNgInf(6, ii).Value)
                    .status.typErr(ii).pcmode = CInt(dgvNgInf(7, ii).Value)
                    .status.typErr(ii).stat = CInt(dgvNgInf(8, ii).Value)
                Next
            End With

            '共有メモリ書き込み
            If Not KizuLibCLI.KizuMod.ksMemWrite(TBL_HY_STATUS_NAME, m_typHyStatus) Then
                strTmp = String.Format("共有メモリ書き込み異常[{0}]", TBL_HY_STATUS_NAME)
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
        InitdgvNgInf(dgvNgInf)

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
    ''' 運転状態テキスト変更イベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub txtUnten_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtUnten.TextChanged
        If IsNumeric(txtUnten.Text) Then
            Dim emUnten As EM_DIV_UNTEN = CType(txtUnten.Text, EM_DIV_UNTEN)
            txtUntenText.Text = GetUntenText(emUnten)
        Else
            txtUntenText.Text = ""
        End If
    End Sub

    ''' <summary>
    ''' 稼動状態テキスト変更イベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub txtKadou_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtKadou.TextChanged
        If IsNumeric(txtKadou.Text) Then
            Dim emKadou As EM_DIV_KADOU = CType(txtKadou.Text, EM_DIV_KADOU)
            txtKadouText.Text = GetKadouText(emKadou)
        Else
            txtKadouText.Text = ""
        End If
    End Sub





    ''' <summary>
    ''' 検査異常度テキスト変更イベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub txtKensa0_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtKensa0.TextChanged, txtKensa1.TextChanged
        Dim txtBox As TextBox

        If sender Is txtKensa0 Then
            txtBox = txtKensa0Text
        ElseIf sender Is txtKensa1 Then
            txtBox = txtKensa1Text
        Else
            Exit Sub
        End If

        If IsNumeric(CType(sender, TextBox).Text) Then
            Dim emKensa As EM_DIV_KENSA = CType(CType(sender, TextBox).Text, EM_DIV_KENSA)
            txtBox.Text = GetKensaText(emKensa)
        Else
            txtBox.Text = ""
        End If

    End Sub

    ''' <summary>
    ''' 機器異常度テキスト変更イベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub txtKiki0_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtKiki0.TextChanged, txtKiki1.TextChanged
        Dim txtBox As TextBox

        If sender Is txtKiki0 Then
            txtBox = txtKiki0Text
        ElseIf sender Is txtKiki1 Then
            txtBox = txtKiki1Text
        Else
            Exit Sub
        End If

        If IsNumeric(CType(sender, TextBox).Text) Then
            Dim emKiki As EM_DIV_KIKI = CType(CType(sender, TextBox).Text, EM_DIV_KIKI)
            txtBox.Text = GetKikiText(emKiki)
        Else
            txtBox.Text = ""
        End If
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

    Private Sub frmHyStatus_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        m_tmAuto.Stop()
        m_tmAuto.Dispose()
    End Sub
End Class
