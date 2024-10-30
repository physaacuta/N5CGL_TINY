'*********************************************************************************
'�������s����Queue�̊��N���X
'	[Ver]
'		Ver.01    2006/03/04  vs2005 �Ή�
'
'	[����]
'		�Z�}�t�H�[���g����Queue�N���X (vs2005��p)
'*********************************************************************************

Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

#If VBC_VER >= 8.0 Then
Namespace tClass
    ''' <summary>
    ''' �������s����Queue�̊��N���X [Generic�Ή�]
    ''' </summary>
    ''' <typeparam name="T">[Generic] �v�f�̌^ (�C���X�^���X�𐶐�����ꍇ�� (Of String) ���w�肷��)</typeparam>
    ''' <remarks>�L���[�ɗ��܂��Ă���� g_hEvent �� �V�O�i���ƂȂ��Ă���</remarks>
    <System.Serializable()> _
    Public Class tClass_Queue(Of T)						' Generic�ɑΉ�
        Implements IDisposable							' �f�X�g���N�^��`


        '********************************************************************
        '���J�ϐ� ��`
        '********************************************************************
        ''' <summary>
        ''' �Z�}�t�H�[
        ''' </summary>
        ''' <remarks>�L���[���L��:�V�O�i�� �L���[����:��V�O�i��</remarks>
        Public g_evSem As System.Threading.Semaphore		' �Z�}�t�H�[


        '********************************************************************
        '���[�J���ϐ���`
        '********************************************************************
        Private disposedValue As Boolean = False			' �d������Ăяo�������o����ɂ�
        Protected Friend my_pQue As Queue(Of T)				' �v�f���X�g

        Protected Friend my_cs As Object					' SyncLock�����I�u�W�F�N�g
        Protected Friend my_nSemCount As Integer			' �Z�}�t�H�T�C�Y
        Protected Friend my_nMaxCount As Integer			' �u�ԍő�o�^����


        '********************************************************************
        '�v���p�e�B
        '********************************************************************
        ''' <summary>
        ''' ���݂̓o�^����
        ''' </summary>
        ''' <returns>���݂̓o�^����</returns>
        Public ReadOnly Property GetCount() As Integer
            Get
                Return my_pQue.Count
            End Get
        End Property

        ''' <summary>
        ''' �ő�o�^�\��
        ''' </summary>
        ''' <returns>�ő�o�^�\��</returns>
        Public ReadOnly Property GetSemCount() As Integer
            Get
                Return my_nSemCount
            End Get
        End Property

        ''' <summary>
        ''' ���܂ł̍ő�o�^����
        ''' </summary>
        ''' <returns>���܂ł̍ő�o�^����</returns>
        Public ReadOnly Property GetMaxCount() As Integer
            Get
                Return my_nMaxCount
            End Get
        End Property
        ''' <summary>
        ''' �u�ԍő�o�^����������
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ReSetMaxCount()
            my_nMaxCount = 0
        End Sub
        ''' <summary>
        ''' �c�o�^����
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public ReadOnly Property GetFreeCount() As Integer
            Get
                Return GetSemCount() - GetCount()
            End Get
        End Property

        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <param name="nSemCnt">�ő�o�^�\��</param>
        ''' <remarks></remarks>
        Public Sub New(Optional ByVal nSemCnt As Integer = 256)
            '' ������
            my_nSemCount = nSemCnt
            my_nMaxCount = 0

            '' �C���X�^���X����
            my_pQue = New Queue(Of T)
            my_cs = New Object
            g_evSem = New Threading.Semaphore(0, my_nSemCount)
        End Sub

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: �����I�ɌĂяo���ꂽ�Ƃ��ɃA���}�l�[�W ���\�[�X��������܂�
                End If

                ' TODO: ���L�̃A���}�l�[�W ���\�[�X��������܂�
                g_evSem.Close()
                g_evSem = Nothing
                my_pQue.Clear()
                my_pQue = Nothing
                my_cs = Nothing
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
        ''' �L���[�ɗv�f��ǉ�
        ''' </summary>
        ''' <param name="gnrT">[Generic] �ǉ�����v�f</param>
        ''' <returns>���A���</returns>
        ''' <remarks>�ǉ�����v�f���C���X�^���X�̏ꍇ�A�g�p�ォ�Ȃ炸�J�����s����</remarks>
        Public Overloads Function SetItem(ByVal gnrT As T) As Boolean
            SyncLock my_cs
                If my_pQue.Count() >= my_nSemCount Then Return False

                my_pQue.Enqueue(gnrT)		'�L���[�Ƀf�[�^�ǉ�
                If my_pQue.Count() >= my_nMaxCount Then my_nMaxCount = my_pQue.Count() '�ő�o�^�����X�V
                g_evSem.Release()			'�Z�}�t�H�[�o�^
                Return True
            End SyncLock
        End Function

        ''' <summary>
        ''' �L���[�ɗv�f��ǉ� (�o�^�����܂Ńu���b�N)
        ''' </summary>
        ''' <param name="gnrT">[Generic] �ǉ�����v�f</param>
        ''' <param name="retryCnt">�҂���</param>
        ''' <param name="waitTime">�҂�����</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Function SetItem(ByVal gnrT As T, ByVal retryCnt As Integer, Optional ByVal waitTime As Integer = 1) As Boolean
            '' �o�^�����܂ŌJ��Ԃ�
            For ii As Integer = 0 To retryCnt
                If setItem(gnrT) Then
                    Return True
                Else
                    '' �o�^�҂�
                    Threading.Thread.Sleep(waitTime)
                End If
            Next ii
            Return False
        End Function

        ''' <summary>
        ''' �L���[����v�f���擾(�Z�}�t�H�[�͌���Ȃ�)
        ''' </summary>
        ''' <returns>�v�f</returns>
        ''' <remarks>�C���X�^���X�̏ꍇ�͎g�p��,�K���J�����s����</remarks>
        Public Overloads Function GetItem() As T
            SyncLock my_cs
                If my_pQue.Count() <= 0 Then
                    Return Nothing
                End If

                Return my_pQue.Dequeue()
            End SyncLock
        End Function

        ''' <summary>
        ''' �L���[����v�f���擾 �҂��o�[�W���� (�Z�}�t�H�[����)
        ''' </summary>
        ''' <param name="waitTime">�҂�����</param>
        ''' <returns>�v�f</returns>
        ''' <remarks></remarks>
        Public Overloads Function GetItem(ByVal waitTime As Integer) As T
            '' �L���[�����o�����܂őҋ@
            If Not g_evSem.WaitOne(waitTime, False) Then
                Return Nothing
            End If
            '' �L���[���o��
            Return GetItem()
        End Function

        ''' <summary>
        ''' �L���[����v�f��S���擾
        ''' </summary>
        ''' <returns>�v�f�z��</returns>
        ''' <remarks></remarks>
        Public Function GetAll() As T()
            SyncLock my_cs
                Dim cnt As Integer = my_pQue.Count
                If cnt <= 0 Then
                    Return Nothing
                End If

                ' �Z�}�t�H�[�S����
                Try
                    For ii As Integer = 0 To cnt - 1
                        g_evSem.WaitOne(0, False)
                    Next ii
                Catch ex As Exception
                End Try

                '' �L���[���f�[�^��S�Ď擾
                Dim gnrAryT() As T = my_pQue.ToArray
                my_pQue.Clear()
                Return gnrAryT
            End SyncLock
        End Function


        ''' <summary>
        ''' �L���[����S�v�f���J�� (New���ăZ�b�g���Ă���ꍇ��,�������[���[�N�����Ⴄ�̂Œ��ӁH)
        ''' </summary>
        ''' <remarks>C++�Ɠ��삪�Ⴄ�̂Œ���</remarks>
        Public Sub Free()
            SyncLock my_cs
                Dim cnt As Integer = GetCount
                If cnt <= 0 Then
                    Return
                End If

                ' �Z�}�t�H�[�S����
                Try
                    For ii As Integer = 0 To cnt - 1
                        g_evSem.WaitOne(0, False)
                    Next ii
                Catch ex As Exception
                End Try
                my_pQue.Clear()
            End SyncLock
        End Sub

    End Class
End Namespace
#End If
