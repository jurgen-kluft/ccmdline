package main

import (
	cpkg "github.com/jurgen-kluft/ccmdline/package"
	ccode "github.com/jurgen-kluft/ccode"
)

func main() {
	ccode.Init()
	ccode.Generate(cpkg.GetPackage())
}
