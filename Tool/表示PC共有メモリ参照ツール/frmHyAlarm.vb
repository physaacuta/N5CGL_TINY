Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary.tCnt

''' <summary>
''' 警報情報
''' </summary>
''' <remarks></remarks>
Public Class frmHyAlarm
    '///////////////////////////////////////////////////////
    'メンバー変数
    '///////////////////////////////////////////////////////
    Private m_typHyAlarm As TBL_HY_ALARM

    '///////////////////////////////////////////////////////
    'メンバー関数
    '///////////////////////////////////////////////////////
    ''' <summary>
    ''' データグリッド初期化
    ''' </summary>
    ''' <param name="dgvCtrl">データグリッドコントロール</param>
    ''' <remarks></remarks>
    Private Sub Initdgv(ByRef dgvCtrl As tCnt_DataGridView)
        '基本設定
        dgvCtrl.dgvInitialize(dgvCtrl)

        '個別設定
        With dgvCtrl
            'セル設定
            .DefaultCellStyle().SelectionBackColor = Color.Transparent
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '行設定
            .RowCount = 2 * NUM_MEN              '行数
            .RowTemplate.Height = 20

            '列設定
			.ColumnCount = ALARM_COL_NUM + 2				  '列数
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20

            '出力位置
            .Columns(0).HeaderText = "出力位置"
            .Columns(0).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(0).Width = 120
            .Columns(0).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(0).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .Columns(0).ReadOnly = True

            '面
            .Columns(1).HeaderText = "面"
            .Columns(1).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(1).Width = 45
            .Columns(1).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(1).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .Columns(1).ReadOnly = True

            'ランプ分割数
            For ii As Integer = 0 To ALARM_COL_NUM - 1
				.Columns(2 + ii).HeaderText = CStr(ii + 1)
				.Columns(2 + ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
				.Columns(2 + ii).Width = 45
				.Columns(2 + ii).SortMode = DataGridViewColumnSortMode.NotSortable
				.Columns(2 + ii).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
			Next ii
        End With
    End Sub

    ''' <summary>
    ''' データ読み込み
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadData()
        Dim nRow As Integer = 0
        Dim strTmp As String

        Try
            '共有メモリ読み込み
            Dim nRetc As ValueType = 0
            m_typHyAlarm = CType(ksMemRead(TBL_HY_ALARM_NAME, 0, GetType(TBL_HY_ALARM), nRetc), TBL_HY_ALARM)

            'データ表示
            For ii As Integer = 0 To 2 - 1           '出力位置
                    For jj As Integer = 0 To NUM_MEN - 1            '面

                        If ii = 0 Then
                            dgvFlag(0, nRow).Value = "表検査台位置"
                        Else
                            dgvFlag(0, nRow).Value = "裏検査台位置"
                        End If

                        If jj = 0 Then
                            dgvFlag(1, nRow).Value = "表"
                        Else
                            dgvFlag(1, nRow).Value = "裏"
                        End If

                    For kk As Integer = 0 To ALARM_COL_NUM - 1

                        If CBool(nRetc) Then
                            dgvFlag(2 + kk, nRow).Value = m_typHyAlarm.a_mode.kendai(ii).torb(jj).row(kk)
                        Else
                            dgvFlag(2 + kk, nRow).Value = ""
                        End If
                    Next
                    nRow += 1

                    Next
                Next

        Catch ex As Exception
            strTmp = String.Format("データ読み込み異常[{0}]", ex.Message)
            MsgBox(strTmp)
        End Try
    End Sub

    ''' <summary>
    ''' データ書き込み
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub WriteData()
        Dim strTmp As String
        Dim nPos As Integer
        Dim nMen As Integer

        Try
            For ii As Integer = 0 To dgvFlag.RowCount - 1      '行数
                If CStr(dgvFlag(0, ii).Value) = "表検査台位置" Then
                    nPos = 0
                Else
                    nPos = 1
                End If

                If CStr(dgvFlag(1, ii).Value) = "表" Then
                    nMen = 0
                Else
                    nMen = 1
                End If

                For jj As Integer = 0 To ALARM_COL_NUM - 1
                    m_typHyAlarm.a_mode.kendai(nPos).torb(nMen).row(jj) = CInt(dgvFlag(2 + jj, ii).Value)
                Next
            Next

            '共有メモリ書き込み
            If Not ksMemWrite(TBL_HY_ALARM_NAME, m_typHyAlarm) Then
                strTmp = String.Format("共有メモリ書き込み異常[{0}]", TBL_HY_ALARM_NAME)
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
    Private Sub frmHyAlarm_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        'データグリッド初期化
        Initdgv(dgvFlag)

        'データ読み込み
        ReadData()
    End Sub

    ''' <summary>
    ''' 読み込みボタン
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
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
	End Sub

	Private WithEvents m_tmAuto As New Timer

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

    Private Sub CheckBox1_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles CheckBox1.CheckedChanged
        Dim chkBox As CheckBox = CType(sender, CheckBox)
        If chkBox.Checked Then
            btnRead.Enabled = False
            btnWrite.Enabled = False
            chkBox.BackColor = Color.Cyan
            m_tmAuto.Interval = 500 ' 500msec 毎に自動読み込み
            m_tmAuto.Start()
        Else
            chkBox.BackColor = SystemColors.Control
            btnRead.Enabled = True
            btnWrite.Enabled = True
            m_tmAuto.Stop()
        End If
    End Sub

    Private Sub frmHyAlarm_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        m_tmAuto.Stop()
        m_tmAuto.Dispose()
    End Sub
End Class
