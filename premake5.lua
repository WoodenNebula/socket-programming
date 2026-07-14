workspace "SocketProgramming"
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
        startproject "ChatApp"
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

    include "src/Networking/Networking.lua"
    include "src/App/ChatApp.lua"


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
