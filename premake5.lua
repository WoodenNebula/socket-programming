workspace "TCP-Messaging"
    configurations { "Debug", "Release" }
    architecture "x64"


    defines {
        'PROJECT_ROOT=\"' .. os.getcwd() .. '\"'
    }

    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG" }
        symbols "On"
    filter{}

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
    filter{}

    filter "system:windows"
        startproject "TCP-Messaging"
        defines { "WINDOWS=1" }
        systemversion "latest"

        filter "action:vs2026"
            buildoptions { "/Zc:preprocessor" }
        filter{}
    filter{}

    filter "system:linux"
        defines { "LINUX=1" }
    filter{}


    outputdir = "%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}"


project "TCP-Messaging"
    language "C++"
    cppdialect "C++20"
    kind "ConsoleApp"

    targetdir("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
    objdir("%{wks.location}/build/obj/" .. outputdir .. "/%{prj.name}")

    includedirs {
        "src",
    }

    files { "src/**.h", "src/**.cpp", "src/**.hpp" }

    vpaths {
        ["Sources"] = { "**.cpp", "**.c" },
        ["Headers"] = { "**.hpp", "**.h" },
    }

    links
    {
        -- "Engine",
        -- "glad",
        -- "glfw"
    }

    filter "system:windows"
        kind "WindowedApp"
        systemversion "latest"
        targetname("TCP-Messaging")
        links
        {
            "user32",
            "shell32",
        }
    filter {}

    filter "system:linux"
        targetname("TCP-Messaging.out")
    filter {}


newaction {
    trigger = "clean",
    description = "Remove all binaries and intermediate binaries, and vs files.",
    execute = function()
        print("==== Cleaning Up ====")
        -- os.chdir("TCP-Messaging")

        os.rmdir("build")
        os.rmdir("./.vs")

        os.remove("**.sln")
        os.remove("**.slnx")
        os.remove("**.vcxproj")
        os.remove("**.vcxproj.**")
        os.remove("**Makefile")
        os.remove("**.make")
        print("==== Success ====\n")
    end
}
