'*********************************************************************************
' 検査パターンマスター登録画面クラス
'	[Ver]
'		Ver.01    2012/06/08  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Class frmPCodeMaster
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' データグリッドビューの列
    Private Enum EM_GRID_ID
        emTxt_Code = 0     ' テキストボックス　検査パターンコード
        emChk_Check        ' チェックボックス　有効無効
        emTxt_Priority     ' テキストボックス　優先度
        emTxt_Name         ' テキストボックス　検査表面状態名称
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' アダプター
    Private m_DS As New DataSet                                     ' 非接続型テーブル

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メニューボタン
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' ファンクション切替
    ''' </summary>
    ''' <param name="FncNo">ファンクションNo (0オリジン)</param>
    ''' <param name="enable">ファンクションボタンの有効無効状態</param>
    ''' <param name="onKey">同時に押されているキー</param>
    ''' <remarks></remarks>
    Private Sub frmPatternMaster_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "検査パターンマスター登録画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    Private Sub frmPatternMaster_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' いろいろ開放処理
        m_DS.Dispose()
        If Not m_Adapter Is Nothing Then
            m_Adapter.Dispose()
            m_Adapter = Nothing
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
    Private Sub frmPCodeMaster_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                     ' メニューリスト生成
        modMain.SetLabelSysName(Me)                                 ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmPCodeMaster           ' 検査パターンマスター設定画面

        'タイトル設定
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        'dgvData初期化
        dgvInitialize(dgvData, True)

        'dgvEtc初期化
		dgvInitialize(dgvEtc, False)

        ' このフォームで必要なデータを取得する
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If

        ' 画面に値を表示
        If Not Data_Disp() Then                                       '  画面項目表示処理
            ' 画面表示失敗なので、強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DBにデータが無い為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If

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
    Private Sub frmPCodeMaster_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

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
        strSQL = " SELECT pcode"
        strSQL &= ", 有効判定"
        strSQL &= ", 優先度"
        strSQL &= ", 検査パターン名称"
        strSQL &= " FROM " & DB_PCODE_MASTER
        strSQL &= " ORDER BY pcode ASC"

		'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PCODE_MASTER & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <returns>正常：True 異常：False</returns>
    ''' <remarks></remarks>
    Private Function Data_Disp() As Boolean
        Dim drowPcode() As DataRow = m_DS.Tables(0).Select()           ' 全レコードを表示するためWHERE句なし

        'データ数が検査パターンMAX(64)と同じでない場合は異常なので強制終了させる
        'HACK 2015/02/04　やはり必要
        If MAX_PCODE <> drowPcode.Length Then
            Return False
        End If

        ' データグリッドビューの行数設定
        dgvData.RowCount = drowPcode.Length - 1

        ' 画面に表示
        For ii As Integer = 0 To drowPcode.Length - 1                 ' 取得件数分 LOOPする
            ' 最終行かどうか
            If ii < drowPcode.Length - 1 Then
                ' 最終行でなければdgvDataに出力
                dgvData(EM_GRID_ID.emTxt_Code, ii).Value = drowPcode(ii).Item("pcode")                      ' 検査パターンコード 1〜64
                ' 有効判定チェックボックス
                If CInt(drowPcode(ii).Item("有効判定")) = 0 Then
                    dgvData(EM_GRID_ID.emChk_Check, ii).Value = False                                       ' 0ならチェックOFF
                Else
                    dgvData(EM_GRID_ID.emChk_Check, ii).Value = True                                        ' 0以外はチェックON
                End If
                dgvData(EM_GRID_ID.emTxt_Priority, ii).Value = drowPcode(ii).Item("優先度")                 ' 優先度
                dgvData(EM_GRID_ID.emTxt_Name, ii).Value = drowPcode(ii).Item("検査パターン名称")           ' 検査表パターン名称

            Else
                ' 最終行はその他dgvに出力
                dgvEtc(EM_GRID_ID.emTxt_Code, 0).Value = drowPcode(ii).Item("pcode")                        ' 検査パターンコード 64
                dgvEtc(EM_GRID_ID.emTxt_Priority, 0).Value = drowPcode(ii).Item("優先度")                  ' 優先度
                dgvEtc(EM_GRID_ID.emTxt_Name, 0).Value = drowPcode(ii).Item("検査パターン名称")            ' 検査表パターン名称

            End If
        Next ii

        ' カーソルスタート位置は入力エリアの先頭 （テキストボックス)
        dgvData(EM_GRID_ID.emTxt_Name, 0).Selected = True
        Return True
    End Function

    ''' <summary>
    ''' 確定処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' 入力値チェック
        If Not Data_Check() Then Exit Sub

        Try

            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            ' 表示データ取得
            Data_Get()

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
    Private Function Data_Check() As Boolean
        Dim strWk As String

        ' 検査パターンコード分ループ
        For ii As Integer = 0 To dgvData.RowCount - 1
            ' 検査パターン名称
            strWk = dgvData(EM_GRID_ID.emTxt_Name, ii).Value.ToString
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, _
                "検査パターンコード=" & dgvData(EM_GRID_ID.emTxt_Code, ii).Value.ToString & " 検査パターン名称") <> 0 Then Return False
            dgvData(EM_GRID_ID.emTxt_Name, ii).Value = strWk

            ' 有効判定取得
            Dim bCheck As Boolean = CBool(dgvData(EM_GRID_ID.emChk_Check, ii).Value)

            ' 優先度の範囲確認
            strWk = dgvData(EM_GRID_ID.emTxt_Priority, ii).Value.ToString
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 9999, Not bCheck, "検査パターンコード=" & dgvData(EM_GRID_ID.emTxt_Code, ii).Value.ToString & " 優先度") <> 0 Then
                ' 有効判定が有効の場合はNULL禁止、無効の場合はNULL許可
                Return False
            End If
            dgvData(EM_GRID_ID.emTxt_Priority, ii).Value = strWk

            ' 優先度の重複チェック
            If bCheck = True Then
                ' 有効な項目を確認する
                For jj As Integer = ii + 1 To dgvData.Rows.Count - 1
                    If Trim(CStr(dgvData(EM_GRID_ID.emTxt_Code, jj).Value)) = "" Then
                        Continue For                                                ' PCodeが空なら次へ
                    End If
                    If Not CBool(dgvData(EM_GRID_ID.emChk_Check, jj).Value) Then
                        Continue For                                                ' 無効なら次へ
                    End If
                    If IsNumeric(dgvData(EM_GRID_ID.emTxt_Priority, jj).Value) Then
                        If CInt(dgvData(EM_GRID_ID.emTxt_Priority, ii).Value) = CInt(dgvData(EM_GRID_ID.emTxt_Priority, jj).Value) Then
                            Dim strMsg As String = ""
                            strMsg = "【 検査パターンマスター登録 】" & vbNewLine
                            strMsg &= "同じ優先度は登録できません。" & vbNewLine
                            strMsg &= "- - - 検査パターンコード「" & CStr(dgvData(EM_GRID_ID.emTxt_Code, ii).Value) & "」 と 「" & CStr(dgvData(EM_GRID_ID.emTxt_Code, jj).Value) & "」"
                            MsgBox(strMsg)
                            Return False
                        End If
                    End If
                Next jj
            End If

        Next ii

        Return True
    End Function

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()
        Dim strWhere As String              ' WHERE句に相当の条件生成
        Dim drow As DataRow                 ' データレコード 1行
        Dim nCheck As Integer               ' 有効判定

        ' 検査パターンコード分ループ その他以外のデータを格納
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            strWhere = String.Format("pcode='{0}'", dgvData(EM_GRID_ID.emTxt_Code, ii).Value.ToString)  ' WHERE句に相当の条件生成
            drow = m_DS.Tables(0).Select(strWhere)(0)                                                   ' 主キーを指定しての取り出しなので、一件しかない

            ' 有効無効チェックボックスのチェック状態により、有効判定をセット
            If CBool(dgvData(EM_GRID_ID.emChk_Check, ii).Value) Then
                nCheck = 1                                          ' 有効
            Else
                nCheck = 0                                          ' 無効
            End If

            ' 取得した値をセット
            drow.Item("有効判定") = nCheck                          ' 有効判定

            ' 優先度
            If Nothing Is dgvData(EM_GRID_ID.emTxt_Priority, ii).Value OrElse _
            "" = dgvData(EM_GRID_ID.emTxt_Priority, ii).Value.ToString.Trim Then
                drow.Item("優先度") = System.DBNull.Value
            Else
                drow.Item("優先度") = CInt(dgvData(EM_GRID_ID.emTxt_Priority, ii).Value)
            End If

            drow.Item("検査パターン名称") = dgvData(EM_GRID_ID.emTxt_Name, ii).Value.ToString  '検査パターン名称

        Next ii
        For ii As Integer = 0 To dgvEtc.Rows.Count - 1
            strWhere = String.Format("pcode='{0}'", dgvEtc(EM_GRID_ID.emTxt_Code, ii).Value.ToString)  ' WHERE句に相当の条件生成
            drow = m_DS.Tables(0).Select(strWhere)(0)                                                   ' 主キーを指定しての取り出しなので、一件しかない

        Next ii
    End Sub

    ''' <summary>
    ''' アップデート
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim transact As SqlClient.SqlTransaction = Nothing   'トランザクション処理

        Try
            ' DB接続
            m_Adapter.SelectCommand.Connection.Open()

            'トランザクション開始
            transact = m_Adapter.SelectCommand.Connection.BeginTransaction()
            m_Adapter.SelectCommand.Transaction = transact

            Using cbWk As New SqlClient.SqlCommandBuilder()

                'コマンドアダプタをセットして、更新
                cbWk.DataAdapter = m_Adapter
                m_Adapter.Update(m_DS)

            End Using

            'コミット
            transact.Commit()

            '統括機能へパラメータ変更通知
            modMain.Send_Main(CH_PARAM_SETTIG_PCODE)
            modMain.HardCopy(Me)
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
            If m_Adapter.SelectCommand.Connection.State = ConnectionState.Open Then
                m_Adapter.SelectCommand.Connection.Close()
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
    ''' 表示設定
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal bDisptype As Boolean)
		Dim bLineVisible As Boolean = True									' ライン別表示非表示判定(true:表示,false:非表示)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し
        Call dgv.DataGridViewDataExec(dgv)

        With dgv
            .ScrollBars = ScrollBars.Both                                   ' スクロールバーの表示

            ' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
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
            .ColumnHeadersHeight = 42                                      ' ヘッダーの高さ

        End With

        '' 列定義

        ' 条件No
        dgvColSetText(dgv, "検査パターン" & vbNewLine & "コード", 130, False, True,
           DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        If bDisptype = True Then
            ' dgvData部品プロパティセット
            ' 有効無効
            dgvColSetCheck(dgv, "有効無効" & vbNewLine & "(ﾁｪｯｸ：有効)", 120, False, False,
             DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        Else
            ' dgvEtc部品プロパティセット
            ' 有効無効
            dgvColSetText(dgv, "", 120, False, True,
             DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
        End If

		If bDisptype = True Then
            ' dgvData部品プロパティセット
            ' 優先度
            dgvColSetText(dgv, "優先度" & vbNewLine & "※1", 80, False, False,
             DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, Nothing, "")
        Else
            ' dgvEtc部品プロパティセット
            ' 優先度
            dgvColSetText(dgv, "優先度" & vbNewLine & "※1", 80, False, True,
             DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, Nothing, "")
        End If

		If bDisptype = True Then
            ' 検査パターン名称
            dgvColSetText(dgv, "検査パターン名称" & vbNewLine & "（半角３２文字）", 300, False, False,
            DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, Nothing, "")
        Else
            ' 検査パターン名称
            dgvColSetText(dgv, "検査パターン名称" & vbNewLine & "（半角３２文字）", 300, False, True,
            DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, Nothing, "")
        End If

        If bDisptype = False Then
            ' dgvEtcの個別設定 他に適当なところが無いのでここへ記載
            dgv.ColumnHeadersVisible = False                           ' ヘッダー非表示
            dgv.Enabled = True                                         ' dgv使用不可
            dgv.RowCount = 1                                           ' 必ず1行
            dgv.Rows(0).DefaultCellStyle.BackColor = g_ColorSetting    ' 背景色を規定色にする
        End If

	End Sub

    ''' <summary>
    ''' 有効無効列のチェック状態が変わったときに、背景色を変更する
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellValueChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)

        'ヘッダは処理しない
        If e.RowIndex = -1 Then Exit Sub

        ' 有効無効列か
        If e.ColumnIndex = EM_GRID_ID.emChk_Check Then
            If CBool(dgvData(e.ColumnIndex, e.RowIndex).Value) Then
                'チェックを付けた時 背景色と選択色を規定色にする
                dgvData.Rows(e.RowIndex).DefaultCellStyle.BackColor = g_ColorSetting
                dgvData.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = g_ColorSetting
            Else
                'チェックを外した時 背景色と選択色ををデフォルトに戻す
                dgvData.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty
                dgvData.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.Empty
            End If
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
