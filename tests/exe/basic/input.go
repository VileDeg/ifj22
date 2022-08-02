//input input_A.in
//input input_B.in
//input input_C.in
//input input_D.in
//input input_E.in
//input ../empty.in
//
package main

func main () {
	err := 0
	resultI := 0
	resultI, err = inputi()
	if (err ==1) {
		print("INVALID INT\n")
	} else {
		print("VALID INT ", resultI, "\n")
	}

	resultF := 0.0
	resultF, err = inputf()
	if (err ==1) {
		print("INVALID FLOAT\n")
	} else {
		print("VALID FLOAT ", resultF, "\n")
	}

	resultS := ""
	resultS, err = inputs()
	if (err ==1) {
		print("INVALID STRING\n")
	} else {
		print("VALID STRING ", resultS, "\n")
	}
}
