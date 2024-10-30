'*********************************************************************************
' 自動生成画面選択項目アイテムマスター画面クラス
'	[Ver]
'		Ver.01    2015/07/08  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmLovItem
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 定数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private Enum EM_LOV_ITEM
		emNo = 0							' No
		emLov_id							' lov_id
		emSeq								' seq
		emDisp_member						' 表示ﾒﾝﾊﾞｰ
		emVal_member_i						' 値ﾒﾝﾊﾞｰ(数値)
		emVal_member_c						' 値ﾒﾝﾊﾞｰ(文字)
		emVal_member_b						' 値ﾒﾝﾊﾞｰ(bit)
		emInsButton							' 挿入ボタン
		emDelButton							' 削除ボタン
	End Enum

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバー変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_AdapterGrid As New SqlClient.SqlDataAdapter		 ' アダプター データグリッド表示設定
	Private m_Grid As New DataSet								 ' 非接続型テーブル データグリッド表示設定
	Private m_AdapterLov_Item As New SqlClient.SqlDataAdapter	' アダプター PAR_LOV_ITEM
	Private m_Lov_Item As New DataSet							' 非接続型テーブル PAR_LOV_ITEM

	Private m_nSendScode As Integer								' メールで送信するScode
	Private m_nDelLov_id(10000 - 1) As Integer					' 削除対象のLov_id
	Private m_nDelLov_id_Count As Integer						' 削除対象件数
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
	Private Sub frmTypeGr_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
		'' ファンクションボタン有効項目のみ
		If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "選択項目アイテムマスター画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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

		m_Lov_Item.Dispose()
		If Not m_AdapterLov_Item Is Nothing Then
			m_AdapterLov_Item.Dispose()
			m_AdapterLov_Item = Nothing
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
        modMain.SetLabelSysName(Me)                             ' メニューラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmLovItem			' 画面呼出

		Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

		'dgv初期化
		dgvInitializeDefType(dgvLov_Item)

		' このフォームで必要なデータを取得する
		If Not DB_Select() Then
			' DB 失敗なので、画面強制終了
			frmMsg.SetSelectedFNo(0)

			frmMsg.Text = "異常"
			frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
			frmMsg.ShowDialog()
			modMain.TaskEnd()						'終了
		End If

		' データのレコード数を初期化
		Data_Init()

		' 画面に値を表示
		Data_Disp()

		' 選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
		dgvLov_Item.DataGridViewSelectionRowColorChange(g_ColorSelection)	  ' 疵種dgv

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
		strSQL = "SELECT lov_id"
		strSQL &= ", seq"
		strSQL &= ", disp_member"
		strSQL &= ", val_member_i"
		strSQL &= ", val_member_c"
		strSQL &= ", val_member_b"
		strSQL &= " FROM " & DB_PAR_LOV_ITEM
		strSQL &= " ORDER BY lov_id, seq ASC"

		'SQL文実行
		If 0 > MyBase.Get_SelectData(strSQL, m_Lov_Item, m_AdapterLov_Item, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PAR_LOV_ITEM & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		Return True

	End Function

    ''' <summary>
    ''' テーブル更新を行う
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function DB_Delete() As Boolean
        Dim transactGrid As SqlClient.SqlTransaction = Nothing  'トランザクション処理
        Dim transactLov As SqlClient.SqlTransaction = Nothing   'トランザクション処理
        Dim strSql As String                                    'Sql文

        Try
            ' Delete用 SqlCommand作成
            strSql = "DELETE FROM " & DB_PAR_LOV_ITEM
            m_AdapterLov_Item.DeleteCommand = New SqlClient.SqlCommand
            m_AdapterLov_Item.DeleteCommand.CommandText = strSql
            m_AdapterLov_Item.DeleteCommand.Connection = m_AdapterLov_Item.SelectCommand.Connection

            ' DB接続
            m_AdapterGrid.SelectCommand.Connection.Open()
            m_AdapterLov_Item.SelectCommand.Connection.Open()

            'トランザクション開始
            transactGrid = m_AdapterGrid.SelectCommand.Connection.BeginTransaction()
            m_AdapterGrid.SelectCommand.Transaction = transactGrid

            transactLov = m_AdapterLov_Item.SelectCommand.Connection.BeginTransaction()
            m_AdapterLov_Item.SelectCommand.Transaction = transactLov
            m_AdapterLov_Item.DeleteCommand.Transaction = transactLov

            ' 選択項目アイテムマスターの全項目を削除
            If 0 > m_AdapterLov_Item.DeleteCommand.ExecuteNonQuery() Then
                ' エラーならロールバックして終了
                transactLov.Rollback()
                Return False
            End If

            ' 選択項目アイテムマスターの登録
            Using cbWk As New SqlClient.SqlCommandBuilder()

                'コマンドアダプタをセットして、DBを更新
                cbWk.DataAdapter = m_AdapterGrid
                m_AdapterGrid.Update(m_Grid)

                cbWk.DataAdapter = m_AdapterLov_Item
                m_AdapterLov_Item.Update(m_Lov_Item)

            End Using

            'コミット
            transactGrid.Commit()
            transactLov.Commit()

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " 選択項目アイテムマスター登録失敗 [" & ex.Message & "] " & Me.Text)

            ' エラーならロールバックして終了
            If Not transactGrid Is Nothing Then
                transactGrid.Rollback()
            End If

            If Not transactLov Is Nothing Then
                transactLov.Rollback()
            End If

            Return False
        Finally

            ' 後始末
            If m_AdapterGrid.SelectCommand.Connection.State = ConnectionState.Open Then
                m_AdapterGrid.SelectCommand.Connection.Close()
            End If

            If m_AdapterLov_Item.SelectCommand.Connection.State = ConnectionState.Open Then
                m_AdapterLov_Item.SelectCommand.Connection.Close()
            End If

            If Not transactGrid Is Nothing Then
                transactGrid.Dispose()
            End If

            If Not transactLov Is Nothing Then
                transactLov.Dispose()
            End If
        End Try
    End Function
    ''' <summary>
    ''' メンバ変数の初期設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Init()

		' 削除対象の疵種EdasysID配列の初期化
		For ii As Integer = 0 To m_nDelLov_id.Length - 1
			m_nDelLov_id(ii) = 0
		Next ii

	End Sub

	''' <summary>
	''' データ行を挿入
	''' </summary>
	''' <param name="nRowIndex">行の挿入位置</param>
	''' <remarks></remarks>
	Private Sub InsertAt_Lov_Item(ByVal nRowIndex As Integer)
		' dgvLov_Itemにレコードを挿入
		dgvLov_Item.Rows.Insert(nRowIndex)

		' データ初期化
		dgvLov_Item(EM_LOV_ITEM.emNo, nRowIndex).Value = ""								' No
		dgvLov_Item(EM_LOV_ITEM.emLov_id, nRowIndex).Value = ""							' lov_id
		dgvLov_Item(EM_LOV_ITEM.emSeq, nRowIndex).Value = ""							' seq
		dgvLov_Item(EM_LOV_ITEM.emDisp_member, nRowIndex).Value = ""					' 表示ﾒﾝﾊﾞｰ
		dgvLov_Item(EM_LOV_ITEM.emVal_member_i, nRowIndex).Value = ""					' 値ﾒﾝﾊﾞｰ(数値)
		dgvLov_Item(EM_LOV_ITEM.emVal_member_c, nRowIndex).Value = ""					' 値ﾒﾝﾊﾞｰ(文字)
		dgvLov_Item(EM_LOV_ITEM.emVal_member_b, nRowIndex).Value = False				' 値ﾒﾝﾊﾞｰ(bit)

	End Sub

	''' <summary>
	''' DBの情報を画面に表示
	''' </summary>
	''' <remarks></remarks>
	Private Sub Data_Disp()
		'疵種のデータを表示
		Data_DispdgvLov_Item(dgvLov_Item)
		'疵種のインデックスとボタンの表示設定
		InitIndexdgvLov_Item(dgvLov_Item)

		' 行の色を変える （初期表示で色が変わる対策）
		dgvLov_Item.CurrentRow.DefaultCellStyle.BackColor = Color.Empty

	End Sub

	''' <summary>
	''' 疵種dgvにデータを表示
	''' </summary>
	''' <remarks></remarks>
	Private Sub Data_DispdgvLov_Item(ByVal dgv As DataGridView)
		Dim drowLov_Item() As DataRow = m_Lov_Item.Tables(0).Select()			' 全レコードを表示するためWHERE句なし
		'行を初期化
		dgv.Rows.Clear()
		' 画面に表示
		dgv.RowCount = drowLov_Item.Length			  ' 
		For ii As Integer = 0 To dgv.RowCount - 1
			'No
			dgv(EM_LOV_ITEM.emNo, ii).Value = ii + 1
			'lov_id
			dgv(EM_LOV_ITEM.emLov_id, ii).Value = drowLov_Item(ii).Item("lov_id")
			'Seq
			dgv(EM_LOV_ITEM.emSeq, ii).Value = drowLov_Item(ii).Item("seq")
			'表示ﾒﾝﾊﾞｰ
			dgv(EM_LOV_ITEM.emDisp_member, ii).Value = drowLov_Item(ii).Item("disp_member").ToString.Trim()
			'値ﾒﾝﾊﾞｰ(数値)
			dgv(EM_LOV_ITEM.emVal_member_i, ii).Value = drowLov_Item(ii).Item("val_member_i").ToString.Trim()
			'値ﾒﾝﾊﾞｰ(文字)
			If System.DBNull.Value Is drowLov_Item(ii).Item("val_member_c").ToString Then
				dgv(EM_LOV_ITEM.emVal_member_c, ii).Value = ""
			Else
				dgv(EM_LOV_ITEM.emVal_member_c, ii).Value = drowLov_Item(ii).Item("val_member_c").ToString.Trim()
			End If
			'値ﾒﾝﾊﾞｰ(bit)
			dgv(EM_LOV_ITEM.emVal_member_b, ii).Value = drowLov_Item(ii).Item("val_member_b").ToString.Trim()
		Next ii
        ''空白行を追加
        'InsertAt_Lov_Item(dgv.RowCount)
        ''Noを振りなおし、ボタンを表示しなおす
        'InitIndexdgvLov_Item(dgv)

        If 1 > dgvLov_Item.RowCount Then ' データがない場合
            ' 空白行を追加
            InsertAt_Lov_Item(dgv.RowCount)
        End If

        ' カーソルスタート位置は入力エリアの先頭
        dgv(1, 0).Selected = True
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

		Try

			' 登録しますか？
			frmMsg.Text = "条件確認"
			frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
			If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

			'画面から削除されたEdasysIDを取得
			Data_CheckDelete()

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
				modMain.TaskEnd()						'終了
			End If

		Catch ex As Exception

		Finally
			If Not frmMsg Is Nothing Then
				frmMsg.Dispose()
				frmMsg = Nothing
			End If
		End Try

		' 設定の初期化
		Data_Init()

		' 画面に値を表示
		Data_Disp()
	End Sub

	''' <summary>
	''' 入力値のチェック
	''' </summary>
	''' <returns>復帰情報</returns>
	''' <remarks></remarks>
	Private Function Data_Check() As Boolean
		Dim strWk As String		' dgvのデータ格納用
		Dim strMsg As String	' 重複用のエラーメッセージ
		Dim bVal_member_i As Boolean	' 値ﾒﾝﾊﾞｰ(数値) 有効無効 true:有効
		Dim bVal_member_c As Boolean	' 値ﾒﾝﾊﾞｰ(文字) 有効無効 true:有効
		Dim bVal_member_b As Boolean	' 値ﾒﾝﾊﾞｰ(bit)  有効無効 true:有効
		' dgv行数分ループ
		For ii As Integer = 0 To dgvLov_Item.Rows.Count - 1
			'lov_id
			strWk = CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, ii).Value)
			If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 99999, True, "No=" & ii + 1 & " lov_id") <> 0 Then Return False
			dgvLov_Item(EM_LOV_ITEM.emLov_id, ii).Value = strWk
			' lov_idが空白の行は無視
			If Trim(strWk) = "" Then Continue For

			'seq
			strWk = CStr(dgvLov_Item(EM_LOV_ITEM.emSeq, ii).Value)
			If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 999, False, "No=" & ii + 1 & " Seq") <> 0 Then Return False
			dgvLov_Item(EM_LOV_ITEM.emSeq, ii).Value = strWk

			'表示ﾒﾝﾊﾞｰ
			strWk = CStr(dgvLov_Item(EM_LOV_ITEM.emDisp_member, ii).Value)
			If tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 64, "No=" & ii + 1 & " 表示ﾒﾝﾊﾞｰ") <> 0 Then Return False
			dgvLov_Item(EM_LOV_ITEM.emDisp_member, ii).Value = strWk

			'値ﾒﾝﾊﾞｰ(数値)
			strWk = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, Int32.MinValue, Int32.MaxValue, True, "No=" & ii + 1 & " 値ﾒﾝﾊﾞｰ(数値)") <> 0 Then Return False
            dgvLov_Item(EM_LOV_ITEM.emVal_member_i, ii).Value = strWk

			'値ﾒﾝﾊﾞｰ(文字)
			strWk = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, ii).Value)
			If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "No=" & ii + 1 & " 値ﾒﾝﾊﾞｰ(文字)") <> 0 Then Return False
			dgvLov_Item(EM_LOV_ITEM.emVal_member_c, ii).Value = strWk

			For jj As Integer = ii + 1 To dgvLov_Item.Rows.Count - 1
				If CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, ii).Value) = CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, jj).Value) Then
					' lov_idが一致してるものを対象としチェックを行う
					If IsNumeric(dgvLov_Item(EM_LOV_ITEM.emSeq, jj).Value) Then
						' lov_idとSeqが同じならメッセージを出して終了
						If CInt(dgvLov_Item(EM_LOV_ITEM.emSeq, ii).Value) = CInt(dgvLov_Item(EM_LOV_ITEM.emSeq, jj).Value) Then
							strMsg = "lov_id及びSeqが重複しています。" & vbNewLine
							strMsg &= "No =[" & CStr(ii + 1) & "] と No =[" & CStr(jj + 1) & "]"
							MsgBox(strMsg)
							Return False
						End If
					End If
					'lov_id内で同一の値ﾒﾝﾊﾞｰが登録されている場合、異常とする
					' 値ﾒﾝﾊﾞｰ(数値)
					If "" <> CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, ii).Value) Then
						If CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, ii).Value) = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, jj).Value) Then
							strMsg = "値ﾒﾝﾊﾞｰ(数値)が重複しています。" & vbNewLine
							strMsg &= "No =[" & CStr(ii + 1) & "] と No =[" & CStr(jj + 1) & "]"
							MsgBox(strMsg)
							Return False
						End If
					End If
					' 値ﾒﾝﾊﾞｰ(文字)
					If "" <> CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, ii).Value) Then
						If CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, ii).Value) = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, jj).Value) Then
							strMsg = "値ﾒﾝﾊﾞｰ(文字)が重複しています。" & vbNewLine
							strMsg &= "No =[" & CStr(ii + 1) & "] と No =[" & CStr(jj + 1) & "]"
							MsgBox(strMsg)
							Return False
						End If
					End If
				End If
			Next jj
			bVal_member_i = False
			bVal_member_c = False
			bVal_member_b = False
			' 値ﾒﾝﾊﾞｰが登録されているか判定する
			' 値ﾒﾝﾊﾞｰ(数値)
			If "" <> CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, ii).Value) Then
				bVal_member_i = True
			End If
			' 値ﾒﾝﾊﾞｰ(文字)
			If "" <> CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, ii).Value) Then
				bVal_member_c = True
			End If
			' 値ﾒﾝﾊﾞｰ(bit)
			If CBool(dgvLov_Item(EM_LOV_ITEM.emVal_member_b, ii).Value) Then
				bVal_member_b = True
			End If
			' 値ﾒﾝﾊﾞｰが複数登録してある場合、異常とする
			If ((bVal_member_i And bVal_member_c) Or (bVal_member_i And bVal_member_b) Or (bVal_member_c And bVal_member_b)) Then
				strMsg = "値ﾒﾝﾊﾞｰを複数登録しています。" & vbNewLine
				strMsg &= "No =[" & CStr(ii + 1) & "]"
				MsgBox(strMsg)
				Return False
			End If

		Next ii
		'値ﾒﾝﾊﾞｰ(bit)チェック
		If Not ChkVal_member_b() Then
			Return False
		End If
		Return True
	End Function

	''' <summary>
	''' 値ﾒﾝﾊﾞｰ(bit)チェック
	''' </summary>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function ChkVal_member_b() As Boolean
		Dim strMsg As String	' 重複用のエラーメッセージ
		Dim bVal_member_i As Boolean	' 値ﾒﾝﾊﾞｰ(数値) 有効無効 true:有効
		Dim bVal_member_c As Boolean	' 値ﾒﾝﾊﾞｰ(文字) 有効無効 true:有効
		Dim nCount As Integer			' カウント値
		' dgv行数分ループ
		For ii As Integer = 0 To dgvLov_Item.Rows.Count - 1
			If "" = CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, ii).Value) Then Continue For
			' 値ﾒﾝﾊﾞｰが複数登録してある場合以上とする
			bVal_member_i = False
			bVal_member_c = False
			' 値ﾒﾝﾊﾞｰが登録されているか判定する
			' 値ﾒﾝﾊﾞｰ(数値)
			If "" <> CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, ii).Value) Then
				bVal_member_i = True
			End If
			' 値ﾒﾝﾊﾞｰ(文字)
			If "" <> CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, ii).Value) Then
				bVal_member_c = True
			End If
			nCount = 0
			' 値ﾒﾝﾊﾞｰ(数値)又は(文字)が登録されていない場合、値ﾒﾝﾊﾞｰ(bit)が有効
			If Not bVal_member_i And Not bVal_member_c Then
				For jj As Integer = 0 To dgvLov_Item.Rows.Count - 1
					If ii = jj Then Continue For '自分の場合はスキップ
					'lov_idが一致し、値ﾒﾝﾊﾞｰ(bit)が有効なものを対象とし、値が一致するかチェックを行い、一致する場合、異常とする
					If CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, ii).Value) = CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, jj).Value) Then
						'If "" = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, jj).Value) Then
						'	If "" = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, jj).Value) Then
						If CBool(dgvLov_Item(EM_LOV_ITEM.emVal_member_b, ii).Value) = CBool(dgvLov_Item(EM_LOV_ITEM.emVal_member_b, jj).Value) Then
							strMsg = "値ﾒﾝﾊﾞｰ(bit)が重複しています。" & vbNewLine
							strMsg &= "No =[" & ii + 1 & "]" & " と No =[" & jj + 1 & "]"
							MsgBox(strMsg)
							Return False
						End If
						nCount = nCount + 1
						'End If
						'	End If
					End If
				Next jj
				' 値ﾒﾝﾊﾞｰ(bit)に対しtrue,falseの２パターンのみの登録でない場合、異常とする
				If nCount <> 1 Then
					strMsg = "値ﾒﾝﾊﾞｰ(bit)は、" & vbNewLine & "ON／OFFの2パターンを登録してください。" & vbNewLine
					strMsg &= "No =[" & ii + 1 & "] lov_id =[" & CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, ii).Value) & "]"
					MsgBox(strMsg)
					Return False
				End If
			End If
		Next ii
		Return True
	End Function

	''' <summary>
	''' 画面に表示中のデータを非接続型テーブルにセット
	''' </summary>
	''' <remarks></remarks>
	Private Sub Data_Get()
		Dim nCountNo As Integer	  'Noカウント用

		' デリートインサートする為、今もっている行をクリアする
		m_Lov_Item.Tables(0).Rows.Clear()

		'No初期化
		nCountNo = 0

		' dgvの値を非接続テーブルへ格納
		For ii As Integer = 0 To dgvLov_Item.RowCount - 1
			'lov_idとseq が空白以外の行だけ取得
			If CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, ii).Value) = "" Then Continue For
			If CStr(dgvLov_Item(EM_LOV_ITEM.emSeq, ii).Value) = "" Then Continue For
            Dim drowType As DataRow = m_Lov_Item.Tables(0).NewRow()                                             ' 新規行

            drowType.Item("lov_id") = CInt(dgvLov_Item(EM_LOV_ITEM.emLov_id, ii).Value)                         ' lov_id
            drowType.Item("seq") = CInt(dgvLov_Item(EM_LOV_ITEM.emSeq, ii).Value)                               ' seq
            drowType.Item("disp_member") = CStr(dgvLov_Item(EM_LOV_ITEM.emDisp_member, ii).Value)               ' 表示ﾒﾝﾊﾞｰ
            If "" = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, ii).Value) Then
				drowType.Item("val_member_i") = System.DBNull.Value
			Else
                drowType.Item("val_member_i") = CInt(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, ii).Value)         ' 値ﾒﾝﾊﾞｰ(数値)
            End If

			If "" = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, ii).Value) Then
				drowType.Item("val_member_c") = System.DBNull.Value
			Else
                drowType.Item("val_member_c") = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, ii).Value)         ' 値ﾒﾝﾊﾞｰ(文字)
            End If

			If CBool(dgvLov_Item(EM_LOV_ITEM.emVal_member_b, ii).Value) Then
				drowType.Item("val_member_b") = 1		  ' 値ﾒﾝﾊﾞｰ(bit)
			Else
				drowType.Item("val_member_b") = 0		  ' 値ﾒﾝﾊﾞｰ(bit)
			End If

			m_Lov_Item.Tables(0).Rows.Add(drowType)		'行を追加
		Next ii

	End Sub

	''' <summary>
	''' アップデート
	''' </summary>
	''' <returns>復帰情報(True:正常,False:異常)</returns>
	''' <remarks></remarks>
	Private Function DB_UpDate() As Boolean
		Try
			'値を登録
			If Not DB_Delete() Then
				Return False
			End If

			Return True
		Catch ex As Exception
			gcls_Log.Write(tClass_LogManager.LNO.ERR, "設定エラー [" & ex.Message & "] " & Me.Text)
			Return False
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
	Private Sub dgvInitializeDefType(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
		'''' 基本を設定
		Call dgv.dgvInitialize(dgv)
		Call dgv.DataGridViewSelectionCancel(dgv)						' 選択色無し

		With dgv
			.ColumnHeadersHeight = 42									' ヘッダーの高さ
			.AllowUserToResizeColumns = True							' ユーザー 列サイズ変更
			.ScrollBars = ScrollBars.Both								' スクロールバーの表示

			' 列ヘッダー 文字サイズ変更,文字色変更
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

			'' 選択セル色定義
			.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor		' 選択色
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue				' 1行おきに色替え
			.AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue		' 一行おきに選択色変更
			.DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.DefaultCellStyle().ForeColor = Color.Black
		End With

		'' 列定義
		' No
		dgvColSetText(dgv, "No", 40, True, True, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
		' lov_id
		dgvColSetText(dgv, "lov_id", 84, True, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
		' seq
		dgvColSetText(dgv, "seq", 42, True, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
        ' 表示ﾒﾝﾊﾞｰ
        dgvColSetText(dgv, "表示ﾒﾝﾊﾞｰ", 180, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
        ' 値ﾒﾝﾊﾞｰ(数値)
        dgvColSetText(dgv, "値ﾒﾝﾊﾞｰ" & vbNewLine & "(数値)", 84, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
		' 値ﾒﾝﾊﾞｰ(文字)
		dgvColSetText(dgv, "値ﾒﾝﾊﾞｰ" & vbNewLine & "(文字)", 84, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
		' 値ﾒﾝﾊﾞｰ(bit)
		dgvColSetCheck(dgv, "値ﾒﾝﾊﾞｰ" & vbNewLine & "(bit)", 84, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

		' 上へ挿入
		dgvColSetButton(dgv, "上へ" & vbNewLine & "挿入", 50, False, False, _
			DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
			New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "挿入")

		' 削除
		dgvColSetButton(dgv, "削除", 50, False, False, _
			DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
			New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "削除")

	End Sub

	''' <summary>
	''' dgvのセルのボタンが押されたときの処理
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvLov_Item_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvLov_Item.CellContentClick
		Dim dgv As DataGridView = CType(sender, DataGridView)

		' 行がクリックされていなければ終了
		If e.RowIndex < 0 Then Exit Sub

		' 列インデックスでどの列がクリックされたか判別
		Select Case e.ColumnIndex
			Case EM_LOV_ITEM.emInsButton	 '上へ挿入ボタン
				InsBtnClick_Lov_Item(dgv, e.RowIndex)
			Case EM_LOV_ITEM.emDelButton	 '削除ボタン
				DelBtnClick_Lov_Item(dgv, e.RowIndex)
		End Select
	End Sub

	''' <summary>
	''' マップ表示色ボタンが押されたら、色選択ダイアログを表示し、ボタンの色を変更
	''' </summary>
	''' <param name="dgv">対象のデータグリッドビュー</param>
	''' <param name="nColIndex">ボタンの列インデックス</param>
	''' <param name="nRowIndex">ボタンの行インデックス</param>
	''' <remarks></remarks>
	Private Sub GetDlgColor(ByVal dgv As DataGridView, ByVal nColIndex As Integer, ByVal nRowIndex As Integer)
		'色選択ダイアログの設定
		'色の作成を使用しない
		dlgColor.AllowFullOpen = False

		'色選択ダイアログの選択色を、ボタンの背景色にする
		dlgColor.Color = dgv(nColIndex, nRowIndex).Style.BackColor

		' 色選択ダイアログでokボタンが押されたら、ボタンの色を変更する 
		If (dlgColor.ShowDialog() = Windows.Forms.DialogResult.OK) Then
			'選択色をボタンの背景色に
			dgv(nColIndex, nRowIndex).Style.BackColor = dlgColor.Color
		End If

		dlgColor.Dispose()
	End Sub

	''' <summary>
	''' 挿入ボタン押されたときの処理
	''' </summary>
	''' <param name="dgv">対象のデータグリッドビュー</param>
	''' <param name="nRowIndex">レコードを挿入する位置</param>
	''' <remarks></remarks>
	Private Sub InsBtnClick_Lov_Item(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)

		' 挿入ボタンが使用可能かチェック
		If (dgv.Rows.Count - 1 = nRowIndex) Then
			' 最終行は使用不可なので抜ける
			Exit Sub
		End If

		Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
		frmMsg.SetSelectedFNo(0)

		Try
			'挿入するかメッセージを出す
			frmMsg.Text = "条件確認"
			frmMsg.ChangeMsg("挿入しますか？", Color.Black.ToArgb, True, True)
			If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

		Catch ex As Exception

		Finally
			If Not frmMsg Is Nothing Then
				frmMsg.Dispose()
				frmMsg = Nothing
			End If
		End Try

		' dgvにレコードを挿入
		InsertAt_Lov_Item(nRowIndex)

		'Noを振りなおし、ボタンを表示しなおす
		InitIndexdgvLov_Item(dgv)

	End Sub

	''' <summary>
	''' 削除ボタンが押されたときの処理
	''' </summary>
	''' <param name="dgv"></param>
	''' <param name="nRowIndex"></param>
	''' <remarks></remarks>
	Private Sub DelBtnClick_Lov_Item(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)

		Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
		' ボタン使用可能かチェック
		If (dgv.Rows.Count - 1 = nRowIndex) Then
			' 最終行は使用不可なので抜ける
			Exit Sub
		End If

		frmMsg.SetSelectedFNo(0)

		Try

			' 削除するかメッセージを出す
			frmMsg.Text = "条件確認"
			frmMsg.ChangeMsg("削除しますか？", Color.Black.ToArgb, True, True)
			If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

		Catch ex As Exception

		Finally
			If Not frmMsg Is Nothing Then
				frmMsg.Dispose()
				frmMsg = Nothing
			End If
		End Try

		' 行を削除
		dgv.Rows.RemoveAt(nRowIndex)

		' Noを振りなおし、ボタンを表示しなおす
		InitIndexdgvLov_Item(dgv)

	End Sub

	''' <summary>
	''' dgvLov_Itemのインデックスを振り、ボタンの表示設定をする
	''' </summary>
	''' <param name="dgv">疵種dgv</param>
	''' <remarks></remarks>
	Private Sub InitIndexdgvLov_Item(ByVal dgv As DataGridView)
		For ii As Integer = 0 To dgv.Rows.Count - 1
			dgv(EM_LOV_ITEM.emNo, ii).Value = ii + 1   ' 列のインデックス番号

			' 挿入ボタンの使用可能状態を設定    (ボタンを使用不可にはできないので、色を変更）
			If ii = dgv.RowCount - 1 Then
				' 最終行はボタンが使用不可
				dgv(EM_LOV_ITEM.emInsButton, ii).Style.ForeColor = Color.Gray
				dgv(EM_LOV_ITEM.emDelButton, ii).Style.ForeColor = Color.Gray
			Else
				' それ以外は使用可
				dgv(EM_LOV_ITEM.emInsButton, ii).Style.ForeColor = Color.Black
				dgv(EM_LOV_ITEM.emDelButton, ii).Style.ForeColor = Color.Black
			End If
		Next ii
	End Sub

	''' <summary>
	''' dgvの選択が変更されたら、セルの選択状態をキャンセル
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dataGridView_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)
		Dim dgv As DataGridView = CType(sender, DataGridView)	' 対象のdgv
		dgv.CurrentCell.Selected = False
	End Sub

	''' <summary>
	''' データグリッド表示項目変更
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvLov_Item_CellValueChanged(ByVal sender As Object, ByVal e As DataGridViewCellEventArgs) Handles dgvLov_Item.CellValueChanged
		Dim nWkx As Integer = 0
		If 0 > e.ColumnIndex Then Exit Sub
		'最終行のlov_idを追加した場合、次の入力エリアを追加する
		If EM_LOV_ITEM.emLov_id = e.ColumnIndex And (dgvLov_Item.Rows.Count - 1) = e.RowIndex Then
			If "" <> CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, e.RowIndex).Value) Then
				'空白行を追加
				InsertAt_Lov_Item(dgvLov_Item.RowCount)
				'Noを振りなおし、ボタンを表示しなおす
				InitIndexdgvLov_Item(dgvLov_Item)
			End If
		End If
	End Sub

	''' <summary>
	''' 更新ボタンの有効/無効
	''' </summary>
	''' <param name="bPassResult">パスワード入力結果 (True:有効 False:無効)</param>
	''' <remarks></remarks>
	Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

		btnF11.Enabled = bPassResult					' ファンクションキーの設定ボタン

	End Sub


	''' <summary>
	''' 画面に表示中のlov_idとDBのlov_idを比べて、削除されたものがあるかチェックする
	''' </summary>
	''' <remarks></remarks>
	Private Sub Data_CheckDelete()
		Dim bCheck As Boolean	'チェックフラグ Trueならlov_idがある
		Dim bDelGrid(10000 - 1) As Boolean		'削除対象グリッド
		Dim nChangeGrid(10000 - 1) As Integer	'変更対象のカウント数
		Dim strDelGID(10000 - 1) As String		'削除対象の画面ＩＤ
		Dim nDelGridID(10000 - 1) As Integer	'削除対象のグリッドＩＤ
		Dim nDelPos(10000 - 1) As Integer		'削除対象のRow位置
		Dim nDelKNo(10000 - 1) As Integer		'削除対象の項目Ｎｏ
		Dim nDelCnt As Integer					'削除太陽の件数

		m_nDelLov_id_Count = 0
		' テーブルで全行ループ
		For ii As Integer = 0 To m_Lov_Item.Tables(0).Rows.Count - 1
			' チェックフラグの初期化
			bCheck = False
			' 疵種dgvで全行ループ
			For kk As Integer = 0 To dgvLov_Item.Rows.Count - 1
				' 空白行以外で、比較
				If CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, kk).Value) <> "" AndAlso _
				 CInt(dgvLov_Item(EM_LOV_ITEM.emLov_id, kk).Value) = CInt(m_Lov_Item.Tables(0).Rows(ii).Item("lov_id")) Then
					' lov_idが一致したとき、チェックOK
					bCheck = True
					Exit For
				End If
			Next kk
			' チェックフラグがFalseのとき消されたlov_idなので、登録する
			If bCheck = False Then
				m_nDelLov_id(m_nDelLov_id_Count) = CInt(m_Lov_Item.Tables(0).Rows(ii).Item("lov_id"))
				m_nDelLov_id_Count = m_nDelLov_id_Count + 1
			End If
		Next ii
		For ii As Integer = 0 To m_nDelLov_id_Count - 1
			For jj As Integer = ii + 1 To m_nDelLov_id_Count - 1
				If m_nDelLov_id(ii) = m_nDelLov_id(jj) Then
					'lov_id 重複しているものはクリアする
					m_nDelLov_id(jj) = 0
				End If
			Next jj
		Next ii
		For ii As Integer = 0 To m_Grid.Tables(0).Rows.Count - 1
			bDelGrid(ii) = False
			nChangeGrid(ii) = 0
			strDelGID(ii) = ""
			nDelGridID(ii) = 0
			nDelPos(ii) = 0
			nDelKNo(ii) = 0
		Next ii
		nDelCnt = 0
		'削除対象となるlov_idに紐付くグリッドの情報を取得する
		For ii As Integer = 0 To m_nDelLov_id_Count - 1
			If 0 = m_nDelLov_id(ii) Then Continue For
			For jj As Integer = 0 To m_Grid.Tables(0).Rows.Count - 1
				If bDelGrid(jj) Then Continue For
				If m_Grid.Tables(0).Rows(jj).Item("lov_id") Is System.DBNull.Value Then Continue For
				If CInt(m_Grid.Tables(0).Rows(jj).Item("lov_id")) = m_nDelLov_id(ii) Then
					bDelGrid(jj) = True
					nDelPos(nDelCnt) = jj
					strDelGID(jj) = CStr(m_Grid.Tables(0).Rows(jj).Item("画面ID"))
					nDelGridID(jj) = CInt(m_Grid.Tables(0).Rows(jj).Item("グリッドID"))
					nDelKNo(jj) = CInt(m_Grid.Tables(0).Rows(jj).Item("項目No"))
					nDelCnt = nDelCnt + 1
				End If
			Next jj
		Next ii
        '削除足しようとなったグリッドに紐付く情報を取得する
        For ii As Integer = 0 To nDelCnt - 1
			For jj As Integer = 0 To m_Grid.Tables(0).Rows.Count - 1
				If strDelGID(nDelPos(ii)) = CStr(m_Grid.Tables(0).Rows(jj).Item("画面ID")) Then
					If nDelGridID(nDelPos(ii)) = CInt(m_Grid.Tables(0).Rows(jj).Item("グリッドID")) Then
						If nDelKNo(nDelPos(ii)) < CInt(m_Grid.Tables(0).Rows(jj).Item("項目No")) Then
							'項目Ｎｏが削除対象より大きいものを記憶する
							nChangeGrid(jj) = nChangeGrid(jj) + 1
						End If
					End If
				End If
			Next jj
		Next ii

		'グリッドの削除及び、情報の更新を行う
		For ii As Integer = 0 To m_Grid.Tables(0).Rows.Count - 1
			If nChangeGrid(ii) <> 0 And False = bDelGrid(ii) Then
				Dim drowGrid As DataRow = m_Grid.Tables(0).NewRow()
				''''''''''''''''''''''''''''''''''''''''''''
				Dim drowGridBase As DataRow = m_Grid.Tables(0).Rows(ii)												  ' 新規行
				drowGrid.Item("画面ID") = drowGridBase.Item("画面ID")
				drowGrid.Item("グリッドID") = drowGridBase.Item("グリッドID")
				drowGrid.Item("項目ヘッダー1") = drowGridBase.Item("項目ヘッダー1")
				drowGrid.Item("項目ヘッダー2") = drowGridBase.Item("項目ヘッダー2")
				drowGrid.Item("備考") = drowGridBase.Item("備考")
				drowGrid.Item("項目セルタイプ") = drowGridBase.Item("項目セルタイプ")
				drowGrid.Item("lov_id") = drowGridBase.Item("lov_id")
				drowGrid.Item("表示_項目表示有無") = drowGridBase.Item("表示_項目表示有無")
				drowGrid.Item("表示_読み取り専用") = drowGridBase.Item("表示_読み取り専用")
				drowGrid.Item("表示_項目幅") = drowGridBase.Item("表示_項目幅")
				drowGrid.Item("表示_項目配置") = drowGridBase.Item("表示_項目配置")
				drowGrid.Item("表示_フォーマット") = drowGridBase.Item("表示_フォーマット")
				drowGrid.Item("表示_列固定") = drowGridBase.Item("表示_列固定")
				drowGrid.Item("DB_テーブル名") = drowGridBase.Item("DB_テーブル名")
				drowGrid.Item("DB_フィールド名") = drowGridBase.Item("DB_フィールド名")
				drowGrid.Item("DB_主キー") = drowGridBase.Item("DB_主キー")
				drowGrid.Item("DB_フィールドタイプ") = drowGridBase.Item("DB_フィールドタイプ")
				drowGrid.Item("入力_入力可能最小値") = drowGridBase.Item("入力_入力可能最小値")
				drowGrid.Item("入力_入力可能最大値") = drowGridBase.Item("入力_入力可能最大値")
				drowGrid.Item("入力_NULL入力可否") = drowGridBase.Item("入力_NULL入力可否")
				drowGrid.Item("入力_入力可能桁数") = drowGridBase.Item("入力_入力可能桁数")
				drowGrid.Item("入力_小数点以下桁数") = drowGridBase.Item("入力_小数点以下桁数")
				drowGrid.Item("入力_表示ダイアログ") = drowGridBase.Item("入力_表示ダイアログ")
				''''''''''''''''''''''''''''''''''''''''''''
				drowGrid.Item("項目No") = CInt(drowGridBase.Item("項目No")) - nChangeGrid(ii)
				m_Grid.Tables(0).Rows(ii).Delete()
				m_Grid.Tables(0).Rows.Add(drowGrid)		'行を追加
			End If
			If bDelGrid(ii) Then
				m_Grid.Tables(0).Rows(ii).Delete()
			End If
		Next ii
	End Sub

End Class
