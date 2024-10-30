'*********************************************************************************
'TabControlの独自クラス
'	[Ver]
'		Ver.01    2006/03/04  vs2005 対応
'
'	[メモ]
'		実行中にタブを見えなくする事が可能
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.Windows.Forms						' コントロール

Namespace tCnt

    Public Class tCnt_TabControl
        Inherits TabControl

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ プライベート変数
        '/ /////////////////////////////////////////////////////////////////////////////////
        Private m_cntPanel As New Panel						' 非表示パネル (タブ隠そうパネル)
        Private m_bTabVisible As Boolean = True				' タブの表示非表示
        Private m_bAddPanel As Boolean = False              ' 非表示パネルをフォームコントロールに追加完了時 true

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 外部プロパティ
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' タブの表示非表示
        ''' </summary>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.DefaultValue(True) _
         , System.ComponentModel.Category("☆専用プロパティ") _
         , System.ComponentModel.Description("タブの表示非表示")> _
        Public Property TabVisible() As Boolean
            Get
                Return m_bTabVisible
            End Get
            Set(ByVal value As Boolean)
                m_bTabVisible = value

                ' 設定反映
                Call tCnt_TabControl_Layout(Nothing, Nothing)
            End Set
        End Property


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ プライベートメソッド
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' 親コントロール変更
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Private Sub tCnt_TabControl_ParentChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.ParentChanged
            If m_bAddPanel Then Return

            '' 親フォームを見つける
            Dim wk As Control = GetParentForm(Me)
            If Not wk Is Nothing Then
                Dim base As Control = CType(wk, Control)

                '' パネルを追加
                base.Controls.Add(m_cntPanel)
                m_bAddPanel = True

                ' パネルの背景色をフォームのバックカラーに合わせる
                m_cntPanel.BackColor = base.BackColor

                '' 即時実行
                Call tCnt_TabControl_Layout(Nothing, Nothing)
            End If
        End Sub


        ''' <summary>
        ''' 非表示パネルサイズ変更
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Private Sub tCnt_TabControl_Layout(ByVal sender As Object, ByVal e As System.Windows.Forms.LayoutEventArgs) Handles Me.Layout
            If Not m_bAddPanel Then
                m_cntPanel.Visible = False
                Return
            End If

            '' 非表示パネルの位置・サイズ変更
            m_cntPanel.Location = Me.Location  ' New Point(0, 0)
            m_cntPanel.Size = New Drawing.Size(Me.Size.Width + 0, Me.ItemSize.Height + Me.Margin.Top)

            '' パネルの表示非表示変更
            If m_cntPanel.Visible = m_bTabVisible Then
                m_cntPanel.Visible = Not m_bTabVisible	' 表示非表示

                '' 非表示時の設定
                If Not m_bTabVisible Then
                    MyBase.TabStop = False
                Else
                    MyBase.TabStop = True
                End If
            End If

            m_cntPanel.BringToFront()	'最前面へ (どうもフォームを継承した場合は、有効になってない対策)

            ' テスト
            'm_cntPanel.BackColor = Drawing.Color.Cyan
            'Me.TabPages(0).Text = MyClass.ItemSize.Height.ToString
        End Sub


        ''' <summary>
        ''' 親のフォームを見つけ出す
        ''' </summary>
        ''' <param name="cnt">対象のコントロール</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Private Function GetParentForm(ByVal cnt As Control) As Control
            Dim wk As Control = cnt.Parent
            While True

                If wk Is Nothing Then
                    Exit While
                ElseIf TypeOf wk Is Form Or TypeOf wk Is Panel Or TypeOf wk Is GroupBox Then
                    Exit While
                Else
                    '次へ
                    wk = wk.Parent
                End If
            End While
            Return wk
        End Function
    End Class
End Namespace