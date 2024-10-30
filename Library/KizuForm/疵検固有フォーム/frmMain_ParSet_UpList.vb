'*********************************************************************************
' パラメータ設定画面のリスト付きの継承フォーム
'	[Ver]
'		Ver.01    2007/03/31  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

''' <summary>
''' パラメータ設定画面用のリスト付きの継承クラス
''' </summary>
''' <remarks></remarks>
Public Class frmMain_ParSet_UpList

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 定数定義
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' アップコードの列位置
	''' </summary>
	''' <remarks></remarks>
	Private Enum em_UpCodeItem
		iCheck = 0						' チェックボックス
		iNo								' No
		iName							' 名称
	End Enum


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

            .SelectionMode = DataGridViewSelectionMode.CellSelect       ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = False                                               ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditOnEnter            ' セル書き換え

            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            '' 選択セル色定義
			.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue			' 選択
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent      ' 選択色 無効化 (dgvLog_SelectionChangedが必須)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1行おきに色替え
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
                clm.Width = 38
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


End Class
