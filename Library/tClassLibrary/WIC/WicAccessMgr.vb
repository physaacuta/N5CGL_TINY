'*********************************************************************************
' Win7系(win2008) において、GDI+の不具合から8bppIndex画像が階調落ちしてしまう対策
'*********************************************************************************

Imports System.Drawing

Namespace tClass
    Public Class WicAccessMgr

        Private Shared m_bWicAccess As Boolean = False			' true:必要(win7系)  false:従来通り

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 公開

        ''' <summary>
        ''' WICからBitmapクラスを作る必要があるかチェック
        ''' </summary>
        ''' <returns>true:必要(win7系)  false:従来通り</returns>
        Public Shared Function CheckWicAccess() As Boolean
            Return m_bWicAccess
        End Function


        ''' <summary>
        ''' JpegバイナリデータからBitmapクラス生成
        ''' </summary>
        ''' <param name="dataJpeg">バイナリ配列</param>
        ''' <returns></returns>
        Public Shared Function DataToImageWic(ByVal dataJpeg() As Byte) As Drawing.Bitmap
            Return DataToImageWic(dataJpeg, dataJpeg.Length())
        End Function
        Public Shared Function DataToImageWic(ByVal dataJpeg() As Byte, ByVal nSize As Integer) As Drawing.Bitmap
            'WIC　ファクトリの作成
            Dim factory As Wic.Factory = Wic.FactoryFactory.CreateFactory()
            Dim wics As Wic.WICStream = factory.CreateStream()
            wics.InitializeFromMemory(dataJpeg, nSize)

            ' 処理
            Dim gdibmp As Bitmap = CreateImaging(factory, wics)

            'WIC COMオブジェクトの解放
            System.Runtime.InteropServices.Marshal.ReleaseComObject(wics)
            System.Runtime.InteropServices.Marshal.ReleaseComObject(factory)
            Return gdibmp
        End Function

        ''' <summary>
        ''' 画像ファイルからBitmapクラス生成
        ''' </summary>
        ''' <param name="strFPath">画像パス</param>
        ''' <returns></returns>
        Public Shared Function ImgFileLoadWic(ByVal strFPath As String) As Bitmap
            'WIC　ファクトリの作成
            Dim factory As Wic.Factory = Wic.FactoryFactory.CreateFactory()
            Dim wics As Wic.WICStream = factory.CreateStream()
            wics.InitializeFromFilename(strFPath, Wic.Generic_Read)

            ' 処理
            Dim gdibmp As Bitmap = CreateImaging(factory, wics)

            'WIC COMオブジェクトの解放
            System.Runtime.InteropServices.Marshal.ReleaseComObject(wics)
            System.Runtime.InteropServices.Marshal.ReleaseComObject(factory)
            Return gdibmp
        End Function




        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ メイン

        ''' <summary>
        ''' 唯一のコンストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Shared Sub New()
            ' NT *.* のカーネルバージョを取得
            Dim nMajor As Integer = System.Environment.OSVersion.Version.Major
            Dim nMinor As Integer = System.Environment.OSVersion.Version.Minor

            ' NT5.1 : WinXP					OK。既存
            ' NT6.0 : Vista, Win 2008		NG。WIC使用
            ' NT6.1 : Win7, Win 2008 R2		NG。WIC使用
            ' NT6.2 : Win8					OK。既存
            If 6 = nMajor AndAlso (0 = nMinor Or 1 = nMinor) Then
                m_bWicAccess = True
            Else
                m_bWicAccess = False
            End If
        End Sub


        ''' <summary>
        ''' WICのpixelフォーマットに対応する、GDI+のPIXELフォーマットをかえす
        ''' </summary>
        Public Shared Function Wic2GDIPixelFormat(ByVal wicPixelFormat As Guid) As System.Drawing.Imaging.PixelFormat
            Select Case wicPixelFormat
                Case Wic.PixelFormat.GUID_1bppIndexed, Wic.PixelFormat.GUID_BlackWhite
                    Return System.Drawing.Imaging.PixelFormat.Format1bppIndexed

                Case Wic.PixelFormat.GUID_4bppIndexed, Wic.PixelFormat.GUID_4bppGray
                    Return System.Drawing.Imaging.PixelFormat.Format4bppIndexed

                Case Wic.PixelFormat.GUID_8bppIndexed, Wic.PixelFormat.GUID_8bppGray
                    Return System.Drawing.Imaging.PixelFormat.Format8bppIndexed

                Case Wic.PixelFormat.GUID_32bppBGRA, Wic.PixelFormat.GUID_32bppRGBA, Wic.PixelFormat.GUID_32bppBGR, Wic.PixelFormat.GUID_32bppRGBA
                    Return System.Drawing.Imaging.PixelFormat.Format32bppArgb

                Case Else   '大胆なELSE。上以外はみんな24bitカラーだ
                    Return System.Drawing.Imaging.PixelFormat.Format24bppRgb
            End Select
        End Function


        ''' <summary>
        ''' WICからの変換
        ''' </summary>
        ''' <param name="factory"></param>
        ''' <param name="wics"></param>
        Protected Friend Shared Function CreateImaging(ByVal factory As Wic.Factory, ByVal wics As Wic.WICStream) As Drawing.Bitmap

            'デコーダの作成
            Dim decoder As Wic.BitmapDecoder = factory.CreateDecoderFromStream(wics, Nothing, Wic.DecodeOptions.MetadataCacheOnDemand)
            Dim frame As Wic.BitmapFrameDecode = decoder.Frame(0)

            '画像の条件確認。
            Dim width As Integer, height As Integer
            frame.Size(width, height)

            Dim PixelFormat As Guid = frame.PixelFormat()

            '返却ビットマップの作成
            Dim gdibmp As Bitmap = New System.Drawing.Bitmap(width, height, Wic2GDIPixelFormat(PixelFormat))

            'とりあえず画像を返したいので、フォーマットにかかわらずコピーしてみる。
            Using bd As New LockingBitmap(gdibmp, Imaging.ImageLockMode.WriteOnly)
                frame.CopyPixels(Nothing, bd.Stride, bd.Stride * height, bd.PtrAt())
            End Using


            Select Case PixelFormat
                Case Wic.PixelFormat.GUID_1bppIndexed, Wic.PixelFormat.GUID_4bppIndexed, Wic.PixelFormat.GUID_8bppIndexed
                    'WICから提供されるパレットをコピーする。
                    Dim wicPalette As Wic.Palette = factory.CreatePalette
                    frame.CopyPalette(wicPalette)
                    Dim palette_count As Integer = wicPalette.Count
                    Dim colors(palette_count - 1) As Integer
                    Dim color_count As Integer = wicPalette.Read(palette_count, colors)

                    Dim bmpPalette As System.Drawing.Imaging.ColorPalette = gdibmp.Palette
                    For ii As Integer = 0 To color_count - 1
                        bmpPalette.Entries(ii) = Color.FromArgb(colors(ii))
                    Next
                    gdibmp.Palette = bmpPalette

                    System.Runtime.InteropServices.Marshal.ReleaseComObject(wicPalette)

                Case Wic.PixelFormat.GUID_8bppGray, Wic.PixelFormat.GUID_4bppGray, Wic.PixelFormat.GUID_BlackWhite
                    'WICからパレットは提供されないので、ここで作製しとく
                    tClass_BmpJpeg.SetGrayPalette(gdibmp)
            End Select


            'WIC COMオブジェクトの解放
            System.Runtime.InteropServices.Marshal.ReleaseComObject(frame)
            System.Runtime.InteropServices.Marshal.ReleaseComObject(decoder)
            Return gdibmp
        End Function
    End Class
End Namespace