'*********************************************************************************
' 自動生成画面データグリッド表示設定クラス
'	[Ver]
'		Ver.01    2015/07/08  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmGridSetting
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' データグリッドビューの列単位
    Private Enum EM_GRID
        emTxt_KNo = 0                           ' 項目No
        emTxt_Hedder1                           ' 項目ヘッダー１
        emTxt_Hedder2                           ' 項目ヘッダー２
        emTxt_Bikou                             ' 備考
        emCmb_CellType                          ' 項目セルタイプ
        emCmb_Lov_id                            ' lov_id
        emChk_DispUmu                           ' 項目表示有無
        emChk_ReadOnly                          ' 読み取り専用
        emTxt_KHaba                             ' 項目幅
        emCmb_KHaiti                            ' 項目配置
        emTxt_Format                            ' フォーマット
        emChk_Frozen                            ' 列固定
        emCmb_TableName                         ' テーブル名
        emCmb_Field                             ' フィールド名
        emChk_Key                               ' 主キー
        emCmb_FieldType                         ' フィールドタイプ
        emTxt_InputMin                          ' 入力可能最小値
        emTxt_InputMax                          ' 入力可能最大値
        emChk_NullOK                            ' NULL入力可否
        emTxt_InputKeta                         ' 入力可能桁数
        emTxt_DecKeta                           ' 小数点以下桁数
        emCmb_DispDialog                        ' 表示ダイアログ
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterGrid As New SqlClient.SqlDataAdapter        ' アダプター データグリッド表示設定
    Private m_Grid As New DataSet                                ' 非接続型テーブル データグリッド表示設定
    Private m_AdapterLov_id As New SqlClient.SqlDataAdapter      ' アダプター Lov_id
    Private m_Lov_id As New DataSet                              ' 非接続型テーブル Lov_id
    Private m_AdapterField As New SqlClient.SqlDataAdapter       ' アダプターDBフィールド名
    Private m_Field As New DataSet                               ' 非接続型テーブル DBフィールド名
    Private m_AdapterDBKey As New SqlClient.SqlDataAdapter       ' アダプター主キー取得
    Private m_DBKey As New DataSet                               ' 非接続型テーブル 主キー取得
    Private m_AdapterDBAttr As New SqlClient.SqlDataAdapter      ' アダプターDB属性取得
    Private m_DBAttr As New DataSet                              ' 非接続型テーブル DB属性取得

    Private Const ITEM_MAX_COUNT As Integer = 10000              ' グリッドROW項目数
    Private m_nDelItem(ITEM_MAX_COUNT - 1) As Integer            ' 削除対象 項目No
    Private m_nDispItem(ITEM_MAX_COUNT - 1) As Integer           ' 表示中 項目No
    Private Const GRID_ROW_COUNT As Integer = 22                 ' グリッドROW項目数
    Private m_KeyGID As String = ""                              ' 選択中画面ID
    Private m_KeyGridID As String = ""                           ' 選択中グリッドID

    Private m_bDispFlg As Boolean = False                        ' 表示処理中判定用フラグ true:表示処理中 false:処理中で無い
    '//////////////////////////////////////////////////////////////////////////////////
    '画面IDに紐付く情報構造体
    '//////////////////////////////////////////////////////////////////////////////////
    Public Structure GID_INFO
        Dim strDBGID As String                                   ' DB上のグリッドID
        Dim nGridCnt As Integer                                  ' グリッド数
        Dim listGrid As Dictionary(Of Integer, String)           ' グリッドIDをキーとした画面表示グリッドID
        Dim listGridID As Dictionary(Of String, Integer)         ' 画面表示グリッドIDをキーとしたグリッドID
        Public Sub initialize()
            listGrid.Clear()
            listGridID.Clear()
        End Sub
    End Structure
    Public m_DBlst As New Dictionary(Of String, GID_INFO)      ' 画面表示グリッドID取得用情報

    ''' <summary>
    ''' ファンクション切替
    ''' </summary>
    ''' <param name="FncNo">ファンクションNo (0オリジン)</param>
    ''' <param name="enable">ファンクションボタンの有効無効状態</param>
    ''' <param name="onKey">同時に押されているキー</param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "データグリッド表示設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    ''' <param name="cancel">選択を取りやめる場合は、Trueをセット</param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' いろいろ開放処理
        m_Grid.Dispose()
        If Not m_AdapterGrid Is Nothing Then
            m_AdapterGrid.Dispose()
            m_AdapterGrid = Nothing
        End If

        m_Lov_id.Dispose()
        If Not m_AdapterLov_id Is Nothing Then
            m_AdapterLov_id.Dispose()
            m_AdapterLov_id = Nothing
        End If

        m_Field.Dispose()
        If Not m_AdapterField Is Nothing Then
            m_AdapterField.Dispose()
            m_AdapterField = Nothing
        End If

        m_DBKey.Dispose()
        If Not m_AdapterField Is Nothing Then
            m_AdapterField.Dispose()
            m_AdapterField = Nothing
        End If

        m_DBAttr.Dispose()
        If Not m_AdapterDBAttr Is Nothing Then
            m_AdapterDBAttr.Dispose()
            m_AdapterDBAttr = Nothing
        End If

        ' 画面切替
        ShowForm(frmID)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メソッド定義
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        'タイトル設定
        Call SetTitleName(Me)

        modMain.SetListMenu(Me)                                 ' メニューリスト生成
        modMain.SetLabelSysName(Me)                             ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmGridSetting       ' 画面呼出

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        'dgv初期化
        dgvInitializeGrid(dgvGrid)

        ' このフォームで必要なデータを取得する
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If

        '画面IDコンボボックス用初期処理
        Call SetComboBox_GID()
        Call SetComboBox_GridID()

        ' 画面に値を表示
        Data_Disp()

        ' 選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
        dgvGrid.DataGridViewSelectionRowColorChange(g_ColorSelection)     ' dgv

        ' 更新ボタンの有効/無効
        Call UpdateEnabled(g_bPassFlg)

        frmMsg.Dispose()
        frmMsg = Nothing

    End Sub
    ''' <summary>
    ''' ディスポーズ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '現在の表示中の画面が自分の画面ならアプリケーションを終了
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    ''' データの取得
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ' SQL文生成
        strSQL = "SELECT 画面ID"
        strSQL &= ", グリッドID"
        strSQL &= ", 項目No"
        strSQL &= ", 項目ヘッダー1"
        strSQL &= ", 項目ヘッダー2"
        strSQL &= ", 備考"
        strSQL &= ", 項目セルタイプ"
        strSQL &= ", lov_id"
        strSQL &= ", 表示_項目表示有無"
        strSQL &= ", 表示_読み取り専用"
        strSQL &= ", 表示_項目幅"
        strSQL &= ", 表示_項目配置"
        strSQL &= ", 表示_フォーマット"
        strSQL &= ", 表示_列固定"
        strSQL &= ", DB_テーブル名"
        strSQL &= ", DB_フィールド名"
        strSQL &= ", DB_主キー"
        strSQL &= ", DB_フィールドタイプ"
        strSQL &= ", 入力_入力可能最小値"
        strSQL &= ", 入力_入力可能最大値"
        strSQL &= ", 入力_NULL入力可否"
        strSQL &= ", 入力_入力可能桁数"
        strSQL &= ", 入力_小数点以下桁数"
        strSQL &= ", 入力_表示ダイアログ"
        strSQL &= " FROM " & DB_PAR_GRID_SETTING

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_Grid, m_AdapterGrid, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PAR_GRID_SETTING & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL文生成
        strSQL = "SELECT DISTINCT lov_id"
        strSQL &= " FROM " & DB_PAR_LOV_ITEM
        strSQL &= " ORDER BY lov_id ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_Lov_id, m_AdapterLov_id, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PAR_LOV_ITEM & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()
        m_bDispFlg = True
        'データを表示
        Data_DispdgvGrid(dgvGrid)
        m_bDispFlg = False

        ' 行の色を変える （初期表示で色が変わる対策）
        dgvGrid.CurrentRow.DefaultCellStyle.BackColor = Color.Empty

    End Sub

    ''' <summary>
    ''' dgvにデータを表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_DispdgvGrid(ByVal dgv As DataGridView)
        ' 表示中画面ID及びグリッドIDに紐付く情報を対象とする
        Dim strGridWhere As String = String.Format("画面ID='{0}' and グリッドID = '{1}'", m_DBlst(cmbGID.Text).strDBGID, m_DBlst(cmbGID.Text).listGridID(cmbGridID.Text))          ' 表示中画面ID 対象
        Dim drowGrid() As DataRow = m_Grid.Tables(0).Select(strGridWhere)           ' データグリッド表示設定
        Dim drowLov_id() As DataRow = m_Lov_id.Tables(0).Select()                   ' Lov_id
        '初期化
        dgv.Rows.Clear()
        dgv.Columns.Clear()

        dgv.RowCount = GRID_ROW_COUNT                   ' ROW項目数
        Call SetDgvTitle()                              ' データグリッド見出し設定

        For ii As Integer = 0 To drowGrid.Length - 1
            '表示項目用の列を追加
            Call AddDgvRetu(ii)
            '表示情報をセット
            dgv(ii, EM_GRID.emTxt_KNo).Value = drowGrid(ii).Item("項目No").ToString.Trim()
            dgv(ii, EM_GRID.emTxt_Hedder1).Value = drowGrid(ii).Item("項目ヘッダー1").ToString.Trim()
            dgv(ii, EM_GRID.emTxt_Hedder2).Value = drowGrid(ii).Item("項目ヘッダー2").ToString.Trim()
            dgv(ii, EM_GRID.emTxt_Bikou).Value = drowGrid(ii).Item("備考").ToString.Trim()

            If "0" = drowGrid(ii).Item("項目セルタイプ").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_CellType).Value = "TextBox"
            ElseIf "1" = drowGrid(ii).Item("項目セルタイプ").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_CellType).Value = "ComboBox"
            ElseIf "2" = drowGrid(ii).Item("項目セルタイプ").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_CellType).Value = "CheckBox"
            ElseIf "3" = drowGrid(ii).Item("項目セルタイプ").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_CellType).Value = "Button"
            Else
                dgv(ii, EM_GRID.emCmb_CellType).Value = "TextBox"
            End If

            If "" = drowGrid(ii).Item("lov_id").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_Lov_id).Value = "対象外"
            ElseIf "0" = drowGrid(ii).Item("lov_id").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_Lov_id).Value = "対象外"
            Else
                dgv(ii, EM_GRID.emCmb_Lov_id).Value = drowGrid(ii).Item("lov_id").ToString.Trim()
            End If

            dgv(ii, EM_GRID.emChk_DispUmu).Value = drowGrid(ii).Item("表示_項目表示有無").ToString.Trim()
            dgv(ii, EM_GRID.emChk_ReadOnly).Value = drowGrid(ii).Item("表示_読み取り専用").ToString.Trim()
            dgv(ii, EM_GRID.emTxt_KHaba).Value = drowGrid(ii).Item("表示_項目幅").ToString.Trim()

            If "1" = drowGrid(ii).Item("表示_項目配置").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "上左"
            ElseIf "2" = drowGrid(ii).Item("表示_項目配置").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "上中"
            ElseIf "4" = drowGrid(ii).Item("表示_項目配置").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "上右"
            ElseIf "16" = drowGrid(ii).Item("表示_項目配置").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "中左"
            ElseIf "32" = drowGrid(ii).Item("表示_項目配置").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "中中"
            ElseIf "64" = drowGrid(ii).Item("表示_項目配置").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "中右"
            ElseIf "256" = drowGrid(ii).Item("表示_項目配置").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "下左"
            ElseIf "512" = drowGrid(ii).Item("表示_項目配置").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "下中"
            ElseIf "1024" = drowGrid(ii).Item("表示_項目配置").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "下右"
            Else
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "上左"
            End If

			If drowGrid(ii).Item("表示_フォーマット") Is DBNull.Value Then
				dgv(ii, EM_GRID.emTxt_Format).Value = ""
			Else
				dgv(ii, EM_GRID.emTxt_Format).Value = drowGrid(ii).Item("表示_フォーマット").ToString.Trim()
			End If
			dgv(ii, EM_GRID.emChk_Frozen).Value = drowGrid(ii).Item("表示_列固定").ToString.Trim()
			dgv(ii, EM_GRID.emCmb_TableName).Value = drowGrid(ii).Item("DB_テーブル名").ToString.Trim()

			'フィールド名取得
			Call GetCmbFieldName(ii)
			dgv(ii, EM_GRID.emCmb_Field).Value = drowGrid(ii).Item("DB_フィールド名").ToString.Trim()

			dgv(ii, EM_GRID.emChk_Key).Value = drowGrid(ii).Item("DB_主キー").ToString.Trim()
			If "0" = drowGrid(ii).Item("DB_フィールドタイプ").ToString.Trim() Then
				dgv(ii, EM_GRID.emCmb_FieldType).Value = "Int32"
			ElseIf "1" = drowGrid(ii).Item("DB_フィールドタイプ").ToString.Trim() Then
				dgv(ii, EM_GRID.emCmb_FieldType).Value = "Float"
			ElseIf "2" = drowGrid(ii).Item("DB_フィールドタイプ").ToString.Trim() Then
				dgv(ii, EM_GRID.emCmb_FieldType).Value = "文字"
			ElseIf "3" = drowGrid(ii).Item("DB_フィールドタイプ").ToString.Trim() Then
				dgv(ii, EM_GRID.emCmb_FieldType).Value = "bit"
			Else
				dgv(ii, EM_GRID.emCmb_FieldType).Value = "Int32"
			End If

			dgv(ii, EM_GRID.emTxt_InputMin).Value = drowGrid(ii).Item("入力_入力可能最小値").ToString.Trim()
			dgv(ii, EM_GRID.emTxt_InputMax).Value = drowGrid(ii).Item("入力_入力可能最大値").ToString.Trim()
			dgv(ii, EM_GRID.emChk_NullOK).Value = drowGrid(ii).Item("入力_NULL入力可否").ToString.Trim()
			If drowGrid(ii).Item("入力_入力可能桁数") Is DBNull.Value Then
				dgv(ii, EM_GRID.emTxt_InputKeta).Value = ""
			Else
				dgv(ii, EM_GRID.emTxt_InputKeta).Value = drowGrid(ii).Item("入力_入力可能桁数").ToString.Trim()
			End If
			dgv(ii, EM_GRID.emTxt_DecKeta).Value = drowGrid(ii).Item("入力_小数点以下桁数").ToString.Trim()

			If "1" = drowGrid(ii).Item("入力_表示ダイアログ").ToString.Trim() Then
				dgv(ii, EM_GRID.emCmb_DispDialog).Value = "表示"
			Else
				dgv(ii, EM_GRID.emCmb_DispDialog).Value = "表示無し"
			End If
			'列の幅を調整します
			dgv.AutoResizeColumn(ii)
		Next ii

        '新規登録用の空白列を追加
        Call AddDgvRetu(drowGrid.Length)
        'ダミー列を削除
        dgv.Columns.RemoveAt(drowGrid.Length + 1)
        ' カーソルスタート位置は入力エリアの先頭
        dgv(0, 0).Selected = True

    End Sub

    ''' <summary>
    ''' 確定処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' 入力値チェック
        If Not Data_Check() Then Exit Sub

        ''画面から削除された項目Noを取得し、最終的なDB登録用項目Noの編集を行う
        If Not Data_CheckDelete() Then Exit Sub

        Try

            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            ' 画面のデータを取得
            Data_Get()

            ' ホントに登録        
            If DB_UpDate() Then
                modMain.HardCopy(Me)

                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("正常に登録しました", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub
            Else
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("登録に失敗", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

            ' このフォームで必要なデータを取得する
            If Not DB_Select() Then
                ' DB 失敗なので、画面強制終了
                frmMsg.Text = "異常"
                frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       '終了
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        ' 画面に値を表示
        Data_Disp()
    End Sub

    ''' <summary>
    ''' 入力値のチェック
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strWk As String     ' dgvのデータ格納用
        Dim strWk2 As String    ' dgvのデータ格納用
        Dim strMsg As String    ' 重複用のエラーメッセージ

        For ii As Integer = 0 To dgvGrid.ColumnCount - 1

            '項目No
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_KNo).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, ITEM_MAX_COUNT, True, "列位置 =[" & ii + 1 & "]") <> 0 Then Return False

            ' 項目Noが空白の行は無視
            If Trim(strWk) = "" Then Continue For

            ' 項目Noの重複チェック
            For jj As Integer = ii + 1 To dgvGrid.ColumnCount - 1
                ' 比較先の項目Noが数値なら比較
                If IsNumeric(dgvGrid(jj, EM_GRID.emTxt_KNo).Value) Then
                    ' 項目Noが同じならメッセージを出して終了
                    If CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) = CInt(dgvGrid(jj, EM_GRID.emTxt_KNo).Value) Then
                        strMsg = "項目No重複" & vbNewLine
                        strMsg &= "列位置 =[" & CStr(ii + 1) & "] と 列位置=[" & CStr(jj + 1) & "]"
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            '項目ヘッダー1
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_Hedder1).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 256, "項目ヘッダー1 項目No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]", True) <> 0 Then Return False

            '項目ヘッダー2
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_Hedder2).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 256, "項目ヘッダー2 項目No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]", True) <> 0 Then Return False

            '備考
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_Bikou).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 256, "備考 項目No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]", True) <> 0 Then Return False

            '項目セルタイプ及び、lov_idの組み合わせ正常判定
            strWk = CStr(dgvGrid(ii, EM_GRID.emCmb_CellType).Value)
            strWk2 = CStr(dgvGrid(ii, EM_GRID.emCmb_Lov_id).Value)
            If "ComboBox" = strWk Then
                'ComboBox選択中の場合、lov_idが選択されていない場合、異常とする
                If "対象外" = strWk2 Then
                    strMsg = "lov_idが選択されていません。" & vbNewLine
                    strMsg &= "項目No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]"
                    MsgBox(strMsg)
                    Return False
                End If
            Else
                'ComboBox選択中でない場合、lov_idが「対象外」でない場合、異常とする
                If "対象外" <> strWk2 Then
                    strMsg = "ComboBox以外ではlov_idの選択は無効です。" & vbNewLine & "「対象外」を選択してください。" & vbNewLine
                    strMsg &= "項目No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]"
                    MsgBox(strMsg)
                    Return False
                End If
            End If

            '項目幅
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_KHaba).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, True, "項目幅 項目No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]") <> 0 Then Return False

            'フォーマット
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_Format).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "フォーマット 項目No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]", True) <> 0 Then Return False

            'フィールドタイプ
            strWk = CStr(dgvGrid(ii, EM_GRID.emCmb_FieldType).Value)
            '入力可能桁数
            strWk2 = CStr(dgvGrid(ii, EM_GRID.emTxt_InputKeta).Value)
            If "文字" <> strWk Then
                If "0" <> strWk2 And "" <> Trim(strWk2) Then
                    strMsg = "入力可能桁数は、フィールドタイプ「文字」の場合のみ有効です。" & vbNewLine
                    strMsg &= "項目No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]"
                    MsgBox(strMsg)
                    Return False
                End If
            End If
            '小数点以下桁数
            strWk2 = CStr(dgvGrid(ii, EM_GRID.emTxt_DecKeta).Value)
            If "Float" <> strWk Then
                If "0" <> strWk2 And "" <> Trim(strWk2) Then
                    strMsg = "小数点以下桁数は、フィールドタイプ「Float」の場合のみ有効です。" & vbNewLine
                    strMsg &= "項目No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]"
                    MsgBox(strMsg)
                    Return False
                End If
            End If

            'フィールド名
            strWk = CStr(dgvGrid(ii, EM_GRID.emCmb_Field).Value)
            If "" = CStr(dgvGrid(ii, EM_GRID.emCmb_Field).Value) Then
                strMsg = "フィールド名が選択されていません。" & vbNewLine
                strMsg &= "項目No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]"
                MsgBox(strMsg)
                Return False
            End If

            '入力可能最小値
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_InputMin).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, Int32.MinValue, Int32.MaxValue, True, "入力可能最小値 項目No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]") <> 0 Then Return False

            '入力可能最大値
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_InputMax).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, Int32.MinValue, Int32.MaxValue, True, "入力可能最大値 項目No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]") <> 0 Then Return False

            '入力可能桁数
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_InputKeta).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, True, "入力可能桁数 項目No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]") <> 0 Then Return False

            '小数点以下桁数
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_DecKeta).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, True, "小数点以下桁数 項目No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]") <> 0 Then Return False


        Next ii

        Return True
    End Function

    ''' <summary>
    ''' 削除された項目があるかチェックする
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Data_CheckDelete() As Boolean
        Dim bCheck As Boolean   'チェックフラグ TrueならEdasysIDがある
        Dim nCount As Integer   'カウンタ
        Dim nPos As Integer     '位置
        Dim nMaxItem As Integer '最大項目No
        Dim nMinItem As Integer '最小項目No
        Dim bChk As Boolean     'チェック結果

        '表示中有効項目が1件も無い場合、チェックを行わない
        nCount = 0
        For ii As Integer = 0 To dgvGrid.ColumnCount - 1
            If IsNullOrEmptyOrSpace(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) = False _
               AndAlso "" <> CStr(dgvGrid(ii, EM_GRID.emTxt_KNo).Value).Trim Then
                nCount = nCount + 1
            End If
        Next ii
        If 0 = nCount Then Return True

        nCount = 0
        ' テーブルで全行ループ
        For ii As Integer = 0 To m_Grid.Tables(0).Rows.Count - 1
            If m_Grid.Tables(0).Rows(ii).Item("画面ID").ToString = m_DBlst(m_KeyGID).strDBGID Then
                If CInt(m_Grid.Tables(0).Rows(ii).Item("グリッドID")) = m_DBlst(m_KeyGID).listGridID(cmbGridID.Text) Then
                    '画面ID及びグリッドIDが一致する箇所を対象とする
                    bCheck = False
                    m_nDelItem(nCount) = 0
                    ' dgvで全行ループ
                    For kk As Integer = 0 To dgvGrid.ColumnCount - 1
                        ' 項目Noが空白行以外で、比較
                        If IsNullOrEmptyOrSpace(dgvGrid(kk, EM_GRID.emTxt_KNo).Value) Then Continue For
                        If CInt(dgvGrid(kk, EM_GRID.emTxt_KNo).Value) = CInt(m_Grid.Tables(0).Rows(ii).Item("項目No")) Then
                            ' 項目Noが一致したとき、チェックOKとする
                            bCheck = True
                            Exit For
                        End If
                    Next kk
                    ' 一致する項目Noが存在しない場合、削除されたと判定する
                    If bCheck = False Then
                        m_nDelItem(nCount) = CInt(m_Grid.Tables(0).Rows(ii).Item("項目No"))
                        nCount = nCount + 1
                    End If
                End If
            End If
        Next ii

        For ii As Integer = 0 To dgvGrid.ColumnCount - 1

            ' 項目Noが空白行であれば未設定
            If IsNullOrEmptyOrSpace(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) Then
                m_nDispItem(ii) = 0
            Else
                m_nDispItem(ii) = CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value)
            End If

        Next ii
        For ii As Integer = 0 To nCount - 1
            '大きい項目Noから前につめる
            nPos = nCount - 1 - ii
            For kk As Integer = 0 To dgvGrid.ColumnCount - 1
                If IsNullOrEmptyOrSpace(dgvGrid(kk, EM_GRID.emTxt_KNo).Value) = False _
                    AndAlso CInt(dgvGrid(kk, EM_GRID.emTxt_KNo).Value) > m_nDelItem(nPos) Then
                    '並び替えた結果をワークに記憶していく
                    'm_nDispItem(ii) = CInt(dgvGrid(kk, EM_GRID.emTxt_KNo).Value) - 1
                    m_nDispItem(kk) = m_nDispItem(kk) - 1
                End If
            Next kk
        Next ii

        'nMinItem = m_nDispItem(0)
        nMinItem = ITEM_MAX_COUNT
        nMaxItem = 0

        '並び替えた結果の項目No最小値と最大値を取得
        For ii As Integer = 0 To dgvGrid.ColumnCount - 1
            If 0 >= m_nDispItem(ii) Then Continue For
            If nMinItem > m_nDispItem(ii) Then
                nMinItem = m_nDispItem(ii)
            End If
            If nMaxItem < m_nDispItem(ii) Then
                nMaxItem = m_nDispItem(ii)
            End If
        Next ii

        '並び替えた結果の項目No最小値が1でない場合、異常とする
        If nMinItem <> 1 Then
            MsgBox("「項目No」は1から指定してください。")
            Return False
        End If

        '次項目Noが連続する数値かチェックを行う
        For ii As Integer = 0 To dgvGrid.ColumnCount - 1
            bChk = False
            '並び替えた結果の項目No最大値が対象の場合、次項目Noのチェックを行わない
            If m_nDispItem(ii) = nMaxItem Then Continue For
            For kk As Integer = 0 To dgvGrid.ColumnCount - 1
                If m_nDispItem(ii) + 1 = m_nDispItem(kk) Then
                    bChk = True
                    Exit For
                End If
            Next kk
            '項目Noが連続しない場合、異常とする
            If False = bChk Then
                MsgBox("「項目No」は連続する数値で指定してください。")
                Return False
            End If
        Next ii

        Return True

    End Function

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()
        ' 非接続テーブルで全行ループ
        For ii As Integer = 0 To m_Grid.Tables(0).Rows.Count - 1
            If m_Grid.Tables(0).Rows(ii).Item("画面ID").ToString = m_DBlst(m_KeyGID).strDBGID Then
                If CInt(m_Grid.Tables(0).Rows(ii).Item("グリッドID")) = m_DBlst(m_KeyGID).listGridID(m_KeyGridID) Then
                    '画面ID及びグリッドIDが一致する箇所を対象とする
                    ' デリートインサートする為、今もっている行をクリアする
                    m_Grid.Tables(0).Rows(ii).Delete()
                End If
            End If
        Next ii

        ' dgvで全行ループ
        For ii As Integer = 0 To dgvGrid.ColumnCount - 1
            ' 項目Noが空白の場合、スキップする
            If IsNullOrEmptyOrSpace(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) Then Continue For
            Dim drowGrid As DataRow = m_Grid.Tables(0).NewRow()                                                ' 新規行

            drowGrid.Item("画面ID") = m_DBlst(m_KeyGID).strDBGID
            drowGrid.Item("グリッドID") = m_DBlst(cmbGID.Text).listGridID(m_KeyGridID)

            '表示中情報をセット
            drowGrid.Item("項目No") = m_nDispItem(ii)
            drowGrid.Item("項目ヘッダー1") = CStr(dgvGrid(ii, EM_GRID.emTxt_Hedder1).Value)
            drowGrid.Item("項目ヘッダー2") = CStr(dgvGrid(ii, EM_GRID.emTxt_Hedder2).Value)
            drowGrid.Item("備考") = CStr(dgvGrid(ii, EM_GRID.emTxt_Bikou).Value)

            If "TextBox" = CStr(dgvGrid(ii, EM_GRID.emCmb_CellType).Value) Then
                drowGrid.Item("項目セルタイプ") = 0
            ElseIf "ComboBox" = CStr(dgvGrid(ii, EM_GRID.emCmb_CellType).Value) Then
                drowGrid.Item("項目セルタイプ") = 1
            ElseIf "CheckBox" = CStr(dgvGrid(ii, EM_GRID.emCmb_CellType).Value) Then
                drowGrid.Item("項目セルタイプ") = 2
            ElseIf "Button" = CStr(dgvGrid(ii, EM_GRID.emCmb_CellType).Value) Then
                drowGrid.Item("項目セルタイプ") = 3
            Else
                drowGrid.Item("項目セルタイプ") = 0
            End If

            If "対象外" = CStr(dgvGrid(ii, EM_GRID.emCmb_Lov_id).Value) Then
				drowGrid.Item("lov_id") = System.DBNull.Value
            Else
                drowGrid.Item("lov_id") = CInt(dgvGrid(ii, EM_GRID.emCmb_Lov_id).Value)
            End If

            If CBool(dgvGrid(ii, EM_GRID.emChk_DispUmu).Value) Then
                drowGrid.Item("表示_項目表示有無") = 1
            Else
                drowGrid.Item("表示_項目表示有無") = 0
            End If

            If CBool(dgvGrid(ii, EM_GRID.emChk_ReadOnly).Value) Then
                drowGrid.Item("表示_読み取り専用") = 1
            Else
                drowGrid.Item("表示_読み取り専用") = 0
            End If
			If "" = CStr(dgvGrid(ii, EM_GRID.emTxt_KHaba).Value) Then
				drowGrid.Item("表示_項目幅") = System.DBNull.Value
			Else
				drowGrid.Item("表示_項目幅") = CInt(dgvGrid(ii, EM_GRID.emTxt_KHaba).Value)
			End If

            If "上左" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("表示_項目配置") = 1
            ElseIf "上中" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("表示_項目配置") = 2
            ElseIf "上右" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("表示_項目配置") = 4
            ElseIf "中左" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("表示_項目配置") = 16
            ElseIf "中中" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("表示_項目配置") = 32
            ElseIf "中右" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("表示_項目配置") = 64
            ElseIf "下左" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("表示_項目配置") = 256
            ElseIf "下中" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("表示_項目配置") = 512
            ElseIf "下右" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("表示_項目配置") = 1024
            Else
                drowGrid.Item("表示_項目配置") = 1
            End If

			If "" = CStr(dgvGrid(ii, EM_GRID.emTxt_Format).Value) Then
				drowGrid.Item("表示_フォーマット") = System.DBNull.Value
			Else
				drowGrid.Item("表示_フォーマット") = CStr(dgvGrid(ii, EM_GRID.emTxt_Format).Value)
			End If

			If CBool(dgvGrid(ii, EM_GRID.emChk_Frozen).Value) Then
				drowGrid.Item("表示_列固定") = 1
			Else
				drowGrid.Item("表示_列固定") = 0
			End If

			drowGrid.Item("DB_テーブル名") = CStr(dgvGrid(ii, EM_GRID.emCmb_TableName).Value)
			drowGrid.Item("DB_フィールド名") = CStr(dgvGrid(ii, EM_GRID.emCmb_Field).Value)

			If CBool(dgvGrid(ii, EM_GRID.emChk_Key).Value) Then
				drowGrid.Item("DB_主キー") = 1
			Else
				drowGrid.Item("DB_主キー") = 0
			End If

			If "Int32" = CStr(dgvGrid(ii, EM_GRID.emCmb_FieldType).Value) Then
				drowGrid.Item("DB_フィールドタイプ") = 0
			ElseIf "Float" = CStr(dgvGrid(ii, EM_GRID.emCmb_FieldType).Value) Then
				drowGrid.Item("DB_フィールドタイプ") = 1
			ElseIf "文字" = CStr(dgvGrid(ii, EM_GRID.emCmb_FieldType).Value) Then
				drowGrid.Item("DB_フィールドタイプ") = 2
			ElseIf "bit" = CStr(dgvGrid(ii, EM_GRID.emCmb_FieldType).Value) Then
				drowGrid.Item("DB_フィールドタイプ") = 3
			Else
				drowGrid.Item("DB_フィールドタイプ") = 0
			End If

			If "" = CStr(dgvGrid(ii, EM_GRID.emTxt_InputMin).Value) Then
				drowGrid.Item("入力_入力可能最小値") = System.DBNull.Value
			Else
				drowGrid.Item("入力_入力可能最小値") = CDbl(dgvGrid(ii, EM_GRID.emTxt_InputMin).Value)
			End If
			If "" = CStr(dgvGrid(ii, EM_GRID.emTxt_InputMax).Value) Then
				drowGrid.Item("入力_入力可能最大値") = System.DBNull.Value
			Else
				drowGrid.Item("入力_入力可能最大値") = CDbl(dgvGrid(ii, EM_GRID.emTxt_InputMax).Value)
			End If

			If CBool(dgvGrid(ii, EM_GRID.emChk_NullOK).Value) Then
				drowGrid.Item("入力_NULL入力可否") = 1
			Else
				drowGrid.Item("入力_NULL入力可否") = 0
			End If

			If "" = CStr(dgvGrid(ii, EM_GRID.emTxt_InputKeta).Value) Then
				drowGrid.Item("入力_入力可能桁数") = System.DBNull.Value
			Else
				drowGrid.Item("入力_入力可能桁数") = CInt(dgvGrid(ii, EM_GRID.emTxt_InputKeta).Value)
			End If
			If "" = CStr(dgvGrid(ii, EM_GRID.emTxt_DecKeta).Value) Then
				drowGrid.Item("入力_小数点以下桁数") = System.DBNull.Value
			Else
				drowGrid.Item("入力_小数点以下桁数") = CInt(dgvGrid(ii, EM_GRID.emTxt_DecKeta).Value)
			End If

			If "表示" = CStr(dgvGrid(ii, EM_GRID.emCmb_DispDialog).Value) Then
				drowGrid.Item("入力_表示ダイアログ") = 1
			Else
				drowGrid.Item("入力_表示ダイアログ") = 0
			End If

			m_Grid.Tables(0).Rows.Add(drowGrid)		'行を追加
		Next ii
    End Sub

    ''' <summary>
    ''' アップデート
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim transact As SqlClient.SqlTransaction = Nothing   'トランザクション処理

        Try
            ' DB接続
            m_AdapterGrid.SelectCommand.Connection.Open()

            'トランザクション開始
            transact = m_AdapterGrid.SelectCommand.Connection.BeginTransaction()
            m_AdapterGrid.SelectCommand.Transaction = transact

            Using cbWk As New SqlClient.SqlCommandBuilder()

                'コマンドアダプタをセットして、更新
                cbWk.DataAdapter = m_AdapterGrid
                m_AdapterGrid.Update(m_Grid)

            End Using

            'コミット
            transact.Commit()

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "設定エラー [" & ex.Message & "] " & Me.Text)

            ' エラーならロールバックして終了
            If Not transact Is Nothing Then
                transact.Rollback()
            End If

            Return False
        Finally
            ' 後始末
            If m_AdapterGrid.SelectCommand.Connection.State = ConnectionState.Open Then
                m_AdapterGrid.SelectCommand.Connection.Close()
            End If

            If Not transact Is Nothing Then
                transact.Dispose()
            End If
        End Try
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 関連
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' dgvの列定義
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub dgvInitializeGrid(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                       ' 選択色無し

        With dgv
            .ColumnHeadersHeight = 42                                   ' ヘッダーの高さ
            .AllowUserToResizeColumns = True                            ' ユーザー 列サイズ変更
            .ScrollBars = ScrollBars.Both                               ' スクロールバーの表示

            ' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black
            '行ヘッダー設定
            .RowHeadersWidth = 140                                   ' ヘッダーの横幅
            .RowHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .RowHeadersDefaultCellStyle.ForeColor = Color.Black

            '' 選択セル色定義
            '.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      ' 選択色
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     ' 一行おきに選択色変更
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            .ColumnHeadersVisible = False
            .RowHeadersVisible = True

        End With

        ''列定義
        ' ダミー列
		dgvColSetText(dgv, "", 140, True, False, _
  DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

    End Sub

    ''' <summary>
    ''' dgvの選択が変更されたら、セルの選択状態をキャンセル
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dataGridView_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' 対象のdgv
        dgv.CurrentCell.Selected = False
    End Sub

    ''' <summary>
    ''' 更新ボタンの有効/無効
    ''' </summary>
    ''' <param name="bPassResult">パスワード入力結果 (True:有効 False:無効)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' ファンクションキーの設定ボタン

    End Sub

    ''' <summary>
    ''' データグリッド チェックボックスセル登録
    ''' </summary>
    ''' <param name="nPos">列位置</param>
    ''' <param name="nIndex">行位置</param>
    ''' <remarks></remarks>
    Private Sub CellSetDataGridViewCheckBox(ByVal nPos As Integer, ByVal nIndex As Integer)

        Using clm As New DataGridViewCheckBoxCell
            Dim wkCell As DataGridViewCheckBoxCell = CType(clm, DataGridViewCheckBoxCell)
            wkCell.Style.Font = New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            wkCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            dgvGrid(nPos, nIndex) = clm
            If EM_GRID.emChk_Key = nIndex Then
                dgvGrid(nPos, nIndex).ReadOnly = True
            ElseIf EM_GRID.emChk_NullOK = nIndex Then
                dgvGrid(nPos, nIndex).ReadOnly = True
            End If
        End Using
    End Sub

    ''' <summary>
    ''' データグリッド テキストボックスセル登録
    ''' </summary>
    ''' <param name="nPos">列位置</param>
    ''' <param name="nIndex">行位置</param>
    ''' <param name="nType">表示型指定(1:数値 以外:文字)</param>
    ''' <remarks></remarks>
    Private Sub CellSetDataGridViewTextBox(ByVal nPos As Integer, ByVal nIndex As Integer, Optional ByVal nType As Integer = Nothing)

        Using clm As New DataGridViewTextBoxCell
            Dim wkCell As DataGridViewTextBoxCell = CType(clm, DataGridViewTextBoxCell)
            wkCell.Style.Font = New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            If 1 = nType Then
                '数値の場合、右寄せ表示
                wkCell.Style.Alignment = DataGridViewContentAlignment.MiddleRight
            Else
                '文字の場合、左寄せ表示
                wkCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            End If
            dgvGrid(nPos, nIndex) = clm
        End Using
    End Sub

    ''' <summary>
    ''' データグリッド コンボボックスセル登録
    ''' </summary>
    ''' <param name="nPos">列位置</param>
    ''' <param name="nIndex">行位置</param>
    ''' <remarks></remarks>
    Private Sub CellSetDataGridViewComboBox(ByVal nPos As Integer, ByVal nIndex As Integer)

        Using clm As New DataGridViewComboBoxCell
            Dim wkCell As DataGridViewComboBoxCell = CType(clm, DataGridViewComboBoxCell)
            Dim ii As Integer
            wkCell.Style.Font = New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            wkCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            Dim listItemFF As DataGridViewComboBoxCell = CType(wkCell, DataGridViewComboBoxCell)

            If EM_GRID.emCmb_CellType = nIndex Then
                '項目セルタイプ
                listItemFF.Items.AddRange("TextBox", "ComboBox", "CheckBox", "Button")
            ElseIf EM_GRID.emCmb_Lov_id = nIndex Then
                'lov_id
                Dim drowLov_id() As DataRow = m_Lov_id.Tables(0).Select()                   ' Lov_id
                listItemFF.Items.Add("対象外")
                'DB上に登録されているlov_idを選択項目とする
                For ii = 0 To drowLov_id.Length - 1
                    listItemFF.Items.Add(drowLov_id(ii).Item("lov_id").ToString)
                Next ii
            ElseIf EM_GRID.emCmb_KHaiti = nIndex Then
                '項目配置
                listItemFF.Items.AddRange("上左", "上中", "上右", "中左", "中中", "中右", "下左", "下中", "下右")
            ElseIf EM_GRID.emCmb_TableName = nIndex Then
                'テーブル名
                ' <20221117> <小島> <最終工程高機能化その1> --->>>
                'listItemFF.Items.AddRange(DB_PARAM_CAMANGLE_SCODE, DB_PARAM_CAMSET, DB_PARAM_CAMSET_ANGLE, DB_PARAM_CAMSET_ANGLE_SCODE, DB_PARAM_CAMSET_SCODE, DB_PARAM_COMMON, DB_PARAM_SCODE)
                listItemFF.Items.AddRange(DB_PARAM_CAMANGLE_SCODE, DB_PARAM_CAMSET, DB_PARAM_CAMSET_ANGLE, DB_PARAM_CAMSET_ANGLE_SCODE, DB_PARAM_CAMSET_SCODE, DB_PARAM_COMMON, DB_PARAM_SCODE, DB_PARAM_FILTERBIN, DB_PARAM_LABELIMAGE, DB_PARAM_SIMPLEBIN)
                ' <20221117> <小島> <最終工程高機能化その1> ---<<<
                'listItemFF.Items.AddRange(DB_PARAM_CAMANGLE_SCODE, DB_PARAM_CAMSET, DB_PARAM_SIDE, DB_PARAM_SIDE_SCODE, DB_PARAM_CAMSET_SCODE, DB_PARAM_COMMON, DB_PARAM_SCODE)
            ElseIf EM_GRID.emCmb_FieldType = nIndex Then
				'フィールドタイプ
				listItemFF.Items.AddRange("Int32", "Float", "文字", "bit")
            ElseIf EM_GRID.emCmb_DispDialog = nIndex Then
                '表示ダイアログ
                listItemFF.Items.AddRange("表示", "表示無し")
            End If
            '''''''''''''''''''''''''''''''''''''''''''''''
            dgvGrid(nPos, nIndex) = clm
            If EM_GRID.emCmb_FieldType = nIndex Then
                dgvGrid(nPos, nIndex).ReadOnly = True
            End If
        End Using
    End Sub

    ''' <summary>
    ''' データグリッド表示項目変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGrid_CellValueChanged(ByVal sender As Object, ByVal e As DataGridViewCellEventArgs) Handles dgvGrid.CellValueChanged
        Dim nWkx As Integer = 0
        If 0 > e.ColumnIndex Then Exit Sub
        '項目セルタイプを変更した場合、lov_idを取得する
        If EM_GRID.emCmb_CellType = e.RowIndex Then
            Call SetCmbCellTypeChgData(e.ColumnIndex)
        End If
        'テーブル名を変更した場合、フィールド名を作り直す
        If EM_GRID.emCmb_TableName = e.RowIndex Then
            Call GetCmbFieldName(e.ColumnIndex)
        End If
        'フィールド名を変更した場合、主キー、フィールドタイプ、NULL入力可否を取得する
        If EM_GRID.emCmb_Field = e.RowIndex Then
            Call SetCmbFieldChgData(e.ColumnIndex)
        End If
        'フィールドタイプを変更した場合、入力可能桁数及び小数点以下桁数を取得する
        If EM_GRID.emCmb_FieldType = e.RowIndex Then
            Call SetCmbFieldTypeChgData(e.ColumnIndex)
        End If
        '最終行のグリッドIDを追加した場合、次の入力エリアを追加する
        If EM_GRID.emTxt_KNo = e.RowIndex And (dgvGrid.Columns.Count - 1) = e.ColumnIndex Then
            Call AddDgvRetu(e.ColumnIndex + 1)
        End If
    End Sub
    ''' <summary>
    ''' データグリッド上に列を追加する
    ''' </summary>
    ''' <param name="nIndex">追加位置</param>
    ''' <remarks></remarks>
    Private Sub AddDgvRetu(ByVal nIndex As Integer)
        '新規列を追加
		dgvGrid.Columns.Add(CStr(nIndex), CStr(nIndex))
		''横幅は自動調整する
		'dgvGrid.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.DisplayedCellsExceptHeader
		'セル情報を定義
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_KNo, 1)            '項目No
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_Hedder1)           '項目ヘッダー１
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_Hedder2)           '項目ヘッダー２
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_Bikou)             '備考
        CellSetDataGridViewComboBox(nIndex, EM_GRID.emCmb_CellType)         '項目セルタイプ
        CellSetDataGridViewComboBox(nIndex, EM_GRID.emCmb_Lov_id)           'lov_id
        CellSetDataGridViewCheckBox(nIndex, EM_GRID.emChk_DispUmu)          '項目表示有無
        CellSetDataGridViewCheckBox(nIndex, EM_GRID.emChk_ReadOnly)         '読み取り専用
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_KHaba, 1)          '項目幅
        CellSetDataGridViewComboBox(nIndex, EM_GRID.emCmb_KHaiti)           '項目配置
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_Format)            'フォーマット
        CellSetDataGridViewCheckBox(nIndex, EM_GRID.emChk_Frozen)           '列固定
        CellSetDataGridViewComboBox(nIndex, EM_GRID.emCmb_TableName)        'DBテーブル名
        'DBフィールド名はここでは定義しない(DBテーブル名を表示後に定義する)
        'CellSetDataGridViewComboBox(nIndex, EM_GRID.emCmb_Field)            '
        CellSetDataGridViewCheckBox(nIndex, EM_GRID.emChk_Key)              'DB主キー
        CellSetDataGridViewComboBox(nIndex, EM_GRID.emCmb_FieldType)        'DBフィールドタイプ
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_InputMin, 1)       '最小値
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_InputMax, 1)       '最大値
        CellSetDataGridViewCheckBox(nIndex, EM_GRID.emChk_NullOK)           'NULL入力可否
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_InputKeta, 1)      '入力可能桁数
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_DecKeta, 1)        '小数点以下桁数
        CellSetDataGridViewComboBox(nIndex, EM_GRID.emCmb_DispDialog)       '表示ダイアログ
        '初期値セット
        dgvGrid(nIndex, EM_GRID.emCmb_CellType).Value = "TextBox"
        dgvGrid(nIndex, EM_GRID.emCmb_Lov_id).Value = "対象外"
        dgvGrid(nIndex, EM_GRID.emChk_DispUmu).Value = False
        dgvGrid(nIndex, EM_GRID.emChk_ReadOnly).Value = False
        dgvGrid(nIndex, EM_GRID.emTxt_KHaba).Value = 0
        dgvGrid(nIndex, EM_GRID.emCmb_KHaiti).Value = "上左"
        dgvGrid(nIndex, EM_GRID.emChk_Frozen).Value = False
        dgvGrid(nIndex, EM_GRID.emCmb_TableName).Value = "PARAM_CAMSET"
        dgvGrid(nIndex, EM_GRID.emCmb_Field).Value = "camset"
        dgvGrid(nIndex, EM_GRID.emChk_Key).Value = True
        dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value = "Int32"
        dgvGrid(nIndex, EM_GRID.emChk_NullOK).Value = False
        dgvGrid(nIndex, EM_GRID.emTxt_InputKeta).Value = 0
        dgvGrid(nIndex, EM_GRID.emTxt_DecKeta).Value = 0
        dgvGrid(nIndex, EM_GRID.emCmb_DispDialog).Value = "表示無し"
    End Sub

    ''' <summary>
    ''' 画面IDコンボボックス初期処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetComboBox_GID()

        '画面IDコンボボックスのﾒﾝﾊﾞｰを登録する
        '固定パラメータ設定画面
        If "" <> EPC_COMMON_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_COMMON_GRID_NAME, EPC_COMMON_GRID_GID, 1))
        End If
        'ロジックパララン設定画面
        If "" <> EPC_PARA_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_PARA_GRID_NAME, EPC_PARA_GRID_GID, 1))
        End If
        'カメラ設定画面
        If "" <> EPC_CAMERA_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_CAMERA_GRID_NAME, EPC_CAMERA_GRID_GID, 1))
        End If
		'混色分離設定画面
		If "" <> EPC_MIXCOLOR_GRID_GID Then
			cmbGID.Items.Add(New cls_CmbItem(EPC_MIXCOLOR_GRID_NAME, EPC_MIXCOLOR_GRID_GID, 1))
		End If
		'エッジ検出設定画面
        If "" <> EPC_EDGE_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_EDGE_GRID_NAME, EPC_EDGE_GRID_GID, 1))
        End If
        'ｼｪｰﾃﾞｨﾝｸﾞ補正設定画面
        If "" <> EPC_SHADING_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_SHADING_GRID_NAME, EPC_SHADING_GRID_GID, 1))
        End If
        '閾値設定画面
        If "" <> EPC_THRESHOLD_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_THRESHOLD_GRID_NAME, EPC_THRESHOLD_GRID_GID, 1))
        End If
        '疵検出設定画面
        If "" <> EPC_DEFECT_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_DEFECT_GRID_NAME, EPC_DEFECT_GRID_GID, 1))
        End If
        '総合処理設定画面
        If "" <> EPC_MERGE_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_MERGE_GRID_NAME, EPC_MERGE_GRID_GID, 1))
        End If
        ' <20221117> <小島> <最終工程高機能化その1> --->>>
        'ＥＰＣ多重分解能閾値設定画面
        If "" <> EPC_TAJUUTHRESHOLD_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_TAJUUTHRESHOLD_GRID_NAME, EPC_TAJUUTHRESHOLD_GRID_GID, 1))
        End If
        ' <20221117> <小島> <最終工程高機能化その1> ---<<<

        '初期選択状態をセットする
        cmbGID.Text = GRID_FIRST_DISP_NAME
        cmbGID.SelectedValue = GRID_FIRST_DISP_GID

        '-------------------------------------
        '画面表示グリッドID取得用情報を作成する
        '-------------------------------------
        ' 固定パラメータ設定画面
        SetComboBoxListData(EPC_COMMON_GRID, EPC_COMMON_GRID_ID, EPC_COMMON_GRID_CNT, EPC_COMMON_GRID_GID, EPC_COMMON_GRID_NAME)
        '-------------------------------------
        ' ロジックパララン設定画面
        SetComboBoxListData(EPC_PARA_GRID, EPC_PARA_GRID_ID, EPC_PARA_GRID_CNT, EPC_PARA_GRID_GID, EPC_PARA_GRID_NAME)
        '-------------------------------------
        ' カメラ設定画面
        SetComboBoxListData(EPC_CAMERA_GRID, EPC_CAMERA_GRID_ID, EPC_CAMERA_GRID_CNT, EPC_CAMERA_GRID_GID, EPC_CAMERA_GRID_NAME)
		'-------------------------------------
		' 混色分離設定画面
		SetComboBoxListData(EPC_MIXCOLOR_GRID, EPC_MIXCOLOR_GRID_ID, EPC_MIXCOLOR_GRID_CNT, EPC_MIXCOLOR_GRID_GID, EPC_MIXCOLOR_GRID_NAME)
		'-------------------------------------
        'エッジ検出設定画面
        SetComboBoxListData(EPC_EDGE_GRID, EPC_EDGE_GRID_ID, EPC_EDGE_GRID_CNT, EPC_EDGE_GRID_GID, EPC_EDGE_GRID_NAME)
        '-------------------------------------
        'ｼｪｰﾃﾞｨﾝｸﾞ補正設定画面
        SetComboBoxListData(EPC_SHADING_GRID, EPC_SHADING_GRID_ID, EPC_SHADING_GRID_CNT, EPC_SHADING_GRID_GID, EPC_SHADING_GRID_NAME)
        '-------------------------------------
        '閾値設定画面
        SetComboBoxListData(EPC_THRESHOLD_GRID, EPC_THRESHOLD_GRID_ID, EPC_THRESHOLD_GRID_CNT, EPC_THRESHOLD_GRID_GID, EPC_THRESHOLD_GRID_NAME)
        '-------------------------------------
        '疵検出設定画面
        SetComboBoxListData(EPC_DEFECT_GRID, EPC_DEFECT_GRID_ID, EPC_DEFECT_GRID_CNT, EPC_DEFECT_GRID_GID, EPC_DEFECT_GRID_NAME)
        '-------------------------------------
        '総合処理設定画面
        SetComboBoxListData(EPC_MERGE_GRID, EPC_MERGE_GRID_ID, EPC_MERGE_GRID_CNT, EPC_MERGE_GRID_GID, EPC_MERGE_GRID_NAME)
        '-------------------------------------
        ' <20221117> <小島> <最終工程高機能化その1> --->>>
        'ＥＰＣ多重分解能閾値設定画面
        SetComboBoxListData(EPC_TAJUUTHRESHOLD_GRID, EPC_TAJUUTHRESHOLD_GRID_ID, EPC_TAJUUTHRESHOLD_GRID_CNT, EPC_TAJUUTHRESHOLD_GRID_GID, EPC_TAJUUTHRESHOLD_GRID_NAME)
        ' <20221117> <小島> <最終工程高機能化その1> ---<<<

    End Sub

    ''' <summary>
    ''' 画面表示グリッドID取得用情報を作成する
    ''' </summary>
    ''' <param name="strGrid">グリッドＩＤ画面表示名称</param>
    ''' <param name="strGridID">グリッドＩＤ</param>
    ''' <param name="nGridCnt">グリッド数</param>
    ''' <param name="strGID">画面ＩＤ</param>
    ''' <param name="strGridName">画面ＩＤ画面表示名称</param>
    ''' <remarks></remarks>
    Private Sub SetComboBoxListData(ByVal strGrid As String, ByVal strGridID As String, ByVal nGridCnt As Integer, ByVal strGID As String, ByVal strGridName As String)
        Dim strWk As String
        Dim strWklst As String()                                    '画面表示グリッドID配列情報取得ワーク
        Dim strWklst2 As String()                                   'DBグリッドID配列情報取得ワーク
        Dim gid_inf As New GID_INFO                                 '画面IDに紐付く情報格納エリア
        Dim listGrid As New Dictionary(Of Integer, String)          'グリッドIDをKEYとした画面表示グリッドID取得用情報
        Dim listGridID As New Dictionary(Of String, Integer)        '画面表示グリッドIDをKEYとしたグリッドID取得用情報

        strWk = strGrid
        strWklst = strWk.Split(","c)
        strWk = strGridID
        strWklst2 = strWk.Split(","c)
        For ii As Integer = 0 To nGridCnt - 1
            listGrid.Add(ii + 1, strWklst(ii))
            listGridID.Add(strWklst(ii), CInt(strWklst2(ii)))
        Next ii
        gid_inf.strDBGID = strGID
        gid_inf.nGridCnt = nGridCnt
        gid_inf.listGrid = listGrid
        gid_inf.listGridID = listGridID
        m_DBlst.Add(strGridName, gid_inf)
    End Sub
    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    ''' <summary>
    ''' テーブル別フィールド名取得処理
    ''' </summary>
    ''' <param name="nIndex"></param>
    ''' <remarks></remarks>
    Private Sub GetCmbFieldName(ByVal nIndex As Integer)
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""    'エラーメッセージ
        Dim strDBName As String         'テーブル名

        'テーブル名を取得
        strDBName = CStr(dgvGrid(nIndex, EM_GRID.emCmb_TableName).Value)
        strSQL = "SELECT name FROM dbo.syscolumns"
        strSQL &= " WHERE ID IN("
        strSQL &= " SELECT ID FROM dbo.sysobjects"
        strSQL &= " WHERE NAME = '" & strDBName & "' )"
        strSQL &= " ORDER BY ID"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_Field, m_AdapterField, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, strDBName & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return
        End If

        Dim drowField() As DataRow = m_Field.Tables(0).Select()          ' 全レコード

        '取得したフィールド名をコンボボックスに登録する
        Using clm As New DataGridViewComboBoxCell
            Dim wkCell As DataGridViewComboBoxCell = CType(clm, DataGridViewComboBoxCell)
            wkCell.Style.Font = New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            wkCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            Dim listItemFF As DataGridViewComboBoxCell = CType(wkCell, DataGridViewComboBoxCell)
            For jj As Integer = 0 To drowField.Length - 1
                listItemFF.Items.Add(drowField(jj).Item("name").ToString)
            Next jj
            dgvGrid(nIndex, EM_GRID.emCmb_Field).Value = ""
            dgvGrid(nIndex, EM_GRID.emCmb_Field) = clm
        End Using
    End Sub

    ''' <summary>
    ''' フィールド名変更時フィールド名に対応する情報を取得し表示する
    ''' </summary>
    ''' <param name="nIndex">列位置</param>
    ''' <remarks></remarks>
    Private Sub SetCmbFieldChgData(ByVal nIndex As Integer)
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""    'エラーメッセージ
        Dim strDBName As String         'テーブル名
        Dim strField As String          'フィールド名

        If m_bDispFlg Then Exit Sub '表示処理中の場合は受付を無視する        

        'テーブル名がない場合、処理を行わない
        If "" = CStr(dgvGrid(nIndex, EM_GRID.emCmb_TableName).Value) Then Exit Sub
        'フィールド名がない場合、処理を行わない
        If "" = CStr(dgvGrid(nIndex, EM_GRID.emCmb_Field).Value) Then Exit Sub

        'テーブル名を取得
        strDBName = CStr(dgvGrid(nIndex, EM_GRID.emCmb_TableName).Value)
        strSQL = "SELECT "
        strSQL &= " cols.name AS col_name"
        strSQL &= " FROM"
        strSQL &= " sys.tables AS tbls"
        strSQL &= " INNER JOIN sys.key_constraints AS key_const ON"
        strSQL &= " tbls.object_id = key_const.parent_object_id"
        strSQL &= " AND key_const.type = 'PK'"
        strSQL &= " AND tbls.name = '" & strDBName & "'"
        strSQL &= " INNER JOIN sys.index_columns AS idx_cols ON"
        strSQL &= " key_const.parent_object_id = idx_cols.object_id"
        strSQL &= " AND key_const.unique_index_id = idx_cols.index_id"
        strSQL &= " INNER JOIN sys.columns AS cols ON"
        strSQL &= " idx_cols.object_id = cols.object_id"
        strSQL &= " AND idx_cols.column_id = cols.column_id"
        '''''''''''''''''''''''''''''
        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DBKey, m_AdapterDBKey, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, strDBName & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return
        End If

        Dim drowDBKey() As DataRow = m_DBKey.Tables(0).Select()          ' 全レコード
        dgvGrid(nIndex, EM_GRID.emChk_Key).Value = False
        For ii As Integer = 0 To drowDBKey.Length - 1
            '主キー登録されているフィールド名と一致する場合
            If drowDBKey(ii).Item("col_name").ToString = CStr(dgvGrid(nIndex, EM_GRID.emCmb_Field).Value) Then
                '主キーとする
                dgvGrid(nIndex, EM_GRID.emChk_Key).Value = True
            End If
        Next ii

        'フィールド名を取得
        strField = CStr(dgvGrid(nIndex, EM_GRID.emCmb_Field).Value)
        'NULL入力可否及びフィールドタイプを取得する
        strSQL = "SELECT is_nullable, data_type FROM information_schema.columns"
        strSQL &= " WHERE table_name = '" & strDBName & "'"
        strSQL &= " AND column_name = '" & strField & "'"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DBAttr, m_AdapterDBAttr, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, strDBName & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return
        End If

        Dim drowDBAttr() As DataRow = m_DBAttr.Tables(0).Select()          ' レコード

        'NULL入力可否を取得
        If drowDBAttr(0).Item("is_nullable").ToString = "YES" Then
            dgvGrid(nIndex, EM_GRID.emChk_NullOK).Value = True
        Else
            dgvGrid(nIndex, EM_GRID.emChk_NullOK).Value = False
        End If

        'フィールドタイプを取得
        If drowDBAttr(0).Item("data_type").ToString = "int" Then
            dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value = "Int32"
        ElseIf drowDBAttr(0).Item("data_type").ToString = "float" Then
            dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value = "Float"
        ElseIf drowDBAttr(0).Item("data_type").ToString = "varchar" Then
            dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value = "文字"
        ElseIf drowDBAttr(0).Item("data_type").ToString = "bit" Then
            dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value = "bit"
        Else
            dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value = "Int32"
        End If

    End Sub
    ''' <summary>
    ''' フィールドタイプ変更時、対応する情報を取得する
    ''' </summary>
    ''' <param name="nIndex">列位置</param>
    ''' <remarks></remarks>
    Private Sub SetCmbFieldTypeChgData(ByVal nIndex As Integer)
        If m_bDispFlg Then Exit Sub '表示処理中の場合は受付を無視する        
        'テーブル名がない場合、処理を行わない
        If "" = CStr(dgvGrid(nIndex, EM_GRID.emCmb_TableName).Value) Then Exit Sub
        'フィールド名がない場合、処理を行わない
        If "" = CStr(dgvGrid(nIndex, EM_GRID.emCmb_Field).Value) Then Exit Sub
        'フィールドタイプがない場合、処理を行わない
        If "" = CStr(dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value) Then Exit Sub

        'フィールドタイプ 文字 以外の場合
        If "文字" <> CStr(dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value) Then
            '入力可能桁数を0とする
            dgvGrid(nIndex, EM_GRID.emTxt_InputKeta).Value = 0
        End If
        'フィールドタイプ Float 以外の場合
        If "Float" <> CStr(dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value) Then
            '小数点以下入力可能桁数を0とする
            dgvGrid(nIndex, EM_GRID.emTxt_DecKeta).Value = 0
        End If

    End Sub
    ''' <summary>
    ''' 項目セルタイプ変更時、対応する情報を取得する
    ''' </summary>
    ''' <param name="nIndex">列位置</param>
    ''' <remarks></remarks>
    Private Sub SetCmbCellTypeChgData(ByVal nIndex As Integer)
        If m_bDispFlg Then Exit Sub '表示処理中の場合は受付を無視する        
        '項目セルタイプがない場合、処理を行わない
        If "" = CStr(dgvGrid(nIndex, EM_GRID.emCmb_CellType).Value) Then Exit Sub

        'フィールドタイプ 文字 以外の場合
        If "ConboBox" <> CStr(dgvGrid(nIndex, EM_GRID.emCmb_CellType).Value) Then
            'lov_idを対象外とする
            dgvGrid(nIndex, EM_GRID.emCmb_Lov_id).Value = "対象外"
        End If

    End Sub


    ''' <summary>
    ''' 行ヘッダータイトルをセットする
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetDgvTitle()
        dgvGrid.Rows(EM_GRID.emTxt_KNo).HeaderCell.Value = "項目No"
        dgvGrid.Rows(EM_GRID.emTxt_Hedder1).HeaderCell.Value = "項目ヘッダー１"
        dgvGrid.Rows(EM_GRID.emTxt_Hedder2).HeaderCell.Value = "項目ヘッダー２"
        dgvGrid.Rows(EM_GRID.emTxt_Bikou).HeaderCell.Value = "備考"
        dgvGrid.Rows(EM_GRID.emCmb_CellType).HeaderCell.Value = "項目セルタイプ"
        dgvGrid.Rows(EM_GRID.emCmb_Lov_id).HeaderCell.Value = "lov_id"
        dgvGrid.Rows(EM_GRID.emChk_DispUmu).HeaderCell.Value = "項目表示有無"
        dgvGrid.Rows(EM_GRID.emChk_ReadOnly).HeaderCell.Value = "読み取り専用"
        dgvGrid.Rows(EM_GRID.emTxt_KHaba).HeaderCell.Value = "項目幅"
        dgvGrid.Rows(EM_GRID.emCmb_KHaiti).HeaderCell.Value = "項目配置"
        dgvGrid.Rows(EM_GRID.emTxt_Format).HeaderCell.Value = "フォーマット"
        dgvGrid.Rows(EM_GRID.emChk_Frozen).HeaderCell.Value = "列固定"
        dgvGrid.Rows(EM_GRID.emCmb_TableName).HeaderCell.Value = "テーブル名"
        dgvGrid.Rows(EM_GRID.emCmb_Field).HeaderCell.Value = "フィールド名"
        dgvGrid.Rows(EM_GRID.emChk_Key).HeaderCell.Value = "主キー"
        dgvGrid.Rows(EM_GRID.emCmb_FieldType).HeaderCell.Value = "フィールドタイプ"
        dgvGrid.Rows(EM_GRID.emTxt_InputMin).HeaderCell.Value = "入力可能最小値"
        dgvGrid.Rows(EM_GRID.emTxt_InputMax).HeaderCell.Value = "入力可能最大値"
        dgvGrid.Rows(EM_GRID.emChk_NullOK).HeaderCell.Value = "NULL入力可否"
        dgvGrid.Rows(EM_GRID.emTxt_InputKeta).HeaderCell.Value = "入力可能桁数"
        dgvGrid.Rows(EM_GRID.emTxt_DecKeta).HeaderCell.Value = "少数点以下桁数"
        dgvGrid.Rows(EM_GRID.emCmb_DispDialog).HeaderCell.Value = "表示ダイアログ"
    End Sub

    ''' <summary>
    ''' 画面IDコンボボックス切り替え
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbGID_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbGID.SelectedIndexChanged
        If "" = cmbGID.Text Then Exit Sub
        ' 今の選択と主キーが同じ場合は無視 (選択して入力異常だったので、 元に戻した場合の対策)
        If m_KeyGID = cmbGID.Text Then Exit Sub

        If "" = m_KeyGID Then
            '画面初期処理内でのイベント受付は表示処理を行わない
            m_KeyGID = cmbGID.Text
            Exit Sub
        End If

        m_KeyGID = cmbGID.Text
        Call SetComboBox_GridID()
        Data_Disp()

    End Sub

    ''' <summary>
    ''' グリッドID切り替え
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbGridID_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbGridID.SelectedIndexChanged
        If "" = cmbGridID.Text Then Exit Sub
        ' 今の選択と主キーが同じ場合は無視 (選択して入力異常だったので、 元に戻した場合の対策)
        If m_KeyGridID = cmbGridID.Text Then Exit Sub

        If "" = m_KeyGridID Then
            '画面初期処理内でのイベント受付は表示処理を行わない
            m_KeyGridID = cmbGridID.Text
            Exit Sub
        End If

        m_KeyGridID = cmbGridID.Text
        Data_Disp()

    End Sub

    ''' <summary>
    ''' グリッドID登録
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetComboBox_GridID()
        cmbGridID.Items.Clear()
        '画面IDコンボボックスのﾒﾝﾊﾞｰを登録する
        '表示中画面IDに紐付くグリッドIDの画面表示情報を取得する
        For ii As Integer = 1 To m_DBlst(cmbGID.Text).nGridCnt
            cmbGridID.Items.Add(m_DBlst(cmbGID.Text).listGrid(ii))
        Next ii

        '初期選択状態をセットする
        cmbGridID.Text = m_DBlst(cmbGID.Text).listGrid(1)
    End Sub

    ''' <summary>
    ''' NULL、未入力、スペースのみか判定 
    ''' </summary>
    ''' <param name="targetObj">判定対象</param>
    ''' <remarks></remarks>
    Private Function IsNullOrEmptyOrSpace(ByVal targetObj As Object) As Boolean

        ' NULL 未入力判定
        If frmMain_AutoParSet.isNullOrEmpty(targetObj) Then
            Return True
        End If

        ' スペースのみ判定
        If "" = targetObj.ToString.Trim Then
            Return True
        End If

        Return False
    End Function
End Class

