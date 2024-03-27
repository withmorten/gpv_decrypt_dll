function addcopypath (gamepath)
	postbuildcommands { '{COPYFILE} "%{cfg.buildtarget.abspath}" "' .. path.translate(gamepath) .. "/" .. '%{cfg.buildtarget.name}"' }
	postbuildcommands { '{COPYFILE} "%{cfg.buildtarget.directory}/%{cfg.buildtarget.basename}.pdb" "' .. path.translate(gamepath) .. "/" .. '%{cfg.buildtarget.basename}.pdb"' }
end

workspace "gpv_decrypt"
	configurations { "Release", "Debug" }
	platforms { "x64" }

	location "build"
	targetdir "bin/%{cfg.buildcfg}"

	characterset ("MBCS")
	staticruntime "on"
	links { "legacy_stdio_definitions" }
	defines { "WIN32_LEAN_AND_MEAN", "VC_EXTRALEAN", "WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS", "_CRT_NONSTDC_NO_DEPRECATE", "NOMINMAX" }
	cppdialect "C++17"

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "full"
		optimize "off"
		runtime "debug"

	filter "configurations:Release"
		defines { "NDEBUG" }
		symbols "on"
		optimize "speed"
		runtime "release"
		flags { "LinkTimeOptimization" }

project "gpv_decrypt"
	kind "SharedLib"
	targetname "gpv_decrypt"

	addcopypath ("C:\\Games\\Age.of.Empires.II.Definitive.Edition.Victors.and.Vanquished-RUNE")

	files { "src/dllmain.cpp" }
	files { "src/gpv_decrypt.cpp" }
	files { "src/gpv_decrypt.h" }

	files { "src/util.cpp" }
	files { "src/util.h" }

	includedirs { "vendor/ModUtils" }
	files { "vendor/ModUtils/MemoryMgr.h" }
	files { "vendor/ModUtils/Trampoline.h" }
	files { "vendor/ModUtils/Patterns.cpp" }
	files { "vendor/ModUtils/Patterns.h" }
