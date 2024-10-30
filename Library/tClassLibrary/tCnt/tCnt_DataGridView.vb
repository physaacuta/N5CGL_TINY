'*********************************************************************************
'DataGridViewの独自クラス
'	[Ver]
'		Ver.01    2006/03/04  vs2005 対応
'
'	[メモ]
'		疵検では、探傷画面の疵マップ部分に使用する。
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.Windows.Forms						' コントロール
Imports System.Drawing

Namespace tCnt

    '' ↓のデザイナアトリビュートを追加したのは、MSのバグの対策。
    ' ※ このほかにも、TableLayoutPanel、MenuStrip、ToolStrip 等々が有る
    ' ・DataGridViewが、継承したフォームでそのDataGridViewオブジェクトのプロパティを変更できない
    ' ・デザイナからのサイズ変更等が出来ない
    <System.ComponentModel.Designer("System.Windows.Forms.Design.ControlDesigner, System.Design")> _
    Public Class tCnt_DataGridView
        Inherits DataGridView

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 外部プロパティ
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' ダブルバッファリングを外に公開
        ''' </summary>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.DefaultValue(True) _
         , System.ComponentModel.Category("☆専用プロパティ") _
         , System.ComponentModel.Description("ダブルバッファリングを外に公開")> _
        Public Shadows Property DoubleBuffered() As Boolean
            Get
                Return MyBase.DoubleBuffered
            End Get
            Set(ByVal value As Boolean)
                MyBase.DoubleBuffered = value
            End Set
        End Property


        ''' <summary>
        ''' オーナーコントロール変更
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Private Sub tCnt_DataGridView_ParentChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.ParentChanged
            ' 画面にコントロール貼り付けた時に、起動されるはず
            Me.DoubleBuffered = True
        End Sub


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ DataGridView 関連
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' 表示設定
        ''' </summary>
        ''' <param name="dgv">データグリッドビュー</param>
        ''' <remarks></remarks>
        Public Overloads Sub dgvInitialize(ByVal dgv As DataGridView)

            With dgv
                .ColumnCount = 0
                .AutoGenerateColumns = False									' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
                .AllowUserToAddRows = False										' ユーザー 行追加
                .AllowUserToDeleteRows = False									' ユーザー 行削除
                .AllowUserToOrderColumns = False								' ユーザー 列移動
                .AllowUserToResizeColumns = False								' ユーザー 列サイズ変更
                .AllowUserToResizeRows = False									' ユーザー 行サイズ変更
                .ShowCellErrors = False											' セルエラーの表示
                .ShowCellToolTips = False										' ツールヒントの表示
                .ShowEditingIcon = False										' 編集中のセルの明示
                .ShowRowErrors = False											' エラー行の明示
                .ScrollBars = ScrollBars.Horizontal								' スクロールバーの表示
                '.ColumnHeadersHeight = 42										' ヘッダーの高さ

                .SelectionMode = DataGridViewSelectionMode.CellSelect			' セル選択
                .MultiSelect = False											' 複数行選択
                .ReadOnly = False												' 読み取り専用はダメ
                .EditMode = DataGridViewEditMode.EditOnEnter					' セル書き換え

                .ColumnHeadersVisible = True									' 列ヘッダ表示
                .RowHeadersVisible = False										' 行ヘッダ表示
                .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None		' 可視列サイズ自動調整
                .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None			' 可視行サイズ自動調整
                .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing	'
                .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing			'

                ' 列ヘッダー 文字サイズ変更,文字色変更
                .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

                '' 選択セル色定義
                ''.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
                '.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
                ''.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
                '.DefaultCellStyle().SelectionForeColor = Color.Black
                '.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue	' 1行おきに色替え
                '.AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
                '.DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                '.DefaultCellStyle().ForeColor = Color.Black
            End With
        End Sub

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ データグリッドビュー 選択行 背景色無効化対応
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' 選択キャンセル
        ''' </summary>
        ''' <param name="dgv">データグリッドビューコントロール</param>
        ''' <remarks></remarks>
        Public Overridable Sub DataGridViewSelectionCancel(ByVal dgv As DataGridView)
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


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ データグリッドビュー 選択行 変更関係
        '/ /////////////////////////////////////////////////////////////////////////////////
        Private m_SelectionColor As Color				' 選択色

        ''' <summary>
        ''' 選択一行色変え
        ''' </summary>
        ''' <param name="dgv">データグリッドビューコントロール</param>
        ''' <remarks></remarks>
        Public Overridable Sub DataGridViewSelectionRowColorChange(ByVal dgv As DataGridView, ByVal selectionColor As Color)
            m_SelectionColor = selectionColor
            ' 下の関数にイベントハンドル割付
            AddHandler dgv.RowEnter, AddressOf dgvData_RowEnter
            AddHandler dgv.RowLeave, AddressOf dgvData_RowLeave
        End Sub
        Public Overridable Sub DataGridViewSelectionRowColorChange(ByVal selectionColor As Color)
            m_SelectionColor = selectionColor
            ' 下の関数にイベントハンドル割付
            AddHandler Me.RowEnter, AddressOf dgvData_RowEnter
            AddHandler Me.RowLeave, AddressOf dgvData_RowLeave
        End Sub
        ''' <summary>
        ''' データグリッドビュー行選択
        ''' </summary>
        Private Sub dgvData_RowEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
            Dim dgv As DataGridView = CType(sender, DataGridView)	' 対象のdgv

            dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = m_SelectionColor		'行の背景を変更
        End Sub
        ''' <summary>
        ''' データグリッドビュー行非選択
        ''' </summary>
        Private Sub dgvData_RowLeave(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
            Dim dgv As DataGridView = CType(sender, DataGridView)	' 対象のdgv

            dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty			'行の背景をデフォルト色に戻す
        End Sub


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 設定項目即時確定 動作定義
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' グリッド動作 ハンドル割り当て
        ''' </summary>
        ''' <param name="dgv"></param>
        ''' <remarks></remarks>
        Public Overridable Sub DataGridViewDataExec(ByVal dgv As DataGridView)
            'AddHandler dgv.CellValueChanged, AddressOf dgv_CellValueChanged
            AddHandler dgv.CurrentCellDirtyStateChanged, AddressOf dgvData_CurrentCellDirtyStateChanged
        End Sub

        ''' <summary>
        ''' セルの状態が変化した時の処理
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub dgvData_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs)
            Dim dgv As DataGridView = CType(sender, DataGridView)

            ' セルがコミットされていない変更を含む場合コミットする
            If dgv.IsCurrentCellDirty Then dgv.CommitEdit(DataGridViewDataErrorContexts.Commit)
        End Sub


        '''' <summary>
        '''' セルチェンジ処理
        '''' </summary>
        '''' <param name="sender"></param>
        '''' <param name="e"></param>
        '''' <remarks></remarks>
        'Private Sub dgv_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        '	Dim dgv As DataGridView = CType(sender, DataGridView)
        '	'ヘッダは処理しない
        '	If e.RowIndex = -1 Then Exit Sub

        '	If e.ColumnIndex = EM_FILTER_COL.emCheck Then
        '		If CBool(dgv(e.ColumnIndex, e.RowIndex).Value) Then
        '			'' チェックを付けた時
        '			dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen
        '			dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.LightGreen
        '		Else
        '			'' チェックを外した時
        '			dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty
        '			dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.Empty
        '		End If
        '	End If

        'End Sub


    End Class
End Namespace