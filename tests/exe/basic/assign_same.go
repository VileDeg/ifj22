//
package main

func alpha () (int, int) {
	return 1, 2
}

func beta (param float64) (float64, string, float64) {
	param = param + 3.7
	return param, "help" ,2.9
}

func main () {
	a := 0
	a,a = alpha()
	print(a, "\n")

	f := 0.0
	f, _, f = beta(f)
	print(f, "\n")
}
