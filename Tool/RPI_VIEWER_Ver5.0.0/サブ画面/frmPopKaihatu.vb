Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmPopKaihatu
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���J�C�x���g
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public Event tEv_ReCreate()																			' �摜�Đ���

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing				' RPI�t�@�C���\���N���X (�|�C���^)


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���J�֐�
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' RPI�N���X
    ''' </summary>
    ''' <param name="cls">RPI�N���X</param>
    ''' <remarks></remarks>
    Public Sub SetCls_RpiView(ByRef cls As tClass_ViewRpiThread)
        mcls_pRpiView = cls
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��{����
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    Private Sub frmPop_Control_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Dim bIsUseImgLibCLI As Boolean = CBool(tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "DRAW_MODE", TASKINI_NAME))
        Dim nQuarity As Integer = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "DRAW_QUARITY", TASKINI_NAME)

        '''' �R���g���[�������ݒ�
        If 0 = nQuarity Then
            radQuarity1.Checked = True
        Else
            radQuarity2.Checked = True
        End If
        If Not bIsUseImgLibCLI Then
            radDraw1.Checked = True
        Else
            radDraw2.Checked = True
        End If

    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �掿
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' ���[�h�ύX�C�x���g	' �C�x���g�͂��ꂼ�������o�^�ł悢�B
    ''' </summary>
    Private Sub radQuarity_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles radQuarity1.CheckedChanged, radDraw1.CheckedChanged
        Call SetQuarity()
    End Sub
    ''' <summary>
    ''' ���[�h�ύX
    ''' </summary>
    ''' <remarks></remarks>
    Sub SetQuarity()
        Dim bIsUseImgLibCLI As Boolean = CBool(IIf(radDraw1.Checked, False, True))
        Dim nQuarity As Integer = CInt(IIf(radQuarity1.Checked, 0, 1))

        '''' ���C�u�����ɐݒ�
        mcls_pRpiView.SetParam_Quarity(nQuarity, bIsUseImgLibCLI)

        '''' �ĕ`��
        RaiseEvent tEv_ReCreate()
    End Sub



End Class