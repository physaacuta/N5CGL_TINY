Module modLib
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 処理
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' フォームタイトル設定
    ''' </summary>
    ''' <param name="frmData">フォーム</param>
    ''' <remarks></remarks>
    Public Sub SetTitleName(ByRef frmData As Form)

        frmData.Text = String.Format("[{0}] {1}", "Tool", frmData.Text)

    End Sub

    ''' <summary>
    ''' 画面高さをPCの解像度に合わせて設定
    ''' </summary>
    ''' <param name="frmData">フォーム</param>
    ''' <remarks></remarks>
    Public Sub SetPcHeight(ByRef frmData As Form)

        '画面の高さがPC解像度の高さより大きい場合
        Dim nScreenHeigth As Integer = Screen.FromControl(frmData).Bounds.Height
        If nScreenHeigth < frmData.Height Then
            frmData.Height = nScreenHeigth      ' PCの解像度を高さに設定
        End If

    End Sub

    ''' <summary>
    ''' 画面幅をPCの解像度に合わせて設定
    ''' </summary>
    ''' <param name="frmData">フォーム</param>
    ''' <remarks></remarks>
    Public Sub SetPcWidth(ByRef frmData As Form)

        '画面の幅がPC解像度の幅より大きい場合
        Dim nScreenWidth As Integer = Screen.FromControl(frmData).Bounds.Width
        If nScreenWidth < frmData.Width Then
            frmData.Width = nScreenWidth      ' PCの解像度を幅に設定
        End If

    End Sub
End Module
