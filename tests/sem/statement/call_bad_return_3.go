//compiler 6 7
//
package main

func alpha() (int,int) {
	return 1,2
}

func main () {
	a:=0
	b:=0
	c:=0
	a,b,c = alpha()
}
