//
package main

func alpha(a int) (int) {
	if a<=0 {
		return 0
	} else {
		result := 0
		result = beta(a)
		return result+a
	}
}

func beta(a int)(int) {
	b := a-1
	result := 0
	result = alpha(b)
	return result
}

func main() {
	result := 0
	result = alpha(20)
	print(result)
}
