'*********************************************************************************
'���L������ �̃A�N�Z�X���s����{�N���X
'	[Ver]
'		Ver.01    2005/12/30  vs2005 �Ή�
'
'	[����]
'		�r���ł́A��������_���H
'
'		�\���̂��g�p����ꍇ�́A�ȉ��̃T���v���̗l�ɍ\���̂��w�肷��
'
    ' '' �\���̃T���v��
    '<StructLayout(LayoutKind.Sequential, Size:=1200, pack:=1)> _
    'Structure SAMPLE
    '	<VBFixedArray(99), MarshalAs(UnmanagedType.ByValArray, SizeConst:=100)> _
    '	Dim dataL() As Integer

    '	<VBFixedString(400), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=400)> _
    '	Dim dataS As String

    '	<VBFixedArray(399), MarshalAs(UnmanagedType.ByValArray, SizeConst:=400)> _
    '	Dim dataB() As Byte

    '	Public Sub Inisialize()
    '		ReDim dataL(99)
    '		ReDim dataB(399)
    '	End Sub
    'End Structure
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.Runtime.InteropServices				'MarshalAs�N���X���X
Imports Microsoft.Win32.SafeHandles

Namespace tClass

    Public Class tClass_GlovalMemory

        '********************************************************************
        'API��`
        '********************************************************************
        Private Const PAGE_READWRITE As Integer = &H4
        Private Const STANDARD_RIGHTS_REQUIRED As Integer = &HF0000
        Private Const SECTION_QUERY As Integer = &H1
        Private Const SECTION_MAP_WRITE As Integer = &H2
        Private Const SECTION_MAP_READ As Integer = &H4
        Private Const SECTION_MAP_EXECUTE As Integer = &H8
        Private Const SECTION_EXTEND_SIZE As Integer = &H10
        Private Const SECTION_ALL_ACCESS As Integer = STANDARD_RIGHTS_REQUIRED Or _
                  SECTION_QUERY Or _
                  SECTION_MAP_WRITE Or _
                  SECTION_MAP_READ Or _
                  SECTION_MAP_EXECUTE Or _
                  SECTION_EXTEND_SIZE
        Private Const FILE_MAP_ALL_ACCESS As Integer = SECTION_ALL_ACCESS '' �ʏ�͂�����g�p����B


        '' �t�@�C���}�b�s���O�I�u�W�F�N�g���J��
        <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi, ExactSpelling:=False, CharSet:=CharSet.Unicode)> _
        Private Shared Function OpenFileMapping( _
           ByVal dwDesiredAccess As Integer, _
           ByVal bInheritHandle As Boolean, _
           ByVal lpName As String) _
           As SafeFileHandle
        End Function

        '' �t�@�C���}�b�s���O�I�u�W�F�N�g���쐬
        <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi, ExactSpelling:=False, CharSet:=CharSet.Unicode)> _
        Private Shared Function CreateFileMapping( _
           ByVal hFile As IntPtr, _
           ByVal lpAttributes As IntPtr, _
           ByVal flProtect As Integer, _
           ByVal dwMaximumSizeHigh As Integer, _
           ByVal dwMaximumSizeLow As Integer, _
           ByVal lpName As String) _
           As SafeFileHandle
        End Function

        '' �Ăяo�����v���Z�X�̃A�h���X��ԂɁA�t�@�C���̃r���[���}�b�s���O����
        <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi)> _
        Private Shared Function MapViewOfFile( _
            ByVal hFMapp As SafeFileHandle, _
            ByVal dwDesiredAccess As Integer, _
            ByVal dwFileOffsetHigh As Integer, _
            ByVal dwFileOffsetLow As Integer, _
            ByVal dwNumberOfBytesToMap As Integer) _
            As IntPtr
        End Function

        '' �t�@�C���̃r���[���J������
        <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi)> _
        Private Shared Function UnmapViewOfFile( _
           ByVal hMapped As IntPtr) _
           As Boolean
        End Function



        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        Private Shared m_fMapBase As New Dictionary(Of String, SafeFileHandle)      '���L������ �A�h���X�B

        Private Shared m_mtxObj As New Threading.Mutex(False, "tClass_GlovalMemoryMtx")     ' �^�X�N�Ԕr������


        ''' <summary>
        ''' ���L����������
        ''' </summary>
        ''' <param name="intMapSize">���L�������T�C�Y</param>
        ''' <param name="strMapName">���L����������</param>
        ''' <returns>�n���h��</returns>
        ''' <remarks>CreateFileMapping, MapViewOfFile���g�p
        ''' 		�������A���łɋ��L����������������Ă���ꍇ�́AOpenFileMapping</remarks>
        Public Shared Function AllocMapped(ByVal intMapSize As Integer, ByVal strMapName As String) As IntPtr
            Dim INVALID_HANDLE_VALUE As New IntPtr(-1L)


            '''' �t�@�C���}�b�v�����łɍ쐬����Ă���΁AOpenFileMapping
            '' ������΁ACreateFileMapping ���s��
            Dim fMap As SafeFileHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, False, strMapName)
            If fMap.IsInvalid Then
                '' ���O�t���܂��͖��O�Ȃ��̃t�@�C���}�b�s���O�I�u�W�F�N�g���쐬
                fMap = CreateFileMapping(INVALID_HANDLE_VALUE, IntPtr.Zero, PAGE_READWRITE, 0, intMapSize, strMapName)

                m_fMapBase.Add(strMapName, fMap)
            End If


            '' �Ăяo�����v���Z�X�̃A�h���X��ԂɁA�t�@�C���̃r���[���}�b�s���O����
            Return MapViewOfFile(fMap, FILE_MAP_ALL_ACCESS, 0, 0, 0)
        End Function
        ''' <summary>
        ''' ���L������ �A�N�Z�X
        ''' </summary>
        ''' <param name="strMapName">���L����������</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function OpenMapped(ByVal strMapName As String) As IntPtr
            Dim INVALID_HANDLE_VALUE As Integer = -1


            '''' �t�@�C���}�b�v�����łɍ쐬����Ă���΁AOpenFileMapping
            ' ������Ύ��s
            Dim fMap As SafeFileHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, False, strMapName)
            If fMap.IsInvalid Then
                Return IntPtr.Zero
            End If

            '' �Ăяo�����v���Z�X�̃A�h���X��ԂɁA�t�@�C���̃r���[���}�b�s���O����
            Return MapViewOfFile(fMap, FILE_MAP_ALL_ACCESS, 0, 0, 0)
        End Function

        '''' <summary>
        '''' ���L�������J��
        '''' </summary>
        '''' <param name="hMap">���L�������n���h��</param>
        '''' <remarks></remarks>
        'Public Shared Sub FreeMapped(ByRef hMap As IntPtr)
        '	'' �n���h���`�F�b�N
        '	If hMap.Equals(IntPtr.Zero) Then Exit Sub

        '	'' ���L�������J��
        '	UnmapViewOfFile(hMap)
        '	CloseHandle(hMap.ToInt32)
        '	hMap = IntPtr.Zero
        'End Sub
        ''' <summary>
        ''' ���L�������J��
        ''' </summary>
        ''' <param name="hMap">���L�������n���h��</param>
        ''' <param name="strMapName">���L����������</param>
        ''' <remarks></remarks>
        Public Shared Sub FreeMapped(ByRef hMap As IntPtr, ByVal strMapName As String)
            '' �n���h���`�F�b�N
            If hMap.Equals(IntPtr.Zero) Then Exit Sub

            '' ���L�������J��
            UnmapViewOfFile(hMap)

            '' �{�̊J��
            Dim fWk As SafeFileHandle = Nothing
            If m_fMapBase.TryGetValue(strMapName, fWk) Then
                fWk.Close()
                m_fMapBase.Remove(strMapName)
            End If
            hMap = IntPtr.Zero
        End Sub


        ''' <summary>
        ''' ���L�������̍\���̂�������
        ''' </summary>
        ''' <param name="hMap">���L�������\���̂̃|�C���^</param>
        ''' <param name="objVal">���L�������\����</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function SetMapped(ByVal hMap As IntPtr, ByRef objVal As ValueType) As Boolean
            Try
                m_mtxObj.WaitOne()

                '' �\���̃f�[�^�����L�������ɃZ�b�g
                'System.Runtime.InteropServices.Marshal.StructureToPtr(objVal, hMap, True)
                'Marshal.StructureToPtr(objVal, hMap, True)
                Marshal.StructureToPtr(objVal, hMap, False)
                Return True
            Catch
                Return False
            Finally
                m_mtxObj.ReleaseMutex()
            End Try
        End Function

        ''' <summary>
        ''' ���L�������̍\���̂�������
        ''' </summary>
        ''' <param name="hMap">���L�������\���̂̃|�C���^</param>
        ''' <param name="objVal">���L�������\����</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function SetMapped(ByVal hMap As IntPtr, ByVal nStartIndex As Integer, ByRef objVal As ValueType) As Boolean
            Try
                m_mtxObj.WaitOne()
                '' �\���̃f�[�^�����L�������ɃZ�b�g
                Dim nOffset As New IntPtr(hMap.ToInt64 + nStartIndex)
                'Marshal.StructureToPtr(objVal, CType(nOffset, IntPtr), True)
                Marshal.StructureToPtr(objVal, nOffset, False)
                Return True
            Catch
                Return False
            Finally
                m_mtxObj.ReleaseMutex()
            End Try
        End Function


        ''' <summary>
        ''' ���L�������̍\���̂�Ǎ���
        ''' </summary>
        ''' <param name="hMap">���L�������\���̂̃|�C���^</param>
        ''' <param name="objType">���L�������\���̂̌^</param>
        ''' <param name="objVal">���L�������\����</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function GetMapped(ByVal hMap As IntPtr, ByVal objType As System.Type, ByRef objVal As ValueType) As Boolean
            Try
                m_mtxObj.WaitOne()

                '' ���L�������̃f�[�^���w��\���̂ɃZ�b�g
                objVal = DirectCast(Marshal.PtrToStructure(hMap, objType), ValueType)          '����������f�[�^���擾���A�\���̂փZ�b�g

                Return True
            Catch
                Return False
            Finally
                m_mtxObj.ReleaseMutex()
            End Try
        End Function

        ''' <summary>
        ''' ���L�������̍\���̂�Ǎ���
        ''' </summary>
        ''' <param name="hMap">���L�������\���̂̃|�C���^</param>
        ''' <param name="nStartIndex">�J�n�ʒu</param>
        ''' <param name="objType">���L�������\���̂̌^</param>
        ''' <param name="objVal">���L�������\����</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function GetMapped(ByVal hMap As IntPtr, ByVal nStartIndex As Integer, ByVal objType As System.Type, ByRef objVal As ValueType) As Boolean
            Try
                m_mtxObj.WaitOne()

                '' ���L�������̃f�[�^���w��\���̂ɃZ�b�g
                Dim nOffset As New IntPtr(hMap.ToInt64 + nStartIndex)
                objVal = DirectCast(Marshal.PtrToStructure(nOffset, objType), ValueType)                     '����������f�[�^���擾���A�\���̂փZ�b�g


                Return True
            Catch
                Return False
            Finally
                m_mtxObj.ReleaseMutex()
            End Try
        End Function

        ''' <summary>
        ''' ���L�������̍\���̂�������
        ''' </summary>
        ''' <param name="hMap">���L�������\���̂̃|�C���^</param>
        ''' <param name="objVal">���L�������\����</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function SetMappedT(Of T)(ByVal hMap As IntPtr, ByVal nStartIndex As Integer, ByRef objVal As T) As Boolean
            Try
                m_mtxObj.WaitOne()
                '' �\���̃f�[�^�����L�������ɃZ�b�g
                tMod.WriteStructure(hMap, nStartIndex, objVal)
                Return True
            Catch
                Return False
            Finally
                m_mtxObj.ReleaseMutex()
            End Try
        End Function

        ''' <summary>
        ''' ���L�������̍\���̂�Ǎ���(�^��)
        ''' </summary>
        ''' <param name="hMap">���L�������\���̂̃|�C���^</param>
        ''' <param name="nStartIndex">�J�n�ʒu</param>
        ''' <param name="objVal">���L�������\����</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function GetMappedT(Of T)(ByVal hMap As IntPtr, ByVal nStartIndex As Integer, ByRef objVal As T) As Boolean
            Try
                m_mtxObj.WaitOne()

                '' ���L�������̃f�[�^���w��\���̂ɃZ�b�g
                objVal = tMod.ReadStructure(Of T)(hMap, nStartIndex)
                Return True
            Catch
                Return False
            Finally
                m_mtxObj.ReleaseMutex()
            End Try
        End Function

    End Class
End Namespace