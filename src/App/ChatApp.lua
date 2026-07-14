project "ChatApp"
    language "C++"
    cppdialect "C++20"

    targetdir("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
    objdir("%{wks.location}/build/obj/" .. outputdir .. "/%{prj.name}")

    kind "ConsoleApp"

    dependson { "Networking" }

    includedirs {
        ".",
        "../Networking",
     }

    files { "**.h", "**.cpp", "**.hpp" }

    vpaths {
        ["Sources"] = { "**.cpp", "**.c" },
        ["Headers"] = { "**.hpp", "**.h" },
    }

    links
    {
        "Networking",
    }

    filter "system:windows"
        systemversion "latest"
        targetname("SocketProgramming")
    filter {}

    filter "system:linux"
        targetname("SocketProgramming.out")
    filter {}
