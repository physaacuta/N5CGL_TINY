'*********************************************************************************
'PictureBoxの独自クラス
'	[Ver]
'		Ver.01    2006/03/04  vs2005 対応
'
'	[メモ]
'		疵検では、探傷画面の疵画像部分に使用する。
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.Windows.Forms						' コントロール

Namespace tCnt

    ' '' ↓のデザイナアトリビュートを追加したのは、MSのバグの対策。
    '' ※ このほかにも、TableLayoutPanel、MenuStrip、ToolStrip 等々が有る
    '' ・DataGridViewが、継承したフォームでそのDataGridViewオブジェクトのプロパティを変更できない
    '' ・デザイナからのサイズ変更等が出来ない
    '<System.ComponentModel.Designer("System.Windows.Forms.Design.ControlDesigner, System.Design")> _
    Public Class tCnt_PictureBox
        Inherits PictureBox

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
    End Class
End Namespace