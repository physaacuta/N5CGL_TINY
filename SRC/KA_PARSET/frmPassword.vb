'*********************************************************************************
' �p�X���[�h�m�F��ʃN���X
'	[Ver]
'		Ver.01    2007/08/28  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

Public Class frmPassword

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O���C�x���g
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �t�@���N�V�����{�^�������C�x���g (�x�[�X�̒u������)
    ''' </summary>
    ''' <param name="FncNo">0�I���W��</param>
    ''' <remarks></remarks>
    Public Shadows Event tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �v���C�x�[�g�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_sPass As String = ""                                         '�p�X���[�h

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���\�b�h
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="sPass">�p�X���[�h</param>
    ''' <param name="sPassChar">PasswordChar</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal sPass As String, Optional ByVal sPassChar As Char = Nothing, Optional ByVal nImeMode As Windows.Forms.ImeMode = Windows.Forms.ImeMode.Disable, Optional ByVal sTitol As String = "")

        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

        '�p�X���[�h�ݒ�
        m_sPass = sPass
        txtPass.PasswordChar = sPassChar
        txtPass.ImeMode = nImeMode

        '�p�X���[�h�t�@�C���̐ݒ�l�̊m�F
        If "" = m_sPass Then
            '�p�X���[�h�t�@�C�����ݒ�܂��̓p�X���[�h���󔒏ꍇ��OK�{�^�������s�Ƃ���B
            txtPass.Enabled = False
            btnF01.Enabled = False
        End If

        If "" <> sTitol.Trim Then
            Me.Text = sTitol
        End If
        'lblMsg.Text = "�p�X���[�h����͂��Ă�������" & vbNewLine & "�p�X���[�h��" & m_sPass & "�ł��B"

    End Sub

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmPop_Pass_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        ''�L�����Z���{�^�����A�N�e�B�u�ɂ���
        'btnF02.Select()
        txtPass.Select()
    End Sub

    ''' <summary>
    ''' �t�@���N�V�����{�^���N���b�N
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmPop_Pass_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton
        Dim strMsg As String

        'OK�Ȃ�p�X���[�h�`�F�b�N
        If 0 = FncNo Then

            If Not CheckPass() Then
                strMsg = "�p�X���[�h���Ⴂ�܂��I"
                strMsg = strMsg & vbNewLine & "�ēx,���͂����ĉ������B"
                Dim frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "�p�X���[�h����", tMod.GetColorToInt(Color.Red))
                frmMsg.ShowDialog()
                Exit Sub
            Else
                Me.DialogResult = Windows.Forms.DialogResult.OK
            End If

        Else
            Me.DialogResult = Windows.Forms.DialogResult.Cancel
        End If

        Me.Close()

        '�Ăь��֒ʒm
        RaiseEvent tEv_FunctionButton(FncNo, onKey)

    End Sub
    ''' <summary>
    ''' �p�X���[�h�`�F�b�N
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckPass() As Boolean

        '�����͂Ȃ�I��
        If "" = txtPass.Text.Trim Then
            '�p�X���[�h������
            Return False
        End If

        '�p�X���[�h�t�@�C���̐ݒ�l�̊m�F
        If "" = m_sPass Then
            '�p�X���[�h�t�@�C�����ݒ�܂��̓p�X���[�h����
            Return False
        End If

        '�p�X���[�h�������Ă��邩�`�F�b�N
        If StrConv(m_sPass, vbLowerCase) = txtPass.Text Or _
            StrConv(m_sPass, vbUpperCase) = txtPass.Text Or _
            StrConv(StrConv(m_sPass, vbLowerCase), vbWide) = txtPass.Text Or _
            StrConv(StrConv(m_sPass, vbUpperCase), vbWide) = txtPass.Text Then
            Return True
        End If

        Return False

    End Function


    Private Sub txtPass_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles txtPass.KeyDown
        If e.KeyData = Keys.Enter Then
            Call Me.frmPop_Pass_tEv_FunctionButton(0, e.KeyData)
        End If
    End Sub

End Class
