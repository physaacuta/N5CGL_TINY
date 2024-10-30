
Public Class FontInfo
        Public name As String
        Public size As Integer
        Public style As Drawing.FontStyle
        Public Sub New()
            name = "ＭＳ ゴシック"
            size = 16
            style = Drawing.FontStyle.Bold
        End Sub

        Public Sub New(f As Drawing.Font)
            Me.name = f.Name
            Me.style = f.Style
            Me.size = CInt(f.SizeInPoints / 72 * 96)
        End Sub

        Public Sub New(s As String)
            Dim strs() As String = s.Split(","c)
            Me.name = strs(2)
            Me.size = Integer.Parse(strs(1))
            Me.style = CType(Integer.Parse(strs(0)), Drawing.FontStyle)
        End Sub

        Public Overrides Function ToString() As String
            Return String.Format("{0},{1},{2}", CInt(style), size, name)
        End Function

    Public Function CreateFont() As Drawing.Font
        Return New Drawing.Font(name, size, style, Drawing.GraphicsUnit.Pixel)
    End Function

    Public Function QueryDialog(Optional parent As Windows.Forms.IWin32Window = Nothing) As Boolean
        Dim fs As New Windows.Forms.FontDialog
        fs.Font = Me.CreateFont
        fs.AllowVerticalFonts = False
        fs.FontMustExist = True
        If fs.ShowDialog(parent) = Windows.Forms.DialogResult.OK Then
            Dim fi As New FontInfo(fs.Font)
            Me.name = fi.name
            Me.size = fi.size
            Me.style = fi.style
            Return True
        End If
        Return False
    End Function
End Class

