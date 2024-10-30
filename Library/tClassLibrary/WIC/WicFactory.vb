Imports System.Runtime.InteropServices

Namespace Wic
    Public Enum DecodeOptions
        ' Fields
        MetadataCacheOnDemand = 0
        MetadataCacheOnLoad = 1
        WICMETADATACACHEOPTION_FORCE_DWORD = &H7FFFFFFF
    End Enum
    Module FactoryGuid
        Public ReadOnly CLSID_WICImagingFactory As New Guid(&HCACAF262&, &H9370, &H4615, &HA1, &H3B, &H9F, &H55, &H39, &HDA, &H4C, &HA)
    End Module

    <ComImport(), Guid("ec5ec8a9-c395-4314-9c77-54d7a935ff70"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)> _
    Public Interface Factory

        Function CreateDecoderFromFilename(<[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal filename As String, _
                                           <[In]()> ByVal guidVendor As IntPtr, _
                                            ByVal dwDesiredAccess As System.UInt32, _
                                            ByVal metadataOptions As DecodeOptions _
                                           ) As BitmapDecoder

        Function CreateDecoderFromStream(<[In]> ByVal pIStream As ComTypes.IStream, <[In]> ByRef pguidVendor As Guid, <[In]> ByVal metadataOptions As DecodeOptions) As BitmapDecoder

        Function CreateDecoderFromFileHandle(<[In]> ByVal hFile As IntPtr, <[In]> ByRef pguidVendor As Guid, <[In]> ByVal metadataOptions As DecodeOptions) As BitmapDecoder

        Function CreateComponentInfo(<[In]> ByRef clsidComponent As Guid) As Object

        Function CreateDecoder(<[In]> ByRef guidContainerFormat As Guid, <[In]> ByRef pguidVendor As Guid) As BitmapDecoder

        Function CreateEncoder(<[In]> ByRef guidContainerFormat As Guid, <[In]> ByRef pguidVendor As Guid) As Object

        Function CreatePalette() As Palette

        Function CreateFormatConverter() As Object 'As FormatConverter)

        Function CreateBitmapScaler() As Object ' As BitmapScaler

        Function CreateBitmapClipper() As Object ' ByRef ppIBitmapClipper As IWICBitmapClipper)

        Function CreateBitmapFlipRotator() As Object '<> ByRef ppIBitmapFlipRotator As IWICBitmapFlipRotator)

        Function CreateStream() As WICStream

        'Function CreateColorContext(<> ByRef ppIWICColorContext As IWICColorContext)

        'Function CreateColorTransformer(<> ByRef ppIWICColorTransform As IWICColorTransform)

        'Sub CreateBitmap(<[In]> ByVal uiWidth As UInt32, <[In]> ByVal uiHeight As UInt32, <[In], ComAliasName("WIC.REFWICPixelFormatGUID")> ByRef pixelFormat As Guid, <[In]> ByVal [option] As WICBitmapCreateCacheOption, <Out, MarshalAs(UnmanagedType.Interface)> ByRef ppIBitmap As IWICBitmap)

        'Sub CreateBitmapFromSource(<[In], MarshalAs(UnmanagedType.Interface)> ByVal pIBitmapSource As IWICBitmapSource, <[In]> ByVal [option] As WICBitmapCreateCacheOption, <Out, MarshalAs(UnmanagedType.Interface)> ByRef ppIBitmap As IWICBitmap)

        'Sub CreateBitmapFromSourceRect(<[In], MarshalAs(UnmanagedType.Interface)> ByVal pIBitmapSource As IWICBitmapSource, <[In]> ByVal X As UInt32, <[In]> ByVal Y As UInt32, <[In]> ByVal Width As UInt32, <[In]> ByVal Height As UInt32, <Out, MarshalAs(UnmanagedType.Interface)> ByRef ppIBitmap As IWICBitmap)

        'Sub CreateBitmapFromMemory(<[In]> ByVal uiWidth As UInt32, <[In]> ByVal uiHeight As UInt32, <[In], ComAliasName("WIC.REFWICPixelFormatGUID")> ByRef pixelFormat As Guid, <[In]> ByVal cbStride As UInt32, <[In]> ByVal cbBufferSize As UInt32, <[In]> ByRef pbBuffer As Byte, <Out, MarshalAs(UnmanagedType.Interface)> ByRef ppIBitmap As IWICBitmap)

        'Sub CreateBitmapFromHBITMAP(<[In], ComAliasName("WIC.wireHBITMAP")> ByRef hBitmap As _userHBITMAP, <[In], ComAliasName("WIC.wireHPALETTE")> ByRef hPalette As _userHPALETTE, <[In]> ByVal options As WICBitmapAlphaChannelOption, <Out, MarshalAs(UnmanagedType.Interface)> ByRef ppIBitmap As IWICBitmap)

        'Sub CreateBitmapFromHICON(<[In], ComAliasName("WIC.wireHICON")> ByRef hIcon As _RemotableHandle, <Out, MarshalAs(UnmanagedType.Interface)> ByRef ppIBitmap As IWICBitmap)

        'Sub CreateComponentEnumerator(<[In]> ByVal componentTypes As UInt32, <[In]> ByVal options As UInt32, <Out, MarshalAs(UnmanagedType.Interface)> ByRef ppIEnumUnknown As IEnumUnknown)

        'Sub CreateFastMetadataEncoderFromDecoder(<[In], MarshalAs(UnmanagedType.Interface)> ByVal pIDecoder As IWICBitmapDecoder, <Out, MarshalAs(UnmanagedType.Interface)> ByRef ppIFastEncoder As IWICFastMetadataEncoder)

        'Sub CreateFastMetadataEncoderFromFrameDecode(<[In], MarshalAs(UnmanagedType.Interface)> ByVal pIFrameDecoder As IWICBitmapFrameDecode, <Out, MarshalAs(UnmanagedType.Interface)> ByRef ppIFastEncoder As IWICFastMetadataEncoder)

        'Sub CreateQueryWriter(<[In]> ByRef guidMetadataFormat As Guid, <[In]> ByRef pguidVendor As Guid, <Out, MarshalAs(UnmanagedType.Interface)> ByRef ppIQueryWriter As IWICMetadataQueryWriter)

        'Sub CreateQueryWriterFromReader(<[In], MarshalAs(UnmanagedType.Interface)> ByVal pIQueryReader As IWICMetadataQueryReader, <[In]> ByRef pguidVendor As Guid, <Out, MarshalAs(UnmanagedType.Interface)> ByRef ppIQueryWriter As IWICMetadataQueryWriter)
    End Interface

    Class FactoryFactory


        Public Shared Function CreateFactory() As Factory

            Dim wic_factory_type As Type = Type.GetTypeFromCLSID(CLSID_WICImagingFactory)

            Dim rv As Factory = CType(Activator.CreateInstance(wic_factory_type), Factory)
            Return rv
        End Function
    End Class
End Namespace
