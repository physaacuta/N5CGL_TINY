'*********************************************************************************
' コイル情報画面(OPL)
'	[Ver]
'		Ver.01    2012/07/13  初版
'
'	[メモ]
'
'*********************************************************************************
Option Strict On

Imports tClassLibrary

Public Class frmDspCoil
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_SqlGetRow(3 - 1) As DataRow           ' 0:次コイル、1:現コイル、2:前コイル

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 列挙
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' グリッド　列設定
    Private Enum EM_GRID_COL_ID
        em_txtName = 0      '名称
        em_txtData          'データ
        em_txtInfo          '内容
        em_Cnt              '数
    End Enum


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const m_WidthMin As Integer = 661       ' フォームサイズ変更時、横幅の最小値
    Private Const m_HeightMin As Integer = 305      ' フォームサイズ変更時、高さの最小値
    Private m_cKizukenNo As String                  ' 出側管理No
    Private m_dctItem As Dictionary(Of String, String)

    ''' <summary>
    ''' フォームアクティブ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDspCoil_Activated(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Activated

        '' 移動可能
        'Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
        'Me.MaximizeBox = False
        'Me.MinimizeBox = False
        'Me.tProp_EndButtonEnable = False

        '' 縮小できるサイズの限界を指定する
        'Me.MinimumSize = New Size(m_WidthMin, m_HeightMin)

    End Sub


    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDspCoil_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        'タイトル設定
        Call SetTitleName(Me)

        'ポップアップ画面 低解像度対応
        Call SetPcHeight(Me)

        ' 初期化
        For ii As Integer = 0 To m_SqlGetRow.Length - 1
            m_SqlGetRow(ii) = Nothing
        Next ii

        ' ファンクションボタン定義
        MyBase.AddFunctionButton(BtnCoilNext, 0)
        MyBase.AddFunctionButton(BtnCoilBack, 1)
        MyBase.AddFunctionButton(BtnClose, 11)

        If BtnCSV.Visible = True Then
            MyBase.AddFunctionButton(BtnCSV, 10)
        End If

        ' dgv初期設定
        Call DgvInit(dgvCoil)

        ' DB 項目名取得
        Call GetclmSQL()

        ' SQL取得(初期表示 最新コイルの取得)
        Call GetSql(0, "")

        ' DB データを表示
        Call SqlDsp()

    End Sub

    ''' <summary>
    ''' フォームリサイズ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDspCoil_ResizeEnd(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.ResizeEnd

        ' フォームサイズが変更されても横幅をを均等に割り付けるための処理
        'dgvCoil.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill
        dgvCoil.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None

    End Sub

    ''' <summary>
    ''' ファンクションボタン
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmDspCoil_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Dim nMode As Integer                    ' SQL取得モード
        Dim strKanriNo As String                ' 管理No

        ' ファンクション切替
        Select Case FncNo                       ' ※0オリジン

            Case 0                              ' F01 次コイル
                nMode = 1

                ' 管理No取得
                strKanriNo = m_SqlGetRow(0).Item("管理No").ToString

                ' コイル情報取得
                Call GetSql(nMode, strKanriNo)

                ' DB データを表示
                Call SqlDsp()

            Case 1                              ' F02 前コイル
                nMode = 2

                ' 管理No取得
                strKanriNo = m_SqlGetRow(2).Item("管理No").ToString

                ' コイル情報取得
                Call GetSql(nMode, strKanriNo)

                ' DB データを表示
                Call SqlDsp()

            Case 10                             ' F11 CSV出力
                Call CsvOut(CSV_PATH, "コイル情報画面.csv", True)
                
            Case 11                             ' F12 閉じる
                Me.Dispose()
        End Select

    End Sub

    ''' <summary>
    ''' DB項目名取得
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetclmSQL()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlAdpter As New SqlClient.SqlDataAdapter
        Dim sqlData As New DataSet
        Dim strSQL As String                    ' SQL文
        Dim ii As Integer                       ' INDEX

        Try
            ' DBオープン
            If Not tcls_DB.Open() Then Exit Sub

            ' SQL文作成
            strSQL = ""
            strSQL &= " SELECT NAME"
            strSQL &= " FROM SYS.COLUMNS"
            strSQL &= " WHERE OBJECT_ID = OBJECT_ID('" & DB_COIL_INF & "')"

            ' Select文実行 
            sqlData.Clear()
            sqlAdpter = tcls_DB.SelectAdapter(strSQL)

            sqlAdpter.Fill(sqlData)

            ' DB項目名称セット
            Dim sqlGet() As DataRow = sqlData.Tables(0).Select()

            ' dgv表示処理
            dgvCoil.RowCount = sqlData.Tables(0).Select().Length
            For ii = 0 To dgvCoil.RowCount - 1
                dgvCoil(0, ii).Value = sqlGet(ii).Item("NAME")

                If m_dctItem.ContainsKey(CStr(dgvCoil(0, ii).Value)) Then
                    dgvCoil(2, ii).Value = m_dctItem(CStr(dgvCoil(0, ii).Value))
                End If
            Next ii

            sqlAdpter.Dispose()

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "[" & DB_COIL_INF & "] 名称取得失敗")
        Finally
            tcls_DB.Cancel()
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub



    ''' <summary>
    ''' SQL文発行
    ''' </summary>
    ''' <param name="Mode">SQL文取得モード</param>
    ''' <param name="KanriNo">管理No</param>
    ''' <remarks></remarks>
    Private Sub GetSql(ByVal Mode As Integer, ByVal KanriNo As String)

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlAdpter As New SqlClient.SqlDataAdapter
        Dim sqlData As New DataSet
        Dim strSQL As String                    ' SQL文

        Try
            ' DBオープン
            If Not tcls_DB.Open() Then Exit Sub

            ' 0:初期表示 1:次コイル 2:前コイル
            Select Case Mode

                Case 0
                    ' SQL文作成
                    If m_cKizukenNo IsNot Nothing Then
                        strSQL = ""
                        strSQL &= " SELECT TOP 2 INF.*"
                        strSQL &= " FROM " & DB_COIL_INF & " as INF"
                        strSQL &= " WHERE INF.管理No = '" & m_cKizukenNo & "'"
                        strSQL &= " ORDER BY INF.管理No DESC"
                    Else
                        strSQL = ""
                        strSQL &= " SELECT TOP 2 *"
                        strSQL &= " FROM " & DB_COIL_INF
                        strSQL &= " ORDER BY 管理No DESC"
                    End If

                    ' Select文実行 
                    sqlData.Clear()
                    sqlAdpter = tcls_DB.SelectAdapter(strSQL)
                    sqlAdpter.Fill(sqlData)

                    sqlAdpter.Dispose()

                    ' DBの値を取得
                    Dim sqlGet() As DataRow = sqlData.Tables(0).Select()
                    For ii As Integer = 1 To m_SqlGetRow.Length - 1
                        If ii <= sqlGet.Length Then
                            m_SqlGetRow(ii) = sqlGet(ii - 1)
                        Else
                            m_SqlGetRow(ii) = Nothing
                        End If
                    Next ii

                Case 1
                    ' SQL文作成
                    If m_cKizukenNo IsNot Nothing Then
                        strSQL = ""
                        strSQL &= " SELECT TOP 2 INF.*"
                        strSQL &= " FROM " & DB_COIL_INF & " as INF"
                        strSQL &= " WHERE INF.管理No = '" & m_cKizukenNo & "'"
                        strSQL &= " ORDER BY INF.管理No DESC"
                    Else
                        strSQL = ""
                        strSQL &= " SELECT TOP 2 *"
                        strSQL &= " FROM " & DB_COIL_INF
                        strSQL &= " WHERE 管理No >= ('" & KanriNo & "') "
                        strSQL &= " ORDER BY 管理No ASC"
                    End If

                    ' Select文実行 
                    sqlData.Clear()
                    sqlAdpter = tcls_DB.SelectAdapter(strSQL)
                    sqlAdpter.Fill(sqlData)

                    sqlAdpter.Dispose()

                    ' DBの値を取得
                    Dim sqlGet() As DataRow = sqlData.Tables(0).Select()
                    m_SqlGetRow(2) = m_SqlGetRow(1)
                    For ii As Integer = 1 To m_SqlGetRow.Length - 1
                        If ii <= sqlGet.Length Then
                            m_SqlGetRow(2 - ii) = sqlGet(ii - 1)
                        Else
                            m_SqlGetRow(2 - ii) = Nothing
                        End If
                    Next ii

                Case 2
                    ' SQL文作成
                    If m_cKizukenNo IsNot Nothing Then
                        strSQL = ""
                        strSQL &= " SELECT TOP 2 INF.*"
                        strSQL &= " FROM " & DB_COIL_INF & " as INF"
                        strSQL &= " WHERE INF.管理No = '" & m_cKizukenNo & "'"
                        strSQL &= " ORDER BY INF.管理No DESC"
                    Else
                        strSQL = ""
                        strSQL &= " SELECT TOP 2 *"
                        strSQL &= " FROM " & DB_COIL_INF
                        strSQL &= " WHERE 管理No <= ('" & KanriNo & "') "
                        strSQL &= " ORDER BY 管理No DESC"
                    End If

                    ' Select文実行 
                    sqlData.Clear()
                    sqlAdpter = tcls_DB.SelectAdapter(strSQL)
                    sqlAdpter.Fill(sqlData)

                    sqlAdpter.Dispose()

                    ' DBの値を取得
                    Dim sqlGet() As DataRow = sqlData.Tables(0).Select()
                    m_SqlGetRow(0) = m_SqlGetRow(1)
                    For ii As Integer = 1 To m_SqlGetRow.Length - 1
                        If ii <= sqlGet.Length Then
                            m_SqlGetRow(ii) = sqlGet(ii - 1)
                        Else
                            m_SqlGetRow(ii) = Nothing
                        End If
                    Next ii

            End Select

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "[" & DB_COIL_INF & "] データ取得失敗")
        Finally
            tcls_DB.Cancel()
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub


    ''' <summary>
    ''' DBデータ表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SqlDsp()

        Try
            If Not m_SqlGetRow(1) Is Nothing Then

                ' dgv表示処理
                For ii As Integer = 0 To m_SqlGetRow(1).ItemArray.Length - 1
                    dgvCoil(1, ii).Value = m_SqlGetRow(1).ItemArray(ii)
                Next ii

            End If

            ' 次コイル、前コイルチェック
            Call CoilCheck()

            If BtnCSV.Visible = True Then
                BtnCSV.Enabled = True
            End If


        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "データ表示失敗")

        End Try

    End Sub

    ''' <summary>
    ''' 次コイル、前コイル有無チェック
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CoilCheck()

        ' 次コイルがあるか判断
        If m_SqlGetRow(0) Is Nothing Then
            BtnCoilNext.Enabled = False
        Else
            BtnCoilNext.Enabled = True
        End If

        ' 前コイルがあるか判断
        If m_SqlGetRow(2) Is Nothing Then
            BtnCoilBack.Enabled = False
        Else
            BtnCoilBack.Enabled = True
        End If

    End Sub

    ''' <summary>
    ''' 動作定義
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub DgvInit(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)

        Call dgv.dgvInitialize(dgv)

        With dgv

            .ScrollBars = ScrollBars.Both                                       ' 垂直スクロールバーの表示
            .ColumnHeadersVisible = False                                       ' 列ヘッダ表示
            .AllowUserToResizeColumns = True                                    ' ユーザー 列サイズ変更

            ' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)

            .DefaultCellStyle().ForeColor = Color.Black

            ' フォームのサイズに合わせて横幅を均等に割り付ける処理
            'dgvCoil.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill
            dgvCoil.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None

            '' 列定義
            .ColumnCount = EM_GRID_COL_ID.em_Cnt        'textbox のみを追加する場合はこれの方が楽
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft

            '列0 (アドレス) 名称
            .Columns(EM_GRID_COL_ID.em_txtName).HeaderText = "名称"
            .Columns(EM_GRID_COL_ID.em_txtName).ReadOnly = True
            .Columns(EM_GRID_COL_ID.em_txtName).Width = 240
            .Columns(EM_GRID_COL_ID.em_txtName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_GRID_COL_ID.em_txtName).SortMode = DataGridViewColumnSortMode.NotSortable

            '列1 (アドレス) データ
            .Columns(EM_GRID_COL_ID.em_txtData).HeaderText = "データ"
            .Columns(EM_GRID_COL_ID.em_txtData).ReadOnly = True
            .Columns(EM_GRID_COL_ID.em_txtData).Width = 220
            .Columns(EM_GRID_COL_ID.em_txtData).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_GRID_COL_ID.em_txtData).SortMode = DataGridViewColumnSortMode.NotSortable

            '列2 (アドレス) 説明
            .Columns(EM_GRID_COL_ID.em_txtInfo).HeaderText = "説明"
            .Columns(EM_GRID_COL_ID.em_txtInfo).ReadOnly = True
            .Columns(EM_GRID_COL_ID.em_txtInfo).Width = 400
            .Columns(EM_GRID_COL_ID.em_txtInfo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_GRID_COL_ID.em_txtInfo).SortMode = DataGridViewColumnSortMode.NotSortable

        End With

    End Sub

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="cKizukenNo"></param>
    ''' <remarks></remarks>
    Public Sub New(Optional ByVal cKizukenNo As String = Nothing)

        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        m_cKizukenNo = cKizukenNo       ' 管理No
        If m_cKizukenNo Is Nothing Then
            BtnCSV.Visible = False
        End If

        m_dctItem = New Dictionary(Of String, String)
        m_dctItem.Add("コイル長", "[m]")
        m_dctItem.Add("材料板厚", "[μm]")
        m_dctItem.Add("材料板幅", "[mm]")
        m_dctItem.Add("注文板厚", "[μm]")
        m_dctItem.Add("注文板幅", "[mm]")
        m_dctItem.Add("現工程トリム幅", "[mm]")
        m_dctItem.Add("SPM伸び率", "[10-2%]")
        m_dctItem.Add("TLV伸び率", "[10-2%]")
        m_dctItem.Add("入側フロント端板長さ", "[mm]")
        m_dctItem.Add("入側テール端板長さ", "[mm]")
        m_dctItem.Add("SPMワークロール径上", "[10-1mm]")
        m_dctItem.Add("SPMワークロール径下", "[10-1mm]")
        m_dctItem.Add("クロメート命令_種類", "厚：E、薄：C、無し：△")
        m_dctItem.Add("クロメート命令_有無", "有り：0、無し：X")
        m_dctItem.Add("品種", "工程記号1桁目（H,Z,G,A,B,S,U,Q）")
        m_dctItem.Add("メッキ種類", "工程記号2桁目（-,X,Y,A,B,C）")
        m_dctItem.Add("主材質", "工程記号3,4桁目（1*,2*,3*,4*,5*,6*,7*）")
        m_dctItem.Add("ディファレンシャルマーク", "工程記号5桁目（-,Z,A～Y）")
        m_dctItem.Add("原板区分", "工程記号6桁目（L,H,C）")
        m_dctItem.Add("メッキサイクル", "工程記号7桁目（F,A,M,N,P,S,T,H）")
        m_dctItem.Add("レベラー_インラインSPM", "工程記号8桁目（0～6,A,B,C,D,E,F,M,X）")
        m_dctItem.Add("化学処理塗油", "工程記号9桁目（K,W,M,S,Y,N,E,C,U,R,X）")
        m_dctItem.Add("オフラインSPM圧下率", "工程記号10桁目（F,2,4,6,8,0,D,L,P,T,B,A,H,E,J,K）")
        m_dctItem.Add("メッキ区分", "工程記号11桁目（1,2,3）")
        m_dctItem.Add("表面処理1桁目", "工程記号12桁目（L処理：L,無処理：M,ｸﾛﾒｰﾄ：C）")
        m_dctItem.Add("表面処理2桁目", "工程記号13桁目")
        m_dctItem.Add("塗油", "工程記号14桁目")
        m_dctItem.Add("工程記号予備", "工程記号15桁目")
        m_dctItem.Add("予備コイル情報", "工程記号16桁目")
        m_dctItem.Add("SPM圧下有無", "有り：0、無し：X")
        m_dctItem.Add("品種区分", "E：EG（計画次工程=87EGL）、M:冷薄（E以外）")
        m_dctItem.Add("GA_CR区分", "G：GA材、C：CR材")
        m_dctItem.Add("SPM圧下実績有無", "有り：0、無し：X")

    End Sub

    ''' <summary>
    ''' CSV出力
    ''' </summary>
    ''' <param name="strFolder">保存ファイルのフォルダパス</param>
    ''' <param name="strName">保存ファイルのファイル名</param>
    ''' <param name="bDelComma">カンマを除去</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub CsvOut(ByVal strFolder As String, ByVal strName As String, Optional ByVal bDelComma As Boolean = False)
        Dim strFPath As String
        Dim strMsg As String = ""
        Dim strData As String = ""                      '1行のデータ
        Dim strWk() As String
        Dim strWkData As String

        '' フルパス
        strFPath = IO.Path.Combine(strFolder, strName)

        '' フォルダの有無チェック(無かったら作ろう)
        tMod.CheckFolder(strFolder)

        '' CSV出力
        Using cls_Stram As New tClass.tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
            Try
                ' オープン
                cls_Stram.Open()

                'ヘッダを出力
                strData = ""
                For ii As Integer = 0 To dgvCoil.ColumnCount - 1

                    '改行コードをすべて取り除く
                    strWk = dgvCoil.Columns(ii).HeaderText.Split(Chr(10), Chr(13))
                    strWkData = ""
                    For jj As Integer = 0 To strWk.Length - 1
                        strWkData &= strWk(jj).Trim(Chr(10), Chr(13))
                    Next jj

                    strData &= Trim(strWkData) & ","
                Next ii

                '最後のタブを排除
                strData = strData.Remove(strData.Length - 1, 1)

                cls_Stram.WriteLine(strData)

                'データ出力
                For ii As Integer = 0 To dgvCoil.Rows.Count - 1
                    strData = ""
                    For jj As Integer = 0 To dgvCoil.ColumnCount - 1
                        '項目をTabで連結する
                        If dgvCoil(jj, ii).Value IsNot Nothing Then
                            If True = bDelComma Then
                                strData &= dgvCoil(jj, ii).Value.ToString.Replace(",", "")
                            Else
                                strData &= dgvCoil(jj, ii).Value.ToString
                            End If
                        End If
                        strData &= ","
                    Next jj

                    '最後のタブを排除
                    strData = strData.Remove(strData.Length - 1, 1)

                    cls_Stram.WriteLine(strData)
                Next ii

                ' クローズ
                cls_Stram.Close()
                strMsg = System.IO.Path.GetFullPath(strFPath) & "にCSVファイルを保存しました。"

            Catch ex As Exception
                strMsg = "CSV出力に失敗しました。"
            End Try
        End Using

        '''' ガイダンス表示
        Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, Me.Text)
            frmMsg.ShowDialog()
        End Using
    End Sub

End Class
