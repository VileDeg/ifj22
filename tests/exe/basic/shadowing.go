//
package main

func main () {
	a := 30
	print(a)
	for a:=10;a>0;a=a-1 {
		print(a)
		a := 3.14
		print(a)
		if a>5.0 {
			print(a)
			a:="hello"
			print(a)
		} else {
			print(a)
			a:="world"
			print(a)
		}
	}
	print(a)
}
