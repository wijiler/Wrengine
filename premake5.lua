workspace "Wrenderer"
project "Wrengine"
    language "C"
    targetname "Wrengine"
    architecture "x64"
    outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"
    kind "ConsoleApp"
    cdialect "C99"

    prebuildcommands {
        "compileShaders.bat"
    }

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
        defines { "VK_USE_PLATFORM_WIN32_KHR" }
    filter ""
    filter "system:linux"
        defines { "VK_USE_PLATFORM_XLIB_KHR" }
    filter ""

    filter "action:gmake"
    toolset "clang"
    links { "user32", "msvcrt", "gdi32", "shell32", "libcmt" }
    buildoptions {"-Wextra", "-Wall"}
    filter "not action:gmake"
    toolset "msc"
    filter ""

include "Wrenderer/premake5.lua"
