Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmPopKizuList

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 公開イベント
    Public Event tEv_KizuClick(ByVal pos As Point)						' クリック (mm座標系) 

    Public Event tEv_ReSetKizuDsp()										' 疵表示再セット



    Private Enum EM_DEF
        em_No = 0		' 疵No
        em_T			' 疵種
        em_G			' ｸﾞﾚｰﾄﾞ
        em_Pos			' T～
        em_C			' C～ (左-)
        em_L			' 左～
        em_R			' 右～
        em_End
    End Enum

    Private Enum EM_ATTR
        em_Angle = 0	' 角度
        em_Enum			' Enum
        em_ID			' EdasysID
        em_Name			' 名称
        em_Val			' 特徴量
        em_End
    End Enum



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数

    ' クラス
    Private mcls_pParam As clsParamManager = Nothing					' パラメータ管理クラス (ポインタ)
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing				' RPIファイル表示クラス (ポインタ)

    ' データ
    Private m_lstDetect As List(Of DATA_DETECT_INF) = Nothing			' 疵情報リスト
    ' もろもろ
    Private m_bDspOk As Boolean = False									' 散布図にちゃんとデータが表示しているとき

    Private m_Session As String											' iniファイルセッション
    Private m_nAttrNum As Integer										' 特徴量テーブル数 (1角度なら、基本+角度1= 2となる)
    Private m_nAttrCnt(MC_MAX_IMG + 1) As Integer						' 各特徴量数
    Private m_nActiveNo As Integer										' 表示中の疵No



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ プロパティ
    ''' <summary>
    ''' パラメータクラスセット
    ''' </summary>
    ''' <param name="cls">計算クラス</param>
    Public Sub SetCls_Param(ByRef cls As clsParamManager)
        mcls_pParam = cls
    End Sub

    ''' <summary>
    ''' RPIクラス
    ''' </summary>
    ''' <param name="cls">RPIクラス</param>
    ''' <remarks></remarks>
    Public Sub SetCls_RpiView(ByRef cls As tClass_ViewRpiThread)
        mcls_pRpiView = cls
    End Sub

    ''' <summary>
    ''' 疵情報リスト
    ''' </summary>
    ''' <param name="lst">疵情報リスト</param>
    ''' <remarks></remarks>
    Friend Sub SetData_LstDetect(ByRef lst As List(Of DATA_DETECT_INF))
        m_lstDetect = lst
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本
    ''' <summary>
    ''' ロード
    ''' </summary>
    Private Sub frmPopKizuList_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        Call DgvInitDefect(dgvDefect)
        Call DgvInitAttr(dgvAttr)
    End Sub



    ''' <summary>
    ''' マップ描画
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub MapDisp()

        If Not mcls_pRpiView.IsRpi Then
            m_bDspOk = False

            dgvDefect.RowCount = 1		' 0はダメ
            dgvAttr.RowCount = 1
            m_nActiveNo = 0
            Return
        End If

        '''' ここからは、RPIファイルが開いているときのみ
        If Me.Visible Then
            Call DgvAttr_RowSet(dgvAttr)
            Call SetDefectInf()
        End If

        m_bDspOk = True
    End Sub

    ''' <summary>
    ''' 新しいコイル表示
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub NewCoil()
        Call SetListType()
        Call SetListGr()
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ デザイン
#Region "デザイン"


    ''' <summary>
    ''' 動作定義
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub DgvInitDefect(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)

        Call dgv.dgvInitialize(dgv)

        With dgv

            .ScrollBars = ScrollBars.Both								   ' スクロールバーの表示
            .AllowUserToResizeColumns = True								' ユーザー 列サイズ変更


            ' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue	' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().WrapMode = DataGridViewTriState.True


            '' 列定義
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft

            '
            Using clm As New DataGridViewButtonColumn
                clm.HeaderText = "疵No"
                clm.Width = 48
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "疵種"
                clm.Width = 80
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "Gr"
                clm.Width = 50
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "T～"
                clm.Width = 60
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "C～"
                clm.Width = 50
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "左～"
                clm.Width = 40
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "右～"
                clm.Width = 40
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
        End With
    End Sub
    Private Sub DgvInitAttr(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)

        Call dgv.dgvInitialize(dgv)

        With dgv
            .ScrollBars = ScrollBars.Both								   ' スクロールバーの表示
            .AllowUserToResizeColumns = True								' ユーザー 列サイズ変更


            ' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue	' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().WrapMode = DataGridViewTriState.False


            '' 列定義
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft

            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "角度"
                clm.Width = 36
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "Enum"
                clm.Width = 40
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.Visible = False
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "EdaID"
                clm.Width = 40
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "名称"
                clm.Width = 150
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "特徴量"
                clm.Width = 150
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
        End With
    End Sub


    ''' <summary>
    ''' 特徴量名称セット
    ''' </summary>
    Private Sub DgvAttr_RowSet(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        m_Session = "DB_" & mcls_pRpiView.GetRpi_THeader.cSysName
        m_nAttrNum = tMod.tMod_IniFile.ReadIniInt(m_Session, "ATTR_NUM", TASKINI_NAME)
        m_nActiveNo = 0
        For ii As Integer = 0 To m_nAttrCnt.Length - 1
            m_nAttrCnt(ii) = 0
        Next

        If 0 = m_nAttrNum Then Return
        m_nAttrNum = m_nAttrNum + 1	' 基本特徴量分は必ずある

        '==============================
        ' 特徴量名称セット
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim sSQL As String

        'SQL文作成
        sSQL = tMod.tMod_IniFile.ReadIniStr(m_Session, "SELECT_ATTR_NAME", TASKINI_NAME)

        Try
            ' DB接続
            If Not tcls_DB.Open() Then Return

            'SQL実行
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then Return


            ' データ取得
            dgv.Enabled = False
            dgv.RowCount = 0
            Dim nCnt As Integer = 0
            While sqlRead.Read()
                If m_nAttrNum - 1 < sqlRead.GetInt32(0) Then Continue While
                m_nAttrCnt(sqlRead.GetInt32(0)) += 1	' 行数加算

                dgv.RowCount = nCnt + 1

                dgv(EM_ATTR.em_Angle, nCnt).Value = sqlRead.GetInt32(0)
                dgv(EM_ATTR.em_Enum, nCnt).Value = sqlRead.GetInt32(1)
                dgv(EM_ATTR.em_ID, nCnt).Value = sqlRead.GetInt32(2)
                dgv(EM_ATTR.em_Name, nCnt).Value = sqlRead.GetString(3)
                dgv(EM_ATTR.em_Val, nCnt).Value = ""


                nCnt += 1
            End While
        Catch ex As Exception
            Return
        Finally
            sqlRead.Close()
            tcls_DB.Close()
            tcls_DB.Dispose()

            dgv.Enabled = True
            dgv.Refresh()
        End Try
    End Sub

#End Region

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ データセット
    ''' <summary>
    ''' 疵情報をセット
    ''' </summary>
    Private Sub SetDefectInf()
        dgvDefect.Enabled = False
        dgvDefect.RowCount = m_lstDetect.Count
        For ii As Integer = 0 To m_lstDetect.Count - 1
            dgvDefect.Rows(ii).Visible = m_lstDetect(ii).bDsp
            dgvDefect(EM_DEF.em_No, ii).Value = m_lstDetect(ii).nNo
            dgvDefect(EM_DEF.em_T, ii).Value = mcls_pParam.GetParamType(m_lstDetect(ii).tId).sSName
            dgvDefect(EM_DEF.em_G, ii).Value = mcls_pParam.GetParamGrade(m_lstDetect(ii).gId).sName
            dgvDefect(EM_DEF.em_Pos, ii).Value = String.Format("{0:0.0}", m_lstDetect(ii).nY / 1000)
            dgvDefect(EM_DEF.em_C, ii).Value = m_lstDetect(ii).nXc
            dgvDefect(EM_DEF.em_L, ii).Value = m_lstDetect(ii).nX
            dgvDefect(EM_DEF.em_R, ii).Value = m_lstDetect(ii).nXws
        Next
        dgvDefect.Enabled = True
        dgvDefect.Refresh()
    End Sub



    ''' <summary>
    ''' 特徴量表示
    ''' </summary>
    ''' <param name="val">選択疵情報</param>
    Private Sub SetAttrData(ByVal val As DATA_DETECT_INF)
        Dim nMen As Integer = mcls_pRpiView.GetRpi_THeader.nMen
        If 0 = m_nAttrNum Then Return

        '==============================
        ' 特徴量名称セット
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sSQL As String

        Try
            ' DB接続
            If Not tcls_DB.Open() Then Return

            Dim nRow As Integer = 0						' データグリッドビューの行
            For ii As Integer = 0 To m_nAttrNum - 1

                'SQL文作成
                Dim sKey As String = String.Format("ATTR_INF_{0}_{1}", nMen, ii)
                sSQL = tMod.tMod_IniFile.ReadIniStr(m_Session, sKey, TASKINI_NAME)
                sSQL = String.Format(sSQL, mcls_pRpiView.GetRpi_THeader.cKizukenNo, Val.nNo)

                'SQL実行
                Using sqlRead As SqlClient.SqlDataReader = tcls_DB.SelectExecute(sSQL)
                    If sqlRead Is Nothing Then Return

                    While sqlRead.Read()
                        For jj As Integer = 0 To m_nAttrCnt(ii) - 1
                            dgvAttr(EM_ATTR.em_Val, nRow).Value = sqlRead.GetDouble(2 + jj)   ' 2は主キー部分
                            nRow += 1
                        Next
                    End While
                End Using
            Next

        Catch ex As Exception
            Return
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
    End Sub


    ''' <summary>
    ''' 疵情報 データグリッドクリック
    ''' </summary>
    Private Sub dgvDefect_CellMouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs) Handles dgvDefect.CellMouseDown
        Dim dgv As DataGridView = CType(sender, DataGridView)
        Dim row As Integer = e.RowIndex

        ' 行がクリックされていなければ終了
        If e.RowIndex < 0 Then Exit Sub

        ' 列インデックスでどの列がクリックされたか判別
        Select Case e.ColumnIndex
            Case EM_DEF.em_No		' 疵No
                RaiseEvent tEv_kizuClick(New Point(m_lstDetect(row).nFrameXc, m_lstDetect(row).nY))
                If m_nActiveNo <> m_lstDetect(row).nNo Then Call SetAttrData(m_lstDetect(row))

            Case Else
                If m_nActiveNo <> m_lstDetect(row).nNo Then Call SetAttrData(m_lstDetect(row))
        End Select
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 疵種、グレード
    ''' <summary>
    ''' 疵種リスト一覧表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetListType()
        clbType.Items.Clear()

        For Each wk As clsParamManager.PARAM_TYPE In mcls_pParam.GetParamType()
            Dim item As TYP_ITEM
            item.sString = wk.sName
            item.nKey = wk.nEdasysID
            clbType.Items.Add(item, True)
        Next
    End Sub
    Private Sub SetListGr()
        clbGr.Items.Clear()

        For Each wk As clsParamManager.PARAM_GRADE In mcls_pParam.GetParamGrade()
            Dim item As TYP_ITEM
            item.sString = wk.sName
            item.nKey = wk.nEdasysID
            clbGr.Items.Add(item, True)
        Next
    End Sub

    ''' <summary>
    ''' 全選択、全解除
    ''' </summary>
    Private Sub AllSetList(ByVal clb As CheckedListBox, ByVal bMode As Boolean)
        For ii As Integer = 0 To clb.Items.Count - 1
            clb.SetItemChecked(ii, bMode)
        Next
    End Sub
    Private Sub btnTypeAllOk_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllOk.Click
        AllSetList(clbType, True)
    End Sub
    Private Sub btnTypeAllNg_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllNg.Click
        AllSetList(clbType, False)
    End Sub
    Private Sub btnGrAllOk_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnGrAllOk.Click
        AllSetList(clbGr, True)
    End Sub
    Private Sub btnGrAllNg_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnGrAllNg.Click
        AllSetList(clbGr, False)
    End Sub


    ''' <summary>
    ''' 疵表示フィルタ 実行
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub btnExec_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExec.Click
        Call FilterList()

        ' 各種反映
        '	Call SetDefectInf()
        RaiseEvent tEv_ReSetKizuDsp()
    End Sub


    ''' <summary>
    ''' 疵表示フィルタ
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub FilterList()
        Dim nCnt As Integer

        '''' 表示対象のEdasysID一覧
        Dim tId(clbType.CheckedIndices.Count - 1) As Integer
        Dim gId(clbGr.CheckedIndices.Count - 1) As Integer
        ' 疵種
        nCnt = 0
        For Each item As TYP_ITEM In clbType.CheckedItems
            tId(nCnt) = item.nKey
            nCnt += 1
        Next

        ' Gr
        nCnt = 0
        For Each item As TYP_ITEM In clbGr.CheckedItems
            gId(nCnt) = item.nKey
            nCnt += 1
        Next



        '''' 表示対象を選別
        '		nCnt = 0
        For ii As Integer = 0 To m_lstDetect.Count - 1		' 値を書き換えるため、コレクションではNG
            Dim data As DATA_DETECT_INF = m_lstDetect(ii)

            data.bDsp = False
            If -1 = Array.IndexOf(gId, data.gId) Then GoTo Ending
            If -1 = Array.IndexOf(tId, data.tId) Then GoTo Ending

            ' ここまで来たら表示対象
            data.bDsp = True

