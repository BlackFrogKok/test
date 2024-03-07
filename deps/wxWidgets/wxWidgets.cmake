set(_wx_git_tag v3.1.5)

set(_wx_toolkit "")
    set(_wx_private_font "-DwxUSE_PRIVATE_FONTS=1")
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(_gtk_ver 2)
    if (DEP_WX_GTK3)
        set(_gtk_ver 3)
    endif ()
    set(_wx_toolkit "-DwxBUILD_TOOLKIT=gtk${_gtk_ver}")
endif()

if (MSVC)
    set(_wx_edge "-DwxUSE_WEBVIEW_EDGE=ON")
else ()
    set(_wx_edge "-DwxUSE_WEBVIEW_EDGE=OFF")
endif ()

if (MSVC)
    set(_patch_cmd if not exist WXWIDGETS_PATCHED ( patch --verbose -p1 -l -i ${CMAKE_CURRENT_LIST_DIR}/0001-wx-3.1.5-patch-for-Orca.patch && type nul > WXWIDGETS_PATCHED ) )
else ()
    set(_patch_cmd test -f WXWIDGETS_PATCHED || patch --verbose -p1 -l -i ${CMAKE_CURRENT_LIST_DIR}/0001-wx-3.1.5-patch-for-Orca.patch && touch WXWIDGETS_PATCHED)
endif ()

if (FLATPAK)
    set(_patch_cmd cat ${CMAKE_CURRENT_LIST_DIR}/0001-wx-3.1.5-patch-for-Orca.patch ${CMAKE_CURRENT_LIST_DIR}/0001-Add-support-for-building-WebView-with-libwebkit2gtk-.patch | patch --verbose -p1 -l)
endif ()

orcaslicer_add_cmake_project(
    wxWidgets
    URL "https://github.com/wxWidgets/wxWidgets/archive/refs/tags/v3.1.5.tar.gz"
    URL_HASH SHA256=e8fd5f9fbff864562aa4d9c094f898c97f5e1274c90f25beb0bfd5cb61319dea
    PATCH_COMMAND ${_patch_cmd}
    DEPENDS ${PNG_PKG} ${ZLIB_PKG} ${EXPAT_PKG} ${TIFF_PKG} ${JPEG_PKG}
    CMAKE_ARGS
        -DwxBUILD_PRECOMP=ON
        ${_wx_toolkit}
        "-DCMAKE_DEBUG_POSTFIX:STRING="
        -DwxBUILD_DEBUG_LEVEL=0
        -DwxBUILD_SAMPLES=OFF
        -DwxBUILD_SHARED=OFF
        -DwxUSE_MEDIACTRL=ON
        -DwxUSE_DETECT_SM=OFF
        -DwxUSE_UNICODE=ON
        ${_wx_private_font}
        -DwxUSE_OPENGL=ON
        -DwxUSE_WEBREQUEST=ON
        -DwxUSE_WEBVIEW=ON
        ${_wx_edge}
        -DwxUSE_WEBVIEW_IE=OFF
        -DwxUSE_REGEX=builtin
        -DwxUSE_LIBXPM=builtin
        -DwxUSE_LIBSDL=OFF
        -DwxUSE_XTEST=OFF
        -DwxUSE_STC=OFF
        -DwxUSE_AUI=ON
        -DwxUSE_LIBPNG=sys
        -DwxUSE_ZLIB=sys
        -DwxUSE_LIBJPEG=sys
        -DwxUSE_LIBTIFF=sys
        -DwxUSE_NANOSVG=OFF
        -DwxUSE_EXPAT=sys
)

if (MSVC)
    add_debug_dep(dep_wxWidgets)
endif ()
