package xcmdline

import (
	"github.com/jurgen-kluft/xcode/denv"
	"github.com/jurgen-kluft/xentry/package"
	"github.com/jurgen-kluft/xunittest/package"
	"github.com/jurgen-kluft/xbase/package"
)

// GetPackage returns the package object of 'xcmdline'
func GetPackage() *denv.Package {
	// Dependencies
	unittestpkg := xunittest.GetPackage()
	entrypkg := xentry.GetPackage()
	basepkg := xbase.GetPackage()

	// The main (xcmdline) package
	mainpkg := denv.NewPackage("xcmdline")
	mainpkg.AddPackage(unittestpkg)
	mainpkg.AddPackage(entrypkg)
	mainpkg.AddPackage(basepkg)

	// 'xcmdline' library
	mainlib := denv.SetupDefaultCppLibProject("xcmdline", "github.com\\jurgen-kluft\\xcmdline")
	mainlib.Dependencies = append(mainlib.Dependencies, basepkg.GetMainLib())

	// 'xcmdline' unittest project
	maintest := denv.SetupDefaultCppTestProject("xcmdline_test", "github.com\\jurgen-kluft\\xcmdline")
	maintest.Dependencies = append(maintest.Dependencies, unittestpkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, entrypkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, basepkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, mainlib)

	mainpkg.AddMainLib(mainlib)
	mainpkg.AddUnittest(maintest)
	return mainpkg
}
