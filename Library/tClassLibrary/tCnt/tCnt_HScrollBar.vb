'*********************************************************************************
'ScrollBar�̓Ǝ��N���X
'	[Ver]
'		Ver.01    2009/03/08  vs2005 �Ή�
'
'	[����]
'		�X�N���[���o�[�Ńv���O�����ύX�ɂ��ValueChanged�Ŕ��f�ł���悤�ɂ����B
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.Windows.Forms						' �R���g���[��

Namespace tCnt
    Public Class tCnt_HScrollBar
        Inherits HScrollBar

        ''' <summary>
        ''' �p������ValueChanged�C�x���g
        ''' </summary>
        ''' <param name="sender">�ʏ�Ɠ���</param>
        ''' <param name="e">ValueProgramable�ŕύX��:True  �ʏ�:false</param>
        ''' <remarks></remarks>
        Public Shadows Event ValueChanged(ByVal sender As Object, ByVal e As Boolean)


        ''' <summary>
        ''' ��������p
        ''' </summary>
        ''' <remarks></remarks>
        Protected IsValueChangedProg As Boolean = False


        ''' <summary>
        ''' �I�[�o�[���C�h�p�B����OnValueChanged������肵�āA�C�x���g�𔭐�������
        ''' </summary>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Protected Overrides Sub OnValueChanged(ByVal e As EventArgs)
            If IsValueChangedProg Then
                IsValueChangedProg = False
                RaiseEvent ValueChanged(Me, True)
            Else
                RaiseEvent ValueChanged(Me, False)
            End If
        End Sub


        ''' <summary>
        ''' �v���O�������Œl��ύX�BValueChanged��e��True�ƂȂ�B
        ''' </summary>
        ''' <value></value>
        ''' <remarks></remarks>
        Public WriteOnly Property ValueProgramable() As Integer
            Set(ByVal value As Integer)
                If value < Me.Minimum Then value = Me.Minimum
                If value > Me.Maximum Then value = Me.Maximum

                IsValueChangedProg = True
                Me.Value = value
                IsValueChangedProg = False	' �O�̈�
            End Set
        End Property


        ''' <summary>
        ''' �X�N���[���������̍ő�l (GUI���瑀�삵���炱�ƒl�ȏ�͋A���Ă��Ȃ�)
        ''' </summary>
        Public ReadOnly Property MaximumVirtual() As Integer
            Get
                Return Me.Maximum - Me.LargeChange + 1
            End Get
        End Property

    End Class
End Namespace