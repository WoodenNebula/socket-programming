project "Networking"
    language "C++"
    cppdialect "C++20"

    kind "StaticLib"
    staticruntime "off"

    targetdir("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
    objdir("%{wks.location}/build/obj/" .. outputdir .. "/%{prj.name}")

    includedirs { "." }

    files { "**.h", "**.cpp", "**.hpp" }

    vpaths {
        ["Sources"] = { "**.cpp", "**.c" },
        ["Headers"] = { "**.hpp", "**.h" },
    }

    filter "system:windows"
        systemversion "latest"
        links
        {
            "Ws2_32",
        }
    filter {}