Option Strict On
Imports system.Runtime.InteropServices
Imports Microsoft.Win32.SafeHandles


Public Class SharedMemoryApi

	'********************************************************************
	'API��`
	'********************************************************************
	Public Const PAGE_READWRITE As Integer = &H4
	Public Const STANDARD_RIGHTS_REQUIRED As Integer = &HF0000
	Public Const SECTION_QUERY As Integer = &H1
	Public Const SECTION_MAP_WRITE As Integer = &H2
	Public Const SECTION_MAP_READ As Integer = &H4
	Public Const SECTION_MAP_EXECUTE As Integer = &H8
	Public Const SECTION_EXTEND_SIZE As Integer = &H10
	Public Const SECTION_ALL_ACCESS As Integer = STANDARD_RIGHTS_REQUIRED Or _
	   SECTION_QUERY Or _
	   SECTION_MAP_WRITE Or _
	   SECTION_MAP_READ Or _
	   SECTION_MAP_EXECUTE Or _
	   SECTION_EXTEND_SIZE
	Public Const FILE_MAP_ALL_ACCESS As Integer = SECTION_ALL_ACCESS	'' �ʏ�͂�����g�p����B


	'' �t�@�C���}�b�s���O�I�u�W�F�N�g���J��
	<DllImport("KERNEL32.DLL", EntryPoint:="OpenFileMapping", SetLastError:=True)> _
	 Public Shared Function OpenFileMapping( _
	  ByVal dwDesiredAccess As Integer, _
	  ByVal bInheritHandle As Boolean, _
	  ByVal lpName As String) _
	  As SafeFileHandle
	End Function

	'' �t�@�C���}�b�s���O�I�u�W�F�N�g���쐬
	<DllImport("KERNEL32.DLL", EntryPoint:="CreateFileMapping", SetLastError:=True)> _
	Public Shared Function CreateFileMapping( _
	 ByVal hFile As IntPtr, _
	 ByVal lpAttributes As IntPtr, _
	 ByVal flProtect As Integer, _
	 ByVal dwMaximumSizeHigh As Integer, _
	 ByVal dwMaximumSizeLow As Integer, _
	 ByVal lpName As String) _
	 As SafeFileHandle
	End Function

	'' �Ăяo�����v���Z�X�̃A�h���X��ԂɁA�t�@�C���̃r���[���}�b�s���O����
	<DllImport("KERNEL32.DLL", EntryPoint:="MapViewOfFile", SetLastError:=True)> _
	Public Shared Function MapViewOfFile( _
	 ByVal hFMapp As SafeFileHandle, _
	 ByVal dwDesiredAccess As Integer, _
	 ByVal dwFileOffsetHigh As Integer, _
	 ByVal dwFileOffsetLow As Integer, _
	 ByVal dwNumberOfBytesToMap As Integer) _
	 As IntPtr
	End Function

	'' �t�@�C���̃r���[���J������
	<DllImport("KERNEL32.dll", EntryPoint:="UnmapViewOfFile", SetLastError:=True)> _
	Public Shared Function UnmapViewOfFile( _
	 ByVal pMapped As IntPtr) _
	 As Boolean
	End Function


	''Map�̃A�h���X����(dwAllocationGranularity)���擾���邽�߂�SystemInfo
	<StructLayout(LayoutKind.Sequential)> _
	Public Structure SystemInfo
		Dim dwOemId As Integer
		Dim dwPageSize As Integer
		Dim lpMinimumApplicationAddress As IntPtr
		Dim lpMaximumApplicationAddress As IntPtr
		Dim dwActiveProcessorMask As IntPtr
		Dim dwNumberOfProcessors As Integer
		Dim dwProcessorType As Integer
		Dim dwAllocationGranularity As Integer
		Dim wProcessorLevel As Short
		Dim wProcessorRevision As Short
	End Structure

	'' SystemInfo���擾����
	<DllImport("KERNEL32.dll", EntryPoint:="GetSystemInfo", SetLastError:=True)> _
	Public Shared Sub GetSystemInfo( _
	 ByRef pMapped As SystemInfo)
	End Sub

End Class

Public Class SharedMemory
	Implements IDisposable							' �f�X�g���N�^��`

	'********************************************************************
	'�����o��`
	'********************************************************************
    Protected m_fMap As New SafeFileHandle(IntPtr.Zero, False)
	Protected m_pMap As IntPtr = IntPtr.Zero

	'********************************************************************
	'���\�b�h��`
	'********************************************************************
	''' <summary>
	''' ���L������ �A�N�Z�X
	''' </summary>
	''' <param name="strMapName">���L����������</param>
	''' <returns></returns>
	''' <remarks></remarks>
	Public Function Open(ByVal strMapName As String) As Boolean
		'�O�̂���
		If Not m_fMap.IsClosed Then m_fMap.Close()

		m_fMap = SharedMemoryApi.OpenFileMapping(SharedMemoryApi.FILE_MAP_ALL_ACCESS, False, strMapName)
		If m_fMap.IsInvalid Then
			Return False
		End If

		'' �Ăяo�����v���Z�X�̃A�h���X��ԂɁA�t�@�C���̃r���[���}�b�s���O����
		m_pMap = SharedMemoryApi.MapViewOfFile(m_fMap, SharedMemoryApi.FILE_MAP_ALL_ACCESS, 0, 0, 0)
        Return IntPtr.Zero <> m_pMap
	End Function

	''' <summary>
	''' Dispose
	''' </summary>
	''' <param name="disposing"></param>
	''' <remarks></remarks>
	Protected Overridable Sub Dispose(ByVal disposing As Boolean)
		If m_pMap <> IntPtr.Zero Then
			SharedMemoryApi.UnmapViewOfFile(m_pMap)
			m_pMap = IntPtr.Zero
		End If
		m_fMap.Dispose()
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
	''' ���L�������̎w��I�t�Z�b�g����\���̂��������ށB
	''' </summary>
	''' <typeparam name="T"> ���L�������\���̂̌^ </typeparam>
	''' <param name="nStartIndex">�J�n�ʒu</param>
	''' <param name="objVal">���L�������\����</param>
	''' <returns></returns>
	''' <remarks></remarks>
    Public Function Write(Of T)(ByVal nStartIndex As Integer, ByRef objVal As T) As Boolean
        Try
            '' �\���̃f�[�^�����L�������ɃZ�b�g
            Dim nOffset As New IntPtr(m_pMap.ToInt64 + nStartIndex)
            Marshal.StructureToPtr(objVal, nOffset, False)
            Return True
        Catch
            Return False
        End Try
    End Function



	''' <summary>
	''' ���L�������̎w��I�t�Z�b�g����\���̂�Ǎ��ݕԂ��B
	''' </summary>
	''' <typeparam name="T"> ���L�������\���̂̌^ </typeparam>
	''' <param name="nStartIndex">�J�n�ʒu</param>
	''' <param name="objVal">���L�������\����</param>
	''' <returns></returns>
	''' <remarks></remarks>
	Public Function Read(Of T)(ByVal nStartIndex As Integer, ByRef objVal As T) As Boolean
		Try

			'' ���L�������̃f�[�^���w��\���̂ɃZ�b�g
			Dim nOffset As New IntPtr(m_pMap.ToInt64 + nStartIndex)
			objVal = DirectCast(Marshal.PtrToStructure(nOffset, GetType(T)), T)					'����������f�[�^���擾���A�\���̂փZ�b�g

			Return True
		Catch
			Return False
		End Try
	End Function

End Class



''' <summary>
''' �K�v�ɂȂ�s�x�A���L���������}�b�s���O���ď�������N���X
''' </summary>
''' <remarks>�A�h���X�E�I�t�Z�b�g�ށ���΁F���L�������S�̂̐擪����́B���΁F���܃}�b�v���Ă��鋤�L�������̓�����B���z�F���z�������̃A�h���X�B
''' </remarks>
Public Class SharedMemoryPartial
	Implements IDisposable							' �f�X�g���N�^��`

	'********************************************************************
	'�����o��`
	'********************************************************************
	Protected m_fMap As SafeFileHandle
	Protected m_accessR As Integer = SharedMemoryApi.FILE_MAP_ALL_ACCESS
	'�}�b�v���ꂽ�G���A�̉��z�A�h���X�B
	Protected m_pMap As IntPtr = IntPtr.Zero


	'�Œ�ł����̂��炢�̃T�C�Y�̓}�b�v���Ă��������ȁA�Ƃ����T�C�Y�B
	Protected ReadOnly m_CacheLenMin As Integer = 65536 * 4
	'���܌��݃}�b�v���Ă��郁�����̗̈�(���L����������)= m_nCacheOffset ~ m_nCacheLen
	Protected m_nCacheOffset As IntPtr
	Protected m_nCacheLen As Integer = 0
	'Map����Ƃ��̃A�h���X����BAPI�ɓn���̂́AnAllocationSize�̔{���łȂ��ƃ_���B
	Protected Shared nAllocationSize As Integer = 0
	Protected Shared nAllocationMask As Long = 0

	'********************************************************************
	'���\�b�h��`
	'********************************************************************
	''' <summary>
	''' ���L������ �A�N�Z�X
	''' </summary>
	''' <param name="strMapName">���L����������</param>
	''' <remarks></remarks>
	Public Sub New(ByVal strMapName As String, Optional ByVal accessR As Integer = SharedMemoryApi.FILE_MAP_ALL_ACCESS)
		m_accessR = accessR
		m_fMap = SharedMemoryApi.OpenFileMapping(m_accessR, False, strMapName)

		If nAllocationSize = 0 Then
			Dim si As New SharedMemoryApi.SystemInfo
			SharedMemoryApi.GetSystemInfo(si)
			nAllocationSize = si.dwAllocationGranularity
			nAllocationMask = Not (CLng(nAllocationSize) - 1)
		End If
	End Sub


	Public ReadOnly Property IsInvalid() As Boolean
		Get
			Return m_fMap.IsInvalid
		End Get
	End Property

	''' <summary>
	''' Dispose
	''' </summary>
	''' <param name="disposing"></param>
	''' <remarks></remarks>
	Protected Overridable Sub Dispose(ByVal disposing As Boolean)
		Unmap()
		m_fMap.Dispose()
	End Sub
#Region " IDisposable Support "
	' ���̃R�[�h�́A�j���\�ȃp�^�[���𐳂��������ł���悤�� Visual Basic �ɂ���Ēǉ�����܂����B
	Public Sub Dispose() Implements IDisposable.Dispose
		' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
		Dispose(True)
		GC.SuppressFinalize(Me)
	End Sub
#End Region



	Sub Unmap()
		If m_pMap <> IntPtr.Zero Then
			SharedMemoryApi.UnmapViewOfFile(m_pMap)
			m_pMap = IntPtr.Zero
		End If
	End Sub


	''' <summary>
	''' ���L�������̐�΃I�t�Z�b�g�����z�������A�h���X�ɕϊ�����
	''' </summary>
	''' <param name="nAddr">���L�������̐�΃I�t�Z�b�g</param>
	''' <returns>���z�������̃A�h���X</returns>
	Function ConvVirtualAddress(ByVal nAddr As IntPtr) As IntPtr
		If IntPtr.Size = 4 Then
			Return New IntPtr(m_pMap.ToInt32 + nAddr.ToInt32 - m_nCacheOffset.ToInt32)
		Else
			Return New IntPtr(m_pMap.ToInt64 + nAddr.ToInt64 - m_nCacheOffset.ToInt64)
		End If
	End Function

	''' <summary>
	''' ���݂̃}�b�v�i�L���b�V���j��ɗv�����ꂽ��Έʒu�`�T�C�Y������Ă��邩�H
	''' </summary>
	Function InCacheRange(ByVal nAddr As IntPtr, ByVal nLength As Integer) As Boolean
		Dim nROffset As Long = (nAddr.ToInt64 - m_nCacheOffset.ToInt64)
		If nROffset < 0 Then Return False
		Return nROffset + nLength < m_nCacheLen
	End Function

	<StructLayout(LayoutKind.Explicit)> _
	Private Structure LargeInteger
		<FieldOffset(0)> Dim QuadPart As Long
		<FieldOffset(0)> Dim LowPart As Integer
		<FieldOffset(4)> Dim HighPart As Integer
	End Structure

	''' <summary>
	''' ���L�������̐�΃I�t�Z�b�g���@���z��������Ƀ}�b�v����
	''' </summary>
	Function Map(ByVal nAddr As IntPtr, ByVal nLength As Integer) As IntPtr
		Unmap()

		'�A�h���X�́AnAllocationSize�ŃA���C������Ă���K�v������B�K�X�O�֖߂��B
		Dim nAddrAlign As Long = nAddr.ToInt64() And nAllocationMask
		'�A�h���X���O�ɖ߂������A�T�C�Y�͑傫���Ȃ�.�@
		' + �T�C�Y��nAllocationSize�ŃA���C�������K�v������B
		Dim nLenAlign As Integer = CInt((nLength + (nAddr.ToInt64() - nAddrAlign) + nAllocationSize - 1) And nAllocationMask)

		'long���Aint*2�ɕ�������B
		Dim li As New LargeInteger
		li.HighPart = 0
		li.LowPart = 0
		li.QuadPart = nAddrAlign

		'�}�b�v����B
		m_nCacheOffset = New IntPtr(nAddrAlign)
		m_nCacheLen = Math.Max(m_CacheLenMin, nLenAlign)
		m_pMap = SharedMemoryApi.MapViewOfFile(m_fMap, m_accessR, li.HighPart, li.LowPart, m_nCacheLen)
		If (IntPtr.Zero = m_pMap) Then
			Throw New System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error(), "���L�������̃}�b�v�Ɏ��s")
		End If
		Return m_pMap
	End Function



	''' <summary>
	''' ���L�������̎w��I�t�Z�b�g����\���̂��������ށB
	''' </summary>
	''' <typeparam name="T"> ���L�������\���̂̌^ </typeparam>
	''' <param name="nAddr">�J�n�ʒu(��΃I�t�Z�b�g)</param>
	''' <param name="objVal">���L�������\����</param>
	Public Function Write(Of T)(ByVal nAddr As IntPtr, ByRef objVal As T) As Boolean
		Try
			If Not InCacheRange(nAddr, Marshal.SizeOf(objVal)) Then
				Map(nAddr, Marshal.SizeOf(objVal))
			End If

			'' �\���̃f�[�^�����L�������ɃZ�b�g
			Marshal.StructureToPtr(objVal, ConvVirtualAddress(nAddr), False)
			Return True
		Catch
			Return False
		End Try
	End Function

	'��̃A�h���X������Int�ŁB
	Public Function Write(Of T)(ByVal nAddr As Integer, ByRef objVal As T) As Boolean
		Return Write(New IntPtr(nAddr), objVal)
	End Function


	''' <summary>
	''' ���L�������̎w��I�t�Z�b�g����\���̂�Ǎ��ݕԂ��B
	''' </summary>
	''' <typeparam name="T"> ���L�������\���̂̌^ </typeparam>
	''' <param name="nAddr">�J�n�ʒu(��΃I�t�Z�b�g)</param>
	''' <param name="objVal">���L�������\����</param>
	''' <returns></returns>
	''' <remarks></remarks>
	Public Function Read(Of T)(ByVal nAddr As IntPtr, ByRef objVal As T) As Boolean
		Try
			If Not InCacheRange(nAddr, Marshal.SizeOf(objVal)) Then
				Map(nAddr, Marshal.SizeOf(objVal))
			End If

			'' ���L�������̃f�[�^���w��\���̂ɃZ�b�g
			objVal = DirectCast(Marshal.PtrToStructure(ConvVirtualAddress(nAddr), GetType(T)), T)					'����������f�[�^���擾���A�\���̂փZ�b�g

			Return True
		Catch
			Return False
		End Try
	End Function

	'��̃A�h���X������Int�ŁB
	Public Function Read(Of T)(ByVal nAddr As Integer, ByRef objVal As T) As Boolean
		Return Read(New IntPtr(nAddr), objVal)
	End Function

End Class

Public Class SharedMemoryReaeWrite
	Inherits SharedMemory

	<DllImport("kernel32")> Shared Function WriteFile(ByVal hFile As Microsoft.Win32.SafeHandles.SafeFileHandle, ByVal lpBuffer As IntPtr, ByVal nWrite As Integer, ByRef nWritten As Integer, ByVal lpOverlapped As IntPtr) As Boolean
	End Function

	<DllImport("kernel32")> Shared Function ReadFile(ByVal hFile As Microsoft.Win32.SafeHandles.SafeFileHandle, ByVal lpBuffer As IntPtr, ByVal nRead As Integer, ByRef nWritten As Integer, ByVal lpOverlapped As IntPtr) As Boolean
	End Function

	<DllImport("kernel32")> Shared Function GetFileSizeEx(ByVal hFile As Microsoft.Win32.SafeHandles.SafeFileHandle, ByRef nFileSize As Long) As Boolean
	End Function

	<StructLayout(LayoutKind.Sequential)> _
	Structure MEMORY_BASIC_INFORMATION
		Public BaseAddress As IntPtr
		Public AllocationBase As IntPtr
		Public AllocationProtect As Integer
		Public RegionSize As IntPtr
		Public State As Integer
		Public Protect As Integer
		Public Type As Integer
	End Structure

	<DllImport("kernel32")> Shared Function VirtualQuery(ByVal lpAddress As IntPtr, ByRef memoryinf As MEMORY_BASIC_INFORMATION, ByVal dwLength As IntPtr) As Integer
	End Function


	''' <summary>
	''' ���L����������ǂݍ���Ńt�@�C���ɏo�͂���
	''' </summary>
	''' <param name="ShName">���L����������</param>
	''' <param name="size">���L�������T�C�Y�i�T�C�Y�w�肵�Ȃ��ꍇ�́A�擾�������L�������T�C�Y�j</param>
	''' <returns>0:���� 1:���L�������I�[�v���G���[ 2:���L�������T�C�Y=0 3:�w��T�C�Y�s�� -1�ȉ� Win32API�G���[ </returns>
	''' <remarks></remarks>
	Public Function WriteToFile(ByVal ShName As String, Optional ByVal size As Integer = 0) As Integer
		Dim rc As Integer = 0

		' ���L���������I�[�v������
		If Open(ShName) = False Then
			Return 1
		End If

		' ���L�������T�C�Y���擾���� �� ���蓖�ăT�C�Y�ł͂Ȃ��A�y�[�W�T�C�Y�̔{���H�ƂȂ�
		Dim mapinfo As New MEMORY_BASIC_INFORMATION
		VirtualQuery(m_pMap, mapinfo, New IntPtr(Marshal.SizeOf(mapinfo)))

		' �擾�������L�������T�C�Y�`�F�b�N
		If mapinfo.RegionSize.ToInt32 = 0 Then
			' ���L�������͊��蓖�Ă��Ă��Ȃ��H
			Return 2
		End If

		' �������݃T�C�Y�����肷��B�������݃t�@�C���T�C�Y�w��̏ꍇ�͂��̒l���g�p����
		Dim wSize As Integer = CInt(IIf(size = 0, mapinfo.RegionSize.ToInt32, size))

		If wSize > mapinfo.RegionSize.ToInt32 Then
			' �w��T�C�Y�����L�������T�C�Y���傫���ꍇ�͂ǂ����邩
			Return 3
		End If

		' �o�̓t�@�C������ "<���L�������e�[�u����>.bin" �Ƃ���B
		Dim fname As String = IO.Path.Combine(".\", ShName & ".bin")

		' �w�肳�ꂽ���L�������̓��e���t�@�C���ɏo�͂���
		Try
			Using fsw As New IO.FileStream(fname, IO.FileMode.Create)
				Dim nWritten As Integer = 0
				If WriteFile(fsw.SafeFileHandle, m_pMap, wSize, nWritten, IntPtr.Zero) = False Then
					rc = Marshal.GetLastWin32Error()
				End If
			End Using
		Catch ex As Exception
			' ���炩�̗�O����
			rc = 999
		End Try

		Return rc

	End Function

	''' <summary>
	''' �t�@�C������ǂݍ���ŋ��L�������ɏo�͂���
	''' </summary>
	''' <param name="ShName">���L����������</param>
	''' <param name="size">���L�������T�C�Y�i�T�C�Y�w�肵�Ȃ��ꍇ�́A�擾�������L�������T�C�Y�j</param>
	''' <returns>0:���� 1:���L�������I�[�v���G���[ 2:���L�������T�C�Y=0 3:�w��T�C�Y�s�� -1�ȉ� Win32API�G���[ </returns>
	''' <remarks></remarks>
	Public Function ReadFromFile(ByVal ShName As String, Optional ByVal size As Integer = 0) As Integer
		Dim rc As Integer = 0

		' ���L���������I�[�v������
		If Open(ShName) = False Then
			Return 1
		End If

		' ���L�������T�C�Y���擾���� �� ���蓖�ăT�C�Y�ł͂Ȃ��A�y�[�W�T�C�Y�̔{���H�ƂȂ�
		Dim mapinfo As New MEMORY_BASIC_INFORMATION
		VirtualQuery(m_pMap, mapinfo, New IntPtr(Marshal.SizeOf(mapinfo)))

		' �擾�������L�������T�C�Y�`�F�b�N
		If mapinfo.RegionSize.ToInt32 = 0 Then
			' ���L�������͊��蓖�Ă��Ă��Ȃ��H
			Return 2
		End If

		' �������݃T�C�Y�����肷��B�T�C�Y�w��̏ꍇ�͂��̒l���g�p����
		Dim wSize As Integer = CInt(IIf(size = 0, mapinfo.RegionSize.ToInt32, size))

		If wSize > mapinfo.RegionSize.ToInt32 Then
			' �w��T�C�Y�����L�������T�C�Y���傫���ꍇ�̓G���[
			Return 3
		End If

		' �ǂݍ��݃t�@�C������ "<���L�������e�[�u����>.bin" �Ƃ���B
		Dim fname As String = IO.Path.Combine(".\", ShName & ".bin")

		If IO.File.Exists(fname) = False Then
			' �w�肳�ꂽ�t�@�C�������݂��Ȃ�
			Return 4
		End If

		Using fsr As New IO.FileStream(fname, IO.FileMode.Open)
			Dim nRead As Integer = 0
			If ReadFile(fsr.SafeFileHandle, m_pMap, wSize, nRead, IntPtr.Zero) = False Then
				rc = Marshal.GetLastWin32Error()
			End If
		End Using

		Return rc

	End Function

End Class
