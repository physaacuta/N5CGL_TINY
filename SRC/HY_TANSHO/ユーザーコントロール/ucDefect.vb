Public Class ucDefect

    ''' <summary>
    ''' タイトル文字の設定
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
       , System.ComponentModel.DefaultValue(True) _
       , System.ComponentModel.Category("☆疵情報") _
       , System.ComponentModel.Description("疵情報ラベル")>
    Public Property Title() As String
        Get
            Return lblDefTitle.Text
        End Get
        Set(ByVal value As String)
            lblDefTitle.Text = value
        End Set
    End Property

    ''' <summary>
    ''' タイトルラベルの色設定
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
       , System.ComponentModel.DefaultValue(True) _
       , System.ComponentModel.Category("☆疵情報") _
       , System.ComponentModel.Description("疵情報ラベル色")>
    Public Property TitleColor() As Color
        Get
            Return lblDefTitle.BackColor
        End Get
        Set(ByVal value As Color)
            lblDefTitle.BackColor = value
        End Set
    End Property


End Class