Ending:
            ' 更新
            '			m_lstDetect(nCnt) = data
            '			nCnt += 1
            m_lstDetect(ii) = data
        Next
    End Sub



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 特徴量出力
    Private Sub btnCsvKizu_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCsvKizu.Click
        Call CsvOut(dgvDefect, CurDir, "疵.csv")
    End Sub

    Private Sub btnCsvAttr_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCsvAttr.Click
        Call CsvOut(dgvAttr, CurDir, "特徴量.csv")
    End Sub



    ''' <summary>
    ''' CSV出力
    ''' </summary>
    ''' <param name="strFolder">保存ファイルのフォルダパス</param>
    ''' <param name="strName">保存ファイルのファイル名</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub CsvOut(ByVal dgv As DataGridView, ByVal strFolder As String, ByVal strName As String)
        Dim strFPath As String
        Dim strMsg As String = ""
        Dim strData As String = ""						'1行のデータ
        Dim strWk() As String
        Dim strWkData As String

        '' フルパス
        strFPath = IO.Path.Combine(strFolder, strName)

        '' フォルダの有無チェック(無かったら作ろう)
        tMod.CheckFolder(strFolder)

        '' CSV出力
        Try
            Using cls_Stram As New System.IO.StreamWriter(strFPath, False, System.Text.Encoding.Default)
                'ヘッダを出力
                strData = ""
                For ii As Integer = 0 To dgv.ColumnCount - 1

                    '改行コードをすべて取り除く
                    strWk = dgv.Columns(ii).HeaderText.Split(Chr(10), Chr(13))
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
                For ii As Integer = 0 To dgv.Rows.Count - 1
                    If Not dgv.Rows(ii).Visible Then Continue For

                    strData = ""
                    For jj As Integer = 0 To dgv.ColumnCount - 1
                        '項目をTabで連結する
                        If dgv(jj, ii).Value Is Nothing Then
                            strData &= ","
                        Else
                            strData &= dgv(jj, ii).Value.ToString & ","
                        End If


                    Next jj

                    '最後のタブを排除
                    strData = strData.Remove(strData.Length - 1, 1)

                    cls_Stram.WriteLine(strData)
                Next ii

            End Using
            strMsg = System.IO.Path.GetFullPath(strFPath) & "にCSVファイルを保存しました。"
        Catch ex As Exception
            strMsg = "CSV出力に失敗しました。"
        End Try

        '''' ガイダンス表示
        MsgBox(strMsg)
    End Sub

End Class