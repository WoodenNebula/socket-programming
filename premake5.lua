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

    filter "system:windows"
        systemversion "latest"
        targetname("TCP-Messaging")
        links
        {
        }
    filter {}

    filter "system:linux"
        targetname("TCP-Messaging.out")
    filter {}

newaction {
    trigger = "clean",
    description = "Remove all binaries, intermediate files, and VS files.",
    execute = function()
        print("==== Cleaning Up ====")

        local function rmdir_safe(path)
            if os.isdir(path) then
                os.rmdir(path)
                print("Removed directory: " .. path)
            end
        end

        local function remove_pattern(pattern)
            local files = os.matchfiles(pattern)
            for _, file in ipairs(files) do
                os.remove(file)
                print("Removed file: " .. file)
            end
        end

        rmdir_safe("build")
        rmdir_safe(".vs")

        remove_pattern("*.sln")
        remove_pattern("*.slnx")
        remove_pattern("*.vcxproj")
        remove_pattern("*.vcxproj.filters")
        remove_pattern("*.vcxproj.user")
        remove_pattern("Makefile")
        remove_pattern("*.make")

        print("==== Success ====")
    end
}
