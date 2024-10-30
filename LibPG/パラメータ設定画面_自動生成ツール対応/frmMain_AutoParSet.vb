'*********************************************************************************
' パラメータ設定画面の親フォーム
'	[Ver]
'		Ver.01    2007/03/31  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On
Imports tClassLibrary.tClass

''' <summary>
''' パラメータ設定画面用の基底クラス
''' </summary>
''' <remarks></remarks>
Public Class frmMain_AutoParSet
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' データグリッドビュー情報
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure DgvControlInfo
        Public dgv As tClassLibrary.tCnt.tCnt_DataGridView      'データグリッドビュー
        Public dgvType As EM_DATAGRIDVIEW_DISPTYPE              '表示タイプ
        Public wSize As ROW_VERTION_WIDTH_SIZE                  '幅サイズ(縦Verのみ有効)
    End Structure

    ''' <summary>
    ''' 画面構成情報
    ''' </summary>
    ''' <remarks></remarks>
    Class DataSetHolder
        Public drGridSetting() As DataRow                       '画面構成情報
        Public dsMain As DataSet                                '非接続型テーブル
        Public adapter As SqlClient.SqlDataAdapter              'アダプター
        Public dgvCnt As DgvControlInfo                         'データグリッドビュー情報

        Public Function Rows() As DataRow()
            Return drGridSetting
        End Function

        ''' <summary>
        ''' 
        ''' </summary>
        ''' <param name="gridSettings"></param>
        ''' <param name="dsMain"></param>
        ''' <param name="adapter"></param>
        ''' <param name="dgv"></param>
        ''' <remarks></remarks>
        Sub New(ByVal gridSettings As DataRow(), ByVal dsMain As DataSet, ByVal adapter As SqlClient.SqlDataAdapter, ByVal dgv As DgvControlInfo)
            Me.drGridSetting = gridSettings
            Me.dsMain = dsMain
            Me.adapter = adapter
            Me.dgvCnt = dgv
        End Sub
    End Class

    Protected m_DataSets As New List(Of DataSetHolder)

    Public m_DBlst As New Dictionary(Of String, TABLE_INFO)      ' 使用DB情報 (DB略称, データ)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 列挙体定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public Enum EM_DATAGRIDVIEW_CELLTYPE
        'データグリッドセルタイプ
        emTextBoxCell = 0
        emComboBoxCell = 1
        emCheckBoxCell = 2
        emButtonCell = 3
    End Enum

    Public Enum EM_DATAGRIDVIEW_DISPTYPE
        '表示タイプ
        emColType = 0             ' 列ヘッダータイプ(横)
        emRowType = 1             ' 行ヘッダータイプ(縦)
    End Enum

    Public Enum EM_DB_TYPE
        ' データタイプ
        emIntType = 0
        emFloatType = 1
        emStrType = 2
        emBitType = 3
    End Enum

    ' データグリッドビューの列 (表面状態別共通)
    Public Enum EM_ROW_VERTION_ID
        emNo = 0                ' No
        emName                  ' 名称
        emValue                 ' 値
        emRange                 ' 範囲
    End Enum

    Public Enum EM_DLG_TYPE
        ' データグリッドダブルクリック時に表示するダイアログタイプ
        emNoSet = 0
        emFileDialog = 1
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 構造体
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' データグリッドビューの列 (表面状態別共通)
    Public Structure ROW_VERTION_WIDTH_SIZE
        Dim nNo As Integer      ' No
        Dim nName As Integer    ' 名称
        Dim nValue As Integer   ' 値
        Dim nRange As Integer   ' 範囲
        Public Sub initialize()
            nNo = 0
            nName = 0
            nValue = 0
            nRange = 0
        End Sub
    End Structure

    '主キー情報 構造体
    Private Structure MAIN_KEY_INFO
        Dim nKeyPos As Integer                                          ' 表示位置
        Dim sKeyName As String                                          ' 主キー名称
        Public Sub initialize()
            nKeyPos = 0
            sKeyName = ""
        End Sub
    End Structure

    '使用DB情報 構造体
    Public Structure TABLE_INFO
        Dim sTable As String                                            'DB名称
        Dim SelectSQLFormat As String                                   'DB参照用フォーマット
        Dim UpdateSQLFormat As String                                   '一括コピー専用UpDateフォーマット
        Dim bCopyDB As Boolean                                          '一括コピー実施有無

        Public Sub initialize()
            sTable = ""
            SelectSQLFormat = ""
            UpdateSQLFormat = ""
            bCopyDB = False
        End Sub
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' 共通の色コード
    Public g_ColorSetting As Color = Color.PeachPuff                    '選択セルの色設定（データグリッドビューのチェックボックス限定）
    Public g_ColorSelection As Color = Color.LightGreen                 '選択セル一行分の色変え

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 外部イベント
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' メニューリストが選択された (継承先でオーバーライドしてね。)
    ''' </summary>
    ''' <param name="frmID">選択画面ID</param>
    ''' <param name="cancel">選択を取りやめたい場合は、Trueをセット</param>
    ''' <remarks></remarks>
    Public Event tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバ 変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private my_frmID As Integer                             ' 自分の画面ID
    Private my_paramID As Integer                           ' 自分のパラメータテーブルID

    ''' <summary>
    ''' コンボボックスに表示するデータ
    ''' </summary>
    ''' <remarks>検査表面状態・検査パターン 用</remarks>
    Public Class cls_CmbItem
        ''' <summary>主キー</summary>
        Public id As String                                 ' 主キー
        ''' <summary>紐付く名称</summary>
        Public name As String                               ' 紐付く名称
        ''' <summary>有効無効フラグ 0:無効 1:有効</summary>
        Public num As Integer                               ' 有効無効フラグ 0:無効 1:有効

        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="strID">主キー</param>
        ''' <param name="strName">紐付く名称</param>
        ''' <param name="inum">有効無効フラグ</param>		
        ''' <remarks></remarks>
        Public Sub New(ByVal strID As String, ByVal strName As String, ByVal inum As Integer)
            id = strID
            name = strName
            num = inum
        End Sub

        ''' <summary>
        ''' 構造体の表示名称
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overrides Function ToString() As String
            Return id
        End Function
    End Class


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 外部プロパティ
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 自分の画面ID(画面表示時に選ばれている項目)
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property tProp_MyFrmID() As Integer
        Get
            Return my_frmID
        End Get
        Set(ByVal Value As Integer)

            Dim nCnt As Integer = 0

            If Value < 0 Then Exit Property

            ' 自分のIDとして保持
            my_frmID = Value
            ' 自分のメニューリストを変更
            For Each lstData As ListViewItem In lstVMenu.Items
                If CInt(lstData.Tag) = Value Then

                    '自分のIDのリストを色変え
                    lstVMenu.Items(nCnt).BackColor = Color.Pink
                    lstVMenu.Items(nCnt).Font = New Font(lstVMenu.Items(nCnt).Font, FontStyle.Bold)
                    Exit For
                End If
                nCnt += 1
            Next

        End Set
    End Property

    Public Property tProp_MyParamID() As Integer
        Get
            Return my_paramID
        End Get
        Set(ByVal value As Integer)
            my_paramID = value
        End Set
    End Property

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メソッド
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' メニューアイテム追加
    ''' </summary>
    ''' <param name="strName">名称</param>
    ''' <param name="frmID">画面ID</param>
    ''' <remarks></remarks>
    Public Sub SetListMenu(ByVal strName As String, ByVal frmID As Integer)
        Dim item As New ListViewItem(strName)
        item.Tag = frmID
        lstVMenu.Items.Add(item)
    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_ParSet_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        '' メニューの定義
        With lstVMenu
            .Columns.Add("メニュー", .ClientSize.Width, HorizontalAlignment.Center)
        End With
    End Sub

    ''' <summary>
    ''' メニューリスト選択
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub lstVMenu_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles lstVMenu.Click
        If 0 = lstVMenu.SelectedItems.Count Then Exit Sub ' おかしい
        Dim frmID As Integer = CInt(lstVMenu.SelectedItems(0).Tag)     ' 選択位置取得
        If my_frmID = frmID Then Exit Sub ' 自分自身ならヤメ

        '' 継承先で処理させる
        Dim cancel As Boolean = False
        RaiseEvent tEv_SelectMenuList(frmID, cancel)

        '' キャンセルされた
        If cancel Then Exit Sub

        '' 自分の終了
        Me.Close()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ データグリッドビュー関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 選択キャンセル
    ''' </summary>
    ''' <param name="dgv">データグリッドビューコントロール</param>
    ''' <remarks></remarks>
    Public Sub DataGridViewSelectionCancel(ByVal dgv As DataGridView)
        ' 下の関数にイベントハンドル割付
        AddHandler dgv.SelectionChanged, AddressOf dgvData_SelectionChanged
    End Sub

    ''' <summary>
    ''' データグリッドビュー選択を無効化
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        CType(sender, DataGridView).CurrentCell.Selected = False
    End Sub

    ''' <summary>
    ''' 選択一行色変え
    ''' </summary>
    ''' <param name="dgv">データグリッドビューコントロール</param>
    ''' <remarks></remarks>
    Public Sub DataGridViewSelectionRowColorChange(ByVal dgv As DataGridView)
        ' 下の関数にイベントハンドル割付
        AddHandler dgv.RowEnter, AddressOf dgvData_RowEnter
        AddHandler dgv.RowLeave, AddressOf dgvData_RowLeave
    End Sub
    ''' <summary>
    ''' データグリッドビュー行選択
    ''' </summary>
    Private Sub dgvData_RowEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' 対象のdgv

        dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = g_ColorSelection      '行の背景を変更
    End Sub
    ''' <summary>
    ''' データグリッドビュー行非選択
    ''' </summary>
    Private Sub dgvData_RowLeave(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' 対象のdgv

        dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty           '行の背景をデフォルト色に戻す
    End Sub

    ''' <summary>
    ''' グリッドデータエラー処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Sub dgvData_DataError(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewDataErrorEventArgs)
        ' データグリッドのデータエラー時に呼ばれる
        ' 処理がなくとも消さないで！！
        MsgBox("入力文字列の形式が正しくありません")
        'e.Cancel = False
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 検査表面状態・検査パターン 取り込み関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 検査表面状態 を DBより取得し、コンボボックスにセット
    ''' </summary>
    ''' <param name="cmb">コンボボックス コントロール</param>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Public Function SetComboBox_Scode(ByVal cmb As ComboBox) As Boolean
        Dim strSQL As String = ""       ' SQL文生成
        strSQL = "SELECT scode, 検査表面状態名称, 有効判定"
        strSQL &= " FROM " & DB_SCODE_MASTER
        strSQL &= " ORDER BY scode ASC"

        ' SQL実行 & コンボボックスにセット
        Return SetComboBox(cmb, strSQL)
    End Function

    ''' <summary>
    ''' 検査パターン を DBより取得し、コンボボックスにセット
    ''' </summary>
    ''' <param name="cmb">コンボボックス コントロール</param>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Public Function SetComboBox_Pcode(ByVal cmb As ComboBox) As Boolean
        Dim strSQL As String = ""       ' SQL文生成
        strSQL = "SELECT pcode, 検査パターン名称, 有効判定"
        strSQL &= " FROM " & DB_PCODE_MASTER
        strSQL &= " ORDER BY pcode ASC"

        ' SQL実行 & コンボボックスにセット
        Return SetComboBox(cmb, strSQL)
    End Function

    ''' <summary>
    ''' 検査パターン を DBより取得し、コンボボックスにセット(PCODE数値対応版)
    ''' </summary>
    ''' <param name="cmb">コンボボックス コントロール</param>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Public Function SetComboBox_Pcode_Ascii(ByVal cmb As ComboBox) As Boolean
        Dim strSQL As String = ""       ' SQL文生成
        strSQL = "SELECT ascii(pcode) as pcode, 検査パターン名称, 有効判定"
        strSQL &= " FROM " & DB_PCODE_MASTER
        strSQL &= " ORDER BY pcode ASC"

        ' SQL実行 & コンボボックスにセット
        Return SetComboBox(cmb, strSQL)
    End Function

    ''' <summary>
    ''' DBより取得し、コンボボックスにセット
    ''' </summary>
    ''' <param name="cmb">コンボボックス コントロール</param>
    ''' <param name="strSQL">SQL分</param>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function SetComboBox(ByVal cmb As ComboBox, ByVal strSQL As String) As Boolean
        Dim tcls_DB As New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing

        '' コンボボックス 初期化
        cmb.Items.Clear()

        Try
            ' SQL実行
            If Not tcls_DB.Open() Then Return False
            sqlRead = tcls_DB.SelectExecute(strSQL)
            ' データ取得
            If Not sqlRead Is Nothing Then
                While sqlRead.Read
                    '' コンボボックス にアイテム追加 (string int 両方あるのでGetValueにしている)
                    cmb.Items.Add(New cls_CmbItem(CStr(sqlRead.GetValue(0)), CStr(sqlRead.GetValue(1)), CInt(sqlRead.GetValue(2))))
                End While
            End If
            Return True
        Catch ex As Exception
            Return False
        Finally
            If Not sqlRead Is Nothing Then sqlRead.Close()
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
    End Function

    ''' <summary>
    ''' コンボボックス選択時に 紐付く名称をセット
    ''' </summary>
    ''' <param name="index">コンボボックスの選択位置</param>
    ''' <param name="cmb">コンボボックス</param>
    ''' <param name="lbl">名称表示ラベル</param>
    ''' <remarks></remarks>
    Public Sub SelectComboBoxItem(ByVal index As Integer, ByVal cmb As ComboBox, ByVal lbl As Label)
        Dim item As cls_CmbItem    'コンボボックスのアイテム
        item = CType(cmb.Items(index), cls_CmbItem)

        ' 名称をセット
        lbl.Text = item.name
        '有効か無効化で背景色変更
        lbl.BackColor = CType(IIf(item.num = 0, Color.LightGray, Color.PeachPuff), Color)
    End Sub

    ''' <summary>
    ''' コンボボックスの選択されいている主キーを取得する
    ''' </summary>
    ''' <param name="index">配列位置</param>
    ''' <param name="cmb">コンボボックス</param>
    ''' <returns>主キー</returns>
    ''' <remarks></remarks>
    Public Function SelectComboBoxKey(ByVal index As Integer, ByVal cmb As ComboBox) As String
        Dim item As cls_CmbItem    'コンボボックスのアイテム
        item = CType(cmb.Items(index), cls_CmbItem)
        Return item.id
    End Function

    ''' <summary>
    ''' 主キーからコンボボックスの配列位置を取得する
    ''' </summary>
    ''' <param name="key">主キー</param>
    ''' <param name="cmb">コンボボックス</param>
    ''' <returns>配列位置</returns>
    ''' <remarks></remarks>
    Public Function SelectComboBoxIndex(ByVal key As String, ByVal cmb As ComboBox) As Integer
        Dim item As cls_CmbItem    'コンボボックスのアイテム
        For ii As Integer = 0 To cmb.Items.Count - 1
            item = CType(cmb.Items(ii), cls_CmbItem)
            If item.id = key Then Return ii
        Next ii
        Return -1
    End Function

    ''' <summary>
    ''' コンボボックスの任意データを削除
    ''' </summary>
    ''' <param name="cmb">コンボボックス</param>
    ''' <param name="strDelItem">削除したい任意のデータ</param>
    ''' <remarks></remarks>
    Public Sub Delete_CodeEtc(ByVal cmb As ComboBox, ByVal strDelItem As String)
        ' アイテム数ループ
        For ii As Integer = 0 To cmb.Items.Count - 1
            ' アイテムがZなら
            If strDelItem = cmb.Items.Item(ii).ToString Then
                ' 削除
                cmb.Items.RemoveAt(ii)
                Exit For
            End If
        Next
    End Sub



    ''' <summary>
    ''' データの取得
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Protected Overridable Function DB_Select() As Boolean
        Try
            For Each Data As DataSetHolder In m_DataSets
                Dim strErrMsg As String = ""
                If 0 > Get_SelectData(Create_Sql_Select(Data), Data.dsMain, Data.adapter, strErrMsg) Then
                    Return False
                End If
            Next
            Return True
        Catch ex As Exception
            Return False
        End Try
    End Function

    ''' <summary>
    ''' 入力値のチェック
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Protected Overridable Function Data_Check() As Boolean
        Try
            For Each Data As DataSetHolder In m_DataSets
                If Data.dgvCnt.dgvType = EM_DATAGRIDVIEW_DISPTYPE.emRowType Then
                    If Not Data_Check_RowHeader(Data) Then Return False
                Else
                    If Not Data_Check_ColHeader(Data) Then Return False
                End If
            Next
            Return True
        Catch ex As Exception
            Return False
        End Try
    End Function

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Protected Overridable Function Data_Get() As Boolean
        Dim strErrMsg As String = ""
        For Each Data As DataSetHolder In m_DataSets
            If Data.dgvCnt.dgvType = EM_DATAGRIDVIEW_DISPTYPE.emRowType Then
                If Not Data_Get_RowHeader(Data, strErrMsg) Then

                End If
            Else
                If Not Data_Get_ColHeader(Data, strErrMsg) Then

                End If
            End If
        Next
        Return True
    End Function

    ''' <summary>
    ''' 表示設定
    ''' </summary>
    ''' <remarks></remarks>
    Public Overridable Sub dgvInitialize()
        For Each Data As DataSetHolder In m_DataSets
            If Data.dgvCnt.dgvType = EM_DATAGRIDVIEW_DISPTYPE.emRowType Then
                dgvInitialize_RowHeader(Data.dgvCnt.dgv)
            Else
                Dim cmbColumns As New Generic.SortedList(Of String, DataGridViewComboBoxColumn)
                dgvInitialize_ColHeader(Data.dgvCnt.dgv, True)
                MakeComb_ColHeader(Data, cmbColumns)
                setGridCellInfo4ColHeader(Data, cmbColumns, DataGridViewColumnSortMode.NotSortable)
            End If
            'グリッドデータエラー処理
            AddHandler Data.dgvCnt.dgv.DataError, AddressOf dgvData_DataError
        Next
    End Sub

    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <param name="Key">検査表面状態(scode)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Overridable Function Data_Disp(Optional ByVal Key As String = "") As Boolean
        For Each Data As DataSetHolder In m_DataSets
            If Data.dgvCnt.dgvType = EM_DATAGRIDVIEW_DISPTYPE.emRowType Then
                'グリッド書式設定・データ表示処理（行ヘッダタイプ用）
                GridSet(Key, Data)
                'データグリッドビュー幅設定
                dgvWidthSet_RowHeader(Data.dgvCnt.dgv, Data.dgvCnt.wSize.nNo, Data.dgvCnt.wSize.nName, Data.dgvCnt.wSize.nValue, Data.dgvCnt.wSize.nRange)
            Else
                'データ表示処理（列ヘッダタイプ用）
                Set_DataSource(Key, Data)
            End If
            '行の背景をデフォルト色に戻す
            SetBackColorEmpty(Data.dgvCnt.dgv)
        Next

        Return True
    End Function

    ''' <summary>
    ''' データソースセット
    ''' </summary>
    ''' <param name="key">検査表面状態(scode)</param>
    ''' <param name="Data"></param>
    ''' <remarks></remarks>
    Private Sub Set_DataSource(ByVal key As String, ByVal Data As DataSetHolder)
        If Not CopyTargetTable(Data) Then
            Data.dgvCnt.dgv.DataSource = New DataView(Data.dsMain.Tables(0))
        Else
            Data.dgvCnt.dgv.DataSource = New DataView(Data.dsMain.Tables(0), "scode=" & key, "scode", DataViewRowState.CurrentRows)
        End If
    End Sub

    ''' <summary>
    ''' データグリッドビューより画面情報を取得
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function Get_GridInfo(ByVal dgv As DataGridView) As DataSetHolder
        Dim Data As DataSetHolder = Nothing
        For Each dtWk As DataSetHolder In m_DataSets
            If dtWk.dgvCnt.dgv.Equals(dgv) Then
                Data = dtWk
                Exit For
            End If
        Next
        Return Data
    End Function

    ''' <summary>
    ''' 初回ローディング処理(検査表面状態(scode)無し)
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub InitLoading()
        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        dgvInitialize()

        ' データの取得
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
            Exit Sub
        End If

        ' 画面に値を表示
        If Not Data_Disp() Then
            ' 画面表示失敗なので、強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DBにデータが無い為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If

        For Each Data As DataSetHolder In m_DataSets
            Data.dgvCnt.dgv.DataGridViewSelectionRowColorChange(g_ColorSelection)
            DataGridViewColumnWidthChanged(Data.dgvCnt.dgv)
        Next

        '=======================================================
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
    ''' 画面構成情報をDBから取得(自動のみ)
    ''' </summary>
    ''' <param name="cID">画面ID</param>
    ''' <param name="dgv">データグリッドビュー情報</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function InitGridSetting(ByVal cID As String, ByVal ParamArray dgv() As DgvControlInfo) As Boolean
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""
        Dim dtst As System.Data.DataSet = New System.Data.DataSet
        Dim sqlAdp As System.Data.SqlClient.SqlDataAdapter = Nothing
        Dim dsTmp As New DataSet


        m_DataSets.Clear()
        Try
            strSQL = "SELECT * FROM " & DB_PAR_GRID_SETTING & " WHERE 画面ID = '" & cID & "'"

            'SQL文実行
            If 0 > Get_SelectData(strSQL, dtst, sqlAdp, strErrMsg) Then
                Return False
            End If
            sqlAdp.Fill(dsTmp)
            For ii As Integer = 1 To 100
                Dim rows() As DataRow = dsTmp.Tables(0).Select(String.Format("グリッドID = '{0}'", ii))

                strSQL = "SELECT * FROM " & DB_PAR_GRID_SETTING & " WHERE 画面ID = '" & cID & "' and  グリッドID = " & ii
                Dim dtstWK As System.Data.DataSet = New System.Data.DataSet
                Dim sqlAdpWK As System.Data.SqlClient.SqlDataAdapter = Nothing
                If 0 > Get_SelectData(strSQL, dtstWK, sqlAdpWK, strErrMsg) Then
                    Return False
                End If

                'If rows Is Nothing Then Exit For
                If rows.Length = 0 Then Exit For
                m_DataSets.Add(New DataSetHolder(rows, dtstWK, sqlAdpWK, dgv(ii - 1)))
            Next ii

        Catch ex As Exception
            Return False
        End Try
        Return True
    End Function


    ''' <summary>
    ''' 確定処理
    ''' </summary>
    ''' <remarks></remarks>
    Public Function subData_UpDate() As Boolean

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' 入力値チェック
        If Not Data_Check() Then
            Return False
        End If

        Data_Get()

        Try
            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then
                Return False
            End If

            ' ホントに登録        
            If DB_UpDate() Then
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("正常に登録しました", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then
                    Return False
                End If
            Else
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("登録に失敗", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
                Return False
            End If
            Return True

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("確定処理 異常[{0}]", ex.Message))
            Return False
        Finally
            If frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

    End Function


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ SQL関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' データ取得
    ''' </summary>
    ''' <param name="strSQL">発行するSQL文</param>
    ''' <param name="DsDataSet">データセットオブジェクト</param>
    ''' <param name="AdapterData">データアダプタオブジェクト</param>
    ''' <param name="strErrMsg">エラーメッセージ</param>
    ''' <returns>影響を受けた行数を返す　-1:失敗　-9:DB接続失敗</returns>
    ''' <remarks></remarks>
    Public Function Get_SelectData(ByVal strSQL As String, ByRef DsDataSet As System.Data.DataSet, _
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






    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 画面構成情報をDBから取得(マニュアル用)
    ''' </summary>
    ''' <param name="cID">画面ID</param>
    ''' <param name="nNo">グリッド番号</param>
    ''' <param name="gdtGridSetting">画面構成情報(OUT)</param>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Public Function gblnGetGridSetting(ByVal cID As String, ByVal nNo As Integer, ByRef gdtGridSetting As DataTable) As Boolean
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""
        Dim dtst As System.Data.DataSet = New System.Data.DataSet
        Dim sqlAdp As System.Data.SqlClient.SqlDataAdapter = Nothing

        Try
            strSQL = "SELECT * FROM " & DB_PAR_GRID_SETTING & " WHERE 画面ID = '" & cID & "' and  グリッドID = " & nNo

            'SQL文実行
            If 0 > Get_SelectData(strSQL, dtst, sqlAdp, strErrMsg) Then
                Return False
            End If
            sqlAdp.Fill(gdtGridSetting)
        Catch ex As Exception
            Return False
        End Try
        Return True
    End Function


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' dgv初期化（行ヘッダタイプ）
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize_RowHeader(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)

        '''' 基本を設定
        'Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                     ' 選択色無し

        '''' 独自の項目を設定
        With dgv
            .AllowUserToAddRows = False                                     ' ユーザー 行追加
            .AllowUserToDeleteRows = False                                  ' ユーザー 行削除
            .AllowUserToOrderColumns = False                                ' ユーザー 列移動
            .AllowUserToResizeColumns = False                               ' ユーザー 列サイズ変更
            .AllowUserToResizeRows = False                                  ' ユーザー 行サイズ変更
            .ShowCellErrors = False                                         ' セルエラーの表示
            .ShowCellToolTips = False                                       ' ツールヒントの表示
            .ShowEditingIcon = False                                        ' 編集中のセルの明示
            .ShowRowErrors = False                                          ' エラー行の明示
            .ScrollBars = ScrollBars.Both                                   ' スクロールバーの表示

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

            ' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' 選択セル色定義
            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .DefaultCellStyle().SelectionBackColor = dgv.DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter

        End With


    End Sub

    ''' <summary>
    ''' データグリッドビュー幅設定（行ヘッダタイプ）
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <param name="n1">No  ：列幅[pixel] ※基本0</param>
    ''' <param name="n2">名称：列幅[pixel]</param>
    ''' <param name="n3">値  ：列幅[pixel]</param>
    ''' <param name="n4">備考：列幅[pixel]</param>
    ''' <remarks></remarks>
    Private Sub dgvWidthSet_RowHeader(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal n1 As Integer, ByVal n2 As Integer, ByVal n3 As Integer, ByVal n4 As Integer)
        With dgv
            .Columns(EM_ROW_VERTION_ID.emNo).Width = n1
            .Columns(EM_ROW_VERTION_ID.emName).Width = n2
            .Columns(EM_ROW_VERTION_ID.emValue).Width = n3
            .Columns(EM_ROW_VERTION_ID.emRange).Width = n4
        End With
    End Sub

    ''' <summary>
    ''' dgv初期化（列ヘッダタイプ）
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <param name="bResizeColumns">列サイズ変更有無(True:列サイズ変更可)</param>
    ''' <remarks></remarks>
    Public Sub dgvInitialize_ColHeader(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView, Optional ByVal bResizeColumns As Boolean = False)

        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        With dgv
            .RowCount = 0

            .AllowUserToResizeColumns = bResizeColumns                      ' ユーザー 列サイズ変更
            .ScrollBars = ScrollBars.Both                                   ' スクロールバーの表示

            ' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' 列定義
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 42                                       ' ヘッダーの高さ

        End With

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView セル情報を設定
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' セル情報を設定します（行ヘッダタイプ）。
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <param name="comboboxCells">コンボボックスセル情報</param>
    ''' <remarks></remarks>
    Private Sub setGridCellInfo4RowHeader(ByVal gdtGridSetting As DataSetHolder, _
                ByRef comboboxCells As Generic.SortedList(Of String, DataGridViewComboBoxCell))

        With gdtGridSetting.dgvCnt.dgv

            .Columns(EM_ROW_VERTION_ID.emNo).Visible = False

            .Columns(EM_ROW_VERTION_ID.emNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_ROW_VERTION_ID.emName).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_ROW_VERTION_ID.emValue).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_ROW_VERTION_ID.emRange).SortMode = DataGridViewColumnSortMode.NotSortable

            For Each gridSetRow As DataRow In gdtGridSetting.Rows

                Dim rowId As Integer = CInt(gridSetRow.Item("項目No")) - 1
                'Alignment設定
                Dim tmpAlignment As DataGridViewContentAlignment = CType(gridSetRow.Item("表示_項目配置"), DataGridViewContentAlignment)
                If tmpAlignment = DataGridViewContentAlignment.NotSet Then
                    .Rows(rowId).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                Else
                    .Rows(rowId).DefaultCellStyle.Alignment = tmpAlignment
                End If
                .Rows(rowId).Cells(EM_ROW_VERTION_ID.emName).Style.Alignment = DataGridViewContentAlignment.MiddleLeft
                .Rows(rowId).Cells(EM_ROW_VERTION_ID.emRange).Style.Alignment = DataGridViewContentAlignment.MiddleLeft

                '表示非表示設定()
                '.Rows(rowId).Visible = CBool(gridSetRow.Item("表示_項目表示有無"))
                If CBool(gridSetRow.Item("表示_項目表示有無")) = False Then
                    If CBool(gridSetRow.Item("DB_主キー")) Then
                        .Rows(rowId).Height = 0
                    Else
                        .Rows(rowId).Visible = False
                    End If
                End If

                .Rows(rowId).ReadOnly = True

                '列単位に設定が必要な部分を設定
                '一旦セルをクリア
                .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue).Dispose()
                'グリッドセル設定
                Select Case CType(gridSetRow.Item("項目セルタイプ"), EM_DATAGRIDVIEW_CELLTYPE)
                    Case EM_DATAGRIDVIEW_CELLTYPE.emTextBoxCell
                        .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue) = New DataGridViewTextBoxCell
                    Case EM_DATAGRIDVIEW_CELLTYPE.emComboBoxCell
                        Dim tmpStr As String = gridSetRow.Item("DB_フィールド名").ToString().ToUpper()

                        .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue) = New DataGridViewComboBoxCell
                        For Each key As String In comboboxCells.Keys
                            If key.ToUpper = tmpStr Then
                                .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue) = DirectCast(comboboxCells.Item(key).Clone(), DataGridViewCell)
                                Exit For
                            End If
                        Next key

                    Case EM_DATAGRIDVIEW_CELLTYPE.emCheckBoxCell
                        .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue) = New DataGridViewCheckBoxCell
                    Case EM_DATAGRIDVIEW_CELLTYPE.emButtonCell
                        .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue) = New DataGridViewButtonCell

                End Select
                .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue).ReadOnly = CBool(gridSetRow.Item("表示_読み取り専用"))
                If CBool(gridSetRow.Item("表示_読み取り専用")) Then
                    .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue).Style.ForeColor = Color.DimGray
                Else
                    .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue).Style.ForeColor = Color.Black
                End If


                ' データグリッドダブルクリック時に表示するダイアログタイプ
                If EM_DLG_TYPE.emFileDialog = CInt(gridSetRow.Item("入力_表示ダイアログ")) Then
                    .Rows(rowId).Cells(EM_ROW_VERTION_ID.emRange).Dispose()
                    .Rows(rowId).Cells(EM_ROW_VERTION_ID.emRange) = New DataGridViewButtonCell
                    .Rows(rowId).Cells(EM_ROW_VERTION_ID.emRange).Style.BackColor = Color.FromName("Control")
                    .Rows(rowId).Cells(EM_ROW_VERTION_ID.emRange).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                    .Rows(rowId).Cells(EM_ROW_VERTION_ID.emRange).Style.Alignment = DataGridViewContentAlignment.MiddleCenter
                    '.Rows(rowId).Cells(EM_ROW_VERTION_ID.emRange).UseColumnTextForButtonValue = True
                    .Rows(rowId).Cells(EM_ROW_VERTION_ID.emRange).Value = "参 照"
                End If

                'PK項目の列固定
                .Rows(rowId).Frozen = CBool(gridSetRow.Item("表示_列固定"))

                '表示フォーマット
                If "" <> CStr(gridSetRow.Item("表示_フォーマット")) Then
                    .Rows(rowId).DefaultCellStyle.Format = CStr(gridSetRow.Item("表示_フォーマット"))
                End If

            Next gridSetRow

        End With
    End Sub

    ''' <summary>
    ''' セル情報を設定します（列ヘッダタイプ）。
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <param name="comboboxCells">コンボボックスセル情報</param>
    ''' <param name="sortMode">ソートモードの設定</param>
    ''' <remarks></remarks>
    Protected Sub setGridCellInfo4ColHeader(ByVal gdtGridSetting As DataSetHolder, _
                ByRef comboboxCells As Generic.SortedList(Of String, DataGridViewComboBoxColumn), _
                Optional ByVal sortMode As DataGridViewColumnSortMode = DataGridViewColumnSortMode.Automatic)

        With gdtGridSetting.dgvCnt.dgv
            For Each gridSetRow As DataRow In gdtGridSetting.Rows
                Dim rowId As Integer = CInt(gridSetRow.Item("項目No")) - 1

                ' 不足している列数を追加
                While (True)
                    If .Columns.Count - 1 < rowId Then
                        'gridView.Columns.Add("", "")
                        Select Case CType(gridSetRow.Item("項目セルタイプ"), EM_DATAGRIDVIEW_CELLTYPE)
                            Case EM_DATAGRIDVIEW_CELLTYPE.emTextBoxCell
                                .Columns.Add(New DataGridViewTextBoxColumn)
                            Case EM_DATAGRIDVIEW_CELLTYPE.emComboBoxCell
                                'gridView.Columns.Add(New DataGridViewComboBoxColumn)
                                Dim comboColumn As DataGridViewComboBoxColumn = CType(comboboxCells(CStr(gridSetRow.Item("DB_フィールド名"))).Clone(), DataGridViewComboBoxColumn)
                                .Columns.Add(comboColumn)
                            Case EM_DATAGRIDVIEW_CELLTYPE.emCheckBoxCell
                                .Columns.Add(New DataGridViewCheckBoxColumn)
                            Case EM_DATAGRIDVIEW_CELLTYPE.emButtonCell
                                .Columns.Add(New DataGridViewButtonColumn)
                        End Select

                    Else
                        Exit While
                    End If
                End While

                '表示非表示設定
                .Columns(rowId).Visible = CBool(gridSetRow.Item("表示_項目表示有無"))

                '表示幅設定
                .Columns(rowId).Width = CInt(gridSetRow.Item("表示_項目幅"))

                'データプロパティ名設定
                .Columns(rowId).DataPropertyName = CStr(gridSetRow.Item("DB_フィールド名"))

                'ヘッダタイトルの設定
                Dim headTitle As New System.Text.StringBuilder
                headTitle.Append(gridSetRow.Item("項目ヘッダー1"))
                If Not isNullOrEmpty(gridSetRow.Item("項目ヘッダー2")) Then
                    headTitle.Append(vbCr)
                    headTitle.Append(gridSetRow.Item("項目ヘッダー2"))
                End If
                .Columns(rowId).HeaderText = headTitle.ToString

                'Alignment設定
                Dim tmpAlignment As DataGridViewContentAlignment = CType(gridSetRow.Item("表示_項目配置"), DataGridViewContentAlignment)
                If tmpAlignment = DataGridViewContentAlignment.NotSet Then
                    .Columns(rowId).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                Else
                    .Columns(rowId).DefaultCellStyle.Alignment = tmpAlignment
                End If

                'ソート情報の設定
                If sortMode <> DataGridViewColumnSortMode.Automatic Then
                    .Columns(rowId).SortMode = sortMode
                End If

                .Columns(rowId).ReadOnly = CBool(gridSetRow.Item("表示_読み取り専用"))
                If CBool(gridSetRow.Item("表示_読み取り専用")) = True Then
                    .Columns(rowId).DefaultCellStyle.ForeColor = Color.DimGray
                Else
                    .Columns(rowId).DefaultCellStyle.ForeColor = Color.Black
                End If

                'PK項目の列固定
                .Columns(rowId).Frozen = CBool(gridSetRow.Item("表示_列固定"))
                '表示フォーマット
                If "" <> CStr(gridSetRow.Item("表示_フォーマット")) Then
                    .Columns(rowId).DefaultCellStyle.Format = CStr(gridSetRow.Item("表示_フォーマット"))
                End If

            Next gridSetRow

            'If .ColumnCount > gdtGridSetting.Rows.Length Then
            '    For ii As Integer = gdtGridSetting.Rows.Length To .ColumnCount - 1
            '        .Columns(ii).Visible = False
            '    Next
            'End If

        End With
    End Sub

    ''' <summary>
    ''' セル情報を設定します（列ヘッダタイプ）[マニュアル用]
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <param name="comboboxCells">コンボボックスセル情報</param>
    ''' <param name="sortMode">ソートモードの設定</param>
    ''' <remarks></remarks>
    Protected Sub setGridCellInfo4ColHeader(ByVal gdtGridSetting As DataTable, ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, _
                ByRef comboboxCells As Generic.SortedList(Of String, DataGridViewComboBoxColumn), _
                Optional ByVal sortMode As DataGridViewColumnSortMode = DataGridViewColumnSortMode.Automatic)

        With dgv
            For Each gridSetRow As DataRow In gdtGridSetting.Rows
                Dim rowId As Integer = CInt(gridSetRow.Item("項目No")) - 1

                ' 不足している列数を追加
                While (True)
                    If .Columns.Count - 1 < rowId Then
                        'gridView.Columns.Add("", "")
                        Select Case CType(gridSetRow.Item("項目セルタイプ"), EM_DATAGRIDVIEW_CELLTYPE)
                            Case EM_DATAGRIDVIEW_CELLTYPE.emTextBoxCell
                                .Columns.Add(New DataGridViewTextBoxColumn)
                            Case EM_DATAGRIDVIEW_CELLTYPE.emComboBoxCell
                                'gridView.Columns.Add(New DataGridViewComboBoxColumn)
                                Dim comboColumn As DataGridViewComboBoxColumn = CType(comboboxCells(CStr(gridSetRow.Item("DB_フィールド名"))).Clone(), DataGridViewComboBoxColumn)
                                .Columns.Add(comboColumn)
                            Case EM_DATAGRIDVIEW_CELLTYPE.emCheckBoxCell
                                .Columns.Add(New DataGridViewCheckBoxColumn)
                            Case EM_DATAGRIDVIEW_CELLTYPE.emButtonCell
                                .Columns.Add(New DataGridViewButtonColumn)
                        End Select

                    Else
                        Exit While
                    End If
                End While

                '表示非表示設定
                .Columns(rowId).Visible = CBool(gridSetRow.Item("表示_項目表示有無"))

                '表示幅設定
                .Columns(rowId).Width = CInt(gridSetRow.Item("表示_項目幅"))

                'データプロパティ名設定
                .Columns(rowId).DataPropertyName = CStr(gridSetRow.Item("DB_フィールド名"))

                'ヘッダタイトルの設定
                Dim headTitle As New System.Text.StringBuilder
                headTitle.Append(gridSetRow.Item("項目ヘッダー1"))
                If Not isNullOrEmpty(gridSetRow.Item("項目ヘッダー2")) Then
                    headTitle.Append(vbCr)
                    headTitle.Append(gridSetRow.Item("項目ヘッダー2"))
                End If
                .Columns(rowId).HeaderText = headTitle.ToString

                'Alignment設定
                Dim tmpAlignment As DataGridViewContentAlignment = CType(gridSetRow.Item("表示_項目配置"), DataGridViewContentAlignment)
                If tmpAlignment = DataGridViewContentAlignment.NotSet Then
                    .Columns(rowId).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                Else
                    .Columns(rowId).DefaultCellStyle.Alignment = tmpAlignment
                End If

                'ソート情報の設定
                If sortMode <> DataGridViewColumnSortMode.Automatic Then
                    .Columns(rowId).SortMode = sortMode
                End If

                .Columns(rowId).ReadOnly = CBool(gridSetRow.Item("表示_読み取り専用"))
                If CBool(gridSetRow.Item("表示_読み取り専用")) = True Then
                    .Columns(rowId).DefaultCellStyle.ForeColor = Color.DimGray
                Else
                    .Columns(rowId).DefaultCellStyle.ForeColor = Color.Black
                End If

                'PK項目の列固定
                .Columns(rowId).Frozen = CBool(gridSetRow.Item("表示_列固定"))
                '表示フォーマット
                If "" <> CStr(gridSetRow.Item("表示_フォーマット")) Then
                    .Columns(rowId).DefaultCellStyle.Format = CStr(gridSetRow.Item("表示_フォーマット"))
                End If

            Next gridSetRow

            'If .ColumnCount > gdtGridSetting.Rows.Length Then
            '    For ii As Integer = gdtGridSetting.Rows.Length To .ColumnCount - 1
            '        .Columns(ii).Visible = False
            '    Next
            'End If

        End With
    End Sub

    ''' <summary>
    ''' グリッド書式設定・データ表示処理（行ヘッダタイプ用）
    ''' </summary>
    ''' <param name="key">主キーの値(scode)</param>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <remarks></remarks>
    Private Sub GridSet(ByVal key As String, ByVal gdtGridSetting As DataSetHolder)
        Dim dtRow As DataRow
        Dim dttbl As DataTable

        ' DGV用DataTable作成
        Dim dc0 As New DataColumn
        Dim dc1 As New DataColumn
        Dim dc2 As New DataColumn
        Dim dc3 As New DataColumn
        dc0.ColumnName = "No"
        dc0.DataType = GetType(Integer)
        dc1.ColumnName = "名称"
        dc1.DataType = GetType(Object)
        dc2.ColumnName = "値"
        dc2.DataType = GetType(Object)
        dc3.ColumnName = "備考"
        dc3.DataType = GetType(Object)
        dttbl = New DataTable
        dttbl.Columns.AddRange(New System.Data.DataColumn() {dc0, dc1, dc2, dc3})
        gdtGridSetting.dgvCnt.dgv.DataSource = dttbl

        'コントロールのレイアウト ロジックを一時的に中断
        gdtGridSetting.dgvCnt.dgv.SuspendLayout()

        ' 一括のテーブルなので、検査パターン毎の値のみ取り出し
        Dim strWhere As String = ""                                  ' WHERE句に相当の条件生成
        If CopyTargetTable(gdtGridSetting) Then
            strWhere = String.Format("scode='{0}'", key)                                  ' WHERE句に相当の条件生成
        End If
        Dim drowScode() As DataRow = gdtGridSetting.dsMain.Tables(0).Select(strWhere)

        ' テーブルの行と列を入れ替え
        dttbl.Rows.Clear()
        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            dtRow = dttbl.NewRow()
            dtRow.Item(EM_ROW_VERTION_ID.emNo) = gridSetRow.Item("項目No")
            Dim strName As String = ""
            If Not gridSetRow.Item("項目ヘッダー1") Is DBNull.Value Then
                strName &= CStr(gridSetRow.Item("項目ヘッダー1"))
            End If
            If Not gridSetRow.Item("項目ヘッダー2") Is DBNull.Value Then
                strName &= CStr(gridSetRow.Item("項目ヘッダー2"))
            End If
            dtRow.Item(EM_ROW_VERTION_ID.emName) = strName.ToString

            If (drowScode(0).Table.Columns.Count > 0) Then
                dtRow.Item(EM_ROW_VERTION_ID.emValue) = drowScode(0).Item(gridSetRow.Item("DB_フィールド名").ToString)
            End If

            dtRow.Item(EM_ROW_VERTION_ID.emRange) = gridSetRow.Item("備考").ToString
            dttbl.Rows.Add(dtRow)
        Next

        Dim cmbCells As New Generic.SortedList(Of String, DataGridViewComboBoxCell)
        MakeComb_RowHeader(gdtGridSetting, cmbCells)

        'セル情報を設定します
        gdtGridSetting.dgvCnt.dgv.CurrentCell = Nothing
        setGridCellInfo4RowHeader(gdtGridSetting, cmbCells)

        '通常のレイアウト ロジックを再開
        gdtGridSetting.dgvCnt.dgv.ResumeLayout()

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView データ入力チェック
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' データ入力チェック（行ヘッダタイプ）
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Public Function Data_Check_RowHeader(ByVal gdtGridSetting As DataSetHolder) As Boolean
        Dim strWk As String


        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            '読み取り専用か？
            If CBool(gridSetRow.Item("表示_読み取り専用")) Then Continue For
            '入力タイプ以外か？
            If EM_DATAGRIDVIEW_CELLTYPE.emTextBoxCell <> CInt(gridSetRow.Item("項目セルタイプ")) Then Continue For

            Dim nRowId As Integer = CInt(gridSetRow.Item("項目No")) - 1

            strWk = ""
            If Not gdtGridSetting.dgvCnt.dgv(EM_ROW_VERTION_ID.emValue, nRowId).Value Is DBNull.Value Then
                strWk = CStr(gdtGridSetting.dgvCnt.dgv(EM_ROW_VERTION_ID.emValue, nRowId).Value)
            End If

            Dim strMsg As String = ""
            If Not gridSetRow.Item("項目ヘッダー1") Is DBNull.Value Then
                strMsg &= CStr(gridSetRow.Item("項目ヘッダー1"))
            End If
            If Not gridSetRow.Item("項目ヘッダー2") Is DBNull.Value Then
                strMsg &= CStr(gridSetRow.Item("項目ヘッダー2"))
            End If

            Select Case CInt(gridSetRow.Item("DB_フィールドタイプ"))
                Case EM_DB_TYPE.emIntType      'Int
                    Dim nMax As Long = Integer.MaxValue
                    Dim nMin As Long = Integer.MinValue
                    If Not gridSetRow.Item("入力_入力可能最大値") Is DBNull.Value Then
                        nMax = CInt(gridSetRow.Item("入力_入力可能最大値"))
                    End If
                    If Not gridSetRow.Item("入力_入力可能最小値") Is DBNull.Value Then
                        nMin = CInt(gridSetRow.Item("入力_入力可能最小値"))
                    End If
                    Dim bNullOK As Boolean = CBool(gridSetRow.Item("入力_NULL入力可否"))
                    If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, nMin, nMax, bNullOK, strMsg, True) <> 0 Then Return False
                    gdtGridSetting.dgvCnt.dgv(EM_ROW_VERTION_ID.emValue, nRowId).Value = CInt(strWk)
                Case EM_DB_TYPE.emFloatType      'float
                    Dim dMax As Double = Double.MaxValue
                    Dim dMin As Double = Double.MinValue
                    If Not gridSetRow.Item("入力_入力可能最大値") Is DBNull.Value Then
                        dMax = CDbl(gridSetRow.Item("入力_入力可能最大値"))
                    End If
                    If Not gridSetRow.Item("入力_入力可能最小値") Is DBNull.Value Then
                        dMin = CDbl(gridSetRow.Item("入力_入力可能最小値"))
                    End If
                    Dim bNullOK As Boolean = CBool(gridSetRow.Item("入力_NULL入力可否"))
                    Dim nDecimalLen As Integer = 0
                    If Not gridSetRow.Item("入力_小数点以下桁数") Is DBNull.Value Then
                        nDecimalLen = CInt(gridSetRow.Item("入力_小数点以下桁数"))
                    End If
                    If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, dMin, dMax, bNullOK, nDecimalLen, strMsg, True) <> 0 Then Return False
                    gdtGridSetting.dgvCnt.dgv(EM_ROW_VERTION_ID.emValue, nRowId).Value = CDbl(strWk)
                Case EM_DB_TYPE.emStrType      'char
                    Dim nMaxLen As Integer = Integer.MaxValue
                    Dim nMinLen As Integer = 0
                    If Not gridSetRow.Item("入力_入力可能桁数") Is DBNull.Value Then
                        nMaxLen = CInt(gridSetRow.Item("入力_入力可能桁数"))
                    End If
                    If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, nMinLen, nMaxLen, strMsg, True) <> 0 Then Return False
                    gdtGridSetting.dgvCnt.dgv(EM_ROW_VERTION_ID.emValue, nRowId).Value = strWk
            End Select
        Next

        Return True

    End Function

    ''' <summary>
    ''' データ入力チェック（列ヘッダタイプ）
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Public Function Data_Check_ColHeader(ByVal gdtGridSetting As DataSetHolder) As Boolean

        Dim strWk As String = ""
        For ii As Integer = 0 To gdtGridSetting.dgvCnt.dgv.Rows.Count - 1

            For Each gridSetRow As DataRow In gdtGridSetting.Rows
                '読み取り専用か？
                If CBool(gridSetRow.Item("表示_読み取り専用")) Then Continue For
                '入力タイプ以外か？
                If EM_DATAGRIDVIEW_CELLTYPE.emTextBoxCell <> CInt(gridSetRow.Item("項目セルタイプ")) Then Continue For

                Dim nColId As Integer = CInt(gridSetRow.Item("項目No")) - 1

                strWk = ""
                If Not gdtGridSetting.dgvCnt.dgv(nColId, ii).Value Is DBNull.Value Then
                    strWk = CStr(gdtGridSetting.dgvCnt.dgv(nColId, ii).Value)
                End If

                Dim strMsg As String = ""
                If Not gridSetRow.Item("項目ヘッダー1") Is DBNull.Value Then
                    strMsg &= CStr(gridSetRow.Item("項目ヘッダー1"))
                End If
                If Not gridSetRow.Item("項目ヘッダー2") Is DBNull.Value Then
                    strMsg &= CStr(gridSetRow.Item("項目ヘッダー2"))
                End If

                Select Case CInt(gridSetRow.Item("DB_フィールドタイプ"))
                    Case EM_DB_TYPE.emIntType      'Int
                        Dim nMax As Long = Integer.MaxValue
                        Dim nMin As Long = Integer.MinValue
                        If Not gridSetRow.Item("入力_入力可能最大値") Is DBNull.Value Then
                            nMax = CInt(gridSetRow.Item("入力_入力可能最大値"))
                        End If
                        If Not gridSetRow.Item("入力_入力可能最小値") Is DBNull.Value Then
                            nMin = CInt(gridSetRow.Item("入力_入力可能最小値"))
                        End If
                        Dim bNullOK As Boolean = CBool(gridSetRow.Item("入力_NULL入力可否"))
                        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, nMin, nMax, bNullOK, strMsg, True) <> 0 Then Return False
                        gdtGridSetting.dgvCnt.dgv(nColId, ii).Value = CInt(strWk)
                    Case EM_DB_TYPE.emFloatType      'float
                        Dim dMax As Double = Double.MaxValue
                        Dim dMin As Double = Double.MinValue
                        If Not gridSetRow.Item("入力_入力可能最大値") Is DBNull.Value Then
                            dMax = CDbl(gridSetRow.Item("入力_入力可能最大値"))
                        End If
                        If Not gridSetRow.Item("入力_入力可能最小値") Is DBNull.Value Then
                            dMin = CDbl(gridSetRow.Item("入力_入力可能最小値"))
                        End If
                        Dim bNullOK As Boolean = CBool(gridSetRow.Item("入力_NULL入力可否"))
                        Dim nDecimalLen As Integer = 0
                        If Not gridSetRow.Item("入力_小数点以下桁数") Is DBNull.Value Then
                            nDecimalLen = CInt(gridSetRow.Item("入力_小数点以下桁数"))
                        End If
                        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, dMin, dMax, bNullOK, nDecimalLen, strMsg, True) <> 0 Then Return False
                        gdtGridSetting.dgvCnt.dgv(nColId, ii).Value = CDbl(strWk)
                    Case EM_DB_TYPE.emStrType      'char
                        Dim nMaxLen As Integer = Integer.MaxValue
                        Dim nMinLen As Integer = 0
                        If Not gridSetRow.Item("入力_入力可能桁数") Is DBNull.Value Then
                            nMaxLen = CInt(gridSetRow.Item("入力_入力可能桁数"))
                        End If
                        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, nMinLen, nMaxLen, strMsg, True) <> 0 Then Return False
                        gdtGridSetting.dgvCnt.dgv(nColId, ii).Value = strWk
                End Select
            Next
        Next ii

        Return True

    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView コンボボックス編集
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コンボボックス編集（行ヘッダタイプ）
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <param name="comboboxCells">対象コンボボックス(OUT)</param>
    ''' <remarks></remarks>
    Public Sub MakeComb_RowHeader(ByVal gdtGridSetting As DataSetHolder, ByRef comboboxCells As Generic.SortedList(Of String, DataGridViewComboBoxCell))
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""
        Dim dtst As System.Data.DataSet = New System.Data.DataSet
        Dim dttbl As System.Data.DataTable = New System.Data.DataTable
        Dim sqlAdp As System.Data.SqlClient.SqlDataAdapter = Nothing

        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            Select Case CType(gridSetRow.Item("項目セルタイプ"), EM_DATAGRIDVIEW_CELLTYPE)
                Case EM_DATAGRIDVIEW_CELLTYPE.emComboBoxCell
                Case Else
                    Continue For
            End Select

            Dim nNo As Integer = CInt(gridSetRow.Item("Lov_id"))
            If nNo < 1 Then Continue For

            ' SQL文作成
            strSQL = "SELECT disp_member, val_member_i, val_member_c, val_member_b  FROM " & DB_PAR_LOV_ITEM & " WHERE (lov_id = " & nNo & ")  ORDER BY seq"
            ' データテーブルに取得
            'SQL文実行
            If 0 > Get_SelectData(strSQL, dtst, sqlAdp, strErrMsg) Then
                'gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LOV_ITEM & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            End If
            dttbl = New DataTable

            sqlAdp.Fill(dttbl)

            Dim cmbDt As New DataGridViewComboBoxCell()
            cmbDt.DataSource = dttbl
            cmbDt.DisplayMember = "disp_member"
            Select Case CInt(gridSetRow.Item("DB_フィールドタイプ"))
                Case EM_DB_TYPE.emStrType
                    cmbDt.ValueMember = "val_member_c"
                Case EM_DB_TYPE.emBitType
                    cmbDt.ValueMember = "val_member_b"
                Case Else
                    cmbDt.ValueMember = "val_member_i"
            End Select

            comboboxCells.Add(gridSetRow.Item("DB_フィールド名").ToString(), cmbDt)
        Next
    End Sub

    ''' <summary>
    ''' コンボボックス編集（列ヘッダタイプ）
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <param name="comboboxColumns">対象コンボボックス(OUT)</param>
    ''' <remarks></remarks>
    Public Sub MakeComb_ColHeader(ByVal gdtGridSetting As DataSetHolder, ByRef comboboxColumns As Generic.SortedList(Of String, DataGridViewComboBoxColumn))
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""
        Dim dtst As System.Data.DataSet = New System.Data.DataSet
        Dim dttbl As System.Data.DataTable = New System.Data.DataTable
        Dim sqlAdp As System.Data.SqlClient.SqlDataAdapter = Nothing

        comboboxColumns.Clear()

        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            Select Case CType(gridSetRow.Item("項目セルタイプ"), EM_DATAGRIDVIEW_CELLTYPE)
                Case EM_DATAGRIDVIEW_CELLTYPE.emComboBoxCell
                Case Else
                    Continue For
            End Select

            Dim nNo As Integer = CInt(gridSetRow.Item("Lov_id"))
            If nNo < 1 Then Continue For

            ' SQL文作成
            strSQL = "SELECT disp_member, val_member_i, val_member_c, val_member_b  FROM " & DB_PAR_LOV_ITEM & " WHERE (lov_id = " & nNo & ")  ORDER BY seq"
            ' データテーブルに取得
            'SQL文実行
            If 0 > Get_SelectData(strSQL, dtst, sqlAdp, strErrMsg) Then
                'gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LOV_ITEM & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            End If
            dttbl = New DataTable

            sqlAdp.Fill(dttbl)

            Dim cmbDt As New DataGridViewComboBoxColumn()
            cmbDt.DataSource = dttbl
            cmbDt.DisplayMember = "disp_member"
            Select Case CInt(gridSetRow.Item("DB_フィールドタイプ"))
                Case EM_DB_TYPE.emStrType
                    cmbDt.ValueMember = "val_member_c"
                Case EM_DB_TYPE.emBitType
                    cmbDt.ValueMember = "val_member_b"
                Case Else
                    cmbDt.ValueMember = "val_member_i"
            End Select

            comboboxColumns.Add(gridSetRow.Item("DB_フィールド名").ToString(), cmbDt)
        Next
    End Sub

    ''' <summary>
    ''' コンボボックス編集（列ヘッダタイプ）[マニュアル用]
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <param name="comboboxColumns">対象コンボボックス(OUT)</param>
    ''' <remarks></remarks>
    Public Sub MakeComb_ColHeader(ByVal gdtGridSetting As DataTable, ByRef comboboxColumns As Generic.SortedList(Of String, DataGridViewComboBoxColumn))
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""
        Dim dtst As System.Data.DataSet = New System.Data.DataSet
        Dim dttbl As System.Data.DataTable = New System.Data.DataTable
        Dim sqlAdp As System.Data.SqlClient.SqlDataAdapter = Nothing

        comboboxColumns.Clear()

        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            Select Case CType(gridSetRow.Item("項目セルタイプ"), EM_DATAGRIDVIEW_CELLTYPE)
                Case EM_DATAGRIDVIEW_CELLTYPE.emComboBoxCell
                Case Else
                    Continue For
            End Select

            Dim nNo As Integer = CInt(gridSetRow.Item("Lov_id"))
            If nNo < 1 Then Continue For

            ' SQL文作成
            strSQL = "SELECT disp_member, val_member_i, val_member_c, val_member_b  FROM " & DB_PAR_LOV_ITEM & " WHERE (lov_id = " & nNo & ")  ORDER BY seq"
            ' データテーブルに取得
            'SQL文実行
            If 0 > Get_SelectData(strSQL, dtst, sqlAdp, strErrMsg) Then
                'gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LOV_ITEM & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            End If
            dttbl = New DataTable

            sqlAdp.Fill(dttbl)

            Dim cmbDt As New DataGridViewComboBoxColumn()
            cmbDt.DataSource = dttbl
            cmbDt.DisplayMember = "disp_member"
            Select Case CInt(gridSetRow.Item("DB_フィールドタイプ"))
                Case EM_DB_TYPE.emStrType
                    cmbDt.ValueMember = "val_member_c"
                Case EM_DB_TYPE.emBitType
                    cmbDt.ValueMember = "val_member_b"
                Case Else
                    cmbDt.ValueMember = "val_member_i"
            End Select

            comboboxColumns.Add(gridSetRow.Item("DB_フィールド名").ToString(), cmbDt)
        Next
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 画面に表示中のデータを非接続型テーブルにセット
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット（列ヘッダタイプ）
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <param name="strErrMsg">エラーメッセージ</param>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Public Function Data_Get_ColHeader(ByVal gdtGridSetting As DataSetHolder, ByRef strErrMsg As String) As Boolean

        Dim drowData As DataRow                 ' データ行
        Dim strWhere As String                  ' WHERE句に相当の条件生成
        Dim lstDt As New List(Of MAIN_KEY_INFO)
        Dim typdt As MAIN_KEY_INFO = Nothing
        Try
            '主キー取得
            lstDt.Clear()
            For Each gridSetRow As DataRow In gdtGridSetting.Rows
                If Not CBool(gridSetRow.Item("DB_主キー")) Then Continue For
                typdt.initialize()
                typdt.nKeyPos = CInt(gridSetRow.Item("項目No"))
                typdt.sKeyName = gridSetRow.Item("DB_フィールド名").ToString
                lstDt.Add(typdt)
            Next

            ' 条件No分ループ
            For ii As Integer = 0 To gdtGridSetting.dgvCnt.dgv.Rows.Count - 1
                strWhere = ""
                For jj As Integer = 0 To lstDt.Count - 1
                    If jj <> 0 Then strWhere &= " and "
                    strWhere &= lstDt(jj).sKeyName & "=" & CInt(gdtGridSetting.dgvCnt.dgv(lstDt(jj).nKeyPos - 1, ii).Value)
                Next
                drowData = gdtGridSetting.dsMain.Tables(0).Select(strWhere)(0)                                             ' 主キーを指定しての取り出しなので、一件しかない

                SetDrowData(gdtGridSetting, ii, drowData)
            Next ii

            Return True

        Catch ex As Exception
            strErrMsg = ex.Message
            Return False
        End Try

    End Function

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット（列ヘッダタイプ）
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <param name="strErrMsg">エラーメッセージ</param>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Public Function Data_Get_RowHeader(ByVal gdtGridSetting As DataSetHolder, ByRef strErrMsg As String) As Boolean

        Dim drowData As DataRow                 ' データ行
        Dim strWhere As String                  ' WHERE句に相当の条件生成
        Dim lstDt As New List(Of MAIN_KEY_INFO)
        Dim typdt As MAIN_KEY_INFO = Nothing
        Try
            '主キー取得
            lstDt.Clear()
            For Each gridSetRow As DataRow In gdtGridSetting.Rows
                If Not CBool(gridSetRow.Item("DB_主キー")) Then Continue For
                typdt.initialize()
                typdt.nKeyPos = CInt(gridSetRow.Item("項目No"))
                typdt.sKeyName = gridSetRow.Item("DB_フィールド名").ToString
                lstDt.Add(typdt)
            Next

            strWhere = ""
            For jj As Integer = 0 To lstDt.Count - 1
                If jj <> 0 Then strWhere &= " and "
                strWhere &= typdt.sKeyName & "=" & CInt(gdtGridSetting.dgvCnt.dgv(EM_ROW_VERTION_ID.emValue, typdt.nKeyPos - 1).Value)
            Next
            drowData = gdtGridSetting.dsMain.Tables(0).Select(strWhere)(0)        ' 主キーを指定しての取り出しなので、一件しかない

            SetDrowData(gdtGridSetting, 0, drowData)

            Return True

        Catch ex As Exception
            strErrMsg = ex.Message
            Return False
        End Try

    End Function

    ''' <summary>
    ''' 非接続型テーブルにセット
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <param name="nPos">データグリッドビュー内の行位置(列ヘッダタイプのみ使用)</param>
    ''' <param name="drowData">非接続型テーブル</param>
    ''' <remarks></remarks>
    Private Sub SetDrowData(ByVal gdtGridSetting As DataSetHolder, ByVal nPos As Integer, ByRef drowData As DataRow)
        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            '読み取り専用か？
            If CBool(gridSetRow.Item("表示_読み取り専用")) Then Continue For
            '列位置取得
            Dim nAddr As Integer = CInt(gridSetRow.Item("項目No")) - 1

            'データセット
            If EM_DATAGRIDVIEW_DISPTYPE.emColType = gdtGridSetting.dgvCnt.dgvType Then
                drowData.Item(CStr(gridSetRow.Item("DB_フィールド名"))) = gdtGridSetting.dgvCnt.dgv(nAddr, nPos).Value     '
            Else
                drowData.Item(CStr(gridSetRow.Item("DB_フィールド名"))) = gdtGridSetting.dgvCnt.dgv(EM_ROW_VERTION_ID.emValue, nAddr).Value    '
            End If
        Next
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' SELECT用SQL文作成
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <returns>SELECT用SQL文</returns>
    ''' <remarks></remarks>
    Public Function Create_Sql_Select(ByVal gdtGridSetting As DataSetHolder) As String
        Dim lstDt As New List(Of MAIN_KEY_INFO)
        Dim typdt As MAIN_KEY_INFO = Nothing

        Dim sql0 As String = ""
        sql0 = Create_Sql_Select_Item(gdtGridSetting)
        Dim DBName As String = GetItemDBName(gdtGridSetting)

        Dim strWk As String = ""
        '主キー取得
        lstDt.Clear()
        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            If Not CBool(gridSetRow.Item("DB_主キー")) Then Continue For
            typdt.initialize()
            typdt.nKeyPos = CInt(gridSetRow.Item("項目No"))
            typdt.sKeyName = gridSetRow.Item("DB_フィールド名").ToString
            lstDt.Add(typdt)
        Next

        '主キーによる並び替え作成
        Dim strOrder As String = ""
        For ii As Integer = 0 To lstDt.Count - 1
            If ii <> 0 Then strOrder &= ","
            strOrder &= lstDt(ii).sKeyName
        Next

        If m_DBlst.ContainsKey(DBName) Then
            If "" = strOrder Then
                strWk = String.Format(m_DBlst.Item(DBName).SelectSQLFormat, sql0, DBName)
            Else
                strWk = String.Format(m_DBlst.Item(DBName).SelectSQLFormat, sql0, DBName, strOrder)
            End If
        End If

        Return strWk
    End Function

    ''' <summary>
    ''' SELECT用データ項目連結
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function Create_Sql_Select_Item(ByVal gdtGridSetting As DataSetHolder) As String
        Dim strWk As String = ""
        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            If strWk.Length <> 0 Then
                strWk &= ", "
            End If
            strWk &= gridSetRow.Item("DB_フィールド名").ToString
        Next
        Return strWk
    End Function

    ''' <summary>
    ''' UPDATE用SQL文作成
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <param name="strFromKey">コピー元キー </param>
    ''' <param name="strToKey">コピー先キー </param>
    ''' <returns>UPDATE用SQL文</returns>
    ''' <remarks></remarks>
    Public Function Create_Sql_Update(ByVal gdtGridSetting As DataSetHolder, ByVal strFromKey As String, ByVal strToKey As String) As String
        Dim sql0 As String = ""
        sql0 = Create_Sql_Update_Item(gdtGridSetting)
        Dim DBName As String = GetItemDBName(gdtGridSetting)
        Dim strWk As String = ""
        If m_DBlst.ContainsKey(DBName) Then
            strWk = String.Format(m_DBlst.Item(DBName).UpdateSQLFormat, DBName, sql0, DBName, DBName, strFromKey, strToKey)
        End If

        Return strWk
    End Function

    ''' <summary>
    ''' UPDATE用データ項目連結
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function Create_Sql_Update_Item(ByVal gdtGridSetting As DataSetHolder) As String
        Dim strWk As String = ""
        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            '読み取り専用か？
            If CBool(gridSetRow.Item("表示_読み取り専用")) Then Continue For
            'コピー元の列名のAsは"b"(固定フォーマットの絡みで・・・)
            strWk &= tClassLibrary.tClass.tClass_SQLServer.sqlColumnUpdate(CStr(gridSetRow.Item("DB_フィールド名")), "b")
        Next
        tClassLibrary.tClass.tClass_SQLServer.sqlColumnTrim(strWk)                  ' おまじない、必ず記述
        Return strWk
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ その他
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' デフォルト色に戻す
    ''' </summary>
    ''' <param name="dgv">データグリッドビューコントロール</param>
    ''' <remarks></remarks>
    Public Sub SetBackColorEmpty(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '行の背景をデフォルト色に戻す
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv.Rows(ii).DefaultCellStyle.BackColor = Color.Empty
        Next
    End Sub

    ''' <summary>
    ''' 更新ボタンの有効/無効
    ''' </summary>
    ''' <param name="bPassResult">パスワード入力結果 (True:有効 False:無効)</param>
    ''' <remarks></remarks>
    Public Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' ファンクションキーの設定ボタン

    End Sub

    ''' <summary>
    ''' 列幅変更
    ''' </summary>
    ''' <param name="dgv">データグリッドビューコントロール</param>
    ''' <remarks></remarks>
    Public Sub DataGridViewColumnWidthChanged(ByVal dgv As DataGridView)
        ' 下の関数にイベントハンドル割付
        AddHandler dgv.ColumnWidthChanged, AddressOf dgvColumnWidthChanged
    End Sub

    ''' <summary>
    ''' 列幅調整時、dgvを再描画
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>ダミーで表示しているスクロールバーに残像が残る為</remarks>
    Private Sub dgvColumnWidthChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewColumnEventArgs)

        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' スクロールバーの再描画
        dgv.Refresh()

    End Sub

    ''' <summary>
    ''' 対象テーブル名称取得
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <returns>テーブル名称</returns>
    ''' <remarks></remarks>
    Public Function GetItemDBName(ByVal gdtGridSetting As DataSetHolder) As String
        Dim strDBName As String = ""
        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            If 1 = CInt(gridSetRow.Item("項目No")) Then
                strDBName = gridSetRow.Item("DB_テーブル名").ToString()
                Exit For
            End If
        Next
        Return strDBName
    End Function


    ''' <summary>
    ''' リストコピー対象有無(scode対象有無)
    ''' </summary>
    ''' <param name="gdtGridSetting">画面構成情報</param>
    ''' <returns>リストコピー対象有無 True:対象、False:対象外</returns>
    ''' <remarks></remarks>
    Public Function CopyTargetTable(ByVal gdtGridSetting As DataSetHolder) As Boolean
        Dim bRet As Boolean = False
        Dim DBName As String = GetItemDBName(gdtGridSetting)
        If m_DBlst.ContainsKey(DBName) Then
            bRet = m_DBlst.Item(DBName).bCopyDB
        End If

        Return bRet
    End Function

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="targetObj"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Shared Function isNullOrEmpty(ByVal targetObj As Object) As Boolean
        'DB評価でNULLの場合もTrueを返す
        If IsDBNull(targetObj) Then
            Return True
        End If
        'ターゲットがnullの場合Trueを返す
        If IsNothing(targetObj) Then
            Return True
        End If
        'ターゲットがnullでない場合は、文字列に変換し判定する
        Return String.IsNullOrEmpty(CStr(targetObj))

    End Function



    ''' <summary>
    ''' アップデート
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Protected Overridable Function DB_UpDate() As Boolean

        Try
            Using cbWk As New SqlClient.SqlCommandBuilder()
                For Each Data As DataSetHolder In m_DataSets
                    cbWk.DataAdapter = Data.adapter
                    Data.adapter.Update(Data.dsMain)
                Next
            End Using

            '統括機能へパラメータ変更通知
            modMain.Send_Main(my_paramID)
            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " 設定エラー [" & ex.Message & "] " & Me.Text)
            Return False
        End Try

    End Function



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ おまけ
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' データグリッドビューテキストボックス列設定
    ''' </summary>
    ''' <param name="dgv">コントロール</param>
    ''' <param name="sHeader">列ヘッダー名称</param>
    ''' <param name="nWidth">列幅</param>
    ''' <param name="bFrozen">横スクロール時のロック</param>
    ''' <param name="bRead">読込専用</param>
    ''' <param name="nSort">並べ替えモード</param>
    ''' <param name="nDefaultCellStyle">列スタイル</param>
    ''' <param name="nHeaderCellStyle">列ヘッダースタイル</param>
    ''' <param name="sNullVal">データがNULL時の表示内容</param>
    ''' <param name="sDataNullVal">NULL入力時のデータ内容</param>
    ''' <param name="bDispVis">表示・非表示</param>
    ''' <remarks></remarks>
    Public Sub dgvColSetText(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer, _
                             ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode, _
                             ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment, _
                             Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewTextBoxColumn
            ' 列ヘッダー名称
            clm.HeaderText = sHeader
            ' 列幅
            clm.Width = nWidth
            ' 横スクロール時のロック
            clm.Frozen = bFrozen
            ' 読込専用設定
            clm.ReadOnly = bRead
            ' 並べ替えモード
            clm.SortMode = nSort
            ' 列スタイル
            clm.DefaultCellStyle.Alignment = nDefaultCellStyle
            ' 列ヘッダースタイル
            clm.HeaderCell.Style.Alignment = nHeaderCellStyle
            ' 表示・非表示
            clm.Visible = bDispVis
            ' データがNULL時の表示内容 
            clm.DefaultCellStyle.NullValue = sNullVal
            ' NULL入力時のデータ内容 
            clm.DefaultCellStyle.DataSourceNullValue = sDataNullVal

            ' 列作成
            dgv.Columns.Add(clm)
        End Using

    End Sub

    ''' <summary>
    ''' データグリッドビューチェックボックス列設定
    ''' </summary>
    ''' <param name="dgv">コントロール</param>
    ''' <param name="sHeader">列ヘッダー名称</param>
    ''' <param name="nWidth">列幅</param>
    ''' <param name="bFrozen">横スクロール時のロック</param>
    ''' <param name="bRead">読込専用</param>
    ''' <param name="nSort">並べ替えモード</param>
    ''' <param name="nDefaultCellStyle">列スタイル</param>
    ''' <param name="nHeaderCellStyle">列ヘッダースタイル</param>
    ''' <param name="sNullVal">データがNULL時の表示内容</param>
    ''' <param name="sDataNullVal">NULL入力時のデータ内容</param>
    ''' <param name="bDispVis">表示・非表示</param>
    ''' <remarks></remarks>
    Public Sub dgvColSetCheck(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer, _
                              ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode, _
                              ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment, _
                              Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewCheckBoxColumn
            ' 列ヘッダー名称
            clm.HeaderText = sHeader
            ' 列幅
            clm.Width = nWidth
            ' 横スクロール時のロック
            clm.Frozen = bFrozen
            ' 読込専用設定
            clm.ReadOnly = bRead
            ' 並べ替えモード
            clm.SortMode = nSort
            ' 列スタイル
            clm.DefaultCellStyle.Alignment = nDefaultCellStyle
            ' 列ヘッダースタイル
            clm.HeaderCell.Style.Alignment = nHeaderCellStyle
            ' 表示・非表示
            clm.Visible = bDispVis
            ' データがNULL時の表示内容 
            clm.DefaultCellStyle.NullValue = sNullVal
            ' NULL入力時のデータ内容 
            clm.DefaultCellStyle.DataSourceNullValue = sDataNullVal

            ' 列作成
            dgv.Columns.Add(clm)
        End Using

    End Sub
    ''' <summary>
    ''' データグリッドビューボタン列設定
    ''' </summary>
    ''' <param name="dgv">コントロール</param>
    ''' <param name="sHeader">列ヘッダー名称</param>
    ''' <param name="nWidth">列幅</param>
    ''' <param name="bFrozen">横スクロール時のロック</param>
    ''' <param name="bRead">読込専用</param>
    ''' <param name="nSort">並べ替えモード</param>
    ''' <param name="nDefaultCellStyle">列スタイル</param>
    ''' <param name="nHeaderCellStyle">列ヘッダースタイル</param>
    ''' <param name="fontDefaultFont">列フォント</param>
    ''' <param name="bBtnTextValue">ボタン内文字表示・非表示</param>
    ''' <param name="sBtnTextName">ボタン内文字設定</param>
    ''' <param name="sNullVal">データがNULL時の表示内容</param>
    ''' <param name="sDataNullVal">NULL入力時のデータ内容</param>
    ''' <param name="bDispVis">表示・非表示</param>
    ''' <remarks></remarks>
    Public Sub dgvColSetButton(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer, _
                               ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode, _
                               ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment, _
                               ByVal fontDefaultFont As System.Drawing.Font, ByVal bBtnTextValue As Boolean, ByVal sBtnTextName As String, _
                               Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewButtonColumn
            ' 列ヘッダー名称
            clm.HeaderText = sHeader
            ' 列幅
            clm.Width = nWidth
            ' 横スクロール時のロック
            clm.Frozen = bFrozen
            ' 読込専用設定
            clm.ReadOnly = bRead
            ' 並べ替えモード
            clm.SortMode = nSort
            ' 列スタイル
            clm.DefaultCellStyle.Alignment = nDefaultCellStyle
            ' 列ヘッダースタイル
            clm.HeaderCell.Style.Alignment = nHeaderCellStyle
            ' 列フォント
            clm.DefaultCellStyle.Font = fontDefaultFont
            ' 表示・非表示
            clm.Visible = bDispVis
            ' データがNULL時の表示内容 
            clm.DefaultCellStyle.NullValue = sNullVal
            ' NULL入力時のデータ内容 
            clm.DefaultCellStyle.DataSourceNullValue = sDataNullVal

            ' ボタン設定作成
            Dim BtnText As DataGridViewButtonColumn = CType(clm, DataGridViewButtonColumn)
            ' ボタン内文字表示・非表示
            BtnText.UseColumnTextForButtonValue = bBtnTextValue
            ' ボタン内文字設定
            BtnText.Text = sBtnTextName

            ' 列作成
            dgv.Columns.Add(clm)
        End Using

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DB情報
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 使用DB情報初期化
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Clear_BaseDBList()
        m_DBlst.Clear()
    End Sub

    ''' <summary>
    ''' 使用DB情報登録
    ''' </summary>
    ''' <param name="DBName"></param>
    ''' <param name="typ"></param>
    ''' <remarks></remarks>
    Public Sub Add_BaseDBList(ByRef DBName As String, ByVal typ As TABLE_INFO)
        m_DBlst.Add(DBName, typ)
    End Sub
End Class