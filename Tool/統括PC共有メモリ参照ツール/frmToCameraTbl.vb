Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Runtime.InteropServices

''' <summary>
''' カメラコントローラ情報
''' </summary>
''' <remarks></remarks>
Public Class frmToCameraTbl
    '///////////////////////////////////////////////////////
    'メンバー変数
    '///////////////////////////////////////////////////////
    Private m_typToCameraTbl As TO_CAMERA_TBL
    Private WithEvents m_timer1 As New Timer
    Private WithEvents m_timer2 As New Timer    ' 共有メモリアクセス可・不可確認タイマー

    '///////////////////////////////////////////////////////
    'メンバー関数
    '///////////////////////////////////////////////////////
    ''' <summary>
    ''' データグリッド初期化
    ''' </summary>
    ''' <param name="dgvCtrl"></param>
    ''' <remarks></remarks>
    Private Sub InitdgvCamInf(ByVal dgvCtrl As tCnt_DataGridView)
        '基本設定
        dgvCtrl.dgvInitialize(dgvCtrl)

        With dgvCtrl
            'スクロールバーの表示
            .ScrollBars = ScrollBars.Both

            '選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.Transparent
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '行設定
            .RowCount = NUM_CAMERA
            .RowTemplate.Height = 20

            '列設定
            .ColumnCount = 3
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 40
            For ii As Integer = 0 To .ColumnCount - 1
                .Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                .Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(ii).HeaderText = ""
                Select Case ii
                    Case 0, 1, 2
                        .Columns(ii).Width = 120
                    Case Else
                        .Columns(ii).Width = 80
                End Select
            Next ii
            .Columns(0).ReadOnly = True
            .Columns(0).Frozen = True

            .Columns(0).HeaderText = "ｶﾒﾗｾｯﾄ/角度順"
            .Columns(1).HeaderText = "現在露光時間"
            .Columns(2).HeaderText = "現在状態"
        End With
    End Sub

    ''' <summary>
    ''' カメラコントローラ設定情報読み込み
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadToCameraTbl()
        Dim strTmp As String

        Try
            Dim nRetc As ValueType = 0                          'リターンコード

            '詳細情報読み込み
            m_typToCameraTbl = CType(ksMemRead(TBL_TO_CAMERA_NAME, 0, GetType(TO_CAMERA_TBL), nRetc), TO_CAMERA_TBL)
        Catch ex As Exception
            strTmp = String.Format("ｶﾒﾗｺﾝﾄﾛｰﾗ設定情報読み込み異常[{0}]", ex.Message)
            MsgBox(strTmp)
        End Try
    End Sub

    ''' <summary>
    ''' カメラコントローラ設定情報書き込み
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub WriteCamInf()
        Dim strTmp As String

        Try
            '共有メモリ書き込み
            If Not ksMemWrite(TBL_TO_CAMERA_NAME, m_typToCameraTbl) Then
                strTmp = String.Format("共有メモリ書き込み異常[{0}]", TBL_TO_CAMERA_NAME)
                MsgBox(strTmp)
            End If
        Catch ex As Exception
            strTmp = String.Format("ｶﾒﾗｺﾝﾄﾛｰﾗ設定情報書き込み異常[{0}]", ex.Message)
            MsgBox(strTmp)
        End Try
    End Sub

    ''' <summary>
    ''' カメラ設定情報表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspCamInf()
        Try

            For nRow As Integer = 0 To NUM_CAMERA - 1

                For nCol As Integer = 0 To dgvCamInf.ColumnCount - 1
                    dgvCamInf(nCol, nRow).Style.BackColor = Color.White
                Next
                With m_typToCameraTbl.caminf(nRow)
                    dgvCamInf(0, nRow).Value = "カメラ" & CStr(nRow + 1)
                    dgvCamInf(1, nRow).Value = CInt(.nExpTime)
                    dgvCamInf(2, nRow).Value = CInt(.nMode)
                End With
            Next

        Catch ex As Exception
            MsgBox("カメラ設定情報表示エラー")
        End Try


    End Sub


    '///////////////////////////////////////////////////////////////////////////
    'イベント関数
    '///////////////////////////////////////////////////////////////////////////
    Private Sub frmToCameraTbl_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Me.Cursor = Cursors.WaitCursor

        ' 共有メモリにアクセス可能かチェックタイマー
        m_timer2.Interval = 100         ' 100msec
        m_timer2.Start()

        '初期化
        InitdgvCamInf(dgvCamInf)

        'カメラコントローラ設定情報情報読み込み
        ReadToCameraTbl()

        'カメラ設定情報表示
        DspCamInf()

        ' 自動更新を有効にする・・・必要ない場合はコメントにする
        chkUpdate.Checked = True

        ' 共有メモリサイズ情報表示
        Dim str As String = ""
        str &= String.Format("TO_CAMERA_TBL Size = {0}", Marshal.SizeOf(GetType(TO_CAMERA_TBL))) & vbCrLf
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
    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
        'Me.Cursor = Cursors.WaitCursor
        ReadToCameraTbl()
        '表示
        DspCamInf()
        'Me.Cursor = Cursors.Default
    End Sub

    ''' <summary>
    ''' 書き込みボタン
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnWrite_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnWrite.Click
        Me.Cursor = Cursors.WaitCursor
        '書き込み
        WriteCamInf()
        Me.Cursor = Cursors.Default
    End Sub


    Private Sub dgvCamInf_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvCamInf.CellValueChanged
        Dim nCol As Integer = e.ColumnIndex
        Dim nRow As Integer = e.RowIndex

        If nRow < 0 OrElse nCol < 0 Then
            Exit Sub
        End If

        If dgvCamInf(nCol, nRow).Value Is Nothing Then
            dgvCamInf(nCol, nRow).Value = 0
        End If

        With m_typToCameraTbl.caminf(nRow)
            Select Case e.ColumnIndex
                Case 1
                    .nExpTime = CInt(dgvCamInf(nCol, nRow).Value)
                Case 2
                    .nMode = CInt(dgvCamInf(nCol, nRow).Value)
            End Select
        End With

        DspCamInf()
    End Sub

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
        btnRead.PerformClick()
    End Sub


    Private Sub frmToCameraTbl_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        m_timer1.Stop()
        m_timer1.Dispose()
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
        Dim tmp As TO_CAMERA_TBL = CType(ksMemRead(TBL_TO_CAMERA_NAME, 0, GetType(TO_CAMERA_TBL), nRetc), TO_CAMERA_TBL)
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
End Class
