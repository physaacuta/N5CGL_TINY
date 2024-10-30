'*********************************************************************************
' 故障ログ画面
'	[Ver]
'		Ver.01    2012/07/13  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports KizuForm
Imports tClassLibrary.tClass
Imports tClassLibrary.tClass.tClass_LogManager
Imports tClassLibrary.tClass.tClass_SQLServer

''' <summary>
''' 故障ログ画面
''' </summary>
''' <remarks></remarks>
Public Class frmLog
    Inherits SO_GAMENN.frmMain_Log2

    Private Const mc_PC_FILTER_ROW As Integer = 3                                 ' PCフィルター行数

    ''' <summary>
    ''' ファンクション切替
    ''' </summary>
    ''' <param name="FncNo">ファンクションNo (0オリジン)</param>
    ''' <param name="enable">ファンクションボタンの有効無効状態</param>
    ''' <param name="onKey">同時に押されているキー</param>
    ''' <remarks></remarks>
    Private Sub frmLog_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' デバッグログ表示
        If 1 - 1 = FncNo AndAlso onKey = Keys.Shift Then
            ShowDebugLog()
            Exit Sub
        End If

        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                            ' ※0オリジン
            Case 0                                   ' F01 RAS画面直接表示
                Dim frm As New frmRas
                frm.Owner = Me.my_frmParent
                frm.SetParentForm(Me.my_frmParent)
                Me.SetParentForm(Nothing)
                frm.Show()
                ExecFromClose()
            Case 1                                  ' F01 全選択
                btnAllOn_Click()
            Case 2                                  ' F02 全解除
                btnAllOff_Click()
            Case 3                                   ' F04 キャンセル
                Call ExecCancel_Log()
            Case 4                                   ' F05 ログ検索
                Call ExecSelect_Log()
            Case 5
            Case 7                                   ' F08 コメント編集
                Call ExecComment()
            Case 8                                   ' F09 CSV出力
                Call CsvOut(CSV_PATH, "故障ログ画面.csv")
            Case 9                                   ' F10 画面印刷
                Call PrintOut()
            Case 10                                  ' F11 ﾊｰﾄﾞｺﾋﾟｰ
                Call HardCopy(IMG_PATH, "故障ログ画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 11                                  ' F12 終了
                MyBase.ExecFromClose()
        End Select
    End Sub

    Public Sub New()

        ' この呼び出しはデザイナーで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        Dim s As Size
        s.Width = WIN_WIDTH_MAX
        s.Height = WIN_HEIGHT_MAX
        Me.MaximumSize = s
        s.Width = WIN_WIDTH_MIN
        s.Height = WIN_HEIGHT_MIN
        Me.MinimumSize = s

    End Sub

    ''' <summary>
    ''' ログリストダブルクリック
    ''' </summary>
    Private Sub dgvLog_CellDoubleClick(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvLog.CellDoubleClick
        Call ExecComment()
    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmLog_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Dim s As System.Windows.Forms.Screen = System.Windows.Forms.Screen.FromControl(Me.Owner)
        Me.Location = s.Bounds.Location
        Me.Height = Me.Owner.Height
        Me.Width = Me.Owner.Width

        'タイトル設定
        Call SetTitleName(Me)

        '遠隔時
        If NOA_ONLINE <> g_NOA Then
            'フォーム状態
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            Me.tProp_EndButtonEnable = False
        End If

        '''' 初期化処理
        Call GetDbInfToSetData()
        Call Get_PCInfo()
        Call InitdgvMap(dgvPcKind, GetMaxPcCnt(False))
        Call InitdgvMap(dgvPcName, GetMaxPcCnt(True))
        Call SetPCFillterType()
        Call SetPCFillterName()

        '''' 初期選択状態
        Call MyBase.SetChkBackColor_Change(chkKubun_A_Jyu, True)
        Call MyBase.SetChkBackColor_Change(chkKubun_S_Jyu, True)
        Call MyBase.SetChkBackColor_Change(chkKubun_H_Jyu, True)
        Call MyBase.SetChkBackColor_Change(chkKubun_A_Kei, True)
        Call MyBase.SetChkBackColor_Change(chkKubun_H_Kei, True)

        '''' コメント機能のON/OFF
        If MyBase.tProp_IsComment Then
            Call MyBase.SetChkBackColor_Change(chkKubun_A_Cmt, True)
            Call MyBase.SetChkBackColor_Change(chkKubun_S_Cmt, True)
            Call MyBase.SetChkBackColor_Change(chkKubun_H_Cmt, True)
        End If

    End Sub

    ''' <summary>
    ''' 表示/最大表示件数取得/データセット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetDbInfToSetData()
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSQL As String

        Dim rowMaxCnt As Integer = 0        ' 行数
        Dim clrEvt As Integer = 0           ' イベント色
        Dim clrJyu As Integer = 0           ' 重故障色
        Dim clrKei As Integer = 0           ' 軽故障色

        strSQL = "SELECT LOG表示件数, LOGイベント色, LOG重故障色, LOG軽故障色 FROM " & DB_COMMON_MASTER
        tcls_DB.Open()

        Dim sqlRead As SqlClient.SqlDataReader = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return

        Try
            sqlRead.Read()
            rowMaxCnt = sqlRead.GetInt32(0)
            clrEvt = sqlRead.GetInt32(1)
            clrJyu = sqlRead.GetInt32(2)
            clrKei = sqlRead.GetInt32(3)

        Catch ex As Exception
            Return
        Finally
            sqlRead.Close()
        End Try

        sqlRead.Close()
        tcls_DB.Close()
        tcls_DB.Dispose()

        '''' データセット
        MyBase.SetLogKindColor(clrEvt, clrJyu, clrKei)
        MyBase.SetLogMaxRow(rowMaxCnt)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ オーバーライド
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' Me.Activatedでは使い物にならないため、Loadの終了1回だけ非同期でメソッド
    ''' </summary>
    ''' <remarks></remarks>
    Protected Overrides Sub LoadEnding()
        '元々のメソッド実行
        MyBase.LoadEnding()

        '''' 拡張分
        '' 画面が開き終わったタイミングで一回検索を行う
        Call ExecSelect_Log()
    End Sub

    ''' <summary>
    ''' ログ検索開始
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overrides Sub ExecStart()
        my_btnFunction(0).Enabled = False
        my_btnFunction(5).Enabled = False
        my_btnFunction(7).Enabled = False
        my_btnFunction(8).Enabled = False
        my_btnFunction(9).Enabled = False
        my_btnFunction(10).Enabled = False

        '' ベース実行
        MyBase.ExecStart()
    End Sub

    ''' <summary>
    ''' ログ検索完了
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overrides Sub ExecEnd()
        my_btnFunction(0).Enabled = True
        my_btnFunction(5).Enabled = True
        my_btnFunction(7).Enabled = True
        my_btnFunction(8).Enabled = True
        my_btnFunction(10).Enabled = True

        'プリンタがなければ、画面印刷無効
        If Not tClass.tClass_HeadCopy.IsCheckPrinter Then
            my_btnFunction(9).Enabled = False
        Else
            my_btnFunction(9).Enabled = True
        End If

        '' ベース実行
        MyBase.ExecEnd()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 表示設定
    ''' </summary>
    ''' <param name="dgvCtrl">疵マップデータグリッド</param>
    ''' <remarks></remarks>
    Private Sub InitdgvMap(ByVal dgvCtrl As tCnt.tCnt_DataGridView, ByVal nMaxCnt As Integer)
        '基本設定
        dgvCtrl.dgvInitialize(dgvCtrl)
        dgvCtrl.DataGridViewSelectionCancel(dgvCtrl)

        With dgvCtrl
            '行定義
            .RowTemplate.Height = 16                                        ' 行高さ

            .RowCount = 0

            .ColumnHeadersVisible = False                                    ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            '動作定義
            '.ScrollBars = ScrollBars.Horizontal
            .DoubleBuffered = True
            .EditMode = DataGridViewEditMode.EditProgrammatically                       ' セル書き換え

            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.AllCellsExceptHeaders      ' 可視行サイズ自動調整

            'セル定義
            '.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            '.DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)


            For ii As Integer = 0 To CInt(Math.Ceiling(nMaxCnt / mc_PC_FILTER_ROW)) - 1
                Using clm As New DataGridViewCheckButtonColumn
                    clm.HeaderText = ""

                    clm.Width = 60
                    clm.Frozen = False
                    clm.ReadOnly = False
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                    clm.DefaultCellStyle.BackColor = Drawing.SystemColors.Control

                    .Columns.Add(clm)
                End Using
            Next

            If nMaxCnt < mc_PC_FILTER_ROW Then
                .RowCount = nMaxCnt
            Else
                .RowCount = mc_PC_FILTER_ROW
            End If

        End With
    End Sub

    ''' <summary>
    ''' SQL文取得
    ''' </summary>
    ''' <param name="bMode">項目取得区分(True:通常,False:個数(count(*)))</param>
    ''' <returns>作成したSQL文</returns>
    ''' <remarks></remarks>
    Private Function GetSQL_PC(ByVal bMode As Boolean) As String

        Dim strSQL As String = ""

        If Not bMode Then

            '行数取得
            strSQL &= " SELECT COUNT(*) "

        Else

            '項目取得
            strSQL = " SELECT "
            strSQL &= " M.PCID as PCID,"
            strSQL &= " I.PCKIND as PCKIND,"
            strSQL &= " M.パソコン名称 as パソコン名称,"
            strSQL &= " M.PC名 as PC名,"
            strSQL &= " I.PC種別 as PC種別,"
            strSQL &= " I.PC区分 as PC区分"
        End If

        '最後の","を削除
        sqlColumnTrim(strSQL)

        '// 異常のみ表示は現状のまま
        '// すべて表示 ･･･ 機器IDが設定されているデータすべて表示
        strSQL &= " FROM " & DB_PC_MASTER & " M "
        strSQL &= " INNER JOIN " & DB_PC_INF & " I ON I.PCID = M.PCID"
        If bMode Then
            strSQL &= " ORDER BY PCID ASC "
        End If

        '作成したSQLを返す
        Return strSQL

    End Function

    Public Structure TYP_PC_INFO_DATA
        Dim nPcId As Integer                    '
        Dim nPcKind As Integer                  '
        Dim cPcName1 As String                  '
        Dim cPcName2 As String                  '
        Dim cPcType As String                   '
        Dim nPcKbn As Integer                   '
        'イニシャライズ
        Public Sub initialize()
            nPcId = 0
            nPcKind = 0
            cPcName1 = String.Empty
            cPcName2 = String.Empty
            cPcType = String.Empty
            nPcKbn = 0
        End Sub
    End Structure

    Private m_lstPcInfo As New List(Of TYP_PC_INFO_DATA)

    ''' <summary>
    ''' データ表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Get_PCInfo()

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文

        'DBオープン
        If Not tcls_DB.Open() Then
            Log(LNO.ERR, "DB接続エラー(PC) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If


        '表示データの項目取得
        strSQL = GetSQL_PC(True)

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try
            m_lstPcInfo.Clear()

            ' データ表示
            While sqlRead.Read()

                Dim typData As TYP_PC_INFO_DATA = Nothing
                typData.initialize()

                ' データ取り出し
                typData.nPcId = sqlRead.GetInt32(sqlRead.GetOrdinal("PCID"))              'PCID
                typData.nPcKind = sqlRead.GetInt32(sqlRead.GetOrdinal("PCKIND"))          'PCKIND
                typData.cPcType = sqlRead.GetString(sqlRead.GetOrdinal("PC種別"))         'PC種別
                typData.cPcName2 = sqlRead.GetString(sqlRead.GetOrdinal("パソコン名称"))  'パソコン名称
                typData.cPcName1 = sqlRead.GetString(sqlRead.GetOrdinal("PC名"))          'PC名
                typData.nPcKbn = sqlRead.GetInt32(sqlRead.GetOrdinal("PC区分"))           'PC区分

                m_lstPcInfo.Add(typData)

            End While

        Catch ex As Exception
            Log(LNO.ERR, String.Format("PCデータ表示中に例外[{0}]", ex.Message))
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub

    ''' <summary>
    ''' PC名フィルターセット
    ''' </summary>
    Private Sub SetPCFillterName()
        For ii As Integer = 0 To m_lstPcInfo.Count - 1
            Dim nX As Integer = 0
            Dim nY As Integer = 0
            nX = ii Mod mc_PC_FILTER_ROW
            nY = CInt(Math.Floor(ii / mc_PC_FILTER_ROW))
            dgvPcName(nY, nX).Value = m_lstPcInfo(ii).cPcName1
            CType(dgvPcName(nY, nX), DataGridViewCheckButtonCell).Enable = True
            AddCheckBoxPCName(CType(dgvPcName(nY, nX), DataGridViewCheckButtonCell), m_lstPcInfo(ii).cPcName1)
            dgvPcName.AutoResizeColumn(nY)
        Next
    End Sub

    ''' <summary>
    ''' PC機能フィルターセット
    ''' </summary>
    Private Sub SetPCFillterType()
        Dim nCnt As Integer = 0

        For ii As Integer = 0 To m_lstPcInfo.Count - 1
            Dim nX As Integer = 0
            Dim nY As Integer = 0

            If m_lstPcInfo(ii).nPcKbn > 0 Then Continue For

            nX = nCnt Mod mc_PC_FILTER_ROW
            nY = CInt(Math.Floor(nCnt / mc_PC_FILTER_ROW))
            dgvPcKind(nY, nX).Value = m_lstPcInfo(ii).cPcType
            CType(dgvPcKind(nY, nX), DataGridViewCheckButtonCell).Enable = True
            AddCheckBoxPCKind(CType(dgvPcKind(nY, nX), DataGridViewCheckButtonCell), CStr(m_lstPcInfo(ii).nPcKind))
            dgvPcKind.AutoResizeColumn(nY)
            nCnt = nCnt + 1
        Next
    End Sub

    ''' <summary>
    ''' PC数取得
    ''' </summary>
    ''' <param name="bMode">True:全PC、False:オンラインPC</param>
    ''' <returns></returns>
    Private Function GetMaxPcCnt(ByVal bMode As Boolean) As Integer
        If bMode Then Return m_lstPcInfo.Count
        Dim nCnt As Integer = 0
        For ii As Integer = 0 To m_lstPcInfo.Count - 1
            If m_lstPcInfo(ii).nPcKbn > 0 Then Continue For
            nCnt = nCnt + 1
        Next
        Return nCnt
    End Function

    ''' <summary>
    ''' ボタンクリック時の色替え処理(PC機能)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub dgvPcKind_CellClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvPcKind.CellClick
        If e.RowIndex < 0 Then Exit Sub
        If e.ColumnIndex < 0 Then Exit Sub
        If CType(dgvPcKind(e.ColumnIndex, e.RowIndex), DataGridViewCheckButtonCell).Checked Then
            dgvPcKind.Item(e.ColumnIndex, e.RowIndex).Style.BackColor = Color.Cyan
        Else                                ' OFF
            dgvPcKind.Item(e.ColumnIndex, e.RowIndex).Style.BackColor = Drawing.SystemColors.Control
        End If
        dgvPcKind.Refresh()
    End Sub

    ''' <summary>
    ''' ボタンクリック時の色替え処理(PC名)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub dgvPcName_CellClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvPcName.CellClick
        If e.RowIndex < 0 Then Exit Sub
        If e.ColumnIndex < 0 Then Exit Sub
        If CType(dgvPcName(e.ColumnIndex, e.RowIndex), DataGridViewCheckButtonCell).Checked Then
            dgvPcName.Item(e.ColumnIndex, e.RowIndex).Style.BackColor = Color.Cyan
        Else                                ' OFF
            dgvPcName.Item(e.ColumnIndex, e.RowIndex).Style.BackColor = Drawing.SystemColors.Control
        End If
        dgvPcName.Refresh()
    End Sub

End Class
