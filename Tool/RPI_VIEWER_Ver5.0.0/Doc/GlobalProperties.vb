Imports tClassLibrary.RpiPlugin

Public Class DisplayShrinker
    Inherits PeropertiesBase

    Public nBaseResX As Double
    Public nBaseResY As Double

    Public nResX As Double
    Public nResY As Double

    Public Sub Reset(x As Double, y As Double)
        nResX = x
        nResY = y
        nBaseResX = x
        nBaseResY = y
    End Sub

End Class


Public Class GlobalProperties
    Inherits PeropertiesBase

    Public displayshrinker As New DisplayShrinker
    Public imagecontrol As New PropImageControl
    Public drawmemori As New DrawMemoriProperties

End Class