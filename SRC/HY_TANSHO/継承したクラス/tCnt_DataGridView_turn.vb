Option Strict On

Imports tClassLibrary.tCnt

Public Class tCnt_DataGridView_turn
	Inherits tCnt_DataGridView

	' 回転フラグ
	Private m_bTranspose As Boolean = False

#Region "プロパティ"
	''' <summary>
	''' 90度 回転設定
	''' </summary>
	''' <value>true:回転あり false:回転無し</value>
	''' <returns></returns>
	''' <remarks></remarks>
	Public Property bTanspose() As Boolean
		Get
			Return m_bTranspose
		End Get
		Set(ByVal value As Boolean)
			m_bTranspose = value
		End Set
	End Property

	Default Public Shadows ReadOnly Property Item(ByVal columnIndex As Integer, ByVal rowIndex As Integer) As System.Windows.Forms.DataGridViewCell
		Get
			If m_bTranspose Then
				Return MyBase.Item(rowIndex, columnIndex)
			Else
				Return MyBase.Item(columnIndex, rowIndex)
			End If

		End Get
	End Property

	Public Shadows Property RowCount() As Integer
		Get
			If m_bTranspose Then
				Return MyBase.ColumnCount
			Else
				Return MyBase.RowCount
			End If
		End Get
		Set(ByVal value As Integer)
			If m_bTranspose Then
				MyBase.ColumnCount = value
			Else
				MyBase.RowCount = value
			End If
		End Set
	End Property

	Public Shadows Property ColumnCount() As Integer
		Get
			If m_bTranspose Then
				Return MyBase.RowCount
			Else
				Return MyBase.ColumnCount
			End If
		End Get
		Set(ByVal value As Integer)
			If m_bTranspose Then
				MyBase.RowCount = value
			Else
				MyBase.ColumnCount = value
			End If

		End Set
	End Property

#End Region


#Region "公開イベント"

	Public Shadows Event CellMouseClick(ByVal sender As tCnt_DataGridView, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs)

	Public Shadows Event CellMouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs)

#End Region

	''' <summary>
	''' セルクリック時のイベントを横取りし、反転設定を考慮した値でイベントを発行する
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Sub tCellMouseClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs)
		Dim new_e As DataGridViewCellMouseEventArgs = e
		If m_bTranspose Then
			new_e = New DataGridViewCellMouseEventArgs(e.RowIndex, e.ColumnIndex, e.X, e.Y, e)
		End If
		RaiseEvent CellMouseClick(Me, new_e)
	End Sub

	''' <summary>
	''' セルでマウスダウンされたときのイベントを横取りし、反転設定を考慮した値でイベントを発行する
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Sub tCellMouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs)
		Dim new_e As DataGridViewCellMouseEventArgs = e
		If m_bTranspose Then
			new_e = New DataGridViewCellMouseEventArgs(e.RowIndex, e.ColumnIndex, e.X, e.Y, e)
		End If
		RaiseEvent CellMouseDown(Me, new_e)
	End Sub

	''' <summary>
	''' 
	''' </summary>
	''' <param name="dgv"></param>
	''' <remarks></remarks>
	Public Overloads Sub dgvInitialize(ByVal dgv As DataGridView)
		Call MyBase.dgvInitialize(dgv)
		AddHandler dgv.CellMouseClick, AddressOf tCellMouseClick
		AddHandler dgv.CellMouseDown, AddressOf tCellMouseDown
	End Sub

End Class
