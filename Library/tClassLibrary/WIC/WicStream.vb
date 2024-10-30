Imports System.Runtime.InteropServices

Namespace Wic
    <ComImport(), Guid("135FF860-22B7-4DDF-B0F6-218F4F299A43"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)> _
    Public Interface WICStream
        Inherits ComTypes.IStream

        Overloads Sub Clone(ByRef ppstm As ComTypes.IStream)

        Overloads Sub Commit(ByVal grfCommitFlags As Integer)

        Overloads Sub CopyTo(ByVal pstm As ComTypes.IStream, ByVal cb As Long, ByVal pcbRead As IntPtr, ByVal pcbWritten As IntPtr)

        Overloads Sub LockRegion(ByVal libOffset As Long, ByVal cb As Long, ByVal dwLockType As Integer)

        Overloads Sub Read(ByVal pv() As Byte, ByVal cb As Integer, ByVal pcbRead As IntPtr)

        Overloads Sub Revert()

        Overloads Sub Seek(ByVal dlibMove As Long, ByVal dwOrigin As Integer, ByVal plibNewPosition As IntPtr)

        Overloads Sub SetSize(ByVal libNewSize As Long)

        Overloads Sub Stat(ByRef pstatstg As ComTypes.STATSTG, ByVal grfStatFlag As Integer)

        Overloads Sub UnlockRegion(ByVal libOffset As Long, ByVal cb As Long, ByVal dwLockType As Integer)

        Overloads Sub Write(ByVal pv() As Byte, ByVal cb As Integer, ByVal pcbWritten As IntPtr)

        Sub InitializeFromIStream(<[In](), MarshalAs(UnmanagedType.Interface)> ByVal pIStream As ComTypes.IStream)

        Sub InitializeFromFilename(<[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal wzFilename As String, <[In]()> ByVal dwDesiredAccess As UInt32)

        Sub InitializeFromMemory(<[In](), MarshalAs(UnmanagedType.LPArray, SizeParamIndex:=1)> ByVal pbBuffer() As Byte, <[In]()> ByVal cbBufferSize As Integer)

        Sub InitializeFromIStreamRegion(<[In](), MarshalAs(UnmanagedType.Interface)> ByVal pIStream As ComTypes.IStream, <[In]()> ByVal ulOffset As Long, <[In]()> ByVal ulMaxSize As Long)
    End Interface
End Namespace
