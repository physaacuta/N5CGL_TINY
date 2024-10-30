'*********************************************************************************
'MailSlot�̑���M���s����{�N���X
'*********************************************************************************
Option Strict On                'tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.Threading

''''' �\���̂͂���Ȋ����ɒ�`���鎖
''<System.Runtime.InteropServices.StructLayout(Runtime.InteropServices.LayoutKind.Sequential)> _
''Public Structure laytest
''    Dim events As Integer
''    <System.Runtime.InteropServices.MarshalAs(System.Runtime.InteropServices.UnmanagedType.ByValTStr, SizeConst:=124)> Dim val As String
''    <System.Runtime.InteropServices.MarshalAs(Runtime.InteropServices.UnmanagedType.ByValArray, SizeConst:=32)> Dim yobi() As Integer

''    Public Sub init()
''        ReDim yobi(32)
''    End Sub
''End Structure

Namespace tClass
    Public Class tClass_MailSlot
        Inherits tClass_MailSlot_Base                   '�e�N���X
        Implements IDisposable                          '�f�X�g���N�^��`

        '********************************************************************
        '�����o�[�ϐ���`
        '********************************************************************
        '�C�x���g
        Private m_hEvent_End As AutoResetEvent          '�I���C�x���g

        '��M�p
        Protected m_RecvThread As System.Threading.Thread       '��M�X���b�h
        Private m_RecvValue As System.ValueType                 '��M�f�[�^
        Private m_ReadInterval As Integer                       '�Ǎ��ݎ���

        '��M��荞�݃C�x���g
        'Public Event getMailRecv(ByVal recvVal As ValueType)    '��M��荞�݃C�x���g
        Public Delegate Sub tDele_GetMail(ByVal recvVal As System.ValueType) '��M��荞�݃f���Q�[�g

        Protected Friend m_Control As System.Windows.Forms.Control      '��M��荞�ݗp�f���Q�[�g�I�u�W�F�N�g(�R���g���[���A�N�Z�X��)
        Protected Friend m_DeleGetMail As tDele_GetMail                 '��M��荞�ݗp�f���Q�[�g�I�u�W�F�N�g(�R���g���[���A�N�Z�X������)

        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        '----------------------------------------------------------------
        ' �T�v�F�R���X�g���N�^
        ' �����F(in :String) strMyTaskName = �������g�̃^�X�N����
        ' �����F(in :Integer) interval = ��M����(ms)
        ' �ߒl�F�Ȃ�
        ' ���l�F
        '----------------------------------------------------------------
        Public Sub New(ByVal strMyTaskName As String, ByVal interval As Integer)
            MyBase.new()        '�e�N���X�C���X�^���X����

            '''' ���[���X���b�g����
            If Not MyBase.Mail_Open(strMyTaskName) Then
                'Debug.Assert(False, "���[���X���b�g�������s")
                Throw New System.Exception("���[���X���b�g�������s")
            End If

            '''' private�ϐ��ɃZ�b�g
            m_hEvent_End = New AutoResetEvent(False)
            m_ReadInterval = interval
        End Sub

        '----------------------------------------------------------------
        ' �T�v�F�f�X�g���N�^
        ' �����F( :)  = 
        ' �ߒl�F�Ȃ�
        ' ���l�F
        '----------------------------------------------------------------
        Protected Overrides Sub Dispose(ByVal disposing As Boolean)
            '''' ��M�X���b�h��~
            Call Mail_ReadStop()

            '''' �C�x���g��n��
            m_hEvent_End.Close()

            '''' ���[���X���b�g�I��
            MyBase.Dispose(disposing)
        End Sub

        '----------------------------------------------------------------
        ' �T�v�F��M�J�n
        ' �����F(in :ValueType) objVal = ��M�\����
        ' �����F(in :tDele_GetMail) deleMethod = ��M���\�b�h
        ' �ߒl�F�Ȃ�
        ' ���l�F�������\�b�h�p(�R���g���[���ɃA�N�Z�X����)
        '----------------------------------------------------------------
        Public Sub Mail_ReadStart(ByVal objVal As ValueType, ByVal deleMethod As tDele_GetMail)
            If Not m_RecvThread Is Nothing Then Exit Sub

            '''' ��M�p�̈�Z�b�g
            m_RecvValue = objVal
            m_DeleGetMail = deleMethod
            m_Control = Nothing

            '''' ��M�X���b�h����
            m_RecvThread = New Thread(AddressOf MailThread)
            m_RecvThread.IsBackground = True   '�o�b�N�O���E���h�Ő���
            m_RecvThread.Priority = Threading.ThreadPriority.AboveNormal '�ʏ��肿����ƍ��߂̗D��x
            m_RecvThread.Start()
        End Sub

        '----------------------------------------------------------------
        ' �T�v�F��M�J�n
        ' �����F(in :ValueType) objVal = ��M�\����
        ' �����F(in :tDele_GetMail) deleMethod = ��M���\�b�h
        ' �����F(in :Control) meCont = ����R���g���[��(�ʏ�Me�ŗǂ��H�ł��p�t�H�[�}���X�I�ɂ͑���R���g���[���w��̕��������H�H)
        ' �ߒl�F�Ȃ�
        ' ���l�F
        '----------------------------------------------------------------
        Public Sub Mail_ReadStart(ByVal objVal As ValueType, ByVal deleMethod As tDele_GetMail, ByVal meCont As System.Windows.Forms.Control)
            If Not m_RecvThread Is Nothing Then Exit Sub

            '''' ��M�p�̈�Z�b�g
            m_RecvValue = objVal
            m_DeleGetMail = deleMethod
            m_Control = meCont

            '''' ��M�X���b�h����
            m_RecvThread = New Thread(AddressOf MailThread)
            m_RecvThread.IsBackground = True   '�o�b�N�O���E���h�Ő���
            m_RecvThread.Priority = Threading.ThreadPriority.AboveNormal '�ʏ��肿����ƍ��߂̗D��x
            m_RecvThread.Start()
        End Sub

        '----------------------------------------------------------------
        ' �T�v�F��M��~
        ' �����F( :)  = 
        ' �ߒl�F�Ȃ�
        ' ���l�F
        '----------------------------------------------------------------
        Public Sub Mail_ReadStop()
            '''' ���C���X���b�h�I���m�F
            If Not m_RecvThread Is Nothing Then
                '���C���X���b�h�̏I��
                m_hEvent_End.Set()
                '�X���b�h����~��ԂɂȂ�܂Ńu���b�N
                If Not m_RecvThread.Join(1000) Then
                    '�����X���b�h�� WaitSleepJoin ��ԂɂȂ��Ă���?(���肦�Ȃ�)
                    m_RecvThread.Interrupt()
                End If
                '�X���b�h�I��
                m_RecvThread.Abort()
                m_RecvThread = Nothing
                'm_DeleGetMail.RemoveAll(m_DeleGetMail, m_DeleGetMail)
            End If
        End Sub

        '----------------------------------------------------------------
        ' �T�v�F���C���X���b�h
        ' �����F( : )  = 
        ' �ߒl�F() 
        ' ���l�F
        '----------------------------------------------------------------
        Private Sub MailThread()
            Dim EventAry(1) As Threading.WaitHandle     '�C�x���g
            Dim iRetc As Integer
            Dim hEvent_Read As New AutoResetEvent(False)    '������M�C�x���g
            Dim iRecvSize As Integer                    '��M�f�[�^�T�C�Y

            '''' �C�x���g�Z�b�g
            EventAry(0) = m_hEvent_End
            EventAry(1) = hEvent_Read

            While (True)
                '''' �V�O�i���҂�
                iRetc = Threading.WaitHandle.WaitAny(EventAry, m_ReadInterval, True)
                '''' �܂����߂ɁA�X���b�h�I��
                If iRetc = 0 Then Exit While

                '''' �����Ǎ��� ������Ǎ���
                ''��M
                iRecvSize = MyBase.Mail_Recv(m_RecvValue)
                If iRecvSize > 0 Then           '��M�L��
                    '��M�C�x���g����
                    'RaiseEvent getMailRecv(m_RecvValue)
                    '''' ��M�f�[�^���f���Q�[�g�ɓn��
                    If m_Control Is Nothing Then
                        m_DeleGetMail.BeginInvoke(m_RecvValue, Nothing, Nothing)
                    Else
                        m_Control.BeginInvoke(m_DeleGetMail, New Object() {m_RecvValue})
                    End If

                    '��M�����ꍇ�́A���̔��������C�x���g�̃��W���[����������܂ő҂������̂ŁA���ɋ�����M������
                    hEvent_Read.Set()

                ElseIf iRecvSize = 0 Or iRecvSize = -1 Then	   '��M�f�[�^����

                Else							'�֐��G���[
                    Debug.Assert(False, "��M�X���b�h �G���[����")
                    Throw New System.Exception("��M�X���b�h �G���[����")
                    m_hEvent_End.Set()	'�I���V�O�i��
                End If
            End While

            '''' ��n��
            hEvent_Read.Close()
        End Sub
    End Class
End Namespace