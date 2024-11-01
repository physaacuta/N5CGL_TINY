'*********************************************************************************
' パラメータ設定画面のリスト付きの継承フォーム
'	[Ver]
'		Ver.01    2007/03/31  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On
Imports tClassLibrary.tClass

''' <summary>
''' パラメータ設定画面用のリスト付きの継承クラス
''' </summary>
''' <remarks></remarks>
Public Class frmMain_AutoParSet_UpList

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' アップコードの列位置
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum em_UpCodeItem
        iCheck = 0                      ' チェックボックス
        iNo                             ' No
        iName                           ' 名称
    End Enum

    Public m_KeyCode As String = ""                                    ' 主キー
    Public m_cmbSCode As New ComboBox
    Public m_lblSCode As New Label
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メソッド
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_ParSet_UpList_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        '''' グリッドビュー定義
        dgvInitialize_UpCode(dgvUpCodeItem)
        '''' グリッドビュー列定義
        dgvColumnDef_UpCode(dgvUpCodeItem)
        MyBase.DataGridViewSelectionCancel(dgvUpCodeItem)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ アップコード部分 関連
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' アップコード コンボボックスにセットされているアイテムを全部セット
    ''' </summary>
    ''' <param name="cmb">アップコード用のコンボボックス</param>
    ''' <param name="dgv">アップコード用のデータグリッドビュー</param>
    ''' <remarks></remarks>
    Public Sub SetCmbUpCodeItem(ByVal cmb As ComboBox, ByVal dgv As DataGridView)
        ' アップコード コンボボックスにセットされているアイテムを全部セット
        dgv.RowCount = 0
        For Each item As cls_CmbItem In cmb.Items
            dgv.Rows.Add(False, item.id, item.name)
        Next item
    End Sub


    ''' <summary>
    ''' アップコード部分の全選択ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnUpCodeItem_All_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnUpCodeItem_All.Click
        ' 全てのチェックボックスON
        For ii As Integer = 0 To dgvUpCodeItem.RowCount - 1
            dgvUpCodeItem(em_UpCodeItem.iCheck, ii).Value = True
        Next
    End Sub
    ''' <summary>
    ''' アップコード部分の全解除ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnUpCodeItem_Clrea_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnUpCodeItem_Clrea.Click
        ' 全てのチェックボックスON
        For ii As Integer = 0 To dgvUpCodeItem.RowCount - 1
            dgvUpCodeItem(em_UpCodeItem.iCheck, ii).Value = False
        Next
    End Sub
    ''' <summary>
    ''' アップコードのチェックボックスが変更された
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvUpCodeItem_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvUpCodeItem.CellValueChanged
        ' チェックボックスのデータ部のみ
        If e.RowIndex < 0 OrElse e.ColumnIndex <> em_UpCodeItem.iCheck Then Exit Sub

        '1行全部色変え
        dgvUpCodeItem.Rows(e.RowIndex).DefaultCellStyle.BackColor = CType(IIf(CBool(dgvUpCodeItem(e.ColumnIndex, e.RowIndex).Value), g_ColorSetting, Color.Empty), Color)
    End Sub

    ''' <summary>
    ''' セルの内容が変化した場合
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>DataGridViewのチェックボックス等押された場合 通常 セルを移動させないと確定にならない為の処置</remarks>
    Private Sub dgvUpCodeItem_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvUpCodeItem.CurrentCellDirtyStateChanged
        ' セルがコミットされていない変更を含む場合 コミットする
        If dgvUpCodeItem.IsCurrentCellDirty Then dgvUpCodeItem.CommitEdit(DataGridViewDataErrorContexts.Commit)
    End Sub


    ''' <summary>
    ''' コンボボックス 選択
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbUpCodeItem_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbUpCodeItem.SelectedIndexChanged
        ' コンボボックス変更
        MyBase.SelectComboBoxItem(cmbUpCodeItem.SelectedIndex, cmbUpCodeItem, lblUpCodeItem)
    End Sub





    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 表示設定
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize_UpCode(ByVal dgv As DataGridView)
        With dgv
            .RowCount = 1
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

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = False                                               ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' セル書き換え

            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent      ' 選択色 無効化 (dgvLog_SelectionChangedが必須)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

        End With
    End Sub

    ''' <summary>
    ''' 表示設定
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvColumnDef_UpCode(ByVal dgv As DataGridView)
        With dgv
            .ColumnCount = 0

            ''=================================
            '' 列定義

            ' 選択用チェックボックス
            Using clm As New DataGridViewCheckBoxColumn
                clm.HeaderText = ""
                clm.Width = 20
                clm.Frozen = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter

                .Columns.Add(clm)
            End Using

            ' No
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "No"
                clm.Width = 35
                clm.Frozen = False
                clm.Selected = False
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter

                .Columns.Add(clm)
            End Using

            ' 名称
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "名称"
                clm.Width = 110
                clm.Frozen = False
                clm.Selected = False
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter

                .Columns.Add(clm)
            End Using
        End With
    End Sub

    ''' <summary>
    ''' アップコードのコピー先アイテム取得
    ''' </summary>
    ''' <param name="isNum">主キーは数値用の場合True</param>
    ''' <returns>アイテム ("1,2,3")または("'1','2','3'")</returns>
    ''' <remarks></remarks>
    Public Function GetUpCodeItem_To(ByVal isNum As Boolean) As String
        Dim strWk As String = ""
        'チェックアイテム取得
        For ii As Integer = 0 To dgvUpCodeItem.RowCount - 1
            If CBool(dgvUpCodeItem(0, ii).Value) Then
                If strWk.Length <> 0 Then
                    strWk &= ", "
                End If

                If isNum Then '数値
                    strWk &= SelectComboBoxKey(ii, cmbUpCodeItem)
                Else            ' 文字
                    strWk &= "'" & SelectComboBoxKey(ii, cmbUpCodeItem) & "'"
                End If
            End If
        Next ii
        Return strWk
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ SQL関連
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' コピー元からコピー先へデータ更新
    ''' </summary>
    ''' <param name="strSQL">発行SQL文</param>
    ''' <param name="strErrMsg">エラーメッセージ</param>
    ''' <returns>影響を受けた行数を返す　-1:失敗 -9:DB接続失敗 </returns>
    ''' <remarks></remarks>
    Public Function UpCode_UpDate(ByVal strSQL As String, ByRef strErrMsg As String) As Integer

        Dim tcls_DB As New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)

        'メッセージ初期化
        strErrMsg = ""

        Try

            ' SQL実行
            If Not tcls_DB.Open() Then
                strErrMsg = "DB接続失敗"
                Return -9
            End If

            ' 更新
            Return tcls_DB.DMLExecute(strSQL)

        Catch ex As Exception
            strErrMsg = ex.Message
            Return -1
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

    End Function

    ''' <summary>
    ''' 更新ボタンの有効/無効
    ''' </summary>
    ''' <param name="bPassResult">パスワード入力結果 (True:有効 False:無効)</param>
    ''' <remarks></remarks>
    Public Overloads Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' ファンクションキーの設定ボタン
        pnlItem.Enabled = bPassResult                   ' コピー用リストの反映ボタン

    End Sub
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ アップコード 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 反映ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Overridable Sub btnUpCodeItem_UpDate_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnUpCodeItem_UpDate.Click

        ' コピー元の条件チェック
        '元
        Dim strFromKey As String = ""                                    'コピー元キー 
        If cmbUpCodeItem.SelectedIndex < 0 Then Exit Sub '元
        strFromKey = SelectComboBoxKey(cmbUpCodeItem.SelectedIndex, cmbUpCodeItem)
        '先
        Dim strToKey As String = GetUpCodeItem_To(False)                'コピー先キー
        If strToKey.Length = 0 Then Exit Sub

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            'DBコピー
            If CopyDB(strFromKey, strToKey) Then
                '正常の時は統括機能へパラメータ変更通知
                modMain.Send_Main(MyBase.tProp_MyParamID, CH_SCODE_ALL)
                modMain.HardCopy(Me)
                frmMsg.Text = "反映処理"
                frmMsg.ChangeMsg("正常に登録しました", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            Else
                frmMsg.Text = "反映処理"
                frmMsg.ChangeMsg("登録に失敗", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

            ' データの再取得
			If Not DB_Select() Then
				' DB 失敗なので、画面強制終了
				frmMsg.Text = "異常"
				frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
				frmMsg.ShowDialog()
				frmMsg.Dispose()
				modMain.TaskEnd()						'終了
			End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        ' 画面に表示
        Data_Disp(m_KeyCode)

    End Sub
    ''' <summary>
    ''' コピー処理
    ''' </summary>
    ''' <param name="strFromKey">コピー元キー </param>
    ''' <param name="strToKey">コピー先キー </param>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Public Overridable Function CopyDB(ByVal strFromKey As String, ByVal strToKey As String) As Boolean
        For Each Data As DataSetHolder In m_DataSets
            If Not CopyTargetTable(Data) Then Continue For
            '表面状態別(ParamScode)を更新
            Dim strErrMsg As String = ""
            If 0 > UpCode_UpDate(MyBase.Create_Sql_Update(Data, strFromKey, strToKey), strErrMsg) Then
                gcls_Log.Write(tClass_LogManager.LNO.ERR, MyBase.GetItemDBName(Data) & " 更新エラー [" & strErrMsg & "] " & Me.Text)
                Return False
            End If
        Next
        Return True
    End Function


    ''' <summary>
    ''' 検査表面状態Noコンボボックス情報初期セット
    ''' </summary>
    ''' <param name="comb">コンボボックスコントロール</param>
    ''' <param name="lblScode">ラベル</param>
    ''' <remarks></remarks>
    Public Sub Set_cmbSCode_Item(ByVal comb As ComboBox, ByVal lblScode As Label)
        m_cmbSCode = comb
        m_lblSCode = lblScode
        ' 下の関数にイベントハンドル割付
        AddHandler comb.SelectedIndexChanged, AddressOf cmbSCode_SelectedIndexChanged
    End Sub

    ''' <summary>
    ''' 検査表面状態No変更時
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbSCode_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        ' 今の選択と主キーが同じ場合は無視
        If m_KeyCode = m_cmbSCode.Text Then Exit Sub

        '' 画面のデータを非接続型テーブルにセット
        ' 初期画面表示時には、データ無しの為、画面のデータを取り込まない
        If m_KeyCode <> "" Then
            ' 入力値チェック
            If Not Data_Check() Then
                ' 入力異常時は、選択位置も戻す
                m_cmbSCode.Text = m_KeyCode
                Exit Sub
            End If

            ' 変更前のキーに対し 画面データ取り込み
            Data_Get()
        End If

        ' 選択変更に伴い ラベル値変更
        MyBase.SelectComboBoxItem(m_cmbSCode.SelectedIndex, m_cmbSCode, m_lblSCode)

        ' 画面にパターン別の値を表示
        m_KeyCode = SelectComboBoxKey(m_cmbSCode.SelectedIndex, m_cmbSCode)
        Data_Disp(m_KeyCode)

        '今の選択状態を保存
        g_nScodeIndex = m_cmbSCode.SelectedIndex

    End Sub

    ''' <summary>
    ''' 前項 次項
    ''' </summary>
    ''' <param name="bln">前項:false 次項:true</param>
    ''' <remarks></remarks>
    Public Sub ChangeCode(ByVal bln As Boolean)
        Dim rec As Integer = CInt(IIf(bln, m_cmbSCode.SelectedIndex + 1, m_cmbSCode.SelectedIndex - 1))
        ' 上下限チェック
        If rec < 0 OrElse rec >= m_cmbSCode.Items.Count Then Exit Sub
        '切替
        m_cmbSCode.SelectedIndex = rec
    End Sub

    ''' <summary>
    ''' 初回ローディング処理(検査表面状態(scode)有り)
    ''' </summary>
    ''' <remarks></remarks>
    Public Overloads Sub InitLoading()
        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
            Exit Sub
        End If

        MyBase.dgvInitialize()

        ' コンボボックスに検査パターンをセット
        MyBase.SetComboBox_Scode(m_cmbSCode)

        If 0 < m_cmbSCode.Items.Count Then

            '前の画面で選択した状態をセット
            If m_cmbSCode.Items.Count - 1 < g_nScodeIndex Then
                '該当項目がない場合は、先頭を表示
                m_cmbSCode.SelectedIndex = 0
            Else
                m_cmbSCode.SelectedIndex = g_nScodeIndex
            End If
        Else
            ' 画面表示失敗なので、強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DBにデータが無い為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If

        For Each Data As DataSetHolder In m_DataSets
            Data.dgvCnt.dgv.DataGridViewSelectionRowColorChange(g_ColorSelection)
            MyBase.DataGridViewColumnWidthChanged(Data.dgvCnt.dgv)
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
    ''' 初回ローディング処理(検査表面状態(scode)有り)(マニュアル用)
    ''' </summary>
    ''' <remarks></remarks>
    Public Overloads Sub InitLoading2()
        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
            Exit Sub
        End If

        ' コンボボックスに検査パターンをセット
        MyBase.SetComboBox_Scode(m_cmbSCode)

        If 0 < m_cmbSCode.Items.Count Then

            '前の画面で選択した状態をセット
            If m_cmbSCode.Items.Count - 1 < g_nScodeIndex Then
                '該当項目がない場合は、先頭を表示
                m_cmbSCode.SelectedIndex = 0
            Else
                m_cmbSCode.SelectedIndex = g_nScodeIndex
            End If
        Else
            ' 画面表示失敗なので、強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DBにデータが無い為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If

        '=======================================================

        frmMsg.Dispose()
        frmMsg = Nothing
    End Sub

End Class
