Imports System.Runtime.InteropServices

Namespace Wic
    Module UtilConstance
        Public Const Generic_Read As UInt32 = &H80000000&
    End Module


    <StructLayout(LayoutKind.Sequential, Pack:=4)> _
    Public Class WicRect
        Public X As Integer
        Public Y As Integer
        Public Width As Integer
        Public Height As Integer
    End Class

    <ComImport(), Guid("3C613A02-34B2-44EA-9A7C-45AEA9C6FD6D"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)> _
    Public Interface ColorContext
        Sub InitializeFromFilename(<[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal wzFilename As String)
        Sub InitializeFromMemory(<[In]()> ByRef pbBuffer As Byte, <[In]()> ByVal cbBufferSize As UInt32)
        Sub InitializeFromExifColorSpace(<[In]()> ByVal value As UInt32)
        Function WicType() As Object
        Function ProfileBytes(<[In]()> ByVal cbBuffer As Integer, <MarshalAs(UnmanagedType.LPArray, SizeParamIndex:=0)> ByVal pbBuffer() As Byte) As Integer
        Function ExifColorSpace() As Integer
    End Interface

    <ComImport(), Guid("00000120-a8f2-4877-ba0a-fd2b6645fb94"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)> _
    Public Interface BitmapSource
        Sub Size(<Out()> ByRef width As Integer, <Out()> ByRef height As Integer)

        Function PixelFormat() As Guid

        Sub Resolution(<Out()> ByRef DpiX As Double, <Out()> ByRef DpiY As Double)

        Sub CopyPalette(ByVal pIPalette As Palette)

        Sub CopyPixels(ByVal rect As WicRect, ByVal stride As Integer, ByVal buffersize As Integer, ByVal Buffer As IntPtr)

    End Interface

    <ComImport(), Guid("3B16811B-6A43-4ec9-A813-3D930C13B940"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)> _
    Public Interface BitmapFrameDecode
        Inherits BitmapSource
        Overloads Sub Size(<Out()> ByRef width As Integer, <Out()> ByRef height As Integer)

        Overloads Function PixelFormat() As Guid

        Overloads Sub Resolution(<Out()> ByRef DpiX As Double, <Out()> ByRef DpiY As Double)

        Overloads Sub CopyPalette(ByVal pIPalette As Palette)

        Overloads Sub CopyPixels(ByVal rect As WicRect, ByVal stride As Integer, ByVal buffersize As Integer, ByVal Buffer As IntPtr)
        Function MetadataQueryReader() As Object

        Function ColorContexts(ByVal count As Integer, <MarshalAs(UnmanagedType.LPArray, SizeParamIndex:=0)> ByVal contexts() As ColorContext) As Integer

        Function Thumbnail() As BitmapSource
    End Interface


    <ComImport(), Guid("9EDDE9E7-8DEE-47ea-99DF-E6FAF2ED44BF"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)> _
    Public Interface BitmapDecoder
        Function QueryCapability(ByVal pistream As ComTypes.IStream) As System.UInt32

        Sub Initialize(ByVal pistream As ComTypes.IStream, ByVal cacheOptions As Integer)

        Function ContainerFormat() As Guid

        Function DecoderInfo() As Object

        Sub CopyPalette(ByVal pIPalette As Palette)

        Function MetadataQueryReader() As Object

        Function Preview() As BitmapSource

        Function ColorContexts(ByVal cCount As Integer, ByVal ppIColorContexts() As ColorContext) As Integer

        Function Thumbnail() As BitmapSource

        Function FrameCount() As Integer

        Function Frame(ByVal Index As Integer) As BitmapFrameDecode
    End Interface

End Namespace