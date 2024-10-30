'*********************************************************************************
' 前工程マスター登録 疵種・グレード設定画面クラス
'	[Ver]
'		Ver.01    2012/06/08  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmMaeTypeGr
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' 前工程マスターデータグリッドビューの列
    Private Enum EM_DATACOLUMNMASTER
        emNo = 0                            ' テキストボックス No
        emTxt_MaeProcessCode                ' テキストボックス 前工程コード
        emChk_Check                         ' チェックボックス 有効無効
        emTxt_Name                          ' テキストボックス 名称
        emCmb_DispMode                      ' コンボボックス   前工程表示モード
        emCmb_ImageKeisiki                  ' コンボボックス   画像形式
        emTxt_KizuImageName1                ' テキストボックス 疵画像名称1
        emTxt_KizuImageName2                ' テキストボックス 疵画像名称2
        emTxt_KizuImageName3                ' テキストボックス 疵画像名称3
        emTxt_KizuImageName4                ' テキストボックス 疵画像名称4
        emCmb_TashozaiCheck                 ' コンボボックス   他所材判定
    End Enum

    ' 前工程疵種データグリッドビューの列
    Private Enum EM_DATACOLUMNTYPE
        emNo = 0                            ' テキストボックス No
        emTxt_FFName                        ' テキストボックス FFName
        emTxt_ID                            ' テキストボックス ID
        emTxt_Name                          ' テキストボックス 疵名称
        emTxt_Yusen                         ' テキストボックス 優先度
        emTxt_MapDispName                   ' テキストボックス マップ表示名称
        emCmb_HyoziUmu                      ' コンボボックス 　表示有無
        emInsButton                         ' 挿入ボタン
        emDelButton                         ' 削除ボタン
    End Enum

    ' 前工程グレードデータグリッドビューの列
    Private Enum EM_DATACOLUMNGRADE
        emNo = 0                            ' テキストボックス No
        emTxt_FFName                        ' テキストボックス FFName
        emTxt_GradeID                       ' テキストボックス グレードID
        emTxt_Name                          ' テキストボックス グレード名称
        emTxt_Yusen                         ' テキストボックス 優先度
        emMapButton                         ' マップ表示色
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterMaeMaster As SqlClient.SqlDataAdapter = Nothing    ' アダプター MaeMaster(前工程マスター)
    Private m_AdapterMaeType As SqlClient.SqlDataAdapter = Nothing      ' アダプター MaeType(前工程疵種)
    Private m_AdapterMaeGrade As SqlClient.SqlDataAdapter = Nothing     ' アダプター MaeGrade(前工程グレード)

    Private m_DsMaeMaster As New DataSet                                ' 非接続型テーブルMAE_MASTER
    Private m_DsMaeType As New DataSet                                  ' 非接続型テーブルMAE_TYPE_NAME
    Private m_DsMaeGrade As New DataSet                                 ' 非接続型テーブルMAE_GRADE_NAME

    Private m_KeyCode As String = ""                                    ' 主キー

    Private m_sMaeGrade() As String                                     '選択前工程コードの前工程グレード名称
    Private m_nMaeGradeID() As Integer                                  '選択前工程コードの前工程グレードID


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
            Case 0
                btnF01.Select()                 ' F01コンボボックス変更中にファンクションが押された時の対策
                ChangeCode(False)               ' 前項
            Case 1
                btnF02.Select()                 ' F02コンボボックス変更中にファンクションが押された時の対策
                ChangeCode(True)                ' 次項
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "前工程設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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

        m_DsMaeMaster.Dispose()
        If Not m_DsMaeMaster Is Nothing Then
            m_DsMaeMaster.Dispose()
            m_DsMaeMaster = Nothing
        End If

        m_DsMaeType.Dispose()
        If Not m_AdapterMaeType Is Nothing Then
            m_AdapterMaeType.Dispose()
            m_AdapterMaeType = Nothing
        End If

        m_DsMaeGrade.Dispose()
        If Not m_AdapterMaeGrade Is Nothing Then
            m_AdapterMaeGrade.Dispose()
            m_AdapterMaeGrade = Nothing
        End If

        ' 画面切替
        ShowForm(frmID)
    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                     ' メニューリスト生成
        modMain.SetLabelSysName(Me)                                 ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmMaeTypeGr             ' 前工程マスター登録 疵種・グレード設定画面呼出

        'タイトル設定
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' このフォームで必要なデータを取得する コンボボックスの値もとってくるので、dgv設定より先
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()   '終了
        End If

        ' コンボボックスの表示データを初期化
        Data_Init()

        '前工程マスターdgv初期化
        dgvInitializeDefMaster(dgvMaeMaster)

        '前工程疵種dgv初期化
        dgvInitializeDefType(dgvMaeType)

        '前工程グレードdgv初期化
        dgvInitializeDefGrade(dgvMaeGrade)

        ' 画面に値を表示
        Data_Disp(m_KeyCode)

        ' 選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
        dgvMaeType.DataGridViewSelectionRowColorChange(g_ColorSelection)        ' 前工程疵種dgv
        dgvMaeGrade.DataGridViewSelectionRowColorChange(g_ColorSelection)       ' 前工程グレードdgv

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
        strSQL = "SELECT 前工程コード"
        strSQL &= ", 有効判定"
        strSQL &= ", 名称"
        strSQL &= ", 前工程表示モード"
        strSQL &= ", 画像形式"
        strSQL &= ", 疵画像名称1"
        strSQL &= ", 疵画像名称2"
        strSQL &= ", 疵画像名称3"
        strSQL &= ", 疵画像名称4"
        strSQL &= ", 他所材判定"
        strSQL &= " FROM " & DB_MAE_MASTER

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsMaeMaster, m_AdapterMaeMaster, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_MAE_MASTER & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL文生成
        strSQL = "SELECT 前工程コード"
        strSQL &= ", FFName"
        strSQL &= ", ID"
        strSQL &= ", Name"
        strSQL &= ", 優先度"
        strSQL &= ", マップ表示名称"
        strSQL &= ", 表示有無"
        strSQL &= " FROM " & DB_MAE_TYPE_NAME
        strSQL &= " ORDER BY 前工程コード ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsMaeType, m_AdapterMaeType, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_MAE_TYPE_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL文生成
        strSQL = "SELECT 前工程コード"
        strSQL &= ", FFName"
        strSQL &= ", グレードID"
        strSQL &= ", Name"
        strSQL &= ", 優先度"
        strSQL &= ", マップ表示色"
        strSQL &= " FROM " & DB_MAE_GRADE_NAME
        strSQL &= " ORDER BY 前工程コード ASC"

        'SQL文実行m_DsMaeGrade
        If 0 > MyBase.Get_SelectData(strSQL, m_DsMaeGrade, m_AdapterMaeGrade, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_MAE_GRADE_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True
    End Function

    ''' <summary>
    ''' DBからとってきたコンボボックス用の値を表示用に加工
    ''' </summary>
    ''' <remarks>グレード名称に以上をつける、無しの行を追加、</remarks>
    Private Sub Data_Init()

        Try
            Dim drowMaster() As DataRow = m_DsMaeMaster.Tables(0).Select()           ' 全レコードを表示するためWHERE句なし
            ' コンボボックスのデータを初期化
            cmbMaeProcessCode.Items.Clear()

            If drowMaster.Length = 0 Then Exit Sub

            For ii As Integer = 0 To drowMaster.Length - 1
                cmbMaeProcessCode.Items.Add(drowMaster(ii).Item("前工程コード").ToString)
            Next ii
            cmbMaeProcessCode.MaxDropDownItems = drowMaster.Length
            If 0 < cmbMaeProcessCode.Items.Count Then
                ' 前の画面で選択した状態をセット
				If cmbMaeProcessCode.Items.Count - 1 < g_nMaeIndex Then
					' 該当項目がない場合は、先頭を表示
					cmbMaeProcessCode.SelectedIndex = 0
				Else
					cmbMaeProcessCode.SelectedIndex = g_nMaeIndex
				End If
            End If
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "コンボボックス用データ 設定エラー [" & ex.Message & "] " & Me.Text)
        End Try

    End Sub

    ''' <summary>
    ''' dgTypeにデータ行を挿入
    ''' </summary>
    ''' <param name="nRowIndex">行の挿入位置</param>
    ''' <remarks></remarks>
    Private Sub InsertAt_TypeData(ByVal nRowIndex As Integer)
        ' dgvMaeTypeにレコードを挿入
        dgvMaeType.Rows.Insert(nRowIndex)

        ' データ初期化
        dgvMaeType(EM_DATACOLUMNTYPE.emTxt_FFName, nRowIndex).Value = ""            ' FFName
        dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, nRowIndex).Value = ""                ' ID
        dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Name, nRowIndex).Value = ""              ' 疵名称
        dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Yusen, nRowIndex).Value = ""             ' 優先度
        dgvMaeType(EM_DATACOLUMNTYPE.emTxt_MapDispName, nRowIndex).Value = ""       ' マップ表示名称
        ' 表示有無
        Using clm As New DataGridViewComboBoxCell
            Dim listItem As DataGridViewComboBoxCell = CType(clm, DataGridViewComboBoxCell)
            listItem.Items.AddRange(m_sMaeGrade)
            listItem.MaxDropDownItems = listItem.Items.Count
            dgvMaeType(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, nRowIndex) = clm
            dgvMaeType(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, nRowIndex).Value = ""
        End Using
    End Sub


    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp(ByVal key As String)

        '前工程マスターのデータを表示
        Data_DispDgvMaeMaster(dgvMaeMaster)
        '前工程マスターのインデックスとボタンの表示設定
        InitIndexDgvMaeMaster(dgvMaeMaster)

        '前工程疵種のデータを表示
        Data_DispDgvMaeType(dgvMaeType, key)
        '前工程疵種のインデックスとボタンの表示設定
        InitIndexDgvMaeType(dgvMaeType)

        '前工程グレードのデータを表示
        Data_DispDgvMaeGrade(dgvMaeGrade, key)
        '前工程グレードのインデックスとボタンの表示設定
        InitIndexDgvMaeGrade(dgvMaeGrade)

        ' 行の色を変える （初期表示で色が変わる対策）
        dgvMaeType.CurrentRow.DefaultCellStyle.BackColor = Color.Empty
        dgvMaeGrade.CurrentRow.DefaultCellStyle.BackColor = Color.Empty

    End Sub

    ''' <summary>
    ''' 前工程マスターdgvにデータを表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Data_DispDgvMaeMaster(ByVal dgv As DataGridView) As Boolean
        Dim drowMaster() As DataRow = m_DsMaeMaster.Tables(0).Select()           ' 全レコードを表示するためWHERE句なし

        '行を初期化
        dgv.Rows.Clear()

        ' 画面に表示
        dgv.RowCount = MAX_MAE_LINE                   ' 前工程マスターdgvのレコード数は固定
        For ii As Integer = 0 To dgv.RowCount - 1
            'グレード非接続テーブルの値がある間はそれを入れる。無ければ初期値
            If ii < drowMaster.Length Then

                ' 前工程コードがNULLの時は空白
                If drowMaster(ii).Item("前工程コード") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, ii).Value = ""
                Else
                    'そのまま表示
                    dgv(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, ii).Value = drowMaster(ii).Item("前工程コード")
                End If

                ' 有効無効チェックボックス
                If CInt(drowMaster(ii).Item("有効判定")) = 0 Then
                    dgv(EM_DATACOLUMNMASTER.emChk_Check, ii).Value = False                                        ' 0ならチェックOFF
                Else
                    dgv(EM_DATACOLUMNMASTER.emChk_Check, ii).Value = True                                         ' 0以外はチェックON
                End If

                ' 前工程名称がNULLの時は空白
                If drowMaster(ii).Item("名称") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNMASTER.emTxt_Name, ii).Value = ""
                Else
                    'そのまま表示
                    dgv(EM_DATACOLUMNMASTER.emTxt_Name, ii).Value = drowMaster(ii).Item("名称")
                End If

                ' 前工程表示モード
                If drowMaster(ii).Item("前工程表示モード").ToString.Trim = "0" Then
                    dgv(EM_DATACOLUMNMASTER.emCmb_DispMode, ii).Value = "対象外"
                ElseIf drowMaster(ii).Item("前工程表示モード").ToString = "1" Then
                    dgv(EM_DATACOLUMNMASTER.emCmb_DispMode, ii).Value = "φω情報"
                ElseIf drowMaster(ii).Item("前工程表示モード").ToString = "2" Then
                    dgv(EM_DATACOLUMNMASTER.emCmb_DispMode, ii).Value = "FF情報"
                End If

                ' 画像形式
                If drowMaster(ii).Item("画像形式").ToString.Trim = "0" Then
                    dgv(EM_DATACOLUMNMASTER.emCmb_ImageKeisiki, ii).Value = "JPEG"
                ElseIf drowMaster(ii).Item("画像形式").ToString = "1" Then
                    dgv(EM_DATACOLUMNMASTER.emCmb_ImageKeisiki, ii).Value = "BMP"
                End If

                ' 疵画像名称1がNULLの時は空白
                If drowMaster(ii).Item("疵画像名称1") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName1, ii).Value = ""
                Else
                    'そのまま表示
                    dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName1, ii).Value = drowMaster(ii).Item("疵画像名称1")
                End If

                ' 疵画像名称2がNULLの時は空白
                If drowMaster(ii).Item("疵画像名称2") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName2, ii).Value = ""
                Else
                    'そのまま表示
                    dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName2, ii).Value = drowMaster(ii).Item("疵画像名称2")
                End If

                ' 疵画像名称3がNULLの時は空白
                If drowMaster(ii).Item("疵画像名称3") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName3, ii).Value = ""
                Else
                    'そのまま表示
                    dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName3, ii).Value = drowMaster(ii).Item("疵画像名称3")
                End If

                ' 疵画像名称4がNULLの時は空白
                If drowMaster(ii).Item("疵画像名称4") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName4, ii).Value = ""
                Else
                    'そのまま表示
                    dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName4, ii).Value = drowMaster(ii).Item("疵画像名称4")
                End If

                ' 他所材判定
                If drowMaster(ii).Item("他所材判定").ToString.Trim = "0" Then
                    dgv(EM_DATACOLUMNMASTER.emCmb_TashozaiCheck, ii).Value = "自所材"
                ElseIf drowMaster(ii).Item("他所材判定").ToString = "1" Then
                    dgv(EM_DATACOLUMNMASTER.emCmb_TashozaiCheck, ii).Value = "他所材"
                End If

            Else
                dgv(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, ii).Value = ""                    ' 前工程コード
                dgv(EM_DATACOLUMNMASTER.emChk_Check, ii).Value = False                          ' 有効無効
                dgv(EM_DATACOLUMNMASTER.emTxt_Name, ii).Value = ""                              ' 前工程名称
                dgv(EM_DATACOLUMNMASTER.emCmb_DispMode, ii).Value = ""                          ' 前工程モード
                dgv(EM_DATACOLUMNMASTER.emCmb_ImageKeisiki, ii).Value = ""                      ' 画像形式
                dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName1, ii).Value = ""                    ' 疵画像名称1
                dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName2, ii).Value = ""                    ' 疵画像名称2
                dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName3, ii).Value = ""                    ' 疵画像名称3
                dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName4, ii).Value = ""                    ' 疵画像名称4
                dgv(EM_DATACOLUMNMASTER.emCmb_TashozaiCheck, ii).Value = ""                     ' 他所材判定
            End If
        Next ii
    End Function

    ''' <summary>
    ''' 前工程疵種dgvにデータを表示
    ''' </summary>
    ''' <param name="key">主キーの値(前工程コード)</param>
    ''' <remarks></remarks>
    Private Sub Data_DispDgvMaeType(ByVal dgv As DataGridView, ByVal key As String)
        ' 一括のテーブルなので、前工程毎の値のみ取り出し
        Dim strWhere As String = String.Format("前工程コード='{0}'", key)          ' WHERE句に相当の条件生成
        Dim drowType() As DataRow = m_DsMaeType.Tables(0).Select(strWhere)
        Dim strWhereGrade As String
        Dim drowGrade() As DataRow = m_DsMaeGrade.Tables(0).Select(strWhere)

        '行を初期化
        dgv.Rows.Clear()

        'グレードテーブルの設定
        ReDim m_sMaeGrade(drowGrade.Length)
        ReDim m_nMaeGradeID(drowGrade.Length)
        m_sMaeGrade(0) = "-"
        m_nMaeGradeID(0) = 0
        ' 全行ループ グレードの名称に以上をつける
        For ii As Integer = 0 To drowGrade.Length - 1
            m_sMaeGrade(ii + 1) = drowGrade(ii).Item("Name").ToString & "以上"
            m_nMaeGradeID(ii + 1) = CInt(drowGrade(ii).Item("グレードID"))
        Next ii

		' 画面に表示		' 2015/02/13 前工程疵種が１つしか表示されない問題の対応 なぜ MAX_MAE_LINE で割っているのかは不明 野村
		'dgv.RowCount = CInt(MAX_MAE_TYPE / MAX_MAE_LINE)	   ' 前工程疵種dgvのレコード数は固定
		dgv.RowCount = MAX_MAE_TYPE	   ' 前工程疵種dgvのレコード数は固定

        For ii As Integer = 0 To dgv.RowCount - 1

            '疵種非接続テーブルの値がある間はそれを入れる。無ければ初期値
            If ii < drowType.Length Then

                ' FFNameがNULLの時は空白
                If drowType(ii).Item("FFName") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_FFName, ii).Value = ""
                Else
                    'そのまま表示
                    dgv(EM_DATACOLUMNTYPE.emTxt_FFName, ii).Value = drowType(ii).Item("FFName")
                End If

                '    IDがNULLの時は空白()
                If drowType(ii).Item("ID") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_ID, ii).Value = ""
                Else
                    'そのまま表示
                    dgv(EM_DATACOLUMNTYPE.emTxt_ID, ii).Value = drowType(ii).Item("ID")
                End If

                ' NameがNULLの時は空白
                If drowType(ii).Item("Name") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value = ""
                Else
                    ' そのまま表示
                    dgv(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value = drowType(ii).Item("Name")
                End If

                ' 優先度がNULLのときは空白
                If drowType(ii).Item("優先度") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_Yusen, ii).Value = ""
                Else
                    ' そのまま表示
                    dgv(EM_DATACOLUMNTYPE.emTxt_Yusen, ii).Value = drowType(ii).Item("優先度")
                End If

                ' マップ表示名称がNULLのときは空白
                If drowType(ii).Item("マップ表示名称") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_MapDispName, ii).Value = ""
                Else
                    'そのまま表示
                    dgv(EM_DATACOLUMNTYPE.emTxt_MapDispName, ii).Value = drowType(ii).Item("マップ表示名称")
                End If

                ' 表示有無
                Using clm As New DataGridViewComboBoxCell
                    Dim listItem As DataGridViewComboBoxCell = CType(clm, DataGridViewComboBoxCell)
                    listItem.Items.AddRange(m_sMaeGrade)
                    listItem.MaxDropDownItems = listItem.Items.Count

                    dgv(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, ii) = clm

                    strWhereGrade = String.Format("前工程コード='{0}' and グレードID={1}", key, drowType(ii).Item("表示有無"))
                    drowGrade = m_DsMaeGrade.Tables(0).Select(strWhereGrade)

                    If drowGrade.Length = 1 Then
                        dgv(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, ii).Value = drowGrade(0).Item("Name").ToString & "以上"
                    Else
                        dgv(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, ii).Value = "-"
                    End If

                End Using

            Else
                ' データが無いので初期化
                dgv(EM_DATACOLUMNTYPE.emTxt_ID, ii).Value = ""                  ' ID
                dgv(EM_DATACOLUMNTYPE.emTxt_FFName, ii).Value = ""              ' FFName
                dgv(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value = ""                ' 疵名称
                dgv(EM_DATACOLUMNTYPE.emTxt_Yusen, ii).Value = ""               ' 優先度
                dgv(EM_DATACOLUMNTYPE.emTxt_MapDispName, ii).Value = ""         ' マップ表示名称
                dgv(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, ii).Value = ""            ' 表示有無
                ' 表示有無
                Using clm As New DataGridViewComboBoxCell
                    Dim listItem As DataGridViewComboBoxCell = CType(clm, DataGridViewComboBoxCell)
                    listItem.Items.AddRange(m_sMaeGrade)
                    listItem.MaxDropDownItems = listItem.Items.Count
                    dgv(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, ii) = clm
                    dgv(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, ii).Value = ""
                End Using

            End If

        Next ii

        ' カーソルスタート位置は入力エリアの先頭
        dgv(1, 0).Selected = True
    End Sub

    ''' <summary>
    ''' 前工程グレードdgvにデータを表示
    ''' </summary>
    ''' <param name="key">主キーの値(前工程コード)</param>
    ''' <remarks></remarks>
    Private Sub Data_DispDgvMaeGrade(ByVal dgv As DataGridView, ByVal key As String)
        ' 一括のテーブルなので、前工程毎の値のみ取り出し
        Dim strWhere As String = String.Format("前工程コード='{0}'", key)          ' WHERE句に相当の条件生成
        Dim drowGrade() As DataRow = m_DsMaeGrade.Tables(0).Select(strWhere)

        '行を初期化
        dgv.Rows.Clear()

		' 画面に表示
		' 2015/02/13 前工程グレードが１つしか表示されない問題の対応 なぜ MAX_MAE_LINE で割っているのかは不明 野村
		'dgv.RowCount = CInt(MAX_MAE_GRADE / MAX_MAE_LINE)				 ' 前工程グレードdgvのレコード数は固定
		dgv.RowCount = MAX_MAE_GRADE										' 前工程グレードdgvのレコード数は固定
		For ii As Integer = 0 To dgv.RowCount - 1
			'グレード非接続テーブルの値がある間はそれを入れる。無ければ初期値
			If ii < drowGrade.Length Then

				' FFNameがNULLの時は空白
				If drowGrade(ii).Item("FFName") Is System.DBNull.Value Then
					dgv(EM_DATACOLUMNGRADE.emTxt_FFName, ii).Value = ""
				Else
					'そのまま表示
					dgv(EM_DATACOLUMNGRADE.emTxt_FFName, ii).Value = drowGrade(ii).Item("FFName")
				End If

				' グレードIDがNULLの時は空白()
				If drowGrade(ii).Item("グレードID") Is System.DBNull.Value Then
					dgv(EM_DATACOLUMNGRADE.emTxt_GradeID, ii).Value = ""
				Else
					'そのまま表示
					dgv(EM_DATACOLUMNGRADE.emTxt_GradeID, ii).Value = drowGrade(ii).Item("グレードID")
				End If

				' NameがNULLの時は空白
				If drowGrade(ii).Item("Name") Is System.DBNull.Value Then
					dgv(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value = ""
				Else
					'そのまま表示
					dgv(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value = drowGrade(ii).Item("Name")
				End If

				' 優先度がNULLのときは空白
				If drowGrade(ii).Item("優先度") Is System.DBNull.Value Then
					dgv(EM_DATACOLUMNGRADE.emTxt_Yusen, ii).Value = ""
				Else
					'そのまま表示
					dgv(EM_DATACOLUMNGRADE.emTxt_Yusen, ii).Value = drowGrade(ii).Item("優先度")
				End If

				' マップ表示色の値がNULLの時は白色
				If drowGrade(ii).Item("マップ表示色") Is System.DBNull.Value Then
					dgv(EM_DATACOLUMNGRADE.emMapButton, ii).Style.BackColor = Color.White		' 白色
				Else
					'設定されている色を表示
					dgv(EM_DATACOLUMNGRADE.emMapButton, ii).Style.BackColor = _
					  Color.FromArgb(CInt(drowGrade(ii).Item("マップ表示色")))			   ' マップ表示色
				End If
			Else
				dgv(EM_DATACOLUMNGRADE.emTxt_GradeID, ii).Value = ""							' グレードID"
				dgv(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value = ""								' 前工程コード
				dgv(EM_DATACOLUMNGRADE.emTxt_FFName, ii).Value = ""								' FFName
				dgv(EM_DATACOLUMNGRADE.emTxt_Yusen, ii).Value = ""								' 優先度
				dgv(EM_DATACOLUMNGRADE.emMapButton, ii).Style.BackColor = Color.White			' マップ表示色に白色
			End If
		Next ii
    End Sub

    ''' <summary>
    ''' 前工程コード変更時
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbMaeProcessCode_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbMaeProcessCode.SelectedIndexChanged
        ' 今の選択と主キーが同じ場合は無視 (選択して入力異常だったので、 元に戻した場合の対策)
        If m_KeyCode = cmbMaeProcessCode.Text Then Exit Sub

        '' 画面のデータを非接続型テーブルにセット
        ' 初期画面表示時には、データ無しの為、画面のデータを取り込まない
        If m_KeyCode <> "" Then
            ' 入力値チェック
            If Not Data_Check(m_KeyCode) Then
                ' 入力異常時は、選択位置も戻す
                cmbMaeProcessCode.Text = m_KeyCode
                Exit Sub
            End If

            ' 変更前のキーに対し 画面データ取り込み
            Data_MaeTypeGrGet(m_KeyCode)
        End If

        ' 選択変更に伴い ラベル値変更
        Dim drowMaster() As DataRow = m_DsMaeMaster.Tables(0).Select()           ' 全レコードを表示するためWHERE句なし

        Dim nIdx As Integer = cmbMaeProcessCode.SelectedIndex

        ' 有効ならばデフォルト色、無効ならば無効色をセットする。
        If CInt(drowMaster(nIdx).Item("有効判定").ToString) = 1 Then
            lblMaeProcessCode.Text = drowMaster(nIdx).Item("名称").ToString
            lblMaeProcessCode.BackColor = g_ColorSetting
        Else
            lblMaeProcessCode.Text = drowMaster(nIdx).Item("名称").ToString
            lblMaeProcessCode.BackColor = Color.LightGray
        End If

        ' 画面に前工程コード別の値を表示
        If m_KeyCode <> "" Then
            m_KeyCode = cmbMaeProcessCode.Text
            Data_Disp(m_KeyCode)
        Else
            m_KeyCode = cmbMaeProcessCode.Text
        End If

        ' 現在の状態を保存
		g_nMaeIndex = cmbMaeProcessCode.SelectedIndex

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
        ' 今の画面表示中の値と取り込み()

        Try

            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            ' 画面のデータを取得
            Data_MaeMasterGet()
            Data_MaeTypeGrGet(m_KeyCode)

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

        ' 設定の初期化
        Data_Init()

        ' 画面に値を表示
        Data_Disp(m_KeyCode)
    End Sub

    ''' <summary>
    ''' 入力値のチェック
    ''' </summary>
    ''' <param name="key">主キーの値(前工程コード) 今の所未使用</param>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_Check(ByVal key As String) As Boolean
        Dim strWk As String     ' dgvのデータ格納用
        Dim strMsg As String    ' 重複用のエラーメッセージ

        ' 前工程マスターdgv、件数文ループ
        For ii As Integer = 0 To dgvMaeMaster.Rows.Count - 1

            ' 有効判定取得
            Dim bCheck As Boolean = CBool(dgvMaeMaster(EM_DATACOLUMNMASTER.emChk_Check, ii).Value)

            ' 前工程コード
            strWk = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 2, "前工程マスター設定 No=" & ii + 1 & " 前工程コード") <> 0 Then Return False
            dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, ii).Value = strWk

            ' 前工程コードが空白の行は無視
            If Trim(strWk) = "" Then Continue For

            '前工程コードの重複チェック
            For jj As Integer = ii + 1 To dgvMaeMaster.Rows.Count - 1
                ' 比較先の前工程コードが入っていれば比較
                If CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, jj).Value) <> "" Then
                    ' 前工程コードが同じならメッセージを出して終了
                    If CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, ii).Value).ToUpper = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, jj).Value).ToUpper Then
                        strMsg = "【 前工程マスター 】" & vbNewLine
                        strMsg &= "同じ前工程コードは登録できません。" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' 前工程名称
            strWk = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_Name, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "前工程マスター設定 No=" & ii + 1 & "前工程名称") <> 0 Then Return False
            dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_Name, ii).Value = strWk
            ' 前工程名称が空白の行は無視
            If Trim(strWk) = "" Then Continue For

            ' 前工程名称の重複チェック
            If bCheck = True Then
                For jj As Integer = ii + 1 To dgvMaeMaster.Rows.Count - 1
                    If Not CBool(dgvMaeMaster(EM_DATACOLUMNMASTER.emChk_Check, jj).Value) Then
                        Continue For                                                ' 無効なら次へ
                    End If
                    ' 比較先の前工程コードが入っていれば比較
                    If CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, jj).Value) <> "" Then
                        ' 前工程名称が同じならメッセージを出して終了
                        If CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_Name, ii).Value) = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_Name, jj).Value) Then
                            strMsg = "【 前工程マスター 】" & vbNewLine
                            strMsg &= "同じ前工程名称は登録できません。" & vbNewLine
                            strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                            MsgBox(strMsg)
                            Return False
                        End If
                    End If
                Next jj
            End If

            ' 疵画像名称1
            strWk = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName1, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "前工程マスター設定 No=" & ii + 1 & "疵画像名称1") <> 0 Then Return False
            dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName1, ii).Value = strWk

            ' 疵画像名称2
            strWk = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName2, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "前工程マスター設定 No=" & ii + 1 & "疵画像名称2") <> 0 Then Return False
            dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName2, ii).Value = strWk

            ' 疵画像名称3
            strWk = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName3, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "前工程マスター設定 No=" & ii + 1 & "疵画像名称3") <> 0 Then Return False
            dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName3, ii).Value = strWk

            ' 疵画像名称4
            strWk = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName4, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "前工程マスター設定 No=" & ii + 1 & "疵画像名称4") <> 0 Then Return False
            dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName4, ii).Value = strWk

        Next ii

        ' 前工程疵種dgv、64行ループ
        For ii As Integer = 0 To dgvMaeType.Rows.Count - 1

            ' FFName
            strWk = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_FFName, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 2, "疵種設定 No=" & ii + 1 & "FFName") <> 0 Then Return False
            dgvMaeType(EM_DATACOLUMNTYPE.emTxt_FFName, ii).Value = strWk

            ' FFNameの重複チェック
            For jj As Integer = ii + 1 To dgvMaeType.Rows.Count - 1
                ' 比較先のIDが数値なら比較
                If IsNumeric(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, jj).Value) Then
                    ' FFNameが同じならメッセージを出して終了
                    If CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_FFName, ii).Value).ToUpper = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_FFName, jj).Value).ToUpper Then
                        strMsg = "【 疵種設定 】" & vbNewLine
                        strMsg &= "同じFFNameは登録できません。" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' ID
            strWk = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 9999, True, "疵種設定 No=" & ii + 1 & " ID") <> 0 Then Return False
            dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, ii).Value = strWk

            ' IDが空白の行は無視
            If Trim(strWk) = "" Then Continue For

            ' IDの重複チェック
            For jj As Integer = ii + 1 To dgvMaeType.Rows.Count - 1
                ' 比較先のIDが数値なら比較
                If IsNumeric(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, jj).Value) Then
                    ' IDが同じならメッセージを出して終了
                    If CInt(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, ii).Value) = CInt(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, jj).Value) Then
                        strMsg = "【 疵種設定 】" & vbNewLine
                        strMsg &= "同じIDは登録できません。" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' マップ表示名称
            strWk = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_MapDispName, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 4, "疵種設定 No=" & ii + 1 & "マップ表示名称") <> 0 Then Return False
            dgvMaeType(EM_DATACOLUMNTYPE.emTxt_MapDispName, ii).Value = strWk

            ' 疵名称
            strWk = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 32, "疵種設定 No=" & ii + 1 & "疵名称") <> 0 Then Return False
            dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value = strWk

            ' 疵名称の重複チェック
            For jj As Integer = ii + 1 To dgvMaeType.Rows.Count - 1
                ' 比較先のIDが数値なら比較
                If IsNumeric(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, jj).Value) Then
                    ' 疵名称が同じならメッセージを出して終了
                    If CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value) = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Name, jj).Value) Then
                        strMsg = "【 疵種設定 】" & vbNewLine
                        strMsg &= "同じ疵名称は登録できません。" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' 優先度
            strWk = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Yusen, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 9999, False, "疵種設定 No=" & ii + 1 & " 優先度") <> 0 Then Return False
            dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Yusen, ii).Value = strWk

            ' 優先度の重複チェック
            For jj As Integer = ii + 1 To dgvMaeType.Rows.Count - 1
                ' 比較先のIDが数値で、優先度が数値なら比較
                If IsNumeric(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, jj).Value) And _
                    Trim(CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, jj).Value)) <> "" And _
                    IsNumeric(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Yusen, jj).Value) Then
                    ' 優先度が同じならメッセージを出して終了
                    If CInt(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Yusen, ii).Value) = CInt(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Yusen, jj).Value) Then
                        strMsg = "【 疵種設定 】" & vbNewLine
                        strMsg &= "同じ優先度は登録できません。" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

        Next ii

        ' 前工程グレードdgv、10行ループ
        For ii As Integer = 0 To dgvMaeGrade.Rows.Count - 1

            ' FFName
            strWk = CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_FFName, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 2, "グレード設定 No=" & ii + 1 & "FFName") <> 0 Then Return False
            dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_FFName, ii).Value = strWk

            ' FFNameの重複チェック
            For jj As Integer = ii + 1 To dgvMaeGrade.Rows.Count - 1
                ' 比較先のグレードIDが数値なら比較
                If IsNumeric(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, jj).Value) Then
                    ' FFNameが同じならメッセージを出して終了
                    If CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_FFName, ii).Value).ToUpper = CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_FFName, jj).Value).ToUpper Then
                        strMsg = "【 グレード設定 】" & vbNewLine
                        strMsg &= "同じFFNameは登録できません。" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' グレードID
            strWk = CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 9999, True, "グレード設定 No=" & ii + 1 & " グレードID") <> 0 Then Return False
            dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, ii).Value = strWk

            ' グレードIDが空白の行は無視
            If Trim(strWk) = "" Then Continue For

            ' グレードIDの重複チェック
            For jj As Integer = ii + 1 To dgvMaeGrade.Rows.Count - 1
                ' 比較先のグレードIDが数値なら比較
                If IsNumeric(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, jj).Value) Then
                    ' グレードIDが同じならメッセージを出して終了
                    If CInt(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, ii).Value) = CInt(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, jj).Value) Then
                        strMsg = "【 グレード設定 】" & vbNewLine
                        strMsg &= "同じグレードIDは登録できません。" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' グレード名称
            strWk = CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 32, "グレード設定 No=" & ii + 1 & "グレード名称") <> 0 Then Return False
            dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value = strWk

            ' グレード名称の重複チェック
            For jj As Integer = ii + 1 To dgvMaeGrade.Rows.Count - 1
                ' 比較先のグレードIDが数値ならグレード名称を比較
                If IsNumeric(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, jj).Value) Then
                    ' グレード名称が同じならメッセージを出して終了
                    If CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value) = CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Name, jj).Value) Then
                        strMsg = "【 グレード設定 】" & vbNewLine
                        strMsg &= "同じグレード名称は登録できません。" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' 優先度
            strWk = CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Yusen, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 9999, False, "グレード設定 No=" & ii + 1 & " 優先度") <> 0 Then Return False
            dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Yusen, ii).Value = strWk

            ' 優先度の重複チェック
            For jj As Integer = ii + 1 To dgvMaeGrade.Rows.Count - 1
                ' 比較先のグレードIDが数値で、優先度が数値なら比較
                If IsNumeric(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, jj).Value) And _
                    Trim(CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, jj).Value)) <> "" And _
                    IsNumeric(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Yusen, jj).Value) Then
                    ' 優先度が同じならメッセージを出して終了
                    If CInt(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Yusen, ii).Value) = CInt(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Yusen, jj).Value) Then
                        strMsg = "【 グレード設定 】" & vbNewLine
                        strMsg &= "同じ優先度は登録できません。" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

        Next ii

        Return True
    End Function

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_MaeMasterGet()
        Dim nCountNo As Integer = 0  'Noカウント用
        Dim nCheck As Integer
        ' デリートインサートする為、今もっている行をクリアする
        m_DsMaeMaster.Tables(0).Rows.Clear()

        ' 前工程マスターdgvの値を前工程マスター非接続テーブルへ格納
        For ii As Integer = 0 To dgvMaeMaster.RowCount - 1

            If CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, ii).Value) = "" Then Continue For

            Dim drowMaster As DataRow = m_DsMaeMaster.Tables(0).NewRow()                                            ' 新規行

            drowMaster.Item("前工程コード") = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, ii).Value)  ' 前工程コード

            ' 有効無効チェックボックスのチェック状態により、有効判定をセット
            If CBool(dgvMaeMaster(EM_DATACOLUMNMASTER.emChk_Check, ii).Value) Then
                nCheck = 1                                              ' 有効
            Else
                nCheck = 0                                              ' 無効
            End If

            drowMaster.Item("有効判定") = nCheck                        ' 有効判定

            If dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_Name, ii).Value Is Nothing = True Then            ' 前工程名称
                drowMaster.Item("名称") = DBNull.Value
            Else
                drowMaster.Item("名称") = dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_Name, ii).Value
            End If

            ' 前工程表示モードは、コンボボックスなので、選択した項目により格納値分岐
            If dgvMaeMaster(EM_DATACOLUMNMASTER.emCmb_DispMode, ii).Value.ToString.Trim = "対象外" Then
                drowMaster.Item("前工程表示モード") = 0
            ElseIf dgvMaeMaster(EM_DATACOLUMNMASTER.emCmb_DispMode, ii).Value.ToString.Trim = "φω情報" Then
                drowMaster.Item("前工程表示モード") = 1
            ElseIf dgvMaeMaster(EM_DATACOLUMNMASTER.emCmb_DispMode, ii).Value.ToString.Trim = "FF情報" Then
                drowMaster.Item("前工程表示モード") = 2
            Else
                drowMaster.Item("前工程表示モード") = 0
            End If

            ' 画像形式は、コンボボックスなので、選択した項目により格納値分岐
            If dgvMaeMaster(EM_DATACOLUMNMASTER.emCmb_ImageKeisiki, ii).Value.ToString.Trim = "JPEG" Then
                drowMaster.Item("画像形式") = 0
            ElseIf dgvMaeMaster(EM_DATACOLUMNMASTER.emCmb_ImageKeisiki, ii).Value.ToString.Trim = "BMP" Then
                drowMaster.Item("画像形式") = 1
            Else
                drowMaster.Item("画像形式") = 0
            End If

            If dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName1, ii).Value Is Nothing = True Then          ' 疵画像名称1
                drowMaster.Item("疵画像名称1") = DBNull.Value
            Else
                drowMaster.Item("疵画像名称1") = dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName1, ii).Value
            End If

            If dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName2, ii).Value Is Nothing = True Then          ' 疵画像名称2
                drowMaster.Item("疵画像名称2") = DBNull.Value
            Else
                drowMaster.Item("疵画像名称2") = dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName2, ii).Value
            End If

            If dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName3, ii).Value Is Nothing = True Then          ' 疵画像名称3
                drowMaster.Item("疵画像名称3") = DBNull.Value
            Else
                drowMaster.Item("疵画像名称3") = dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName3, ii).Value
            End If

            If dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName4, ii).Value Is Nothing = True Then          ' 疵画像名称4
                drowMaster.Item("疵画像名称4") = DBNull.Value
            Else
                drowMaster.Item("疵画像名称4") = dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName4, ii).Value
            End If

            ' 他所材判定は、コンボボックスなので、選択した項目により格納値分岐
            If dgvMaeMaster(EM_DATACOLUMNMASTER.emCmb_TashozaiCheck, ii).Value.ToString.Trim = "自所材" Then
                drowMaster.Item("他所材判定") = 0
            ElseIf dgvMaeMaster(EM_DATACOLUMNMASTER.emCmb_TashozaiCheck, ii).Value.ToString.Trim = "他所材" Then
                drowMaster.Item("他所材判定") = 1
            Else
                drowMaster.Item("他所材判定") = 0
            End If

            m_DsMaeMaster.Tables(0).Rows.Add(drowMaster)        '行を追加
            nCountNo += 1                                       'No = No + 1

        Next ii
    End Sub

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <param name="key">主キーの値(前工程コード)</param>
    ''' <remarks></remarks>
    Private Sub Data_MaeTypeGrGet(ByVal key As String)
        ' 前工程グレードのデータをセット
        Data_MaeGradeGet(key)
        ' 前工程疵種のデータをセット
        Data_MaeTypeGet(key)
    End Sub

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <param name="key">主キーの値(前工程コード)</param>
    ''' <remarks></remarks>
    Private Sub Data_MaeTypeGet(ByVal key As String)
        Dim strWhere As String = String.Format("前工程コード='{0}'", key)           ' WHERE句に相当の条件生成
        Dim drow() As DataRow = m_DsMaeType.Tables(0).Select(strWhere)              ' 前工程コードが主キーの値の行を取り出す
        Dim drowGrade() As DataRow = m_DsMaeGrade.Tables(0).Select(strWhere)        ' 前工程コードが主キーの値の行を取り出す
        Dim nRowCount As Integer                                                    ' 削除対象のデータ数

        nRowCount = drow.Length
        '現在画面に表示している前工程コードの非接続テーブルのデータを削除(後ろから削除)
        For ii As Integer = 1 To nRowCount
            m_DsMaeType.Tables(0).Rows.Remove(drow(nRowCount - ii))
        Next ii

        ' 前工程疵種dgvの値を前工程疵種非接続テーブルへ格納
        For ii As Integer = 0 To dgvMaeType.Rows.Count - 1
            'ID が空白以外の行だけ取得
            If CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, ii).Value) = "" Then Continue For

            Dim drowNew As DataRow = m_DsMaeType.Tables(0).NewRow()  ' 新規行

            drowNew.Item("前工程コード") = key                                                                 ' 前工程コード
            drowNew.Item("FFName") = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_FFName, ii).Value)                ' FFName
            drowNew.Item("ID") = CInt(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, ii).Value)                        ' ID
            drowNew.Item("Name") = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value)                    ' 疵名称
            drowNew.Item("優先度") = CInt(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Yusen, ii).Value)                 ' 優先度
            drowNew.Item("マップ表示名称") = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_MapDispName, ii).Value)   ' マップ表示名称
            ' 表示有無
            If dgvMaeType(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, ii).Value.ToString = "-" Then
                drowNew.Item("表示有無") = 0
            Else
                drowNew.Item("表示有無") = 0
                For jj As Integer = 0 To m_sMaeGrade.Length - 1
                    If dgvMaeType(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, ii).Value.ToString = m_sMaeGrade(jj) Then
                        For ll As Integer = 0 To drowGrade.Length - 1
                            If m_nMaeGradeID(jj) = CInt(drowGrade(ll).Item("グレードID")) Then
                                drowNew.Item("表示有無") = drowGrade(ll).Item("グレードID")
                                Exit For
                            End If
                        Next ll
                    End If
                Next jj
            End If

            ' テーブルに追加
            m_DsMaeType.Tables(0).Rows.InsertAt(drowNew, m_DsMaeType.Tables(0).Rows.Count)
        Next ii

    End Sub

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <param name="key">主キーの値(前工程コード)</param>
    ''' <remarks></remarks>
    Private Sub Data_MaeGradeGet(ByVal key As String)
        Dim strWhere As String = String.Format("前工程コード='{0}'", key)       ' WHERE句に相当の条件生成
        Dim drow() As DataRow                                       ' データ行
        Dim nRowCount As Integer                                    ' 削除対象のデータ数
        drow = m_DsMaeGrade.Tables(0).Select(strWhere)              ' 前工程コードが主キーの値の行を取り出す

        '現在画面に表示している前工程コードの非接続テーブルのデータを削除(後ろから削除)
        nRowCount = drow.Length
        For ii As Integer = 1 To nRowCount
            m_DsMaeGrade.Tables(0).Rows.Remove(drow(nRowCount - ii))
        Next ii

        ' グレードdgvの値を前工程グレード非接続テーブルへ格納
        For ii As Integer = 0 To dgvMaeGrade.RowCount - 1

            'グレードID が空白以外の行だけ取得
            If CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, ii).Value) = "" Then Continue For

            Dim drowNew As DataRow = m_DsMaeGrade.Tables(0).NewRow()  ' 新規行

            drowNew.Item("前工程コード") = key                                                                  ' 前工程コード
            drowNew.Item("FFName") = CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_FFName, ii).Value)               ' FFName
            drowNew.Item("グレードID") = CInt(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, ii).Value)          ' グレードID
            drowNew.Item("Name") = CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value)                   ' グレード名称
            drowNew.Item("優先度") = CInt(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Yusen, ii).Value)                ' 優先度

            'マップ表示色 ボタンの背景色をARGB形式にして格納
            drowNew.Item("マップ表示色") = _
                dgvMaeGrade(EM_DATACOLUMNGRADE.emMapButton, ii).Style.BackColor.ToArgb()

            ' テーブルに追加
            m_DsMaeGrade.Tables(0).Rows.InsertAt(drowNew, m_DsMaeGrade.Tables(0).Rows.Count)
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

            ' デリートインサートするので前工程疵種設定テーブルの全項目を削除
            strSql = "DELETE FROM " & DB_MAE_MASTER
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' エラーならロールバックして終了
                tcls_DB.TransactRollback()
                Return False
            End If

            '前工程疵種設定テーブルへデータを登録 
            'データテーブルは複数テーブルを参照している為、アップデートできないので、インサート文で
            For ii As Integer = 0 To m_DsMaeMaster.Tables(0).Rows.Count - 1
                With m_DsMaeMaster.Tables(0).Rows(ii)

                    If .Item("前工程コード").ToString <> "" Then
                        'MAE_MASTER登録SQL文作成()
                        strSql = ""
                        strSql &= "INSERT INTO " & DB_MAE_MASTER & " VALUES ('"
                        strSql &= .Item("前工程コード").ToString & "',"         ' 前工程コード
                        strSql &= .Item("有効判定").ToString & ","              ' 有効判定
                        strSql &= "'" & .Item("名称").ToString & "',"           ' 名称
                        strSql &= .Item("前工程表示モード").ToString & ","      ' 前工程表示モード
                        strSql &= .Item("画像形式").ToString & ","              ' 画像形式
                        strSql &= "'" & .Item("疵画像名称1").ToString & "',"    ' 疵画像名称1
                        strSql &= "'" & .Item("疵画像名称2").ToString & "',"    ' 疵画像名称2
                        strSql &= "'" & .Item("疵画像名称3").ToString & "',"    ' 疵画像名称3
                        strSql &= "'" & .Item("疵画像名称4").ToString & "',"    ' 疵画像名称4
                        strSql &= "'" & .Item("他所材判定").ToString & "')"     ' 他所材判定
                        'SQL実行
                        If 0 > tcls_DB.DMLExecute(strSql) Then
                            ' エラーならロールバックして終了
                            tcls_DB.TransactRollback()
                            Return False
                        End If
                    End If
                End With
            Next ii

            ' デリートインサートするので前工程疵種設定テーブルの全項目を削除
            strSql = "DELETE FROM " & DB_MAE_TYPE_NAME
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' エラーならロールバックして終了
                tcls_DB.TransactRollback()
                Return False
            End If

            '前工程疵種設定テーブルへデータを登録 
            'データテーブルは複数テーブルを参照している為、アップデートできないので、インサート文で
            For ii As Integer = 0 To m_DsMaeType.Tables(0).Rows.Count - 1
                With m_DsMaeType.Tables(0).Rows(ii)
                    'MAE_TYPE登録SQL文作成
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_MAE_TYPE_NAME & " VALUES ('"
                    strSql &= .Item("前工程コード").ToString & "',"             ' 前工程コード
                    strSql &= "'" & .Item("FFName").ToString & "',"             ' FFName
                    strSql &= .Item("ID").ToString & ","                        ' ID
                    strSql &= "'" & .Item("Name").ToString & "',"               ' 疵名称
                    strSql &= .Item("優先度").ToString & ","                    ' 優先度
                    strSql &= "'" & .Item("マップ表示名称").ToString & "',"     ' マップ表示名称
                    strSql &= .Item("表示有無").ToString & ")"                  ' 表示有無
                    'SQL実行
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' エラーならロールバックして終了
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                End With
            Next ii

            ' デリートインサートするので前工程グレード設定テーブルの全項目を削除
            strSql = "DELETE FROM " & DB_MAE_GRADE_NAME
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' エラーならロールバックして終了
                tcls_DB.TransactRollback()
                Return False
            End If

            '前工程グレード設定テーブルへデータを登録 
            'データテーブルは複数テーブルを参照している為、アップデートできないので、インサート文で
            For ii As Integer = 0 To m_DsMaeGrade.Tables(0).Rows.Count - 1
                With m_DsMaeGrade.Tables(0).Rows(ii)
                    'MAE_GRADE登録SQL文作成
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_MAE_GRADE_NAME & " VALUES ('"
                    strSql &= .Item("前工程コード").ToString & "',"             ' 前工程コード
                    strSql &= "'" & .Item("FFName").ToString & "',"             ' FFName
                    strSql &= .Item("グレードID").ToString & ","                ' グレードID
                    strSql &= "'" & .Item("Name").ToString & "',"               ' グレード名称
                    strSql &= .Item("優先度").ToString & ","                    ' 優先度
                    strSql &= .Item("マップ表示色").ToString & ")"              ' マップ表示色
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

            ' 統括機能へパラメータ変更通知
            modMain.Send_Main(CH_PARAM_MAE_MAST)
            modMain.HardCopy(Me)
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
    ''' 前工程マスターdgvの列定義
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub dgvInitializeDefMaster(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                       ' 選択色無し
        Call dgv.DataGridViewDataExec(dgv)

        With dgv
            .ColumnHeadersHeight = 42                                   ' ヘッダーの高さ
            .AllowUserToResizeColumns = True                            ' ユーザー 列サイズ変更
            .ScrollBars = ScrollBars.Both                               ' スクロールバーの表示

            ' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      ' 選択色
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     ' 一行おきに選択色変更
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
        End With

        '' 列定義
        ' No
        dgvColSetText(dgv, "No", 42, True, True, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' 前工程コード
        dgvColSetText(dgv, "前工程" & vbNewLine & "コード", 70, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' 有効無効
        dgvColSetCheck(dgv, "有効無効" & vbNewLine & "(ﾁｪｯｸ：有効)", 110, False, False, _
          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' 前工程名称
        dgvColSetText(dgv, "前工程名称", 95, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' 前工程表示モード
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "前工程" & vbNewLine & "表示モード"
            clm.Width = 103
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim listItemFF As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            listItemFF.Items.AddRange("対象外", "φω情報", "FF情報")
            listItemFF.MaxDropDownItems = listItemFF.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' 画像形式
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "画像形式"
            clm.Width = 80
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim listItemFF As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            listItemFF.Items.AddRange("JPEG", "BMP")
            listItemFF.MaxDropDownItems = listItemFF.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' 疵画像名称1
        dgvColSetText(dgv, "疵画像名称1" & vbNewLine & "(半角32文字)", 130, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' 疵画像名称2
        dgvColSetText(dgv, "疵画像名称2" & vbNewLine & "(半角32文字)", 130, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' 疵画像名称3
        dgvColSetText(dgv, "疵画像名称3" & vbNewLine & "(半角32文字)", 130, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' 疵画像名称4
        dgvColSetText(dgv, "疵画像名称4" & vbNewLine & "(半角32文字)", 130, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' 他所材判定
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "他所材判定"
            clm.Width = 100
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim listItemTasho As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            listItemTasho.Items.AddRange("自所材", "他所材")
            listItemTasho.MaxDropDownItems = listItemTasho.Items.Count
            dgv.Columns.Add(clm)
        End Using
    End Sub

    ''' <summary>
    ''' 前工程疵種dgvの列定義
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub dgvInitializeDefType(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
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

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      ' 選択色
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     ' 一行おきに選択色変更
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
        End With

        '' 列定義
        ' No
        dgvColSetText(dgv, "No", 42, True, True, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' FFName
        dgvColSetText(dgv, "FF" & vbNewLine & "Name", 50, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' ID
        dgvColSetText(dgv, "ID" & vbNewLine & "1以上", 54, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' Name
        dgvColSetText(dgv, "疵名称", 97, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' 優先度
        dgvColSetText(dgv, "優先度" & vbNewLine & "※１", 62, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.TopCenter, DataGridViewContentAlignment.TopCenter, "")

        ' マップ表示名称
        dgvColSetText(dgv, "マップ" & vbNewLine & "表示名称", 78, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' 表示有無
        dgvColSetText(dgv, "表示" & vbNewLine & "有無", 85, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

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
    ''' 前工程グレードdgvの列定義
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub dgvInitializeDefGrade(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
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

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      ' 選択色
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     ' 一行おきに選択色変更
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
        End With

        '' 列定義
        ' No
        dgvColSetText(dgv, "No", 42, True, True, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' FFName
        dgvColSetText(dgv, "FF" & vbNewLine & "Name", 50, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' グレードID
        dgvColSetText(dgv, "ｸﾞﾚｰﾄﾞID" & vbNewLine & "1以上", 78, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' グレード名称
        dgvColSetText(dgv, "ｸﾞﾚｰﾄﾞ名称" & vbNewLine & "(半角32文字)", 110, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' 優先度
        dgvColSetText(dgv, "優先度" & vbNewLine & "※１", 62, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' マップ表示色
        dgvColSetButton(dgv, "探傷画面" & vbNewLine & "表示色", 86, False, False, _
            DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
            New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "変更")

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
        Dim rec As Integer = CInt(IIf(bln, cmbMaeProcessCode.SelectedIndex + 1, cmbMaeProcessCode.SelectedIndex - 1))
        ' 上下限チェック
        If rec < 0 OrElse rec >= cmbMaeProcessCode.Items.Count Then Exit Sub
        ' 切替
        cmbMaeProcessCode.SelectedIndex = rec
    End Sub

    ''' <summary>
    ''' 有効無効列のチェック状態が変わったときに、背景色を変更する
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvMaeMaster_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvMaeMaster.CellValueChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)

        'ヘッダは処理しない
        If e.RowIndex = -1 Then Exit Sub

        ' 有効無効列か
        If e.ColumnIndex = EM_DATACOLUMNMASTER.emChk_Check Then
            If CBool(dgvMaeMaster(e.ColumnIndex, e.RowIndex).Value) Then
                'チェックを付けた時 背景色と選択色を規定色にする
                dgvMaeMaster.Rows(e.RowIndex).DefaultCellStyle.BackColor = g_ColorSetting
                dgvMaeMaster.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = g_ColorSetting
            Else
                'チェックを外した時 背景色と選択色ををデフォルトに戻す
                dgvMaeMaster.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty
                dgvMaeMaster.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.Empty
            End If
        End If
    End Sub

    ''' <summary>
    ''' 特定セルの値が変わったら色を変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvMaeType_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvMaeType.CellValueChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' 行以外はスルー
        If e.RowIndex < 0 Then Exit Sub

        ' 表示有無出力列なら色変更
        If e.ColumnIndex = EM_DATACOLUMNTYPE.emCmb_HyoziUmu Then

            ' セルの値を取得
            Dim strID As String = dgv(e.ColumnIndex, e.RowIndex).Value.ToString

            ' セルの値（ID）で色を取得し、背景色を変更する 
            dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = GetCmbColor(strID)
        End If
    End Sub

    ''' <summary>
    ''' 色取得
    ''' </summary>
    ''' <param name="key">GrEdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetCmbColor(ByVal key As String) As Color
        ' EdasysIDの値を取り出し
        If cmbMaeProcessCode.Text = "" Or key = "-" Or key = "" Then Return Color.Empty
        key = key.Remove(key.Length - 2, 2)
        Dim strWhere As String = String.Format("前工程コード='{0}' and name='{1}'", cmbMaeProcessCode.Text, key)   ' WHERE句に相当の条件生成
        Dim drowGrade() As DataRow = m_DsMaeGrade.Tables(0).Select(strWhere)       ' 1行あるはず

        Dim retColor As Color = Color.Empty   ' 戻り値 初期値はNULL色

        ' 1行しかないと思うけど
        For ii As Integer = 0 To drowGrade.Length - 1
            ' カラーを取得
            retColor = Color.FromArgb(CInt(drowGrade(0).Item("マップ表示色").ToString))
        Next ii

        Return retColor
    End Function

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
    ''' 疵種dgvのセルのボタンが押されたときの処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvType_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvMaeType.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' 行がクリックされていなければ終了
        If e.RowIndex < 0 Then Exit Sub

        ' 列インデックスでどの列がクリックされたか判別
        Select Case e.ColumnIndex
            Case EM_DATACOLUMNTYPE.emInsButton     '上へ挿入ボタン
                InsBtnClick_Type(dgv, e.RowIndex)
            Case EM_DATACOLUMNTYPE.emDelButton     '削除ボタン
                DelBtnClick_Type(dgv, e.RowIndex)
        End Select
    End Sub

    ''' <summary>
    ''' 前工程グレードdgvのセルのボタンが押されたときの処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGrade_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvMaeGrade.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' 行がクリックされていなければ終了
        If e.RowIndex < 0 Then Exit Sub

        ' 列インデックスでどの列がクリックされたか判別
        Select Case e.ColumnIndex
            Case EM_DATACOLUMNGRADE.emMapButton, EM_DATACOLUMNGRADE.emMapButton  'マップ表示色
                GetDlgColor(dgv, e.ColumnIndex, e.RowIndex)
        End Select
    End Sub

    ''' <summary>
    ''' 前工程疵種dgvの挿入ボタン押されたときの処理
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

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try
            '最終行のテキストボックスに何かしらのデータが入っている場合は挿入不可
            If Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_FFName, dgv.Rows.Count - 1).Value)) <> "" Or _
            Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_ID, dgv.Rows.Count - 1).Value)) <> "" Or _
            Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_Name, dgv.Rows.Count - 1).Value)) <> "" Or _
            Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_Yusen, dgv.Rows.Count - 1).Value)) <> "" Or _
            Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_MapDispName, dgv.Rows.Count - 1).Value)) <> "" Then
                frmMsg.Text = "条件確認"
                frmMsg.ChangeMsg("最終行にデータが存在します。", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                Exit Sub
            End If

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

        ' 前工程疵種dgvにレコードを挿入
        InsertAt_TypeData(nRowIndex)

        ' 前工程疵種dgvの最終行を削除
        dgv.Rows.RemoveAt(dgvMaeType.Rows.Count - 1)

        'Noを振りなおしボタンを表示しなおす
        InitIndexDgvMaeType(dgv)

    End Sub

    ''' <summary>
    ''' 前工程疵種dgvの削除ボタンが押されたときの処理
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="nRowIndex"></param>
    ''' <remarks></remarks>
    Private Sub DelBtnClick_Type(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
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

        ' 前工程疵種dgvの最終行にレコードを挿入
        InsertAt_TypeData(dgv.Rows.Count)

        ' 前工程疵種dgvのレコードを削除
        dgv.Rows.RemoveAt(nRowIndex)

        ' Noを振りなおしボタンを表示しなおす
        InitIndexDgvMaeType(dgv)

    End Sub

    ' 後に使用するかもしれないので残しておく
    ''' <summary>
    ''' dgvMaeMasterのインデックスを振り、ボタンの表示設定をする
    ''' </summary>
    ''' <param name="dgv">前工程マスターdgv</param>
    ''' <remarks></remarks>
    Private Sub InitIndexDgvMaeMaster(ByVal dgv As DataGridView)
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv(EM_DATACOLUMNMASTER.emNo, ii).Value = ii + 1   ' 列のインデックス番号
        Next ii
    End Sub

    ''' <summary>
    ''' dgvMaeTypeのインデックスを振り、ボタンの表示設定をする
    ''' </summary>
    ''' <param name="dgv">前工程疵種dgv</param>
    ''' <remarks></remarks>
    Private Sub InitIndexDgvMaeType(ByVal dgv As DataGridView)
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv(EM_DATACOLUMNTYPE.emNo, ii).Value = ii + 1   ' 列のインデックス番号
            ' 挿入ボタンの使用可能状態を設定    (ボタンを使用不可にはできないので、色を変更）
            If ii = dgv.RowCount - 1 Then
                ' 最終行は挿入ボタンが使用不可
                dgv(EM_DATACOLUMNTYPE.emInsButton, ii).Style.ForeColor = Color.Gray
            Else
                ' それ以外は使用可
                dgv(EM_DATACOLUMNTYPE.emInsButton, ii).Style.ForeColor = Color.Black
            End If
        Next ii
    End Sub

    ''' <summary>
    ''' dgvMaeGradeのインデックスを振り、ボタンの表示設定をする
    ''' </summary>
    ''' <param name="dgv">前工程グレードdgv</param>
    ''' <remarks></remarks>
    Private Sub InitIndexDgvMaeGrade(ByVal dgv As DataGridView)
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv(EM_DATACOLUMNGRADE.emNo, ii).Value = ii + 1   ' 列のインデックス番号
        Next ii
    End Sub

    ''' <summary>
    ''' dgvの選択が変更されたら、セルの選択状態をキャンセル
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dataGridView_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvMaeMaster.SelectionChanged, dgvMaeType.SelectionChanged, dgvMaeGrade.SelectionChanged
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
    Private Sub dataGridView_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvMaeMaster.CellEnter, dgvMaeType.CellEnter, dgvMaeGrade.CellEnter
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

End Class