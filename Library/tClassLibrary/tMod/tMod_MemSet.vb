'*********************************************************************************
'������������s�����W���[���Q
'	[Ver]
'		Ver.01    2005/12/25  vs2005 �Ή�
'
'	[����]
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
Imports System.IO
Imports System.Runtime.InteropServices


Namespace tMod
    ''' <summary>
    ''' ������������s�����W���[���Q
    ''' </summary>
    ''' <remarks></remarks>
    Public Module tMod_MemSet

        ''' <summary>
        ''' �\���́E�N���X�̃T�C�Y�擾
        ''' </summary>
        ''' <param name="objVal">�z��Ŗ����C���X�^���X</param>
        ''' <returns>�T�C�Y</returns>
        ''' <remarks></remarks>
        Public Function SizeOf(ByVal objVal As Object) As Integer
            Try
                Return Marshal.SizeOf(objVal)
            Catch ex As Exception
                Return 0
            End Try
        End Function

        ''' <param name="objVal">�z��̃N���X�E�\���́E�ϐ� (�������A�S�ē����^�ł��鎖)</param>
        ''' <returns>�T�C�Y</returns>
        Public Function SizeOf(ByVal objVal As System.Array) As Integer
            Try
                Return objVal.Length * SizeOf(objVal.GetValue(0))
            Catch ex As Exception
                Return 0
            End Try
        End Function

        ''' <summary>
        ''' �\���̂�����������ǂݏo��
        ''' </summary>
        ''' <typeparam name="T">�\���̌^</typeparam>
        ''' <param name="addr_base">������</param>
        ''' <param name="offset">�������I�t�Z�b�g</param>
        ''' <returns>����������ǂ񂾍\����</returns>
        Public Function ReadStructure(Of T)(ByVal addr_base As IntPtr, Optional ByVal offset As Integer = 0) As T
            Dim addr As New IntPtr(addr_base.ToInt64 + offset)
            Return DirectCast(Marshal.PtrToStructure(addr, GetType(T)), T)
        End Function

        ''' <summary>
        ''' �\���̂��������ɏ�������
        ''' </summary>
        ''' <typeparam name="T">�^�i���������j</typeparam>
        ''' <param name="addr_base">������</param>
        ''' <param name="offset">�������I�t�Z�b�g</param>
        ''' <param name="obj">�\����</param>
        Public Sub WriteStructure(Of T)(ByVal addr_base As IntPtr, ByVal offset As Integer, ByRef obj As T)
            Dim addr As New IntPtr(addr_base.ToInt64 + offset)
            Marshal.StructureToPtr(obj, addr, False)
        End Sub

        ''' <summary>
        ''' �\���̂��������ɏ�������
        ''' </summary>
        ''' <typeparam name="T">�^�i���������j</typeparam>
        ''' <param name="addr_base">������</param>
        ''' <param name="obj">�\����</param>
        Public Sub WriteStructure(Of T)(ByVal addr_base As IntPtr, ByRef obj As T)
            Marshal.StructureToPtr(obj, addr_base, False)
        End Sub


        ''' <summary>
        ''' �o�C�g�z�񂩂�\���̂փR�s�[
        ''' </summary>
        ''' <param name="from">�R�s�[���f�[�^</param>
        ''' <param name="startIndex">�R�s�[���̊J�n�z��ʒu (0�ؼ��)</param>
        ''' <param name="typTo">�R�s�[��f�[�^�̈� (�\����)</param>
        ''' <param name="length">�R�s�[�T�C�Y</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function MemSet(ByVal from() As Byte, ByVal startIndex As Integer, ByRef typTo As ValueType, ByVal length As Integer) As Boolean
            Dim p As IntPtr
            Try
                p = Marshal.AllocHGlobal(length)                        '�������擾
                Marshal.Copy(from, startIndex, p, length)               '��������Ƀf�[�^���Z�b�g
                typTo = DirectCast(Marshal.PtrToStructure(p, typTo.GetType), ValueType)       '����������f�[�^���擾���A�\���̂փZ�b�g
                Return True
            Catch ex As Exception
                typTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  '�������J��
            End Try
        End Function

        ''' <summary>
        ''' �o�C�g�z�񂩂�I�u�W�F�N�g�փR�s�[
        ''' </summary>
        ''' <param name="from">�R�s�[���f�[�^</param>
        ''' <param name="startIndex">�R�s�[���̊J�n�z��ʒu (0�ؼ��)</param>
        ''' <param name="objTo">�R�s�[��f�[�^�̈� (�I�u�W�F�N�g)</param>
        ''' <param name="length">�R�s�[�T�C�Y</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function MemSet(ByVal from() As Byte, ByVal startIndex As Integer, ByRef objTo As Object, ByVal length As Integer) As Boolean
            Dim p As IntPtr
            Try
                p = Marshal.AllocHGlobal(length)                        '�������擾
                Marshal.Copy(from, startIndex, p, length)               '��������Ƀf�[�^���Z�b�g
                objTo = Marshal.PtrToStructure(p, objTo.GetType)        '����������f�[�^���擾���A�\���̂փZ�b�g
                Return True
            Catch ex As Exception
                objTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  '�������J��
            End Try
        End Function

        ''' <summary>
        ''' �\���̂���o�C�g�z��փR�s�[
        ''' </summary>
        ''' <param name="from">�R�s�[���\����</param>
        ''' <param name="aryTo">�R�s�[��f�[�^�z��</param>
        ''' <param name="startIndex">�R�s�[��̊J�n�z��ʒu (0�ؼ��)</param>
        ''' <param name="length">�R�s�[�T�C�Y</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function MemSet(ByVal from As ValueType, ByRef aryTo() As Byte, ByVal startIndex As Integer, ByVal length As Integer) As Boolean
            Dim p As IntPtr
            Try
                p = Marshal.AllocHGlobal(SizeOf(from))                  '�������擾
                Marshal.StructureToPtr(from, p, True)                   '��������Ƀf�[�^���Z�b�g
                Marshal.Copy(p, aryTo, startIndex, length)
                Return True
            Catch ex As Exception
                aryTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  '�������J��
            End Try
        End Function

        ''' <summary>
        ''' �\���̂���\���̂փR�s�[
        ''' </summary>
        ''' <param name="from">�R�s�[���f�[�^</param>
        ''' <param name="typTo">�R�s�[��f�[�^�̈� (�\����)</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function MemSet(ByVal from As ValueType, ByRef typTo As ValueType) As Boolean
            Dim p As IntPtr
            Try
                p = Marshal.AllocHGlobal(SizeOf(from))                  '�������擾
                Marshal.StructureToPtr(from, p, True)                   '��������Ƀf�[�^���Z�b�g
                typTo = DirectCast(Marshal.PtrToStructure(p, typTo.GetType), ValueType)       '����������f�[�^���擾���A�\���̂փZ�b�g
                Return True
            Catch ex As Exception
                typTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  '�������J��
            End Try
        End Function

#Region "byte�z�񂩂�z��փR�s�["
        ''' <summary>
        ''' byte�z�񂩂�z��փR�s�[
        ''' </summary>
        ''' <param name="from">�R�s�[���f�[�^</param>
        ''' <param name="startIndex">�R�s�[��̊J�n�z��ʒu (0�ؼ��)</param>
        ''' <param name="aryTo">�R�s�[��f�[�^�̈�</param>
        ''' <param name="count">�R�s�[����v�f��</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function MemSet(ByVal from() As Byte, ByVal startIndex As Integer, ByRef aryTo() As Char, ByVal count As Integer) As Boolean
            Dim p As IntPtr
            Try
                Dim allsize As Integer
                allsize = SizeOf(aryTo(startIndex)) * count
                p = Marshal.AllocHGlobal(allsize)                       '�������擾
                Marshal.Copy(from, startIndex, p, allsize)              '��������Ƀf�[�^���Z�b�g
                Marshal.Copy(p, aryTo, startIndex, count)
                Return True
            Catch ex As Exception
                aryTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  '�������J��
            End Try
        End Function
        ''' <param name="from">�R�s�[���f�[�^</param>
        ''' <param name="startIndex">�R�s�[��̊J�n�z��ʒu (0�ؼ��)</param>
        ''' <param name="aryTo">�R�s�[��f�[�^�̈�</param>
        ''' <param name="count">�R�s�[����v�f��</param>
        ''' <returns>���A���</returns>
        Public Function MemSet(ByVal from() As Byte, ByVal startIndex As Integer, ByRef aryTo() As Single, ByVal count As Integer) As Boolean
            Dim p As IntPtr
            Try
                Dim allsize As Integer
                allsize = SizeOf(aryTo(startIndex)) * count
                p = Marshal.AllocHGlobal(allsize)                       '�������擾
                Marshal.Copy(from, startIndex, p, allsize)              '��������Ƀf�[�^���Z�b�g
                Marshal.Copy(p, aryTo, startIndex, count)
                Return True
            Catch ex As Exception
                aryTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  '�������J��
            End Try
        End Function
        ''' <param name="from">�R�s�[���f�[�^</param>
        ''' <param name="startIndex">�R�s�[��̊J�n�z��ʒu (0�ؼ��)</param>
        ''' <param name="aryTo">�R�s�[��f�[�^�̈�</param>
        ''' <param name="count">�R�s�[����v�f��</param>
        ''' <returns>���A���</returns>
        Public Function MemSet(ByVal from() As Byte, ByVal startIndex As Integer, ByRef aryTo() As Double, ByVal count As Integer) As Boolean
            Dim p As IntPtr
            Try
                Dim allsize As Integer
                allsize = SizeOf(aryTo(startIndex)) * count
                p = Marshal.AllocHGlobal(allsize)                       '�������擾
                Marshal.Copy(from, startIndex, p, allsize)              '��������Ƀf�[�^���Z�b�g
                Marshal.Copy(p, aryTo, startIndex, count)
                Return True
            Catch ex As Exception
                aryTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  '�������J��
            End Try
        End Function
        ''' <param name="from">�R�s�[���f�[�^</param>
        ''' <param name="startIndex">�R�s�[��̊J�n�z��ʒu (0�ؼ��)</param>
        ''' <param name="aryTo">�R�s�[��f�[�^�̈�</param>
        ''' <param name="count">�R�s�[����v�f��</param>
        ''' <returns>���A���</returns>
        Public Function MemSet(ByVal from() As Byte, ByVal startIndex As Integer, ByRef aryTo() As Short, ByVal count As Integer) As Boolean
            Dim p As IntPtr
            Try
                Dim allsize As Integer
                allsize = SizeOf(aryTo(startIndex)) * count
                p = Marshal.AllocHGlobal(allsize)                       '�������擾
                Marshal.Copy(from, startIndex, p, allsize)              '��������Ƀf�[�^���Z�b�g
                Marshal.Copy(p, aryTo, startIndex, count)
                Return True
            Catch ex As Exception
                aryTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  '�������J��
            End Try
        End Function
        ''' <param name="from">�R�s�[���f�[�^</param>
        ''' <param name="startIndex">�R�s�[��̊J�n�z��ʒu (0�ؼ��)</param>
        ''' <param name="aryTo">�R�s�[��f�[�^�̈�</param>
        ''' <param name="count">�R�s�[����v�f��</param>
        ''' <returns>���A���</returns>
        Public Function MemSet(ByVal from() As Byte, ByVal startIndex As Integer, ByRef aryTo() As Integer, ByVal count As Integer) As Boolean
            Dim p As IntPtr
            Try
                Dim allsize As Integer
                allsize = SizeOf(aryTo(startIndex)) * count
                p = Marshal.AllocHGlobal(allsize)                       '�������擾
                Marshal.Copy(from, startIndex, p, allsize)              '��������Ƀf�[�^���Z�b�g
                Marshal.Copy(p, aryTo, startIndex, count)
                Return True
            Catch ex As Exception
                aryTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  '�������J��
            End Try
        End Function
        ''' <param name="from">�R�s�[���f�[�^</param>
        ''' <param name="startIndex">�R�s�[��̊J�n�z��ʒu (0�ؼ��)</param>
        ''' <param name="aryTo">�R�s�[��f�[�^�̈�</param>
        ''' <param name="count">�R�s�[����v�f��</param>
        ''' <returns>���A���</returns>
        Public Function MemSet(ByVal from() As Byte, ByVal startIndex As Integer, ByRef aryTo() As Long, ByVal count As Integer) As Boolean
            Dim p As IntPtr
            Try
                Dim allsize As Integer
                allsize = SizeOf(aryTo(startIndex)) * count
                p = Marshal.AllocHGlobal(allsize)                       '�������擾
                Marshal.Copy(from, startIndex, p, allsize)              '��������Ƀf�[�^���Z�b�g
                Marshal.Copy(p, aryTo, startIndex, count)
                Return True
            Catch ex As Exception
                aryTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  '�������J��
            End Try
        End Function
#End Region
    End Module
End Namespace


''' <summary>
''' �z��f�[�^���s���~�߂��āA�������|�C���^���擾����B
''' </summary>
''' <remarks></remarks>
Public Class PinnedPtr
    Implements IDisposable
    Private g1 As GCHandle

    Public Sub New(ByVal obj() As Byte)
        g1 = GCHandle.Alloc(obj, GCHandleType.Pinned)
    End Sub

    Public ReadOnly Property Ptr As IntPtr
        Get
            Return g1.AddrOfPinnedObject
        End Get
    End Property

    Public Sub Dispose() Implements IDisposable.Dispose
        g1.Free()
        GC.SuppressFinalize(Me)
    End Sub

End Class


Public Class VirtualAllocMemory
    Implements IDisposable

    Const MEM_COMMIT As Integer = &H1000
    Const MEM_TOPDOWN As Integer = &H100000
    Const PAGE_READWRITE As Integer = 4
    Const MEM_RELEASE As Integer = &H8000

    <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi)> Shared Function VirtualAlloc(lpAddress As IntPtr, dwSize As IntPtr, flAllocationType As Integer, flProtect As Integer) As IntPtr
    End Function
    <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi)> Shared Function VirtualFree(lpAddress As IntPtr, dwSize As IntPtr, dwFreeType As Integer) As Boolean
    End Function

    Private m_ptr As IntPtr
    Private m_size As Long

    ''' <summary>
    ''' ���������L���̏���n���BUsing�������ǉ���������Ȃ��Ȃ����Ƃ��ɁB
    ''' </summary>
    ''' <returns></returns>
    Public Function MoveTo() As VirtualAllocMemory
        Dim vm As New VirtualAllocMemory
        vm.m_ptr = Me.m_ptr
        vm.m_size = Me.Size
        m_ptr = IntPtr.Zero
        m_size = 0
        Return vm
    End Function

    Public ReadOnly Property Valid As Boolean
        Get
            Return m_ptr <> IntPtr.Zero
        End Get
    End Property

    Public ReadOnly Property Ptr As IntPtr
        Get
            Return m_ptr
        End Get
    End Property

    Public ReadOnly Property Size As Long
        Get
            Return m_size
        End Get
    End Property


    Public Sub New(size As Long)
        m_ptr = VirtualAlloc(IntPtr.Zero, New IntPtr(size), MEM_COMMIT Or MEM_TOPDOWN, PAGE_READWRITE)
        m_size = size
        GC.AddMemoryPressure(size)
    End Sub
    Private Sub New()
    End Sub

    Public Sub Dispose() Implements IDisposable.Dispose
        Free()
        GC.SuppressFinalize(Me)
    End Sub

    ''' <summary>
    ''' �t�@�C�i���C�U�i�ʏ��Dispose�ŉ������j
    ''' </summary>
    Protected Overrides Sub Finalize()
        Free()
    End Sub

    Private Sub Free()
        If m_ptr <> IntPtr.Zero Then
            VirtualFree(m_ptr, IntPtr.Zero, MEM_RELEASE)
            GC.RemoveMemoryPressure(m_size)
        End If
        m_ptr = IntPtr.Zero
        m_size = 0
    End Sub


End Class