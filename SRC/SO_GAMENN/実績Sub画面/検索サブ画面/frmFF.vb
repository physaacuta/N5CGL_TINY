'*********************************************************************************
'　検査実績FFクラス
'	[Ver]
'		Ver.01    2007/09/22  初版
'
'	[メモ]
'		
'*********************************************************************************
'Option Strict On
Option Strict Off

Imports tClassLibrary

Public Class frmFF

    Private m_mode As Integer               '要求モード(1:オンライン用、0:ツール用）
    Private m_strKizukenNo As String        '疵検管理No

    Private Const DgvMaxCol_MRare As Integer = 11            'データ列MAX

    ' グリッド　列設定
    Private Enum EM_MRATE_COL_ID
        iNO = 0             'NO
        iKizuOut            '検出区分
        iTypeID             '疵種
        iGradeID            '疵グレード
        iHaba               '疵巾方向位置
        iLen                '疵長さ
        iTotalLen           '疵総長さ方向位置
        iKizuDiv            '疵区分
        iKizuNo             '疵No
        iOpeDsp             'オペガエ表示フラグ
        iSysKbn             'システム区分
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
    Private Sub frmFF_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        Me.tProp_Moveable = True

        'タイトル設定
        Call SetTitleName(Me)

        'ポップアップ画面 低解像度対応
        Call SetPcHeight(Me)

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
            strSQL &= " FROM " & DB_BUF_FF_S_ZISEKI
            strSQL &= " WHERE 管理No = '" & m_strKizukenNo & "'"
            strSQL &= " ORDER BY 登録日付 DESC"
        Else                         'ツールモード
            'strSQL &= " SELECT TOP 200 ID"
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

        '疵検査管理No
        lblKizuKen.Text = ""

        '命令No
        lblMeiNo.Text = ""

        '生産NO
        lblProductNo.Text = ""

        'コイル長
        lblCoilLen1.Text = ""
        lblCoilLen2.Text = ""

        '疵検処理モード
        lblMode1.Text = ""
        lblMode2.Text = ""

        ' 0.1m化 対応 --->>>
        ''カット信号回数
        'lblCutCnt1.Text = ""
        'lblCutCnt2.Text = ""
        ' 0.1m化 対応 <<<---

        '疵個数オーバーステータス
        lblOverCnt1.Text = ""
        lblOverCnt2.Text = ""

        'データ設定数
        lblSetCnt1.Text = ""
        lblSetCnt2.Text = ""

        '疵混入率表 初期化
        dgvInitialize(dgvFF1)
        dgvInitialize(dgvFF2)

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
    ''' <param name="typZiseki1">FF実績データ</param>
    ''' <param name="typZiseki2">FF実績データ</param>
    ''' <param name="strKizuken">疵検管理No</param>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strID As String, ByRef typZiseki1 As FF_JISSEKI,
                             ByRef typZiseki2 As FF_JISSEKI, ByRef strKizuken As String) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文
        Dim bytData1() As Byte  '生伝文バイナリデータ
        Dim bytData2() As Byte  '生伝文バイナリデータ


        '疵検管理Noあり
        strSQL = ""
        strSQL &= " SELECT 生伝文1,"
        strSQL &= "        生伝文2,"
        strSQL &= "        管理No"
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

            '疵検査管理No
            strKizuken = sqlRead.GetValue(2).ToString

            tMod.MemSet(bytData1, 0, typZiseki1, tMod.SizeOf(typZiseki1))
            tMod.MemSet(bytData2, 0, typZiseki2, tMod.SizeOf(typZiseki2))


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
    ''' グリッドの初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
        'Dim ii As Integer = 0
        With dgv
            .RowCount = MAX_FF_DFCT_CNT

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
            .Columns(EM_MRATE_COL_ID.iNO).HeaderText = "NO"
            .Columns(EM_MRATE_COL_ID.iNO).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iNO).Width = 40
            .Columns(EM_MRATE_COL_ID.iNO).SortMode = DataGridViewColumnSortMode.NotSortable



            '列１ (アドレス) 疵検出区分
            .Columns(EM_MRATE_COL_ID.iKizuOut).HeaderText = "疵検出区分"
            .Columns(EM_MRATE_COL_ID.iKizuOut).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iKizuOut).Width = 60
            .Columns(EM_MRATE_COL_ID.iKizuOut).SortMode = DataGridViewColumnSortMode.NotSortable


            '列２ (アドレス) 疵種
            .Columns(EM_MRATE_COL_ID.iTypeID).HeaderText = "疵種"
            .Columns(EM_MRATE_COL_ID.iTypeID).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iTypeID).Width = 50
            .Columns(EM_MRATE_COL_ID.iTypeID).SortMode = DataGridViewColumnSortMode.NotSortable


            '列３ (アドレス) 疵グレード
            .Columns(EM_MRATE_COL_ID.iGradeID).HeaderText = "疵ｸﾞﾚｰﾄﾞ"
            .Columns(EM_MRATE_COL_ID.iGradeID).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGradeID).Width = 80
            .Columns(EM_MRATE_COL_ID.iGradeID).SortMode = DataGridViewColumnSortMode.NotSortable


            '列４ (アドレス) 疵巾方向位置
            .Columns(EM_MRATE_COL_ID.iHaba).HeaderText = "疵巾" & vbNewLine & "方向位置"
            .Columns(EM_MRATE_COL_ID.iHaba).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iHaba).Width = 80
            .Columns(EM_MRATE_COL_ID.iHaba).SortMode = DataGridViewColumnSortMode.NotSortable


            '列５ (アドレス) 疵長さ
            .Columns(EM_MRATE_COL_ID.iLen).HeaderText = "疵長さ"
            .Columns(EM_MRATE_COL_ID.iLen).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iLen).Width = 60
            .Columns(EM_MRATE_COL_ID.iLen).SortMode = DataGridViewColumnSortMode.NotSortable



            '列６ (アドレス) 疵総長さ方向位置
            .Columns(EM_MRATE_COL_ID.iTotalLen).HeaderText = "疵総長さ" & vbNewLine & "方向位置"
            .Columns(EM_MRATE_COL_ID.iTotalLen).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iTotalLen).Width = 80
            .Columns(EM_MRATE_COL_ID.iTotalLen).SortMode = DataGridViewColumnSortMode.NotSortable


            '列７ (アドレス) 疵区分
            .Columns(EM_MRATE_COL_ID.iKizuDiv).HeaderText = "疵区分"
            .Columns(EM_MRATE_COL_ID.iKizuDiv).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iKizuDiv).Width = 60
            .Columns(EM_MRATE_COL_ID.iKizuDiv).SortMode = DataGridViewColumnSortMode.NotSortable


            '列8 (アドレス) 疵No
            .Columns(EM_MRATE_COL_ID.iKizuNo).HeaderText = "疵NO"
            .Columns(EM_MRATE_COL_ID.iKizuNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iKizuNo).Width = 110
            .Columns(EM_MRATE_COL_ID.iKizuNo).SortMode = DataGridViewColumnSortMode.NotSortable

            '列9 (アドレス) オペガエ
            .Columns(EM_MRATE_COL_ID.iOpeDsp).HeaderText = "オペガエ"
            .Columns(EM_MRATE_COL_ID.iOpeDsp).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iOpeDsp).Width = 80
            .Columns(EM_MRATE_COL_ID.iOpeDsp).SortMode = DataGridViewColumnSortMode.NotSortable

            '列10 (アドレス) システム区分
            .Columns(EM_MRATE_COL_ID.iSysKbn).HeaderText = "ｼｽﾃﾑ区分"
            .Columns(EM_MRATE_COL_ID.iSysKbn).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iSysKbn).Width = 80
            .Columns(EM_MRATE_COL_ID.iSysKbn).SortMode = DataGridViewColumnSortMode.NotSortable
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

    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
        Dim typZiseki1 As FF_JISSEKI = Nothing  '疵検査実績データ
        Dim typZiseki2 As FF_JISSEKI = Nothing  '疵検査実績データ
        Dim strKizuken As String = ""       '疵検管理No

        'データ表示
        If GetData(cmbID.Text, typZiseki1, typZiseki2, strKizuken) Then
            Call SetData(typZiseki1, typZiseki2, strKizuken)
        End If
    End Sub
    ''' <summary>
    ''' データセット
    ''' </summary>
    '''<param name="typZiseki1">FF実績データ1</param>
    ''' <param name="typZiseki2">FF実績データ2</param>
    ''' <param name="strKizuken">疵検管理No</param>
    ''' <remarks></remarks>
    Private Sub SetData(ByVal typZiseki1 As FF_JISSEKI, ByVal typZiseki2 As FF_JISSEKI, ByVal strKizuken As String)

        Dim ii As Integer   'ループカウンタ
        ' 0.1m化 対応 --->>>
        Dim strBuff As String
        ' 0.1m化 対応 <<<---

        '疵検管理No
        lblKizuKen.Text = tMod.ByteToString(typZiseki1.Id)

        ''命令No
        lblMeiNo.Text = tMod.ByteToString(typZiseki1.Hd.Meino)

        ''生産NO
        lblProductNo.Text = tMod.ByteToString(typZiseki1.Hd.Coilno)

        'コイル長
        ' 0.1m化 対応 --->>>
        'lblCoilLen1.Text = tMod.ByteToString(typZiseki1.CoilLen)
        'lblCoilLen2.Text = tMod.ByteToString(typZiseki2.CoilLen)
        'lblCoilLen1.Text = Format(CSng(IIf(IsNumeric(tMod.ByteToString(typZiseki1.CoilLen)), tMod.ByteToString(typZiseki1.CoilLen).Trim, "0")) / 10, "0.0")
        lblCoilLen1.Text = Format(IIf(IsNumeric(tMod.ByteToString(typZiseki1.CoilLen)), CDbl(tMod.ByteToString(typZiseki1.CoilLen)) / 10.0, 0.0), "0.0")
        'lblCoilLen2.Text = Format(CSng(IIf(IsNumeric(tMod.ByteToString(typZiseki2.CoilLen)), tMod.ByteToString(typZiseki2.CoilLen).Trim, "0")) / 10, "0.0")
        lblCoilLen2.Text = Format(IIf(IsNumeric(tMod.ByteToString(typZiseki2.CoilLen)), CDbl(tMod.ByteToString(typZiseki2.CoilLen)) / 10.0, 0.0), "0.0")
        ' 0.1m化 対応 <<<---

        '疵検処理モード
        lblMode1.Text = tMod.ByteToString(typZiseki1.Mode)
        lblMode2.Text = tMod.ByteToString(typZiseki2.Mode)

        ' 0.1m化 対応 --->>>
        ''カット信号回数
        'lblCutCnt1.Text = tMod.ByteToString(typZiseki1.CutCount)
        'lblCutCnt2.Text = tMod.ByteToString(typZiseki2.CutCount)
        ' 0.1m化 対応 <<<---

        '疵個数オーバーステータス
        lblOverCnt1.Text = tMod.ByteToString(typZiseki1.OverStatus)
        lblOverCnt2.Text = tMod.ByteToString(typZiseki2.OverStatus)

        'データ設定数
        lblSetCnt1.Text = tMod.ByteToString(typZiseki1.DataDefault)
        lblSetCnt2.Text = tMod.ByteToString(typZiseki2.DataDefault)

        ''表
        For ii = 0 To MAX_FF_DFCT_CNT - 1
            dgvFF1(EM_MRATE_COL_ID.iNO, ii).Value = ii + 1
            dgvFF1(EM_MRATE_COL_ID.iKizuOut, ii).Value = Chr(typZiseki1.Dt(ii).Kizukbn(0))
            dgvFF1(EM_MRATE_COL_ID.iTypeID, ii).Value = tMod.ByteToString(typZiseki1.Dt(ii).Kind)
            dgvFF1(EM_MRATE_COL_ID.iGradeID, ii).Value = tMod.ByteToString(typZiseki1.Dt(ii).Grade)
            dgvFF1(EM_MRATE_COL_ID.iHaba, ii).Value = tMod.ByteToString(typZiseki1.Dt(ii).WidPos)
            ' 0.1m化 対応 --->>>
            '疵長さ
            ''dgvFF1(EM_MRATE_COL_ID.iLen, ii).Value = tMod.ByteToString(typZiseki1.Dt(ii).Len)
            strBuff = tMod.ByteToString(typZiseki1.Dt(ii).Len)
            If strBuff.Trim().Length <> 0 Then
                dgvFF1(EM_MRATE_COL_ID.iLen, ii).Value = Format(CSng(IIf(IsNumeric(strBuff), strBuff, "0")) / 10, "0.0")
            Else
                dgvFF1(EM_MRATE_COL_ID.iLen, ii).Value = ""
            End If

            '疵総長さ方向位置
            ''dgvFF1(EM_MRATE_COL_ID.iTotalLen, ii).Value = tMod.ByteToString(typZiseki1.Dt(ii).DefectPos)
            strBuff = tMod.ByteToString(typZiseki1.Dt(ii).DefectPos)
            If strBuff.Trim().Length <> 0 Then
                dgvFF1(EM_MRATE_COL_ID.iTotalLen, ii).Value = Format(CSng(IIf(IsNumeric(strBuff), strBuff, "0")) / 10, "0.0")
            Else
                dgvFF1(EM_MRATE_COL_ID.iTotalLen, ii).Value = ""
            End If
            ' 0.1m化 対応 <<<---

            dgvFF1(EM_MRATE_COL_ID.iKizuDiv, ii).Value = Chr(typZiseki1.Dt(ii).kbn(0))

            Dim sNoHex As String = tMod.ByteToString(typZiseki1.Dt(ii).No)
            Dim nNo As Integer
            Dim sNo As String = sNoHex

            '// 20200520 疵保存個数上限拡張 --->>>
            ''疵No 16進数(10進数)
            'If Integer.TryParse(sNoHex, Globalization.NumberStyles.HexNumber, Nothing, nNo) = True Then
            '    sNo &= String.Format(" ({0,5})", nNo)
            'End If

            ''疵No 32進数(10進数)
            If Base32Hex_Decord(typZiseki1.Dt(ii).No, nNo) = True Then
                sNo &= String.Format(" ({0,7})", nNo)
            End If
            '// 20200520 疵保存個数上限拡張 ---<<<
            dgvFF1(EM_MRATE_COL_ID.iKizuNo, ii).Value = sNo

            dgvFF1(EM_MRATE_COL_ID.iOpeDsp, ii).Value = Chr(typZiseki1.Dt(ii).OpeDsp(0))
            dgvFF1(EM_MRATE_COL_ID.iSysKbn, ii).Value = Chr(typZiseki1.Dt(ii).SysKbn(0))
        Next ii


        ''裏
        For ii = 0 To MAX_FF_DFCT_CNT - 1
            dgvFF2(EM_MRATE_COL_ID.iNO, ii).Value = ii + 1
            dgvFF2(EM_MRATE_COL_ID.iKizuOut, ii).Value = Chr(typZiseki2.Dt(ii).Kizukbn(0))
            dgvFF2(EM_MRATE_COL_ID.iTypeID, ii).Value = tMod.ByteToString(typZiseki2.Dt(ii).Kind)
            dgvFF2(EM_MRATE_COL_ID.iGradeID, ii).Value = tMod.ByteToString(typZiseki2.Dt(ii).Grade)
            dgvFF2(EM_MRATE_COL_ID.iHaba, ii).Value = tMod.ByteToString(typZiseki2.Dt(ii).WidPos)

            ' 0.1m化 対応 --->>>
            '疵長さ
            ''dgvFF2(EM_MRATE_COL_ID.iLen, ii).Value = tMod.ByteToString(typZiseki2.Dt(ii).Len)
            strBuff = tMod.ByteToString(typZiseki2.Dt(ii).Len)
            If strBuff.Trim().Length <> 0 Then
                dgvFF2(EM_MRATE_COL_ID.iLen, ii).Value = Format(CSng(IIf(IsNumeric(strBuff), strBuff, "0")) / 10, "0.0")
            Else
                dgvFF2(EM_MRATE_COL_ID.iLen, ii).Value = ""
            End If

            '疵総長さ方向位置
            ''dgvFF2(EM_MRATE_COL_ID.iTotalLen, ii).Value = tMod.ByteToString(typZiseki2.Dt(ii).DefectPos)
            strBuff = tMod.ByteToString(typZiseki2.Dt(ii).DefectPos)
            If strBuff.Trim().Length <> 0 Then
                dgvFF2(EM_MRATE_COL_ID.iTotalLen, ii).Value = Format(CSng(IIf(IsNumeric(strBuff), strBuff, "0")) / 10, "0.0")
            Else
                dgvFF2(EM_MRATE_COL_ID.iTotalLen, ii).Value = ""
            End If

            ' 0.1m化 対応 <<<---

            dgvFF2(EM_MRATE_COL_ID.iKizuDiv, ii).Value = Chr(typZiseki2.Dt(ii).kbn(0))

            Dim sNoHex As String = tMod.ByteToString(typZiseki2.Dt(ii).No)
            Dim nNo As Integer
            Dim sNo As String = sNoHex

            '// 20200520 疵保存個数上限拡張 --->>>
            '疵No 16進数(10進数)
            'If Integer.TryParse(sNoHex, Globalization.NumberStyles.HexNumber, Nothing, nNo) = True Then
            '    sNo &= String.Format(" ({0,5})", nNo)
            'End If

            ''疵No 32進数(10進数)
            If Base32Hex_Decord(typZiseki2.Dt(ii).No, nNo) = True Then
                sNo &= String.Format(" ({0,7})", nNo)
            End If
            '// 20200520 疵保存個数上限拡張 ---<<<
            dgvFF2(EM_MRATE_COL_ID.iKizuNo, ii).Value = sNo

            dgvFF2(EM_MRATE_COL_ID.iOpeDsp, ii).Value = Chr(typZiseki2.Dt(ii).OpeDsp(0))
            dgvFF2(EM_MRATE_COL_ID.iSysKbn, ii).Value = Chr(typZiseki2.Dt(ii).SysKbn(0))
        Next ii

    End Sub

    '// 2015.03.02 T.Mizoguchi オペレータガイダンス強化(検査状態伝文追加) ----->>>
    ''' <summary>
    ''' 検査状態伝文ボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnStatus_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnStatus.Click
        If "" = cmbID.Text Then Exit Sub

        Dim frm As New frmKensaStatus(1, cmbID.Text)

        frm.Show()
        'frm.Dispose()
        'frm = Nothing
    End Sub
    '// 2015.03.02 T.Mizoguchi オペレータガイダンス強化(検査状態伝文追加) <<<-----

    '// 20200520 疵保存個数上限拡張 --->>>
    ''' <summary>
    ''' Base32HEXデコード
    ''' </summary>
    ''' <param name="val"></param>
    ''' <param name="nResult"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function Base32Hex_Decord(ByVal val As Byte(), ByRef nResult As Integer) As Boolean

        Dim nDec As Integer = 0
        Dim nTotal As Integer = 0

        For ii As Integer = 0 To val.Length - 1
            Dim bt As Byte = val(val.Length - (1 + ii))
            If &H30 <= bt And &H39 >= bt Then
                nDec = CInt(bt - &H30)
            ElseIf &H41 <= bt And &H56 >= bt Then
                nDec = CInt((bt - &H41) + 10)
            ElseIf &H61 <= bt And &H76 >= bt Then
                nDec = CInt((bt - &H61) + 10)
            Else
                Return False
            End If

            nTotal = nTotal + nDec * CInt(Math.Pow(32, ii))
        Next

        nResult = nTotal
        Return True

    End Function
    '// 20200520 疵保存個数上限拡張 ---<<<


End Class
