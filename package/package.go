package ccmdline

import (
	cbase "github.com/jurgen-kluft/cbase/package"
	denv "github.com/jurgen-kluft/ccode/denv"
	ccore "github.com/jurgen-kluft/ccore/package"
	cunittest "github.com/jurgen-kluft/cunittest/package"
)

// GetPackage returns the package object of 'ccmdline'
func GetPackage() *denv.Package {
	// Dependencies
	unittestpkg := cunittest.GetPackage()
	cbasepkg := cbase.GetPackage()
	ccorepkg := ccore.GetPackage()

	// The main (ccmdline) package
	mainpkg := denv.NewPackage("ccmdline")
	mainpkg.AddPackage(unittestpkg)
	mainpkg.AddPackage(cbasepkg)
	mainpkg.AddPackage(ccorepkg)

	// 'ccmdline' library
	mainlib := denv.SetupDefaultCppLibProject("ccmdline", "github.com\\jurgen-kluft\\ccmdline")
	mainlib.Dependencies = append(mainlib.Dependencies, cbasepkg.GetMainLib())
	mainlib.Dependencies = append(mainlib.Dependencies, ccorepkg.GetMainLib())

	// 'ccmdline' unittest project
	maintest := denv.SetupDefaultCppTestProject("ccmdline_test", "github.com\\jurgen-kluft\\ccmdline")
	maintest.Dependencies = append(maintest.Dependencies, unittestpkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, mainlib)

	mainpkg.AddMainLib(mainlib)
	mainpkg.AddUnittest(maintest)
	return mainpkg
}
