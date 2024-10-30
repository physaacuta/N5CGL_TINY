Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Runtime.InteropServices

''' <summary>
''' マップ位置
''' </summary>
''' <remarks></remarks>
Public Class frmHyMap
    '///////////////////////////////////////////////////////
    'メンバー変数
    '///////////////////////////////////////////////////////
    Private m_typHyMapRow As TBL_HY_MAP_ROW

    Private m_typMapRowMen As MAP_ROW_MEN

    Private m_bCmbFlg As Boolean

    Private m_nRec As Integer
    Private m_nStage As Integer
    Private m_nMen As Integer
    Private m_nSpm As Integer

    '///////////////////////////////////////////////////////
    'メンバー関数
    '///////////////////////////////////////////////////////
    ''' <summary>
    ''' マップ位置データグリッド初期化
    ''' </summary>
    ''' <param name="dgvCtrl"></param>
    ''' <remarks></remarks>
    Private Sub InitdgvHyMapRow(ByVal dgvCtrl As tCnt_DataGridView)
        '基本設定
        dgvCtrl.dgvInitialize(dgvCtrl)

        With dgvCtrl
            'スクロールバーの表示
            .ScrollBars = ScrollBars.Vertical

            '選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.Transparent
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '行設定
            .RowCount = MAP_ROW_NUM
            .RowTemplate.Height = 20

            '列設定
            .ColumnCount = 22
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 40
            For ii As Integer = 0 To .ColumnCount - 1
                .Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                .Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(ii).HeaderText = ""
                Select Case ii
                    Case 0
						.Columns(ii).Width = 35
                    Case 1, 2, 3, 4, 5, 6, 7, 8, 9
                        .Columns(ii).Width = 65
                    Case 10, 11, 12, 13
                        .Columns(ii).Width = 90
                    Case Else
                        .Columns(ii).Width = 50
                End Select
            Next ii
            .Columns(0).ReadOnly = True
            .Columns(0).Frozen = True

            .Columns(0).HeaderText = "" & vbCrLf & "№"
            .Columns(1).HeaderText = "受信状態"
            .Columns(2).HeaderText = "稼動状態"
            .Columns(3).HeaderText = "検査状態" & vbCrLf & "表示用"
            .Columns(4).HeaderText = "機器状態" & vbCrLf & "表示用"
            .Columns(5).HeaderText = "表示疵" & vbCrLf & "個数"
            .Columns(6).HeaderText = "入側距離"
            .Columns(7).HeaderText = "出側距離"
            .Columns(8).HeaderText = "ｶｯﾄ区分"
            .Columns(9).HeaderText = "付加情報"
            .Columns(10).HeaderText = "疵情報" & vbCrLf & "最小ﾎﾟｲﾝﾀ"
            .Columns(11).HeaderText = "疵情報" & vbCrLf & "最大ﾎﾟｲﾝﾀ"
            .Columns(12).HeaderText = "前工程疵情報" & vbCrLf & "最小ﾎﾟｲﾝﾀ"
            .Columns(13).HeaderText = "前工程疵情報" & vbCrLf & "最大ﾎﾟｲﾝﾀ"
        End With
    End Sub

    ''' <summary>
    ''' マップ位置読み込み
    ''' </summary>
    ''' <remarks></remarks>
    Private Function ReadHyMapRow() As Boolean
        Dim bRet As Boolean = False
        Dim strTmp As String

        m_nRec = cmbRecord.SelectedIndex
        m_nStage = cmbStage.SelectedIndex
        m_nMen = cmbMen.SelectedIndex

        Try
            Dim nReadAddr As Integer = Nothing                  '読み込みアドレス
            Dim nRetc As ValueType = 0                          'リターンコード

            '読み込みアドレス
            nReadAddr = SIZE_DSP_MAP_ROW * m_nRec + SIZE_MAP_ROW_STAGE * m_nStage + SIZE_MAP_ROW_MEN * m_nMen

            '詳細情報読み込み
            m_typMapRowMen = CType(ksMemRead(TBL_HY_MAP_ROW_NAME, nReadAddr, GetType(MAP_ROW_MEN), nRetc), MAP_ROW_MEN)
            If Not CBool(nRetc) Then
                Return bRet
            End If

            m_typHyMapRow = CType(ksMemRead(TBL_HY_MAP_ROW_NAME, GetType(TBL_HY_MAP_ROW)), TBL_HY_MAP_ROW)

            bRet = True

        Catch ex As Exception
            strTmp = String.Format("マップ位置読み込み異常[{0}]", ex.Message)
            MsgBox(strTmp)
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' マップ位置書き込み
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub WriteHyMapRow()
        Dim strTmp As String

        m_typHyMapRow.rec(m_nRec).stage(m_nStage).men(m_nMen) = m_typMapRowMen

        Try
            '共有メモリ書き込み
            If Not ksMemWrite(TBL_HY_MAP_ROW_NAME, m_typHyMapRow) Then
                strTmp = String.Format("共有メモリ書き込み異常[{0}]", TBL_HY_MAP_ROW_NAME)
                MsgBox(strTmp)
            End If
        Catch ex As Exception
            strTmp = String.Format("マップ位置書き込み異常[{0}]", ex.Message)
            MsgBox(strTmp)
        End Try
    End Sub

    ''' <summary>
    ''' マップ情報表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispMapInf()
        'コンボボックス設定中
        If m_bCmbFlg Then
            Exit Sub
        End If

        'マップデータ表示
        DspMap(0)
    End Sub

    ''' <summary>
    ''' マップ位置表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispHyMapRow()
        'マップデータ表示
        DspMap(2)
    End Sub

    ''' <summary>
    ''' マップデータ表示
    ''' </summary>
    ''' <param name="nTarget"></param>
    ''' <remarks></remarks>
	Private Sub DspMap(ByVal nTarget As Integer)
		Try
			Dim nRec As Integer = cmbRecord.SelectedIndex
			Dim nMen As Integer = cmbMen.SelectedIndex
            Dim nStage As Integer = cmbStage.SelectedIndex

            'データセットイベントが多段で発生するので一時抑制
            RemoveHandler dgvHyMapRow.CellValueChanged, AddressOf dgvHyMapRow_CellValueChanged

            For nRow As Integer = 0 To MAP_ROW_NUM - 1
                'マップ位置情報の表示
                If nTarget = 0 OrElse nTarget = 2 Then
                    For nCol As Integer = 0 To dgvHyMapRow.ColumnCount - 1
                        dgvHyMapRow(nCol, nRow).Style.BackColor = Color.White
                    Next
                    With m_typMapRowMen.row(nRow).typ
                        dgvHyMapRow(0, nRow).Value = nRow + 1
                        dgvHyMapRow(1, nRow).Value = CInt(.nEnable)
                        dgvHyMapRow(2, nRow).Value = CInt(.emKadou)
                        dgvHyMapRow(3, nRow).Value = CInt(.emDspKensa)
                        dgvHyMapRow(4, nRow).Value = CInt(.emDspKiki)
                        dgvHyMapRow(5, nRow).Value = .nKizuCnt
                        dgvHyMapRow(6, nRow).Value = .nLen_i
                        dgvHyMapRow(7, nRow).Value = .nLen_o
                        dgvHyMapRow(8, nRow).Value = .nCut
                        dgvHyMapRow(9, nRow).Value = .nKind
                        dgvHyMapRow(10, nRow).Value = .pnt(0)
                        dgvHyMapRow(11, nRow).Value = .pnt(1)
                        dgvHyMapRow(12, nRow).Value = .pntMae(0)
                        dgvHyMapRow(13, nRow).Value = .pntMae(1)
                    End With
                End If
            Next

            AddHandler dgvHyMapRow.CellValueChanged, AddressOf dgvHyMapRow_CellValueChanged

		Catch ex As Exception
            MsgBox("マップ位置表示エラー")
        End Try


	End Sub


    '///////////////////////////////////////////////////////////////////////////
    'イベント関数
    '///////////////////////////////////////////////////////////////////////////
    Private Sub frmMapInf_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Me.Cursor = Cursors.WaitCursor

        '初期化
        InitdgvHyMapRow(dgvHyMapRow)

        m_bCmbFlg = True                                    'コンボボックス設定開始
        'レコード
        For ii As Integer = 1 To TBL_HY_REC_MAX
            cmbRecord.Items.Add(CStr(ii))
        Next ii
        cmbRecord.SelectedIndex = 0
        'マップ切替
        cmbStage.Items.Add("最小間隔")
        cmbStage.Items.Add("間隔(小)")
        cmbStage.Items.Add("間隔(大)")
        cmbStage.SelectedIndex = 0
        '面
        cmbMen.Items.Add("表")
        cmbMen.Items.Add("裏")
        cmbMen.SelectedIndex = 0

        m_bCmbFlg = False                                   'コンボボックス設定終了

        'マップ情報読み込み
        If ReadHyMapRow() Then
            'マップ情報表示
            DispMapInf()
        End If

        ' 共有メモリサイズ情報表示
        Dim str As String = ""
        str = String.Format("DSP_TYP_ROW Size = {0}", Marshal.SizeOf(GetType(DSP_TYP_ROW))) & vbCrLf
        str &= String.Format("MAP_ROW_MEN Size = {0}", Marshal.SizeOf(GetType(MAP_ROW_MEN))) & vbCrLf
        str &= String.Format("MAP_ROW_STAGE Size = {0}", Marshal.SizeOf(GetType(MAP_ROW_STAGE))) & vbCrLf
        str &= String.Format("DSP_MAP_ROW Size = {0}", Marshal.SizeOf(GetType(DSP_MAP_ROW))) & vbCrLf
        str &= String.Format("TBL_HY_MAP_ROW Size = {0}", Marshal.SizeOf(GetType(TBL_HY_MAP_ROW))) & vbCrLf
        txtInfo.Text = str

        Me.Cursor = Cursors.Default
        Me.CenterToScreen()
    End Sub


    ''' <summary>
    ''' マップ位置読み込みボタン
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnReadHyMapRow_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnReadHyMapRow.Click
        'Me.Cursor = Cursors.WaitCursor
        If ReadHyMapRow() Then
            'マップ位置表示
            DispHyMapRow()
        End If
        'Me.Cursor = Cursors.Default
    End Sub

    ''' <summary>
    ''' マップ位置書き込みボタン
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnWriteHyMapRow_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnWriteHyMapRow.Click
        Me.Cursor = Cursors.WaitCursor
        'マップ位置書き込み
        WriteHyMapRow()
        Me.Cursor = Cursors.Default
    End Sub


    Private Sub dgvHyMapRow_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvHyMapRow.CellValueChanged
        Dim nCol As Integer = e.ColumnIndex
        Dim nRow As Integer = e.RowIndex
        Dim nTmp As Integer

        If nRow < 0 OrElse nCol < 0 Then
            Exit Sub
        End If

        If dgvHyMapRow(nCol, nRow).Value Is Nothing Then
            dgvHyMapRow(nCol, nRow).Value = 0
        End If

        If Not Integer.TryParse(CStr(dgvHyMapRow(nCol, nRow).Value), nTmp) Then
            dgvHyMapRow(nCol, nRow).Value = 0
        End If

        With m_typMapRowMen.row(nRow).typ
            Select Case e.ColumnIndex
                Case 1
                    .nEnable = CInt(dgvHyMapRow(nCol, nRow).Value)
                Case 2
                    .emKadou = CType(dgvHyMapRow(nCol, nRow).Value, EM_DIV_KADOU)
                Case 3
                    .emDspKensa = CType(dgvHyMapRow(nCol, nRow).Value, EM_DIV_KENSA)
                Case 4
                    .emDspKiki = CType(dgvHyMapRow(nCol, nRow).Value, EM_DIV_KIKI)
                Case 5
                    .nKizuCnt = CInt(dgvHyMapRow(nCol, nRow).Value)
                Case 6
                    .nLen_i = CInt(dgvHyMapRow(nCol, nRow).Value)
                Case 7
                    .nLen_o = CInt(dgvHyMapRow(nCol, nRow).Value)
                Case 8
                    .nCut = CInt(dgvHyMapRow(nCol, nRow).Value)
                Case 9
                    .nKind = CInt(dgvHyMapRow(nCol, nRow).Value)
                Case 10
                    .pnt(0) = CInt(dgvHyMapRow(nCol, nRow).Value)
                Case 11
                    .pnt(1) = CInt(dgvHyMapRow(nCol, nRow).Value)
                Case 12
                    .pntMae(0) = CInt(dgvHyMapRow(nCol, nRow).Value)
                Case 13
                    .pntMae(1) = CInt(dgvHyMapRow(nCol, nRow).Value)
            End Select
        End With

        DspMap(2)
    End Sub

    ''' <summary>
    ''' コンボボックス 変更イベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbRecord_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbRecord.SelectedIndexChanged, cmbStage.SelectedIndexChanged, cmbMen.SelectedIndexChanged
        Me.Cursor = Cursors.WaitCursor

        If m_bCmbFlg Then Exit Sub

        'マップ情報読み込み
        If ReadHyMapRow() Then
            'マップ情報表示
            DispMapInf()
        End If

        Me.Cursor = Cursors.Default
    End Sub

    Private WithEvents m_timer1 As New Timer

	Private Sub chkUpdate_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkUpdate.CheckedChanged
		Dim chk As CheckBox = CType(sender, CheckBox)

		If chk.Checked Then
			' チェックされた時
			chk.BackColor = Color.Cyan
			m_timer1.Interval = 500
			m_timer1.Start()
		Else
			' チェックが解除された時
			chk.BackColor = SystemColors.Control
			m_timer1.Stop()
		End If
	End Sub

	''' <summary>
	''' 自動更新処理
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub m_timer1_tick(ByVal sender As Object, ByVal e As EventArgs) Handles m_timer1.Tick
		btnReadHyMapRow.PerformClick()
	End Sub


    Private Sub frmHyMap_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        m_timer1.Stop()
        m_timer1.Dispose()
    End Sub
End Class
