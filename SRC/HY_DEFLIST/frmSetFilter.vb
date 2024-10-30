
'*********************************************************************************
'　フィルター設定画面クラス
'	[Ver]
'		Ver.01    2008/07/25  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmSetFilter

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    'DataGridViewタグNO
    Private Const MC_SHOUSAIDATA As Integer = 0    '詳細dgv データ表示エリア
    Private Const MC_SHOUSAIETC As Integer = 1     '詳細dgv 「上記以外」

    '詳細グリッド定義
    Private Enum em_dgvShousaiColum
        iText_No = 0          'NO
        iCombo_Kizushu        '疵種
        iCombo_Grade          '詳細出力
        iCombo_KaniUmu        '簡易出力
    End Enum

    ' 共通の色コード
    Public m_ColorSelection As Color = Color.LightGreen                 '選択セル一行分の色変え


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバークラス
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager                     'ログ管理クラス

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバ変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterAll As SqlClient.SqlDataAdapter = Nothing      'アダプター 共通出力設定テーブル
    Private m_AdapterCommon As SqlClient.SqlDataAdapter = Nothing   'アダプター 疵検定数マスターテーブル
    Private m_AdapterType As SqlClient.SqlDataAdapter = Nothing     'アダプター 疵種マスター
    Private m_AdapterGrade As SqlClient.SqlDataAdapter = Nothing    'アダプター グレードマスター

    Private m_DsAll As New DataSet                                  '非接続型テーブル 共通出力設定テーブル
    Private m_DsCommon As New DataSet                               '非接続型テーブル 疵検定数マスターテーブル
    Private m_DsShousaiType As New DataSet                          '非接続型テーブル 詳細dgv 疵種マスター コンボボックス用 データ先頭に「無し」を追加する
    Private m_DsShousaiGrade As New DataSet                         '非接続型テーブル 詳細dgv グレードマスター コンボボックス用 データ先頭に「－」を追加する
    Private m_DsType As New DataSet                                 '非接続型テーブル 簡易dgv 疵種マスター用
    Private m_DsGrade As New DataSet                                '非接続型テーブル 簡易dgv グレードマスター用

    ''' <summary>
    ''' コンボボックスに表示するグレード
    ''' </summary>
    ''' <remarks>コンボボックス表示名称</remarks>
    Private Class mcls_CmbItem

        ''' <summary>EdasysID</summary>
        Public id As Integer                             'EdasysID
        ''' <summary>表示名</summary>
        Public name As String                            '表示名

        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="nid">EdasysID</param>
        ''' <param name="strName">表示名</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal nid As Integer, ByVal strName As String)
            id = nid
            name = strName
        End Sub

        ''' <summary>
        ''' 構造体の表示名称
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overrides Function ToString() As String
            Return name
        End Function
    End Class

    ''' <summary>
    ''' イニシャライズ
    ''' </summary>
    ''' <param name="clsLog"></param>
    ''' <remarks></remarks>
    Public Sub New(ByRef clsLog As tClassLibrary.tClass.tClass_LogManager)

        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        'ログクラスセット
        mcls_Log = clsLog

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 表示設定
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize(ByVal dgv As DataGridView)
        With dgv

            .ColumnCount = 0

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
            '.ColumnHeadersHeight = 38                                      ' ヘッダーの高さ
            .RowTemplate.Height = 19                                        ' 改行されてる文字が見えてしまうので、デフォルトの行高さを変更

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = False                                               ' 読み取り専用はダメ
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' セル書き換え

            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '
            '''' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '選択セル色定義()
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue                ' 選択
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent      ' 選択色 無効化 (dgvLog_SelectionChangedが必須)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '一行おきに選択色変更
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True

        End With
    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmSetFilter_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        'データ取得処理
        If Not DB_Select() Then
            Exit Sub
        End If

        'コンボボックスの表示データを初期化
        Data_Init()

        'DataGridViewタグNO設定
        dgvShousaiData.Tag = MC_SHOUSAIDATA
        dgvShousaiEtc.Tag = MC_SHOUSAIETC

        '詳細dgv設定
        dgvInitialize(dgvShousaiData)
        dgvShousaiColunmDefData(dgvShousaiData, True)
        SetCommonEvent(dgvShousaiData)

        '詳細dgv「上記以外」
        dgvInitialize(dgvShousaiEtc)
        dgvShousaiColunmDefData(dgvShousaiEtc, False)
        SetCommonEvent(dgvShousaiEtc)

        'データ表示
        Data_Disp()

        '選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
        DataGridViewSelectionRowColorChange(dgvShousaiData)
    
        '詳細dgv「上記以外はCellEnterのみ
        AddHandler dgvShousaiEtc.CellEnter, AddressOf dgvData_CellEnter

    End Sub

    ''' <summary>
    ''' データグリッドの共通のイベントハンドラーの割付
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub SetCommonEvent(ByVal dgv As DataGridView)

        AddHandler dgv.SelectionChanged, AddressOf dgvData_SelectionChanged
        AddHandler dgv.CurrentCellDirtyStateChanged, AddressOf dgv_CurrentCellDirtyStateChanged
        AddHandler dgv.CellValueChanged, AddressOf dgv_CellValueChanged

    End Sub

    ''' <summary>
    ''' 選択一行色変え
    ''' </summary>
    ''' <param name="dgv">データグリッドビューコントロール</param>
    ''' <remarks></remarks>
    Private Sub DataGridViewSelectionRowColorChange(ByVal dgv As DataGridView)
        ' 下の関数にイベントハンドル割付
        AddHandler dgv.RowEnter, AddressOf dgvData_RowEnter
        AddHandler dgv.RowLeave, AddressOf dgvData_RowLeave
        AddHandler dgv.CellEnter, AddressOf dgvData_CellEnter

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 操作関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' データグリッドビュー選択を無効化
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)

        '選択を無効にする
        CType(sender, DataGridView).CurrentCell.Selected = False
    End Sub

    ''' <summary>
    ''' データグリッドビュー行選択
    ''' </summary>
    Private Sub dgvData_RowEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' 対象のdgv

        dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = m_ColorSelection      '行の背景を変更

    End Sub
    
    ''' <summary>
    ''' データグリッドビュー行非選択
    ''' </summary>
    Private Sub dgvData_RowLeave(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' 対象のdgv

        dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty           '行の背景をデフォルト色に戻す

    End Sub

    ''' <summary>
    ''' データグリッドビューセル選択
    ''' </summary>
    Private Sub dgvData_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' 対象のdgv

        ' 列のタイプがコンボボックスか？
        If TypeOf dgv.Columns(e.ColumnIndex) Is DataGridViewComboBoxColumn Then
            ' ドロップダウン表示
            SendKeys.Send("%{DOWN}")
        End If

    End Sub

    ''' <summary>
    ''' CurrentCellDirtyStateChangedイベントハンドラ
    ''' </summary>
    Private Sub dgv_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)
        ' セルがコミットされていない変更を含む場合コミットする
        If dgv.IsCurrentCellDirty Then dgv.CommitEdit(DataGridViewDataErrorContexts.Commit)
    End Sub

    ''' <summary>
    ''' 特定セルの値が変わったら色を変更
    ''' </summary>
    Private Sub dgv_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) 
        Dim dgv As DataGridView = CType(sender, DataGridView)
        Dim check_cnt As Integer = 0    ' チェック数
        Dim strEdasysID As String

        ' 行以外はスルー
        If e.RowIndex < 0 Then Exit Sub

        ' グレードコンボボックスの色変更
        If CInt(dgv.Tag) = MC_SHOUSAIDATA Or CInt(dgv.Tag) = MC_SHOUSAIETC Then

            '詳細設定時-------------------------------------------------------------------------------------------------
            If e.ColumnIndex = em_dgvShousaiColum.iCombo_Grade Then
                'グレードコンボボックス設定変更
                ' セルの値を取得
                strEdasysID = dgv(e.ColumnIndex, e.RowIndex).Value.ToString

                ' セルの値（EdasysID）で色を取得し、背景色を変更する 
                dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = GetCmbColor(strEdasysID)
            End If

            If e.ColumnIndex = em_dgvShousaiColum.iCombo_KaniUmu Then
                ' 簡易出力設定変更
                If dgv(e.ColumnIndex, e.RowIndex).Value.ToString = "○" Then
                    dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = Color.PaleTurquoise

                Else
                    dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = Color.Empty
                End If

            End If

        End If

    End Sub


    ''' <summary>
    ''' ファンクションボタン押下
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmSetFilter_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton

        Select Case FncNo

            Case 0  '設定

                '設定処理
                If Not subData_UpDate() Then
                    Exit Sub
                End If

                ''統括機能へパラメータ変更通知
                modMain.Send_Mail()

                Me.Close()

            Case 11 'キャンセル
                Me.Close()
        End Select

    End Sub

    ''' <summary>
    ''' DBからとってきたコンボボックス用の値を表示用に加工
    ''' </summary>
    ''' <remarks>グレード名称に以上をつける、無しの行を追加、</remarks>
    Private Sub Data_Init()
        Dim drowNewType As DataRow      ' 疵種マスターの新規行
        Dim drowNewGrade As DataRow     ' 疵種グレードの新規行
        Dim ii As Integer               ' ループカウンタ

        Dim nKaniKomoku As Integer      ' ALLCODE_OUTPUTの簡易項目をセットする

        '疵種非接続テーブルの設定
        ' 疵種の新規行を設定 EdasysID、疵名称 
        drowNewType = m_DsShousaiType.Tables(0).NewRow()
        drowNewType.Item("EdasysID") = 0
        drowNewType.Item("Name") = "無し"
        ' テーブルの一番上に挿入
        m_DsShousaiType.Tables(0).Rows.InsertAt(drowNewType, 0)

        'グレード非接続テーブルの設定
        ' 全行ループ グレードの名称に以上をつける
        For ii = 0 To m_DsShousaiGrade.Tables(0).Rows.Count - 1
            m_DsShousaiGrade.Tables(0).Rows(ii).Item("Name") = m_DsShousaiGrade.Tables(0).Rows(ii).Item("Name").ToString & "以上"
            m_DsGrade.Tables(0).Rows(ii).Item("Name") = m_DsGrade.Tables(0).Rows(ii).Item("Name").ToString & "以上"
        Next
        ' グレードの新規行を設定 EdasysID、グレード名称、マップ表示色
        drowNewGrade = m_DsShousaiGrade.Tables(0).NewRow()
        drowNewGrade.Item("EdasysID") = 0
        drowNewGrade.Item("Name") = "-"
        drowNewGrade.Item("マップ表示色") = -1      ' マップ表示色、使わないけど一応設定
        ' テーブルの一番上に挿入
        m_DsShousaiGrade.Tables(0).Rows.InsertAt(drowNewGrade, 0)

        '重欠陥一覧画面表示_簡易項目を取得する
        nKaniKomoku = 0
        For ii = 0 To m_DsAll.Tables(0).Rows.Count - 1
            If CInt(m_DsAll.Tables(0).Rows(ii).Item("重欠陥一覧画面表示_簡易項目").ToString.Trim) <> 0 Then
                nKaniKomoku = CInt(m_DsAll.Tables(0).Rows(ii).Item("重欠陥一覧画面表示_簡易項目").ToString.Trim)
                Exit For
            End If
        Next ii
        ' グレード設定コンボボックスにアイテムをセット
        For ii = 0 To m_DsGrade.Tables(0).Rows.Count - 1
            cmbKaniGrade.Items.Add(New mcls_CmbItem(CInt(m_DsGrade.Tables(0).Rows(ii).Item("EdasysID").ToString),
                                                    m_DsGrade.Tables(0).Rows(ii).Item("Name").ToString))
            'グレード設定コンボボックスの初期表示
            If CType(cmbKaniGrade.Items(ii), mcls_CmbItem).id = nKaniKomoku Then
                cmbKaniGrade.SelectedIndex = ii

            End If
        Next ii

    End Sub

    ''' <summary>
    ''' 詳細設定のデータdgv列定義
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <param name="nkbn">dgvのタイプ True = 詳細 データ, False = 詳細「上記以外」</param>
    ''' <remarks></remarks>
    Private Sub dgvShousaiColunmDefData(ByVal dgv As DataGridView, ByVal nkbn As Boolean)

        With dgv
            '詳細dgv-----------------------------------------------------------------------------
            'NO
            Using clm As New DataGridViewTextBoxColumn
                '詳細dgvデータ表示エリア
                clm.HeaderText = "NO"
                clm.Width = 25
                clm.Frozen = True
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                .Columns.Add(clm)
            End Using

            '疵種
            If nkbn = True Then
                '詳細dgv 疵種
                Using clm As New DataGridViewComboBoxColumn
                    clm.HeaderText = "疵種"
                    clm.Width = 160
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.Frozen = True
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    clm.DataSource = m_DsShousaiType.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
                    clm.ValueMember = "EdasysID"                    ' コンボボックスの実際の値（データソースの列名）
                    clm.DisplayMember = "Name"                      ' コンボボックスで表示される値（データソースの列名）
                    clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
                    clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
                    .Columns.Add(clm)
                End Using
            Else
                '詳細dgv 「上記以外」
                Using clm As New DataGridViewTextBoxColumn
                    clm.Width = 160
                    clm.Frozen = True
                    clm.ReadOnly = True
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    .Columns.Add(clm)
                End Using
            End If

            'グレード
            Using clm As New DataGridViewComboBoxColumn
                clm.HeaderText = "詳細出力"
                clm.Width = 93
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.Frozen = True
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.DataSource = m_DsShousaiGrade.Tables(0)     ' コンボボックスで表示するデータソース（グレード非接続テーブル）
                clm.ValueMember = "EdasysID"                    ' コンボボックスの実際の値（データソースの列名）
                clm.DisplayMember = "Name"                      ' コンボボックスで表示される値（データソースの列名）
                clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
                clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
                .Columns.Add(clm)

            End Using

            '簡易出力
            If nkbn = True Then
                '詳細dgv 
                Using clm As New DataGridViewComboBoxColumn
                    clm.HeaderText = "簡易出力"
                    clm.Width = 93
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.Frozen = True
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    clm.Items.AddRange("○", "-")
                    clm.MaxDropDownItems = clm.Items.Count
                    .Columns.Add(clm)

                End Using
            Else
                '詳細dgv 「上記以外」
                Using clm As New DataGridViewTextBoxColumn
                    clm.Width = 93
                    clm.Frozen = True
                    clm.ReadOnly = True
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    .Columns.Add(clm)
                End Using
            End If

            If nkbn = False Then
                '詳細dgv「上記以外」は列ヘッダーを表示しない
                .ColumnHeadersVisible = False        ' 列ヘッダー非表示
            End If
        End With
    End Sub


    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()
        Dim drowAllData() As DataRow = m_DsAll.Tables(0).Select("疵種EdasysID>=0")
        Dim drowAllEtc() As DataRow = m_DsAll.Tables(0).Select("疵種EdasysID=-1")
        Dim drowType() As DataRow = m_DsType.Tables(0).Select()
        Dim drowGrade() As DataRow = m_DsGrade.Tables(0).Select()
        Dim drowCommon() As DataRow = m_DsCommon.Tables(0).Select()             '疵検定数マスターテーブル

        Dim ii As Integer                                                       'ループカウンタ
        Dim jj As Integer                                                       'ループカウンタ
        Dim nRowCount As Integer = 0                                            'コンボ表示用カウンタ

        Dim ndisp_flg As Integer = CInt(drowCommon(0).Item("重欠陥一覧画面_フィルタ"))   '詳細<>簡易項目表示切替フラグ
        Dim strWhere As String = ""


        '表示レコード数セット
        dgvShousaiData.RowCount = MAX_TYPE                                      '詳細dgv 最大表示レコード数セット(=64)

        '詳細 疵種、グレード表示----------------------------------------------------------------------------------------------
        For ii = 0 To dgvShousaiData.RowCount - 1
            dgvShousaiData(em_dgvShousaiColum.iText_No, ii).Value = ii + 1      'No
            If ii < drowAllData.Length Then
                For jj = nRowCount To m_DsAll.Tables(0).Rows.Count - 1
                    If CInt(m_DsAll.Tables(0).Rows(jj).Item("疵種EdasysID").ToString) >= 0 Then
                        dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, ii).Value = m_DsAll.Tables(0).Rows(jj).Item("疵種EdasysID")              '詳細dgv 疵種コンボボックス初期表示
                        dgvShousaiData(em_dgvShousaiColum.iCombo_Grade, ii).Value = m_DsAll.Tables(0).Rows(jj).Item("重欠陥一覧画面表示_詳細条件")     '詳細dgv グレードコンボボックス初期表示
                        '詳細dgv 簡易出力表示
                        If m_DsAll.Tables(0).Rows(jj).Item("重欠陥一覧画面表示_簡易項目").ToString.Trim <> "0" Then
                            dgvShousaiData(em_dgvShousaiColum.iCombo_KaniUmu, ii).Value = "○"
                        Else
                            dgvShousaiData(em_dgvShousaiColum.iCombo_KaniUmu, ii).Value = "-"
                        End If
                        nRowCount = jj + 1
                        Exit For
                    End If
                Next jj
            Else
                dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, ii).Value = 0 '詳細dgv 疵種コンボボックス初期表示
                dgvShousaiData(em_dgvShousaiColum.iCombo_Grade, ii).Value = 0   '詳細dgv グレードコンボボックス初期表示
                dgvShousaiData(em_dgvShousaiColum.iCombo_KaniUmu, ii).Value = "-"  '詳細dgv 簡易有期表示
            End If
        Next ii

        '詳細 「上位以外」表示
        'その他dgvの設定
        dgvShousaiEtc.RowCount = 1                                              '詳細dgv「上記以外」表示は1行固定
        dgvShousaiEtc(em_dgvShousaiColum.iText_No, 0).Value = ""                '詳細dgv「上記以外」Noは空白
        dgvShousaiEtc(em_dgvShousaiColum.iCombo_Kizushu, 0).Value = "上記以外"  '詳細dgv「上記以外」疵種（"上記以外"と表示する）

        'データが存在するか
        If drowAllEtc.Length > 0 Then
            dgvShousaiEtc(em_dgvShousaiColum.iCombo_Grade, 0).Value = drowAllEtc(0).Item("重欠陥一覧画面表示_詳細条件")            '詳細dgv「上記以外」グレードコンボボックス初期表示
        Else
            dgvShousaiEtc(em_dgvShousaiColum.iCombo_Grade, 0).Value = 0
        End If

        dgvShousaiEtc(em_dgvShousaiColum.iCombo_KaniUmu, 0).Value = "-"         '詳細dgv「上記以外」簡易出力（"-"固定）
        '----------------------------------------------------------------------------------------------------------------------

        '詳細、簡易設定切替        
        If MC_SHOUSAI = ndisp_flg Then
            '詳細設定
            radShousai.Checked = True
        Else
            '簡易設定
            radKani.Checked = True
        End If

    End Sub


    ''' <summary>
    ''' データベースから値を取得する
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""         'SQL文格納エリア
        Dim strErrMsg As String = ""      'エラー内容格納エリア

        'テーブル"ALLCODE_OUTPUT"値取得
        strSQL = "SELECT 疵種EdasysID, 重欠陥一覧画面表示_詳細条件, 重欠陥一覧画面表示_簡易条件, 重欠陥一覧画面表示_簡易項目"
        strSQL &= " FROM " & DB_PALL_OUTPUT
        strSQL &= " ORDER BY 疵種EdasysID ASC"

        'SQL文実行
        If 0 > Get_SelectData(strSQL, m_DsAll, m_AdapterAll, strErrMsg) Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, DB_PALL_OUTPUT & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        'テーブル"COMMON_MASTER"値取得
        strSQL = "SELECT LINEID, 重欠陥一覧画面_フィルタ "
        strSQL &= " FROM " & DB_COMMON_MASTER

        'SQL文実行
        If 0 > Get_SelectData(strSQL, m_DsCommon, m_AdapterCommon, strErrMsg) Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, DB_COMMON_MASTER & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        'テーブル"TYPE_NAME"値取得
        strSQL = "SELECT EdasysID, Name "
        strSQL &= " FROM " & DB_TYPE_NAME
        strSQL &= " ORDER BY ID ASC"

        If 0 > Get_SelectData(strSQL, m_DsType, m_AdapterType, strErrMsg) Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, DB_TYPE_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        'テーブル"GRADE_NAME"値取得
        strSQL = "SELECT EdasysID, Name, マップ表示色 "
        strSQL &= " FROM " & DB_GRADE_NAME
        strSQL &= " ORDER BY 優先度 ASC"

        If 0 > Get_SelectData(strSQL, m_DsGrade, m_AdapterGrade, strErrMsg) Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, DB_GRADE_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        m_DsShousaiType = m_DsType.Copy()      '取得した疵種を詳細dgv用テーブルにコピー
        m_DsShousaiGrade = m_DsGrade.Copy()    '取得したグレードを詳細dgv用テーブルにコピー
        Return True

    End Function

    ''' <summary>
    ''' データ取得
    ''' </summary>
    ''' <param name="strSQL">発行するSQL文</param>
    ''' <param name="DsDataSet">データセットオブジェクト</param>
    ''' <param name="AdapterData">データアダプタオブジェクト</param>
    ''' <param name="strErrMsg">エラーメッセージ</param>
    ''' <returns>影響を受けた行数を返す　-1:失敗　-9:DB接続失敗</returns>
    ''' <remarks></remarks>
    Private Function Get_SelectData(ByVal strSQL As String, ByRef DsDataSet As System.Data.DataSet,
                                    ByRef AdapterData As SqlClient.SqlDataAdapter, ByRef strErrMsg As String) As Integer

        Dim tcls_DB As New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)

        'メッセージ初期化
        strErrMsg = ""

        Try

            ' SQL実行
            If Not tcls_DB.Open() Then
                strErrMsg = "DB接続失敗"
                Return -9
            End If

            ' データ取得
            DsDataSet.Clear()
            AdapterData = tcls_DB.SelectAdapter(strSQL)

            Return AdapterData.Fill(DsDataSet)


        Catch ex As Exception
            strErrMsg = ex.Message
            Return -1

        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

    End Function


    ''' <summary>
    ''' 色取得
    ''' </summary>
    ''' <param name="key">GrEdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetCmbColor(ByVal key As String) As Color
        'EdasysIDの値を取り出し()
        Dim strWhere As String = String.Format("EdasysID='{0}'", key)           ' WHERE句に相当の条件生成
        Dim drowGrade() As DataRow = m_DsShousaiGrade.Tables(0).Select(strWhere)       ' 1行あるはず

        Dim retColor As Color = Color.Empty   ' 戻り値 初期値はNULL色

        ' Keyが0ならNULL色を返す
        If 0 = CInt(key) Then Return retColor

        ' 1行しかないと思うけど
        For ii As Integer = 0 To drowGrade.Length - 1
            ' カラーを取得
            retColor = Color.FromArgb(CInt(drowGrade(0).Item("マップ表示色").ToString))
        Next

        Return retColor
    End Function

    ''' <summary>
    ''' 入力値のチェック
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strMsg As String       ' エラーメッセージ用
        Dim ii As Integer          ' ループカウンタ
        Dim jj As Integer          ' ループカウンタ

        Dim check_cnt As Integer    '○選択数
        Dim bKizuUmu As Boolean     '疵種が１つも選択されてないときFalse 1個でも疵種が選択されていたらTrue
        check_cnt = 0
        bKizuUmu = False

        '簡易出力を"○"選択してる項目数をチェック
        For ii = 0 To dgvShousaiData.Rows.Count - 1
            If CStr(dgvShousaiData(em_dgvShousaiColum.iCombo_KaniUmu, ii).Value) = "○" And
               CInt(dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, ii).Value) <> 0 Then
                '疵種が未選択、簡易出力が○の項目があったらカウントアップ
                check_cnt = check_cnt + 1
            End If
        Next ii

        ' 行数分ループ コンボボックスしかないので、疵種EdasysIDの重複チェックのみ行う
        For ii = 0 To dgvShousaiData.Rows.Count - 1
            ' 疵種が無しの行は無視
            If CInt(dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, ii).Value) = 0 Then Continue For

            '疵種が選択されていたらフラグをTrueにする
            bKizuUmu = True

            ' 疵種の重複チェック
            For jj = ii + 1 To dgvShousaiData.Rows.Count - 1
                ' 比較先が無しの行は無視
                If CInt(dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, jj).Value) = 0 Then Continue For

                ' 疵種が同じならメッセージを出して終了
                If CInt(dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, ii).Value) =
                                    CInt(dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, jj).Value) Then
                    strMsg = "【 疵種 】" & vbNewLine
                    strMsg &= "同じ疵種は登録できません。" & vbNewLine
                    strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)

                    'メッセージ表示
                    Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "データ登録異常")
                        frmMsg.ShowDialog()
                    End Using
                    Return False
                End If
            Next

        Next ii

        '簡易出力数を判定 超え時
        If check_cnt > MC_KIZUSHU_NUM Then
            strMsg = "【 簡易出力 】" & vbNewLine
            strMsg &= "簡易出力は" & MC_KIZUSHU_NUM & "個以上設定できません。" & vbNewLine

            'メッセージ表示
            Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "データ登録異常")
                frmMsg.ShowDialog()
            End Using
            Return False
        End If

        '簡易出力数を判定 未選択
        If check_cnt <= 0 Or bKizuUmu = False Then
            strMsg = "【 疵種 】" & vbNewLine
            strMsg &= "簡易出力が未設定になるため、削除できません。" & vbNewLine

            'メッセージ表示
            Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "データ登録異常")
                frmMsg.ShowDialog()
            End Using
            Return False
        End If

        '簡易出力数を判定 未選択
        If check_cnt <= 0 Then
            strMsg = "【 簡易出力 】" & vbNewLine
            strMsg &= "簡易出力が未設定です。" & vbNewLine

            'メッセージ表示
            Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "データ登録異常")
                frmMsg.ShowDialog()
            End Using
            Return False
        End If

        '簡易グレード設定が選択されてないとき(ないハズ)
        If cmbKaniGrade.Text.Trim = "" Then
            strMsg = "【 簡易グレード設定 】" & vbNewLine
            strMsg &= "簡易グレード設定が未設定です。" & vbNewLine

            'メッセージ表示
            Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "データ登録異常")
                frmMsg.ShowDialog()
            End Using
            Return False
        End If

        Return True
    End Function

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <returns>True:正常 False:異常</returns>
    ''' <remarks></remarks>
    Private Function Data_Get() As Boolean
        Dim drow() As DataRow = m_DsAll.Tables(0).Select("")        'データ行 共通出力設定テーブルのデータ全て割り当て
        Dim drowNew As DataRow                                      '新規行
        Dim nRowCount As Integer                                    '削除対象のデータ数

        Dim strWhere2 As String = ""                                '簡易グレード抽出条件

        Dim bShousai_flg As Boolean = False  '詳細設定にデータが１つでも選択されているか否か True=設定あり False=設定なし
        Dim bKizu_flg As Boolean = False     '簡易設定の疵種が１つでも選択されているか否か True=チェックあり False=チェックなし
        Dim bGrade_flg As Boolean = False    '簡易設定のグレード１つでもが選択されているか否か True=チェックあり False=チェックなし

        Dim ii As Integer
        Dim mm As Integer

        '詳細設定時-------------------------------------------------------------------------------------------------------
        '現在画面に表示しているpcodeの非接続テーブルのデータを削除(後ろから削除)
        nRowCount = drow.Length
        For ii = 1 To nRowCount
            m_DsAll.Tables(0).Rows.Remove(drow(nRowCount - ii))
        Next

        ' 今表示しているデータを全て格納
        For ii = 0 To dgvShousaiData.Rows.Count - 1
            drowNew = m_DsAll.Tables(0).NewRow()                             ' 新規行
            '' データをセット
            drowNew.Item("疵種EdasysID") = dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, ii).Value
            drowNew.Item("重欠陥一覧画面表示_詳細条件") = dgvShousaiData(em_dgvShousaiColum.iCombo_Grade, ii).Value
            If CStr(dgvShousaiData(em_dgvShousaiColum.iCombo_KaniUmu, ii).Value) = "○" Then
                drowNew.Item("重欠陥一覧画面表示_簡易条件") = CStr(CType(cmbKaniGrade.Items(cmbKaniGrade.SelectedIndex), mcls_CmbItem).id)
                drowNew.Item("重欠陥一覧画面表示_簡易項目") = CStr(CType(cmbKaniGrade.Items(cmbKaniGrade.SelectedIndex), mcls_CmbItem).id)
            Else
                drowNew.Item("重欠陥一覧画面表示_簡易条件") = 0
                drowNew.Item("重欠陥一覧画面表示_簡易項目") = 0
            End If
            ' テーブルに追加
            m_DsAll.Tables(0).Rows.InsertAt(drowNew, m_DsAll.Tables(0).Rows.Count)
        Next ii

        ' 上記以外dgvのデータを全て格納 詳細設定の上記以外の設定は1行しかない
        For mm = 0 To dgvShousaiEtc.Rows.Count - 1
            drowNew = m_DsAll.Tables(0).NewRow()                             ' 新規行
            ' データをセット
            drowNew.Item("疵種EdasysID") = -1              '「上記以外」の設定の疵種EdasysIDは-1固定
            drowNew.Item("重欠陥一覧画面表示_詳細条件") = dgvShousaiEtc(em_dgvShousaiColum.iCombo_Grade, mm).Value
            drowNew.Item("重欠陥一覧画面表示_簡易条件") = 0
            drowNew.Item("重欠陥一覧画面表示_簡易項目") = 0
            ' テーブルに追加
            m_DsAll.Tables(0).Rows.InsertAt(drowNew, m_DsAll.Tables(0).Rows.Count)
        Next mm

        Return True

    End Function

    ''' <summary>
    ''' 確定処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Function subData_UpDate() As Boolean

        Dim strMsg As String = ""       ' エラーメッセージ用

        '入力値チェック
        If Not Data_Check() Then Return False

        '画面のデータを取得
        If Not Data_Get() Then Return False

        'DB書込み
        If Not DB_UpDate() Then
            strMsg = "登録に失敗"
            'メッセージ表示
            Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "確定処理")
                frmMsg.ShowDialog()
            End Using
        End If

        Return True

    End Function

    ''' <summary>
    ''' アップデート
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB接続
        tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSql As String                                'Sql文
        Dim ndisp_flg As Integer                            '詳細<>簡易項目表示切替フラグ
        Dim ii As Integer                                   'ループカウンタ

        Try
            'データベースオープン
            If Not tcls_DB.Open() Then Return False

            'トランザクション開始
            tcls_DB.TransactStart()

            'デリートインサートするので検査パターン出力設定テーブルの全項目を削除
            strSql = "DELETE FROM " & DB_PALL_OUTPUT
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' エラーならロールバックして終了
                tcls_DB.TransactRollback()
                Return False
            End If

            '検査パターン出力設定テーブルへデータを登録 
            'データテーブルは複数テーブルを参照している為、アップデートできないので、インサート文で
            For ii = 0 To m_DsAll.Tables(0).Rows.Count - 1
                With m_DsAll.Tables(0).Rows(ii)
                    If CInt(.Item("疵種EdasysID").ToString) <> 0 Then
                        'TYPEOUT登録SQL文作成
                        strSql = ""
                        strSql &= "INSERT INTO " & DB_PALL_OUTPUT & " VALUES ("
                        strSql &= .Item("疵種EdasysID").ToString & ","                  ' 疵種EdasysID
                        strSql &= .Item("重欠陥一覧画面表示_詳細条件").ToString & ","       ' 重欠陥一覧画面表示_詳細条件
                        strSql &= .Item("重欠陥一覧画面表示_簡易条件").ToString & ","       ' 重欠陥一覧画面表示_簡易条件
                        strSql &= .Item("重欠陥一覧画面表示_簡易項目").ToString & ")"       ' 重欠陥一覧画面表示_簡易項目

                        'SQL実行
                        If 0 > tcls_DB.DMLExecute(strSql) Then
                            ' エラーならロールバックして終了
                            tcls_DB.TransactRollback()
                            Return False
                        End If
                    End If
                End With
            Next

            '疵検定数マスターテーブルの重欠陥一覧画面_フィルタ更新-----------------------------------------------------------------
            '使用する画面表示設定の判定
            If radShousai.Checked = True Then
                '詳細設定を選択
                ndisp_flg = MC_SHOUSAI
            Else
                '簡易設定を選択
                ndisp_flg = MC_KANI
            End If

            strSql = ""
            strSql &= "UPDATE " & DB_COMMON_MASTER & " SET "
            strSql &= "重欠陥一覧画面_フィルタ = " & ndisp_flg
            'strSql &= " WHERE LINEID=" & KizuLibCLI.KizuMod.GetLineID()
            tClass_SQLServer.sqlColumnTrim(strSql)                  'おまじない 必ず書いてね
            'SQL実行
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' エラーならロールバックして終了
                tcls_DB.TransactRollback()
                Return False
            End If

            'コミット
            tcls_DB.TransactCommint()

            Return True

        Catch ex As Exception
            ' エラーならロールバックして終了
            tcls_DB.TransactRollback()
            mcls_Log.Write(tClass_LogManager.LNO.ERR, " 設定エラー [" & ex.Message & "] " & Me.Text)
            Return False
        Finally
            '''' 後始末
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

    End Function


    ''' <summary>
    ''' 表示方法選択
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub radButton_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles radShousai.CheckedChanged, radKani.CheckedChanged

        Dim rdoData As RadioButton = CType(sender, RadioButton)

        'ラジオボタン背景色色変え
        If rdoData.Checked Then
            rdoData.BackColor = Color.Lime
        Else
            rdoData.BackColor = g_ColorControl
        End If

    End Sub
    ''' <summary>
    ''' 簡易グレード設定変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbKaniGrade_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cmbKaniGrade.SelectedIndexChanged
        cmbKaniGrade.BackColor = GetCmbColor(CStr(CType(cmbKaniGrade.Items(cmbKaniGrade.SelectedIndex), mcls_CmbItem).id))
    End Sub

End Class
