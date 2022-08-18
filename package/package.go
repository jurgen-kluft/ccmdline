package ccmdline

import (
	cbase "github.com/jurgen-kluft/cbase/package"
	"github.com/jurgen-kluft/ccode/denv"
	cunittest "github.com/jurgen-kluft/cunittest/package"
)

// GetPackage returns the package object of 'ccmdline'
func GetPackage() *denv.Package {
	// Dependencies
	unittestpkg := cunittest.GetPackage()
	basepkg := cbase.GetPackage()

	// The main (ccmdline) package
	mainpkg := denv.NewPackage("ccmdline")
	mainpkg.AddPackage(unittestpkg)
	mainpkg.AddPackage(basepkg)

	// 'ccmdline' library
	mainlib := denv.SetupDefaultCppLibProject("ccmdline", "github.com\\jurgen-kluft\\ccmdline")
	mainlib.Dependencies = append(mainlib.Dependencies, basepkg.GetMainLib())

	// 'ccmdline' unittest project
	maintest := denv.SetupDefaultCppTestProject("ccmdline_test", "github.com\\jurgen-kluft\\ccmdline")
	maintest.Dependencies = append(maintest.Dependencies, unittestpkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, basepkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, mainlib)

	mainpkg.AddMainLib(mainlib)
	mainpkg.AddUnittest(maintest)
	return mainpkg
}
