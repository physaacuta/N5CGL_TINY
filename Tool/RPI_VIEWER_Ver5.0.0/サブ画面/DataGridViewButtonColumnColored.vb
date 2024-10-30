Public Class DataGridViewButtonCellColored
    Inherits DataGridViewButtonCell


    Protected Overrides Function GetValue(rowIndex As Integer) As Object
        If Me.UseColumnTextForButtonValue AndAlso _
            Me.DataGridView IsNot Nothing AndAlso _
            Me.DataGridView.NewRowIndex <> rowIndex AndAlso _
            Me.OwningColumn IsNot Nothing Then

            Dim c As DataGridViewButtonColumnColored = TryCast(Me.OwningColumn, DataGridViewButtonColumnColored)
            If c IsNot Nothing Then
                Return c.Text
            End If

        End If
        Return MyBase.GetValue(rowIndex)
    End Function



    Protected Overrides Sub Paint(g As Graphics, clipBounds As Rectangle, cellBounds As Rectangle, rowIndex As Integer, elementState As DataGridViewElementStates, value As Object, formattedValue As Object, errorText As String, cellStyle As DataGridViewCellStyle, advancedBorderStyle As DataGridViewAdvancedBorderStyle, paintParts As DataGridViewPaintParts)
        '        MyBase.Paint(graphics, clipBounds, cellBounds, rowIndex, elementState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts)
        Dim ptCurrentCell As Point = Me.DataGridView.CurrentCellAddress
        Dim cellSelected As Boolean = (elementState And DataGridViewElementStates.Selected) <> 0
        Dim cellCurrent As Boolean = (ptCurrentCell.X = Me.ColumnIndex And ptCurrentCell.Y = rowIndex)

        Dim valBounds As Rectangle = cellBounds

        Using backBrush As New SolidBrush(cellStyle.BackColor)
            g.FillRectangle(backBrush, valBounds)
        End Using

        ControlPaint.DrawBorder(g, valBounds, SystemColors.Control, ButtonBorderStyle.Outset)

        Dim formattedString As String = CStr(GetValue(rowIndex))
        If Not String.IsNullOrEmpty(formattedString) Then

            Dim flags As TextFormatFlags = TextFormatFlags.VerticalCenter Or TextFormatFlags.HorizontalCenter


            TextRenderer.DrawText(g, formattedString, cellStyle.Font, valBounds, cellStyle.ForeColor, flags)

        End If

    End Sub
End Class

Public Class DataGridViewButtonColumnColored
    Inherits DataGridViewColumn


    Dim m_text As String

    Sub New()
        MyBase.New(New DataGridViewButtonCellColored())
        Dim DefaultCellStyle As New DataGridViewCellStyle
        DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
        MyBase.DefaultCellStyle = DefaultCellStyle
    End Sub

    Public Property UseColumnTextForButtonValue As Boolean
        Get
            Return DirectCast(Me.CellTemplate, DataGridViewButtonCellColored).UseColumnTextForButtonValue
        End Get
        Set(value As Boolean)
            DirectCast(Me.CellTemplate, DataGridViewButtonCellColored).UseColumnTextForButtonValue = value
        End Set
    End Property

    Public Property Text As String
        Get
            Return m_text
        End Get
        Set(value As String)
            m_text = value
        End Set
    End Property

    Public Overrides Function Clone() As Object
        Return MyBase.Clone()

    End Function

End Class
