'*********************************************************************************
' パラメータ設定画面の親フォーム
'	[Ver]
'		Ver.01    2007/03/31  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

''' <summary>
''' パラメータ設定画面用の基底クラス
''' </summary>
''' <remarks></remarks>
Public Class frmMain_ParSet

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 定数
	'/ /////////////////////////////////////////////////////////////////////////////////
	' 共通の色コード
	Public g_ColorSetting As Color = Color.PeachPuff					'選択セルの色設定（データグリッドビューのチェックボックス限定）
	Public g_ColorSelection As Color = Color.LightGreen					'選択セル一行分の色変え


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部イベント
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' メニューリストが選択された (継承先でオーバーライドしてね。)
	''' </summary>
	''' <param name="frmID">選択画面ID</param>
	''' <param name="cancel">選択を取りやめたい場合は、Trueをセット</param>
	''' <remarks></remarks>
	Public Event tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean)


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバ 変数
    '/ /////////////////////////////////////////////////////////////////////////////////
	Private my_frmID As Integer								' 自分の画面ID


	''' <summary>
	''' コンボボックスに表示するデータ
	''' </summary>
	''' <remarks>検査表面状態・検査パターン 用</remarks>
	Public Class cls_CmbItem
		''' <summary>主キー</summary>
		Public id As String									' 主キー
		''' <summary>紐付く名称</summary>
		Public name As String								' 紐付く名称
		''' <summary>有効無効フラグ 0:無効 1:有効</summary>
		Public num As Integer								' 有効無効フラグ 0:無効 1:有効

		''' <summary>
		''' コンストラクタ
		''' </summary>
		''' <param name="strID">主キー</param>
		''' <param name="strName">紐付く名称</param>
		''' <param name="inum">有効無効フラグ</param>		
		''' <remarks></remarks>
		Public Sub New(ByVal strID As String, ByVal strName As String, ByVal inum As Integer)
			id = strID
			name = strName
			num = inum
		End Sub

		''' <summary>
		''' 構造体の表示名称
		''' </summary>
		''' <returns></returns>
		''' <remarks></remarks>
		Public Overrides Function ToString() As String
			Return id
		End Function
	End Class




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 外部プロパティ
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 自分の画面ID(画面表示時に選ばれている項目)
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property tProp_MyFrmID() As Integer
        Get
            Return my_frmID
        End Get
		Set(ByVal Value As Integer)

			Dim nCnt As Integer = 0

			If Value < 0 Then Exit Property

			' 自分のIDとして保持
			my_frmID = Value
			' 自分のメニューリストを変更
			For Each lstData As ListViewItem In lstVMenu.Items
				If CInt(lstData.Tag) = Value Then

					'自分のIDのリストを色変え
					lstVMenu.Items(nCnt).BackColor = Color.Pink
					lstVMenu.Items(nCnt).Font = New Font(lstVMenu.Items(nCnt).Font, FontStyle.Bold)
					Exit For
				End If
				nCnt += 1
			Next

		End Set
    End Property


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メソッド
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' メニューアイテム追加
    ''' </summary>
    ''' <param name="strName">名称</param>
    ''' <param name="frmID">画面ID</param>
    ''' <remarks></remarks>
    Public Sub SetListMenu(ByVal strName As String, ByVal frmID As Integer)
        Dim item As New ListViewItem(strName)
        item.Tag = frmID
        lstVMenu.Items.Add(item)
    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_ParSet_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        '' メニューの定義
        With lstVMenu
			.Columns.Add("メニュー", .ClientSize.Width, HorizontalAlignment.Center)
		End With
    End Sub

    ''' <summary>
    ''' メニューリスト選択
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub lstVMenu_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles lstVMenu.Click
        If 0 = lstVMenu.SelectedItems.Count Then Exit Sub ' おかしい
        Dim frmID As Integer = CInt(lstVMenu.SelectedItems(0).Tag)     ' 選択位置取得
        If my_frmID = frmID Then Exit Sub ' 自分自身ならヤメ

        '' 継承先で処理させる
        Dim cancel As Boolean = False
        RaiseEvent tEv_SelectMenuList(frmID, cancel)

        '' キャンセルされた
        If cancel Then Exit Sub

        '' 自分の終了
        Me.Close()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ データグリッドビュー関連
    '/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 選択キャンセル
	''' </summary>
	''' <param name="dgv">データグリッドビューコントロール</param>
	''' <remarks></remarks>
	Public Sub DataGridViewSelectionCancel(ByVal dgv As DataGridView)
		' 下の関数にイベントハンドル割付
		AddHandler dgv.SelectionChanged, AddressOf dgvData_SelectionChanged
	End Sub

	''' <summary>
	''' データグリッドビュー選択を無効化
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
    Private Sub dgvData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        CType(sender, DataGridView).CurrentCell.Selected = False
    End Sub


	''' <summary>
	''' 選択一行色変え
	''' </summary>
	''' <param name="dgv">データグリッドビューコントロール</param>
	''' <remarks></remarks>
	Public Sub DataGridViewSelectionRowColorChange(ByVal dgv As DataGridView)
		' 下の関数にイベントハンドル割付
		AddHandler dgv.RowEnter, AddressOf dgvData_RowEnter
		AddHandler dgv.RowLeave, AddressOf dgvData_RowLeave
	End Sub
	''' <summary>
	''' データグリッドビュー行選択
	''' </summary>
	Private Sub dgvData_RowEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
		Dim dgv As DataGridView = CType(sender, DataGridView)	' 対象のdgv

		dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = g_ColorSelection		'行の背景を変更
	End Sub
	''' <summary>
	''' データグリッドビュー行非選択
	''' </summary>
	Private Sub dgvData_RowLeave(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
		Dim dgv As DataGridView = CType(sender, DataGridView)	' 対象のdgv

		dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty			'行の背景をデフォルト色に戻す
	End Sub



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 検査表面状態・検査パターン 取り込み関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 検査表面状態 を DBより取得し、コンボボックスにセット
    ''' </summary>
    ''' <param name="cmb">コンボボックス コントロール</param>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Public Function SetComboBox_Scode(ByVal cmb As ComboBox) As Boolean
        Dim strSQL As String = ""       ' SQL文生成
        strSQL = "SELECT scode, 検査表面状態名称, 有効判定"
        strSQL &= " FROM " & DB_SCODE_MASTER
        strSQL &= " ORDER BY scode ASC"

        ' SQL実行 & コンボボックスにセット
        Return SetComboBox(cmb, strSQL)
    End Function

    ''' <summary>
    ''' 検査パターン を DBより取得し、コンボボックスにセット
    ''' </summary>
    ''' <param name="cmb">コンボボックス コントロール</param>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Public Function SetComboBox_Pcode(ByVal cmb As ComboBox) As Boolean
        Dim strSQL As String = ""       ' SQL文生成
        strSQL = "SELECT pcode, 検査パターン名称, 有効判定"
        strSQL &= " FROM " & DB_PCODE_MASTER
        strSQL &= " ORDER BY pcode ASC"

        ' SQL実行 & コンボボックスにセット
        Return SetComboBox(cmb, strSQL)
    End Function

	''' <summary>
	''' 検査パターン を DBより取得し、コンボボックスにセット(PCODE数値対応版)
	''' </summary>
	''' <param name="cmb">コンボボックス コントロール</param>
	''' <returns>復帰情報</returns>
	''' <remarks></remarks>
	Public Function SetComboBox_Pcode_Ascii(ByVal cmb As ComboBox) As Boolean
		Dim strSQL As String = ""		' SQL文生成
		strSQL = "SELECT ascii(pcode) as pcode, 検査パターン名称, 有効判定"
		strSQL &= " FROM " & DB_PCODE_MASTER
		strSQL &= " ORDER BY pcode ASC"

		' SQL実行 & コンボボックスにセット
		Return SetComboBox(cmb, strSQL)
	End Function

    ''' <summary>
    ''' DBより取得し、コンボボックスにセット
    ''' </summary>
    ''' <param name="cmb">コンボボックス コントロール</param>
    ''' <param name="strSQL">SQL分</param>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function SetComboBox(ByVal cmb As ComboBox, ByVal strSQL As String) As Boolean
        Dim tcls_DB As New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing

        '' コンボボックス 初期化
        cmb.Items.Clear()

        Try
            ' SQL実行
            If Not tcls_DB.Open() Then Return False
            sqlRead = tcls_DB.SelectExecute(strSQL)
            ' データ取得
            If Not sqlRead Is Nothing Then
                While sqlRead.Read
                    '' コンボボックス にアイテム追加 (string int 両方あるのでGetValueにしている)
                    cmb.Items.Add(New cls_CmbItem(CStr(sqlRead.GetValue(0)), CStr(sqlRead.GetValue(1)), CInt(sqlRead.GetValue(2))))
                End While
            End If
            Return True
        Catch ex As Exception
            Return False
        Finally
            If Not sqlRead Is Nothing Then sqlRead.Close()
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
    End Function

    ''' <summary>
    ''' コンボボックス選択時に 紐付く名称をセット
    ''' </summary>
    ''' <param name="index">コンボボックスの選択位置</param>
    ''' <param name="cmb">コンボボックス</param>
    ''' <param name="lbl">名称表示ラベル</param>
    ''' <remarks></remarks>
    Public Sub SelectComboBoxItem(ByVal index As Integer, ByVal cmb As ComboBox, ByVal lbl As Label)
        Dim item As cls_CmbItem    'コンボボックスのアイテム
        item = CType(cmb.Items(index), cls_CmbItem)

        ' 名称をセット
        lbl.Text = item.name
        '有効か無効化で背景色変更
		lbl.BackColor = CType(IIf(item.num = 0, Color.LightGray, Color.PeachPuff), Color)
    End Sub

    ''' <summary>
    ''' コンボボックスの選択されいている主キーを取得する
    ''' </summary>
    ''' <param name="index">配列位置</param>
    ''' <param name="cmb">コンボボックス</param>
    ''' <returns>主キー</returns>
    ''' <remarks></remarks>
    Public Function SelectComboBoxKey(ByVal index As Integer, ByVal cmb As ComboBox) As String
        Dim item As cls_CmbItem    'コンボボックスのアイテム
        item = CType(cmb.Items(index), cls_CmbItem)
        Return item.id
    End Function

    ''' <summary>
    ''' 主キーからコンボボックスの配列位置を取得する
    ''' </summary>
    ''' <param name="key">主キー</param>
    ''' <param name="cmb">コンボボックス</param>
    ''' <returns>配列位置</returns>
    ''' <remarks></remarks>
    Public Function SelectComboBoxIndex(ByVal key As String, ByVal cmb As ComboBox) As Integer
        Dim item As cls_CmbItem    'コンボボックスのアイテム
        For ii As Integer = 0 To cmb.Items.Count - 1
            item = CType(cmb.Items(ii), cls_CmbItem)
            If item.id = key Then Return ii
        Next ii
        Return -1
    End Function

    ''' <summary>
    ''' コンボボックスの任意データを削除
    ''' </summary>
    ''' <param name="cmb">コンボボックス</param>
    ''' <param name="strDelItem">削除したい任意のデータ</param>
    ''' <remarks></remarks>
    Public Sub Delete_CodeEtc(ByVal cmb As ComboBox, ByVal strDelItem As String)
        ' アイテム数ループ
        For ii As Integer = 0 To cmb.Items.Count - 1
            ' アイテムがZなら
            If strDelItem = cmb.Items.Item(ii).ToString Then
                ' 削除
                cmb.Items.RemoveAt(ii)
                Exit For
            End If
        Next
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ SQL関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' データ取得
    ''' </summary>
    ''' <param name="strSQL">発行するSQL文</param>
    ''' <param name="DsDataSet">データセットオブジェクト</param>
    ''' <param name="AdapterData">データアダプタオブジェクト</param>
    ''' <param name="strErrMsg">エラーメッセージ</param>
    ''' <returns>影響を受けた行数を返す　-1:失敗　-9:DB接続失敗</returns>
    ''' <remarks></remarks>
    Public Function Get_SelectData(ByVal strSQL As String, ByRef DsDataSet As System.Data.DataSet, _
                                    ByRef AdapterData As SqlClient.SqlDataAdapter, ByRef strErrMsg As String) As Integer

        Dim tcls_DB As New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)

        'メッセージ初期化
        strErrMsg = ""

        Try

            ' SQL実行
            If Not tcls_DB.Open() Then
                strErrMsg = "DB接続失敗"
                Return -9
            End If

            ' データ取得
            DsDataSet.Clear()
            AdapterData = tcls_DB.SelectAdapter(strSQL)

            Return AdapterData.Fill(DsDataSet)


        Catch ex As Exception
            strErrMsg = ex.Message
            Return -1

        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

    End Function

End Class