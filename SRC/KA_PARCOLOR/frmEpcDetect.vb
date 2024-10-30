''*********************************************************************************
'' ＥＰＣ疵検出設定画面クラス
''	[Ver]
''		Ver.01    2012/10/08  初版
''
''	[メモ]
''		
''*********************************************************************************
Option Strict On

Public Class frmEpcDetect

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const GAMEN_ID_NAME As String = GAMEN_ID_EPC_DEFECT
    Private Const MY_EM_FRM_ID As EM_FRM_ID = EM_FRM_ID.emfrmEpcDetect
    Private Const MY_CH_PARAM_ID As Integer = CH_PARAM_DETECT
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
    Private Sub frmEpcDtect_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
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
                Call HardCopy(IMG_PATH, "疵検出設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
                modMain.ChgProject()            ' F09 KA_PARSETへ切替処理
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
    Private Sub frmEpcDetect_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        '' いろいろ開放処理

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
    Private Sub frm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        'タイトル設定
        Call SetTitleName(Me)

        'メニューリスト生成
        modMain.SetListMenu(Me)

        'システム名ラベル生成
        modMain.SetLabelSysName(Me)

        '自画面呼出
        MyBase.tProp_MyFrmID = MY_EM_FRM_ID
        'パラメータ変更区分セット
        MyBase.tProp_MyParamID = MY_CH_PARAM_ID

        ' カメラ名称セット
        Call modMain.SetCamAria(camName, 0)

        ' アップコードのコンボボックス
        MyBase.SetComboBox_Scode(cmbUpCodeItem)
        MyBase.SetCmbUpCodeItem(cmbUpCodeItem, dgvUpCodeItem)

        ' 更新ボタンの有効/無効
        MyBase.UpdateEnabled(g_bPassFlg)

        '=======================================================
        '対象データグリッドビュー設定
        Dim dgv() As DgvControlInfo
		ReDim dgv(2 - 1)

		dgv(0).dgv = dgvPScode
        dgv(0).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emRowType
		dgv(0).wSize.nNo = 0
        dgv(0).wSize.nName = 280
        dgv(0).wSize.nValue = 550
		dgv(0).wSize.nRange = 200

		dgv(1).dgv = dgvCamAngleScode
		dgv(1).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emColType
		dgv(1).wSize.initialize()
        '=======================================================
        Dim wk As TABLE_INFO
        MyBase.Clear_BaseDBList()

		wk.sTable = DB_PARAM_SCODE
        wk.SelectSQLFormat = SELECT_KEY_ORDER
        wk.UpdateSQLFormat = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.scode IN({5})"
        wk.bCopyDB = True
        MyBase.Add_BaseDBList(DB_PARAM_SCODE, wk)

        wk.sTable = DB_PARAM_CAMANGLE_SCODE
        wk.SelectSQLFormat = SELECT_KEY_ORDER
        wk.UpdateSQLFormat = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.camangle = b.camangle AND a.scode IN({5})"
        wk.bCopyDB = True
        MyBase.Add_BaseDBList(DB_PARAM_CAMANGLE_SCODE, wk)
        '=======================================================

        'データグリッド設定情報をDBから取得
        MyBase.InitGridSetting(GAMEN_ID_NAME, dgv)

        '検査表面状態Noコンボボックス情報初期セット
        MyBase.Set_cmbSCode_Item(cmbSCode, lblSCode)

        'ロード処理
        MyBase.InitLoading()
    End Sub

    ''' <summary>
    ''' ディスポーズ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEpcDetect_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '現在の表示中の画面が自分の画面ならアプリケーションを終了
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 画面特殊処理
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' データグリッドビューのセルのボタンが押されたときの処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvPScode_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvPScode.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        Dim Data As DataSetHolder = MyBase.Get_GridInfo(dgv)
        If Data Is Nothing Then
            Exit Sub
        End If
        Try
            If ((e.ColumnIndex <> EM_ROW_VERTION_ID.emRange) Or (e.RowIndex = -1)) Then
                Exit Try
            End If
            If (CType(Data.Rows(CInt(dgv.Item(EM_ROW_VERTION_ID.emNo, e.RowIndex).Value) - 1).Item("入力_表示ダイアログ"), CallType) = EM_DLG_TYPE.emFileDialog) Then
                With ofdDcdFile
                    .CheckFileExists = True     'ファイルが存在するか確認
                    .RestoreDirectory = True    'ディレクトリの復元
                    .ReadOnlyChecked = True
                    .ShowReadOnly = True
                    .Filter = _
                    "すべてのファイル(*.*)|*.*|DCDファイル(*.dcd)|*.dcd"
                    .FilterIndex = 2            'Filterプロパティの2つ目を表示

                    'ダイアログボックスを表示し、［開く]をクリックした場合
                    If .ShowDialog = Windows.Forms.DialogResult.OK Then
                        'テキストファイルが選択されている場合はファイルを開く
                        If System.IO.Path.GetExtension(.FileName) = ".dcd" Then

                            '開いたファイルをテキストボックスに表示する
                            dgv(EM_ROW_VERTION_ID.emValue, e.RowIndex).Value = .FileName
                        Else
                            MessageBox.Show("DCDファイルを指定してください", "エラー")
                            Exit Sub
                        End If
                    End If
                End With

            End If

        Catch ex As Exception
            gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.ERR, String.Format("備考欄ボタン処理 異常[{0}]", ex.Message))
        End Try

    End Sub

End Class
