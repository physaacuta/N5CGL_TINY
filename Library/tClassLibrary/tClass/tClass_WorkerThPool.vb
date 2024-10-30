'*********************************************************************************
'Thread�̓�����s����{�N���X
'*********************************************************************************
Option Strict On                'tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.Threading


Namespace tClass
    Public Class tClass_WorkerThPool
        Implements IDisposable          '�f�X�g���N�^��`

        '********************************************************************
        '�����o�[�ϐ���`
        '********************************************************************
        Private Shared m_ary As ArrayList

        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        '----------------------------------------------------------------
        ' �T�v�F�R���X�g���N�^
        ' �����F(in :)  = 
        '----------------------------------------------------------------
        Shared Sub New()
            m_ary = New ArrayList
        End Sub

        '----------------------------------------------------------------
        ' �T�v�FDispose�f�X�g���N�^
        ' �����F(in :)  = 
        ' �ߒl�F() 
        ' ���l�F
        '----------------------------------------------------------------
        Public Sub Dispose() Implements System.IDisposable.Dispose
            Try
                '' �S���폜
                ResetEvent()
            Catch ex As Exception
            End Try
        End Sub

        '----------------------------------------------------------------
        ' �T�v�F�t�@�C�i���C�Y
        ' �����F(in :)  = 
        '----------------------------------------------------------------
        Protected Overrides Sub Finalize()
            Dispose()
            MyBase.Finalize()
        End Sub


        '----------------------------------------------------------------
        ' �T�v�F�C�x���g�^�C�}�[�Z�b�g
        ' �����F(in :Integer) key = ��ӂȃL�[
        ' �����F(in :Object) returnSender = �f���Q�[�g�̖߂�l
        ' �����F(in :Object) returnE = �f���Q�[�g�̖߂�l
        ' �����F(in :Integer) timeOut = �^�C���A�E�g�l [ms]
        ' �����F(in :Delegate) callBack = �f���Q�[�g�C���X�^���X (New tClass.tClass_232C.tDele_232C_Event(AddressOf subDele_232C_Event))����Ȋ���
        ' �ߒl�F�Ȃ�
        ' ���l�F
        '----------------------------------------------------------------
        Public Shared Sub SetEvent(ByVal key As Integer, ByVal returnSender As Object, ByVal returnE As Object, ByVal timeOut As Integer, ByVal callBack As [Delegate])
            Dim inf As New tClass.tClass_WorkerThPool.TaskInfo(key, callBack, returnSender, returnE)      '�󂯓n���p�N���X����
            inf._ev = New AutoResetEvent(False)                                                         '�V�O�i������
            '�X���b�h�v�[���ɓo�^
            inf._handle = ThreadPool.RegisterWaitForSingleObject(inf._ev, _
                            New WaitOrTimerCallback(AddressOf subCallBack), _
                            inf, timeOut, True)
SyncLock m_ary
            m_ary.Add(inf)
End SyncLock
        End Sub

        '----------------------------------------------------------------
        ' �T�v�F�C�x���g�^�C�}�[�L�����Z��
        ' �����F(in :) 
        ' �ߒl�F�Ȃ�
        ' ���l�F�S�A�C�e���폜
        '----------------------------------------------------------------
        Public Overloads Shared Sub ResetEvent()
SyncLock m_ary
            Dim inf As TaskInfo
            '' �S�A�C�e���폜
            For Each inf In m_ary
                If Not inf._ev Is Nothing Then
                    '������
                    inf._handle.Unregister(Nothing)
                    inf._handle = Nothing
                    inf._ev.Close()
                    inf._ev = Nothing
                    inf._callBack = Nothing
                    inf._key = Nothing
                    inf._sender = Nothing
                    inf._e = Nothing
                    inf = Nothing
                End If
            Next
            '' ���X�g�S�폜
            m_ary.Clear()
End SyncLock
        End Sub

        '----------------------------------------------------------------
        ' �T�v�F�C�x���g�^�C�}�[�L�����Z��
        ' �����F(in :Integer) key = ��ӂȃL�[
        ' �ߒl�F(Boolean) True:�L�����Z������ False:�Y������(���s�ς݁H)
        ' ���l�F
        '----------------------------------------------------------------
        Public Overloads Shared Function ResetEvent(ByVal key As Integer) As Boolean
SyncLock m_ary
            Dim inf As TaskInfo
            '' �w��L�[�̂ݍ폜
            For Each inf In m_ary
                If inf._key = key Then
                    If inf._ev Is Nothing Then Return False
                    m_ary.Remove(inf)
                    '������
                    inf._handle.Unregister(Nothing)
                    inf._handle = Nothing
                    inf._ev.Close()
                    inf._ev = Nothing
                    inf._callBack = Nothing
                    inf._key = Nothing
                    inf._sender = Nothing
                    inf._e = Nothing
                    inf = Nothing
                    Return True
                End If
            Next inf
End SyncLock
            Return False
        End Function

        '�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
        ' �T�v�F�R�[���o�b�N
        '�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
        Private Shared Sub subCallBack(ByVal state As Object, ByVal timedOut As Boolean)
            Dim inf As TaskInfo
SyncLock m_ary
            inf = CType(state, TaskInfo)
            If inf._ev Is Nothing Then Exit Sub
            m_ary.Remove(inf)
End SyncLock
            '' �w�肳�ꂽ�f���Q�[�g�֒ʒm
            inf._callBack.DynamicInvoke(New Object() {inf._sender, inf._e})
            '������
            inf._handle.Unregister(Nothing)
            inf._handle = Nothing
            inf._ev.Close()
            inf._ev = Nothing
            inf._callBack = Nothing
            inf._key = Nothing
            inf._sender = Nothing
            inf._e = Nothing
            inf = Nothing
        End Sub

        '********************************************************************
        ' �󂯓n���N���X
        '********************************************************************
        Private Class TaskInfo
            Public _handle As RegisteredWaitHandle = Nothing    '�X���b�h�n���h��
            Public _ev As AutoResetEvent
            Public _key As Integer              '��ӂȃL�[���
            Public _callBack As [Delegate]      '�Ăяo����

            ''' �󂯓n���f�[�^
            Public _sender As Object
            Public _e As Object

            ''' �R���X�g���N�^
            Public Sub New(ByVal key As Integer, ByVal callBack As [Delegate], ByVal state As Object, ByVal e As Object)
                _key = key
                _callBack = callBack
                _sender = state
                _e = e
            End Sub
        End Class

    End Class
End Namespace