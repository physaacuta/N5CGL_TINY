'*********************************************************************************
' 上位情報内容設定
'	[Ver]
'		Ver.01    2012/10/08  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcPcInfo
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////

    ' 上位情報内容設定
    Private Enum EM_PCINFO_GRID_ID
        emTxt_Order = 0                 ' テキストボックス  No
        emTxt_Name                      ' テキストボックス  名称
        emTxt_Size                      ' テキストボックス  バイトサイズ
        emTxt_EdasysID                  ' テキストボックス  EdasysID
        emCmb_IsAscii                   ' コンボボックス    Ascii or Binary
        emCmb_IsNum                     ' コンボボックス    Ascii数値変換
        emTxt_ConvCoef                  ' テキストボックス  換算係数
        emTxt_CoilInfColName            ' テキストボックス  コイル情報テーブル名称
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバ変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterPcInfo As SqlClient.SqlDataAdapter = Nothing   'アダプター (PcInfo_Desk)
    Private m_DsPcInfo As New DataSet                               '非接続型テーブル (PcInfo_Desk)


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
    Private Sub frmEpcPcInfo_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "上位情報内容設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    Private Sub frmEpcPcInfo_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        '' いろいろ開放処理
        'PcInfo関係
        m_DsPcInfo.Dispose()
        If Not m_AdapterPcInfo Is Nothing Then
            m_AdapterPcInfo.Dispose()
            m_AdapterPcInfo = Nothing
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
    Private Sub frmEpcPcInfo_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        'タイトル設定
        Call SetTitleName(Me)

        modMain.SetListMenu(Me)                                 ' メニューリスト生成
        modMain.SetLabelSysName(Me)                             ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmEpcPcInfo         ' 上位情報内容設定呼び出し

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        'グリッドの初期設定 ------------->>>>>
        ' 集計対象グループ設定
        dgvInitializePcInfo(dgvPcInfo)

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
        If Not Data_Disp() Then
            ' 画面表示失敗なので、強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DBにデータが無い為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If

        ''選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
        dgvPcInfo.DataGridViewSelectionRowColorChange(g_ColorSelection)

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

        ''PcInfoデータ取得
        strSQL = "SELECT [Order]"
        strSQL &= ", Name, Size, EdasysID, IsAscii"
        strSQL &= ", IsNum, ConvCoef, CoilInfColName"
        strSQL &= " FROM " & DB_PARAM_PCINFO_DESC
        strSQL &= " ORDER BY [Order] ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsPcInfo, m_AdapterPcInfo, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_PCINFO_DESC & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function Data_Disp() As Boolean

        'PcInfoデータの表示
        If Not Data_DispPcInfo() Then Return False

        Return True
    End Function

    ''' <summary>
    ''' 上位情報内容(PcInfo)データの表示
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function Data_DispPcInfo() As Boolean
        Dim drowPcInfo() As DataRow = m_DsPcInfo.Tables(0).Select()         ' 全レコードを表示するためWHERE句なし

        ' 画面に表示
        Try
            ' カメラセット別
            dgvPcInfo.RowCount = drowPcInfo.Length + 1                      ' 基本的にマスターには正常なレコードを登録済みのはずだが、汎用的にする為

            For ii As Integer = 0 To drowPcInfo.Length - 1

                dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Order, ii).Value = drowPcInfo(ii).Item("Order")                       ' No
                dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Name, ii).Value = drowPcInfo(ii).Item("Name")                         ' 名称
                dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Size, ii).Value = drowPcInfo(ii).Item("Size")                         ' バイトサイズ
                dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_EdasysID, ii).Value = drowPcInfo(ii).Item("EdasysID")                 ' EdasysID

                ' Ascii or Binary
                If CBool(drowPcInfo(ii).Item("IsAscii")) Then
                    dgvPcInfo(EM_PCINFO_GRID_ID.emCmb_IsAscii, ii).Value = "Ascii"
                Else
                    dgvPcInfo(EM_PCINFO_GRID_ID.emCmb_IsAscii, ii).Value = "Binary"
                End If

                ' Ascii数値変換
                If CBool(drowPcInfo(ii).Item("IsNum")) Then
                    dgvPcInfo(EM_PCINFO_GRID_ID.emCmb_IsNum, ii).Value = "True"
                Else
                    dgvPcInfo(EM_PCINFO_GRID_ID.emCmb_IsNum, ii).Value = "False"
                End If

                dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_ConvCoef, ii).Value = drowPcInfo(ii).Item("ConvCoef")                 ' バイトサイズ
                dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_CoilInfColName, ii).Value = drowPcInfo(ii).Item("CoilInfColName")     ' EdasysID

            Next ii

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_PCINFO_DESC & " 表示エラー [" & ex.Message & "] " & Me.Text)
            Return False
        End Try

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

            ' データ取得
            Data_Get()

            ' ホントに登録        
            If DB_UpDate() Then
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("正常に登録しました", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

                Data_Disp()
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

        'PcInfoデータ入力チェック
        If Not Data_CheckPcInfo() Then Return False

        Return True
    End Function

    ''' <summary>
    ''' 上位情報内容(PcInfo)データ入力チェック
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function Data_CheckPcInfo() As Boolean

        Dim strWk As String = ""
        Dim strMsg As String    ' 重複用のエラーメッセージ

        For ii As Integer = 0 To dgvPcInfo.Rows.Count - 2

            ' 名称
            strWk = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Name, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 64, "名称", True) <> 0 Then Return False
            dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Name, ii).Value = strWk

            ' バイトサイズ
            strWk = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Size, ii).Value)
            If 0 <> tClassLibrary.tClass.tClass_ImputCheck.CheckImputData_Int(strWk, 0, Int32.MaxValue, False) Then
                strMsg = "【 バイトサイズ 】" & vbNewLine
                strMsg &= "適切な値ではありません。" & vbNewLine
                strMsg &= "入力範囲：「0以上」"
                MsgBox(strMsg, MsgBoxStyle.OkOnly, "入力チェック")
                Return False

            End If

            ' EdasysID
            strWk = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_EdasysID, ii).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99999, False, "EdasysID", True) <> 0 Then Return False
			dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_EdasysID, ii).Value = strWk
			
			' EdasysIDが0の場合は重複チェックを行わない
			If 0 <> CInt(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_EdasysID, ii).Value) Then
				' EdasysIDの重複チェック
				For jj As Integer = ii + 1 To dgvPcInfo.Rows.Count - 1
					' 比較先のEdasysIDが数値なら比較
					If IsNumeric(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_EdasysID, jj).Value) Then
						' EdasysIDが同じならメッセージを出して終了
						If CInt(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_EdasysID, ii).Value) = CInt(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_EdasysID, jj).Value) Then
							strMsg = "【 上位情報内容設定 】" & vbNewLine
							strMsg &= "同じEdasysIDは登録できません。" & vbNewLine
							strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
							MsgBox(strMsg)
							Return False
						End If
					End If
				Next jj
			End If

			' Ascii or Binary 
			If "" = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emCmb_IsAscii, ii).Value) Then
				strMsg = "【 上位情報内容設定 】" & vbNewLine
				strMsg &= "Ascii or Binary の選択して下さい。" & vbNewLine
				strMsg &= "- - - No." & CStr(ii + 1)
				MsgBox(strMsg)
				Return False
			End If

			' Ascii数値変換
			If "" = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emCmb_IsNum, ii).Value) Then
				strMsg = "【 上位情報内容設定 】" & vbNewLine
				strMsg &= "Ascii数値変換の選択して下さい。" & vbNewLine
				strMsg &= "- - - No." & CStr(ii + 1)
				MsgBox(strMsg)
				Return False
			End If

            ' 換算係数
            strWk = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_ConvCoef, ii).Value)
            If 0 <> tClassLibrary.tClass.tClass_ImputCheck.CheckImputData_Dbl(strWk, 0, , , 8) Then
                strMsg = "【 換算係数 】" & vbNewLine
                strMsg &= "適切な値ではありません。" & vbNewLine
                strMsg &= "入力範囲：「0以上」、有効小数点桁数「8桁」"
                MsgBox(strMsg, MsgBoxStyle.OkOnly, "入力チェック")
                Return False

            End If

            ' コイル情報テーブル名称
			If System.DBNull.Value Is dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_CoilInfColName, ii).Value Then
			Else
				strWk = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_CoilInfColName, ii).Value)
				If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 64, "コイル情報テーブル名称", True) <> 0 Then Return False
				dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_CoilInfColName, ii).Value = strWk
			End If

		Next ii

        Return True

    End Function

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()

        'PcInfoセット
        Call Data_GetPcInfo()
    End Sub

    ''' <summary>
    ''' 画面に表示中のデータ(PcInfo)を非接続型テーブルにセット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_GetPcInfo()

        m_DsPcInfo.Tables(0).Rows.Clear()

        For ii As Integer = 0 To dgvPcInfo.Rows.Count - 2
            Dim drowNew As DataRow = m_DsPcInfo.Tables(0).NewRow()                                          ' 新規行
            drowNew.Item("Order") = dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Order, ii).Value                      ' No
            drowNew.Item("Name") = dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Name, ii).Value                        ' 名称
            drowNew.Item("Size") = CInt(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Size, ii).Value)                  ' バイトサイズ
            drowNew.Item("EdasysID") = CInt(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_EdasysID, ii).Value)          ' EdasysID
            If "Ascii" = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emCmb_IsAscii, ii).Value) Then                    ' Ascii or Binary
                drowNew.Item("IsAscii") = "True"
            Else
                drowNew.Item("IsAscii") = "False"
            End If
            If "True" = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emCmb_IsNum, ii).Value) Then                           ' Ascii数値変換
                drowNew.Item("IsNum") = "True"
            Else
                drowNew.Item("IsNum") = "False"
            End If
            drowNew.Item("ConvCoef") = CDbl(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_ConvCoef, ii).Value)              ' 換算係数
            drowNew.Item("CoilInfColName") = dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_CoilInfColName, ii).Value  ' コイル情報テーブル名称
            ' テーブルに追加
            m_DsPcInfo.Tables(0).Rows.InsertAt(drowNew, m_DsPcInfo.Tables(0).Rows.Count)
        Next ii

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
            strSql = "DELETE FROM " & DB_PARAM_PCINFO_DESC
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' エラーならロールバックして終了
                tcls_DB.TransactRollback()
                Return False
            End If

            For ii As Integer = 0 To m_DsPcInfo.Tables(0).Rows.Count - 1
                With m_DsPcInfo.Tables(0).Rows(ii)
                    '登録SQL文作成
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_PARAM_PCINFO_DESC & " VALUES ("
                    strSql &= .Item("Order").ToString & ",'"                 ' No
                    strSql &= .Item("Name").ToString & "',"                  ' 名称
                    strSql &= .Item("Size").ToString & ","                   ' バイトサイズ
                    strSql &= .Item("EdasysID").ToString & ",'"              ' EdasysID
                    strSql &= .Item("IsAscii").ToString & "','"               ' Ascii or Binary
                    strSql &= .Item("IsNum").ToString & "',"                 ' Ascii数値変換
                    strSql &= .Item("ConvCoef").ToString & ",'"              ' 換算係数
                    strSql &= .Item("CoilInfColName").ToString & "')"        ' コイル情報テーブル名称

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
            modMain.Send_Main(CH_PARAM_PCINFODESC)    ' 上位情報
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
    ''' 上位情報内容設定
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub dgvInitializePcInfo(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        '''' 個別設定
        With dgv
            .RowCount = 0

            .AllowUserToAddRows = True                                      ' ユーザー 行追加
			.AllowUserToResizeColumns = True								' ユーザー 列サイズ変更

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
        ' No
        dgvColSetText(dgv, "No", 50, True, True, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' 名称
        dgvColSetText(dgv, "名称", 260, False, False, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' バイトサイズ
        dgvColSetText(dgv, "バイト" & vbNewLine & "サイズ", 88, False, False, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' EdasysID
        dgvColSetText(dgv, "EdasysID" & vbNewLine & "(0以上)", 88, False, False, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' Ascii or Binary
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "Ascii or" & vbNewLine & "Binary"
            clm.Width = 90
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.Items.AddRange("Ascii", "Binary")
            clm.MaxDropDownItems = clm.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' Ascii数値変換
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "Ascii" & vbNewLine & "数値変換"
            clm.Width = 90
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.Items.AddRange("True", "False")
            clm.MaxDropDownItems = clm.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' 換算係数
        dgvColSetText(dgv, "換算係数", 104, False, False, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' コイル情報テーブル名称
        dgvColSetText(dgv, "コイル情報" & vbNewLine & "テーブル名称", 260, False, False, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

    End Sub

    ''' <summary>
    ''' dgvの最終行セルが押されたときの処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvPcInfo_UserAddedRow(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewRowEventArgs) Handles dgvPcInfo.UserAddedRow
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' No連番表示
        If (dgv.RowCount - 3) < 0 Then
            dgv(EM_PCINFO_GRID_ID.emTxt_Order, dgv.RowCount - 2).Value = "1"            ' No
        Else
            dgv(EM_PCINFO_GRID_ID.emTxt_Order, dgv.RowCount - 2).Value = CInt(dgv(EM_PCINFO_GRID_ID.emTxt_Order, dgv.RowCount - 3).Value) + 1
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
