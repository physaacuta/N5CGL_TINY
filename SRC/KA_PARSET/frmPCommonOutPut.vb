
'*********************************************************************************
' 検査パターン固有出力設定画面クラス
'	[Ver]
'		Ver.01    2009/07/29  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmPCommonOutPut
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' PCOMMON_OUTPUT用 データグリッドビューの列
    Private Enum em_PComOutDtColumn
        iText_Code = 0          ' テキストボックス コード
        iText_Name              ' テキストボックス 検査パターン名称
        iCheck_StopMark         ' チェックボックス ストップマークFF送信有無
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterPComOut As New SqlClient.SqlDataAdapter        ' アダプター PCOMMON_OUTPUT
    Private m_AdapterPCode As New SqlClient.SqlDataAdapter          ' アダプター PARAM_MASTER(パターン名称取得用)

    Private m_DsPCode As New DataSet                                ' 非接続型テーブル PARAM_MASTER
    Private m_DsPComOut As New DataSet                              ' 非接続型テーブル 検査パターン固有出力設定 PCOMMON_OUTPUT

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
    Private Sub frmPCommonOutPut_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "検査パターン固有出力設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    Private Sub frmPCommonOutPut_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' いろいろ開放処理
        m_DsPCode.Dispose()
        If Not m_AdapterPCode Is Nothing Then
            m_AdapterPCode.Dispose()
            m_AdapterPCode = Nothing
        End If

        m_DsPComOut.Dispose()
        If Not m_AdapterPComOut Is Nothing Then
            m_AdapterPComOut.Dispose()
            m_AdapterPComOut = Nothing
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
    Private Sub frmPCommonOutPut_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        modMain.SetListMenu(Me)                                         ' メニューリスト生成
        modMain.SetLabelSysName(Me)                                     ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmPCommonOutPut             ' 検査パターン固有出力設定

        'タイトル設定
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' このフォームで必要なデータを取得する
        '（コンボボックスに疵種、グレードを設定する為,Grid設定の前で実施）
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If

        '検査パターン固有出力設定
        'dgvData初期化
        dgvInitialize(dgvPComOut)
        dgvColunmDefData(dgvPComOut, True)
        MyBase.DataGridViewSelectionCancel(dgvPComOut)

        'dgvEtc初期化()
        dgvInitialize(dgvPComOutEtc)
        dgvColunmDefData(dgvPComOutEtc, False)
        MyBase.DataGridViewSelectionCancel(dgvPComOutEtc)

        '// 検査パターン固有出力設定 画面に値を表示
        If Not Data_PComOutDisp() Then                                       '  画面項目表示処理
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
    ''' このフォームで必要なデータを取得する DB読込
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        '// 検査ﾊﾟﾀｰﾝ読込 検査ﾊﾟﾀｰﾝはpcodeで昇順
        ' SQL文生成
        strSQL = " SELECT "
        strSQL &= " pcode, "
        strSQL &= " 有効判定 , "
        strSQL &= " 検査パターン名称 "
        strSQL &= " FROM " & DB_PCODE_MASTER
        strSQL &= " ORDER BY pcode ASC"


        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsPCode, m_AdapterPCode, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PCODE_MASTER & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        '// 表示、更新用
        '検査パターン固有出力設定
        strSQL = " SELECT "
        strSQL &= " pcode, "
        strSQL &= " ストップマークFF送信有無 "
        strSQL &= " FROM " & DB_PCOMMON_OUTPUT
        strSQL &= " ORDER BY pcode ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsPComOut, m_AdapterPComOut, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PCOMMON_OUTPUT & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' DBの情報を画面に表示 検査パターン固有出力設定
    ''' </summary>
    ''' <returns>正常：True 異常：False</returns>
    ''' <remarks></remarks>
    Private Function Data_PComOutDisp() As Boolean
        Dim strWhere As String              ' WHERE句に相当の条件生成
        Dim drowPcode() As DataRow = m_DsPComOut.Tables(0).Select()         ' 全レコードを表示するためWHERE句なし
        Dim drow As DataRow                                                 ' 検査パターン取得用 データレコード 1行

        'データ数が検査パターンMAXと同じでない場合は異常なので強制終了させる
        If MAX_PCODE <> drowPcode.Length Then
            Return False
        End If

        Try

            ' データグリッドビューの行数設定
            dgvPComOut.RowCount = drowPcode.Length - 1

            ' 画面に表示
            For ii As Integer = 0 To drowPcode.Length - 1                         ' 取得件数分 LOOPする
                ' 最終行かどうか
                If ii < drowPcode.Length - 1 Then
                    ' 最終行出なければdgvPComOutに出力
                    dgvPComOut(em_PComOutDtColumn.iText_Code, ii).Value = drowPcode(ii).Item("pcode")


                    ' ストップマークFF送信有無チェックボックス
                    If CInt(drowPcode(ii).Item("ストップマークFF送信有無")) = 0 Then
						dgvPComOut(em_PComOutDtColumn.iCheck_StopMark, ii).Value = False								   ' 0ならチェックOFF
					Else
						dgvPComOut(em_PComOutDtColumn.iCheck_StopMark, ii).Value = True									   ' 0以外はチェックON
					End If


                    'パターン名称設定
                    strWhere = String.Format("pcode='{0}'", dgvPComOut(em_PComOutDtColumn.iText_Code, ii).Value.ToString)   ' WHERE句に相当の条件生成

                    drow = m_DsPCode.Tables(0).Select(strWhere)(0)											' 主キーを指定しての取り出しなので、一件しかない
					If drow.Item("検査パターン名称") Is System.DBNull.Value Then
						dgvPComOut(em_PComOutDtColumn.iText_Name, ii).Value = ""										  ' 検査パターン名称
					Else
						dgvPComOut(em_PComOutDtColumn.iText_Name, ii).Value = drow.Item("検査パターン名称").ToString	  ' 検査パターン名称
					End If


				Else
                    ' 最終行はdgvPComOutEtcに出力
                    dgvPComOutEtc(em_PComOutDtColumn.iText_Code, 0).Value = drowPcode(ii).Item("pcode")                       ' 検査パターンコード Z

                    ' ストップマークFF送信有無チェックボックス
                    If CInt(drowPcode(ii).Item("ストップマークFF送信有無")) = 0 Then
						dgvPComOutEtc(em_PComOutDtColumn.iCheck_StopMark, 0).Value = False									  ' 0ならチェックOFF
					Else
						dgvPComOutEtc(em_PComOutDtColumn.iCheck_StopMark, 0).Value = True									  ' 0以外はチェックON
					End If

                    'パターン名称設定
                    strWhere = String.Format("pcode='{0}'", dgvPComOutEtc(em_PComOutDtColumn.iText_Code, 0).Value.ToString) ' WHERE句に相当の条件生成
                    drow = m_DsPCode.Tables(0).Select(strWhere)(0)											' 主キーを指定しての取り出しなので、一件しかない
					dgvPComOutEtc(em_PComOutDtColumn.iText_Name, 0).Value = drow.Item("検査パターン名称").ToString			' 検査表パターン名称

				End If
			Next ii

            ' カーソルスタート位置は入力エリアの先頭 （チェックボックス)

            dgvPComOut(em_PComOutDtColumn.iCheck_StopMark, 0).Selected = True
            dgvPComOutEtc(em_PComOutDtColumn.iCheck_StopMark, 0).Selected = True

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "検査パターン固有出力設定 表示設定エラー [" & ex.Message & "] " & Me.Text)
            Return False
        End Try

    End Function


    ''' <summary>
    ''' 入力値のチェック 検査パターン固有出力設定
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_CheckPComOut() As Boolean

        '入力箇所がストップマークFF送信有無のみ為、チェック処理はなし

        Return True
    End Function


    ''' <summary>
    ''' アップデート
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim transact As SqlClient.SqlTransaction = Nothing   'トランザクション処理

        Try
            ' DB接続
            m_AdapterPComOut.SelectCommand.Connection.Open()

            'トランザクション開始
            transact = m_AdapterPComOut.SelectCommand.Connection.BeginTransaction()
            m_AdapterPComOut.SelectCommand.Transaction = transact

            Using cbWk As New SqlClient.SqlCommandBuilder()
                'コマンドアダプタをセットして、更新
                '検査パターン固有出力設定
                cbWk.DataAdapter = m_AdapterPComOut
                m_AdapterPComOut.Update(m_DsPComOut)
            End Using

            'コミット
            transact.Commit()

            '統括機能へパラメータ変更通知
            modMain.Send_Main(CH_PARAM_SETTIG_PCOMMON)
            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " 設定エラー [" & ex.Message & "] " & Me.Text)

            ' エラーならロールバックして終了
            If Not transact Is Nothing Then
                transact.Rollback()
            End If

            Return False
        Finally
            ' 後始末
            If m_AdapterPComOut.SelectCommand.Connection.State = ConnectionState.Open Then
                m_AdapterPComOut.SelectCommand.Connection.Close()
            End If

            If Not transact Is Nothing Then
                transact.Dispose()
            End If
        End Try
    End Function



    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット 検査パターン固有出力設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get_PComOut()
        Dim strWhere As String              ' WHERE句に相当の条件生成
        Dim drow As DataRow                 ' データレコード 1行
        Dim nCheck As Integer               ' 有効判定

        Try
            ' 検査パターンコード分ループ その他以外のデータを格納
			For ii As Integer = 0 To dgvPComOut.Rows.Count - 1
                strWhere = String.Format("pcode='{0}'", dgvPComOut(em_PComOutDtColumn.iText_Code, ii).Value.ToString)     ' WHERE句に相当の条件生成

                drow = m_DsPComOut.Tables(0).Select(strWhere)(0)											' 主キーを指定しての取り出しなので、一件しかない

				' 有効無効チェックボックスのチェック状態によりセット
				If CBool(dgvPComOut(em_PComOutDtColumn.iCheck_StopMark, ii).Value) Then
					nCheck = 1											' 有効
				Else
					nCheck = 0											' 無効
				End If

				' 取得した値をセット
				drow.Item("ストップマークFF送信有無") = nCheck													 ' 有効判定

			Next ii

            'デフォルト更新 実際には１件しかない
			For ii As Integer = 0 To dgvPComOutEtc.Rows.Count - 1
                strWhere = String.Format("pcode='{0}'", dgvPComOutEtc(em_PComOutDtColumn.iText_Code, ii).Value.ToString)     ' WHERE句に相当の条件生成
                drow = m_DsPComOut.Tables(0).Select(strWhere)(0)                                            ' 主キーを指定しての取り出しなので、一件しかない

                ' 有効無効チェックボックスのチェック状態によりセット
                If CBool(dgvPComOutEtc(em_PComOutDtColumn.iCheck_StopMark, ii).Value) Then
					nCheck = 1											' 有効
				Else
					nCheck = 0											' 無効
				End If

				' 取得した値をセット
				drow.Item("ストップマークFF送信有無") = nCheck													 ' 有効判定
			Next


        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "検査パターン固有出力設定 非接続型テーブル データセットエラー [" & ex.Message & "] " & Me.Text)
        End Try

    End Sub


    ''' <summary>
    ''' 確定処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()
        ' 入力値チェック
        'If Not Data_CheckPComOut() Then Exit Sub               'チェック項目が無いのでコメント化

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            ' 表示データ取得
            Call Data_Get_PComOut()

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

            ' 画面更新
            ' データの再取得
            If Not DB_Select() Then
                ' DB 失敗なので、画面強制終了
                frmMsg.SetSelectedFNo(0)

                frmMsg.Text = "異常"
                frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       '終了
            End If

            ' 再表示
            '// 検査パターン固有出力設定 画面に値を表示
            If Not Data_PComOutDisp() Then                                       '  画面項目表示処理
                ' 画面表示失敗なので、強制終了
                frmMsg.SetSelectedFNo(0)

                frmMsg.Text = "異常"
                frmMsg.ChangeMsg("DBにデータが無い為 画面を強制終了します", Color.Red.ToArgb, True, False)
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
            .AutoGenerateColumns = False                                    ' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
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
            .ColumnHeadersHeight = 42                                       ' ヘッダーの高さ

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
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' 選択セル色定義
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
        End With
    End Sub

    ''' <summary>
    ''' dgvの選択が変更されたら、セルの選択状態をキャンセル
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvPComOut.SelectionChanged,
                                                                                                      dgvPComOutEtc.SelectionChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' 対象のdgv
        dgv.CurrentCell.Selected = False
    End Sub

    ''' <summary>
    ''' dgvのセルが選択されたときにコンボボックスならドロップダウンする
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>%{DOWN}でのドロップダウン操作が、開発PCだと出来ない、原因不明</remarks>
    Private Sub dataGridView_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' 対象のdgv

        ' 列のタイプがコンボボックスか？
        If TypeOf dgv.Columns(e.ColumnIndex) Is DataGridViewComboBoxColumn Then
            ' ドロップダウン表示
            SendKeys.Send("%{DOWN}")
        End If
    End Sub

    ''' <summary>
    ''' セルの状態が変化した時の処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvPComOut.CurrentCellDirtyStateChanged, dgvPComOutEtc.CurrentCellDirtyStateChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' セルがコミットされていない変更を含む場合コミットする
        If dgv.IsCurrentCellDirty Then dgv.CommitEdit(DataGridViewDataErrorContexts.Commit)
    End Sub

    ''' <summary>
    ''' 有効無効列のチェック状態が変わったときに、背景色を変更する (検査パターン固有出力設定)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvPComOut.CellValueChanged, dgvPComOutEtc.CellValueChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)

        'ヘッダは処理しない
        If e.RowIndex = -1 Then Exit Sub

        ' 有効無効列か
        If e.ColumnIndex = em_PComOutDtColumn.iCheck_StopMark Then
            If CBool(dgv(e.ColumnIndex, e.RowIndex).Value) Then
                'チェックを付けた時 背景色と選択色を規定色にする
                dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = g_ColorSetting
                dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = g_ColorSetting
            Else
                'チェックを外した時 背景色と選択色ををデフォルトに戻す
                dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty
                dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.Empty
            End If
        End If
    End Sub


    ''' <summary>
    ''' dgvPComOutの横スクロールが変更されたときに その他のdgvPComOutEtcをスクロールさせる
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>項目が増えたとき用</remarks>
    Private Sub dgvPComOut_Scroll(ByVal sender As Object, ByVal e As System.Windows.Forms.ScrollEventArgs) Handles dgvPComOut.Scroll
        ' スクロールイベントが水平スクロールなら
        If e.ScrollOrientation = ScrollOrientation.HorizontalScroll Then
            ' dgvPComOutEtcとdgvPComOutのスクロール位置を合わせる
            dgvPComOutEtc.HorizontalScrollingOffset = dgvPComOut.HorizontalScrollingOffset
        End If
    End Sub


    ''' <summary>
    ''' 検査パターン固有出力設定 列定義
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <param name="bDisptype">dgvのタイプ ture = dgvData, false = dgvEtc</param>
    ''' <remarks></remarks>
    Private Sub dgvColunmDefData(ByVal dgv As DataGridView, ByVal bDisptype As Boolean)

        With dgv
            ''=================================
            '' 列定義

            ' 条件No
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "検査ﾊﾟﾀｰﾝｺｰﾄﾞ"
                clm.Width = 120
                clm.Frozen = False
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                .Columns.Add(clm)
            End Using

            ' 検査パターン名称
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "検査ﾊﾟﾀｰﾝ名称"
                'clm.Width = 300
                clm.Width = 260
                clm.Frozen = False
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.DefaultCellStyle.DataSourceNullValue = ""
                .Columns.Add(clm)
            End Using

            ' dgvData部品プロパティセット
            ' ｽﾄｯﾌﾟﾏｰｸFF送信有無
            Using clm As New DataGridViewCheckBoxColumn
                clm.HeaderText = "ｽﾄｯﾌﾟﾏｰｸFF送信有無" & vbNewLine & "(ﾁｪｯｸ：有効)"
                clm.Width = 170
                clm.Frozen = False
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                .Columns.Add(clm)
            End Using

            If bDisptype = False Then

                ' dgvEtcの個別設定 他に適当なところが無いのでここへ記載
                .ColumnHeadersVisible = False                           ' ヘッダー非表示
                .RowCount = 1                                           ' 必ず1行
                '.Rows(0).DefaultCellStyle.BackColor = g_ColorSetting    ' 背景色を規定色にする

            End If

        End With
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
