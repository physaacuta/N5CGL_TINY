'*********************************************************************************
' 縮小率設定
'	[Ver]
'		Ver.01    2017/01/10  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcReduction
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' データグリッドビューの列
    Private Enum em_DataColumn
        emTxt_Order = 0                 ' テキストボックス  No
        emTxt_ID                        ' テキストボックス  縮小率ID
        emTxt_Name                      ' テキストボックス  名称
        emTxt_MeanX                     ' テキストボックス  平均サイズX
        emTxt_MeanY                     ' テキストボックス  平均サイズY
        emTxt_RatioX                    ' テキストボックス  縮小率X
        emTxt_RatioY                    ' テキストボックス  縮小率Y
        iInsButton                      ' 挿入ボタン
        iDelButton                      ' 削除ボタン
    End Enum

    '削除対象のID
    Private Structure TypeStruct
        Dim nID As Integer        ' 削除対象のID

        Public Sub initialize()
            nID = 0
        End Sub

    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' アダプター
    Private m_DS As New DataSet                                     ' 非接続型テーブル（多重分解能）
    Private m_AdapterReduction As New SqlClient.SqlDataAdapter      ' アダプター
    Private m_DsReduction As New DataSet                            ' 非接続型テーブルPARAM_REDUCTION
    Private mlst_DelID As New List(Of TypeStruct)                   ' 削除対象のID

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
    Private Sub frmEpcReduction_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "縮小率設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    ''' <param name="cancel">選択を取りやめたい場合は、Trueをセット</param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' いろいろ開放処理
        m_DS.Dispose()
        If Not m_Adapter Is Nothing Then
            m_Adapter.Dispose()
            m_Adapter = Nothing
        End If
        m_DsReduction.Dispose()
        If Not m_AdapterReduction Is Nothing Then
            m_AdapterReduction.Dispose()
            m_AdapterReduction = Nothing
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
    Private Sub frmEpcReduction_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        'タイトル設定
        Call SetTitleName(Me)

        modMain.SetListMenu(Me)                                     ' メニューリスト生成
        modMain.SetLabelSysName(Me)                                 ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmEpcReduction          ' 上位情報内容設定呼び出し

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' 更新ボタンの有効/無効
        Call UpdateEnabled(g_bPassFlg)

        ' このフォームで必要なデータを取得する
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()
        End If

        'dgv初期化
        dgvInitialize(dgvReduction)
        dgvColunmDef(dgvReduction)

        ' 画面に値を表示
        Data_Disp()

        ' 選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
        MyBase.DataGridViewSelectionRowColorChange(dgvReduction)

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
        strSQL = "SELECT id, "
        strSQL &= "name, MeanX, MeanY, RatioX, RatioY "
        strSQL &= " FROM " & DB_PARAM_REDUCTION
        strSQL &= " ORDER BY ID ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsReduction, m_AdapterReduction, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_REDUCTION & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' <20221213> <溝口> <最終工程高機能化その1> --->>>
        '' SQL文生成
        'strSQL = "SELECT scode"
        'strSQL &= ",camangle"
        'strSQL &= ",id"
        'strSQL &= ",ReductionId"
        'strSQL &= ",FilterId"
        'strSQL &= ",name"

        'strSQL &= " FROM {0}"
        'strSQL &= " ORDER BY id ASC"

        '' SQL文実行（多重分解能設定）
        'If 0 > MyBase.Get_SelectData(String.Format(strSQL, DB_PARAM_THRESHOLD), m_DS, m_Adapter, strErrMsg) Then
        '    gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_THRESHOLD & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
        '    Return False
        'End If

        '' SQL文生成
        strSQL = "SELECT scode"
        strSQL &= ",id_filterbin"
        strSQL &= ",ReductionId"
        strSQL &= ",FilterId"
        strSQL &= ",name"

        strSQL &= " FROM {0}"
        strSQL &= " ORDER BY id_filterbin ASC"

        ' SQL文実行（多重分解能設定）
        If 0 > MyBase.Get_SelectData(String.Format(strSQL, DB_PARAM_FILTERBIN), m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_FILTERBIN & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If
        ' <20221213> <溝口> <最終工程高機能化その1> <<<---

        Return True

    End Function

    ''' <summary>
    ''' アップデート
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Try
            '値を登録
            If Not DB_DeleteUp() Then
                Return False
            Else
                modMain.Send_Main(CH_PARAM_REDUCTION)
            End If

            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "設定エラー [" & ex.Message & "] " & Me.Text)
            Return False
        End Try
    End Function

    ''' <summary>
    ''' dgvにデータ行を挿入
    ''' </summary>
    ''' <param name="nRowIndex">行の挿入位置</param>
    ''' <remarks></remarks>
    Private Sub InsertAt_TypeData(ByVal nRowIndex As Integer)
        ' dgvReductionにレコードを挿入
        dgvReduction.Rows.Insert(nRowIndex)

        ' データ初期化
        dgvReduction(em_DataColumn.emTxt_ID, nRowIndex).Value = ""         ' 縮小率ID
        dgvReduction(em_DataColumn.emTxt_Name, nRowIndex).Value = ""       ' 名称
        dgvReduction(em_DataColumn.emTxt_MeanX, nRowIndex).Value = ""      ' 平均サイズX
        dgvReduction(em_DataColumn.emTxt_MeanY, nRowIndex).Value = ""      ' 平均サイズY
        dgvReduction(em_DataColumn.emTxt_RatioX, nRowIndex).Value = ""     ' 縮小率X
        dgvReduction(em_DataColumn.emTxt_RatioY, nRowIndex).Value = ""     ' 縮小率Y

    End Sub

    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()
        'データを表示
        Data_DispdgvReduction(dgvReduction)
        'インデックスとボタンの表示設定
        InitIndexdgvReduction(dgvReduction)

        ' 行の色を変える （初期表示で色が変わる対策）
        dgvReduction.CurrentRow.DefaultCellStyle.BackColor = Color.Empty

    End Sub

    ''' <summary>
    ''' dgvにデータを表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_DispdgvReduction(ByVal dgv As DataGridView)
        Dim drowReduction() As DataRow = m_DsReduction.Tables(0).Select()         ' 全レコードを表示するためWHERE句なし

        '行を初期化
        dgv.Rows.Clear()
        ' 画面に表示
        dgv.RowCount = MAX_REDUCTION                                       ' dgvのレコード数は固定

        ' 画面に表示
        Try

            For ii As Integer = 0 To drowReduction.Length - 1
                If ii < dgv.RowCount Then
                    dgv(em_DataColumn.emTxt_ID, ii).Value = drowReduction(ii).Item("id")              ' 縮小率ID
                    dgv(em_DataColumn.emTxt_Name, ii).Value = drowReduction(ii).Item("name")          ' 名称
                    dgv(em_DataColumn.emTxt_MeanX, ii).Value = drowReduction(ii).Item("MeanX")        ' 平均サイズX
                    dgv(em_DataColumn.emTxt_MeanY, ii).Value = drowReduction(ii).Item("MeanY")        ' 平均サイズY
                    dgv(em_DataColumn.emTxt_RatioX, ii).Value = drowReduction(ii).Item("RatioX")      ' 縮小率X
                    dgv(em_DataColumn.emTxt_RatioY, ii).Value = drowReduction(ii).Item("RatioY")      ' 縮小率Y
                End If
            Next ii

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_PCINFO_DESC & " 表示エラー [" & ex.Message & "] " & Me.Text)
        End Try

        ' カーソルスタート位置は入力エリアの先頭
        dgv(1, 0).Selected = True

        Return
    End Sub

    ''' <summary>
    ''' 入力値チェック
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strWk As String     'dgvのデータ格納用
        Dim strMsg As String    '重複用のエラーメッセージ

        'dgv行ループ
        For ii As Integer = 0 To dgvReduction.Rows.Count - 1
            'IDが未入力であればチェック不要
            If Trim(CStr(dgvReduction(em_DataColumn.emTxt_ID, ii).Value)) = "" Then
                Continue For
            End If
            ' 縮小率ID
            strWk = CStr(dgvReduction(em_DataColumn.emTxt_ID, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 999999, True, "縮小率設定 No=" & ii + 1 & " 縮小率ID") <> 0 Then Return False
            dgvReduction(em_DataColumn.emTxt_ID, ii).Value = strWk

            ' 重複チェック
            For jj As Integer = ii + 1 To dgvReduction.Rows.Count - 1
                ' 比較先のIDが数値なら比較
                If IsNumeric(dgvReduction(em_DataColumn.emTxt_ID, jj).Value) Then
                    ' IDが同じならメッセージを出して終了
                    If CInt(dgvReduction(em_DataColumn.emTxt_ID, ii).Value) = CInt(dgvReduction(em_DataColumn.emTxt_ID, jj).Value) Then
                        strMsg = "【 縮小率設定 】" & vbNewLine
                        strMsg &= "同じIDは登録できません。" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next

            '名称
            strWk = CStr(dgvReduction(em_DataColumn.emTxt_Name, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 256, "縮小率設定 No=" & ii + 1 & " 名称") <> 0 Then Return False
            dgvReduction(em_DataColumn.emTxt_Name, ii).Value = strWk

            '名称の重複チェック
            For jj As Integer = ii + 1 To dgvReduction.Rows.Count - 1
                '名称が同じならメッセージを出して終了
                If CStr(dgvReduction(em_DataColumn.emTxt_Name, ii).Value) = CStr(dgvReduction(em_DataColumn.emTxt_Name, jj).Value) Then
                    strMsg = "【 縮小率設定 】" & vbNewLine
                    strMsg &= "同じ名称は登録できません。" & vbNewLine
                    strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                    MsgBox(strMsg)
                    Return False
                End If
            Next

            '平均サイズX
            strWk = CStr(dgvReduction(em_DataColumn.emTxt_MeanX, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 512, False, "縮小率設定 No=" & ii + 1 & " 平均サイズX") <> 0 Then Return False
            dgvReduction(em_DataColumn.emTxt_MeanX, ii).Value = strWk

            '平均サイズY
            strWk = CStr(dgvReduction(em_DataColumn.emTxt_MeanY, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 512, False, "縮小率設定 No=" & ii + 1 & " 平均サイズY") <> 0 Then Return False
            dgvReduction(em_DataColumn.emTxt_MeanY, ii).Value = strWk

            '縮小率X
            strWk = CStr(dgvReduction(em_DataColumn.emTxt_RatioX, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 256, False, "縮小率設定 No=" & ii + 1 & " 縮小率X") <> 0 Then Return False
            dgvReduction(em_DataColumn.emTxt_RatioX, ii).Value = strWk

            '縮小率Y
            strWk = CStr(dgvReduction(em_DataColumn.emTxt_RatioY, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 256, False, "縮小率設定 No=" & ii + 1 & " 縮小率Y") <> 0 Then Return False
            dgvReduction(em_DataColumn.emTxt_RatioY, ii).Value = strWk

        Next ii

        Return True

    End Function

    ''' <summary>
    ''' 画面に表示中のIDとDBのIDを比べて、削除されたIDがあるかチェックする
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_CheckDelete()
        Dim drowReduction() As DataRow = m_DsReduction.Tables(0).Select()
        Dim bCheck As Boolean   'チェックフラグ TrueならIDがある
        Dim typ_Type As TypeStruct = Nothing

        ' 非接続テーブルで全行ループ
        For ii As Integer = 0 To drowReduction.Length - 1
            ' チェックフラグの初期化
            bCheck = False
            ' dgvで全行ループ
            For kk As Integer = 0 To dgvReduction.Rows.Count - 1

                typ_Type.nID = CInt(drowReduction(ii).Item("id"))      ' 縮小率ID

                ' IDが空白行以外で、比較
                If CStr(dgvReduction(em_DataColumn.emTxt_ID, kk).Value) <> "" AndAlso _
                    CInt(dgvReduction(em_DataColumn.emTxt_ID, kk).Value) = CInt(typ_Type.nID) Then
                    ' IDが一致したとき、チェックOK
                    bCheck = True
                    ' 削除したキーがまたあった場合
                    If mlst_DelID.Contains(typ_Type) Then
                        '削除リストより削除
                        mlst_DelID.Remove(typ_Type)
                    End If
                    Exit For
                End If
            Next kk

            ' チェックフラグがFalseのとき消されたIDなので、登録する
            If bCheck = False Then
                typ_Type.initialize()
                ' 削除対象のIDを格納
                typ_Type.nID = CInt(drowReduction(ii).Item("id"))

                ' 既にキーがあった場合
                If mlst_DelID.Contains(typ_Type) Then
                    '削除リストより削除
                    mlst_DelID.Remove(typ_Type)
                End If
                mlst_DelID.Add(typ_Type)
            End If
        Next ii
    End Sub

    ''' <summary>
    ''' 削除対象IDが閾値上に登録されているかチェックを行う
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function DelIDSetUseCheck() As Boolean
        Dim strWhere As String
        Dim typ_Type As TypeStruct = Nothing
        Dim strMsg As String    'エラーメッセージ
        For ii As Integer = 0 To mlst_DelID.Count - 1
            Dim drow() As DataRow                                        ' データ行
            typ_Type = mlst_DelID(ii)
            strWhere = String.Format("ReductionId='{0}'", typ_Type.nID)          ' WHERE句に相当の条件生成
            drow = m_DS.Tables(0).Select(strWhere)
            If 1 <= drow.Length Then
                '登録済みのため異常とする
                strMsg = "縮小率ID= " & CStr(typ_Type.nID) & " は削除できません" & vbNewLine
                strMsg &= "多重分解能閾値設定にて使用しています。"
                MsgBox(strMsg)
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
        Dim drowReduction() As DataRow = m_DsReduction.Tables(0).Select()
        Dim nRowCount As Integer    '削除対象のデータ数

        '現在画面に表示している前工程コードの非接続テーブルのデータを削除(後ろから削除)
        nRowCount = drowReduction.Length
        For ii As Integer = 1 To nRowCount
            m_DsReduction.Tables(0).Rows.Remove(drowReduction(nRowCount - ii))
        Next

        ' dgvの値を非接続テーブルへ格納
        For ii As Integer = 0 To dgvReduction.RowCount - 1
            'ID が空白以外の行だけ取得
            If CStr(dgvReduction(em_DataColumn.emTxt_ID, ii).Value) = "" Then Continue For
            Dim drowNew As DataRow = m_DsReduction.Tables(0).NewRow()    '新規行

            'データをセット
            drowNew.Item("id") = CInt(dgvReduction(em_DataColumn.emTxt_ID, ii).Value)               ' 縮小率ID
            drowNew.Item("name") = CStr(dgvReduction(em_DataColumn.emTxt_Name, ii).Value)           ' 名称
            drowNew.Item("MeanX") = CInt(dgvReduction(em_DataColumn.emTxt_MeanX, ii).Value)         ' 平均サイズX
            drowNew.Item("MeanY") = CInt(dgvReduction(em_DataColumn.emTxt_MeanY, ii).Value)         ' 平均サイズY
            drowNew.Item("RatioX") = CInt(dgvReduction(em_DataColumn.emTxt_RatioX, ii).Value)       ' 縮小率X
            drowNew.Item("RatioY") = CInt(dgvReduction(em_DataColumn.emTxt_RatioY, ii).Value)       ' 縮小率Y
            'テーブルに追加
            m_DsReduction.Tables(0).Rows.InsertAt(drowNew, m_DsReduction.Tables(0).Rows.Count)
        Next
    End Sub

    ''' <summary>
    ''' 確定処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        '入力値チェック
        If Not Data_Check() Then Exit Sub

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try
            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '画面から削除されたIDを取得
            Data_CheckDelete()

            ' 削除対象IDが閾値上に登録されているかチェックを行う
            If Not DelIDSetUseCheck() Then
                '画面に値を表示しなおす
                Data_Disp()
                Exit Sub
            End If

            '画面のデータを取得
            Data_Get()

            '画面から削除されたIDが再度登録されたかを取得
            Data_CheckDelete()

            'ホントに登録
            If DB_UpDate() Then
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("正常に登録しました", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            Else
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("登録に失敗", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

            'このフォームで必要なデータを取得する
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

        ' 設定の初期化
        Data_Init()

        '画面に値を表示
        Data_Disp()

    End Sub

    ''' <summary>
    ''' データグリッドビュー初期化
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        ''''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        With dgv
            .ColumnHeadersHeight = 42                                       ' ヘッダーの高さ
            .AllowUserToResizeColumns = True                            ' ユーザー 列サイズ変更
            .ScrollBars = ScrollBars.Both                                   ' スクロールバーの表示

            ' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '一行おきに選択色変更
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
        End With


    End Sub

    ''' <summary>
    ''' dgvの列定義
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub dgvColunmDef(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        With dgv
            ''=================================
            '' 列定義
            ' No
            dgvColSetText(dgv, "No", 50, True, True, _
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
            ' 縮小率ID
            dgvColSetText(dgv, "縮小率ID", 100, True, False, _
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

            ' 名称
            dgvColSetText(dgv, "名称", 260, True, False, _
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

            ' 平均サイズX
            dgvColSetText(dgv, "平均サイズX" & vbNewLine & "( 1 - 512 )", 110, False, False, _
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

            ' 平均サイズY
            dgvColSetText(dgv, "平均サイズY" & vbNewLine & "( 1 - 512 )", 110, False, False, _
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

            ' 縮小率X
            dgvColSetText(dgv, "縮小率X" & vbNewLine & "( 1 - 256 )", 110, False, False, _
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

            ' 縮小率Y
            dgvColSetText(dgv, "縮小率Y" & vbNewLine & "( 1 - 256 )", 110, False, False, _
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

            ' 上へ挿入
            dgvColSetButton(dgv, "上へ" & vbNewLine & "挿入", 50, False, False, _
                            DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
                            New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "挿入")

            ' 削除
            dgvColSetButton(dgv, "削除", 50, False, False, _
                            DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
                            New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "削除")

        End With
    End Sub


    ''' <summary>
    ''' dgvのセルのボタンが押されたときの処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvReduction_A_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvReduction.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' 行がクリックされていなければ終了
        If e.RowIndex < 0 Then Exit Sub

        ' 列インデックスでどの列がクリックされたか判別
        Select Case e.ColumnIndex
            Case em_DataColumn.iInsButton   '上へ挿入ボタン
                InsBtnClick_Type(dgv, e.RowIndex)
            Case em_DataColumn.iDelButton   '削除ボタン
                DelBtnClick_Type(dgv, e.RowIndex)
        End Select
    End Sub

    ''' <summary>
    ''' dgvの挿入ボタン押されたときの処理
    ''' </summary>
    ''' <param name="dgv">対象のデータグリッドビュー</param>
    ''' <param name="nRowIndex">レコードを挿入する位置</param>
    ''' <remarks></remarks>
    Private Sub InsBtnClick_Type(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)
        ' 挿入ボタンが使用可能かチェック
        If (dgv.Rows.Count - 1 = nRowIndex) Then
            ' 最終行は使用不可なので抜ける
            Exit Sub
        End If

        '最終行のテキストボックスに何かしらのデータが入っている場合は挿入不可
        If Trim(CStr(dgv(em_DataColumn.emTxt_ID, dgv.Rows.Count - 1).Value)) <> "" Or _
        Trim(CStr(dgv(em_DataColumn.emTxt_Name, dgv.Rows.Count - 1).Value)) <> "" Or _
        Trim(CStr(dgv(em_DataColumn.emTxt_MeanX, dgv.Rows.Count - 1).Value)) <> "" Or _
        Trim(CStr(dgv(em_DataColumn.emTxt_MeanY, dgv.Rows.Count - 1).Value)) <> "" Or _
        Trim(CStr(dgv(em_DataColumn.emTxt_RatioX, dgv.Rows.Count - 1).Value)) <> "" Or _
        Trim(CStr(dgv(em_DataColumn.emTxt_RatioY, dgv.Rows.Count - 1).Value)) <> "" Then
            ' <<----------------
            MsgBox("最終行にデータが存在します。")
            Exit Sub
        End If

        '挿入するかメッセージを出す
        If MsgBox("挿入しますか？", MsgBoxStyle.YesNo Or MsgBoxStyle.DefaultButton1, "条件確認") = MsgBoxResult.No Then Exit Sub

        ' dgvにレコードを挿入
        InsertAt_TypeData(nRowIndex)

        ' dgvの最終行を削除
        dgv.Rows.RemoveAt(dgvReduction.Rows.Count - 1)

        'Noを振りなおしボタンを表示しなおす
        InitIndexdgvReduction(dgv)
    End Sub

    ''' <summary>
    ''' dgvの削除ボタンが押されたときの処理
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="nRowIndex"></param>
    ''' <remarks></remarks>
    Private Sub DelBtnClick_Type(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)
        '削除するかメッセージを出す
        If MsgBox("削除しますか？", MsgBoxStyle.YesNo Or MsgBoxStyle.DefaultButton1, "条件確認") = MsgBoxResult.No Then Exit Sub

        ' dgvの最終行にレコードを挿入
        InsertAt_TypeData(dgv.Rows.Count)

        ' dgvのレコードを削除
        dgv.Rows.RemoveAt(nRowIndex)

        'Noを振りなおしボタンを表示しなおす
        InitIndexdgvReduction(dgv)
    End Sub

    ''' <summary>
    ''' dgvReductionのインデックスを振り、ボタンの表示設定をする
    ''' </summary>
    ''' <param name="dgv">dgv</param>
    ''' <remarks></remarks>
    Private Sub InitIndexdgvReduction(ByVal dgv As DataGridView)
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv(em_DataColumn.emTxt_Order, ii).Value = ii + 1   ' 列のインデックス番号

            ' 挿入ボタンの使用可能状態を設定    (ボタンを使用不可にはできないので、色を変更）
            If ii = dgv.RowCount - 1 Then
                '最終行は挿入ボタンが使用不可
                dgv(em_DataColumn.iInsButton, ii).Style.ForeColor = Color.Gray
            Else
                'それ以外は使用可
                dgv(em_DataColumn.iInsButton, ii).Style.ForeColor = Color.Black
            End If
        Next
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


    'セルが選択されたときの処理
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
    ''' 更新ボタンの有効/無効
    ''' </summary>
    ''' <param name="bPassResult">パスワード入力結果 (True:有効 False:無効)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' ファンクションキーの設定ボタン

    End Sub


    ''' <summary>
    ''' アップデート
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_DeleteUp() As Boolean
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB接続
        tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)

        Dim sqlReader As SqlClient.SqlDataReader = Nothing   'DataReaderオブジェクト
        Dim strSql As String    'SQL文

        Try
            'データベースオープン
            If Not tcls_DB.Open() Then Return False

            'トランザクション開始
            tcls_DB.TransactStart()

            strSql = "DELETE FROM " & DB_PARAM_REDUCTION
            'SQL文の発行
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' エラーならロールバックして終了
                tcls_DB.TransactRollback()
                Return False
            End If

            'データを登録 
            For ii As Integer = 0 To m_DsReduction.Tables(0).Rows.Count - 1
                With m_DsReduction.Tables(0).Rows(ii)

                    '登録SQL文作成
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_PARAM_REDUCTION & " VALUES ("

                    strSql &= .Item("id").ToString & ","                        '縮小率ID
                    strSql &= "'" & .Item("name").ToString & "',"               '名称
                    strSql &= .Item("MeanX").ToString & ","                     '平均サイズX
                    strSql &= .Item("MeanY").ToString & ","                     '平均サイズY
                    strSql &= .Item("RatioX").ToString & ","                    '縮小率X
                    strSql &= .Item("RatioY").ToString                          '縮小率Y
                    strSql &= " )"

                    'SQL実行
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        'エラーならロールバックして終了
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                End With
            Next ii

            'コミット
            tcls_DB.TransactCommint()

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " 登録エラー [" & ex.Message & "] " & Me.Text)
            'エラーならロールバックして終了
            tcls_DB.TransactRollback()
            Return False

        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()

        End Try
    End Function

    ''' <summary>
    ''' メンバ変数の初期設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Init()
        ' 削除対象のIDリストの初期化
        mlst_DelID.Clear()
    End Sub

End Class
