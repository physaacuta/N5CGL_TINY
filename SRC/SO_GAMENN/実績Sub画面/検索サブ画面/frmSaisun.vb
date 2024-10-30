'*********************************************************************************
'　採寸実績クラス
'	[Ver]
'		Ver.01    2007/09/22  初版
'
'	[メモ]
'		
'*********************************************************************************
'Option Strict On
Option Strict Off

Imports tClassLibrary
Public Class frmSaisun

    Private m_mode As Integer               '要求モード(1:オンライン用、0:ツール用）
    Private m_strKizukenNo As String        '疵検管理No

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const DgvMaxRow_MRare As Integer = 16           'データ行MAX
    Private Const DgvMaxCol_MRare As Integer = 3            'データ列MAX

    ' グリッド　列設定
    Private Enum EM_MRATE_COL_ID
        iTitle = 0          '項目見出し
        iStartPos           '開始位置
        iEndPos             '終了位置
    End Enum

    ''' <summary>
    ''' パラメータに要求モード(1:オンライン用、0:ツール用）
    ''' </summary>
    ''' <param name="mode">1:オンライン用、0:ツール用</param>
    ''' <param name="strKizukenNo">管理No</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal mode As Integer, Optional ByVal strKizukenNo As String = "")
        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。
        'メンバ変数セット
        m_mode = mode                   'パラメータに要求モード(1:オンライン用、0:ツール用）
        m_strKizukenNo = strKizukenNo   '管理No

        '-------------------------------------------------------------------
        ' ファンクションキー設定
        '-------------------------------------------------------------------
        AddFunctionButton(btnEnd, 12 - 1)
    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmSaisun_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        Me.tProp_Moveable = True

        Call SetTitleName(Me)

        '画面初期化
        Call InitDsp()

        'IDリスト作成
        Call MakeIDList()

        If 1 = m_mode Then                  ' オンラインモード
            btnRead.Visible = False         ' 読込ボタン「非表示」
            cmbID.DropDownStyle = ComboBoxStyle.DropDownList
            If cmbID.Items.Count <> 0 Then
                cmbID.SelectedIndex = 0
            End If
        Else                                ' ツールモード
            btnRead.Visible = True          ' 読込ボタン「表示」
        End If

    End Sub


    ''' <summary>
    ''' IDリスト作成
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MakeIDList()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文

        strSQL = ""

        If m_mode = 1 Then           'オンラインモード
            ' オンラインモードの場合は、管理No.から検索する
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_PRO_R_SAISUN
            strSQL &= " WHERE 管理No = '" & m_strKizukenNo & "'"
            strSQL &= " ORDER BY 登録日付 DESC"
        Else                         'ツールモード
            'strSQL &= " SELECT TOP 100 ID"
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_PRO_R_SAISUN
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
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try


    End Sub

    ''' <summary>
    ''' 画面初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitDsp()

        '命令No
        lblMeiNo.Text = ""

        '生産NO
        lblProductNo.Text = ""

        '実績コイル長
        lblCoilLen.Text = ""

        'フロント巻き込まれ長さ
        lblFLen.Text = ""

        'セットした採寸個数
        lblSetCnt.Text = ""

        'グラントフラグ
        lblGrant.Text = ""
        lblGrant.BackColor = Color.FromKnownColor(KnownColor.Control)

        ''疵個数オーバーステータス

        ''データ設定数


        'カットデータ
        dgvInitialize(dgvCut)

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
    ''' <param name="strID">ID</param>
    ''' <param name="typZiseki">カット実績伝文</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strID As String, ByRef typZiseki As PRO_TYPE_SAISUN) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String        'SQL文
        Dim bytData() As Byte       '生伝文バイナリデータ

        '疵検管理Noあり
        strSQL = ""
        strSQL &= " SELECT 生伝文"
        strSQL &= " FROM " & DB_BUF_PRO_R_SAISUN
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

            ' 生伝文
            bytData = CType(sqlRead.GetValue(0), Byte())
            Dim ii As Integer
            ii = tMod.SizeOf(typZiseki)
            tMod.MemSet(bytData, 0, typZiseki, tMod.SizeOf(typZiseki))

        Catch ex As Exception
            Return False
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        Return True

    End Function

    ''' <summary>
    ''' IDリスト選択
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>

    Private Sub cmbID_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cmbID.SelectedIndexChanged
        Call btnRead_Click(sender, e)
    End Sub

    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
        Dim typZiseki As PRO_TYPE_SAISUN = Nothing  '疵検査実績データ

        'データ表示
        If GetData(cmbID.Text, typZiseki) Then
            Call SetData(typZiseki)
        End If
    End Sub

    ''' <summary>
    ''' データセット
    ''' </summary>
    '''<param name="typZiseki">カット実績</param>
    ''' <remarks></remarks>
    Private Sub SetData(ByVal typZiseki As PRO_TYPE_SAISUN)

        ''命令No
        lblMeiNo.Text = tMod.ByteToString(typZiseki.data.KeyInf.cMeiNo)

        ''生産NO
        lblProductNo.Text = tMod.ByteToString(typZiseki.data.KeyInf.cCoilNo)

        '実績コイル長
        lblCoilLen.Text = Format(CSng(typZiseki.data.nLen) / 10, "0.0")
        'lblCoilLen.Text = typZiseki.data.nLen

        'フロント巻き込まれ長さ
        lblFLen.Text = Format(CSng(typZiseki.data.nMakiLenF) / 10, "0.0")
        'lblFLen.Text = typZiseki.data.nMakiLenF

        'セットした採寸個数
        lblSetCnt.Text = typZiseki.data.nSetCnt

        'グラントフラグ
        If 0 = typZiseki.data.Common.nGrandFlg Then
            lblGrant.Text = "正常"
            lblGrant.BackColor = Color.Blue
            lblGrant.ForeColor = Color.Yellow
        Else
            lblGrant.Text = "異常"
            lblGrant.BackColor = Color.Red
            lblGrant.ForeColor = Color.Yellow
        End If

        ' 採寸開始位置、採寸終了位置
        For ii As Integer = 0 To DgvMaxRow_MRare - 1
            dgvCut(0, ii).Value = ii + 1
            'dgvCut(1, ii).Value = typZiseki.data.saisun(ii).nStart
            dgvCut(1, ii).Value = Format(CSng(typZiseki.data.inf(ii).nStart) / 10, "0.0")

            'dgvCut(2, ii).Value = typZiseki.data.saisun(ii).nEnd
            dgvCut(2, ii).Value = Format(CSng(typZiseki.data.inf(ii).nEnd) / 10, "0.0")
        Next ii

    End Sub

    ''' <summary>
    ''' グリッドの初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
        With dgv
            .RowCount = DgvMaxRow_MRare

            '' 動作定義
            .AutoGenerateColumns = False                                    ' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
            .AllowUserToAddRows = False                                     ' ユーザー 行追加
            .AllowUserToDeleteRows = False                                  ' ユーザー 行削除
            .AllowUserToOrderColumns = False                                ' ユーザー 列移動
            .AllowUserToResizeColumns = False                               ' ユーザー 列サイズ変更
            .AllowUserToResizeRows = False                                  ' ユーザー 行サイズ変更
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False
            .ScrollBars = ScrollBars.Both


            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = True                                                ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え

            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            '' 選択セル色定義
            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue                 ' 選択
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' 列定義
            .ColumnCount = DgvMaxCol_MRare           'textbox のみを追加する場合はこれの方が楽
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 50                                       ' 列ヘッダサイズ

            '列0 (アドレス) 疵検出区分
            .Columns(EM_MRATE_COL_ID.iTitle).HeaderText = "NO"
            .Columns(EM_MRATE_COL_ID.iTitle).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iTitle).Width = 40
            .Columns(EM_MRATE_COL_ID.iTitle).SortMode = DataGridViewColumnSortMode.NotSortable



            '列１ (アドレス) 疵検出区分
            .Columns(EM_MRATE_COL_ID.iStartPos).HeaderText = "開始位置"
            .Columns(EM_MRATE_COL_ID.iStartPos).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iStartPos).Width = 100
            .Columns(EM_MRATE_COL_ID.iStartPos).SortMode = DataGridViewColumnSortMode.NotSortable


            '列２ (アドレス) 疵種
            .Columns(EM_MRATE_COL_ID.iEndPos).HeaderText = "終了位置"
            .Columns(EM_MRATE_COL_ID.iEndPos).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iEndPos).Width = 108
            .Columns(EM_MRATE_COL_ID.iEndPos).SortMode = DataGridViewColumnSortMode.NotSortable
        End With
    End Sub
End Class
