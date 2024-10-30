'*********************************************************************************
' 間引き設定
'	[Ver]
'		Ver.01    2019/01/28  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcCropShrink
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' データグリッドビューの列
    Private Enum em_DataColumn
        emTxt_Order = 0                 ' テキストボックス  Order
        emTxt_Width                     ' テキストボックス  基準幅
        emTxt_Height                    ' テキストボックス  基準高
        emTxt_ShrinkRatio               ' テキストボックス  縮小率
        emTxt_InputRange                ' テキストボックス  入力範囲
        iInsButton                      ' 挿入ボタン
        iDelButton                      ' 削除ボタン
    End Enum

    '削除対象のID
    Private Structure TypeStruct
        Dim Order As Integer        ' 削除対象のOrder

        Public Sub initialize()
            Order = 0
        End Sub

    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterCropShrink As New SqlClient.SqlDataAdapter      ' アダプター
    Private m_DsCropShrink As New DataSet                            ' 非接続型テーブルPARAM_CROP_SHRINK
    Private mlst_DelID As New List(Of TypeStruct)                    ' 削除対象のID

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
    Private Sub frmEpcCropShrink_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "間引き設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
        ' 開放処理
        m_DsCropShrink.Dispose()
        If Not m_AdapterCropShrink Is Nothing Then
            m_AdapterCropShrink.Dispose()
            m_AdapterCropShrink = Nothing
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
    Private Sub frmEpcCropShrink_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        'タイトル設定
        Call SetTitleName(Me)

        modMain.SetListMenu(Me)                                     ' メニューリスト生成
        modMain.SetLabelSysName(Me)                                 ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmEpcCropShrink         ' 間引き設定呼び出し

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
            modMain.TaskEnd()                       '終了
        End If

        'dgv初期化
        dgvInitialize(dgvCropShrink)
        dgvColunmDef(dgvCropShrink)

        ' 画面に値を表示
        Data_Disp()

        ' 選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
        MyBase.DataGridViewSelectionRowColorChange(dgvCropShrink)

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
        strSQL = "SELECT [Order], "
        strSQL &= "Width, Height, ShrinkRatio "
        strSQL &= " FROM " & DB_PARAM_CROP_SHRINK
        strSQL &= " ORDER BY [Order] ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsCropShrink, m_AdapterCropShrink, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_REDUCTION & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

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
                modMain.Send_Main(CH_PARAM_CROP_SHRINK)
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
        dgvCropShrink.Rows.Insert(nRowIndex)

        ' データ初期化
        dgvCropShrink(em_DataColumn.emTxt_Order, nRowIndex).Value = ""       ' Order
        dgvCropShrink(em_DataColumn.emTxt_Width, nRowIndex).Value = ""       ' 基準幅
        dgvCropShrink(em_DataColumn.emTxt_Height, nRowIndex).Value = ""      ' 基準高
        dgvCropShrink(em_DataColumn.emTxt_ShrinkRatio, nRowIndex).Value = "" ' 縮小率
    End Sub

    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()

        ' セル値変更イベントを解除
        ' 当関数内でセル値を変更する為、イベントを解除
        RemoveHandler dgvCropShrink.CellValueChanged, AddressOf dgvCropShrink_CellValueChanged

        'データを表示
        Data_DispdgvReduction(dgvCropShrink)

        'インデックスとボタンの表示設定
        InitIndexdgvReduction(dgvCropShrink)

        ' 入力範囲設定
        Data_InputRangeSet(dgvCropShrink)

        ' 行の色を変える （初期表示で色が変わる対策）
        dgvCropShrink.CurrentRow.DefaultCellStyle.BackColor = Color.Empty

        ' セル値変更イベントを設定
        AddHandler dgvCropShrink.CellValueChanged, AddressOf dgvCropShrink_CellValueChanged

    End Sub

    ''' <summary>
    ''' dgvにデータを表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_DispdgvReduction(ByVal dgv As DataGridView)
        Dim drowCropShrink() As DataRow = m_DsCropShrink.Tables(0).Select()         ' 全レコードを表示するためWHERE句なし

        '行を初期化
        dgv.Rows.Clear()
        ' 画面に表示
        dgv.RowCount = MAX_CROP_SHRINK                                  ' dgvのレコード数は固定

        ' 画面に表示
        Try

            For ii As Integer = 0 To drowCropShrink.Length - 1
                Dim nLine As Integer = CInt(drowCropShrink(ii).Item("Order")) - 1     ' 格納行

                If ii < dgv.RowCount And nLine < dgv.RowCount Then
                    dgv(em_DataColumn.emTxt_Width, nLine).Value = drowCropShrink(ii).Item("Width")              ' 基準幅
                    dgv(em_DataColumn.emTxt_Height, nLine).Value = drowCropShrink(ii).Item("Height")            ' 基準高
                    dgv(em_DataColumn.emTxt_ShrinkRatio, nLine).Value = drowCropShrink(ii).Item("ShrinkRatio")  ' 縮小率
                End If
            Next ii

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_CROP_SHRINK & " 表示エラー [" & ex.Message & "] " & Me.Text)
        End Try

        ' カーソルスタート位置は入力エリアの先頭
        dgv(1, 0).Selected = True

        Return
    End Sub

    ''' <summary>
    ''' 入力範囲設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_InputRangeSet(ByVal dgv As DataGridView)
        Dim sBeforeWidth As String = "0"            ' 基準幅（前回値）
        Dim sBeforeHeight As String = "0"           ' 基準高（前回値）
        Dim lstRow As New List(Of DataGridViewRow)

        'dgv行ループ
        For ii As Integer = 0 To dgvCropShrink.Rows.Count - 1
            Dim sWidth As String = Trim(CStr(dgvCropShrink(em_DataColumn.emTxt_Width, ii).Value))             ' 基準幅
            Dim sHeight As String = Trim(CStr(dgvCropShrink(em_DataColumn.emTxt_Height, ii).Value))           ' 基準高
            Dim sShrinkRatio As String = Trim(CStr(dgvCropShrink(em_DataColumn.emTxt_ShrinkRatio, ii).Value)) ' 縮小率

            '幅 or 高さが未入力であれば入力範囲は未入力
            If sWidth = "" OrElse sHeight = "" Then
                dgvCropShrink(em_DataColumn.emTxt_InputRange, ii).Value = ""
                Continue For
            End If

            ' 入力範囲設定判定行を取得
            lstRow.Add(dgvCropShrink.Rows(ii))
        Next ii

        ' 幅で昇順にソート
        lstRow.Sort(AddressOf SortWidth)

        ' 入力範囲をソート順に設定
        For ii As Integer = 0 To lstRow.Count - 1
            ' チェック対象データ取得
            Dim sNowWidth As String = Trim(CStr(lstRow(ii).Cells(em_DataColumn.emTxt_Width).Value))            ' 基準幅
            Dim sNowHeight As String = Trim(CStr(lstRow(ii).Cells(em_DataColumn.emTxt_Height).Value))          ' 基準高
            Dim sShrinkRatio As String = Trim(CStr(lstRow(ii).Cells(em_DataColumn.emTxt_ShrinkRatio).Value))   ' 縮小率
            Dim nLen As Integer = CInt(lstRow(ii).Cells(em_DataColumn.emTxt_InputRange).Tag)                   ' 対象の行位置

            ' 入力範囲表示
            dgv(em_DataColumn.emTxt_InputRange, nLen).Value = String.Format("{0,4}≦基準幅＜{1,4},{2,4}≦基準高＜{3,4} → 縮小率={4}倍",
                                                                            sBeforeWidth, sNowWidth, sBeforeHeight, sNowHeight, sShrinkRatio)

            sBeforeWidth = sNowWidth            ' 基準幅（前回値）更新
            sBeforeHeight = sNowHeight          ' 基準高（前回値）更新
        Next

    End Sub

    ''' <summary>
    ''' 入力値チェック
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strWk As String     'dgvのデータ格納用
        Dim lstRow As New List(Of DataGridViewRow)
        Dim lstTarget As New List(Of Integer)

        'dgv行ループ
        For ii As Integer = 0 To dgvCropShrink.Rows.Count - 1
            Dim sWidth As String = Trim(CStr(dgvCropShrink(em_DataColumn.emTxt_Width, ii).Value))             ' 基準幅
            Dim sHeight As String = Trim(CStr(dgvCropShrink(em_DataColumn.emTxt_Height, ii).Value))           ' 基準高
            Dim sShrinkRatio As String = Trim(CStr(dgvCropShrink(em_DataColumn.emTxt_ShrinkRatio, ii).Value)) ' 縮小率

            '全て未入力であればチェック不要
            If sWidth = "" AndAlso sHeight = "" AndAlso sShrinkRatio = "" Then
                Continue For
            End If

            '基準幅
            strWk = CStr(dgvCropShrink(em_DataColumn.emTxt_Width, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 4096, False, "間引き設定 No=" & ii + 1 & " 基準幅") <> 0 Then Return False
            dgvCropShrink(em_DataColumn.emTxt_Width, ii).Value = strWk

            '基準高
            strWk = CStr(dgvCropShrink(em_DataColumn.emTxt_Height, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 4096, False, "間引き設定 No=" & ii + 1 & " 基準高") <> 0 Then Return False
            dgvCropShrink(em_DataColumn.emTxt_Height, ii).Value = strWk

            '縮小率
            strWk = CStr(dgvCropShrink(em_DataColumn.emTxt_ShrinkRatio, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 4, False, "間引き設定 No=" & ii + 1 & " 縮小率") <> 0 Then Return False
            dgvCropShrink(em_DataColumn.emTxt_ShrinkRatio, ii).Value = strWk

            ' チェック対象行を取得
            lstRow.Add(dgvCropShrink.Rows(ii))
        Next ii

        ' 幅で昇順にソート
        lstRow.Sort(AddressOf SortWidth)

        ' 間引き範囲の幅、高さが順に大きくなっているか判定
        Dim wkMsg As String = ""
        For ii As Integer = 0 To lstRow.Count - 1
            ' チェック対象データ取得
            Dim nTargetWidth As Integer = CInt(lstRow(ii).Cells(em_DataColumn.emTxt_Width).Value)
            Dim nTargetHeight As Integer = CInt(lstRow(ii).Cells(em_DataColumn.emTxt_Height).Value)
            Dim nTargetOrder As Integer = CInt(lstRow(ii).Cells(em_DataColumn.emTxt_Order).Value)

            ' 基準幅が同じデータはNG
            If lstTarget.Contains(nTargetWidth) Then
                Dim nErrOrder As Integer = CInt(lstRow(lstTarget.IndexOf(nTargetWidth)).Cells(em_DataColumn.emTxt_Order).Value)
                wkMsg = "【 間引き設定 No=" & nErrOrder & "," & nTargetOrder & " 基準幅,基準高" & " 】" & vbCrLf
                MsgBox(wkMsg & "適切な値ではありません。値を変更してください。", MsgBoxStyle.OkOnly, "入力チェック")
                Return False
            End If

            ' チェック対象データより幅が大きければ高さも大きいことを確認
            For jj As Integer = ii + 1 To lstRow.Count - 1
                Dim nComp As Integer = CInt(lstRow(jj).Cells(em_DataColumn.emTxt_Height).Value)
                If nTargetHeight >= nComp Then
                    Dim nErrOrder As Integer = CInt(lstRow(jj).Cells(em_DataColumn.emTxt_Order).Value)
                    wkMsg = "【 間引き設定 No=" & nTargetOrder & "," & nErrOrder & " 基準幅,基準高" & " 】" & vbCrLf
                    MsgBox(wkMsg & "適切な値ではありません。値を変更してください。", MsgBoxStyle.OkOnly, "入力チェック")
                    Return False
                End If
            Next

            ' データ格納
            lstTarget.Add(nTargetWidth)
        Next

        Return True

    End Function

    ''' <summary>
    ''' ソート関数
    ''' 幅でソート
    ''' </summary>
    ''' <param name="a"></param>
    ''' <param name="b"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function SortWidth(ByVal a As DataGridViewRow, ByVal b As DataGridViewRow) As Integer

        ' 判定値を取得
        Dim nCmpWidthA As Integer
        Dim nCmpWidthB As Integer
        tClass_ImputCheck.CheckData(a.Cells(em_DataColumn.emTxt_Width).Value.ToString, nCmpWidthA)
        tClass_ImputCheck.CheckData(b.Cells(em_DataColumn.emTxt_Width).Value.ToString, nCmpWidthB)

        ' 幅を判定
        If nCmpWidthA <= nCmpWidthB Then
            Return -1
        End If

        Return 1

    End Function

    ''' <summary>
    ''' 画面に表示中のOrderとDBのOrderを比べて、削除されたIDがあるかチェックする
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_CheckDelete()
        Dim drowReduction() As DataRow = m_DsCropShrink.Tables(0).Select()
        Dim bCheck As Boolean   'チェックフラグ TrueならIDがある
        Dim typ_Type As TypeStruct = Nothing

        ' 非接続テーブルで全行ループ
        For ii As Integer = 0 To drowReduction.Length - 1
            ' チェックフラグの初期化
            bCheck = False
            ' dgvで全行ループ
            For kk As Integer = 0 To dgvCropShrink.Rows.Count - 1

                typ_Type.Order = CInt(drowReduction(ii).Item("Order"))      ' Order

                ' Order比較
                If CInt(dgvCropShrink(em_DataColumn.emTxt_Order, kk).Value) = CInt(typ_Type.Order) Then
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

            ' チェックフラグがFalseのとき消されたOrderなので、登録する
            If bCheck = False Then
                typ_Type.initialize()
                ' 削除対象のOrderを格納
                typ_Type.Order = CInt(drowReduction(ii).Item("Order"))

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
    ''' 画面表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()
        Dim drowReduction() As DataRow = m_DsCropShrink.Tables(0).Select()
        Dim nRowCount As Integer    '削除対象のデータ数

        '現在画面に表示している前工程コードの非接続テーブルのデータを削除(後ろから削除)
        nRowCount = drowReduction.Length
        For ii As Integer = 1 To nRowCount
            m_DsCropShrink.Tables(0).Rows.Remove(drowReduction(nRowCount - ii))
        Next

        ' dgvの値を非接続テーブルへ格納
        For ii As Integer = 0 To dgvCropShrink.RowCount - 1
            Dim sOrder As String = CStr(dgvCropShrink(em_DataColumn.emTxt_Order, ii).Value)             ' Order
            Dim sWidth As String = CStr(dgvCropShrink(em_DataColumn.emTxt_Width, ii).Value)             ' 基準幅
            Dim sHeight As String = CStr(dgvCropShrink(em_DataColumn.emTxt_Height, ii).Value)           ' 基準高
            Dim sShrinkRatio As String = CStr(dgvCropShrink(em_DataColumn.emTxt_ShrinkRatio, ii).Value) ' 縮小率

            '全て入力されている行だけ取得
            If sWidth = "" AndAlso sHeight = "" AndAlso sShrinkRatio = "" Then Continue For
            Dim drowNew As DataRow = m_DsCropShrink.Tables(0).NewRow()    '新規行

            'データをセット
            drowNew.Item("Order") = CInt(sOrder)                ' Order
            drowNew.Item("Width") = CInt(sWidth)                ' 基準幅
            drowNew.Item("Height") = CInt(sHeight)              ' 基準高
            drowNew.Item("ShrinkRatio") = CInt(sShrinkRatio)    ' 縮小率
            'テーブルに追加
            m_DsCropShrink.Tables(0).Rows.InsertAt(drowNew, m_DsCropShrink.Tables(0).Rows.Count)
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
            ' Order
            dgvColSetText(dgv, "No", 40, True, True,
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

            ' 基準幅
            dgvColSetText(dgv, "基準幅" & vbNewLine & "( 0 - 4096 )", 147, False, False,
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

            ' 基準高
            dgvColSetText(dgv, "基準高" & vbNewLine & "( 0 - 4096 )", 147, False, False,
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

            ' 縮小率
            dgvColSetText(dgv, "縮小率" & vbNewLine & "( 1 - 4 )", 147, False, False,
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

            ' 入力範囲
            dgvColSetText(dgv, "入力範囲", 430, False, True,
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

            ' 上へ挿入
            dgvColSetButton(dgv, "上へ" & vbNewLine & "挿入", 50, False, False,
                            DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter,
                            New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "挿入")

            ' 削除
            dgvColSetButton(dgv, "削除", 50, False, False,
                            DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter,
                            New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "削除")

        End With
    End Sub


    ''' <summary>
    ''' dgvのセルのボタンが押されたときの処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvReduction_A_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvCropShrink.CellContentClick
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
        If Trim(CStr(dgv(em_DataColumn.emTxt_Width, dgv.Rows.Count - 1).Value)) <> "" Or
        Trim(CStr(dgv(em_DataColumn.emTxt_Height, dgv.Rows.Count - 1).Value)) <> "" Or
        Trim(CStr(dgv(em_DataColumn.emTxt_ShrinkRatio, dgv.Rows.Count - 1).Value)) <> "" Then
            ' <<----------------
            MsgBox("最終行にデータが存在します。")
            Exit Sub
        End If

        '挿入するかメッセージを出す
        If MsgBox("挿入しますか？", MsgBoxStyle.YesNo Or MsgBoxStyle.DefaultButton1, "条件確認") = MsgBoxResult.No Then Exit Sub

        ' dgvにレコードを挿入
        InsertAt_TypeData(nRowIndex)

        ' dgvの最終行を削除
        dgv.Rows.RemoveAt(dgvCropShrink.Rows.Count - 1)

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

            ' 入力範囲設定時に行番号を判別する為にTagに行番号を設定
            dgv(em_DataColumn.emTxt_InputRange, ii).Tag = ii

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

    ''' <summary>
    ''' セルの選択値が変更された場合、入力範囲を更新
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvCropShrink_CellValueChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        ' セル値変更イベントを解除
        ' 当関数内でセル値を変更する為、イベントを解除
        RemoveHandler dgvCropShrink.CellValueChanged, AddressOf dgvCropShrink_CellValueChanged

        Data_InputRangeSet(dgvCropShrink)

        ' セル値変更イベントを設定
        AddHandler dgvCropShrink.CellValueChanged, AddressOf dgvCropShrink_CellValueChanged
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

            strSql = "DELETE FROM " & DB_PARAM_CROP_SHRINK
            'SQL文の発行
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' エラーならロールバックして終了
                tcls_DB.TransactRollback()
                Return False
            End If

            'データを登録 
            For ii As Integer = 0 To m_DsCropShrink.Tables(0).Rows.Count - 1
                With m_DsCropShrink.Tables(0).Rows(ii)

                    '登録SQL文作成
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_PARAM_CROP_SHRINK & " VALUES ("

                    strSql &= .Item("Order").ToString & ","                 'Order
                    strSql &= .Item("Width").ToString & ","                 '基準幅
                    strSql &= .Item("Height").ToString & ","                '基準高
                    strSql &= .Item("ShrinkRatio").ToString                 '縮小率
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
