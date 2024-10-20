workspace "Wrenderer"
project "Wrengine"
    language "C"
    targetname "Wrengine"
    architecture "x64"
    outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"
    kind "ConsoleApp"
    toolset "msc"
    cdialect "C99"

    targetdir("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")
    objdir("%{wks.location}/Binaries/Intermediates/" .. outputdir .. "/%{prj.name}")
    files { "include/**.h", "src/**.c" }
    libdirs { "./libs/" }
    includedirs { "./include/" }
    includedirs { "./Wrenderer/include/" }
    includedirs { "./include/libs/" }
    includedirs { os.getenv("VULKAN_SDK") .. "/Include" }
    libdirs { os.getenv("VULKAN_SDK") .. "/Lib" }
    links { "vulkan-1", "glfw3" }
    links { "Wrenderer" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
    filter ""
    filter "configurations:Release"
        optimize "On"
    filter ""
    filter "system:windows"
        links { "user32", "msvcrt", "gdi32", "shell32", "libcmt" }
        defines { "VK_USE_PLATFORM_WIN32_KHR" }
    filter ""
    filter "system:linux"
        defines { "VK_USE_PLATFORM_XLIB_KHR" }
    filter ""
    filter "action:gmake"
    filter ""

    prebuildcommands {
        "compileShaders.bat"
    }
include "Wrenderer/premake5.lua"
