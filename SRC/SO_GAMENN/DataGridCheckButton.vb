Imports System.ComponentModel

<DataGridViewColumnDesignTimeVisible(True)> Public Class DataGridViewCheckButtonColumn
    Inherits DataGridViewButtonColumn

    ReadOnly Property DataSource As Object   ' Must be Read-Only
        Get
            DataSource = Me.DataGridView.DataSource
        End Get
    End Property

    ' All these attributes make it possible to select a datamember for the CheckedDataPropertyName in the DataGridView Column designer
    <Bindable(True)>
    <TypeConverter("System.Windows.Forms.Design.DataMemberFieldConverter, System.Design")>
    <Editor("System.Windows.Forms.Design.DataMemberFieldEditor, System.Design",
            "System.Drawing.Design.UITypeEditor, System.Drawing")>
    <Category("Data")>
    Property CheckedDataPropertyName As String = ""

    Sub New()
        Me.CellTemplate = New DataGridViewCheckButtonCell
    End Sub

    Public Overrides Function Clone() As Object
        Dim newColumn As DataGridViewCheckButtonColumn = CType(MyBase.Clone, DataGridViewCheckButtonColumn)

        newColumn.CheckedDataPropertyName = Me.CheckedDataPropertyName
        Return newColumn
    End Function
End Class

<Bindable(True)>
Public Class DataGridViewCheckButtonCell
    Inherits DataGridViewButtonCell
    Private mbChecked As Boolean
    Private mbEnable As Boolean = False

    Public WriteOnly Property Enable As Boolean
        Set(value As Boolean)
            mbEnable = value
        End Set
    End Property

    Public Property Checked As Boolean
        Get
            Return mbChecked
        End Get
        Set(value As Boolean)
            mbChecked = value
        End Set
    End Property
    Protected Overrides Sub OnClick(e As DataGridViewCellEventArgs)
        If Not mbEnable Then Exit Sub
        Me.mbChecked = Not mbChecked
        DataGridView.Invalidate()

        MyBase.OnClick(e)
    End Sub

    Protected Overrides Sub Paint(graphics As System.Drawing.Graphics,
                                  clipBounds As System.Drawing.Rectangle,
                                  cellBounds As System.Drawing.Rectangle,
                                  rowIndex As Integer,
                                  elementState As System.Windows.Forms.DataGridViewElementStates,
                                  value As Object,
                                  formattedValue As Object,
                                  errorText As String,
                                  cellStyle As System.Windows.Forms.DataGridViewCellStyle,
                                  advancedBorderStyle As System.Windows.Forms.DataGridViewAdvancedBorderStyle,
                                  paintParts As System.Windows.Forms.DataGridViewPaintParts)

        Dim ButtonState As VisualStyles.PushButtonState = VisualStyles.PushButtonState.Normal

        If Me.DataGridView.DataSource IsNot Nothing Then
            If CallByName(CType(Me.DataGridView.DataSource, BindingSource).Item(rowIndex), CType(Me.OwningColumn, DataGridViewCheckButtonColumn).CheckedDataPropertyName, CallType.Get) IsNot Nothing Then
                ButtonState = VisualStyles.PushButtonState.Pressed
            End If
        Else
            If Me.mbChecked Then
                ButtonState = VisualStyles.PushButtonState.Pressed
            End If
        End If

        MyBase.Paint(graphics, clipBounds, cellBounds, rowIndex, elementState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts)
        Using br As New SolidBrush(cellStyle.BackColor)
            graphics.FillRectangle(br, cellBounds)
        End Using
        'Dim rbt As Rectangle = cellBounds
        'cellBounds.Offset(1, 1)
        'cellBounds.Width -= 2
        'cellBounds.Height -= 2


        If ButtonState <> VisualStyles.PushButtonState.Pressed Then
            ControlPaint.DrawBorder3D(graphics, cellBounds, Border3DStyle.Raised)
        Else
            ControlPaint.DrawBorder3D(graphics, cellBounds, Border3DStyle.Sunken)
        End If
        TextRenderer.DrawText(graphics, CStr(formattedValue), cellStyle.Font, cellBounds, cellStyle.ForeColor, cellStyle.BackColor, TextFormatFlags.VerticalCenter Or TextFormatFlags.HorizontalCenter)
        '        ButtonRenderer.DrawButton(graphics, cellBounds, formattedValue, cellStyle.Font, False, ButtonState)
    End Sub

End Class