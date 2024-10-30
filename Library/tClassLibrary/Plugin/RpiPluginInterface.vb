Namespace RpiPlugin
    Public Interface IPluginActivatable
        ReadOnly Property PluginLevel As Single
        ReadOnly Property PluginName As String
        Sub InitPlugin(v As tClass.tClass_ViewRpiThread, mainfrm As System.Windows.Forms.Form)
    End Interface

    ''' <summary>
    ''' ツールストリップにアイテムを追加するため。
    ''' </summary>
    Public Interface IToolStrip
        ''' <summary>
        ''' ツールストリップにアイテムを追加する
        ''' </summary>
        ''' <param name="mainstrip">下方につくToolStrip。(0)がボタン、(1)はステータスバー</param>
        Sub InitToolbar(mainstrip() As System.Windows.Forms.ToolStrip)
    End Interface


    ''' <summary>
    ''' 準備済みストリップ（RPI_VIEWERで用意する）を使用.
    ''' RPIVIEWER内蔵機能を、プラグイン用のインタフェイス経由で実現するため
    ''' </summary>
    Public Interface IEvToolStrip

        ''' <summary>
        ''' RPI_VIEWERに伝える名前
        ''' </summary>
        ''' <returns></returns>
        ReadOnly Property ToolStripName As String
        ''' <summary>
        ''' ツールバー初期化
        ''' </summary>
        ''' <param name="tbButton">ツールバーボタン</param>
        ''' <param name="o">付帯情報(RPIVEIWER本体側で適当なインスタンスが渡る)</param>
        Sub InitToolbar(tbButton As System.Windows.Forms.ToolStripButton, o As Object)
    End Interface
    ''' <summary>
    ''' 全長画像面に対するクリックが欲しい
    ''' </summary>
    Public Interface IKeyInput
        Function OnKeyDown(e As Windows.Forms.KeyEventArgs) As Boolean
    End Interface



    ''' <summary>
    ''' 全長画像面に対するクリックが欲しい
    ''' </summary>
    Public Interface IMouseToolStrip
        Function InitToolbar() As System.Windows.Forms.ToolStripButton
        Function Activate(bActive As Boolean) As Boolean

        Function OnMouseClick(e As System.EventArgs) As Boolean
    End Interface

    Public Interface IMouseInputDetail
        Function OnMouseUp(e As Windows.Forms.MouseEventArgs) As Boolean
        Function OnMouseDown(e As Windows.Forms.MouseEventArgs) As Boolean
        Function OnMouseMove(e As Windows.Forms.MouseEventArgs) As Boolean
        Function OnMouseWheel(e As Windows.Forms.MouseEventArgs) As Boolean
    End Interface

    ''' <summary>
    ''' RpiのOpen/Closeイベントを処理する
    ''' </summary>
    Public Interface IRpiOpen
        Sub OnRpiOpen(rpi As tClass.tClass_ReadRPI)
        Sub OnRpiClose()
    End Interface

    ''' <summary>
    ''' 全長画像のうえに描画したい
    ''' </summary>
    Public Interface IDrawer
        Sub OnChangeFrameNo(nf_start As Integer, nf_end As Integer)
        Sub OnDraw(g As System.Drawing.Graphics)
    End Interface


    ''' <summary>
    ''' RPIの全長画像を1フレーム毎を編集する
    ''' </summary>
    Public Interface IFilterBitmap
        Sub OnFilterImage(ByRef bmp As System.Drawing.Bitmap, ByVal p As tClass.tClass_ViewRpiThread.CLS_IMG_INF)
    End Interface

    ''' <summary>
    ''' RPIの圧縮を展開する
    ''' </summary>
    Public Interface IRPIDecoder
        ''' <summary>
        ''' 展開できる圧縮方式か？
        ''' </summary>
        ''' <param name="compresstype">圧縮方式（RPIヘッダ）</param>
        ''' <returns>対応可能ならTrue。Decompressが呼ばれる。</returns>
        Function IsCompressionSupported(ByVal compresstype As String) As Boolean
        ''' <summary>
        ''' 展開
        ''' </summary>
        ''' <param name="bmp">展開後画像。Newして返す。</param>
        ''' <param name="compressData">圧縮データ</param>
        ''' <param name="header">RPIヘッダ(参考用)</param>
        ''' <returns></returns>
        Function Decompress(<Runtime.InteropServices.Out> ByRef bmp As System.Drawing.Bitmap, ByVal compressData() As Byte, ByVal header As tClass.tClass_ReadRPI) As Boolean

    End Interface
End Namespace