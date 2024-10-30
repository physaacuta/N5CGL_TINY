'*********************************************************************************
' 検査パターン条件設定画面クラス
'	[Ver]
'		Ver.01    2012/06/08  初版
'
'	[メモ]
'
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmPCodeJudge

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' データグリッドビューの列
    Private Enum EM_DATACOLUMN
        emTxt_No = 0                                        ' テキストボックス　条件No
        emTxt_MeireiNo                                      ' テキストボックス  命令No
        emTxt_CoilNo                                        ' テキストボックス  生産No
        emTxt_HinSyu                                        ' テキストボックス  品種
        emTxt_SyuZai                                        ' テキストボックス  主材質
        emTxt_Kensa                                         ' テキストボックス　検査基準
        emTxt_Youto                                         ' テキストボックス　用途
        emTxt_Hyoumen                                       ' テキストボックス　表面処理1桁目
        emTxt_TyuAtu_l                                      ' テキストボックス　注文厚_low
        emTxt_TyuAtu_h                                      ' テキストボックス　注文厚_high
        emTxt_Mukesaki                                      ' テキストボックス　向先
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' アダプター
    Private m_DS As New DataSet                                     ' 非接続型テーブル
    Private m_KeyCode As String = ""                                ' 主キー

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
    Private Sub frmPCodeJudge_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 0
                btnF01.Select()                 ' F01コンボボックス変更中にファンクションが押された時の対策
                ChangeCode(False)               ' 前項
            Case 1
                btnF02.Select()                 ' F02コンボボックス変更中にファンクションが押された時の対策
                ChangeCode(True)                ' 次項
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "検査パターン条件設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    Private Sub frmPCodeJudge_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
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
    ''' <remarks></remarks>
    Private Sub frmPCodeJudge_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                     ' メニューリスト生成
        modMain.SetLabelSysName(Me)                                 ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmPCodeJudge            ' 検査パターン条件設定画面呼出

        'タイトル設定
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' アップコードのコンボボックス
        MyBase.SetComboBox_Pcode(cmbUpCodeItem)

        ' コンボボックスの"デフォルト値(MAX_PCODE)を削除する
        MyBase.Delete_CodeEtc(cmbUpCodeItem, CStr(MAX_PCODE))

        MyBase.SetCmbUpCodeItem(cmbUpCodeItem, dgvUpCodeItem)

        ' ここの設定
        dgvInitialize(dgvData)

        ' このフォームで必要なデータを取得する
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       ' 終了
        End If

        ' コンボボックスに検査パターンをセット
        MyBase.SetComboBox_Pcode(cmbPCode)

        ' コンボボックスのデフォルト値(MAX_PCODE)を削除する
        MyBase.Delete_CodeEtc(cmbPCode, CStr(MAX_PCODE))

        If 0 < cmbPCode.Items.Count Then
            ' 前の画面で選択した状態をセット
            If cmbPCode.Items.Count - 1 < g_nPaternIndex Then
                ' 該当項目がない場合は、先頭を表示
                cmbPCode.SelectedIndex = 0
            Else
                cmbPCode.SelectedIndex = g_nPaternIndex
            End If
        Else
            ' 画面表示失敗なので、強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DBにデータが無い為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()

            modMain.TaskEnd()                       ' 終了
        End If

        ' 画面にパターン別の値を表示
        m_KeyCode = SelectComboBoxKey(cmbPCode.SelectedIndex, cmbPCode)
        Data_Disp(m_KeyCode)

        ' 選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
        dgvData.DataGridViewSelectionRowColorChange(g_ColorSelection)

        If dgvData.RowCount > 0 Then
            ' カーソルスタート位置は入力エリアの先頭
            dgvData(1, 0).Selected = True
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
    Private Sub frmPCodeJudge_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        ' 現在の表示中の画面が自分の画面ならアプリケーションを終了
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    ''' このフォームで必要なデータを取得する
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ' SQL文生成
        strSQL = "SELECT pcode"
        strSQL &= ",条件No"
        strSQL &= ",有効判定"
        strSQL &= ",命令No"
        strSQL &= ",生産No"
        strSQL &= ",注文板厚下限"
        strSQL &= ",注文板厚上限"
        strSQL &= ",検査基準"
        strSQL &= ",向先"
        strSQL &= ",用途"
        strSQL &= ",主材質"
        strSQL &= ",表面処理1桁目"
        strSQL &= ",品種"

        strSQL &= " FROM " & DB_PCODE_JUDGE
        strSQL &= " ORDER BY pcode ASC, 条件No ASC"

        ' SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PCODE_JUDGE & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <param name="key">主キーの値(pcode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Disp(ByVal key As String)
        ' 一括のテーブルなので、検査パターン毎の値のみ取り出し
        Dim strWhere As String = String.Format("pcode='{0}'", key)          ' WHERE句に相当の条件生成
        Dim drow() As DataRow = m_DS.Tables(0).Select(strWhere)

        ' 画面に表示
        dgvData.RowCount = drow.Length                  ' 基本的にマスターには 正常なレコードを登録済みのはず。汎用的にする為
        For ii As Integer = 0 To drow.Length - 1
            dgvData(EM_DATACOLUMN.emTxt_No, ii).Value = drow(ii).Item("条件No")

            dgvData(EM_DATACOLUMN.emTxt_MeireiNo, ii).Value = drow(ii).Item("命令No").ToString
            dgvData(EM_DATACOLUMN.emTxt_CoilNo, ii).Value = drow(ii).Item("生産No").ToString
            dgvData(EM_DATACOLUMN.emTxt_HinSyu, ii).Value = drow(ii).Item("品種").ToString
            dgvData(EM_DATACOLUMN.emTxt_SyuZai, ii).Value = drow(ii).Item("主材質").ToString
            dgvData(EM_DATACOLUMN.emTxt_Kensa, ii).Value = drow(ii).Item("検査基準").ToString
            dgvData(EM_DATACOLUMN.emTxt_Youto, ii).Value = drow(ii).Item("用途").ToString
            dgvData(EM_DATACOLUMN.emTxt_Hyoumen, ii).Value = drow(ii).Item("表面処理1桁目").ToString
            dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value = drow(ii).Item("注文板厚下限").ToString
            dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value = drow(ii).Item("注文板厚上限").ToString
            dgvData(EM_DATACOLUMN.emTxt_Mukesaki, ii).Value = drow(ii).Item("向先").ToString
        Next ii
    End Sub

    ''' <summary>
    ''' 検査パターン変更時
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbPCode_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbPCode.SelectedIndexChanged
        ' 今の選択と主キーが同じ場合は無視 (選択して入力異常だったので 元に戻したよ の時の対策)
        If m_KeyCode = cmbPCode.Text Then Exit Sub

        '' 画面のデータを非接続型テーブルにセット
        ' 初期画面表示時には、データ無しの為、画面のデータを取り込まないよ
        If m_KeyCode <> "" Then
            ' 入力値チェック
            If Not Data_Check(m_KeyCode) Then
                ' 入力異常時は、選択位置も戻す
                cmbPCode.Text = m_KeyCode
                Exit Sub
            End If

            ' 変更前のキーに対し 画面データ取り込み
            Data_Get(m_KeyCode)
        End If

        ' 選択変更に伴い ラベル値変更
        MyBase.SelectComboBoxItem(cmbPCode.SelectedIndex, cmbPCode, lblPCode)

        ' 画面にパターン別の値を表示
        m_KeyCode = SelectComboBoxKey(cmbPCode.SelectedIndex, cmbPCode)
        Data_Disp(m_KeyCode)

        '現在の状態を保存
        g_nPaternIndex = cmbPCode.SelectedIndex

    End Sub

    ''' <summary>
    ''' 確定処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' 今の画面表示中の値と取り込み
        If m_KeyCode <> "" Then
            ' 入力値チェック
            If Not Data_Check(m_KeyCode) Then Exit Sub

            Data_Get(m_KeyCode)
        End If

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
    ''' <param name="key">主キーの値(pcode) 今の所使わないかなー</param>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_Check(ByVal key As String) As Boolean
        Dim strWk As String
        Dim strMsg As String

        ' 条件No分ループ
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            ' 命令No
            strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_MeireiNo, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "条件No=" & ii + 1 & " 命令No") <> 0 Then Return False
            dgvData(EM_DATACOLUMN.emTxt_MeireiNo, ii).Value = strWk

            ' コイルNo
            strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_CoilNo, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "条件No=" & ii + 1 & " コイルNo") <> 0 Then Return False
            dgvData(EM_DATACOLUMN.emTxt_CoilNo, ii).Value = strWk

            ' 品種
            strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_HinSyu, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "条件No=" & ii + 1 & " 品種") <> 0 Then Return False
            dgvData(EM_DATACOLUMN.emTxt_HinSyu, ii).Value = strWk

            ' 主材質
            strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_SyuZai, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "条件No=" & ii + 1 & " 主材質") <> 0 Then Return False
            dgvData(EM_DATACOLUMN.emTxt_SyuZai, ii).Value = strWk

            ' 検査基準
            strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_Kensa, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "条件No=" & ii + 1 & " 検査基準") <> 0 Then Return False
            dgvData(EM_DATACOLUMN.emTxt_Kensa, ii).Value = strWk

            ' 用途
            strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_Youto, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "条件No=" & ii + 1 & " 用途") <> 0 Then Return False
            dgvData(EM_DATACOLUMN.emTxt_Youto, ii).Value = strWk

            ' 表面処理1桁目
            strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_Hyoumen, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "条件No=" & ii + 1 & " 表面処理1桁目") <> 0 Then Return False
            dgvData(EM_DATACOLUMN.emTxt_Hyoumen, ii).Value = strWk

            ' 注文板厚下限
            If System.DBNull.Value IsNot dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value Then
                strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value)
                If "" <> strWk Then
                    If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Integer.MaxValue, False, "条件No=" & ii + 1 & " 板厚下限値") <> 0 Then Return False
                    dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value = strWk
                End If
            End If

            ' 注文板厚上限
            If System.DBNull.Value IsNot dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value Then
                strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value)
                If "" <> strWk Then
                    If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Integer.MaxValue, False, "条件No=" & ii + 1 & " 板厚上限値") <> 0 Then Return False
                    dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value = strWk
                End If
            End If

            ' 注文板厚下限、上限チェック
            If System.DBNull.Value IsNot dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value And
                System.DBNull.Value IsNot dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value Then
                If "" <> CStr(dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value) And
                    "" <> CStr(dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value) Then

                    If CInt(dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value) >
                     CInt(dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value) Then

                        strMsg = "【 条件No=" & ii + 1 & " 板厚下限値 】" & vbCrLf
                        strMsg &= "板厚上限値を超えています。" & vbCrLf
                        strMsg &= "板厚上限値以下に設定してください。"
                        Call MsgBox(strMsg, MsgBoxStyle.OkOnly, "入力チェック")
                        Return False
                    End If
                End If
            End If

            ' 向先
            strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_Mukesaki, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "条件No=" & ii + 1 & " 向先") <> 0 Then Return False
            dgvData(EM_DATACOLUMN.emTxt_Mukesaki, ii).Value = strWk

        Next ii
        Return True
    End Function

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <param name="key">主キーの値(pcode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Get(ByVal key As String)
        Dim strWhere As String                          ' WHERE句に相当の条件生成
        Dim drow As DataRow

        ' 条件No分ループ
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            strWhere = String.Format("pcode='{0}' and 条件No={1}", key, ii + 1)     ' WHERE句に相当の条件生成
            drow = m_DS.Tables(0).Select(strWhere)(0)           ' 主キーを指定しての取り出しなので、一件しかない

            '''' 条件チェック後セット
            ' 条件が全部空白なら無効行と認識
            Dim umu As Integer

            If (dgvData(EM_DATACOLUMN.emTxt_MeireiNo, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_MeireiNo, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_CoilNo, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_CoilNo, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_HinSyu, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_HinSyu, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_SyuZai, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_SyuZai, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_Kensa, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_Kensa, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_Youto, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_Youto, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_Hyoumen, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_Hyoumen, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_Mukesaki, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_Mukesaki, ii).Value.ToString.Trim = "") Then
                umu = 0                                                 ' 無効
            Else
                umu = 1													' 有効
			End If

            ' セット
            drow.Item("有効判定") = umu
            drow.Item("命令No") = dgvData(EM_DATACOLUMN.emTxt_MeireiNo, ii).Value
            drow.Item("生産No") = dgvData(EM_DATACOLUMN.emTxt_CoilNo, ii).Value
            drow.Item("品種") = dgvData(EM_DATACOLUMN.emTxt_HinSyu, ii).Value
            drow.Item("主材質") = dgvData(EM_DATACOLUMN.emTxt_SyuZai, ii).Value
            drow.Item("検査基準") = dgvData(EM_DATACOLUMN.emTxt_Kensa, ii).Value
            drow.Item("用途") = dgvData(EM_DATACOLUMN.emTxt_Youto, ii).Value
            drow.Item("表面処理1桁目") = dgvData(EM_DATACOLUMN.emTxt_Hyoumen, ii).Value

            If Nothing Is dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value OrElse
            "" = dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value.ToString.Trim Then
                drow.Item("注文板厚下限") = System.DBNull.Value
            Else
                drow.Item("注文板厚下限") = CInt(dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value)
            End If

            If Nothing Is dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value OrElse
            "" = dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value.ToString.Trim Then
                drow.Item("注文板厚上限") = System.DBNull.Value
            Else
                drow.Item("注文板厚上限") = CInt(dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value)
            End If

            drow.Item("向先") = dgvData(EM_DATACOLUMN.emTxt_Mukesaki, ii).Value

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
            m_Adapter.SelectCommand.Connection.Open()

            'トランザクション開始
            transact = m_Adapter.SelectCommand.Connection.BeginTransaction()
            m_Adapter.SelectCommand.Transaction = transact

            Using cbWk As New SqlClient.SqlCommandBuilder()

                ' コマンドアダプタをセットして、更新
                cbWk.DataAdapter = m_Adapter
                m_Adapter.Update(m_DS)

            End Using

            'コミット
            transact.Commit()

            ' 統括機能へパラメータ変更通知
            modMain.Send_Main(CH_PARAM_SETTIG_PCODE)
            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "設定エラー[" & ex.Message & "] " & Me.Text)

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
    Private Overloads Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し
        Call dgv.DataGridViewDataExec(dgv)

        With dgv
            .AllowUserToResizeColumns = True                                ' ユーザー 列サイズ変更
            .ScrollBars = ScrollBars.Both                                   ' スクロールバーの表示

            '''' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      ' 選択色をバックカラーに
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     ' 選択色も一行おきに色変え

            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' 列定義
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 42             ' ヘッダーの高さ

        End With

        '' 列定義

        ' 条件No
        dgvColSetText(dgv, "条件NO", 60, True, True, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' 命令No
        dgvColSetText(dgv, "命令No" & vbNewLine & "[正規表現許可]", 140, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' コイルNo
        dgvColSetText(dgv, "コイルNo" & vbNewLine & "[正規表現許可]", 140, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' 品種
        dgvColSetText(dgv, "品種" & vbNewLine & "[正規表現許可]", 140, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' 主材質
        dgvColSetText(dgv, "主材質" & vbNewLine & "[正規表現許可]", 140, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' 検査基準
        dgvColSetText(dgv, "検査基準" & vbNewLine & "[正規表現許可]", 140, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' 用途
        dgvColSetText(dgv, "用途" & vbNewLine & "[正規表現許可]", 140, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' 表面処理1桁目
        dgvColSetText(dgv, "表面処理1桁目" & vbNewLine & "[正規表現許可]", 140, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' 注文板厚下限
        dgvColSetText(dgv, "板厚下限値" & vbNewLine & "[μm]", 120, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' 注文板厚上限
        dgvColSetText(dgv, "板厚上限値" & vbNewLine & "[μm]", 120, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' 向先
        dgvColSetText(dgv, "向先" & vbNewLine & "[正規表現許可]", 140, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ その他こまごました操作 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 前項 次項
    ''' </summary>
    ''' <param name="bln">前項:false 次項:true</param>
    ''' <remarks></remarks>
    Private Sub ChangeCode(ByVal bln As Boolean)
        Dim rec As Integer = CInt(IIf(bln, cmbPCode.SelectedIndex + 1, cmbPCode.SelectedIndex - 1))
        ' 上下限チェック
        If rec < 0 OrElse rec >= cmbPCode.Items.Count Then Exit Sub
        ' 切替
        cmbPCode.SelectedIndex = rec
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
    Private Sub btnUpCodeItem_UpDate_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnUpCodeItem_UpDate.Click
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing
        Dim strSQL As String = ""

        '''' 条件チェック
        ' 元
        Dim strFromKey As String = ""                                    ' コピー元キー 
        If cmbUpCodeItem.SelectedIndex < 0 Then Exit Sub '元
        strFromKey = SelectComboBoxKey(cmbUpCodeItem.SelectedIndex, cmbUpCodeItem)
        ' 先
        Dim strToKey As String = MyBase.GetUpCodeItem_To(False)          ' コピー先キー
        If strToKey.Length = 0 Then Exit Sub

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '''' コピー
            If CopyDB(strFromKey, strToKey) Then
                ' 統括機能へパラメータ変更通知
                modMain.Send_Main(CH_PARAM_SETTIG_PCODE)
                modMain.HardCopy(Me)

                frmMsg.Text = "反映処理"
                frmMsg.ChangeMsg("正常に登録しました", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            Else
                frmMsg.Text = "反映処理"
                frmMsg.ChangeMsg("登録に失敗", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

            '''' データの再取得
            If Not DB_Select() Then
                ' DB 失敗なので、画面強制終了
                frmMsg.Text = "異常"
                frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       ' 終了
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        '''' 画面再表示
        Data_Disp(m_KeyCode)

    End Sub

    ''' <summary>
    ''' コピー処理
    ''' </summary>
    ''' <param name="strFromKey">コピー元キー </param>
    ''' <param name="strToKey">コピー先キー </param>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function CopyDB(ByVal strFromKey As String, ByVal strToKey As String) As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        '''' SQL文の生成
        strSQL = "UPDATE " & DB_PCODE_JUDGE & " SET "
        ' 可変列
        strSQL &= tClass_SQLServer.sqlColumnUpdate("有効判定", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("命令No", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("生産No", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("注文板厚下限", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("注文板厚上限", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("検査基準", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("向先", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("用途", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("主材質", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("表面処理1桁目", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("品種", "b")

        tClass_SQLServer.sqlColumnTrim(strSQL)      ' おまじない 必ず書いてね

        ' From句
        strSQL &= "FROM " & DB_PCODE_JUDGE & " a "
        strSQL &= ", (SELECT * FROM " & DB_PCODE_JUDGE
        strSQL &= " WHERE pcode='" & strFromKey & "') b "

        ' WHERE句
        strSQL &= "WHERE a.条件No=b.条件No AND a.pcode IN(" & strToKey & ")"

        ' SQL文実行
        If 0 > MyBase.UpCode_UpDate(strSQL, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PCODE_JUDGE & " 更新エラー [" & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' 更新ボタンの有効/無効
    ''' </summary>
    ''' <param name="bPassResult">パスワード入力結果 (True:有効 False:無効)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)
        btnF11.Enabled = bPassResult                    ' ファンクションキーの設定ボタン
        pnlItem.Enabled = bPassResult                   ' コピー用リストの反映ボタン
    End Sub
End Class
