Imports System.ComponentModel

Public Class frmPopGridLine
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���J�C�x���g
    Public Event tEv_RePaint()                                                          ' �ĕ`��

    '/ /////////////////////////////////////////////////////////////////////////////////
    ' ���J�ϐ�
    Public g_nX As Integer                  ' X�Ԋu [mm]
    Public g_nY As Integer                  ' Y�Ԋu [mm]
    Public g_nOffsetX As Integer            ' X�Y���X [mm]
    Public g_nOffsetY As Integer            ' Y�Y���X [mm]

    Private m_nDefVal As Integer = 0        ' �f�t�H���g�l [mm] ���s�b�`�w�莞�̓s�b�`


    ''/ /////////////////////////////////////////////////////////////////////////////////
    '' �R�C���`�F���W
    ''' <summary>
    ''' �R�C�������l
    ''' </summary>
    ''' <param name="nJumpVal">�W�����v�l [mm]</param>
    ''' <remarks></remarks>
    Public Sub ChangeDefVal(ByVal nJumpVal As Integer)
        If 0 >= nJumpVal Then nJumpVal = CInt(nudY.Value)

        m_nDefVal = nJumpVal
        nudY.Value = nJumpVal
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    ' ����
    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    Private Sub frmPop_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SetFormLocationAtOnce = True

        ' �����l�Z�b�g
        Call SetData()
    End Sub

    Protected Overrides Sub OnClosing(e As CancelEventArgs)
        MyBase.OnClosing(e)
        RaiseEvent tEv_RePaint()
    End Sub

    '''' <summary>
    '''' ���ʔ��f
    '''' </summary>
    'Private Sub btnExec_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExec.Click
    '	Call SetData()
    'End Sub

    ''' <summary>
    ''' �f�[�^�Z�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetData()
        g_nX = CInt(nudX.Value)
        g_nY = CInt(nudY.Value)
        g_nOffsetX = CInt(trbOffsetX.Value)
        g_nOffsetY = CInt(trbOffsetY.Value)

        ' �C�x���g�ʒm
        RaiseEvent tEv_RePaint()
    End Sub

    ''' <summary>
    ''' �l�ύX
    ''' </summary>
    Private Sub nud_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles nudY.ValueChanged, nudX.ValueChanged, trbOffsetY.ValueChanged, trbOffsetX.ValueChanged
        Call SetData()
    End Sub


    ''' <summary>
    ''' �����l
    ''' </summary>
    Private Sub btnReLoad_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnReLoad.Click
        ' �ݒ�
        nudY.Value = m_nDefVal
    End Sub

End Class