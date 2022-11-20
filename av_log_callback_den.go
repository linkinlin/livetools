package main

import "C"
import (
	"fmt"
	"unsafe"
)

//export logCallback
func logCallback(v unsafe.Pointer, level C.int, format *C.char, arglist *C.char) {
	fmt.Println("66666666666666")
	fmt.Println(C.GoString(format))
	fmt.Println(C.GoString(arglist))
}
