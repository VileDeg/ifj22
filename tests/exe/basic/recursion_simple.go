//
package main

func alpha(a int) (int) {
	if a<=0 {
		return 0
	} else {
		b := a-1
		result := 0
		result = alpha(b)
		return result+a
	}
}

func main() {
	result := 0
	result = alpha(10)
	print(result)
}
