Option Strict On

Imports tClassLibrary
Imports KizuForm

Public Class frmSecurityDisp

    '===========================================================
    ' �萔
    '===========================================================
    ' ====================
    ' �v�f�̕���
    ' ====================
    Private Const m_sNonString As String = "�s��"     ' �s�����̐F
    Private Const m_sOpenString As String = "�J"      ' �J��Ԏ��̐F
    Private Const m_sCloseString As String = "��"     ' ��Ԏ��̐F

    ' ====================
    ' �^�C�}�[����
    ' ====================
    Private Const ERR_CHECK_TIME As Integer = 500     '�G���[�������ݸ�����^�C�}�[
    Private m_lblData As New List(Of Control)

    '===========================================================
    ' �񋓌^
    '===========================================================
    ' ====================
    ' �v�f�̏��
    ' ====================
    Enum EM_STATUS
        emNon = -1                                  ' �s��
        emClose = 0                                 ' ��ԁi����j
        emOpen = 1                                  ' �J��ԁi�ُ�j
    End Enum

    '===========================================================
    ' �����o�[�ϐ�
    '===========================================================
    Private m_nData As Integer()                ' ���f�[�^

    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()
        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        '�^�C�g���ݒ�
        modMain.SetTitleName(Me)

        ' �\���Ԋu��0.5�b�Ԋu�ɂ���
        timDisp.Interval = ERR_CHECK_TIME
        SetLabel()
        '-------------------------------------------------------------------
        ' �t�@���N�V�����L�[�ݒ�
        '-------------------------------------------------------------------
        AddFunctionButton(cmdEnd, 12 - 1)
    End Sub

    Private Sub SetLabel()
        m_lblData.Add(lblStat11)    ' ���ްׯ��E����Ձ@�����
        m_lblData.Add(lblStat12)    ' ���ްׯ��E����Ձ@�����
    End Sub

    '***************************************************************************
    ' �L�[�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �t�@���N�V�����{�^��
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V�����{�^��No 0�I���W��</param>
    ''' <param name="onKey"></param>
    Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo                                                   ' �t�@���N�V�����{�^�� (0�I���W��)
                '---------------------------------------------------------------
                Case 12 - 1                                                     ' �I��
                    cmdEnd_Click(Nothing, Nothing)
            End Select
        Catch ex As Exception
        End Try
    End Sub
    ''' <summary>
    ''' ����{�^��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmdEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Me.Close()
    End Sub

    ''' <summary>
    ''' ���x���Ƀf�[�^���Z�b�g
    ''' </summary>
    ''' <param name="strName">���O</param>
    ''' <param name="nStat">�@����</param>
    ''' <remarks></remarks>
    Public Sub SetData(ByVal strName As String, ByVal nStat As Integer, ByVal clrDspColor As Color)
        ' �@��ID�������ȏꍇ
        For ii As Integer = 0 To m_lblData.Count - 1
            If m_lblData(ii).Name = strName Then
                ' �@��ID�������ȏꍇ
                Select Case nStat
                    Case EM_STATUS.emClose      ' ���
                        m_lblData(ii).Text = m_sCloseString
                    Case EM_STATUS.emOpen       ' �J���
                        m_lblData(ii).Text = m_sOpenString
                    Case Else                   ' ���̑�
                        m_lblData(ii).Text = m_sNonString
                End Select
                m_lblData(ii).BackColor = clrDspColor
            End If
        Next
    End Sub

    ''' <summary>
    ''' F02�{�^���̏�Ԃ�Ԃ�
    ''' </summary>
    ''' <returns>true:F02���� false:F02�ُ�</returns>
    ''' <remarks></remarks>
    Public Function ChkF02btn() As Boolean
        Dim nRet As Boolean = True  ' �߂�l

        ' �S���x�����[�v
        For ii As Integer = 0 To m_lblData.Count - 1
            ' ��Ԃ���ԈȊO�̏ꍇ
            If m_sCloseString <> m_lblData(ii).Text Then
                ' �߂�lFALSE
                nRet = False
                Exit For
            End If
        Next
        Return nRet
    End Function
End Class