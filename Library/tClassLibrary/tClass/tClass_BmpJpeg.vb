'*********************************************************************************
'BitmapやJpegの画像データ変換クラス
'	[Ver]
'		Ver.01    2007/06/20  vs2005 対応
'
'	[メモ]
'		ある意味疵検専用？？
'*********************************************************************************
Option Strict On				'tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports System.IO
Imports System.Runtime.InteropServices
Imports System.Drawing
Imports System.Drawing.Imaging


Namespace tClass
    ''' <summary>
    ''' BitmapやJpegの画像データ変換クラス
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_BmpJpeg

        ''' <summary>ビットマップファイルヘッダ</summary>
        Public Const BITMAPFILEHEADER_SIZE As Integer = 14
        <StructLayout(LayoutKind.Sequential, Size:=BITMAPFILEHEADER_SIZE, pack:=1)> _
        Public Structure BITMAPFILEHEADER
            <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
            Dim bfType As Byte()												' ファイルタイプ
            Dim bfSize As UInteger												' ファイルサイズ [byte]
            Dim bfReserved1 As UShort											' 予約 (0固定)
            Dim bfReserved2 As UShort											' 予約 (0固定)
            Dim bfOffBits As UInteger											' ファイル先頭からデータ部までのオフセット [byte]

            ''' <summary>
            ''' 初期化
            ''' </summary>
            ''' <remarks></remarks>
            Public Sub initialize()
                ReDim bfType(2 - 1)
            End Sub
        End Structure

        ''' <summary>ビットマップ情報ヘッダ</summary>
        Public Const BITMAPINFOHEADER_SIZE As Integer = 40
        <StructLayout(LayoutKind.Sequential, Size:=BITMAPINFOHEADER_SIZE, pack:=1)> _
        Public Structure BITMAPINFOHEADER
            Dim biSize As UInteger												' 情報ヘッダサイズ [byte] (40固定)
            Dim biWidth As Integer												' 画像の幅 [pixel]
            Dim biHeight As Integer												' 画像の高さ [pixel]
            Dim biPlanes As UShort												' プレーン数 (1固定)
            Dim biBitCount As UShort											' 色ビット数 [bit]
            Dim biCompression As UInteger										' 圧縮形式
            Dim biSizeImage As UInteger											' 画像データサイズ [byte]
            Dim biXPelsPerMeter As Integer										' 水平解像度 [dot/m]
            Dim biYPelsPerMeter As Integer										' 垂直解像度 [dot/m]
            Dim biClrUsed As UInteger											' 格納パレット数(使用色数)
            Dim biClrImportant As UInteger										' 重要色数
        End Structure

        ''' <summary>カラーテーブル</summary>
        Public Const RGBQUAD_SIZE As Integer = 4
        <StructLayout(LayoutKind.Sequential, Size:=RGBQUAD_SIZE, pack:=1)> _
        Public Structure RGBQUAD
            Dim rgbBlue As Byte													' 青
            Dim rgbGreen As Byte												' 緑
            Dim rgbRed As Byte													' 赤
            Dim rgbReserved As Byte												' 予約
        End Structure

        ''' <summary>ビットマップ情報本体</summary>
        Public Const BITMAPINFO_SIZE As Integer = BITMAPINFOHEADER_SIZE + RGBQUAD_SIZE * 256
        <StructLayout(LayoutKind.Sequential, Size:=BITMAPINFO_SIZE, pack:=1)> _
        Public Structure BITMAPINFO
            Dim bmiHeader As BITMAPINFOHEADER
            <MarshalAs(UnmanagedType.ByValArray, SizeConst:=256)> _
            Dim bmiColors() As RGBQUAD
        End Structure

        '====================================================================================================
        ' 基本処理

        ''' <summary>
        ''' 通常の小さめの画像ファイル表示
        ''' </summary>
        ''' <param name="strFPath">フルパス</param>
        ''' <returns>imageオブジェクト</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function ImgFileLoad(ByVal strFPath As String) As Bitmap
            Try
                '' WIC対応
                If WicAccessMgr.CheckWicAccess Then
                    Return WicAccessMgr.ImgFileLoadWic(strFPath)
                End If


                Return New Bitmap(strFPath)
            Catch ex As Exception
                Return Nothing
            Finally
            End Try
        End Function


        ''' <summary>
        ''' Bitmapファイル高速表示 (画像ファイルが2,3Mとか巨大なとき用)
        ''' </summary>
        ''' <param name="strFPath">フルパス</param>
        ''' <param name="bCopyPalette">True:カラーパレットもコピー(たまにエラーになる画像がある)</param>
        ''' <returns>imageオブジェクト</returns>
        ''' <remarks>逐次ファイル読込でむちゃむちゃ表示を早くします。</remarks>
        Public Overloads Shared Function BmpFileLoad(ByVal strFPath As String, Optional ByVal bCopyPalette As Boolean = False) As Bitmap
            '' Ver1
            '         Dim stream As FileStream
            '         Dim img As Image
            '         stream = IO.File.OpenRead(strFPath)
            'img = Image.FromStream(stream, False, True)		   '第３引数がミソ。ファイル検証しない。 ＝ 必要な時に、逐次ファイルからデータを表示
            '         'ファイルクローズ処理を追加
            'stream.Close()

            '         Return img	' これだとなぜか、画像の反転とかすると落ちる場合がある


            '' Ver2
            'Dim stream As FileStream
            'Dim img As Image
            'stream = IO.File.OpenRead(strFPath)
            'img = Image.FromStream(stream, False, True)       '第３引数がミソ。ファイル検証しない。 ＝ 必要な時に、逐次ファイルからデータを表示
            ''ファイルクローズ処理を追加
            'stream.Close()



            'If Not bCopyPalette Then
            '    ' 通常はこっち。カラーパレットはコピーされない。
            '    Return New Bitmap(img)
            'Else
            '    ' とするとカラーパレットの情報を忘れてしまうようなので、対策 
            '    ' ガンマ補正とかするときは、こちら。
            '    ' ただし、よくわからないが、画像によっては、エラーになる
            '    Dim palet As Drawing.Imaging.ColorPalette = img.Palette
            '    Dim bitmapWk As New Bitmap(img)
            '    bitmapWk.Palette = palet
            '    Return bitmapWk
            'End If


            '' WIC対応         ※未検証
            Dim img As Image
            If WicAccessMgr.CheckWicAccess Then
                img = WicAccessMgr.ImgFileLoadWic(strFPath)
            Else
                Using stream As FileStream = IO.File.OpenRead(strFPath)
                    img = Bitmap.FromStream(stream, False, True)          '第３引数がミソ。ファイル検証しない。 ＝ 必要な時に、逐次ファイルからデータを表示
                    'ファイルクローズ処理を追加
                End Using
            End If


            If Not bCopyPalette Then
                ' 通常はこっち。カラーパレットはコピーされない。
                Return New Bitmap(img)
            Else
                ' とするとカラーパレットの情報を忘れてしまうようなので、対策 
                ' ガンマ補正とかするときは、こちら。
                ' ただし、よくわからないが、画像によっては、エラーになる
                Dim palet As Drawing.Imaging.ColorPalette = img.Palette
                Dim bitmapWk As New Bitmap(img)
                bitmapWk.Palette = palet
                Return bitmapWk
            End If

        End Function

        ''' <summary>
        ''' 色数から、ピクセル形式を返却するクラス
        ''' </summary>
        ''' <param name="nc">色数(2,16,256)</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Shared Function PixelFormatFromBpp(ByVal nc As Integer) As PixelFormat
            Dim pf As Imaging.PixelFormat
            If 2 = nc Then
                pf = Imaging.PixelFormat.Format1bppIndexed
            ElseIf 16 = nc Then
                pf = Imaging.PixelFormat.Format4bppIndexed
            ElseIf 256 = nc Then
                pf = Imaging.PixelFormat.Format8bppIndexed
            End If
            Return pf
        End Function

        ''' <summary>
        ''' モノクロ画像（データ部のみ）から、Bitmapとしてを作成するクラス
        ''' </summary>
        ''' <param name="bW">生成するBitmap幅</param>
        ''' <param name="bH">生成するBitmap高さ</param>
        ''' <param name="dataBmp">バイナリデータ(4バイトストライド)</param>
        ''' <param name="nc">色数(2,16,256)</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function DataToBmp(ByVal bW As Integer, ByVal bH As Integer, ByVal dataBmp As IntPtr, Optional ByVal nc As Integer = 256) As Drawing.Bitmap
            '' 出力するBitmap生成
            Dim bitmap As New Bitmap(bW, bH, PixelFormatFromBpp(nc))     'モノクロ画像用

            '' パレットセット
            SetGrayPalette(bitmap)

            '' Bitmapデータセット
            Using bd As New LockingBitmap(bitmap, ImageLockMode.WriteOnly)
                RtlMoveMemory(bd.PtrAt(), dataBmp, New IntPtr(bd.Stride * bH))
            End Using

            Return bitmap
        End Function

        ''' <summary>
        ''' モノクロ画像（データ部のみ）から、Bitmapとしてを作成するクラス
        ''' </summary>
        ''' <param name="bW">生成するBitmap幅</param>
        ''' <param name="bH">生成するBitmap高さ</param>
        ''' <param name="dataBmp">バイナリデータ</param>
        ''' <param name="nc">色数(2,16,256)</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function DataToBmp(ByVal bW As Integer, ByVal bH As Integer, ByVal dataBmp() As Byte, Optional ByVal nc As Integer = 256) As Drawing.Bitmap
            '' 出力するBitmap生成
            Dim bitmap As New Bitmap(bW, bH, PixelFormatFromBpp(nc))     'モノクロ画像用

            '' パレットセット
            SetGrayPalette(bitmap)

            '' Bitmapデータセット
            Using bd As New LockingBitmap(bitmap, ImageLockMode.WriteOnly)
                System.Runtime.InteropServices.Marshal.Copy(dataBmp, 0, bd.PtrAt(), bd.Stride * bH)
            End Using

            Return bitmap
        End Function



        <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi)> _
        Public Shared Function RtlMoveMemory(dst As IntPtr, src As IntPtr, len As IntPtr) As Integer
        End Function



        ''' <summary>
        ''' BMPファイルのヘッダ＋データ部から、Bitmapインスタンスを作成する
        ''' </summary>
        Public Overloads Shared Function BMPDataToBitmap(ByVal bmp() As Byte) As Bitmap
            '配列をポインタ化する。
            Using g1 As New PinnedPtr(bmp)
                Return BMPDataToBitmap(g1.Ptr)
            End Using
                
        End Function

        ''' <summary>
        ''' BMPファイルのヘッダ＋データ部から、Bitmapインスタンスを作成する
        ''' </summary>
        Public Overloads Shared Function BMPDataToBitmap(ByVal srcptr As IntPtr) As Bitmap

   
            Dim bf As BITMAPFILEHEADER = tMod.ReadStructure(Of BITMAPFILEHEADER)(srcptr, 0)
            Dim bi As BITMAPINFOHEADER = tMod.ReadStructure(Of BITMAPINFOHEADER)(srcptr, BITMAPFILEHEADER_SIZE)


            Dim pf As Imaging.PixelFormat
            Select Case bi.biBitCount
                Case 1
                    pf = Imaging.PixelFormat.Format1bppIndexed
                Case 4
                    pf = Imaging.PixelFormat.Format4bppIndexed
                Case 8
                    pf = Imaging.PixelFormat.Format8bppIndexed
                Case 24
                    pf = Imaging.PixelFormat.Format24bppRgb
                Case 32
                    pf = Imaging.PixelFormat.Format32bppRgb
                Case Else
                    Return Nothing
            End Select

            Dim nW As Integer = bi.biWidth
            Dim nH As Integer = Math.Abs(bi.biHeight)
            Dim bReverseHeight As Boolean = bi.biHeight > 0

            '' 出力するBitmap生成
            Dim bitmap As New Bitmap(nW, nH, pf)

            If bi.biBitCount <= 8 Then
                '' パレットセット( Or Load Actureal Palette?)
                SetGrayPalette(bitmap)
            End If

           '' Bitmapデータセット
            Using bd As New LockingBitmap(bitmap, ImageLockMode.WriteOnly)
                If bReverseHeight Then
                    For y As Integer = 0 To nH - 1
                        Dim src_addr As New IntPtr(srcptr.ToInt64() + CInt(bf.bfOffBits) + bd.Stride * (nH - y - 1))
                        RtlMoveMemory(bd.PtrAt(y), src_addr, New IntPtr(bd.Stride))
                    Next
                Else
                    Dim src_addr As New IntPtr(srcptr.ToInt64() + CInt(bf.bfOffBits))
                    RtlMoveMemory(bd.PtrAt(), src_addr, New IntPtr(bd.Stride * nH))
                End If
            End Using

            Return bitmap


        End Function

        ''' <summary>
        ''' カラーパレットを生成
        ''' </summary>
        ''' <remarks></remarks>
        Public Shared Sub SetGrayPalette(ByVal bitmap As Image)
            Dim nColor As Integer = 256
            Select Case bitmap.PixelFormat
                Case Imaging.PixelFormat.Format8bppIndexed
                    nColor = 256
                Case Imaging.PixelFormat.Format4bppIndexed
                    nColor = 16
                Case Imaging.PixelFormat.Format1bppIndexed
                    nColor = 2
            End Select


            Dim palette As Imaging.ColorPalette = bitmap.Palette
            Dim alpha As Integer                   ' 透明度
            Dim intensity As Integer
            '' カラーパレットのデータセット
            For ii As Integer = 0 To nColor - 1
                alpha = &HFF
                intensity = (ii * &HFF) \ (nColor - 1)
                palette.Entries(ii) = Color.FromArgb(alpha, intensity, intensity, intensity)
            Next ii


            bitmap.Palette = palette
        End Sub

        ''' <summary>
        ''' Jpegデータからイメージクラスを生成
        ''' </summary>
        ''' <param name="dataJpeg"></param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function DataToImage(ByVal dataJpeg() As Byte) As Drawing.Bitmap
            Try
                '' WIC対応
                If WicAccessMgr.CheckWicAccess Then
                    Return WicAccessMgr.DataToImageWic(dataJpeg)
                End If

                Return New Bitmap(New System.IO.MemoryStream(dataJpeg))
            Catch ex As Exception
                Return Nothing
            Finally

            End Try
        End Function

        ''' <summary>
        ''' カラーパレットを生成
        ''' </summary>
        ''' <param name="nColor">階調</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Protected Shared Function GetColortPalette(ByVal nColor As Integer) As Drawing.Imaging.ColorPalette
            Dim pixFormat As Drawing.Imaging.PixelFormat        'カラーデータ形式
            If nColor > 16 Then
                pixFormat = Imaging.PixelFormat.Format8bppIndexed
            ElseIf nColor > 2 Then
                pixFormat = Imaging.PixelFormat.Format4bppIndexed
            Else
                pixFormat = Imaging.PixelFormat.Format1bppIndexed
            End If

            '' カラーパレット生成
            Dim bitmap As New Bitmap(1, 1, pixFormat)
            Dim palette As Imaging.ColorPalette
            palette = bitmap.Palette
            bitmap.Dispose()

            Dim alpha As Integer                   ' 透明度
            Dim intensity As Integer

            '' カラーパレットのデータセット
            For ii As Integer = 0 To nColor - 1
                alpha = &HFF
                intensity = (ii * &HFF) \ (nColor - 1)
                palette.Entries(ii) = Color.FromArgb(alpha, intensity, intensity, intensity)
            Next ii


            Return palette
        End Function

        ''' <summary>
        ''' Imageクラス(実際には派生先のBitmapクラスを使用)からビットマップデータ部分のバイト配列に変換
        ''' </summary>
        ''' <param name="objBmp"></param>
        ''' <returns></returns>
        ''' <remarks></remarks>
         Public Overloads Shared Function ImagToData(ByVal objBmp As Bitmap) As Byte()
            ' BitmapDataへ画像データ部分をセットしメモリをロックする
            Using bd As New LockingBitmap(objBmp, ImageLockMode.ReadOnly)

                'ビットマップ用バイト配列のサイズを画像に合わせる
                Dim bytBitmap(bd.Height * bd.Stride - 1) As Byte

                'ビットマップの先頭アドレスからバイト配列へ画像データを読み込み
                System.Runtime.InteropServices.Marshal.Copy(bd.PtrAt(), bytBitmap, 0, bd.Height * bd.Stride)

                Return bytBitmap
                'メモリをアンロック
            End Using
        End Function

        ''' <summary>
        ''' Jpeg画像のバイト配列をビットマップデータ部分のバイト配列に変換
        ''' </summary>
        ''' <param name="bytJpeg">Jpeg画像のバイト配列</param>
        ''' <returns>ビットマップデータ部分のバイト配列</returns>
        ''' <remarks>アンマネージコードを使用、512*256の画像で約5msかかる</remarks>
        Public Overloads Shared Function JpegToData(ByVal bytJpeg() As Byte) As Byte()
            'Dim ms As IO.MemoryStream = New IO.MemoryStream(bytJpeg)    'jpgのバイトデータ読込用ストリーム
            'Dim bmp As Bitmap = New Bitmap(ms)                          'ビットマップにセット  こいつがすごく重たい jpegから変換しているから仕方が無い？
            Dim bmp As Bitmap = DataToImage(bytJpeg)

            Try
                Return ImagToData(bmp)
            Catch ex As Exception
                Return Nothing
            Finally
                bmp.Dispose()
            End Try
        End Function

        ''' <summary>
        ''' Jpegファイルをビットマップデータ部分のバイト配列に変換
        ''' </summary>
        ''' <param name="strJpegFilePath">Jpegファイルファイルパス</param>
        ''' <returns>ビットマップデータ部分のバイト配列</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function JpegToData(ByVal strJpegFilePath As String) As Byte()
            '' WIC対応
            'Dim bmp As Bitmap = New Bitmap(strJpegFilePath)							 'ビットマップにセット  こいつがすごく重たい jpegから変換しているから仕方が無い？
            Dim bmp As Bitmap
            If WicAccessMgr.CheckWicAccess Then
                bmp = WicAccessMgr.ImgFileLoadWic(strJpegFilePath)
            Else
                bmp = New Bitmap(strJpegFilePath)
            End If

            Try
                Return ImagToData(bmp)
            Catch ex As Exception
                Return Nothing
            Finally
                bmp.Dispose()
            End Try
        End Function

        ''' <summary>
        ''' Bitmap画像からJpeg画像に変換してmemstreamで返す
        ''' </summary>
        ''' <param name="bmp">Bitmap画像</param>
        ''' <param name="quality">品質</param>
        ''' <returns>Jpegに変換したバイト配列</returns>
        Public Overloads Shared Function BmpToJpegData(ByVal bmp As Bitmap, ByVal quality As Integer) As MemoryStream
            Dim ms As New MemoryStream

            ' エンコーディング指定
            Dim eps As New System.Drawing.Imaging.EncoderParameters(1)
            eps.Param(0) = New System.Drawing.Imaging.EncoderParameter(System.Drawing.Imaging.Encoder.Quality, quality)

            ' エンコーダー
            Dim encs() As System.Drawing.Imaging.ImageCodecInfo = System.Drawing.Imaging.ImageCodecInfo.GetImageEncoders()
            Dim enc As System.Drawing.Imaging.ImageCodecInfo = Nothing
            For Each enc In encs
                If enc.FormatDescription = "JPEG" Then
                    Exit For
                End If
            Next

            ' 変換
            bmp.Save(ms, enc, eps)
            Return ms


            '''' テスト
            'Dim b() As Byte = ms.ToArray
            'IO.File.WriteAllBytes("c:\bbb.jpg", b)
        End Function

        '====================================================================================================
        ' 拡張処理

        ''' <summary>
        ''' ガンマ補正後の輝度値を取得
        ''' </summary>
        ''' <param name="nIntensity">元の輝度値</param>
        ''' <param name="dGamma">ガンマ補正値 (> 0.0)</param>
        ''' <returns>ガンマ補正後の輝度値</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetIntensityGamma(ByVal nIntensity As Integer, ByVal dGamma As Double) As Integer

            Return CInt(Int(((nIntensity / (256 - 1)) ^ (1 / dGamma)) * (256 - 1)))             ' 少数点以下は切捨て
        End Function

        ''' <summary>
        ''' ガンマ補正適用カラーパレットを生成
        ''' </summary>
        ''' <param name="dGamma">ガンマ補正値 (> 0.0)
        ''' dGamma ＜ 1.0 ： 暗い方向への補正
        ''' dGamma ＝ 1.0 ： デフォルトの明るさ
        ''' dGamma ＞ 1.0 ： 明るい方向への補正
        ''' </param>
        ''' <returns>ガンマ補正適用カラーパレット</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetColorPaletteGamma(ByVal dGamma As Double) As Drawing.Imaging.ColorPalette
            Dim palette As Imaging.ColorPalette
            Dim alpha As Integer                    ' 透明度
            Dim intensity As Integer

            '' カラーパレット生成
            palette = GetColortPalette(256)         ' 256階調のカラーパレット生成

            '' ガンマ補正
            For ii As Integer = 0 To 256 - 1
                alpha = &HFF
                intensity = GetIntensityGamma(ii, dGamma)
                palette.Entries(ii) = Color.FromArgb(alpha, intensity, intensity, intensity)
            Next

            Return palette
        End Function

        ''' <summary>
        ''' ガンマ補正
        ''' </summary>
        ''' <param name="objBmp"></param>
        ''' <param name="dGamma">ガンマ補正値 (> 0.0)
        ''' dGamma ＜ 1.0 ： 暗い方向への補正
        ''' dGamma ＝ 1.0 ： デフォルトの明るさ
        ''' dGamma ＞ 1.0 ： 明るい方向への補正
        ''' </param>
        ''' <returns>True:成功 False:失敗</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GammaCmpBmp(ByRef objBmp As Bitmap, ByVal dGamma As Double) As Boolean

            '' 256階調のカラーパレットのみ対象
            If 256 <> objBmp.Palette.Entries.Length Then
                Return False
            End If

            '' ガンマ補正値の範囲チェック
            If dGamma <= 0.0 Then
                Return False
            End If

            '' ガンマ補正
            objBmp.Palette = GetColorPaletteGamma(dGamma)

            Return True
        End Function

        ''' <summary>
        ''' Bitmapヘッダサイズ取得
        ''' </summary>
        ''' <returns>Bitmapヘッダサイズ</returns>
        ''' <remarks></remarks>
        Public Shared Function GetBitmapHeaderSize_Gray() As Integer
            Return BITMAPFILEHEADER_SIZE + BITMAPINFOHEADER_SIZE + RGBQUAD_SIZE * 256
        End Function

        Public Overloads Shared Function CreateBitmapHeader_Gray( _
                    ByVal nWidth As Integer, _
                    ByVal nHeight As Integer, _
                    ByVal aryBmpData() As Byte, _
                    Optional ByVal bRevTB As Boolean = False) As Integer

            Using pinptr As New PinnedPtr(aryBmpData)
                Return CreateBitmapHeader_Gray(nWidth, nHeight, pinptr.Ptr, bRevTB)
            End Using

        End Function

        ''' <summary>
        ''' グレー画像のBitmapヘッダ生成
        ''' </summary>
        ''' <param name="nWidth">画像幅</param>
        ''' <param name="nHeight">画像高さ</param>
        ''' <param name="aryBmpData">Bitmapヘッダ</param>
        ''' <param name="bRevTB">上下反転フラグ True:反転する False:反転しない</param>
        ''' <returns>復帰情報 0;正常 1～:各所エラー</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function CreateBitmapHeader_Gray( _
        ByVal nWidth As Integer, _
        ByVal nHeight As Integer, _
                    ByVal aryBmpData As IntPtr, _
        Optional ByVal bRevTB As Boolean = False) As Integer
            Dim nBitCount As UShort = 8                                     ' ビットの深さ

            '' ファイルヘッダ作成
            Dim bmpFile As New BITMAPFILEHEADER
            bmpFile.initialize()
            With bmpFile
                .bfType = tMod.StringToByte("BM")                           ' ファイル識別子 ("BM"固定)
                .bfSize = CUInt((nWidth * nHeight * nBitCount / 8) + GetBitmapHeaderSize_Gray())
                .bfOffBits = CUInt(GetBitmapHeaderSize_Gray())              ' ビットマップイメージデータ開始位置
            End With

            '' 情報ヘッダ作成
            Dim bmpInfo As New BITMAPINFOHEADER
            With bmpInfo
                .biSize = 40                                                ' ヘッダサイズ(40固定)
                .biWidth = nWidth                                           ' イメージの横幅(ピクセル単位)
                .biHeight = CInt(IIf(bRevTB, nHeight, -nHeight))            ' イメージの高さ(ピクセル単位) (Windowsは画像が上下反転の為)
                .biPlanes = 1                                               ' プレーン数(1固定)
                .biBitCount = nBitCount                                     ' １ピクセル当たりのビット数(1,4,8,24)
                .biCompression = 0                                          ' 圧縮形式(BI_RGB:0->非圧縮, BI_RLE8:1->RLE圧縮)
                .biSizeImage = CUInt(nWidth * nHeight * nBitCount / 8)      ' 圧縮後のイメージサイズ(バイト単位)
                .biClrUsed = 0                                              ' 格納パレット数(使用色数) (0固定)
                .biClrImportant = 0                                         ' 重要色数 (0固定)
            End With


            tMod.WriteStructure(aryBmpData, 0, bmpFile)
            tMod.WriteStructure(aryBmpData, BITMAPFILEHEADER_SIZE, bmpInfo)
            '' カラーパレット作成
            For ii As Integer = 0 To 256 - 1
                Dim rgb As New RGBQUAD
                rgb.rgbBlue = CByte(ii)
                rgb.rgbRed = CByte(ii)
                rgb.rgbGreen = CByte(ii)
                tMod.WriteStructure(aryBmpData, BITMAPFILEHEADER_SIZE + BITMAPINFOHEADER_SIZE + RGBQUAD_SIZE * ii, rgb)
            Next

            Return 0
        End Function

        ''' <summary>
        ''' ビットマップファイルを保存します
        ''' </summary>
        ''' <param name="strPath">保存先ファイルパス</param>
        ''' <param name="aryHead">ビットマップヘッダ部</param>
        ''' <param name="aryData">ビットマップデータ部</param>
        ''' <returns>True:保存成功 False:保存失敗</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function BmpFileSave(ByVal strPath As String, ByVal aryHead() As Byte, ByVal aryData() As Byte) As Boolean
            Dim out As FileStream = Nothing
            Try
                out = IO.File.OpenWrite(strPath)
                out.Write(aryHead, 0, aryHead.Length)
                out.Write(aryData, 0, aryData.Length)
            Catch ex As Exception
                Return False
            Finally
                If out IsNot Nothing Then
                    out.Close()
                End If
            End Try

            Return True
        End Function

        ''' <summary>
        ''' ビットマップファイルを保存します
        ''' </summary>
        ''' <param name="strPath">保存先ファイルパス</param>
        ''' <param name="bmp">ビットマップクラス</param>
        ''' <param name="bRevTB">上下反転フラグ True:反転する False:反転しない</param>
        ''' <returns>True:保存成功 False:保存失敗</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function BmpFileSave(ByVal strPath As String, ByVal bmp As Bitmap, Optional ByVal bRevTB As Boolean = False) As Boolean
            Dim aryHead() As Byte
            Dim nHeadSize As Integer = GetBitmapHeaderSize_Gray()
            ReDim aryHead(nHeadSize - 1)

            CreateBitmapHeader_Gray(bmp.Width, bmp.Height, aryHead, bRevTB)
            Dim aryData() As Byte = ImagToData(bmp)

            Dim bRet As Boolean = BmpFileSave(strPath, aryHead, aryData)

            Return bRet
        End Function

        ''' <summary>
        ''' コントラスト調整後の輝度値を取得
        ''' </summary>
        ''' <param name="nIntensity">元の輝度値</param>
        ''' <param name="nContrastX">コントラスト補正値(0～255)</param>
        ''' <param name="nBright">明るさ補正値(0～255)</param>
        ''' <returns>コントラスト調整後の輝度値</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetIntensityContrast(ByVal nIntensity As Integer, ByVal nContrastX As Integer, ByVal nBright As Integer) As Integer
            Dim nMax As Integer         '最大輝度
            Dim nMin As Integer         '最小輝度
            Dim dContrast As Double     'コントラスト
            Dim nVal As Integer         '調整後の輝度値用ワーク

            nMax = nContrastX + 128
            nMin = 128 - nContrastX
            dContrast = (nMax - nMin) / (nMax + nMin)

            nVal = CInt(Int((nIntensity - 128) * dContrast) + (nBright * 2 - 128))

            '上下限チェック
            If nVal < 0 Then nVal = 0
            If nVal > 255 Then nVal = 255

            Return nVal

        End Function

        ''' <summary>
        ''' コントラスト調整適用カラーパレットを生成
        ''' </summary>
        ''' <param name="nContrastX">コントラスト補正値(0～255)</param>
        ''' <param name="nBright">明るさ補正値(0～255)</param>
        ''' <returns>コントラスト調整適用カラーパレット</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetColorPaletteContrast(ByVal nContrastX As Integer, ByVal nBright As Integer) As Drawing.Imaging.ColorPalette
            Dim palette As Imaging.ColorPalette
            Dim alpha As Integer                    ' 透明度
            Dim intensity As Integer

            '' カラーパレット生成
            palette = GetColortPalette(256)         ' 256階調のカラーパレット生成

            '' コントラスト調整
            For ii As Integer = 0 To 256 - 1
                alpha = &HFF
                intensity = GetIntensityContrast(ii, nContrastX, nBright)
                palette.Entries(ii) = Color.FromArgb(alpha, intensity, intensity, intensity)
            Next

            Return palette
        End Function

        ''' <summary>
        ''' コントラスト調整
        ''' </summary>
        ''' <param name="objBmp"></param>
        ''' <param name="nContrastX">コントラスト補正値(0～255)</param>
        ''' <param name="nBright">明るさ補正値(0～255)</param>
        ''' <returns>True:成功 False:失敗</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function ContrastCmpBmp(ByRef objBmp As Bitmap, ByVal nContrastX As Integer, ByVal nBright As Integer) As Boolean

            '' 256階調のカラーパレットのみ対象
            If 256 <> objBmp.Palette.Entries.Length Then
                Return False
            End If

            '' コントラスト調整
            objBmp.Palette = GetColorPaletteContrast(nContrastX, nBright)

            Return True
        End Function

        ''' <summary>
        ''' シグモイド補正後の輝度値を取得
        ''' </summary>
        ''' <param name="nIntensity">元の輝度値</param>
        ''' <param name="nKidoX">基準値</param>
        ''' <param name="nKeisu">係数</param>
        ''' <returns>シグモイド補正後の輝度値</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetIntensitySigmoid(ByVal nIntensity As Integer, ByVal nKidoX As Integer, ByVal nKeisu As Integer) As Integer

            Dim dSigmoid As Double

            '０割対策
            If 0 = nKidoX Then
                Return 255
            End If

            '通常は
            'dSigmoid = 1 / (1 + mc_Sigmoid_e ^ (-1 * nKeisu * (nIntensity - nKidoX) / nKidoX))
            'だけであるが、基準値(nKidoX)を境にグラフが変わるため以下のように分ける。
            If nIntensity < nKidoX Then
                dSigmoid = 1 / (1 + Math.E ^ (-1 * nKeisu * (nIntensity - nKidoX) / nKidoX))
            ElseIf nIntensity = nKidoX Then
                dSigmoid = 0.5
            Else
                dSigmoid = 1 / (1 + Math.E ^ (-1 * nKeisu * (nIntensity - nKidoX) / (255 - nKidoX)))
            End If

            Return CInt(Int(255 * dSigmoid))

        End Function

        ''' <summary>
        ''' シグモイド補正適用カラーパレットを生成
        ''' </summary>
        ''' <param name="nKidoX">基準値</param>
        ''' <param name="nKeisu">係数</param>
        ''' <returns>シグモイド補正適用カラーパレット</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetColorPaletteSigmoid(ByVal nKidoX As Integer, ByVal nKeisu As Integer) As Drawing.Imaging.ColorPalette
            Dim palette As Imaging.ColorPalette
            Dim alpha As Integer                    ' 透明度
            Dim intensity As Integer

            '' カラーパレット生成
            palette = GetColortPalette(256)         ' 256階調のカラーパレット生成

            '' シグモイド補正
            For ii As Integer = 0 To 256 - 1
                alpha = &HFF
                intensity = GetIntensitySigmoid(ii, nKidoX, nKeisu)
                palette.Entries(ii) = Color.FromArgb(alpha, intensity, intensity, intensity)
            Next

            Return palette
        End Function

        ''' <summary>
        ''' シグモイド補正
        ''' </summary>
        ''' <param name="objBmp"></param>
        ''' <param name="nKidoX">基準値</param>
        ''' <param name="nKeisu">係数</param>
        ''' <returns>True:成功 False:失敗</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function SigmoidCmpBmp(ByRef objBmp As Bitmap, ByVal nKidoX As Integer, ByVal nKeisu As Integer) As Boolean

            '' 256階調のカラーパレットのみ対象
            If 256 <> objBmp.Palette.Entries.Length Then
                Return False
            End If

            '' シグモイド補正
            objBmp.Palette = GetColorPaletteSigmoid(nKidoX, nKeisu)

            Return True
        End Function

        ''' <summary>
        ''' ２値化補正後の輝度値を取得
        ''' </summary>
        ''' <param name="nIntensity">元の輝度値</param>
        ''' <param name="nBinari01">閾値１</param>
        ''' <param name="nBinari02">閾値２</param>
        ''' <returns>２値化補正後の輝度値</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetIntensityBinari(ByVal nIntensity As Integer, ByVal nBinari01 As Integer, ByVal nBinari02 As Integer) As Integer

            '閾値１と閾値２を比較
            If nBinari01 > nBinari02 Then
                If nBinari01 >= nIntensity And nBinari02 <= nIntensity Then
                    Return 255
                Else
                    Return 0
                End If
            Else
                If nBinari02 >= nIntensity And nBinari01 <= nIntensity Then
                    Return 255
                Else
                    Return 0
                End If
            End If

        End Function

        ''' <summary>
        ''' ２値化補正適用カラーパレットを生成
        ''' </summary>
        ''' <param name="nBinari01">閾値１</param>
        ''' <param name="nBinari02">閾値２</param>
        ''' <returns>２値化補正適用カラーパレット</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetColorPaletteBinari(ByVal nBinari01 As Integer, ByVal nBinari02 As Integer) As Drawing.Imaging.ColorPalette
            Dim palette As Imaging.ColorPalette
            Dim alpha As Integer                    ' 透明度
            Dim intensity As Integer

            '' カラーパレット生成
            palette = GetColortPalette(256)         ' 256階調のカラーパレット生成

            '' ２値化補正
            For ii As Integer = 0 To 256 - 1
                alpha = &HFF
                intensity = GetIntensityBinari(ii, nBinari01, nBinari02)
                palette.Entries(ii) = Color.FromArgb(alpha, intensity, intensity, intensity)
            Next

            Return palette
        End Function

        ''' <summary>
        ''' ２値化補正適用カラーパレットを生成
        ''' </summary>
        ''' <param name="nBinari01">閾値１</param>
        ''' <param name="nBinari02">閾値２</param>
        ''' <returns>２値化補正適用カラーパレット</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetColorPaletteBinari(ByRef objBmp As Bitmap, ByVal nBinari01 As Integer, ByVal nBinari02 As Integer) As Drawing.Imaging.ColorPalette
            Dim palette As Imaging.ColorPalette
            Dim alpha As Integer                    ' 透明度
            Dim intensity As Integer

            '' カラーパレット生成
            palette = GetColortPalette(256)         ' 256階調のカラーパレット生成

            '' ２値化補正
            For ii As Integer = 0 To 256 - 1
                alpha = &HFF
                intensity = GetIntensityBinari(objBmp.Palette.Entries(ii).R, nBinari01, nBinari02)
                palette.Entries(ii) = Color.FromArgb(alpha, intensity, intensity, intensity)
            Next

            Return palette
        End Function

        ''' <summary>
        ''' ２値化補正
        ''' </summary>
        ''' <param name="objBmp"></param>
        ''' <param name="nBinari01">閾値１</param>
        ''' <param name="nBinari02">閾値２</param>
        ''' <returns>True:成功 False:失敗</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function BinariCmpBmp(ByRef objBmp As Bitmap, ByVal nBinari01 As Integer, ByVal nBinari02 As Integer) As Boolean

            '' 256階調のカラーパレットのみ対象
            If 256 <> objBmp.Palette.Entries.Length Then
                Return False
            End If

            '' ２値化補正
            objBmp.Palette = GetColorPaletteBinari(objBmp, nBinari01, nBinari02)

            Return True
        End Function

    End Class



    ''' <summary>
    ''' ビットマップのLockDataを保持するクラス
    ''' </summary>
    ''' <remarks>必ず、「Using構文で使う」or「Disposeを忘れず呼ぶ」こと</remarks>
    Public Class LockingBitmap
        Implements IDisposable

        Private bmp As Bitmap
        Private bd As BitmapData
        Private pixelformat As PixelFormat

        ''' <summary>
        ''' 画像の幅
        ''' </summary>
        Public ReadOnly Property Width() As Integer
            Get
                Return bmp.Width
            End Get
        End Property

        ''' <summary>
        ''' 画像の高さ
        ''' </summary>
        Public ReadOnly Property Height() As Integer
            Get
                Return bmp.Height
            End Get
        End Property

        ''' <summary>
        ''' 画像の、幅のバイト数。（パディング等含む）
        ''' </summary>
        Public ReadOnly Property Stride() As Integer
            Get
                Return bd.Stride
            End Get
        End Property

        ''' <summary>
        ''' 指定座標の画素のポインタを返す
        ''' </summary>
        ''' <param name="y">Y座標</param>
        ''' <param name="x">X座標</param>
        ''' <returns>画素のポインタ</returns>
        ''' <remarks></remarks>
        Public Function PtrAt(Optional ByVal y As Integer = 0, Optional ByVal x As Integer = 0) As IntPtr
            If IntPtr.Size = 8 Then
                Return New IntPtr(bd.Scan0.ToInt64() + y * bd.Stride + x)
            Else
                Return New IntPtr(bd.Scan0.ToInt32() + y * bd.Stride + x)
            End If
        End Function

        ''' <summary>
        ''' 指定座標から、指定幅分の画像を配列で返す
        ''' </summary>
        ''' <param name="y">Y座標</param>
        ''' <param name="x">X座標</param>
        ''' <param name="width">取得する幅</param>
        ''' <returns>画素の配列</returns>
        ''' <remarks>1byte未満の端数がでるような幅を指定しないこと ( 1bppで、width=20とか）</remarks>
        Public Function ToByteArray(ByVal y As Integer, Optional ByVal x As Integer = 0, Optional ByVal width As Integer = 0) As Byte()
            If width = 0 Then
                width = Me.Width
            End If

            Dim byte_width As Integer = bd.Stride * width \ bd.Width
            Dim rv(byte_width - 1) As Byte
            Marshal.Copy(PtrAt(y, x), rv, 0, byte_width)
            Return rv
        End Function

        ''' <summary>
        ''' 画像のラインを、バイト配列からコピーして書き換える
        ''' </summary>
        ''' <param name="y">y座標</param>
        ''' <param name="src">コピー元配列</param>
        Public Sub CopyFrom(ByVal y As Integer, src As Byte())
            Marshal.Copy(src, 0, PtrAt(y), src.Length)
        End Sub


        ''' <summary>
        ''' コンストラクタ。画像をロックする
        ''' </summary>
        ''' <param name="Bmp">画像(ビットマップ)</param>
        ''' <param name="lockmode">ロック方式(書き込み/読み込み)</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal Bmp As Bitmap, ByVal lockmode As ImageLockMode)

            Me.bmp = Bmp
            Me.pixelformat = Bmp.PixelFormat
            Me.bd = Bmp.LockBits(New Rectangle(0, 0, Bmp.Width, Bmp.Height), lockmode, pixelformat)
        End Sub


        ''' <summary>
        ''' ビットマップのロックを解除する。以降は使用不可
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub Dispose() Implements IDisposable.Dispose
            If bd IsNot Nothing Then
                bmp.UnlockBits(bd)
                bd = Nothing
                bmp = Nothing
            End If
        End Sub
    End Class
End Namespace

