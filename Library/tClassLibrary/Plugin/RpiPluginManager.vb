Imports System.Reflection
Imports tClassLibrary.tClass
Namespace RpiPlugin
    ''' <summary>
    ''' プラグインのクラスを保持しておく管理クラス。
    ''' </summary>
    Public Class RpiPluginManager
        Private m_plugin_dlls As New List(Of String)
        Private m_plugins As New List(Of IPluginActivatable)

        Private m_pl_toolstrip As New List(Of IToolStrip)
        Private m_pl_evtools As New List(Of IEvToolStrip)
        Private m_pl_drawers As New List(Of IDrawer)
        Private m_pl_rpiopen As New List(Of IRpiOpen)
        Private m_pl_mouseip As New List(Of IMouseToolStrip)
        Private m_pl_filters As New List(Of IFilterBitmap)
        Private m_pl_keydown As New List(Of IKeyInput)

        Public ReadOnly Property PluginDlls As IEnumerable(Of String)
            Get
                Return m_plugin_dlls
            End Get
        End Property
        Public ReadOnly Property AllPlugins As IEnumerable(Of IPluginActivatable)
            Get
                Return m_plugins
            End Get
        End Property

        Public ReadOnly Property EvToolStrips As IEnumerable(Of IEvToolStrip)
            Get
                Return m_pl_evtools
            End Get
        End Property
        Public ReadOnly Property ToolStrips As IEnumerable(Of IToolStrip)
            Get
                Return m_pl_toolstrip
            End Get
        End Property
        Public ReadOnly Property Drawers As IEnumerable(Of IDrawer)
            Get
                Return m_pl_drawers
            End Get
        End Property
        Public ReadOnly Property FilterBitmaps As IEnumerable(Of IFilterBitmap)
            Get
                Return m_pl_filters
            End Get
        End Property
        Public ReadOnly Property RpiOpens As IEnumerable(Of IRpiOpen)
            Get
                Return m_pl_rpiopen
            End Get
        End Property
        Public ReadOnly Property MouseInputs As IEnumerable(Of IMouseToolStrip)
            Get
                Return m_pl_mouseip
            End Get
        End Property
        Public ReadOnly Property KeyInputs As IEnumerable(Of IKeyInput)
            Get
                Return m_pl_keydown
            End Get
        End Property

        ''' <summary>
        ''' plが、Tを実装していたら、listに追加する。
        ''' </summary>
        ''' <typeparam name="T">インタフェイス</typeparam>
        ''' <param name="list">インタフェイスリスト</param>
        ''' <param name="pl">プラグインクラス</param>
        Private Sub TryAddList(Of T As Class)(list As List(Of T), pl As IPluginActivatable)
            Dim s As T = TryCast(pl, T)
            If s IsNot Nothing Then
                list.Add(s)
            End If
        End Sub


        ''' <summary>
        ''' プラグイン優先宇順位順にソートするための比較子
        ''' </summary>
        Private Function comparator(x As IPluginActivatable, y As IPluginActivatable) As Integer
            Return x.PluginLevel.CompareTo(y.PluginLevel)
        End Function

        ''' <summary>
        ''' プラグインクラスを追加する
        ''' </summary>
        ''' <param name="p"></param>
        Public Sub AddPlugin(p As IPluginActivatable)
            m_plugins.Add(p)
        End Sub

        Dim dlls As New List(Of Assembly)

        Dim m_decoder_types As New List(Of Type)
        Public ReadOnly Property Decoders As IEnumerable(Of Type)
            Get
                Return m_decoder_types
            End Get
        End Property


        ''' <summary>
        ''' 指定DLLをロードしてIPluginActivatable実装クラスを実体化させる。
        ''' </summary>
        ''' <param name="dllname">プラグインDLL名</param>
        ''' <returns> 実体化したインスタンスの数</returns>
        Public Function LoadDll(dllname As String) As Integer
            ' Try
            Dim dllasm As Assembly = Assembly.LoadFile(dllname)
                Dim isValidPluginDll As Boolean = False

                Dim types() As Type = dllasm.GetTypes()
                For Each t As Type In types
                    If Not t.IsClass Then Continue For


                    If t.GetInterface("tClassLibrary.RpiPlugin.IPluginActivatable") IsNot Nothing Then
                        Dim o As Object = Activator.CreateInstance(t)
                        Dim iplugin As IPluginActivatable = DirectCast(o, IPluginActivatable)

                        m_plugins.Add(iplugin)
                        isValidPluginDll = True
                    End If

                    If t.GetInterface("tClassLibrary.RpiPlugin.IRPIDecoder") IsNot Nothing Then
                        m_decoder_types.Add(t)
                    End If
                Next

                If isValidPluginDll Then
                    dlls.Add(dllasm)
                Else
                    'free したいが、.NETはいちどAssembly.LoadFileしたDLLは二度と離せない。
                End If
            'Catch e As Exception
            '    Throw
            'End Try

            Return 0
        End Function

        ''' <summary>
        ''' 実体化済みのプラグインクラスを初期化してまわる。
        ''' </summary>
        ''' <param name="rpiview"></param>
        Public Sub Init(rpiview As tClass_ViewRpiThread, mainfrm As System.Windows.Forms.Form)
            m_plugins.Sort(AddressOf comparator)

            For Each pl As IPluginActivatable In m_plugins
                pl.InitPlugin(rpiview, mainfrm)

                TryAddList(m_pl_evtools, pl)
                TryAddList(m_pl_filters, pl)
                TryAddList(m_pl_rpiopen, pl)
                TryAddList(m_pl_mouseip, pl)
                TryAddList(m_pl_drawers, pl)
                TryAddList(m_pl_toolstrip, pl)
                TryAddList(m_pl_keydown, pl)
            Next

        End Sub
    End Class
End Namespace