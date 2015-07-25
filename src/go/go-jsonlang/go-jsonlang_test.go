package jsonlang_test

//import jsonlang "github.com/iqstack/jsonlang/src/go/go-jsonlang"

import (
	"errors"
	"fmt"
	"testing"
)

// Demo for the README.
func Test_Demo(t *testing.T) {
	vm := jsonlang.Make()
	vm.ExtVar("color", "purple")

	x, err := vm.EvaluateSnippet(`Test_Demo`, `"dark " + std.extVar("color")`)
	if err != nil {
		panic(err)
	}
	if x != "\"dark purple\"\n" {
		panic("fail: we got " + x)
	}
	vm.Destroy()
}

// importFunc returns a couple of hardwired responses.
func importFunc(base, rel string) (result string, err error) {
	if rel == "alien.conf" {
		return `{ type: "alien", origin: "Ork", name: "Mork" }`, nil
	}
	if rel == "human.conf" {
		return `{ type: "human", origin: "Earth", name: "Mendy" }`, nil
	}
	return "", errors.New(fmt.Sprintf("Cannot import %q", rel))
}

// check there is no err, and a == b.
func check(t *testing.T, err error, a, b string) {
	if err != nil {
		t.Errorf("got error: %q", err.Error())
	}
	if a != b {
		t.Errorf("got %q but wanted %q", a, b)
	}
}

func Test_Simple(t *testing.T) {
	vm := jsonlang.Make()
	vm.ExtVar("color", "purple")
	vm.ExtVar("size", "XXL")
	vm.ExtVar("gooselevel", "12345")
	vm.ImportCallback(importFunc)

	x, err := vm.EvaluateSnippet(`test1`, `20 + 22`)
	check(t, err, x, `42`+"\n")
	x, err = vm.EvaluateSnippet(`test2`, `std.extVar("color")`)
	check(t, err, x, `"purple"`+"\n")
	x, err = vm.EvaluateSnippet(`test3`, `
    local a = import "alien.conf";
    local b = import "human.conf";
    a.name + b.name
    `)
	check(t, err, x, `"MorkMendy"`+"\n")
	x, err = vm.EvaluateSnippet(`test4`, `
    local a = import "alien.conf";
    local b = a { type: "fictitious" };
    b.type + b.name
    `)
	check(t, err, x, `"fictitiousMork"`+"\n")
}

func Test_FileScript(t *testing.T) {
	vm := jsonlang.Make()
	x, err := vm.EvaluateFile("test2.j")
	check(t, err, x, `{
   "awk": "/usr/bin/awk",
   "shell": "/bin/csh"
}
`)
}

func Test_Misc(t *testing.T) {
	vm := jsonlang.Make()

	vm.MaxStack(10)
	vm.MaxTrace(10)
	vm.GcMinObjects(10)
	vm.GcGrowthTrigger(2.0)

	x, err := vm.EvaluateSnippet("Misc", `
    local a = import "test2.j";
    a.awk + a.shell`)
	check(t, err, x, `"/usr/bin/awk/bin/csh"`+"\n")
}

func Test_AST(t *testing.T) {
	vm := jsonlang.Make()
	vm.DebugAst(1)

	x, err := vm.EvaluateSnippet("AST", "local a = 3 + 5; a + 10")
	check(t, err, x, `(local a = ((3) + (5)); ((a) + (10)))`+"\n")
}
