workspace "chip8"
	architecture "x86"
	startproject "Interpreter"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Interpreter"