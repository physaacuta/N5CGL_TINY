========================================================================
スタティック ライブラリ : AvalLib プロジェクトの概要
========================================================================

この AvalLib ライブラリ プロジェクトは、AppWizard によって
作成されました。 

このファイルには、AvalLib アプリケーションを構成する各ファイルの
内容の概略が記述されています。


AvalLib.vcproj
    これは、アプリケーション ウィザードで生成される VC++ プロジェクトのメインの
    プロジェクト ファイルです。 
    ファイルを生成した Visual C++ のバージョン情報と、アプリケーション 
    ウィザードで選択したプラットフォーム、構成、およびプロジェクトの機能に関する
    情報が記述されています。


/////////////////////////////////////////////////////////////////////////////

StdAfx.h, StdAfx.cpp
    これらのファイルは、コンパイル済みヘッダー (PCH) ファイル 
    AvalLib.pch とプリコンパイル済み型ファイル StdAfx.obj を
    ビルドするために使用します。

/////////////////////////////////////////////////////////////////////////////
コンパイラとリンカのスイッチが変更され、MFC をサポートするようになりました。
このプロジェクトで MFC ClassWizard を使用するためには、いくつかのファイルを
プロジェクトに追加する必要があります。たとえば、"resource.h"、
"AvalLib.rc"、および resource.h を含む "AvalLib.h"
 などです。rc ファイルをスタティック ライブラリに追加すると、DLL または EXE に
表示できる rc ファイルは 1 つに限られるという制限により、不具合が生じる場合が
あります。この問題は、ライブラリの .rc ファイルを親プロジェクトの .rc ファイルに
含めることで解決できます。

/////////////////////////////////////////////////////////////////////////////
その他のメモ :

AppWizard では "TODO:" コメントを使用して、ユーザーが追加またはカスタマイズする
ソース部分を示します。

/////////////////////////////////////////////////////////////////////////////




