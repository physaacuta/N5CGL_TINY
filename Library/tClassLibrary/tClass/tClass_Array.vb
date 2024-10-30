'*********************************************************************************
'�������s�����z��v�f�̊��N���X
'	[Ver]
'		Ver.01    2005/12/23  vs2005 �Ή�
'			                  Generic �Ή� (���o�[�W�����Ƃ��Ďg�������ꍇ�́A�^��Object���w��)
'		Ver.02    2006/01/28  �o�O �Ή�
'		Ver.03    2006/01/28  vs2003 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On                                ' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Namespace tClass
    '///////////////////////////////////////////////////////////////////////////////
    '[Generic�Ή�]
    '///////////////////////////////////////////////////////////////////////////////
#If VBC_VER >= 8.0 Then                         ' Generic �ɑΉ����Ă���o�[�W������
    ''' <summary>
    ''' �������s�����z��v�f�̊��N���X [Generic�Ή�]
    ''' </summary>
    ''' <typeparam name="T">[Generic] �v�f�̌^</typeparam>
    ''' <typeparam name="KEY">[Generic] �����L�[�̌^</typeparam>
    ''' <remarks>[Generic] �C���X�^���X�𐶐�����ꍇ�� (Of String) ���w�肷��</remarks>
    <System.Serializable()> _
    Public Class tClass_Array(Of T, KEY)				' Generic�ɑΉ�
        Implements IDisposable							' �f�X�g���N�^��`
        Implements IEnumerable							' For each ���o�����`

        '********************************************************************
        '���[�J���ϐ���`
        '********************************************************************
        Private disposedValue As Boolean = False		' �d������Ăяo�������o����ɂ�
        Protected Friend m_pAry As ArrayList			' �v�f���X�g
        Private m_pLockObject As Object					' SyncLock�����I�u�W�F�N�g


        '********************************************************************
        '�v���p�e�B��`
        '********************************************************************
        ''' <summary>
        ''' �w�肵���C���f�b�N�X�ɂ���v�f���擾�܂��͐ݒ�
        ''' </summary>
        ''' <param name="index">�C���f�b�N�X(0�I���W��)</param>
        ''' <value>�ݒ肷��v�f</value>
        ''' <returns>�擾�����v�f</returns>
        ''' <remarks></remarks>
        Default Public Property Item(ByVal index As Integer) As T
            Get
                SyncLock m_pLockObject
                    If index >= m_pAry.Count OrElse index < 0 Then Return Nothing
                    Return CType(m_pAry.Item(index), T)
                End SyncLock
            End Get
            Set(ByVal Value As T)
                SyncLock m_pLockObject
                    If index >= m_pAry.Count OrElse index < 0 Then Return
                    m_pAry.Item(index) = Value
                End SyncLock
            End Set
        End Property

        ''' <summary>
        ''' �v�f�̐�
        ''' </summary>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public ReadOnly Property Count() As Integer
            Get
                SyncLock m_pLockObject
                    Return m_pAry.Count
                End SyncLock
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
            m_pAry = New ArrayList
            m_pLockObject = New Object
        End Sub

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: �����I�ɌĂяo���ꂽ�Ƃ��ɃA���}�l�[�W ���\�[�X��������܂�
                End If

                ' TODO: ���L�̃A���}�l�[�W ���\�[�X��������܂�
                m_pAry.Clear()
            End If
            Me.disposedValue = True
        End Sub

#Region " IDisposable Support "
        ''' <summary>
        ''' Dispose�f�X�g���N�^
        ''' </summary>
        ''' <remarks>�K�� �L���[�ɗ��܂��Ă���v�f�� �J�����Ă���</remarks>
        Public Sub Dispose() Implements IDisposable.Dispose
            ' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region


        ''' <summary>
        ''' �v�f�����X�g�̍ŏ��ɑ}��
        ''' </summary>
        ''' <param name="gnrT">[Generic] �ǉ�����v�f</param>
        ''' <remarks>�ǉ�����v�f���C���X�^���X�̏ꍇ�A�g�p�ォ�Ȃ炸�J�����s����</remarks>
        Public Sub Prepend(ByVal gnrT As T)
            SyncLock m_pLockObject
                m_pAry.Insert(0, gnrT)
            End SyncLock
        End Sub

        ''' <summary>
        ''' �v�f�����X�g�̍Ō�ɒǉ�
        ''' </summary>
        ''' <param name="gnrT">[Generic] �ǉ�����v�f</param>
        ''' <returns>�ǉ����ꂽ�v�f�̈ʒu</returns>
        ''' <remarks>�ǉ�����v�f���C���X�^���X�̏ꍇ�A�g�p�ォ�Ȃ炸�J�����s����</remarks>
        Public Function Add(ByVal gnrT As T) As Integer
            SyncLock m_pLockObject
                Return m_pAry.Add(gnrT)
            End SyncLock
        End Function

        ''' <summary>
        ''' �v�f���w��ʒu�ɒǉ�
        ''' </summary>
        ''' <param name="index">�ʒu (0�I���W��) [0�`Count�̊�]</param>
        ''' <param name="gnrT">[Generic] �ǉ�����v�f</param>
        ''' <returns>���A���</returns>
        ''' <remarks>�ǉ�����v�f���C���X�^���X�̏ꍇ�A�g�p�ォ�Ȃ炸�J�����s����</remarks>
        Public Function Insert(ByVal index As Integer, ByVal gnrT As T) As Boolean
            SyncLock m_pLockObject
                If index > m_pAry.Count OrElse index < 0 Then Return False
                m_pAry.Insert(index, gnrT)
                Return True
            End SyncLock
        End Function

        ''' <summary>
        ''' �ŏ��̗v�f�����o��
        ''' </summary>
        ''' <returns>���o�����v�f (�ُ펞 Nothing)</returns>
        ''' <remarks>�擾�v�f�́A���X�g����폜�����</remarks>
        Public Function GetHead() As T
            Return Me.GetItem(0)
        End Function

        ''' <summary>
        ''' �Ō�̗v�f�����o��
        ''' </summary>
        ''' <returns>���o�����v�f (�ُ펞 Nothing)</returns>
        ''' <remarks>�擾�v�f�́A���X�g����폜�����</remarks>
        Public Function GetTail() As T
            Return Me.GetItem(m_pAry.Count - 1)
        End Function

        ''' <summary>
        ''' �w��ʒu�̗v�f�����o��
        ''' </summary>
        ''' <param name="index">�ʒu (0�I���W��) [0�`Count-1�̊�]</param>
        ''' <returns>���o�����v�f (�ُ펞 Nothing)</returns>
        ''' <remarks>�擾�v�f�́A���X�g����폜�����</remarks>
        Public Function GetItem(ByVal index As Integer) As T
            SyncLock m_pLockObject
                Dim bln As Boolean = True
                If index >= m_pAry.Count OrElse index < 0 Then Return Nothing
                Try
                    Return Me.Item(index)
                Catch ex As Exception
                    bln = False
                    Return Nothing
                Finally
                    If bln Then m_pAry.RemoveAt(index)
                End Try
            End SyncLock
        End Function

        ''' <summary>
        ''' �w��ʒu�̗v�f���폜����
        ''' </summary>
        ''' <param name="gnrT">[Generic] �폜����v�f</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function Remove(ByVal gnrT As T) As Boolean
            SyncLock m_pLockObject
                m_pAry.Remove(gnrT)
                Return True
            End SyncLock
        End Function

        ''' <summary>
        ''' �w��ʒu�̗v�f���폜����
        ''' </summary>
        ''' <param name="index">�폜����ʒu (0�I���W��) [0�`Count-1�̊�]</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function RemoveAt(ByVal index As Integer) As Boolean
            SyncLock m_pLockObject
                If index >= m_pAry.Count OrElse index < 0 Then Return False
                m_pAry.RemoveAt(index)
                Return True
            End SyncLock
        End Function

        ''' <summary>
        ''' �S�v�f���N���A����
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub Clear()
            SyncLock m_pLockObject
                m_pAry.Clear()
            End SyncLock
        End Sub

        ''' <summary>
        ''' �����q���擾����C���^�[�t�F�[�X
        ''' </summary>
        ''' <returns>�����q</returns>
        ''' <remarks>For each���R�[�������ꍇ�����I�ɂ��̃R�[�h����擾�����</remarks>
        Private Function GetEnumerator() As System.Collections.IEnumerator Implements System.Collections.IEnumerable.GetEnumerator
            Return m_pAry.GetEnumerator
        End Function

        '********************************************************************
        '�C���^�[�t�F�[�X �֐��R�[��
        '********************************************************************
        ''' <summary>
        ''' �v�f����w��L�[�Ō���
        ''' </summary>
        ''' <param name="gnrKey">�����L�[</param>
        ''' <param name="findMethod">���������������������\�b�h(find���\�b�h�̎����֐�(�ʏ� Implements ���� �N���X����Ăԏꍇ Me ��OK))</param>
        ''' <returns>�������� ������:nothing �L�莞�F�Y���N���X</returns>
        ''' <remarks>Implements tIface_Find ���`���鎖</remarks>
        Public Overridable Overloads Function Find(ByVal gnrKey As KEY, ByVal findMethod As tIface.tIface_Find(Of KEY)) As T
            Dim index As Integer

            SyncLock m_pLockObject
                Try
                    '''' �������b�\�h �R�[��
                    index = findMethod.Find(gnrKey, m_pAry)
                    If index > m_pAry.Count OrElse index < 0 Then Return Nothing

                    '''' ���ʂ̔z��ʒu����I�u�W�F�N�g���o��
                    Return Me.Item(index)
                Catch ex As Exception
                    Return Nothing
                End Try
            End SyncLock
        End Function

        '''' <summary>
        '''' Find�̃C���^�[�t�F�[�X
        '''' </summary>
        '''' <remarks>tClass_Array �̌Ăь��N���X�� Implements tIface_Find ���`���鎖</remarks>
        'Public Interface tIface_Find
        '    ''' <summary>
        '    ''' �v�f���X�g����w��L�[�Ō������s���A�������ʂ̔z��ʒu��߂� ��������������
        '    ''' </summary>
        '    ''' <param name="gnrKey">�����L�[</param>
        '    ''' <param name="aryList">�v�f���X�g</param>
        '    ''' <returns>�������ʂ�Ԃ� 0�I���W���̔z��ʒu -1:�Y������</returns>
        '    ''' <remarks></remarks>
        '    Function Find(ByVal gnrKey As KEY, ByVal aryList As ArrayList) As Integer

        'End Interface
    End Class
#End If

    '///////////////////////////////////////////////////////////////////////////////
    '[Generic��Ή�]
    '///////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �������s�����z��v�f�̊��N���X
    ''' </summary>
    ''' <remarks></remarks>
    <System.Serializable()> _
    Public Class tClass_Array
        Implements IDisposable                          ' �f�X�g���N�^��`
        Implements IEnumerable                          ' For each ���o�����`

        '********************************************************************
        '���[�J���ϐ���`
        '********************************************************************
        Private disposedValue As Boolean = False        ' �d������Ăяo�������o����ɂ�
        Protected Friend m_pAry As ArrayList            ' �v�f���X�g
        Private m_pLockObject As Object                 ' SyncLock�����I�u�W�F�N�g


        '********************************************************************
        '�v���p�e�B��`
        '********************************************************************
        ''' <summary>
        ''' �w�肵���C���f�b�N�X�ɂ���v�f���擾�܂��͐ݒ�
        ''' </summary>
        ''' <param name="index">�C���f�b�N�X(0�I���W��)</param>
        ''' <value>�ݒ肷��v�f</value>
        ''' <returns>�擾�����v�f</returns>
        ''' <remarks></remarks>
        Default Public Property Item(ByVal index As Integer) As Object
            Get
                SyncLock m_pLockObject
                    If index >= m_pAry.Count OrElse index < 0 Then Return Nothing
                    Return m_pAry.Item(index)
                End SyncLock
            End Get
            Set(ByVal Value As Object)
                SyncLock m_pLockObject
                    If index >= m_pAry.Count OrElse index < 0 Then Return
                    m_pAry.Item(index) = Value
                End SyncLock
            End Set
        End Property

        ''' <summary>
        ''' �v�f�̐�
        ''' </summary>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public ReadOnly Property Count() As Integer
            Get
                SyncLock m_pLockObject
                    Return m_pAry.Count
                End SyncLock
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
            m_pAry = New ArrayList
            m_pLockObject = New Object
        End Sub

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: �����I�ɌĂяo���ꂽ�Ƃ��ɃA���}�l�[�W ���\�[�X��������܂�
                End If

                ' TODO: ���L�̃A���}�l�[�W ���\�[�X��������܂�
                m_pAry.Clear()
            End If
            Me.disposedValue = True
        End Sub

#Region " IDisposable Support "
        ''' <summary>
        ''' Dispose�f�X�g���N�^
        ''' </summary>
        ''' <remarks>�K�� �L���[�ɗ��܂��Ă���v�f�� �J�����Ă���</remarks>
        Public Sub Dispose() Implements IDisposable.Dispose
            ' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region

        '********************************************************************
        '[Generic�Ή�]
        '********************************************************************
        ''' <summary>
        ''' �v�f�����X�g�̍ŏ��ɑ}��
        ''' </summary>
        ''' <param name="gnrT">[Object] �ǉ�����v�f</param>
        ''' <remarks>�ǉ�����v�f���C���X�^���X�̏ꍇ�A�g�p�ォ�Ȃ炸�J�����s����</remarks>
        Public Sub Prepend(ByVal gnrT As Object)
            SyncLock m_pLockObject
                m_pAry.Insert(0, gnrT)
            End SyncLock
        End Sub

        ''' <summary>
        ''' �v�f�����X�g�̍Ō�ɒǉ�
        ''' </summary>
        ''' <param name="gnrT">[Object] �ǉ�����v�f</param>
        ''' <returns>�ǉ����ꂽ�v�f�̈ʒu</returns>
        ''' <remarks>�ǉ�����v�f���C���X�^���X�̏ꍇ�A�g�p�ォ�Ȃ炸�J�����s����</remarks>
        Public Function Add(ByVal gnrT As Object) As Integer
            SyncLock m_pLockObject
                Return m_pAry.Add(gnrT)
            End SyncLock
        End Function

        ''' <summary>
        ''' �v�f���w��ʒu�ɒǉ�
        ''' </summary>
        ''' <param name="index">�ʒu (0�I���W��) [0�`Count�̊�]</param>
        ''' <param name="gnrT">[Object] �ǉ�����v�f</param>
        ''' <returns>���A���</returns>
        ''' <remarks>�ǉ�����v�f���C���X�^���X�̏ꍇ�A�g�p�ォ�Ȃ炸�J�����s����</remarks>
        Public Function Insert(ByVal index As Integer, ByVal gnrT As Object) As Boolean
            SyncLock m_pLockObject
                If index > m_pAry.Count OrElse index < 0 Then Return False
                m_pAry.Insert(index, gnrT)
                Return True
            End SyncLock
        End Function

        ''' <summary>
        ''' �ŏ��̗v�f�����o��
        ''' </summary>
        ''' <returns>���o�����v�f (�ُ펞 Nothing)</returns>
        ''' <remarks>�擾�v�f�́A���X�g����폜�����</remarks>
        Public Function GetHead() As Object
            Return Me.GetItem(0)
        End Function

        ''' <summary>
        ''' �Ō�̗v�f�����o��
        ''' </summary>
        ''' <returns>���o�����v�f (�ُ펞 Nothing)</returns>
        ''' <remarks>�擾�v�f�́A���X�g����폜�����</remarks>
        Public Function GetTail() As Object
            Return Me.GetItem(m_pAry.Count - 1)
        End Function

        ''' <summary>
        ''' �w��ʒu�̗v�f�����o��
        ''' </summary>
        ''' <param name="index">�ʒu (0�I���W��) [0�`Count-1�̊�]</param>
        ''' <returns>���o�����v�f (�ُ펞 Nothing)</returns>
        ''' <remarks>�擾�v�f�́A���X�g����폜�����</remarks>
        Public Function GetItem(ByVal index As Integer) As Object
            SyncLock m_pLockObject
                Dim bln As Boolean = True
                If index >= m_pAry.Count OrElse index < 0 Then Return Nothing
                Try
                    Return Me.Item(index)
                Catch ex As Exception
                    bln = False
                    Return Nothing
                Finally
                    If bln Then m_pAry.RemoveAt(index)
                End Try
            End SyncLock
        End Function

        ''' <summary>
        ''' �w��ʒu�̗v�f���폜����
        ''' </summary>
        ''' <param name="gnrT">[Object] �폜����v�f</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function Remove(ByVal gnrT As Object) As Boolean
            SyncLock m_pLockObject
                m_pAry.Remove(gnrT)
                Return True
            End SyncLock
        End Function

        ''' <summary>
        ''' �w��ʒu�̗v�f���폜����
        ''' </summary>
        ''' <param name="index">�폜����ʒu (0�I���W��) [0�`Count-1�̊�]</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function RemoveAt(ByVal index As Integer) As Boolean
            SyncLock m_pLockObject
                If index >= m_pAry.Count OrElse index < 0 Then Return False
                m_pAry.RemoveAt(index)
                Return True
            End SyncLock
        End Function

        ''' <summary>
        ''' �S�v�f���N���A����
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub Clear()
            SyncLock m_pLockObject
                m_pAry.Clear()
            End SyncLock
        End Sub

        ''' <summary>
        ''' �����q���擾����C���^�[�t�F�[�X
        ''' </summary>
        ''' <returns>�����q</returns>
        ''' <remarks>For each���R�[�������ꍇ�����I�ɂ��̃R�[�h����擾�����</remarks>
        Private Function GetEnumerator() As System.Collections.IEnumerator Implements System.Collections.IEnumerable.GetEnumerator
            Return m_pAry.GetEnumerator
        End Function

        '********************************************************************
        '�C���^�[�t�F�[�X �֐��R�[��
        '********************************************************************
        ''' <summary>
        ''' �v�f����w��L�[�Ō���
        ''' </summary>
        ''' <param name="gnrKey">�����L�[</param>
        ''' <param name="findMethod">���������������������\�b�h(find���\�b�h�̎����֐�(�ʏ� Implements ���� �N���X����Ăԏꍇ Me ��OK))</param>
        ''' <returns>�������� ������:nothing �L�莞�F�Y���N���X</returns>
        ''' <remarks>Implements tIface_Find ���`���鎖</remarks>
        Public Overridable Overloads Function Find(ByVal gnrKey As Object, ByVal findMethod As tIface.tIface_Find) As Object
            Dim index As Integer

            SyncLock m_pLockObject
                Try
                    '''' �������b�\�h �R�[��
                    index = findMethod.Find(gnrKey, m_pAry)
                    If index > m_pAry.Count OrElse index < 0 Then Return Nothing

                    '''' ���ʂ̔z��ʒu����I�u�W�F�N�g���o��
                    Return Me.Item(index)
                Catch ex As Exception
                    Return Nothing
                End Try
            End SyncLock
        End Function
    End Class

End Namespace