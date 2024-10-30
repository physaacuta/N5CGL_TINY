'*********************************************************************************
'Thread����̊Ǘ����s�����N���X
'	[Ver]
'		Ver.01    2006/03/04  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.Threading						' �X���b�h

Namespace tClass
    ''' <summary>
    ''' Thread����̊Ǘ����s�����N���X
    ''' </summary>
    ''' <remarks>���[�J�[�X���b�h�N���X�𐶐�����ꍇ�́A�K���{�N���X���p�����邩�����o�[�ɉ����鎖</remarks>
    Public Class tClass_WorkThread
        Implements IDisposable							' �f�X�g���N�^��`


        '********************************************************************
        '�O���[�o���ϐ���`
        '********************************************************************
        Public g_evStop As AutoResetEvent				' �X���b�h�I���V�O�i��

        '********************************************************************
        '���[�J���ϐ���`
        '********************************************************************
        Private disposedValue As Boolean = False		' �d������Ăяo�������o����ɂ�
        Protected Friend my_pThread As Thread			' ���C���X���b�h�C���X�^���X

        '********************************************************************
        '�v���p�e�B
        '********************************************************************
        ''' <summary>
        ''' �X���b�hID (�ǂݎ���p)
        ''' </summary>
        ''' <returns>�X���b�hID</returns>
        Public ReadOnly Property GetTid() As Integer
            Get
                If my_pThread Is Nothing Then
                    Return -1
                Else
                    Return my_pThread.ManagedThreadId()
                End If
            End Get
        End Property

        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()
            ' �V�O�i������ (��V�O�i��)
            g_evStop = New AutoResetEvent(False)
            my_pThread = Nothing
        End Sub

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: �����I�ɌĂяo���ꂽ�Ƃ��ɃA���}�l�[�W ���\�[�X��������܂�
                End If

                ' TODO: ���L�̃A���}�l�[�W ���\�[�X��������܂�
                g_evStop.Close()
                g_evStop = Nothing
            End If
            Me.disposedValue = True
        End Sub

#Region " IDisposable Support "
        ' ���̃R�[�h�́A�j���\�ȃp�^�[���𐳂��������ł���悤�� Visual Basic �ɂ���Ēǉ�����܂����B
        Public Sub Dispose() Implements IDisposable.Dispose
            ' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region

        ''' <summary>
        ''' �X���b�h���s
        ''' </summary>
        ''' <param name="method">�X���b�h�Ŏ��s����郁�\�b�h (AddressOf�Ŏw�肷�鎖)</param>
        ''' <param name="priority">�X���b�h�̗D��x</param>
        ''' <param name="strTreadName">�J�����g�X���b�h����</param>
        ''' <param name="background">�X���b�h�̎��s�`�� (True:BackGround(thred�����ʂƈꏏ�Ɏ���) False:�ʏ�)</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function ThreadStart(ByVal method As Threading.ThreadStart, Optional ByVal priority As Threading.ThreadPriority = ThreadPriority.Normal, Optional ByVal strTreadName As String = "", Optional ByVal background As Boolean = False) As Boolean
            '' �X���b�h�`�F�b�N
            If Not my_pThread Is Nothing Then Return False

            '' ���s�X���b�h��`
            my_pThread = New System.Threading.Thread(method)
            If "" <> strTreadName Then my_pThread.Name = strTreadName
            my_pThread.IsBackground = background						   'BackGround�ɂ���Ɛ����X���b�h�����ʂƓ��X���b�h������ł�����
            my_pThread.Priority = priority

            '' ���s
            my_pThread.Start()
            Return True
        End Function

#If VBC_VER >= 8.0 Then
        ''' <summary>
        ''' �X���b�h���s
        ''' </summary>
        ''' <param name="method">�X���b�h�Ŏ��s����郁�\�b�h (AddressOf�Ŏw�肷�鎖)</param>
        ''' <param name="objData">���s�X���b�h���\�b�h�ɓn��������</param>
        ''' <param name="priority">�X���b�h�̗D��x</param>
        ''' <param name="strTreadName">�J�����g�X���b�h����</param>
        ''' <param name="background">�X���b�h�̎��s�`�� (True:BackGround(thred�����ʂƈꏏ�Ɏ���) False:�ʏ�)</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function ThreadStart(ByVal method As Threading.ParameterizedThreadStart, ByVal objData As Object, Optional ByVal priority As Threading.ThreadPriority = ThreadPriority.Normal, Optional ByVal strTreadName As String = "", Optional ByVal background As Boolean = False) As Boolean
            '' �X���b�h�`�F�b�N
            If Not my_pThread Is Nothing Then Return False

            '' ���s�X���b�h��`
            my_pThread = New System.Threading.Thread(method)
            If "" <> strTreadName Then my_pThread.Name = strTreadName
            my_pThread.IsBackground = background						   'BackGround�ɂ���Ɛ����X���b�h�����ʂƓ��X���b�h������ł�����
            my_pThread.Priority = priority

            '' ���s
            my_pThread.Start(objData)
            Return True
        End Function
#End If

        ''' <summary>
        ''' �X���b�h�I��
        ''' </summary>
        ''' <param name="iInterval">�I���҂����� [ms]</param>
        ''' <returns>���A���</returns>
        ''' <remarks>��O�����Ƃ���[ThreadAbortException]���������鎖</remarks>
        Public Function ThreadStop(Optional ByVal iInterval As Integer = 3000) As Boolean
            '' �X���b�h�`�F�b�N
            If my_pThread Is Nothing Then Return False

            '' �X���b�h��~�V�O�i�����s
            g_evStop.Set()

            '' �X���b�h����~��ԂɂȂ�܂Ńu���b�N
            Try
                If Not my_pThread.Join(iInterval) Then
                    '�X���b�h�����I��
                    my_pThread.Abort()
                    my_pThread = Nothing
                    '����ς�_�����������
                    Return False
                End If
            Catch ex As Exception
            End Try

            '' �X���b�h�I��
            If my_pThread.IsAlive Then
                my_pThread.Abort()
            End If
            my_pThread = Nothing
            Return True
        End Function


    End Class
End Namespace
