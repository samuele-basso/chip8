project "Interpreter"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++14"
	staticruntime "on"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"dependencies/SDL2-2.0.12/include"
	}

	libdirs
	{
		"dependencies/SDL2-2.0.12/lib/Win32"
	}

	links
	{
		"SDL2",
		"SDL2main",
	}

	configuration "windows"
		postbuildcommands
		{
			"{COPY} ".._WORKING_DIR.."/%{prj.name}/assets/s.wav ".._WORKING_DIR.."/bin/"..outputdir.."/%{prj.name}/",
			"{COPY} ".._WORKING_DIR.."/%{prj.name}/dependencies/SDL2-2.0.12/lib/Win32/SDL2.dll ".._WORKING_DIR.."/bin/"..outputdir.."/%{prj.name}/"
		}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
