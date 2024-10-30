Public Class PopBaseForm
    Inherits System.Windows.Forms.Form

    Dim m_bSetFormLocation As Boolean = False
    Dim m_bSetFormLocationAtOnce As Boolean = False

    ''' <summary>
    ''' 表示の都度、SetFormLocationで位置を初期化するか。
    ''' </summary>
    ''' <returns></returns>
    Public Property SetFormLocationAtOnce As Boolean
        Get
            Return m_bSetFormLocationAtOnce
        End Get
        Set(value As Boolean)
            m_bSetFormLocationAtOnce = True
        End Set
    End Property

    ''' <summary>
    ''' ウインドウを表示する
    ''' </summary>
    ''' <param name="fowner">TopMostライクにしたいばあい、メインフォームを指定。</param>
    Public Shadows Sub Show(ByVal fowner As Form)
        If fowner Is Nothing Then
            MyBase.Show()
        Else
            If MyBase.Visible Then
                MyBase.Activate()
            Else
                MyBase.Show(fowner)
            End If
        End If
    End Sub


    ''' <summary>
    ''' フォームの表示・非表示を反転させる
    ''' </summary>
    ''' <param name="fowner">TopMostライクにしたいばあい、メインフォームを指定。</param>
    ''' <returns>最終的に表示状態になった場合、TRUEを返す</returns>
    Public Function ShowToggle(ByVal fowner As Form) As Boolean
        If MyBase.Visible Then
            If Me.Owner IsNot Nothing Then
                Me.Owner.Activate()
            End If
            MyBase.Visible = False
            Return False
        Else
            Me.Show(fowner)
            Return True
        End If

    End Function

    ''' <summary>
    ''' ウインドウ位置調整を、表示の都度実行するか、起動時の1発目に限るか？
    ''' </summary>
    ''' <param name="e"></param>
    Protected Overrides Sub OnVisibleChanged(e As EventArgs)
        If Me.Visible Then
            If Not m_bSetFormLocationAtOnce Or (m_bSetFormLocationAtOnce And Not m_bSetFormLocation) Then
                Call SubFormLocation(Me, Me.Owner)
                m_bSetFormLocation = True
            End If
        End If
        MyBase.OnVisibleChanged(e)
    End Sub

    ''' <summary>
    ''' ×ボタンを無効化する。
    ''' </summary>
    ''' <param name="e"></param>
    Protected Overrides Sub OnFormClosing(e As FormClosingEventArgs)

        'これ以外のとき（OnwerCloseとかShutdownも効かせると副作用がおおきい）
        If e.CloseReason = CloseReason.UserClosing Then
            If Me.Owner IsNot Nothing Then
                Me.Owner.Activate()
            End If
            Me.Visible = False
            e.Cancel = True
            Exit Sub
        End If

        MyBase.OnFormClosing(e)
    End Sub
End Class
