'*********************************************************************************
' 疵一覧画面表示設定クラス
'	[Ver]
'		Ver.01    2008/06/17  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmAllCodeOutput

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    'DataGridViewタグNO
    Private Const MC_SHOUSAIDATA As Integer = 0    '詳細dgv データ表示エリア
    Private Const MC_SHOUSAIETC As Integer = 1     '詳細dgv 「上記以外」

    Private Enum em_dgvShousaiColum
        iText_No = 0          'NO
        iCombo_Kizushu        '疵種
        iCombo_Grade          '詳細出力
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバ変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterAll As SqlClient.SqlDataAdapter = Nothing      'アダプター 共通出力設定テーブル
    Private m_AdapterType As SqlClient.SqlDataAdapter = Nothing     'アダプター 疵種マスター
    Private m_AdapterGrade As SqlClient.SqlDataAdapter = Nothing    'アダプター グレードマスター

    Private m_DsAll As New DataSet                                  '非接続型テーブル 共通出力設定テーブル
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

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メニューボタン
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ファンクションボタン押下時処理
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="enable"></param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmSetPat_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "重大欠陥表示設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
                modMain.ChgProject()            ' F09 KA_PARCOLORへ切替処理
            Case 10
                btnF11.Select()                 ' F11を選択状態にする dgvが書き込み中の時に、入力完了になるように
                subData_UpDate()                ' 設定
            Case 11
                modMain.TaskEnd()               ' F12 終了
        End Select
    End Sub

    ''' <summary>
    ''' 画面変更要求されたときの処理 (継承元クラスよりコールされる)
    ''' </summary>
    ''' <param name="frmID">選択画面ID</param>
    ''' <param name="cancel">選択を取りやめたい場合は、Trueをセット</param>
    ''' <remarks></remarks>
    Private Sub frmSetPat_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        'メモリキャッシュの開放
        m_DsAll.Dispose()                       '共通出力設定テーブル
        m_DsShousaiType.Dispose()               '疵検定数マスターテーブル
        m_DsShousaiGrade.Dispose()              '詳細dgv 疵種マスター コンボボックス用
        m_DsType.Dispose()                      '詳細dgv グレードマスター コンボボックス用
        m_DsGrade.Dispose()                     '簡易dgv 疵種マスター用

        '共通出力設定テーブル アダプター開放
        If Not m_AdapterAll Is Nothing Then
            m_AdapterAll.Dispose()
            m_AdapterAll = Nothing
        End If

        '疵種マスター アダプター開放
        If Not m_AdapterType Is Nothing Then
            m_AdapterType.Dispose()
            m_AdapterType = Nothing
        End If

        'グレードマスター アダプター開放
        If Not m_AdapterGrade Is Nothing Then
            m_AdapterGrade.Dispose()
            m_AdapterGrade = Nothing
        End If

        ' 画面切替
        ShowForm(frmID)

    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmAllCodeOutput_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        modMain.SetListMenu(Me)                                     ' メニューリスト生成
        modMain.SetLabelSysName(Me)                                 ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmAllCodeOutput         ' 疵一覧画面表示設定呼び出し

        'タイトル設定
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' このフォームで必要なデータを取得する コンボボックスの値もとってくるので、dgv設定より先
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If

        'コンボボックスの表示データを初期化
        Data_Init()

        'DataGridViewタグNO設定
        dgvShousaiData.Tag = MC_SHOUSAIDATA
        dgvShousaiEtc.Tag = MC_SHOUSAIETC

        '詳細dgv設定
        dgvInitialize(dgvShousaiData)
        dgvShousaiColunmDefData(dgvShousaiData, True)
        MyBase.DataGridViewSelectionCancel(dgvShousaiData)

        '詳細dgv「上記以外」
        dgvInitialize(dgvShousaiEtc)
        dgvShousaiColunmDefData(dgvShousaiEtc, False)
        MyBase.DataGridViewSelectionCancel(dgvShousaiEtc)

        'データ表示
        Data_Disp()

        '選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
        MyBase.DataGridViewSelectionRowColorChange(dgvShousaiData)

        ' 更新ボタンの有効/無効
        Call UpdateEnabled(g_bPassFlg)

        'プリンタがなければ、画面印刷無効
        If Not tClass_HeadCopy.IsCheckPrinter Then
            btnF07.Enabled = False
        Else
            btnF07.Enabled = True
        End If

        frmMsg.Dispose()
        frmMsg = Nothing

    End Sub

    ''' <summary>
    ''' ディスポーズ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmAllCodeOutput_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        
        '現在の表示中の画面が自分の画面ならアプリケーションを終了
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

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
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '一行おきに選択色変更
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True

        End With
    End Sub

    ''' <summary>
    ''' dgvの選択状態が変更されたら、セルの選択をキャンセル
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvShousaiData.SelectionChanged, dgvShousaiEtc.SelectionChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
        dgv.CurrentCell.Selected = False

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
                    clm.Width = 239
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
                    clm.Width = 239
                    clm.Frozen = True
                    clm.ReadOnly = True
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    .Columns.Add(clm)
                End Using
            End If

            '詳細出力
            Using clm As New DataGridViewComboBoxColumn
                clm.HeaderText = "詳細出力"
                clm.Width = 200
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

            If nkbn = False Then
                '詳細dgv「上記以外」は列ヘッダーを表示しない
                .ColumnHeadersVisible = False        ' 列ヘッダー非表示
            End If
        End With
    End Sub

    ''' <summary>
    ''' dgvのセルが選択されたときにコンボボックスならドロップダウンする
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>%{DOWN}でのドロップダウン操作が、開発PCだと出来ない、原因不明</remarks>
    Private Sub dataGridView_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvShousaiData.CellEnter, dgvShousaiEtc.CellEnter
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' 対象のdgv

        ' 列のタイプがコンボボックスか？
        If TypeOf dgv.Columns(e.ColumnIndex) Is DataGridViewComboBoxColumn Then
            ' ドロップダウン表示
            SendKeys.Send("%{DOWN}")
        End If
    End Sub

    ''' <summary>
    ''' DBからとってきたコンボボックス用の値を表示用に加工
    ''' </summary>
    ''' <remarks>グレード名称に以上をつける、無しの行を追加、</remarks>
    Private Sub Data_Init()
        Dim drowNewType As DataRow      ' 疵種マスターの新規行
        Dim drowNewGrade As DataRow     ' 疵種グレードの新規行
        Dim ii As Integer               ' ループカウンタ

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

    End Sub

    ''' <summary>
    ''' データベースから値を取得する
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""         'SQL文格納エリア
        Dim strErrMsg As String = ""      'エラー内容格納エリア

        'テーブル"PALL_OUTPUT"値取得
        strSQL = "SELECT 疵種EdasysID, 重欠陥一覧画面表示_詳細条件, 重欠陥一覧画面表示_簡易条件, 重欠陥一覧画面表示_簡易項目"
        strSQL &= " FROM " & DB_PALL_OUTPUT
        strSQL &= " ORDER BY 疵種EdasysID ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsAll, m_AdapterAll, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PCODE_OUTPUT & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        'テーブル"TYPE_NAME"値取得
        strSQL = "SELECT EdasysID, Name "
        strSQL &= " FROM " & DB_TYPE_NAME
        strSQL &= " ORDER BY ID ASC"

        If 0 > MyBase.Get_SelectData(strSQL, m_DsType, m_AdapterType, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_TYPE_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        'テーブル"GRADE_NAME"値取得
        strSQL = "SELECT EdasysID, Name, マップ表示色 "
        strSQL &= " FROM " & DB_GRADE_NAME
        strSQL &= " ORDER BY 優先度 ASC"

        If 0 > MyBase.Get_SelectData(strSQL, m_DsGrade, m_AdapterGrade, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_GRADE_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        m_DsShousaiType = m_DsType.Copy()      '取得した疵種を詳細dgv用テーブルにコピー
        m_DsShousaiGrade = m_DsGrade.Copy()    '取得したグレードを詳細dgv用テーブルにコピー
        Return True

    End Function

    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()
        Dim drowAllData() As DataRow = m_DsAll.Tables(0).Select("疵種EdasysID>=0")
        Dim drowAllEtc() As DataRow = m_DsAll.Tables(0).Select("疵種EdasysID=-1")
        Dim drowType() As DataRow = m_DsType.Tables(0).Select()
        Dim drowGrade() As DataRow = m_DsGrade.Tables(0).Select()

        Dim ii As Integer                                                       'ループカウンタ
        Dim jj As Integer                                                       'ループカウンタ
        Dim nRowCount As Integer = 0                                            'コンボ表示用カウンタ
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
                        nRowCount = jj + 1
                        Exit For
                    End If
                Next jj
            Else
                dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, ii).Value = 0     '詳細dgv 疵種コンボボックス初期表示
                dgvShousaiData(em_dgvShousaiColum.iCombo_Grade, ii).Value = 0       '詳細dgv グレードコンボボックス初期表示
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

        '----------------------------------------------------------------------------------------------------------------------

    End Sub

    ''' <summary>
    ''' 入力値のチェック
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strMsg As String       ' エラーメッセージ用
        Dim ii As Integer          ' ループカウンタ
        Dim jj As Integer          ' ループカウンタ

        Dim bKizuUmu As Boolean     '疵種が１つも選択されてないときFalse 1個でも疵種が選択されていたらTrue

        bKizuUmu = False

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
                If CInt(dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, ii).Value) = _
                                    CInt(dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, jj).Value) Then
                    strMsg = "【 疵種 】" & vbNewLine
                    strMsg &= "同じ疵種は登録できません。" & vbNewLine
                    strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                    MsgBox(strMsg, MsgBoxStyle.OkOnly Or MsgBoxStyle.Information, "データ登録異常")
                    Return False
                End If
            Next
        Next ii

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
            drowNew.Item("重欠陥一覧画面表示_簡易条件") = 0
            drowNew.Item("重欠陥一覧画面表示_簡易項目") = 0
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
    Private Sub subData_UpDate()
        '入力値チェック
        If Not Data_Check() Then Exit Sub

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try
            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '画面のデータを取得
            If Not Data_Get() Then Exit Sub

            ' ホントに登録        
            If DB_UpDate() Then
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("正常に登録しました", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub
            Else
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("登録に失敗", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

            ' データの再取得
            If Not DB_Select() Then
                ' DB 失敗なので、画面強制終了
                frmMsg.SetSelectedFNo(0)

                frmMsg.Text = "異常"
                frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       '終了
            End If

            ' 表示データを初期化、「無し」行を追加、「－」行を追加
            Data_Init()

            'データ再表示
            Data_Disp()

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

    End Sub

    ''' <summary>
    ''' アップデート
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB接続
        tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSql As String                                'Sql文
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
                        strSql &= .Item("重欠陥一覧画面表示_詳細条件").ToString & ","       ' 疵一覧画面表示_詳細条件
                        strSql &= .Item("重欠陥一覧画面表示_簡易条件").ToString & ","       ' 疵一覧画面表示_簡易条件
                        strSql &= .Item("重欠陥一覧画面表示_簡易項目").ToString & ")"       ' 疵一覧画面表示_簡易項目

                        'SQL実行
                        If 0 > tcls_DB.DMLExecute(strSql) Then
                            ' エラーならロールバックして終了
                            tcls_DB.TransactRollback()
                            Return False
                        End If
                    End If
                End With
            Next

            'コミット
            tcls_DB.TransactCommint()

            '統括機能へパラメータ変更通知
            modMain.Send_Main(CH_PARAM_DEFLIST)
			modMain.HardCopy(Me)
            Return True

        Catch ex As Exception
            ' エラーならロールバックして終了
            tcls_DB.TransactRollback()
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " 設定エラー [" & ex.Message & "] " & Me.Text)
            Return False
        Finally
            '''' 後始末
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

    End Function

    ''' <summary>
    ''' CurrentCellDirtyStateChangedイベントハンドラ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgv_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvShousaiEtc.CurrentCellDirtyStateChanged, dgvShousaiData.CurrentCellDirtyStateChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
        ' セルがコミットされていない変更を含む場合コミットする
        If dgv.IsCurrentCellDirty Then dgv.CommitEdit(DataGridViewDataErrorContexts.Commit)
    End Sub

    ''' <summary>
    ''' 特定セルの値が変わったら色を変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgv_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvShousaiData.CellValueChanged, dgvShousaiEtc.CellValueChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
        Dim drow() As DataRow = m_DsType.Tables(0).Select()
        Dim check_cnt As Integer = 0    ' チェック数
        Dim msg As String = ""          ' メッセージボックス内容格納用
        Dim strEdasysID As String

        ' 行以外はスルー
        If e.RowIndex < 0 Then Exit Sub

        '詳細設定時-------------------------------------------------------------------------------------------------
        If CInt(dgv.Tag) = MC_SHOUSAIDATA Or CInt(dgv.Tag) = MC_SHOUSAIETC Then
            ' グレードコンボボックスの色変更
            If e.ColumnIndex = em_dgvShousaiColum.iCombo_Grade Then
                ' セルの値を取得
                strEdasysID = dgv(e.ColumnIndex, e.RowIndex).Value.ToString

                ' セルの値（EdasysID）で色を取得し、背景色を変更する 
                dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = GetCmbColor(strEdasysID)
            End If
        End If

    End Sub

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
    ''' 表示方法選択
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub radButton_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Dim rdoData As RadioButton = CType(sender, RadioButton)

        'ラジオボタン背景色色変え
        If rdoData.Checked Then
            rdoData.BackColor = Color.Lime
        Else
            rdoData.BackColor = g_ColorControl
        End If

    End Sub

    ''' <summary>
    ''' 更新ボタンの有効/無効
    ''' </summary>
    ''' <param name="bPassResult">パスワード入力結果 (True:有効 False:無効)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' ファンクションキーの設定ボタン

    End Sub

End Class
