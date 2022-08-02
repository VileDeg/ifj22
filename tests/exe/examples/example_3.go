//input alphabet_A.in
//input alphabet_B.in
//input ../empty.in
//
// Program 3: Prace s retezci a vestavenymi funkcemi
package main

func main() {
	s1 := "Toto je nejaky text"
	s2 := s1 + ", ktery jeste trochu obohatime"
	print(s1, "\n", s2)
	s1len := 0
	s1len = len(s1)
	s1len = s1len - 4
	s1, _ = substr(s2, s1len, 4)
	s1len = s1len + 1
	print("4 znaky od", s1len, ". znaku v \"", s2, "\":", s1, "\n")
	print("Zadejte serazenou posloupnost vsech malych pismen a-h, ")
	print("pricemz se pismena nesmeji v posloupnosti opakovat: ")
	err := 0
	s1, err = inputs()
	if err != 1 {
		for ;s1 != "abcdefgh"; {
			print("\n", "Spatne zadana posloupnost, zkuste znovu: ")
			s1, _ = inputs()
		}
	} else {
	}
}
