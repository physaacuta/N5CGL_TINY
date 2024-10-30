'*********************************************************************************
' 特徴量名称マスター設定
'	[Ver]
'		Ver.01    2012/06/08  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcAttrName

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////

    ' 特徴量名称マスター設定
    Private Enum EM_ATTRNAME_GRID_ID
        emTxt_Enum = 0                  ' テキストボックス  特徴量並び位置
        emTxt_EdasysID                  ' テキストボックス  EdasysID
        emTxt_Name                      ' テキストボックス  特徴量名称
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバ変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterAttrName As SqlClient.SqlDataAdapter = Nothing     ' アダプター (AttrName_Desk)
    Private m_DsAttrName As New DataSet                                 ' 非接続型テーブル (AttrName_Desk)
    Private m_KeyCode As String = ""                                    ' 主キー

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ファンクションボタン押下時処理
    ''' </summary>
    ''' <param name="FncNo">ファンクションNo (0オリジン)</param>
    ''' <param name="enable">ファンクションボタンの有効無効状態</param>
    ''' <param name="onKey">同時に押されているキー</param>
    ''' <remarks></remarks>
    Private Sub frmEpcAttrName_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "特徴量名称マスター設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
                modMain.ChgProject()            ' F09 KA_PARSETへ切替処理
            Case 10
                btnF11.Select()                 ' F11を選択状態にする dgvが書き込み中の時に、入力完了になるように
                subData_UpDate()                ' F11 設定
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
    Private Sub frmEpcAttrName_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        '' いろいろ開放処理
        'AttrName関係
        m_DsAttrName.Dispose()
        If Not m_AdapterAttrName Is Nothing Then
            m_AdapterAttrName.Dispose()
            m_AdapterAttrName = Nothing
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
    Private Sub frmEpcAttrName_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                     ' メニューリスト生成
        modMain.SetLabelSysName(Me)                                 ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmEpcAttrName           ' 特徴量名称マスター設定呼び出し

        'タイトル設定
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' 特徴量名称マスター設定
        dgvInitializeAttrName(dgvAttrName)

        ' コンボボックスに特徴量テーブルをセット
        SetComboBox_Attr()


        '' データの取得
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If

        '' 画面に値を表示
        If Not Data_Disp(m_KeyCode) Then
            ' 画面表示失敗なので、強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DBにデータが無い為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If


        ''選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
        dgvAttrName.DataGridViewSelectionRowColorChange(g_ColorSelection)

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
    Private Sub frmGapSetting_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '現在の表示中の画面が自分の画面ならアプリケーションを終了
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    ''' コンボボックス初期設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetComboBox_Attr()

        ' 特徴量テーブル
        cmbAttrName.Items.Add("基本特徴量")
        cmbAttrName.Items.Add("角度別特徴量1")
        cmbAttrName.Items.Add("角度別特徴量2")
        cmbAttrName.Items.Add("角度別特徴量3")
        cmbAttrName.Items.Add("角度別特徴量4")
        m_KeyCode = "0"
        cmbAttrName.SelectedIndex = CInt(m_KeyCode)

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メソッド定義
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' データの取得
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ''AttrNameデータ取得
        strSQL = "SELECT Angle"
        strSQL &= ", Enum, EdasysID, Name"
        strSQL &= " FROM " & DB_ATTR_NAME
        strSQL &= " ORDER BY Angle, Enum ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsAttrName, m_AdapterAttrName, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_ATTR_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function Data_Disp(ByVal key As String) As Boolean

        'AttrNameデータの表示
        If Not Data_DispAttrName(key) Then Return False

        Return True
    End Function

    ''' <summary>
    ''' 特徴量名称マスター(AttrName)データの表示
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function Data_DispAttrName(ByVal key As String) As Boolean
        ' 一括のテーブルなので、テーブル毎の値のみ取り出し
        Dim strWhere As String = String.Format("Angle='{0}'", key)          ' WHERE句に相当の条件生成
        Dim drowAttrName() As DataRow = m_DsAttrName.Tables(0).Select(strWhere)         ' 全レコードを表示するためWHERE句なし


        ' 画面に表示
        Try
            dgvAttrName.RowCount = drowAttrName.Length + 1                  ' 基本的にマスターには正常なレコードを登録済みのはずだが、汎用的にする為

            For ii As Integer = 0 To drowAttrName.Length - 1
                dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Enum, ii).Value = drowAttrName(ii).Item("Enum")                       ' 特徴量並び位置
                dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, ii).Value = drowAttrName(ii).Item("EdasysID")               ' 特徴量EdasysID
                dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Name, ii).Value = drowAttrName(ii).Item("Name")                       ' 特徴量名称

            Next ii

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_ATTR_NAME & " 表示エラー [" & ex.Message & "] " & Me.Text)
            Return False
        End Try

    End Function

    ''' <summary>
    ''' 特徴量テーブル変更時
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbAttrAngle_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cmbAttrName.SelectedIndexChanged
        ' 今の選択と主キーが同じ場合は無視
        If m_KeyCode = CStr(cmbAttrName.SelectedIndex) Then Exit Sub

        '' 画面のデータを非接続型テーブルにセット
        ' 初期画面表示時には、データ無しの為、画面のデータを取り込まない
        If m_KeyCode <> "" Then
            ' 入力値チェック
            If Not Data_Check(m_KeyCode) Then
                ' 入力異常時は、選択位置も戻す
                cmbAttrName.SelectedIndex = CInt(m_KeyCode)
                Exit Sub
            End If

            ' 変更前のキーに対し 画面データ取り込み
            Data_Get(m_KeyCode)
        End If

        ' 画面にパターン別の値を表示
        m_KeyCode = CStr(cmbAttrName.SelectedIndex)
        Data_Disp(m_KeyCode)

    End Sub

    ''' <summary>
    ''' 確定処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' 入力値チェック
        If Not Data_Check(m_KeyCode) Then Exit Sub

        Data_Get(m_KeyCode)

        Try
            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            ' ホントに登録        
            If DB_UpDate() Then
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("正常に登録しました", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

                Data_Disp(m_KeyCode)
            Else
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("登録に失敗", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If

        End Try

    End Sub

    ''' <summary>
    ''' 入力値のチェック
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function Data_Check(ByVal key As String) As Boolean

        'AttrNameデータ入力チェック
        If Not Data_CheckAttrName(key) Then Return False

        Return True
    End Function

    ''' <summary>
    ''' 特徴量名称マスター(AttrName)データ入力チェック
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function Data_CheckAttrName(ByVal key As String) As Boolean

        Dim strWk As String = ""
        Dim strMsg As String    ' 重複用のエラーメッセージ
        Dim drowAttrName() As DataRow = m_DsAttrName.Tables(0).Select()         ' 全レコードを表示するためWHERE句なし
        Dim nAngle As Integer
        Dim nEnum As Integer
        Dim nEdasysID As Integer

        For ii As Integer = 0 To dgvAttrName.Rows.Count - 2

            ' 特徴量並び位置
            strWk = CStr(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Enum, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, False, "特徴量並び位置", True) <> 0 Then Return False
            dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Enum, ii).Value = strWk

            ' EdasysID
            strWk = CStr(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99999, False, "特徴量EdasysID", True) <> 0 Then Return False
            dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, ii).Value = strWk

            ' EdasysIDが0の場合は重複チェックを行わない
            If 0 <> CInt(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, ii).Value) Then
                ' EdasysIDの重複チェック(同テーブル)
                For jj As Integer = ii + 1 To dgvAttrName.Rows.Count - 1
                    ' 比較先のEdasysIDが数値なら比較
                    If IsNumeric(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, jj).Value) Then
                        ' EdasysIDが同じならメッセージを出して終了
                        If CInt(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, ii).Value) = CInt(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, jj).Value) Then
                            strMsg = "【 特徴量名称ﾏｽﾀｰ設定 】" & vbNewLine
                            strMsg &= "同じEdasysIDは登録できません。" & vbNewLine
                            strMsg &= "- - - " & cmbAttrName.Items.Item(CInt(key)).ToString & " 並び位置 " & CStr(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Enum, ii).Value) & vbNewLine
                            strMsg &= "- - - " & cmbAttrName.Items.Item(CInt(key)).ToString & " 並び位置 " & CStr(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Enum, jj).Value)
                            MsgBox(strMsg)
                            Return False
                        End If
                    End If
                Next jj

                ' EdasysIDの重複チェック(別テーブル)
                For jj As Integer = 0 To drowAttrName.Length - 1
                    nAngle = CInt(drowAttrName(jj).Item("Angle"))
                    nEnum = CInt(drowAttrName(jj).Item("Enum"))
                    nEdasysID = CInt(drowAttrName(jj).Item("EdasysID"))
                    If CInt(key) <> nAngle Then
                        If CInt(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, ii).Value) = nEdasysID Then
                            strMsg = "【 特徴量名称ﾏｽﾀｰ設定 】" & vbNewLine
                            strMsg &= "同じEdasysIDは登録できません。" & vbNewLine
                            strMsg &= "- - - " & cmbAttrName.Items.Item(CInt(key)).ToString & " 並び位置 " & CStr(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Enum, ii).Value) & vbNewLine
                            strMsg &= "- - - " & cmbAttrName.Items.Item(nAngle).ToString & " 並び位置 " & CStr(nEnum)
                            MsgBox(strMsg)
                            Return False
                        End If
                    End If
                Next jj
            End If

			' 名称
			If System.DBNull.Value Is dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Name, ii).Value Then
			Else
				strWk = CStr(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Name, ii).Value)
				If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 64, "特徴量名称", True) <> 0 Then Return False
				dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Name, ii).Value = strWk
			End If
		Next ii

        Return True

    End Function

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get(ByVal key As String)

        'AttrNameセット
        Call Data_GetAttrName(key)

    End Sub

    ''' <summary>
    ''' 画面に表示中のデータ(AttrName)を非接続型テーブルにセット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_GetAttrName(ByVal key As String)

        '' 一括のテーブルなので、テーブル毎の値のみ取り出し
        Dim strWhere As String = String.Format("Angle='{0}'", key)                  ' WHERE句に相当の条件生成
        Dim drowAttrName() As DataRow = m_DsAttrName.Tables(0).Select(strWhere)     ' 全レコードを表示するためWHERE句なし
        Dim nEnum As Integer = 0

        Try
            ' 条件No分ループ
            For ii As Integer = 0 To drowAttrName.Length - 1

                ' セット
                drowAttrName(ii).Item("Angle") = key                                                            ' 特徴量テーブル
                drowAttrName(ii).Item("Enum") = dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Enum, ii).Value           ' 特徴量並び位置
                drowAttrName(ii).Item("EdasysID") = CInt(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, ii).Value)   ' 特徴量EdasysID
                drowAttrName(ii).Item("Name") = dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Name, ii).Value          ' 特徴量名称
            Next ii

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_ATTR_NAME & " 表示エラー [" & ex.Message & "] " & Me.Text)
        End Try

        ' 新規行の追加
        If drowAttrName.Length < (dgvAttrName.RowCount - 1) Then
            For ii As Integer = drowAttrName.Length To dgvAttrName.RowCount - 2
                Dim drowNew As DataRow = m_DsAttrName.Tables(0).NewRow                                          ' 新規行
                drowNew.Item("Angle") = key                                                                     ' 特徴量テーブル
                drowNew.Item("Enum") = dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Enum, ii).Value                    ' 特徴量並び位置
                drowNew.Item("EdasysID") = CInt(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, ii).Value)      ' 特徴量EdasysID
                drowNew.Item("Name") = dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Name, ii).Value                    ' 特徴量名称
                ' テーブルに追加
                m_DsAttrName.Tables(0).Rows.InsertAt(drowNew, m_DsAttrName.Tables(0).Rows.Count)

            Next ii
		End If

    End Sub

    ''' <summary>
    ''' アップデート
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB接続
        tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSql As String                                'Sql文

        Try
            '''' データベースオープン
            If Not tcls_DB.Open() Then Return False

            'トランザクション開始
            tcls_DB.TransactStart()

            ' デリートインサートするのでテーブルの全項目を削除
            strSql = "DELETE FROM " & DB_ATTR_NAME
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' エラーならロールバックして終了
                tcls_DB.TransactRollback()
                Return False
            End If

            For ii As Integer = 0 To m_DsAttrName.Tables(0).Rows.Count - 1
                With m_DsAttrName.Tables(0).Rows(ii)
                    '登録SQL文作成
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_ATTR_NAME & " VALUES ("
                    strSql &= .Item("Angle").ToString & ","                 ' 特徴量テーブル
                    strSql &= .Item("Enum").ToString & ","                  ' 特徴量並び位置
                    strSql &= .Item("EdasysID").ToString & ",'"             ' 特徴量EdasysID
                    strSql &= .Item("Name").ToString & "')"                 ' 特徴量名称

                    'SQL実行
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' エラーならロールバックして終了
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                End With
            Next ii

            'コミット
            tcls_DB.TransactCommint()

            '統括機能へパラメータ変更通知
            modMain.Send_Main(CH_PARAM_ATTR_NAME)     ' 特徴量名称
            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            ' エラーならロールバックして終了
            tcls_DB.TransactRollback()
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "設定エラー [" & ex.Message & "] " & Me.Text)
            Return False
        Finally
            '''' 後始末
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 特徴量名称マスター設定
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub dgvInitializeAttrName(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        '''' 個別設定
        With dgv
            .RowCount = 0

            .AllowUserToAddRows = True                                      ' ユーザー 行追加

            .ScrollBars = ScrollBars.Both                                   ' スクロールバーの表示

            ' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' 列定義
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 42                                       ' ヘッダーの高さ

        End With

        '' 列定義
        ' 特徴量並び位置
        dgvColSetText(dgv, "特徴量並び位置" & vbNewLine & "(0～255)", 120, True, True, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' 特徴量EdasysID
        dgvColSetText(dgv, "特徴量EdasysID" & vbNewLine & "(0以上)", 120, False, False, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' 特徴量名称
        dgvColSetText(dgv, "特徴量名称", 520, False, False, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

    End Sub

    ''' <summary>
    ''' データグリッドビューの最終行追加時処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvAttrName_UserAddedRow(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewRowEventArgs) Handles dgvAttrName.UserAddedRow
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' 特徴量並び位置連番表示
        If dgv.RowCount <= 2 Then           ' 空白だけのときに(最初の)1行追加時の処理 
            ' １行目追加時の処理
            dgv(EM_ATTRNAME_GRID_ID.emTxt_Enum, dgv.RowCount - 2).Value = "1"
        Else
            ' ２行目以降追加時の処理
            dgv(EM_ATTRNAME_GRID_ID.emTxt_Enum, dgv.RowCount - 2).Value = CInt(dgv(EM_ATTRNAME_GRID_ID.emTxt_Enum, dgv.RowCount - 3).Value) + 1
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