package main

import (
	"github.com/jurgen-kluft/xcmdline/package"
	"github.com/jurgen-kluft/xcode"
)

func main() {
	xcode.Init()
	xcode.Generate(xcmdline.GetPackage())
}
