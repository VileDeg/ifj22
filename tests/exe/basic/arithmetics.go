//
package main

func main () {
	a := 10
	b := 5
	c := 2.5
	d := 10.25
	e := "hello"
	f := "world"

	a = a + b
	print(a, " ")
	b = b - a
	print(b, " ")
	a = a * b
	print(a, " ")
	b = b / a
	print(b, "\n")

	c = d + c
	print(c, " ")
	d = c - d
	print(d, " ")
	d = c * d
	print(d, " ")
	c = d / c
	print(c, "\n")

	e = e + f
	print(e, "\n")
}
