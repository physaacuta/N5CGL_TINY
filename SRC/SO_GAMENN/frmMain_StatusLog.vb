'*********************************************************************************
' ガイダンスログ画面の親クラス
'	[Ver]
'		Ver.01    2013/09/19  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

''' <summary>
''' ガイダンスログ画面の親クラス
''' </summary>
''' <remarks></remarks>
Public Class frmMain_StatusLog
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 定義
	'/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' グリッド（ガイダンスログ）設定項目
    ''' </summary>
    ''' <remarks></remarks>
	Private Enum em_LogListColm
        ''' <summary>変更日</summary>
        idayHenTime = 0
        ''' <summary>管理No</summary>
        iKizuNo
        ''' <summary>コイルNo</summary>
        iCoilNo
        ''' <summary>稼動状態</summary>
        iKado
        ''' <summary>表_機器状態</summary>
        iKiki_O
        ''' <summary>表_検査状態</summary>
        iKensa_O
        ''' <summary>裏_機器状態</summary>
        iKiki_U
        ''' <summary>裏_検査状態</summary>
        iKensa_U
        ''' <summary>ガイダンスメッセージ</summary>
        iMsg


        iend
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバ 変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_lstChkKadKind As New List(Of CheckBox)        ' 区分(稼動状態)チェックボックスコントロール リスト
    Private m_lstChkKikiKind_O As New List(Of CheckBox)       ' 区分(機器状態表)チェックボックスコントロール リスト
    Private m_lstChkKenKind_O As New List(Of CheckBox)        ' 区分(検査状態表)チェックボックスコントロール リスト
    Private m_lstChkKikiKind_U As New List(Of CheckBox)       ' 区分(機器状態裏)チェックボックスコントロール リスト
    Private m_lstChkKenKind_U As New List(Of CheckBox)        ' 区分(検査状態裏)チェックボックスコントロール リスト

    ' DB関連
    Private m_sDB_STATUS_LOG As String = DB_STATUS_LOG                        ' DBテーブル 機器状態ログテーブル
    Private m_sDbSession As String = INI_DB                                 ' DBセッション

    ' ログ関連
    Private m_bLog_ExecCancel As Boolean = False                            ' キャンセル時 True



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 外部プロパティ
    '/ /////////////////////////////////////////////////////////////////////////////////



    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ 外部アクセス メソッド
    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ログ表示最大行数
    ''' </summary>
    ''' <param name="nMaxRow">ログ表示最大行数</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub SetLogMaxRow(ByVal nMaxRow As Integer)
        'm_nLogMaxRow = nMaxRow
        nudRowMaxCount.Value = nMaxRow
    End Sub

    ''' <summary>
    ''' 区分（稼動状態）チェックボックスコントロール を登録
    ''' </summary>
    ''' <param name="chk">区分（稼動状態）チェックボックスコントロール</param>
    ''' <param name="pcName">DB検索するPCKind</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub AddCheckBoxKadKind(ByVal chk As CheckBox, ByVal pcName As String)
        chk.Tag = pcName        ' Tagに文字列を追加
        m_lstChkKadKind.Add(chk)     ' リストに登録
        '' ついでにクリックイベントハンドラに追加
        AddHandler chk.CheckedChanged, AddressOf chk_CheckedChanged
    End Sub

    ''' <summary>
    ''' 区分（機器状態表）チェックボックスコントロール を登録
    ''' </summary>
    ''' <param name="chk">区分（稼動状態）チェックボックスコントロール</param>
    ''' <param name="pcName">DB検索するPCKind</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub AddCheckBoxKikiKind_O(ByVal chk As CheckBox, ByVal pcName As String)
        chk.Tag = pcName        ' Tagに文字列を追加
        m_lstChkKikiKind_O.Add(chk)     ' リストに登録
        '' ついでにクリックイベントハンドラに追加
        AddHandler chk.CheckedChanged, AddressOf chk_CheckedChanged
    End Sub

    ''' <summary>
    ''' 区分（機器状態裏）チェックボックスコントロール を登録
    ''' </summary>
    ''' <param name="chk">区分（稼動状態）チェックボックスコントロール</param>
    ''' <param name="pcName">DB検索するPCKind</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub AddCheckBoxKikiKind_U(ByVal chk As CheckBox, ByVal pcName As String)
        chk.Tag = pcName        ' Tagに文字列を追加
        m_lstChkKikiKind_U.Add(chk)     ' リストに登録
        '' ついでにクリックイベントハンドラに追加
        AddHandler chk.CheckedChanged, AddressOf chk_CheckedChanged
    End Sub


    ''' <summary>
    ''' 区分（検査状態表）チェックボックスコントロール を登録
    ''' </summary>
    ''' <param name="chk">区分（検査状態）チェックボックスコントロール</param>
    ''' <param name="pcName">DB検索するPCKind</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub AddCheckBoxKenKind_O(ByVal chk As CheckBox, ByVal pcName As String)
        chk.Tag = pcName        ' Tagに文字列を追加
        m_lstChkKenKind_O.Add(chk)     ' リストに登録
        '' ついでにクリックイベントハンドラに追加
        AddHandler chk.CheckedChanged, AddressOf chk_CheckedChanged
    End Sub

    ''' <summary>
    ''' 区分（検査状態裏）チェックボックスコントロール を登録
    ''' </summary>
    ''' <param name="chk">区分（検査状態）チェックボックスコントロール</param>
    ''' <param name="pcName">DB検索するPCKind</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub AddCheckBoxKenKind_U(ByVal chk As CheckBox, ByVal pcName As String)
        chk.Tag = pcName        ' Tagに文字列を追加
        m_lstChkKenKind_U.Add(chk)     ' リストに登録
        '' ついでにクリックイベントハンドラに追加
        AddHandler chk.CheckedChanged, AddressOf chk_CheckedChanged
    End Sub


    ''' <summary>
    ''' チェックボックス 背景色変更
    ''' </summary>
    ''' <param name="chk">チェックボックス</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub SetChkBackColor_Change(ByVal chk As CheckBox)
        If chk.Checked Then                 ' ON
            chk.BackColor = Color.Cyan
        Else                                ' OFF
            chk.BackColor = Drawing.SystemColors.Control
        End If
    End Sub
    ''' <summary>
    ''' チェックボックス 背景色変更
    ''' </summary>
    ''' <param name="chk">チェックボックス</param>
    ''' <param name="checked"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub SetChkBackColor_Change(ByVal chk As CheckBox, ByVal checked As Boolean)
        If checked Then                 ' ON
            chk.Checked = checked
            chk.BackColor = Color.Cyan
        Else                                ' OFF
            chk.Checked = checked
            chk.BackColor = Drawing.SystemColors.Control
        End If
    End Sub

    ''' <summary>
    ''' リスト配列に登録されているコントロールのチェック済み個数を算出
    ''' </summary>
    ''' <param name="lstChk"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Protected Friend Function GetLstCheckCount(ByVal lstChk As List(Of CheckBox)) As Integer
        Dim iCnt As Integer = 0
        For ii As Integer = 0 To lstChk.Count - 1
            If lstChk.Item(ii).Checked Then iCnt += 1
        Next ii
        Return iCnt
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ イベント
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' 生成
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmBase_Log_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        '' グリッド定義
        dgvInitialize(dgvLog)


        '' 時刻セット
        dtpStartDay.Value = CDate(Now.ToString("yyyy/MM/dd 00:00:00"))
        dtpStartTime.Value = CDate(Now.ToString("yyyy/MM/dd 00:00:00"))
        dtpEndDay.Value = CDate(Now.ToString("yyyy/MM/dd 23:59:59"))
        dtpEndTime.Value = CDate(Now.ToString("yyyy/MM/dd 23:59:59"))

    End Sub
    ''' <summary>
    ''' 開放
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmBase_Log_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        m_lstChkKadKind.Clear()       ' 区分(稼動状態)チェックボックスコントロール リスト
        m_lstChkKikiKind_O.Clear()    ' 区分(機器状態表)チェックボックスコントロール リスト
        m_lstChkKenKind_O.Clear()     ' 区分(検査状態表)チェックボックスコントロール リスト
        m_lstChkKikiKind_U.Clear()    ' 区分(機器状態裏)チェックボックスコントロール リスト
        m_lstChkKenKind_U.Clear()     ' 区分(検査状態裏)チェックボックスコントロール リスト
    End Sub
    ''' <summary>
    ''' チェックボックス チェック状態変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chk_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)
        ' チェックボックス 背景変更
        SetChkBackColor_Change(CType(sender, CheckBox))
    End Sub
    ''' <summary>
    ''' 時刻表示
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub timClock_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timClock.Tick
        lblClock.Text = Now.ToString("yyyy/MM/dd HH:mm:ss")
    End Sub
    ''' <summary>
    ''' 時刻範囲設定チェックボックス
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkStartEndDay_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkStartDay.CheckedChanged, chkEndDay.CheckedChanged
        If chkStartDay Is sender Then
            dtpStartDay.Enabled = chkStartDay.Checked
            dtpStartTime.Enabled = chkStartDay.Checked
        Else
            dtpEndDay.Enabled = chkEndDay.Checked
            dtpEndTime.Enabled = chkEndDay.Checked
        End If
    End Sub
    ''' <summary>
    ''' 日付コントロールに対して ホイールで加算減算が可能とする
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpStartDay.MouseWheel, dtpStartTime.MouseWheel, dtpEndDay.MouseWheel, dtpEndTime.MouseWheel
        ' 現在のdatetimepicker に↑↓キーを押された事とする
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 表示設定
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
        With dgv
            '' 動作定義
            .AutoGenerateColumns = False                                    ' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
            .AllowUserToAddRows = False                                     ' ユーザー 行追加
            .AllowUserToDeleteRows = False                                  ' ユーザー 行削除
            .AllowUserToOrderColumns = False                                ' ユーザー 列移動
            .AllowUserToResizeColumns = True                                ' ユーザー 列サイズ変更
            .AllowUserToResizeRows = False                                  ' ユーザー 行サイズ変更
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False

            .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' セル選択
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
            .DefaultCellStyle().SelectionBackColor = Color.DarkBlue         ' 選択
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue 'Color.LightSteelBlue         ' 1行おきに色替え


            '' 列定義
            .ColumnCount = CInt(em_LogListColm.iend)


            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .RowCount = 0
            .ColumnHeadersHeight = 25                                       ' 列ヘッダサイズ

            '列0 (時刻)
            .Columns(em_LogListColm.idayHenTime).HeaderText = "変更日"
            .Columns(em_LogListColm.idayHenTime).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.idayHenTime).DefaultCellStyle.Format = "yyyy/MM/dd HH:mm:ss"
            .Columns(em_LogListColm.idayHenTime).Width = 160
            .Columns(em_LogListColm.idayHenTime).SortMode = DataGridViewColumnSortMode.NotSortable 'DataGridViewColumnSortMode.Automatic
            '列1 (管理No)
            .Columns(em_LogListColm.iKizuNo).HeaderText = "管理No"
            .Columns(em_LogListColm.iKizuNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.iKizuNo).Width = 160
            .Columns(em_LogListColm.iKizuNo).SortMode = DataGridViewColumnSortMode.NotSortable
            '列2 (コイルNo)
            .Columns(em_LogListColm.iCoilNo).HeaderText = "コイルNo"
            .Columns(em_LogListColm.iCoilNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.iCoilNo).Width = 110
            .Columns(em_LogListColm.iCoilNo).SortMode = DataGridViewColumnSortMode.NotSortable
            '列3 (稼動状態)
            .Columns(em_LogListColm.iKado).HeaderText = "稼動状態"
            .Columns(em_LogListColm.iKado).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(em_LogListColm.iKado).Width = 100
            .Columns(em_LogListColm.iKado).SortMode = DataGridViewColumnSortMode.NotSortable
            '列4 (表_機器状態)
            .Columns(em_LogListColm.iKiki_O).HeaderText = "表_機器状態"
            .Columns(em_LogListColm.iKiki_O).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(em_LogListColm.iKiki_O).Width = 100
            .Columns(em_LogListColm.iKiki_O).SortMode = DataGridViewColumnSortMode.NotSortable
            '列5 (表_検査状態)
            .Columns(em_LogListColm.iKensa_O).HeaderText = "表_検査状態"
            .Columns(em_LogListColm.iKensa_O).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(em_LogListColm.iKensa_O).Width = 100
            .Columns(em_LogListColm.iKensa_O).SortMode = DataGridViewColumnSortMode.NotSortable

            '列6 (裏_機器状態)
            .Columns(em_LogListColm.iKiki_U).HeaderText = "裏_機器状態"
            .Columns(em_LogListColm.iKiki_U).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(em_LogListColm.iKiki_U).Width = 100
            .Columns(em_LogListColm.iKiki_U).SortMode = DataGridViewColumnSortMode.NotSortable
            '列7 (裏_検査状態)
            .Columns(em_LogListColm.iKensa_U).HeaderText = "裏_検査状態"
            .Columns(em_LogListColm.iKensa_U).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(em_LogListColm.iKensa_U).Width = 100
            .Columns(em_LogListColm.iKensa_U).SortMode = DataGridViewColumnSortMode.NotSortable
            '列8 (内容)
            .Columns(em_LogListColm.iMsg).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.iMsg).HeaderText = "ガイダンスメッセージ"
            .Columns(em_LogListColm.iMsg).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.iMsg).Width = 545
            .Columns(em_LogListColm.iMsg).SortMode = DataGridViewColumnSortMode.NotSortable


        End With
    End Sub


    ''' <summary>
    ''' 選択行無効化対策 (選択行色替えを無効化する場合は、必須。無しにするとデータセット時の1行目が変な事になる)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvLog_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvLog.SelectionChanged
        ' 背景色が透明の場合のみキャンセル
        If dgvLog.DefaultCellStyle().SelectionBackColor = Color.Transparent Then CType(sender, DataGridView).ClearSelection()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ SQL文生成用
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' SQL文生成 (SELECT句,FROM句,WHERE句,END句 の順番にコール)
    ''' </summary>
    ''' <param name="sql">生成SQL文</param>
    ''' <param name="bMode">true:通常 false:Count(*)</param>
    ''' <param name="nSelectCnt">0以外の場合 top 10 とかにする</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub subSQL_Create(ByRef sql As String, ByVal bMode As Boolean, Optional ByVal nSelectCnt As Integer = 0)
        '' SELECT句
        subSQL_Create_Select(sql, bMode, nSelectCnt)
        '' FROM句
        subSQL_Create_From(sql)
        '' WHERE句
        subSQL_Create_Where(sql)
        '' END句
        subSQL_Create_End(sql, bMode)

    End Sub
    ''' <summary>
    ''' SQL文生成 (SELECT句)
    ''' </summary>
    ''' <param name="sql">生成SQL文</param>
    ''' <param name="bMode">true:通常 false:Count(*)</param>
    ''' <param name="nSelectCnt">0以外の場合 top 10 とかにする</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub subSQL_Create_Select(ByRef sql As String, ByVal bMode As Boolean, Optional ByVal nSelectCnt As Integer = 0)
        If bMode Then
            sql = "SELECT "
            If 0 <> nSelectCnt Then sql &= "TOP " & nSelectCnt & " "
            sql &= "ISNULL(D.登録日付,''),"
            sql &= "ISNULL(D.稼働状態,'0'),"
            sql &= "ISNULL(D.検査状態_表,'0'),"
            sql &= "ISNULL(D.検査状態_裏,'0'),"
            sql &= "ISNULL(D.機器状態_表,'0'),"
            sql &= "ISNULL(D.機器状態_裏,'0'),"
            sql &= "ISNULL(D.ガイダンス,''),"
            sql &= "ISNULL(D.管理No,''),"
            sql &= "ISNULL(D.コイルNo,''),"
            sql &= "ISNULL(D.AutoNo,'0')"
        Else
            sql = "SELECT "
            sql &= "COUNT(*) "
        End If
    End Sub
    ''' <summary>
    ''' SQL文生成 (FROM句)
    ''' </summary>
    ''' <param name="sql">生成SQL文</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub subSQL_Create_From(ByRef sql As String)
        sql &= "FROM " & m_sDB_STATUS_LOG & " D "
    End Sub
    ''' <summary>
    ''' SQL文生成 (WHERE句)
    ''' </summary>
    ''' <param name="sql">生成SQL文</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub subSQL_Create_Where(ByRef sql As String)
        Dim strWhere As String = ""             ' Where句ワーク
        Dim ii As Integer
        Dim iWk As Integer
        Dim bWk As Boolean

        Dim iWk1 As Integer
        Dim iWk2 As Integer
        Dim iWk3 As Integer
        Dim iWk4 As Integer
        Dim iWk5 As Integer

        Dim bKubun As Boolean = False

        '''' 時刻フィルタ
        '' 開始
        If chkStartDay.Checked Then
            strWhere &= "AND D.登録日付 >= '" & dtpStartDay.Value.ToString("yyyy/MM/dd ") & dtpStartTime.Value.ToString("HH:mm:ss") & "' "
        End If
        '' 終了
        If chkEndDay.Checked Then
            strWhere &= "AND D.登録日付 <= '" & dtpEndDay.Value.ToString("yyyy/MM/dd ") & dtpEndTime.Value.ToString("HH:mm:ss") & "' "
        End If


        iWk1 = GetLstCheckCount(m_lstChkKadKind)                   ' 区分（稼動状態）フィルタ
        iWk2 = GetLstCheckCount(m_lstChkKikiKind_O)                ' 区分（機器状態表）フィルタ
        iWk3 = GetLstCheckCount(m_lstChkKikiKind_U)                ' 区分（機器状態裏）フィルタ
        iWk4 = GetLstCheckCount(m_lstChkKenKind_O)                 ' 区分（検査状態表）フィルタ
        iWk5 = GetLstCheckCount(m_lstChkKenKind_U)                 ' 区分（検査状態裏）フィルタ
        iWk = iWk1 + iWk2 + iWk3 + iWk4 + iWk5

        If iWk = 0 Then                                     ' 0件選択
            ' 何も無し
        Else

            strWhere &= "AND ("

            '''' 区分（稼動状態）フィルタ
            If iWk1 = 0 Then      ' 0件選択
                ' 何も無し
            Else

                bWk = False                                         ' 区分フィルタが1件でも有り時 true
                For ii = 0 To m_lstChkKadKind.Count - 1
                    If m_lstChkKadKind.Item(ii).Checked Then                 ' PCフィルタ チェック
                        If Not bWk Then
                            If Not bKubun Then      '最初に見つけたデータ
                                strWhere &= "("
                                bKubun = True
                            Else
                                strWhere &= "("
                            End If
                            bWk = True
                        Else
                            strWhere &= " OR "
                        End If

                        ' 稼動状態セット
                        strWhere &= " D.稼働状態 = '" & CStr(m_lstChkKadKind.Item(ii).Tag) & "'"
                    End If
                Next ii
                If bWk Then
                    strWhere &= ") "
                End If
            End If


            '''' 区分（機器状態表）フィルタ
            If iWk2 = 0 Then      ' 0件選択
                ' 何も無し
            Else
                bWk = False                                         ' 区分フィルタが1件でも有り時 true
                For ii = 0 To m_lstChkKikiKind_O.Count - 1
                    If m_lstChkKikiKind_O.Item(ii).Checked Then                 ' PCフィルタ チェック
                        If Not bWk Then
                            If Not bKubun Then      '最初に見つけたデータ
                                strWhere &= "("
                                bKubun = True
                            Else
                                strWhere &= " OR ("
                            End If
                            bWk = True
                        Else
                            strWhere &= " OR "
                        End If

                        ' 稼動状態セット
                        strWhere &= " D.機器状態_表 = '" & CStr(m_lstChkKikiKind_O.Item(ii).Tag) & "'"
                    End If
                Next ii
                If bWk Then
                    strWhere &= ") "
                End If
            End If

            '''' 区分（機器状態裏）フィルタ
            If iWk3 = 0 Then      ' 0件選択
                ' 何も無し
            Else
                bWk = False                                         ' 区分フィルタが1件でも有り時 true
                For ii = 0 To m_lstChkKikiKind_U.Count - 1
                    If m_lstChkKikiKind_U.Item(ii).Checked Then                 ' PCフィルタ チェック
                        If Not bWk Then
                            If Not bKubun Then      '最初に見つけたデータ
                                strWhere &= "("
                                bKubun = True
                            Else
                                strWhere &= " OR ("
                            End If
                            bWk = True
                        Else
                            strWhere &= " OR "
                        End If

                        ' 稼動状態セット
                        strWhere &= " D.機器状態_裏 = '" & CStr(m_lstChkKikiKind_U.Item(ii).Tag) & "'"
                    End If
                Next ii
                If bWk Then
                    strWhere &= ") "
                End If
            End If

            '''' 区分（検査状態表）フィルタ
            If iWk4 = 0 Then      ' 0件選択
                ' 何も無し
            Else
                bWk = False                                         ' 区分フィルタが1件でも有り時 true
                For ii = 0 To m_lstChkKenKind_O.Count - 1
                    If m_lstChkKenKind_O.Item(ii).Checked Then                 ' PCフィルタ チェック
                        If Not bWk Then
                            If Not bKubun Then      '最初に見つけたデータ
                                strWhere &= "("
                                bKubun = True
                            Else
                                strWhere &= " OR ("
                            End If
                            bWk = True
                        Else
                            strWhere &= " OR "
                        End If

                        ' 稼動状態セット
                        strWhere &= " D.検査状態_表 = '" & CStr(m_lstChkKenKind_O.Item(ii).Tag) & "'"
                    End If
                Next ii
                If bWk Then
                    strWhere &= ") "
                End If
            End If


            '''' 区分（検査状態裏）フィルタ
            If iWk5 = 0 Then      ' 0件選択 又は 全件選択
                ' 何も無し
            Else
                bWk = False                                         ' 区分フィルタが1件でも有り時 true
                For ii = 0 To m_lstChkKenKind_U.Count - 1
                    If m_lstChkKenKind_U.Item(ii).Checked Then                 ' PCフィルタ チェック
                        If Not bWk Then
                            If Not bKubun Then      '最初に見つけたデータ
                                strWhere &= "("
                                bKubun = True
                            Else
                                strWhere &= " OR ("
                            End If
                            bWk = True
                        Else
                            strWhere &= " OR "
                        End If

                        ' 稼動状態セット
                        strWhere &= " D.検査状態_裏 = '" & CStr(m_lstChkKenKind_U.Item(ii).Tag) & "'"
                    End If
                Next ii
                If bWk Then
                    strWhere &= ") "
                End If
            End If

            strWhere &= ") "

        End If



        '''' WHERE句生成
        If 0 <> strWhere.Length Then
            sql &= "WHERE 1=1 " & strWhere
        End If
    End Sub
    ''' <summary>
    ''' SQL文生成 (END句)
    ''' </summary>
    ''' <param name="sql">生成SQL文</param>
    ''' <param name="bMode">true:通常 false:Count(*)</param>	
    ''' <remarks></remarks>
    Protected Friend Overridable Sub subSQL_Create_End(ByRef sql As String, ByVal bMode As Boolean)
        If bMode Then
            sql &= "ORDER BY D.登録日付 DESC, D.AutoNo  DESC"
        End If
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 検索実行
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 検索実行中断
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub ExecCancel_Log()
        m_bLog_ExecCancel = True
    End Sub
    ''' <summary>
    ''' ログ表示開始時
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub ExecStart()
        my_btnFunction(3).Enabled = True
        my_btnFunction(4).Enabled = False
        my_btnFunction(11).Enabled = False
        m_bLog_ExecCancel = False
    End Sub
    ''' <summary>
    ''' ログ表示終了時
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub ExecEnd()
        my_btnFunction(3).Enabled = False
        my_btnFunction(4).Enabled = True
        my_btnFunction(11).Enabled = True
        m_bLog_ExecCancel = False
    End Sub



    ''' <summary>
    ''' 検索実行
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub ExecSelect_Log()
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""
        Dim ii As Integer = 0
        Dim rowCnt As Integer = 0
        Dim rowMaxCnt As Integer = 0

        Try
            '''' 初期化
            Call ExecStart()
            dgvLog.RowCount = 0
            If Not tcls_DB.Open() Then Exit Sub
            Application.DoEvents()

            '''' リスト件数取得
            Call subSQL_Create(strSQL, False)
            If Not tcls_DB.GetSelectKey(strSQL, rowMaxCnt) Then Exit Sub
            ' パフォーマンスの為に、データグリッドビューに行をセット
            If nudRowMaxCount.Value < rowMaxCnt Then        ' 上限超え
                rowMaxCnt = CInt(nudRowMaxCount.Value)
                lblRowCount.BackColor = Color.Pink
            Else
                rowMaxCnt = rowMaxCnt
                lblRowCount.BackColor = SystemColors.Control
            End If
            dgvLog.RowCount = rowMaxCnt
            lblRowCount.Text = rowMaxCnt.ToString("#,##0")
            prbReadRow.Minimum = 0
            prbReadRow.Value = 0
            prbReadRow.Maximum = rowMaxCnt
            prbReadRow.Visible = True
            Application.DoEvents()

            '''' データを取得
            Call subSQL_Create(strSQL, True, rowMaxCnt)
            sqlRead = tcls_DB.SelectExecute(strSQL) ' Select文実行
            If sqlRead Is Nothing Then Exit Sub
            ' データ取り出し
            While sqlRead.Read()

                ' データ取り出し
                dgvLog(em_LogListColm.idayHenTime, rowCnt).Value = sqlRead.GetDateTime(0).ToString("yyyy/MM/dd HH:mm:ss")
                dgvLog(em_LogListColm.iKizuNo, rowCnt).Value = sqlRead.GetString(7)
                dgvLog(em_LogListColm.iCoilNo, rowCnt).Value = sqlRead.GetString(8)
                dgvLog(em_LogListColm.iKado, rowCnt).Value = sqlRead.GetInt32(1)
                dgvLog(em_LogListColm.iKensa_O, rowCnt).Value = sqlRead.GetInt32(2)
                dgvLog(em_LogListColm.iKiki_O, rowCnt).Value = sqlRead.GetInt32(4)
                dgvLog(em_LogListColm.iKensa_U, rowCnt).Value = sqlRead.GetInt32(3)
                dgvLog(em_LogListColm.iKiki_U, rowCnt).Value = sqlRead.GetInt32(5)
                dgvLog(em_LogListColm.iMsg, rowCnt).Value = sqlRead.GetString(6)

                ' プログレスバー
                prbReadRow.PerformStep()


                If 0 = (rowCnt Mod 100) Then
                    Application.DoEvents()
                End If

                '''' 次行準備
                rowCnt += 1
                If rowCnt >= rowMaxCnt Then Exit Sub
                If m_bLog_ExecCancel Then                   ' キャンセル
                    ' キャンセルだったら 表示している行数だけにする
                    dgvLog.RowCount = rowCnt
                    lblRowCount.Text = rowCnt.ToString
                    Exit Sub
                End If
            End While

            '''' DB切断
            tcls_DB.Close()
        Catch ex As Exception
        Finally
            ' DB開放
            If Not sqlRead Is Nothing Then sqlRead.Close()
            Call tcls_DB.Dispose()

            ' コントロール初期化
            prbReadRow.Visible = False
            Call ExecEnd()
        End Try
    End Sub

    ''' <summary>
    ''' 表示エリアのみの描画指定
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvLog_CellFormatting(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellFormattingEventArgs) Handles dgvLog.CellFormatting
        ' 稼動状態から文字に変更
        If e.ColumnIndex = em_LogListColm.iKado Then
            Dim strVal As String = ""
            strVal = GetKadouStatusName(CType(e.Value, EM_DIV_KADOU))
            e.Value = strVal
            If e.CellStyle.ForeColor = Drawing.SystemColors.ControlText Then        'デフォルト色の場合のみ設定
                Dim color As Color = Drawing.Color.Black
                ChangeColor_Kado(e)
            End If
        End If

        ' 機器状態（表）から文字に変更
        If e.ColumnIndex = em_LogListColm.iKiki_O Then
            Dim strVal As String = ""
            strVal = GetKiKiStatusName(CType(e.Value, EM_DIV_KIKI))
            e.Value = strVal
            If e.CellStyle.ForeColor = Drawing.SystemColors.ControlText Then        'デフォルト色の場合のみ設定
                Dim color As Color = Drawing.Color.Black

                ChangeColor_Kiki(e)
            End If
        End If

        ' 機器状態（裏）から文字に変更
        If e.ColumnIndex = em_LogListColm.iKiki_U Then
            Dim strVal As String = ""
            strVal = GetKiKiStatusName(CType(e.Value, EM_DIV_KIKI))
            e.Value = strVal
            If e.CellStyle.ForeColor = Drawing.SystemColors.ControlText Then        'デフォルト色の場合のみ設定
                Dim color As Color = Drawing.Color.Black
                ChangeColor_Kiki(e)
            End If
        End If

        ' 検査状態（表）から文字に変更
        If e.ColumnIndex = em_LogListColm.iKensa_O Then
            Dim strVal As String = ""
            strVal = GetKensaStatusName(CType(e.Value, EM_DIV_KENSA))
            e.Value = strVal
            If e.CellStyle.ForeColor = Drawing.SystemColors.ControlText Then        'デフォルト色の場合のみ設定
                Dim color As Color = Drawing.Color.Black

                ChangeColor_Kensa(e)
            End If
        End If

        ' 検査状態（表）から文字に変更
        If e.ColumnIndex = em_LogListColm.iKensa_U Then
            Dim strVal As String = ""
            strVal = GetKensaStatusName(CType(e.Value, EM_DIV_KENSA))
            e.Value = strVal
            If e.CellStyle.ForeColor = Drawing.SystemColors.ControlText Then        'デフォルト色の場合のみ設定
                Dim color As Color = Drawing.Color.Black
                ChangeColor_Kensa(e)
            End If
        End If

    End Sub



    ''' <summary>
    ''' 稼動状態名取得
    ''' </summary>
    ''' <param name="nVal">値</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Overridable Function GetKadouStatusName(ByVal nVal As EM_DIV_KADOU) As String
        'frmStatusLog側にて、各ライン別の名称取得

        Return ""

    End Function


    ''' <summary>
    ''' 検査状態名取得
    ''' </summary>
    ''' <param name="nVal">値</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Overridable Function GetKensaStatusName(ByVal nVal As EM_DIV_KENSA) As String
        'frmStatusLog側にて、各ライン別の名称取得

        Return ""

    End Function

    ''' <summary>
    ''' 機器状態名取得
    ''' </summary>
    ''' <param name="nVal">値</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Overridable Function GetKiKiStatusName(ByVal nVal As EM_DIV_KIKI) As String
        'frmStatusLog側にて、各ライン別の名称取得

        Return ""
    End Function

    ''' <summary>
    ''' 色変え(稼動状態)
    ''' </summary>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Overridable Sub ChangeColor_Kado(ByVal e As System.Windows.Forms.DataGridViewCellFormattingEventArgs)
        'frmStatusLog側にて、各ライン別の色指定

    End Sub

    ''' <summary>
    ''' 色変え(機器状態)
    ''' </summary>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Overridable Sub ChangeColor_Kiki(ByVal e As System.Windows.Forms.DataGridViewCellFormattingEventArgs)
        'frmStatusLog側にて、各ライン別の色指定

    End Sub

    ''' <summary>
    ''' 色変え(検査状態)
    ''' </summary>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Overridable Sub ChangeColor_Kensa(ByVal e As System.Windows.Forms.DataGridViewCellFormattingEventArgs)
        'frmStatusLog側にて、各ライン別の色指定

    End Sub

    ''' <summary>
    ''' CSV出力
    ''' </summary>
    ''' <param name="strFolder">保存ファイルのフォルダパス</param>
    ''' <param name="strName">保存ファイルのファイル名</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub CsvOut(ByVal strFolder As String, ByVal strName As String)
        Dim strFPath As String
        Dim strMsg As String = ""
        Dim strData As String = ""                      '1行のデータ

        '' フルパス
        strFPath = IO.Path.Combine(strFolder, strName)

        '' フォルダの有無チェック(無かったら作ろう)
        tMod.CheckFolder(strFolder)

        '' CSV出力
        Using cls_Stram As New tClass.tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
            Try
                ' オープン
                cls_Stram.Open()

                strData = ""
                'ヘッダー出力
                For ii As Integer = 0 To dgvLog.Columns.Count - 1
                    strData &= dgvLog.Columns(ii).HeaderText & ","
                Next ii

                '最後の","を排除
                strData = strData.Remove(strData.Length - 1, 1)

                cls_Stram.WriteLine(strData)


                'データ出力
                For ii As Integer = 0 To dgvLog.Rows.Count - 1
                    strData = ""
                    For jj As Integer = 0 To dgvLog.ColumnCount - 1
                        If jj = 3 Then      '稼動状態
                            strData &= GetKadouStatusName(CType(dgvLog(jj, ii).Value, EM_DIV_KADOU)) & ","
                        ElseIf jj = 4 Then  '機器状態（表）
                            strData &= GetKiKiStatusName(CType(dgvLog(jj, ii).Value, EM_DIV_KIKI)) & ","
                        ElseIf jj = 5 Then  '検査状態（表）
                            strData &= GetKensaStatusName(CType(dgvLog(jj, ii).Value, EM_DIV_KENSA)) & ","
                        ElseIf jj = 6 Then  '機器状態（裏）
                            strData &= GetKiKiStatusName(CType(dgvLog(jj, ii).Value, EM_DIV_KIKI)) & ","
                        ElseIf jj = 7 Then  '検査状態（裏）
                            strData &= GetKensaStatusName(CType(dgvLog(jj, ii).Value, EM_DIV_KENSA)) & ","
                        Else
                            strData &= dgvLog(jj, ii).Value.ToString & ","
                        End If
                    Next jj
                    '最後の","を排除
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
