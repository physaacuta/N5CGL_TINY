'*********************************************************************************
'　検査状態FFクラス
'	[Ver]
'		Ver.01    2015/03/04  初版
'
'	[メモ]
'		
'*********************************************************************************
'Option Strict On
Option Strict Off

Imports tClassLibrary
Imports tClassLibrary.tCnt
Public Class frmKensaStatus

    '---------------------------------------------------------------------------
    ' 定数定義
    '---------------------------------------------------------------------------
    Private Const STATUS_MAP_COL As Integer = 20                                    ' データ表示列数(見出しは除く)
    Private Const STATUS_MAP_ROW As Integer = CInt(MAX_FF_STS_CNT / STATUS_MAP_COL) ' データ表示行数

    '---------------------------------------------------------------------------
    ' 構造体
    '---------------------------------------------------------------------------
    '表示用検査状態構造体
    Private Structure DSP_CONTROL_MEMBER
        Dim lblHeader_Code As Label         '伝文コード 表:95 裏:96
        Dim lblRec_Code As Label            '連番(00)
        Dim lblDate As Label                '伝文発生時刻 形式 YYYYMMDHHNNSS
        Dim lblLine_Code As Label           '疵検ラインの工程コード['81','85','54','82(53)']設定
        Dim lblMeino As Label               '命令番号
        Dim lblCoilno As Label              '冷延コイル番号
        Dim lblId As Label                  '疵検管理No
        Dim lbldataSize As Label            '検査状態セット数
        Dim dgv As tCnt_DataGridView        '検査状態
    End Structure

    '---------------------------------------------------------------------------
    ' メンバ変数
    '---------------------------------------------------------------------------
    Private m_typ(NUM_MEN - 1) As DSP_CONTROL_MEMBER
    Private m_mode As Integer                           '要求モード(1:オンライン用、0:ツール用）
    Private m_strID As String                           'ID

    '***************************************************************************
    ' フォームイベント
    '***************************************************************************
    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="mode"></param>
    ''' <param name="strID"></param>
    ''' <remarks></remarks>
    Public Sub New(ByVal mode As Integer, Optional ByVal strID As String = "")
        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。
        'メンバ変数セット
        m_mode = mode                   'パラメータに要求モード(1:オンライン用、0:ツール用）
        m_strID = strID                 'ID

        '-------------------------------------------------------------------
        ' ファンクションキー設定
        '-------------------------------------------------------------------
        AddFunctionButton(btnEnd, 12 - 1)
    End Sub

    '***************************************************************************
    ' フォームイベント
    '***************************************************************************
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmKensaStatus_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        'タイトル設定
        Call SetTitleName(Me)

        'ポップアップ画面 低解像度対応
        Call SetPcHeight(Me)

        'コントロールセット
        Call Set_Control()

        '画面初期化
        Call InitDsp()

        'IDリスト作成
        Call MakeIDList()

        If 1 = m_mode Then                      '要求モード(1:オンライン用）
            btnRead.Visible = False
            cmbID.DropDownStyle = ComboBoxStyle.DropDownList
            If cmbID.Items.Count > 0 Then
                cmbID.SelectedIndex = 0         '先頭データ選択
            End If
        Else                                ' ツールモード
            btnRead.Visible = True          ' 読込ボタン「表示」
        End If

    End Sub

    ''' <summary>
    ''' コントロールセット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Set_Control()
        ' 表用コントロールセット
        m_typ(0).lblHeader_Code = lblHeader_Code1
        m_typ(0).lblRec_Code = lblRec_Code1
        m_typ(0).lblDate = lblDate1
        m_typ(0).lblLine_Code = lblLine_Code1
        m_typ(0).lblMeino = lblMeino1
        m_typ(0).lblCoilno = lblCoilno1
        m_typ(0).lblId = lblId1
        m_typ(0).lbldataSize = lbldataSize1
        m_typ(0).dgv = dgvStatus1

        ' 裏用コントロールセット
        m_typ(1).lblHeader_Code = lblHeader_Code2
        m_typ(1).lblRec_Code = lblRec_Code2
        m_typ(1).lblDate = lblDate2
        m_typ(1).lblLine_Code = lblLine_Code2
        m_typ(1).lblMeino = lblMeino2
        m_typ(1).lblCoilno = lblCoilno2
        m_typ(1).lblId = lblId2
        m_typ(1).lbldataSize = lbldataSize2
        m_typ(1).dgv = dgvStatus2
    End Sub

    ''' <summary>
    ''' IDリスト作成
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MakeIDList()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文

        If 1 = m_mode Then                      '要求モード(1:オンライン用）
            strSQL = ""
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_FF_S_ZISEKI
            strSQL &= " WHERE ID=" & "'" & m_strID & "'"
            strSQL &= " ORDER BY 登録日付 DESC"
        Else                                    '要求モード(0:ツール用）
            strSQL = ""
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_FF_S_ZISEKI
            strSQL &= " ORDER BY 登録日付 DESC"
        End If


        'DBオープン
        If Not tcls_DB.Open() Then
            Exit Sub
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try
            cmbID.Items.Clear()
            ' データ取得
            While sqlRead.Read()

                cmbID.Items.Add(sqlRead.GetValue(0).ToString)

            End While

        Catch ex As Exception
        Finally
            If tcls_DB Is Nothing = False Then
                tcls_DB.Cancel()
                If sqlRead Is Nothing = False Then
                    sqlRead.Close()
                    sqlRead = Nothing
                End If
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If
        End Try

    End Sub

    ''' <summary>
    ''' 画面初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitDsp()
        For Each typ As DSP_CONTROL_MEMBER In m_typ
            typ.lblHeader_Code.Text = ""
            typ.lblRec_Code.Text = ""
            typ.lblDate.Text = ""
            typ.lblLine_Code.Text = ""
            typ.lblMeino.Text = ""
            typ.lblCoilno.Text = ""
            typ.lblId.Text = ""
            typ.lbldataSize.Text = ""
            dgvInitialize(typ.dgv)
        Next

    End Sub

    '***************************************************************************
    ' キーイベント
    '***************************************************************************
    ''' <summary>
    ''' ファンクションボタン
    ''' </summary>
    ''' <param name="FncNo">ファンクションボタンNo 0オリジン</param>
    ''' <param name="onKey"></param>
    Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo                                                   ' ファンクションボタン (0オリジン)
                '---------------------------------------------------------------
                Case 12 - 1                                                     ' 終了
                    btnEnd_Click(Nothing, Nothing)
            End Select
        Catch ex As Exception
        End Try
    End Sub
    ''' <summary>
    ''' 画面終了
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

        '画面終了
        Me.Close()

    End Sub

    ''' <summary>
    ''' データ取得
    ''' </summary>
    ''' <param name="typStatus1">検査状態データ</param>
    ''' <param name="typStatus2">検査状態データ</param>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strID As String, ByRef typStatus1 As FF_STS_JISSEKI, ByRef typStatus2 As FF_STS_JISSEKI) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文
        Dim bytData1() As Byte  '生伝文バイナリデータ
        Dim bytData2() As Byte  '生伝文バイナリデータ


        '疵検管理Noあり
        strSQL = ""
        strSQL &= " SELECT "
        strSQL &= "        生伝文3,"
        strSQL &= "        生伝文4 "
        strSQL &= " FROM " & DB_BUF_FF_S_ZISEKI
        strSQL &= " WHERE ID = '" & strID & "'"
        strSQL &= " ORDER BY 登録日付 DESC"

        'DBオープン
        If Not tcls_DB.Open() Then
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return False

        Try

            ' データ取得
            sqlRead.Read()

            '生伝文
            bytData1 = CType(sqlRead.GetValue(0), Byte())

            '生伝文
            bytData2 = CType(sqlRead.GetValue(1), Byte())

            tMod.MemSet(bytData1, 0, typStatus1, tMod.SizeOf(typStatus1))
            tMod.MemSet(bytData2, 0, typStatus2, tMod.SizeOf(typStatus2))


        Catch ex As Exception
            Return False
        Finally
            If tcls_DB Is Nothing = False Then
                tcls_DB.Cancel()
                If sqlRead Is Nothing = False Then
                    sqlRead.Close()
                    sqlRead = Nothing
                End If
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If
        End Try

        Return True

    End Function


    ''' <summary>
    ''' グリッドの初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '' 基本を設定
        'Call dgv.dgvInitialize(dgv)
        'Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        With dgv
            .ColumnCount = 0
            .AutoGenerateColumns = False                                    ' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
            .AllowUserToAddRows = False                                     ' ユーザー 行追加
            .AllowUserToDeleteRows = False                                  ' ユーザー 行削除
            .AllowUserToOrderColumns = False                                ' ユーザー 列移動
            .AllowUserToResizeColumns = False                                ' ユーザー 列サイズ変更
            .AllowUserToResizeRows = False                                  ' ユーザー 行サイズ変更
            .ShowCellErrors = False                                         ' セルエラーの表示
            .ShowCellToolTips = False                                       ' ツールヒントの表示
            .ShowEditingIcon = False                                        ' 編集中のセルの明示
            .ShowRowErrors = False                                          ' エラー行の明示
            .ScrollBars = ScrollBars.Both                                   ' スクロールバーの表示
            '.ColumnHeadersHeight = 42										' ヘッダーの高さ

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = True                                               ' 読み取り専用はダメ
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' セル書き換え

            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            .RowCount = STATUS_MAP_ROW


            ' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' 列定義
            .ColumnCount = STATUS_MAP_COL + 1           '
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 24                                       ' ヘッダーの高さ

            '列0 (アドレス) 長手位置
            .Columns(0).HeaderText = "位置[m]"
            .Columns(0).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(0).Width = 58
            .Columns(0).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(0).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '列1～ (データ)
            For ii As Integer = 1 To STATUS_MAP_COL
                .Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                .Columns(ii).Width = 27
                .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .Columns(ii).HeaderText = CStr(ii)
            Next ii
        End With
    End Sub

    ''' <summary>
    ''' IDリスト選択
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbID_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cmbID.SelectedIndexChanged
        Call btnRead_Click(sender, e)
    End Sub

    ''' <summary>
    ''' 読み込みボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
        Dim typStatus(NUM_MEN - 1) As FF_STS_JISSEKI
        Try
            'データ表示
            If GetData(cmbID.Text, typStatus(0), typStatus(1)) Then
                For ii As Integer = 0 To NUM_MEN - 1
                    If Not SetData(m_typ(ii), typStatus(ii)) Then
                        Call InitDsp()
                        Exit For
                    End If
                Next
            Else
                Call InitDsp()
                MsgBox("データの取得に失敗しました。")
            End If
        Catch ex As Exception
        End Try
    End Sub

    ''' <summary>
    ''' データセット
    ''' </summary>
    ''' <param name="typ">コントロール</param>
    ''' <param name="typStatus">検査状態データ</param>
    ''' <remarks></remarks>
    Private Function SetData(ByVal typ As DSP_CONTROL_MEMBER, ByVal typStatus As FF_STS_JISSEKI) As Boolean

        Dim ii As Integer   'ループカウンタ
        Dim jj As Integer   'ループカウンタ
        Dim nPos As Integer
        Try
            typ.lblHeader_Code.Text = tMod.ByteToString(typStatus.Hd.Header_Code)
            typ.lblRec_Code.Text = tMod.ByteToString(typStatus.Hd.Rec_Code)
            typ.lblDate.Text = tMod.ByteToString(typStatus.Hd.bytDate)
            typ.lblLine_Code.Text = tMod.ByteToString(typStatus.Hd.Line_Code)
            typ.lblRec_Code.Text = tMod.ByteToString(typStatus.Hd.Rec_Code)
            typ.lblMeino.Text = tMod.ByteToString(typStatus.Hd.Meino)
            typ.lblCoilno.Text = tMod.ByteToString(typStatus.Hd.Coilno)
            typ.lblId.Text = tMod.ByteToString(typStatus.Dt.Id)
            typ.lbldataSize.Text = tMod.ByteToString(typStatus.Dt.dataSize)

            ' 検査状態
            For ii = 0 To STATUS_MAP_ROW - 1
                'typ.dgv(0, ii).Value = CStr(ii * 20) & "m"
                typ.dgv(0, ii).Value = CStr(ii * STATUS_MAP_COL)
                For jj = 0 To STATUS_MAP_COL - 1
                    nPos = ii * STATUS_MAP_COL + jj
                    If nPos > MAX_FF_STS_CNT - 1 Then Exit For
                    typ.dgv(jj + 1, ii).Value = tMod.ByteToString(typStatus.Dt.data(nPos).status)
                Next
            Next
            Return True
        Catch ex As Exception
            Return False
        End Try
    End Function

End Class
