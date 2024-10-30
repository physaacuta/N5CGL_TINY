Imports System.Drawing
Imports System.Drawing.Imaging
Imports System.Runtime.InteropServices
Imports tClassLibrary
Imports tClassLibrary.tClass

''' <summary>
''' 縮小共通インタフェース
''' </summary>
''' <remarks></remarks>
Public Interface IImageResize
    Property Quality() As Integer
    Function ImgReSize(bmp As Bitmap, ByVal szNew As Size, ByRef resizeMethod As Boolean) As Bitmap
End Interface

''' <summary>
''' IPPによる縮小
''' </summary>
''' <remarks></remarks>
Public Class tClass_ImgResize_IPP
    Implements IImageResize

    Const mcMaxSize_X As Integer = 1024 * 6
    Const mcMaxSize_Y As Integer = 1024 * 6
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ダイナミックリンク  (IPPを使っている)
    '/ /////////////////////////////////////////////////////////////////////////////////

    Public Enum ResizeMethod
        ippNN = 1
        ippLinear = 2
        ippCubic = 4
        ippSuper = 8
        ippR = 16
    End Enum
    '            Private m_Resizer As New ImgLibCLI.IppResize                       ' IPP縮小ライブラリ（スレッド別）
    Private Shared ms_LoaderLockIpp As New Object                                   ' クリティカルセクション
    Private Shared mscls_pIppResize As Object                                       ' IPPインスタンス
    Private Shared msmod_pResize_8u_C1R As System.Reflection.MethodInfo             ' IPP-縮小実行モジュール  8bpp
    Private Shared msmod_pResize_8u_C3R As System.Reflection.MethodInfo             ' IPP-縮小実行モジュール 24bpp
    Private Shared msmod_pResize_8u_C4R As System.Reflection.MethodInfo             ' IPP-縮小実行モジュール 32bpp


    Private m_ResizeMethod As ResizeMethod = ResizeMethod.ippNN
    Public Property Quality As Integer Implements IImageResize.Quality
        Get
            Return m_ResizeMethod
        End Get
        Set(value As Integer)
            If 0 = value Then
                m_ResizeMethod = ResizeMethod.ippNN
            Else
                m_ResizeMethod = ResizeMethod.ippSuper
            End If

        End Set
    End Property
    Public Property Resize As ResizeMethod
        Get
            Return m_ResizeMethod
        End Get
        Set(value As ResizeMethod)
            m_ResizeMethod = value
        End Set
    End Property
    Public Sub New()
        Load_ippResize()
    End Sub

    ''' <summary>
    ''' DLLロード
    ''' </summary>
    ''' <remarks></remarks>
    Private Shared Sub Load_ippResize()
        SyncLock (ms_LoaderLockIpp)
            If msmod_pResize_8u_C1R Is Nothing Then
                's_dllImgLibCLI = System.Reflection.Assembly.Load("ImgLibCLI")
                's_clsIppResize = s_dllImgLibCLI.GetType("ImgLibCLI.IppResize")
                's_pResize_8u_C1R = s_clsIppResize.GetMethod("Resize_8u_C1R")

                ' アセンブリ読込み
                Dim s_dllImgLibCLI As System.Reflection.Assembly = System.Reflection.Assembly.Load("ImgLibCLI")
                Dim s_clsIppResize As System.Type = s_dllImgLibCLI.GetType("ImgLibCLI.IppResize")
                mscls_pIppResize = System.Activator.CreateInstance(s_clsIppResize)

                msmod_pResize_8u_C1R = s_clsIppResize.GetMethod("Resize_8u_C1R")
                msmod_pResize_8u_C3R = s_clsIppResize.GetMethod("Resize_8u_C3R")
                msmod_pResize_8u_C4R = s_clsIppResize.GetMethod("Resize_8u_C4R")

            End If
        End SyncLock
    End Sub


    Public Function ippResize( _
    ByVal SrcPtr As IntPtr, ByVal SrcStride As Integer, ByVal SrcWidth As Integer, ByVal SrcHeight As Integer, _
    ByVal DstPtr As IntPtr, ByVal DstStride As Integer, ByVal DstWidth As Integer, ByVal DstHeight As Integer, ByVal bpp As Imaging.PixelFormat) As Integer
        Dim ippResizeArgs() As Object = New Object() { _
                              SrcPtr, SrcStride, SrcWidth, SrcHeight, _
                              DstPtr, DstStride, DstWidth, DstHeight, _
                              m_ResizeMethod}
        '縮小ルーチンの呼び出し
        Select Case bpp
            Case PixelFormat.Format8bppIndexed
                Return CInt(msmod_pResize_8u_C1R.Invoke(mscls_pIppResize, ippResizeArgs))
            Case PixelFormat.Format24bppRgb
                Return CInt(msmod_pResize_8u_C3R.Invoke(mscls_pIppResize, ippResizeArgs))
            Case PixelFormat.Format32bppArgb, PixelFormat.Format32bppPArgb, PixelFormat.Format32bppRgb
                Return CInt(msmod_pResize_8u_C4R.Invoke(mscls_pIppResize, ippResizeArgs))
            Case Else
                'どうしよう..
                Return -19
        End Select
    End Function

    ''' <summary>
    ''' 画像縮小（ＩＰＰ使用）
    ''' </summary>
    Private Function ImgReSize_IPP(ByVal bmp As Bitmap, ByVal szNew As Size, ByRef bImgDspSizeLimit As Boolean) As Bitmap Implements IImageResize.ImgReSize
        Dim srcPixelFormat As PixelFormat = bmp.PixelFormat
        '===================================================
        ' 前準備


        ' 元サイズよりも大きくなるのはNG
        If szNew.Width > bmp.Width Or szNew.Height > bmp.Height Then
            If szNew.Width > bmp.Width Then szNew.Width = bmp.Width
            If szNew.Height > bmp.Height Then szNew.Height = bmp.Height

            bImgDspSizeLimit = True
            Return Nothing
        End If


        ' 画像生成サイズチェック
        szNew.Width = szNew.Width
        szNew.Height = szNew.Height
        If szNew.Width > mcMaxSize_X Or szNew.Height > mcMaxSize_Y Then
            If szNew.Width > mcMaxSize_X Then szNew.Width = mcMaxSize_X
            If szNew.Height > mcMaxSize_Y Then szNew.Height = mcMaxSize_Y

            bImgDspSizeLimit = True
            Return Nothing
        Else
            bImgDspSizeLimit = False
        End If


        '===================================================

        ' 画面表示用Bitmapの作成

        Dim dspbmp As New Bitmap(szNew.Width, szNew.Height, srcPixelFormat)

        'パレットはある場合コピー
        If (srcPixelFormat And PixelFormat.Indexed) > 0 Then
            dspbmp.Palette = bmp.Palette
        End If

        Dim rc As Integer
        'Bitmapロック(Dst = 縮小画像、Src = 生フレームファイル)。
        Using bdDst As New LockingBitmap(dspbmp, ImageLockMode.WriteOnly), _
                           bdSrc As New LockingBitmap(bmp, ImageLockMode.ReadOnly)

            rc = ippResize(bdSrc.PtrAt(0), bdSrc.Stride, bdSrc.Width, bdSrc.Height, _
                           bdDst.PtrAt(0), bdDst.Stride, bdDst.Width, bdDst.Height, srcPixelFormat)

        End Using
        Return dspbmp

    End Function

End Class


''' <summary>
''' API（GDI)による縮小
''' </summary>
''' <remarks></remarks>
Public Class tClass_ImgResize_API
    Implements IImageResize

    Const mcMaxSize_X As Integer = 1024 * 6
    Const mcMaxSize_Y As Integer = 1024 * 6

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ API
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public Enum TernaryRasterOperations
        SRCCOPY = &HCC0020 ', /* dest = source*/			' これしか使わないはず
    End Enum

    Private Const COLORONCOLOR As Integer = 3   ' 低画質
    Private Const COLORONCOLOR_H As Integer = 4 ' 高画質


    Private m_nQuarity As Integer
    Public Property Quality As Integer Implements IImageResize.Quality
        Get
            Return m_nQuarity
        End Get
        Set(value As Integer)
            If 0 = value Then
                m_nQuarity = COLORONCOLOR
            Else
                m_nQuarity = COLORONCOLOR_H
            End If

        End Set
    End Property

    <Runtime.InteropServices.DllImport("gdi32.dll", SetLastError:=True, CallingConvention:=CallingConvention.Winapi)> _
    Private Shared Function StretchDIBits(ByVal hdcDest As IntPtr, _
        ByVal XDest As Integer, ByVal YDest As Integer, _
        ByVal nDestWidth As Integer, ByVal nDestHeight As Integer, _
        ByVal XSrc As Integer, ByVal YSrc As Integer, _
        ByVal nSrcWidth As Integer, ByVal nSrcHeight As Integer, _
        ByVal lpBits As IntPtr, _
        ByRef lpBitsInfo As tClass_BmpJpeg.BITMAPINFO, _
        ByVal iUsage As Integer, _
        ByVal dwRop As TernaryRasterOperations) As Integer
    End Function


    <Runtime.InteropServices.DllImport("gdi32.dll", SetLastError:=True, CallingConvention:=CallingConvention.Winapi)> _
    Private Shared Function SetStretchBltMode(ByVal hdc As IntPtr, ByVal nStretchMode As Integer) As IntPtr
    End Function

    ''' <summary>
    ''' 指定されたpixelformatが何ビットかをかえす。
    ''' </summary>
    Private Shared Function GetPixelbit(ByVal pixelformat As PixelFormat) As Integer
        Select Case pixelformat
            Case pixelformat.Format8bppIndexed
                Return 8
            Case pixelformat.Format24bppRgb
                Return 24
            Case pixelformat.Format32bppArgb, pixelformat.Format32bppPArgb, pixelformat.Format32bppRgb
                Return 32
            Case pixelformat.Format48bppRgb
                Return 48
            Case pixelformat.Format64bppArgb, pixelformat.Format64bppPArgb
                Return 64
            Case Else
                'どうしよう..
                Return 0
        End Select

    End Function



    ''' <summary>
    ''' 画像縮小 (API)
    ''' </summary>
    ''' <remarks></remarks>
    Private Function ImgReSize_API(ByVal bmp As Bitmap, ByVal szNew As Size, ByRef bImgDspSizeLimit As Boolean) As Bitmap Implements IImageResize.ImgReSize

        Dim srcPixelFormat As PixelFormat = bmp.PixelFormat
        '===================================================
        ' 元画像のビットマップヘッダー生成
        Dim bmpInfo As New tClass_BmpJpeg.BITMAPINFO
        With bmpInfo.bmiHeader
            If srcPixelFormat = PixelFormat.Format8bppIndexed Then
                .biSize = 40                                                ' ヘッダサイズ(40固定)
                .biWidth = bmp.Size.Width                                         ' イメージの横幅(ピクセル単位)
                .biHeight = IIf(False, bmp.Size.Height, -bmp.Size.Height)           ' イメージの高さ(ピクセル単位) (Windowsは画像が上下反転の為) (△反転)
                .biPlanes = 1                                               ' プレーン数(1固定)
                .biBitCount = 8                                     ' １ピクセル当たりのビット数(1,4,8,24)
                .biCompression = 0                                        ' 圧縮形式(BI_RGB:0->非圧縮, BI_RLE8:1->RLE圧縮)
                .biSizeImage = CUInt(bmp.Size.Width * bmp.Size.Height * .biBitCount \ .biBitCount)  ' 圧縮後のイメージサイズ(バイト単位)
                .biClrUsed = 256                                                ' 格納パレット数(使用色数) (0固定)
                .biClrImportant = 0                                         ' 重要色数 (0固定)

                '''' ガンマ補正対応 (入力画像のカラーパレットから色データを取得)
                ReDim bmpInfo.bmiColors(255)
                For jj As Integer = 0 To 255
                    bmpInfo.bmiColors(jj).rgbBlue = bmp.Palette.Entries(jj).B
                    bmpInfo.bmiColors(jj).rgbGreen = bmp.Palette.Entries(jj).G
                    bmpInfo.bmiColors(jj).rgbRed = bmp.Palette.Entries(jj).R
                    bmpInfo.bmiColors(jj).rgbReserved = bmp.Palette.Entries(jj).A
                Next
            Else
                .biSize = 40                                                ' ヘッダサイズ(40固定)
                .biWidth = bmp.Size.Width                                         ' イメージの横幅(ピクセル単位)
                .biHeight = CInt(IIf(False, bmp.Size.Height, -bmp.Size.Height))         ' イメージの高さ(ピクセル単位) (Windowsは画像が上下反転の為) (△反転)
                .biPlanes = 1                                              ' プレーン数(1固定)
                .biBitCount = CUShort(GetPixelbit(srcPixelFormat))                                  ' １ピクセル当たりのビット数(1,4,8,24)
                .biCompression = 0                                        ' 圧縮形式(BI_RGB:0->非圧縮, BI_RLE8:1->RLE圧縮)
                .biSizeImage = CUInt(bmp.Size.Width * bmp.Size.Height * .biBitCount \ .biBitCount)  ' 圧縮後のイメージサイズ(バイト単位)
                .biClrUsed = 0                                              ' 格納パレット数(使用色数) (0固定)
                .biClrImportant = 0                                         ' 重要色数 (0固定)

            End If
        End With


        '===================================================
        ' 前準備

        ' 元サイズよりも大きくなるのはNG
        If szNew.Width > bmp.Width Or szNew.Height > bmp.Height Then
            If szNew.Width > bmp.Width Then szNew.Width = bmp.Width
            If szNew.Height > bmp.Height Then szNew.Height = bmp.Height

            bImgDspSizeLimit = True
            Return Nothing
        End If

        ' 画像生成サイズチェック
        If szNew.Width > mcMaxSize_X Or szNew.Height > mcMaxSize_Y Then
            If szNew.Width > mcMaxSize_X Then szNew.Width = mcMaxSize_X
            If szNew.Height > mcMaxSize_Y Then szNew.Height = mcMaxSize_Y

            bImgDspSizeLimit = True
        Else
            bImgDspSizeLimit = False
        End If


        '===================================================
        ' 表示画像生成()
        Dim smallbmp As New Bitmap(szNew.Width, szNew.Height, Imaging.PixelFormat.Format24bppRgb)    ' 画面表示用

        Dim rec As New Drawing.Rectangle(0, 0, bmp.Width, bmp.Height)

        Using g As Graphics = Graphics.FromImage(smallbmp)
            ' GDI 用の DIB ポインタを取得
            Dim hDst As IntPtr = g.GetHdc()

            ' 元画像のデータ部ポインタを取得
            Using bdSrc As New LockingBitmap(bmp, ImageLockMode.ReadOnly)

                ' コピー先の画質指定
                SetStretchBltMode(hDst, m_nQuarity)


                ' 変換
                Dim ret As Integer = StretchDIBits(hDst, _
                      0, 0, szNew.Width, szNew.Height, _
                      0, 0, bmp.Width, bmp.Height, _
                      bdSrc.PtrAt(0), bmpInfo, _
                      0, TernaryRasterOperations.SRCCOPY)

            End Using
            ' 作ったものの開放
            g.ReleaseHdc()
        End Using
        Return smallbmp
    End Function
End Class
