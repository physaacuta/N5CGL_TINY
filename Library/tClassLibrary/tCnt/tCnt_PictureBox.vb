'*********************************************************************************
'PictureBox�̓Ǝ��N���X
'	[Ver]
'		Ver.01    2006/03/04  vs2005 �Ή�
'
'	[����]
'		�r���ł́A�T����ʂ��r�摜�����Ɏg�p����B
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.Windows.Forms						' �R���g���[��

Namespace tCnt

    ' '' ���̃f�U�C�i�A�g���r���[�g��ǉ������̂́AMS�̃o�O�̑΍�B
    '' �� ���̂ق��ɂ��ATableLayoutPanel�AMenuStrip�AToolStrip ���X���L��
    '' �EDataGridView���A�p�������t�H�[���ł���DataGridView�I�u�W�F�N�g�̃v���p�e�B��ύX�ł��Ȃ�
    '' �E�f�U�C�i����̃T�C�Y�ύX�����o���Ȃ�
    '<System.ComponentModel.Designer("System.Windows.Forms.Design.ControlDesigner, System.Design")> _
    Public Class tCnt_PictureBox
        Inherits PictureBox

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �O���v���p�e�B
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �_�u���o�b�t�@�����O���O�Ɍ��J
        ''' </summary>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.DefaultValue(True) _
         , System.ComponentModel.Category("����p�v���p�e�B") _
         , System.ComponentModel.Description("�_�u���o�b�t�@�����O���O�Ɍ��J")> _
        Public Shadows Property DoubleBuffered() As Boolean
            Get
                Return MyBase.DoubleBuffered
            End Get
            Set(ByVal value As Boolean)
                MyBase.DoubleBuffered = value
            End Set
        End Property


        ''' <summary>
        ''' �I�[�i�[�R���g���[���ύX
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Private Sub tCnt_DataGridView_ParentChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.ParentChanged
            ' ��ʂɃR���g���[���\��t�������ɁA�N�������͂�
            Me.DoubleBuffered = True
        End Sub
    End Class
End Namespace