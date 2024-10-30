
Imports tClassLibrary

Public Class clsRecvMail




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���J�C�x���g
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public Event tEv_RecvMail(ByVal nEvtNo As Integer, ByVal que As COMMON_QUE_STRMIX)	' �U�z�}�N���b�N (�C�x���gNo, Que�{�̏��)


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���[�J���ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private WithEvents mcls_Mail As tClassLibrary.tClass.tClass_MailSlot			   ' ���[���X���b�g��M�N���X
    Private m_RecvQueBuf As COMMON_QUE_STRMIX							' ���[���X���b�g��M�o�b�t�@

    Private m_bRecving As Boolean = False								' ��M�\true

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New(ByVal cont As Control)
        Try
            mcls_Mail = New tClass.tClass_MailSlot(g_typStartArgs.sTaskName, 300)
            mcls_Mail.Mail_ReadStart(m_RecvQueBuf, AddressOf tDele_GetMail, cont)
            m_bRecving = True

        Catch ex As Exception
            'MsgBox("Init���s")
            m_bRecving = False
            End
        End Try
    End Sub

    ''' <summary>
    ''' �f�X�g���N�^
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Dispose()

        Try
            mcls_Mail.Mail_ReadStop()
            mcls_Mail.Dispose()
            mcls_Mail = Nothing
        Catch ex As Exception
        Finally
            m_bRecving = False
        End Try
    End Sub

    ''' <summary>
    ''' ��M
    ''' </summary>
    ''' <param name="recvVal"></param>
    ''' <remarks></remarks>
    Public Sub tDele_GetMail(ByVal recvVal As System.ValueType)
        Dim que As COMMON_QUE_STRMIX = CType(recvVal, COMMON_QUE_STRMIX)
        Dim nEvtNo As Integer = que.nEventNo
        RaiseEvent tEv_RecvMail(nEvtNo, que)
    End Sub


End Class
