'*********************************************************************************
'　疵混入率画面クラス
'	[Ver]
'		Ver.01    2007/09/22  初版
'
'	[メモ]
'		
'*********************************************************************************
'Option Strict On
Option Strict Off

Imports tClassLibrary
Public Class frmMixedRate

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const DgvMaxRow_MRare As Integer = 11           'データ行MAX
    Private Const DgvMaxCol_MRare As Integer = 7            'データ列MAX

    ' グリッド　列設定
    Private Enum EM_MRATE_COL_ID
        iTitle = 0              '項目見出し
        iGrade_No01             'Grade1
        iGrade_No02             'Grade2
        iGrade_No03             'Grade3
        iGrade_No04             'Grade4
        iGrade_No05             'Grade5
        iGrade_No06             'Grade6
    End Enum

    ' グリッド 行設定
    Private Enum EM_MRATE_ROW_ID
        iRate = 0               '疵混入率
        iType_No01              '疵種No01
        iType_No02              '疵種No02
        iType_No03              '疵種No03
        iType_No04              '疵種No04
        iType_No05              '疵種No05
        iType_No06              '疵種No06
        iType_No07              '疵種No07
        iType_No08              '疵種No08
        iType_No09              '疵種No09
        iType_No10              '疵種No10
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager    'ログ管理クラス
    Private m_strKizuken As String                  '管理No
    Private m_dctID As New Dictionary(Of Integer, String)   '該当管理Noのカット実績のID
    Private m_nIDCnt As Integer                     'カット実績のIDのカウンタ

    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="cls_Log"></param>
    ''' <remarks></remarks>
    Public Sub New(ByRef cls_Log As tClass.tClass_LogManager, Optional ByVal strKizuken As String = "")

        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        Me.Text = String.Format("[{0}] 疵混入率表示画面", SYSTEM_TITLE)

        mcls_Log = cls_Log
        m_strKizuken = strKizuken

        '探傷画面の場合は、前項、次項ボタンは無効
        If "" = m_strKizuken Then
            btnPre.Visible = False
            btnNext.Visible = False
            lblPage.Visible = False
        End If

        '-------------------------------------------------------------------
        ' ファンクションキー設定
        '-------------------------------------------------------------------
        AddFunctionButton(btnEnd, 12 - 1)

    End Sub



    ''' <summary>
    ''' グリッドの初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
        Dim ii As Integer = 0
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
            .ScrollBars = ScrollBars.None


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
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' 列定義
            .ColumnCount = DgvMaxCol_MRare           'textbox のみを追加する場合はこれの方が楽
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20                                       ' 列ヘッダサイズ

            '列１ (アドレス) 項目見出し
            .Columns(EM_MRATE_COL_ID.iTitle).HeaderText = ""
            .Columns(EM_MRATE_COL_ID.iTitle).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iTitle).Width = 120
            .Columns(EM_MRATE_COL_ID.iTitle).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iTitle).Frozen = True                                       ' 水平移動禁止

            '列２ (アドレス) グレード１
            .Columns(EM_MRATE_COL_ID.iGrade_No01).HeaderText = "Grｸﾞﾙｰﾌﾟ1"
            .Columns(EM_MRATE_COL_ID.iGrade_No01).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No01).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No01).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No01).Frozen = True                                       ' 水平移動禁止

            '列３ (アドレス) グレード２
            .Columns(EM_MRATE_COL_ID.iGrade_No02).HeaderText = "Grｸﾞﾙｰﾌﾟ2"
            .Columns(EM_MRATE_COL_ID.iGrade_No02).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No02).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No02).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No02).Frozen = True                                       ' 水平移動禁止

            '列４ (アドレス) グレード３
            .Columns(EM_MRATE_COL_ID.iGrade_No03).HeaderText = "Grｸﾞﾙｰﾌﾟ3"
            .Columns(EM_MRATE_COL_ID.iGrade_No03).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No03).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No03).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No03).Frozen = True                                       ' 水平移動禁止

            '列５ (アドレス) グレード４
            .Columns(EM_MRATE_COL_ID.iGrade_No04).HeaderText = "Grｸﾞﾙｰﾌﾟ4"
            .Columns(EM_MRATE_COL_ID.iGrade_No04).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No04).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No04).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No04).Frozen = True                                       ' 水平移動禁止


            '列６ (アドレス) グレード５
            .Columns(EM_MRATE_COL_ID.iGrade_No05).HeaderText = "Grｸﾞﾙｰﾌﾟ5"
            .Columns(EM_MRATE_COL_ID.iGrade_No05).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No05).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No05).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No05).Frozen = True                                       ' 水平移動禁止

            '列７ (アドレス) グレード６
            .Columns(EM_MRATE_COL_ID.iGrade_No06).HeaderText = "Grｸﾞﾙｰﾌﾟ6"
            .Columns(EM_MRATE_COL_ID.iGrade_No06).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No06).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No06).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No06).Frozen = True                                       ' 水平移動禁止

        End With
    End Sub

    ''' <summary>
    ''' 画面ロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMixedRate_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        Dim strCutID As String = ""      'カット実績ID

        '画面初期化
        Call InitDsp()

        '実績画面の場合、該当管理Noのカット実績のIDを全て取得
        If m_strKizuken <> "" Then
            If Not GetCutID() Or 0 = m_dctID.Count Then Exit Sub
            m_nIDCnt = 0
            strCutID = m_dctID(m_nIDCnt)
        End If

        'データ表示
        Call DspData(strCutID)

        'ボタ有効、無効
        Call SetPreNextBotton()

    End Sub

    ''' <summary>
    ''' データ表示
    ''' </summary>
    ''' <param name="strCutID"></param>
    ''' <remarks></remarks>
    Private Sub DspData(ByVal strCutID)

        Dim nOutCutNo As Integer = 0                '分割No
        Dim typZiseki As PRO_TYPE_ZISEKI = Nothing  '疵検査実績データ

        'データ取得
        If Not GetData(nOutCutNo, typZiseki, strCutID) Then
            Exit Sub
        End If

        'データセット
        SetData(nOutCutNo, typZiseki)

    End Sub


    ''' <summary>
    ''' データ存在有無チェック
    ''' </summary>
    ''' <param name="strKizuKen"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function ChkData(ByVal strKizuKen As String) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSQL As String    'SQL文
        Dim strWK As String = ""

        strSQL = ""
        strSQL &= " SELECT TOP 1 管理No"
        strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
        strSQL &= " WHERE 管理No = '" & strKizuKen & "'"
        strSQL &= " ORDER BY 登録日付 DESC"

        'DBオープン
        If Not tcls_DB.Open() Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_BUF_PRO_S_ZISEKI & "]")
            Return False
        End If

        ' Select文実行
        ChkData = tcls_DB.GetSelectKey(strSQL, strWK)
        tcls_DB.Dispose()
        tcls_DB = Nothing


    End Function



    ''' <summary>
    ''' 画面初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitDsp()

        '命令No
        lblMeiNo.Text = ""

        '生産NO
        lblProductNo.Text = ""

        '出側カット順番（5CGLでは非表示）
        lblOutCutNo.Text = ""

        '実績区分
        lblZisekiDiv.Text = ""
        lblZisekiDiv.BackColor = Color.FromKnownColor(KnownColor.Control)

        '収集区分
        lblColectDiv.Text = ""
        lblColectDiv.BackColor = Color.FromKnownColor(KnownColor.Control)


        '疵混入率表 初期化
        dgvInitialize(dgvMixedRate)

    End Sub


    ''' <summary>
    ''' データ取得
    ''' </summary>
    ''' <param name="nOutCutNo">分割No</param>
    ''' <param name="typZiseki">疵検査実績データ</param>
    ''' <remarks></remarks>
    Private Function GetData(ByRef nOutCutNo As Integer, ByRef typZiseki As PRO_TYPE_ZISEKI, Optional ByVal strCutID As String = "") As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文
        Dim bytData() As Byte        '生伝文バイナリデータ
        Dim bRet As Boolean = False

        If "" = m_strKizuken Then

            '管理Noなし()
            strSQL = ""
            strSQL &= " SELECT TOP 1 "
            strSQL &= " 分割No"
            strSQL &= " ,生伝文"
            strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
            strSQL &= " ORDER BY 登録日付 DESC"

        Else

            '管理Noあり
            strSQL = ""
            strSQL &= " SELECT "
            strSQL &= " 分割No"
            strSQL &= " ,生伝文"
            strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
            strSQL &= " WHERE 管理No = '" & m_strKizuken & "'"
            strSQL &= " AND   ID = '" & strCutID & "'"

        End If

        'DBオープン
        If Not tcls_DB.Open() Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_BUF_PRO_S_ZISEKI & "]")
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return False

        Try

            ' データ取得
            sqlRead.Read()

            If sqlRead.HasRows Then

                '分割No
                nOutCutNo = CInt(sqlRead.GetValue(0).ToString)

                '生伝文
                bytData = CType(sqlRead.GetValue(1), Byte())

                tMod.MemSet(bytData, 0, typZiseki, tMod.SizeOf(typZiseki))

                '正常終了
                bRet = True

            End If

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "疵混入率表示エラー [" & ex.Message & "]")
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        Return bRet

    End Function


    ''' <summary>
    ''' データセット
    ''' </summary>
    '''<param name="nOutCutNo">分割No</param>
    '''<param name="typZiseki">プロコン実績データ</param>
    ''' <remarks></remarks>
    Private Sub SetData(ByVal nOutCutNo As Integer, ByVal typZiseki As PRO_TYPE_ZISEKI)

        Dim ii As Integer   'ループカウンタ
        Dim jj As Integer   'ループカウンタ

        '命令No
        lblMeiNo.Text = tMod.ByteToString(typZiseki.data.cMeiNo)

        '生産NO
        lblProductNo.Text = tMod.ByteToString(typZiseki.data.cCoilNo)

        '出側カット順番（5CGLでは非表示）
        lblOutCutNo.Text = nOutCutNo

        '実績区分
        If 0 = typZiseki.data.nZiMode Then
            lblZisekiDiv.Text = "正常"
            lblZisekiDiv.BackColor = Color.Blue
            lblZisekiDiv.ForeColor = Color.Yellow
        Else
            lblZisekiDiv.Text = "異常"
            lblZisekiDiv.BackColor = Color.Red
            lblZisekiDiv.ForeColor = Color.Yellow
        End If

        '収集区分
        If 0 = typZiseki.data.nKikiMode Then
            lblColectDiv.Text = "正常"
            lblColectDiv.BackColor = Color.Blue
            lblColectDiv.ForeColor = Color.Yellow
        Else
            lblColectDiv.Text = "異常"
            lblColectDiv.BackColor = Color.Red
            lblColectDiv.ForeColor = Color.Yellow
        End If

        '項目見出し設定
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iRate).Value = "疵混入率 [％]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No01).Value = "疵種０１ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No02).Value = "疵種０２ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No03).Value = "疵種０３ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No04).Value = "疵種０４ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No05).Value = "疵種０５ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No06).Value = "疵種０６ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No07).Value = "疵種０７ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No08).Value = "疵種０８ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No09).Value = "疵種０９ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No10).Value = "疵種１０ [個]"


        '桁数確認用
        For jj = EM_MRATE_COL_ID.iGrade_No01 To EM_MRATE_COL_ID.iGrade_No06
            dgvMixedRate(jj, EM_MRATE_ROW_ID.iRate).Value = Format(typZiseki.data.grade_inf(jj - 1).nMixedRate / 100, "#0.00")
            dgvMixedRate(jj, EM_MRATE_ROW_ID.iRate).Style.Alignment = DataGridViewContentAlignment.MiddleRight
            For ii = EM_MRATE_ROW_ID.iType_No01 To EM_MRATE_ROW_ID.iType_No10
                dgvMixedRate(jj, ii).Value = typZiseki.data.grade_inf(jj - 1).nTypeCnt(ii - 1)
                dgvMixedRate(jj, ii).Style.Alignment = DataGridViewContentAlignment.MiddleRight
            Next ii
        Next jj

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
    ''' セル選択管理No
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvMixedRate_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvMixedRate.SelectionChanged

        '選択を解除
        dgvMixedRate.CurrentCell.Selected = False

    End Sub

    ''' <summary>
    ''' 該当管理Noのカット実績のIDを全て取得
    ''' </summary>
    ''' <remarks></remarks>
    Private Function GetCutID() As Boolean


        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文
        Dim nCnt As Integer = 0    'カウンタ

        strSQL = ""
        strSQL &= " SELECT ID"
        strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
        strSQL &= " WHERE 管理No = '" & m_strKizuken & "'"
        strSQL &= " ORDER BY 登録日付 ASC"

        'DBオープン
        If Not tcls_DB.Open() Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_STATUS_MASTER & "]")
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return False


        Try

            m_dctID.Clear()
            ' データ取得
            While sqlRead.Read()

                m_dctID.Add(nCnt, sqlRead.GetString(0))
                nCnt += 1
            End While

            Return True
        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "疵混入率表示エラー [" & ex.Message & "]")
            Return False
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try



    End Function

    ''' <summary>
    ''' 前項ボタン押下処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnPre_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnPre.Click

        m_nIDCnt -= 1

        'データ表示
        Call DspData(m_dctID(m_nIDCnt))
        SetPreNextBotton()

    End Sub

    ''' <summary>
    ''' 次項ボタン押下処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnNext_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnNext.Click

        m_nIDCnt += 1

        'データ表示
        Call DspData(m_dctID(m_nIDCnt))
        SetPreNextBotton()
    End Sub


    ''' <summary>
    ''' 前項、次項ボタンの有効、無効を設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetPreNextBotton()

        If m_nIDCnt = 0 Then

            btnPre.Enabled = False
        Else
            btnPre.Enabled = True
        End If

        If m_nIDCnt = m_dctID.Count - 1 Then
            btnNext.Enabled = False
        Else
            btnNext.Enabled = True
        End If

        lblPage.Text = CStr(m_nIDCnt + 1) & "/ " & CStr(m_dctID.Count)
    End Sub

End Class
