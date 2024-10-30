Imports System.Runtime.InteropServices

Namespace Wic

    Public Class PixelFormat
        Public Shared ReadOnly GUID_DontCare As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H00)
        Public Shared ReadOnly GUID_1bppIndexed As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H01)
        Public Shared ReadOnly GUID_2bppIndexed As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H02)
        Public Shared ReadOnly GUID_4bppIndexed As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H03)
        Public Shared ReadOnly GUID_8bppIndexed As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H04)
        Public Shared ReadOnly GUID_BlackWhite As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H05)
        Public Shared ReadOnly GUID_2bppGray As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H06)
        Public Shared ReadOnly GUID_4bppGray As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H07)
        Public Shared ReadOnly GUID_8bppGray As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H08)
        Public Shared ReadOnly GUID_8bppAlpha As New Guid(&HE6CD0116&, &HEEBA, &H4161, &HAA, &H85, &H27, &HDD, &H9F, &HB3, &HA8, &H95)
        Public Shared ReadOnly GUID_16bppBGR555 As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H09)
        Public Shared ReadOnly GUID_16bppBGR565 As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H0A)
        Public Shared ReadOnly GUID_16bppBGRA5551 As New Guid(&H05EC7C2B, &HF1E6, &H4961, &HAD, &H46, &HE1, &HCC, &H81, &H0A, &H87, &HD2)
        Public Shared ReadOnly GUID_16bppGray As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H0B)
        Public Shared ReadOnly GUID_24bppBGR As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H0C)
        Public Shared ReadOnly GUID_24bppRGB As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H0D)
        Public Shared ReadOnly GUID_32bppBGR As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H0E)
        Public Shared ReadOnly GUID_32bppBGRA As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H0F)
        Public Shared ReadOnly GUID_32bppPBGRA As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H10)
        Public Shared ReadOnly GUID_32bppGrayFloat As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H11)
        Public Shared ReadOnly GUID_32bppRGBA As New Guid(&HF5C7AD2D, &H6A8D, &H43DD, &HA7, &HA8, &HA2, &H99, &H35, &H26, &H1A, &HE9)
        Public Shared ReadOnly GUID_32bppPRGBA As New Guid(&H3CC4A650, &HA527, &H4D37, &HA9, &H16, &H31, &H42, &HC7, &HEB, &HED, &HBA)
        'Public Shared ReadOnly GUID_48bppRGB As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H15)
        'Public Shared ReadOnly GUID_48bppBGR As New Guid(&HE605A384&, &HB468, &H46CE, &HBB, &H2E, &H36, &HF1, &H80, &HE6, &H43, &H13)
        'Public Shared ReadOnly GUID_64bppRGBA As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H16)
        'Public Shared ReadOnly GUID_64bppBGRA As New Guid(&H1562FF7C, &HD352, &H46F9, &H97, &H9E, &H42, &H97, &H6B, &H79, &H22, &H46)
        'Public Shared ReadOnly GUID_64bppPRGBA As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H17)
        'Public Shared ReadOnly GUID_64bppPBGRA As New Guid(&H8C518E8E&, &HA4EC, &H468B, &HAE, &H70, &HC9, &HA3, &H5A, &H9C, &H55, &H30)
        'Public Shared ReadOnly GUID_16bppGrayFixedPoint As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H13)
        'Public Shared ReadOnly GUID_32bppBGR101010 As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H14)
        'Public Shared ReadOnly GUID_48bppRGBFixedPoint As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H12)
        'Public Shared ReadOnly GUID_48bppBGRFixedPoint As New Guid(&H49CA140E, &HCAB6, &H493B, &H9D, &HDF, &H60, &H18, &H7C, &H37, &H53, &H2A)
        'Public Shared ReadOnly GUID_96bppRGBFixedPoint As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H18)
        'Public Shared ReadOnly GUID_128bppRGBAFloat As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H19)
        'Public Shared ReadOnly GUID_128bppPRGBAFloat As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H1A)
        'Public Shared ReadOnly GUID_128bppRGBFloat As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H1B)
        'Public Shared ReadOnly GUID_32bppCMYK As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H1C)
        'Public Shared ReadOnly GUID_64bppRGBAFixedPoint As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H1D)
        'Public Shared ReadOnly GUID_64bppBGRAFixedPoint As New Guid(&H356DE33C, &H54D2, &H4A23, &HBB, &H4, &H9B, &H7B, &HF9, &HB1, &HD4, &H2D)
        'Public Shared ReadOnly GUID_64bppRGBFixedPoint As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H40)
        'Public Shared ReadOnly GUID_128bppRGBAFixedPoint As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H1E)
        'Public Shared ReadOnly GUID_128bppRGBFixedPoint As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H41)
        'Public Shared ReadOnly GUID_64bppRGBAHalf As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H3A)
        'Public Shared ReadOnly GUID_64bppRGBHalf As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H42)
        'Public Shared ReadOnly GUID_48bppRGBHalf As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H3B)
        'Public Shared ReadOnly GUID_32bppRGBE As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H3D)
        'Public Shared ReadOnly GUID_16bppGrayHalf As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H3E)
        'Public Shared ReadOnly GUID_32bppGrayFixedPoint As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H3F)
        'Public Shared ReadOnly GUID_32bppRGBA1010102 As New Guid(&H25238D72, &HFCF9, &H4522, &HB5, &H14, &H55, &H78, &HE5, &HAD, &H55, &HE0)
        'Public Shared ReadOnly GUID_32bppRGBA1010102XR As New Guid(&H00DE6B9A, &HC101, &H434B, &HB5, &H02, &HD0, &H16, &H5E, &HE1, &H12, &H2C)
        'Public Shared ReadOnly GUID_64bppCMYK As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H1F)
        'Public Shared ReadOnly GUID_24bpp3Channels As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H20)
        'Public Shared ReadOnly GUID_32bpp4Channels As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H21)
        'Public Shared ReadOnly GUID_40bpp5Channels As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H22)
        'Public Shared ReadOnly GUID_48bpp6Channels As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H23)
        'Public Shared ReadOnly GUID_56bpp7Channels As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H24)
        'Public Shared ReadOnly GUID_64bpp8Channels As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H25)
        'Public Shared ReadOnly GUID_48bpp3Channels As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H26)
        'Public Shared ReadOnly GUID_64bpp4Channels As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H27)
        'Public Shared ReadOnly GUID_80bpp5Channels As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H28)
        'Public Shared ReadOnly GUID_96bpp6Channels As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H29)
        'Public Shared ReadOnly GUID_112bpp7Channels As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H2A)
        'Public Shared ReadOnly GUID_128bpp8Channels As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H2B)
        'Public Shared ReadOnly GUID_40bppCMYKAlpha As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H2C)
        'Public Shared ReadOnly GUID_80bppCMYKAlpha As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H2D)
        'Public Shared ReadOnly GUID_32bpp3ChannelsAlpha As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H2E)
        'Public Shared ReadOnly GUID_40bpp4ChannelsAlpha As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H2F)
        'Public Shared ReadOnly GUID_48bpp5ChannelsAlpha As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H30)
        'Public Shared ReadOnly GUID_56bpp6ChannelsAlpha As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H31)
        'Public Shared ReadOnly GUID_64bpp7ChannelsAlpha As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H32)
        'Public Shared ReadOnly GUID_72bpp8ChannelsAlpha As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H33)
        'Public Shared ReadOnly GUID_64bpp3ChannelsAlpha As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H34)
        'Public Shared ReadOnly GUID_80bpp4ChannelsAlpha As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H35)
        'Public Shared ReadOnly GUID_96bpp5ChannelsAlpha As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H36)
        'Public Shared ReadOnly GUID_112bpp6ChannelsAlpha As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H37)
        'Public Shared ReadOnly GUID_128bpp7ChannelsAlpha As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H38)
        'Public Shared ReadOnly GUID_144bpp8ChannelsAlpha As New Guid(&H6FDDC324, &H4E03, &H4BFE, &HB1, &H85, &H3D, &H77, &H76, &H8D, &HC9, &H39)

    End Class

    Public Class ContainerFormat
        Public Shared ReadOnly GUID_ContainerFormatBmp As New Guid(&HAF1D87E, &HFCFE, &H4188, &HBD, &HEB, &HA7, &H90, &H64, &H71, &HCB, &HE3)
        Public Shared ReadOnly GUID_ContainerFormatPng As New Guid(&H1B7CFAF4, &H713F, &H473C, &HBB, &HCD, &H61, &H37, &H42, &H5F, &HAE, &HAF)
        Public Shared ReadOnly GUID_ContainerFormatIco As New Guid(&HA3A860C4, &H338F, &H4C17, &H91, &H9A, &HFB, &HA4, &HB5, &H62, &H8F, &H21)
        Public Shared ReadOnly GUID_ContainerFormatJpeg As New Guid(&H19E4A5AA, &H5662, &H4FC5, &HA0, &HC0, &H17, &H58, &H2, &H8E, &H10, &H57)
        Public Shared ReadOnly GUID_ContainerFormatTiff As New Guid(&H163BCC30, &HE2E9, &H4F0B, &H96, &H1D, &HA3, &HE9, &HFD, &HB7, &H88, &HA3)
        Public Shared ReadOnly GUID_ContainerFormatGif As New Guid(&H1F8A5601, &H7D4D, &H4CBD, &H9C, &H82, &H1B, &HC8, &HD4, &HEE, &HB9, &HA5)
        Public Shared ReadOnly GUID_ContainerFormatWmp As New Guid(&H57A37CAA, &H367A, &H4540, &H91, &H6B, &HF1, &H83, &HC5, &H9, &H3A, &H4B)
    End Class


    Public Enum BitmapPaletteType
        ' Fields
        WICBITMAPPALETTETYPE_FORCE_DWORD = &H7FFFFFFF
        WICBitmapPaletteTypeCustom = 0
        WICBitmapPaletteTypeFixedBW = 2
        WICBitmapPaletteTypeFixedGray16 = 11
        WICBitmapPaletteTypeFixedGray256 = 12
        WICBitmapPaletteTypeFixedGray4 = 10
        WICBitmapPaletteTypeFixedHalftone125 = 6
        WICBitmapPaletteTypeFixedHalftone216 = 7
        WICBitmapPaletteTypeFixedHalftone252 = 8
        WICBitmapPaletteTypeFixedHalftone256 = 9
        WICBitmapPaletteTypeFixedHalftone27 = 4
        WICBitmapPaletteTypeFixedHalftone64 = 5
        WICBitmapPaletteTypeFixedHalftone8 = 3
        WICBitmapPaletteTypeFixedWebPalette = 7
        WICBitmapPaletteTypeMedianCut = 1
    End Enum

    <ComImport(), Guid("00000040-A8F2-4877-BA0A-FD2B6645FB94"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)> _
    Public Interface Palette

        Sub InitializePredefined(<[In]()> ByVal ePaletteType As BitmapPaletteType, <[In]()> ByVal fAddTransparentColor As Integer)

        Sub InitializeCustom(<[In]()> ByRef pColors As UInt32, <[In]()> ByVal cCount As UInt32)

        Sub InitializeFromBitmap(<[In]()> ByVal pISurface As BitmapSource, <[In]()> ByVal cCount As UInt32, <[In]()> ByVal fAddTransparentColor As Integer)

        Sub InitializeFromPalette(<[In]()> ByVal pIPalette As Palette)

        Function PaletteType() As BitmapPaletteType

        Function Count() As Integer

        Function Read(<[In]()> ByVal cCount As Integer, <Out(), MarshalAs(UnmanagedType.LPArray, SizeParamIndex:=0)> ByVal Colors() As Integer) As Integer

        Function IsBlackWhite() As Integer

        Function IsGrayscale() As Integer

        Function HasAlpha() As Integer
    End Interface
End Namespace
